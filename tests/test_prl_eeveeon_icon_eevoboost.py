"""Regression checks for PRL Eeveeon icon colours and EevoBoost battle name."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8")


class PRLEeveeonIconEevoboostTests(unittest.TestCase):
    def test_eevoboost_keeps_save_compatible_move_id_and_five_pp(self):
        ids = source("include/constants/moves.h")
        self.assertRegex(ids, r"MOVE_EEVOBOOST\s*=\s*MOVES_COUNT_DYNAMAX")
        self.assertRegex(ids, r"MOVE_EEVOBOOST\s*=\s*MOVES_COUNT_DYNAMAX,\s*MOVES_COUNT_ALL")
        moves = source("src/data/moves_info.h")
        match = re.search(r"\[MOVE_EEVOBOOST\]\s*=\s*\{(.*?)\n    \},", moves, re.S)
        self.assertIsNotNone(match)
        info = match.group(1)
        self.assertIn('COMPOUND_STRING("Eevoboost")', info)
        self.assertRegex(info, r"\.pp\s*=\s*5\s*,")
        self.assertIn(".effect = EFFECT_EXTREME_EVOBOOST", info)

    def test_battle_prints_custom_move_name_for_used_current_and_last_move(self):
        code = source("src/battle_message.c")
        cases = (
            ("case STRINGID_USEDMOVE:", "case STRINGID_BATTLEEND:", "currentMove"),
            ("case B_TXT_CURRENT_MOVE:", "case B_TXT_LAST_MOVE:", "currentMove"),
            ("case B_TXT_LAST_MOVE:", "case B_TXT_LAST_ITEM:", "originallyUsedMove"),
        )
        for start, end, name in cases:
            with self.subTest(case=start):
                self.assertIn(start, code)
                block = code.split(start, 1)[1].split(end, 1)[0]
                self.assertIn(f"gBattleMsgDataPtr->{name} != MOVE_EEVOBOOST", block)
                self.assertIn(f"GetMoveName(gBattleMsgDataPtr->{name})", block)
                self.assertIn(f"!IsZMove(gBattleMsgDataPtr->{name})", block)
                self.assertIn(f"!IsMaxMove(gBattleMsgDataPtr->{name})", block)

    def test_eeveeon_has_exclusive_eight_palette_with_16_entries(self):
        code = source("src/pokemon_icon.c")
        mon = source("src/data/pokemon/species_info/prl_custom.h")
        palette = re.search(
            r"static const u16 sPRLEeveeonIconPalette\[16\]\s*=\s*\{(.*?)\};", code, re.S
        )
        self.assertIsNotNone(palette)
        self.assertEqual(len(re.findall(r"RGB\(", palette.group(1))), 16)
        table = code.split("gMonIconPaletteTable[] =", 1)[1].split("};", 1)[0]
        self.assertEqual(table.count("POKE_ICON_BASE_PAL_TAG +"), 8)
        self.assertIn("{ sPRLEeveeonIconPalette, POKE_ICON_BASE_PAL_TAG + 7 }", table)
        eeveeon = mon.split("[SPECIES_EEVEEON] =", 1)[1].split("[SPECIES_NIMBEON]", 1)[0]
        self.assertIn(".iconSprite = gMonIcon_Eeveeon, .iconPalIndex = 7", eeveeon)
        self.assertIn("ARRAY_COUNT(gMonIconPaletteTable)", code)


if __name__ == "__main__":
    unittest.main()
