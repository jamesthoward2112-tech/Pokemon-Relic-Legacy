"""PKR -> PRL: preserve Erika's locked Big Root reward and repair legacy saves."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class ErikaRewardMigration(unittest.TestCase):
    def test_correct_primary_reward_and_no_duplicate_cash(self):
        source = read("data/scripts/prl_gym_rewards.inc")
        block = source.split("PRL_EventScript_GymExtra_KANTO_ERIKA::", 1)[1].split("PRL_EventScript_GymExtra_KANTO_ERIKA_Done::",1)[0]
        self.assertIn("checkitemspace ITEM_BIG_ROOT", block)
        self.assertIn("giveitem ITEM_BIG_ROOT", block)
        self.assertNotIn("ITEM_MIRACLE_SEED", block)
        self.assertLess(block.index("giveitem ITEM_BIG_ROOT"), block.index("addmoney 10000"))
        self.assertLess(block.index("addmoney 10000"), block.index("setflag FLAG_PRL_GYM_KANTO_ERIKA_EXTRA"))
        self.assertLess(block.index("setflag FLAG_PRL_GYM_KANTO_ERIKA_EXTRA"), block.index("setflag FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION"))
        legacy = source.split("PRL_EventScript_GymExtra_KANTO_ERIKA_LegacyBigRoot::",1)[1].split("PRL_EventScript_GymExtra_KANTO_ERIKA_LegacyDone::",1)[0]
        self.assertIn("goto_if_unset FLAG_PRL_GYM_KANTO_ERIKA_EXTRA",legacy)
        self.assertIn("goto_if_set FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION",legacy)
        self.assertIn("checkitemspace ITEM_BIG_ROOT",legacy)
        self.assertIn("giveitem ITEM_BIG_ROOT",legacy)
        self.assertLess(legacy.index("giveitem ITEM_BIG_ROOT"),legacy.index("setflag FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION"))
        self.assertNotIn("addmoney",legacy)
        self.assertNotIn("removeitem",legacy)
        self.assertIn("PRL_EventScript_GymExtraNoRoom",legacy)

    def test_save_persistent_flag_and_script_priority(self):
        flags = read("include/constants/flags.h")
        self.assertRegex(flags,r"(?m)^#define FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION\s+0x4D8\b")
        script = read("data/maps/CeladonCity_Gym_Frlg/scripts.inc")
        first = script.split("CeladonCity_Gym_EventScript_Erika::",1)[1].split("CeladonCity_Gym_EventScript_ClaimLegacyBigRoot::",1)[0]
        self.assertLess(first.index("goto_if_unset FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION"),
                        first.index("ShouldTryRematchBattle"))
        self.assertLess(first.index("goto_if_unset FLAG_PRL_GYM_KANTO_ERIKA_EXTRA"),
                        first.index("ShouldTryRematchBattle"))
        self.assertIn("call_if_unset FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION, PRL_EventScript_GymExtra_KANTO_ERIKA_LegacyBigRoot",script)
        self.assertIn("setflag FLAG_BADGE04_GET",script)
        self.assertIn("FLAG_GOT_TM19_FROM_ERIKA",script)

    def test_spec_updated_without_changing_other_gyms(self):
        spec=read("docs/prl_gym_rewards_and_storage_requirements.md")
        self.assertIn("| ERIKA | TM19 | BIG_ROOT |",spec)
        self.assertNotIn("| ERIKA | TM19 | MIRACLE_SEED |",spec)
        rewards=read("data/scripts/prl_gym_rewards.inc")
        self.assertEqual(rewards.split("@ Badge Heist recovery gifts:", 1)[0].count("addmoney 10000"),16)
        self.assertIn("giveitem ITEM_TWISTED_SPOON",rewards)
        self.assertIn("giveitem ITEM_POISON_BARB",rewards)

if __name__ == "__main__":
    unittest.main()
