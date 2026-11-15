"""PRR opening chapter challenges layered on the PRL Kanto source.

These guard against accidental overwrites of existing PRL rewards, gifts,
Gym levels, badge-heist save flags, or the 384-roster manifest.
"""
from pathlib import Path
import json
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]
PARTY = (ROOT / "src/data/trainers_frlg.party").read_text(encoding="utf8")
FOREST_SCRIPT = (ROOT / "data/maps/ViridianForest_Frlg/scripts.inc").read_text(encoding="utf8")
FOREST_MAP = json.loads((ROOT / "data/maps/ViridianForest_Frlg/map.json").read_text(encoding="utf8"))
FLAGS = (ROOT / "include/constants/flags_frlg.h").read_text(encoding="utf8")
ROOT_FLAGS = (ROOT / "include/constants/flags.h").read_text(encoding="utf8")

def trainer_section(key):
    return PARTY.split(f"=== {key} ===", 1)[1].split("\n===", 1)[0]

class OpeningDifficultyTests(unittest.TestCase):
    def test_brock_keeps_all_three_prl_species_and_level_sixteen_cap(self):
        team = trainer_section("TRAINER_LEADER_BROCK")
        self.assertIn("AI: Smart Trainer", team)
        self.assertEqual(["Geodude", "Kabuto", "Onix"],
                         re.findall(r"^(Geodude|Kabuto|Onix)(?: @ .*)?$", team, re.M))
        self.assertEqual([13, 14, 16], [int(x) for x in re.findall(r"^Level: (\d+)$", team, re.M)])
        self.assertIn("Stealth Rock", team)
        self.assertIn("Rock Tomb", team)
        self.assertIn("Bulldoze", team)

    def test_charlie_opt_in_and_reward_claimable_after_full_bag(self):
        npcs = [x for x in FOREST_MAP["object_events"] if x.get("script") == "ViridianForest_EventScript_Charlie"]
        self.assertEqual(1, len(npcs))
        self.assertEqual("TRAINER_TYPE_NONE", npcs[0]["trainer_type"])
        self.assertEqual("0", npcs[0]["trainer_sight_or_berry_tree_id"])
        reward = FOREST_SCRIPT.split("ViridianForest_EventScript_PRRCharlieReward::", 1)[1].split("ViridianForest_EventScript_PRRCharlieNoRoom::", 1)[0]
        self.assertIn("checkitemspace ITEM_ABILITY_CAPSULE, 1", reward)
        self.assertIn("giveitem_msg", reward)
        self.assertLess(reward.index("giveitem_msg"), reward.index("setflag FLAG_PRR_FOREST_MINIBOSS_REWARD"))
        self.assertIn("goto_if_eq VAR_RESULT, FALSE, ViridianForest_EventScript_PRRCharlieNoRoom", reward)
        self.assertIn("goto_if_unset FLAG_PRR_FOREST_MINIBOSS_REWARD", FOREST_SCRIPT)
        self.assertIn("TRAINER_BUG_CATCHER_CHARLIE", FOREST_SCRIPT)
        self.assertNotIn("FLAG_BADGE01_GET", reward)
        team = trainer_section("TRAINER_BUG_CATCHER_CHARLIE")
        self.assertIn("AI: Smart Trainer", team)
        self.assertIn("Butterfree @ Oran Berry", team)
        self.assertEqual([8, 9, 10], [int(x) for x in re.findall(r"^Level: (\d+)$", team, re.M)])

    def test_no_reuse_prl_boss_flag_or_badge_heist_save_bits(self):
        self.assertRegex(FLAGS, r"#define FLAG_PRR_FOREST_MINIBOSS_REWARD\s+0x4AD")
        self.assertNotIn("FLAG_UNUSED_0x4AD", FLAGS)
        reserved = [
            "FLAG_DEFEATED_BROCK",
            "FLAG_PRL_BADGE_HEIST_STARTED",
            "FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED",
        ]
        for name in reserved:
            self.assertIn(name, FLAGS)
        self.assertIn("FLAG_PRL_GYM_HOENN_NORMAN_EXTRA", ROOT_FLAGS)
        self.assertIn("FLAG_PRL_GOT_BULBASAUR_GIFT", FOREST_SCRIPT)
        self.assertIn("ViridianForest_EventScript_PRLBulbasaurAide", FOREST_SCRIPT)

if __name__ == "__main__":
    unittest.main()
