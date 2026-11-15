"""Source-level guardrails for PRL events carried into PRR.

These tests verify source wiring, not a successful emulator playthrough.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]
ROUTE = ROOT / "data/maps/Route23_Frlg/scripts.inc"
GATE = ROOT / "data/maps/IndigoPlateau_Exterior_Frlg/scripts.inc"
ITEMS = ROOT / "src/data/items.h"


class PrrPrlCarryoverTests(unittest.TestCase):
    def test_badge_heist_keeps_balloon_arrival_boarding_and_escape(self):
        source = ROUTE.read_text(encoding="utf-8")
        start = source.index("Route23_EventScript_PRLGreatBadgeHeist::")
        stop = source.index("Route23_EventScript_PRLGreatBadgeHeistDone::")
        heist = source[start:stop]
        steps = (
            "FLAG_PRL_BADGE_HEIST_STARTED",
            "Route23_Movement_PRLBalloonEnter",
            "Route23_Movement_PRLJessieBoard",
            "Route23_Movement_PRLJamesBoard",
            "Route23_Movement_PRLMeowthBoard",
            "Route23_Movement_PRLBalloonTakeoff",
        )
        for step in steps:
            self.assertIn(step, heist, f"Missing badge-heist step: {step}")
        self.assertEqual(sorted(heist.index(s) for s in steps),
                         [heist.index(s) for s in steps])
        for number in range(1, 9):
            self.assertIn(f"FLAG_BADGE{number:02}_GET", heist)
        self.assertNotRegex(heist, r"clearflag\s+FLAG_BADGE\d+_GET")
        self.assertIn("Route23_Movement_PRLBalloonEnter:", source)
        self.assertIn("Route23_Movement_PRLBalloonTakeoff:", source)

    def test_indigo_gate_checks_recovery_progress(self):
        source = GATE.read_text(encoding="utf-8")
        self.assertIn("FLAG_PRL_BADGE_HEIST_STARTED", source)
        self.assertIn("FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", source)

    def test_kanto_rocket_encounters_not_lost(self):
        paths = (
            "MtMoon_B1F_Frlg",
            "RocketHideout_B4F_Frlg",
            "SilphCo_11F_Frlg",
            "VictoryRoad_2F_Frlg",
        )
        for name in paths:
            script = ROOT / f"data/maps/{name}/scripts.inc"
            source = script.read_text(encoding="utf-8")
            self.assertRegex(source, r"(?i)Jessie|James", name)

    def test_pokemon_centre_upstairs_remain(self):
        for name in ("CeladonCity", "CeruleanCity", "FuchsiaCity", "PewterCity", "ViridianCity"):
            self.assertTrue((ROOT / f"data/maps/{name}_PokemonCenter_2F_Frlg").is_dir(), name)

    def test_dream_patch_reuses_existing_hidden_ability_item(self):
        source = ITEMS.read_text(encoding="utf-8")
        block = re.search(r"\[ITEM_ABILITY_PATCH\]\s*=\s*\{(.*?)\n\s*\},", source, re.S)
        self.assertIsNotNone(block)
        self.assertIn('ITEM_NAME("Dream Patch")', block.group(1))
        self.assertIn("ItemUseOutOfBattle_AbilityPatch", block.group(1))


if __name__ == "__main__":
    unittest.main()
