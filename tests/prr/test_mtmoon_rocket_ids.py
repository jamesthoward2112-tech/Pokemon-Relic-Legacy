"""Ensure PRR preserves Jessie/James/Meowth scripted object IDs by map floor."""
from pathlib import Path
import json
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]

class MtMoonRocketIdsTests(unittest.TestCase):
    def test_each_floor_has_unique_jessie_james_meowth_local_ids(self):
        ids = {}
        for floor in ("B1F", "B2F"):
            folder = ROOT / "data/maps" / f"MtMoon_{floor}_Frlg"
            data = json.loads((folder / "map.json").read_text(encoding="utf8"))
            selection = {item["local_id"] for item in data["object_events"]
                         if item.get("local_id", "").startswith("LOCALID_PRL_")}
            expected = {f"LOCALID_PRL_{name}_MT_MOON_{floor}" for name in ("JESSIE", "JAMES", "MEOWTH")}
            self.assertEqual(expected, selection)
            ids[floor] = selection
        self.assertTrue(ids["B1F"].isdisjoint(ids["B2F"]))

    def test_b1f_rocket_battle_targets_its_actual_objects(self):
        source = (ROOT / "data/maps/MtMoon_B1F_Frlg/scripts.inc").read_text(encoding="utf8")
        for name in ("JESSIE", "JAMES", "MEOWTH"):
            self.assertIn(f"LOCALID_PRL_{name}_MT_MOON_B1F", source)
        self.assertIn("TRAINER_JESSIE_JAMES_MT_MOON", source)
        self.assertIn("MtMoon_B1F_EventScript_PRL_RocketWon", source)
        self.assertNotRegex(source, r"LOCALID_PRL_(JESSIE|JAMES|MEOWTH)_MT_MOON\b")
        self.assertNotIn("MT_MOON_B2F", source.replace("MtMoon_B2F_Text", ""))

if __name__ == "__main__":
    unittest.main()
