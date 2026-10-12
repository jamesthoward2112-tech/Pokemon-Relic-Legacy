"""Regression tests for Kanto relic starter move/ability descriptions.

All strings deliberately fit the two-line GBA Summary windows.
Battle mechanics and IDs must remain stable for existing saves.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def entry(path, constant):
    content = (ROOT / path).read_text()
    return content.split(f"[{constant}] =", 1)[1].split("\n},", 1)[0]


def displayed_lines(source):
    match = re.search(r'\.description = COMPOUND_STRING\("([^"]+)"\)', source)
    if match is None:
        raise AssertionError("No description found")
    return match.group(1).split(r"\n")


class TestRelicStarterText(unittest.TestCase):
    def test_move_names_descriptions_and_unchanged_mechanics(self):
        specifications = (
            ("MOVE_EDEN_BLOOM", "Eden Bloom", "TYPE_GRASS",
             "MOVE_EFFECT_POISON", 30),
            ("MOVE_BASTION_CANNON", "Bastion Cannon", "TYPE_STEEL",
             "MOVE_EFFECT_STAT_PLUS", 30),
        )
        for move, name, move_type, side_effect, chance in specifications:
            with self.subTest(move=move):
                source = entry("src/data/moves_info.h", move)
                self.assertIn(f'.name = COMPOUND_STRING("{name}")', source)
                self.assertIn(f".type = {move_type}", source)
                self.assertIn(".power = 95,", source)
                self.assertIn(".accuracy = 100,", source)
                self.assertIn(".pp = 10,", source)
                self.assertIn(f".moveEffect = {side_effect}", source)
                self.assertIn(f".chance = {chance}", source)
                lines = displayed_lines(source)
                self.assertEqual(len(lines), 2)
                self.assertTrue(all(len(line) <= 27 for line in lines), lines)
                self.assertNotIn("PRL signature move.", source)

        fortotoise = entry("src/data/moves_info.h", "MOVE_BASTION_CANNON")
        self.assertIn(".defense = 1, .self = TRUE", fortotoise)

    def test_ability_descriptions_fit_and_match_battle_behaviour(self):
        expected = (
            ("ABILITY_EDENS_CANOPY", "Eden's Canopy", "Grassy Terrain", "status"),
            ("ABILITY_CITADEL_SHELL", "Citadel Shell", "20%", "Def drops"),
        )
        for ability, name, term1, term2 in expected:
            with self.subTest(ability=ability):
                source = entry("src/data/abilities.h", ability)
                self.assertIn(f'.name = _("{name}")', source)
                lines = displayed_lines(source)
                self.assertEqual(len(lines), 2)
                self.assertTrue(all(len(line) <= 27 for line in lines), lines)
                self.assertIn(term1, source)
                self.assertIn(term2, source)
        battle_util = (ROOT / "src/battle_util.c").read_text()
        stat_change = (ROOT / "src/battle_stat_change.c").read_text()
        self.assertIn("case ABILITY_EDENS_CANOPY:", battle_util)
        self.assertIn("case ABILITY_CITADEL_SHELL:", battle_util)
        self.assertIn("cv->abilities[cv->battlerDef] == ABILITY_CITADEL_SHELL", stat_change)


if __name__ == "__main__":
    unittest.main()
