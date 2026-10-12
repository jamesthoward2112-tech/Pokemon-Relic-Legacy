"""PRL Charaxis text regression: fit GBA Summary without altering move ID/effect."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

class TestCharaxisDescriptions(unittest.TestCase):
    def test_scorch_wings_move_text_and_existing_burn_mechanics(self):
        data = (ROOT / "src/data/moves_info.h").read_text()
        block = data.split("[MOVE_SCORCHWING] =", 1)[1].split("\n},", 1)[0]
        self.assertIn('.name = COMPOUND_STRING("Scorch Wings")', block)
        self.assertIn('Scorching wings strike the\\nfoe. May cause a burn.', block)
        self.assertIn('.power = 95,', block)
        self.assertIn('.accuracy = 100,', block)
        self.assertIn('.type = TYPE_FLYING,', block)
        self.assertIn('.moveEffect = MOVE_EFFECT_BURN, .chance = 20,', block)

    def test_thermal_wings_summary_text_is_two_short_lines(self):
        data = (ROOT / "src/data/abilities.h").read_text()
        block = data.split("[ABILITY_THERMAL_WINGS] =", 1)[1].split("\n},", 1)[0]
        match = re.search(r'\\.description = COMPOUND_STRING\\("([^"]+)"\\)', block)
        self.assertIsNotNone(match)
        lines = match.group(1).split(r"\\n")
        self.assertEqual(len(lines), 2)
        self.assertTrue(all(len(line) <= 29 for line in lines), lines)
        self.assertIn("20%", match.group(1))
        self.assertIn("recoil", match.group(1))

if __name__ == "__main__":
    unittest.main()
