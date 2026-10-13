from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
LEADERS = [('PewterCity','Brock','BROCK','BOULDER','ROCKY_HELMET'), ('CeruleanCity','Misty','MISTY','CASCADE','LEFTOVERS'), ('VermilionCity','LtSurge','SURGE','THUNDER','CHOICE_SPECS'), ('CeladonCity','Erika','ERIKA','RAINBOW','WIDE_LENS'), ('FuchsiaCity','Koga','KOGA','SOUL','BLACK_SLUDGE'), ('SaffronCity','Sabrina','SABRINA','MARSH','WISE_GLASSES'), ('CinnabarIsland','Blaine','BLAINE','VOLCANO','EXPERT_BELT'), ('ViridianCity','Giovanni','GIOVANNI','EARTH','CHOICE_BAND')]

def blocks(text):
    return {m[1]:m[2] for m in re.finditer(r'^([\w]+)::\n(.*?)(?=^\w+::|\Z)',text,re.M|re.S)}

class RecoveryRewards(unittest.TestCase):
    def test_rewards_and_pending_paths(self):
        rewards = blocks((ROOT/'data/scripts/prl_gym_rewards.inc').read_text())
        for city, leader, key, badge, item in LEADERS:
            with self.subTest(leader=leader):
                reward = rewards.get('PRL_EventScript_BadgeHeistReward_'+key,'')
                self.assertIn('giveitem ITEM_'+item, reward)
                self.assertLess(reward.index('giveitem'), reward.index('addmoney 10000'))
                self.assertLess(reward.index('goto_if_eq VAR_RESULT, FALSE', reward.index('giveitem')),reward.index('addmoney'))
                self.assertLess(reward.index('addmoney'),reward.index('setflag FLAG_PRL_BADGE_HEIST_REWARD_'+key))
                script = blocks((ROOT/f'data/maps/{city}_Gym_Frlg/scripts.inc').read_text())
                entry = script[f'{city}_Gym_EventScript_{leader}']
                self.assertIn(f'call {city}_Gym_Frlg_EventScript_PRLBadgeRewardPending',entry)
                self.assertLess(entry.index('PRLBadgeRewardPending'), entry.index('FLAG_PRL_BADGE_HEIST_STARTED'))
                pending = script[f'{city}_Gym_Frlg_EventScript_PRLBadgeRewardPending']
                self.assertIn('goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_'+badge,pending)
                self.assertIn('goto_if_set FLAG_PRL_BADGE_HEIST_REWARD_'+key,pending)
                self.assertIn('release\n\tend',pending)
                won = script[f'{city}_Gym_Frlg_EventScript_PRLBadgeRecoveryWon']
                self.assertLess(won.index('setflag FLAG_PRL_BADGE_HEIST_RECOVERED_'+badge),won.index('call PRL_EventScript_BadgeHeistReward_'+key))
                self.assertNotIn('giveitem ITEM_TM', won)

    def test_recovery_claim_flags_are_separate_from_badges_and_first_clear(self):
        flags=(ROOT/'include/constants/flags_frlg.h').read_text()
        aliases=re.findall(r'^#define FLAG_PRL_BADGE_HEIST_REWARD_\w+\s+FLAG_0x([0-9A-F]+)$',flags,re.M)
        self.assertEqual(sorted(int(value,16) for value in aliases),list(range(0x4E6,0x4EE)))

    def test_healers_have_victory_callback_and_repeat_service(self):
        for floor, name, trainer in [(1,'Rolando','COOLTRAINER_ROLANDO'),(2,'Gregory','JUGGLER_GREGORY'),(3,'George','COOLTRAINER_GEORGE')]:
            text = (ROOT/f'data/maps/VictoryRoad_{floor}F_Frlg/scripts.inc').read_text()
            script = blocks(text)
            prefix=f'VictoryRoad_{floor}F_EventScript_{name}'
            self.assertIn(prefix+'Heal', script[prefix].splitlines()[0])
            self.assertIn('goto '+prefix+'Heal', script[prefix])
            self.assertIn('\tspecial HealPlayerParty\n',script.get(prefix+'Heal',''))
            self.assertNotIn('setflag', script[prefix+'Heal'])

if __name__ == '__main__': unittest.main()
