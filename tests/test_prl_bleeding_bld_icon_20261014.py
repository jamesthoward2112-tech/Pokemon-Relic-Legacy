"""PRL bleeding has an independent red BLD status badge in all UI contexts.

This guards against the previous accidental use of poison's purple PSN icon.
The Bleeding gameplay bit (STATUS1_BLEED) is intentionally unchanged.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class BleedingIconTests(unittest.TestCase):
    def test_distinct_status_id_and_party_mapping(self):
        hdr = read("include/constants/party_menu.h")
        party = read("src/party_menu.c")
        self.assertIn("#define AILMENT_BLD   9", hdr)
        self.assertRegex(party, r"if \(status & STATUS1_BLEED\)\s+return AILMENT_BLD;")
        self.assertNotRegex(party, r"if \(status & STATUS1_BLEED\)\s+return AILMENT_PSN;")
        self.assertIn("sSpriteTemplate_PRLBleedStatus", party)
        self.assertIn("LoadSpriteSheet(&sSpriteSheet_PRLBleedStatus)", party)
        self.assertIn("LoadSpritePalette(&sSpritePalette_PRLBleedStatus)", party)

    def test_battle_bld_palette_and_own_tiles_for_all_battlers(self):
        gfx = read("src/graphics.c")
        battle = read("src/battle_interface.c")
        self.assertIn("gPRLBleedHealthboxGfx[4][24]", gfx)
        self.assertIn("gPRLBleedStatusIconGfx[32]", gfx)
        self.assertIn("gPRLBleedStatusIconPal[16]", gfx)
        self.assertIn("PAL_STATUS_BLD", battle)
        bleed = battle.split("else if (status & STATUS1_BLEED)", 1)[1].split("else if", 1)[0]
        self.assertIn("gPRLBleedHealthboxGfx[battler]", bleed)
        self.assertIn("statusPalId = PAL_STATUS_BLD", bleed)
        self.assertNotIn("HEALTHBOX_GFX_STATUS_PSN", bleed)

    def test_summary_badge_is_separate_from_psn_and_frb(self):
        summary = read("src/pokemon_summary_screen.c")
        partydata = read("src/data/party_menu.h")
        self.assertIn("sBleedStatusSpriteSheet", summary)
        self.assertIn("sBleedStatusSpritePalette", summary)
        self.assertIn("sSpriteTemplate_StatusBleed", summary)
        self.assertIn("statusAnim == AILMENT_BLD", summary)
        self.assertIn("sSpriteAnimTable_PRLBleed", partydata)
        self.assertIn("sSpriteTemplate_PRLBleedStatus", partydata)
        self.assertIn("sSpriteAnim_StatusFrostbite", partydata)
        self.assertIn("sSpriteAnim_StatusFrostbite", summary)

    def test_actual_poison_and_bleeding_mechanics_remain_distinct(self):
        battleflags = read("include/constants/battle.h")
        engine = read("src/battle_script_commands.c")
        self.assertIn("#define STATUS1_BLEED            (1 << 13)", battleflags)
        self.assertIn("MOVE_EFFECT_BLEED", engine)
        self.assertIn("status1 |= STATUS1_BLEED", engine)
        self.assertIn("status1 |= STATUS1_POISON", engine)


if __name__ == "__main__":
    unittest.main()
