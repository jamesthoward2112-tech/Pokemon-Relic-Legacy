"""Validate approved 2026-10-11 duo trainer presentation assets and final Kanto J&J roster."""
import hashlib
import pathlib
import struct
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class PRLDuoApprovedArt(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        import sys
        sys.path.insert(0, str(ROOT / "tools"))
        import prl_duo_20261011_assets
        cls.expected_paths = prl_duo_20261011_assets.unpack()

    def test_packs_have_full_bg0_tiles_and_correct_palette_bank(self):
        for name in ("jessie_james", "eevee_twins"):
            with self.subTest(name=name):
                root = ROOT / "graphics/battle_transitions/prl" / name
                self.assertEqual(len(root.with_suffix(".4bpp").read_bytes()), 512 * 32)
                self.assertEqual(len(root.with_suffix(".gbapal").read_bytes()), 16 * 2)
                m = root.with_suffix(".bin").read_bytes()
                self.assertEqual(len(m), 32 * 32 * 2)
                tiles = struct.unpack("<1024H", m)
                self.assertTrue(all((t >> 12) == 13 for t in tiles))
                self.assertTrue(all((t & 0x3ff) < 512 for t in tiles))

    def test_exclusive_routing_uses_original_kanto_trainer_ids(self):
        setup = (ROOT / "src/battle_setup.c").read_text()
        for token in (
            "TRAINER_JESSIE_JAMES_MT_MOON",
            "TRAINER_JESSIE_JAMES_HIDEOUT",
            "TRAINER_JESSIE_JAMES_SILPH",
            "TRAINER_JESSIE_JAMES_VICTORY_ROAD",
            "TRAINER_STEEVE_NEEVEE_ROUTE3",
            "TRAINER_STEEVE_NEEVEE_SS_ANNE",
            "TRAINER_STEEVE_NEEVEE_TOWER",
            "TRAINER_STEEVE_NEEVEE_ROUTE23",
        ):
            self.assertIn("case " + token + ":", setup)
        self.assertIn("B_TRANSITION_JESSIE_JAMES", setup)
        self.assertIn("B_TRANSITION_EEVEE_TWINS", setup)

    def test_patternweave_sprite_data_uses_matching_bank_13(self):
        code = (ROOT / "src/battle_transition.c").read_text()
        self.assertIn("Task_PRL_JessieJames", code)
        self.assertIn("Task_PRL_EeveeTwins", code)
        self.assertIn("BG_PLTT_ID(13)", code)
        self.assertIn("sPRL_jessie_james_Tiles", code)
        self.assertIn("sPRL_eevee_twins_Tiles", code)

    def test_kanto_finale_has_strong_unchanged_mascots(self):
        party = (ROOT / "src/data/trainers_frlg.party").read_text()
        section = party.split("=== TRAINER_JESSIE_JAMES_VICTORY_ROAD ===", 1)[1].split("===", 1)[0]
        self.assertEqual(section.count("Level:"), 6)
        self.assertNotIn("Hypno\n", section)
        for line in ("Gyarados @ Sitrus Berry", "Pikachu @ Light Ball", "Meowth @ Focus Sash",
                     "Ability: Technician", "- Fake Out", "- Feint", "- Protect"):
            self.assertIn(line, section)


if __name__ == "__main__":
    unittest.main()
