"""Prevent every Littleroot exit from targeting a still-excluded Hoenn map."""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

class PR87HoennLandingGates(unittest.TestCase):
    def test_town_only_warps_to_approved_birch_lab(self):
        town = json.loads((ROOT / "data/maps/LittlerootTown/map.json").read_text(encoding="utf-8"))
        self.assertEqual(town.get("connections", []), [],
                         "Route 101 is not linked yet and must not be reachable")
        self.assertEqual(len(town["warp_events"]), 1)
        self.assertEqual(town["warp_events"][0]["dest_map"],
                         "MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB")

    def test_lab_exit_returns_to_safe_town(self):
        lab = json.loads((ROOT / "data/maps/LittlerootTown_ProfessorBirchsLab/map.json").read_text(encoding="utf-8"))
        self.assertTrue(lab["warp_events"])
        self.assertTrue(all(w["dest_map"] == "MAP_LITTLEROOT_TOWN"
                            for w in lab["warp_events"]))
    def test_no_region_save_layout_change(self):
        storage = (ROOT / "include/pokemon_storage_system.h").read_text(encoding="utf-8")
        self.assertIn("#define TOTAL_BOXES_COUNT       14", storage)

if __name__ == "__main__":
    unittest.main()
