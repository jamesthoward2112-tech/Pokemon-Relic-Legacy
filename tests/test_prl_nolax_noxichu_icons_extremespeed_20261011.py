"""Nolax and Noxichu icon, relearning and Extreme Speed regressions.

GBA graphics and icon tags are checked directly rather than relying on
declarations alone; emulator visual/Move Relearner QA remains a release gate.
"""
import struct
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

class RelicIconAndMoveTests(unittest.TestCase):
    def test_nolax_uses_distinct_32x64_4bpp_party_icon(self):
        nolax = (ROOT / "graphics/pokemon/nolax/icon.png").read_bytes()
        snorlax = (ROOT / "graphics/pokemon/snorlax/icon.png").read_bytes()
        self.assertNotEqual(nolax, snorlax, "Nolax must not reuse its Snorlax fallback")
        self.assertEqual(nolax[:8], b"\x89PNG\r\n\x1a\n")
        self.assertEqual(struct.unpack(">II", nolax[16:24]), (32, 64))
        self.assertIn(nolax[25], (3,), "Use the existing indexed 4bpp icon artwork")
        self.assertEqual(nolax[24], 4, "4 bits per pixel; GBA 16-colour graphics")
        self.assertIn(b"PLTE", nolax)
        self.assertIn(b"tRNS", nolax)
        species = (ROOT / "src/data/pokemon/species_info/prl_custom.h").read_text()
        block = species.split("[SPECIES_NOLAX] =",1)[1].split("[SPECIES_NUMEW]",1)[0]
        self.assertIn(".iconSprite = gMonIcon_Nolax, .iconPalIndex = 3", block)

    def test_nolax_knock_off_is_level_one_relearner_eligible(self):
        learnsets = (ROOT / "src/data/pokemon/level_up_learnsets/prl_custom.h").read_text()
        learnset = learnsets.split("sNolaxLevelUpLearnset[] =",1)[1].split("LEVEL_UP_END",1)[0]
        self.assertIn("LEVEL_UP_MOVE(1, MOVE_KNOCK_OFF)",learnset)
        self.assertIn("LEVEL_UP_MOVE(0, MOVE_EXTREME_SPEED)",learnset)
        self.assertEqual(learnset.count("MOVE_KNOCK_OFF"), 1)

    def test_extreme_speed_base_pp_is_ten(self):
        moves = (ROOT / "src/data/moves_info.h").read_text()
        block = moves.split("[MOVE_EXTREME_SPEED] =",1)[1].split("[MOVE_ANCIENT_POWER] =",1)[0]
        self.assertIn(".pp = 10,",block)
        self.assertNotIn(".pp = 5,",block)
        self.assertIn(".power = 80,",block)
        self.assertIn(".priority =",block)

    def test_noxichu_pc_palette_has_dedicated_tag_not_display_collision(self):
        storage = (ROOT / "src/pokemon_storage_system.c").read_text()
        start = storage.index("PALTAG_MON_ICON_0 = POKE_ICON_BASE_PAL_TAG")
        end = storage.index("PALTAG_ITEM_ICON_0", start)
        enum = storage[start:end]
        self.assertIn("PALTAG_MON_ICON_5,",enum)
        self.assertIn("PALTAG_MON_ICON_6,",enum)
        self.assertLess(enum.index("PALTAG_MON_ICON_6,"),enum.index("PALTAG_DISPLAY_MON,"))
        self.assertIn("PALTAG_MON_ICON_0 + gSpeciesInfo[species].iconPalIndex",storage)
        self.assertIn("LoadMonIconPalettes();",storage)
        mon = (ROOT / "src/data/pokemon/species_info.h").read_text()
        nox = mon.split("[SPECIES_NOXICHU] =",1)[1].split("#include \"species_info/prl_custom.h\"",1)[0]
        self.assertIn(".iconSprite = gMonIcon_Noxichu,",nox)
        self.assertIn(".iconPalIndex = 6,",nox)
        icon = (ROOT / "src/pokemon_icon.c").read_text()
        self.assertIn("gMonIconPalettes[6], POKE_ICON_BASE_PAL_TAG + 6",icon)
        self.assertIn("ARRAY_COUNT(gMonIconPaletteTable)",icon)

if __name__=="__main__":
    unittest.main()
