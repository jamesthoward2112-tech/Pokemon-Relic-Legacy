"""PR80: prevent ghost boarding of a departed S.S. Anne without affecting Mew."""
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


class PR80DepartedShip(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.exterior = json.loads(
            (ROOT / "data/maps/SSAnne_Exterior_Frlg/map.json").read_text(encoding="utf-8")
        )
        cls.exterior_script = (
            ROOT / "data/maps/SSAnne_Exterior_Frlg/scripts.inc"
        ).read_text(encoding="utf-8")
        cls.field = (
            ROOT / "src/field_control_avatar.c"
        ).read_text(encoding="utf-8")

    def test_two_original_ship_interiors_blocked_only_after_departure(self):
        interior = {
            (warp["x"], warp["y"])
            for warp in self.exterior["warp_events"]
            if warp["dest_map"] == "MAP_SSANNE_1F_CORRIDOR"
        }
        self.assertEqual(interior, {(32, 14), (33, 15)})
        triggers = {
            (e["x"], e["y"]): e
            for e in self.exterior["coord_events"]
            if e["script"] == "SSAnne_Exterior_EventScript_PRLNoBoardingDepartedShip"
        }
        self.assertEqual(set(triggers), interior)
        for e in triggers.values():
            self.assertEqual(e["var"], "VAR_MAP_SCENE_VERMILION_CITY")
            self.assertEqual(e["var_value"], "3")
        self.assertIn("SSAnne_Exterior_EventScript_PRLNoBoardingDepartedShip::", self.exterior_script)
        self.assertIn("The S.S. ANNE has already", self.exterior_script)

    def test_held_arrow_and_warp_cannot_bypass_departure_guard(self):
        block = self.field.split("static s8 GetWarpEventAtPosition(struct MapHeader *mapHeader,", 1)[1]
        block = block.split("static bool32 ShouldTriggerScriptRun", 1)[0]
        self.assertIn("LAYOUT_SSANNE_EXTERIOR", block)
        self.assertIn("VarGet(VAR_MAP_SCENE_VERMILION_CITY) == 3", block)
        for check in ("x == 32 && y == 14", "x == 33 && y == 15"):
            self.assertIn(check, block)
        self.assertIn("return WARP_ID_NONE;", block)

    def test_no_regression_to_pier_truck_mew_or_sailor(self):
        self.assertEqual(
            [(w["x"], w["y"]) for w in self.exterior["warp_events"]
             if w["dest_map"] == "MAP_VERMILION_CITY"],
            [(31, 5), (32, 5), (33, 5)],
        )
        self.assertTrue(any(
            bg.get("script") == "SSAnne_Exterior_EventScript_PRLTruckMew"
            for bg in self.exterior["bg_events"]
        ))
        self.assertIn("FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED", self.exterior_script)
        vermilion = json.loads(
            (ROOT / "data/maps/VermilionCity_Frlg/map.json").read_text(encoding="utf-8")
        )
        sailor = next(o for o in vermilion["object_events"]
                      if o.get("local_id") == "LOCALID_VERMILION_FERRY_SAILOR")
        self.assertEqual((sailor["x"], sailor["y"]), (24, 33))
        self.assertEqual(
            {(w["x"], w["y"]) for w in vermilion["warp_events"]
             if w["dest_map"] == "MAP_SSANNE_EXTERIOR"},
            {(22, 34), (23, 34), (24, 34)},
        )

    def test_regional_voyage_has_separate_ship_name(self):
        oak = (ROOT / "data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc").read_text(
            encoding="utf-8"
        )
        sailor = (ROOT / "data/maps/VermilionCity_Frlg/scripts.inc").read_text(
            encoding="utf-8"
        )
        self.assertIn("S.S. LEGACY, one way for now.", oak)
        self.assertIn("Your S.S. LEGACY voyage waits", oak)
        self.assertIn("ready to board the S.S. LEGACY?", sailor)
        self.assertIn("The S.S. ANNE has already", self.exterior_script)


if __name__ == "__main__":
    unittest.main()
