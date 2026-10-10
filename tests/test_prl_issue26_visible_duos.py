"""Issue #26 source regression: two visible Eevee Twins at each mandatory story scene.

These tests cannot substitute for fresh/old-save emulator coverage.
"""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
CASES = (
    ("Route3_Frlg", "ROUTE3", "VAR_PRL_RECURRING_ROUTE3_TRIGGER", "0"),
    ("SSAnne_2F_Corridor_Frlg", "SS_ANNE", "VAR_MAP_SCENE_S_S_ANNE_2F_CORRIDOR", "1"),
    ("PokemonTower_1F_Frlg", "TOWER", "VAR_PRL_RECURRING_TOWER_TRIGGER", "0"),
    ("Route23_Frlg", "ROUTE23", "VAR_MAP_SCENE_ROUTE23", "8"),
)

class Issue26VisibleDuos(unittest.TestCase):
    def test_all_four_maps_have_two_approved_twin_actors(self):
        for map_name, tag, _, _ in CASES:
            with self.subTest(map=map_name):
                map_data = json.loads((ROOT / f"data/maps/{map_name}/map.json").read_text())
                actors = [obj for obj in map_data["object_events"] if obj.get("local_id") in (
                    f"LOCALID_PRL_STEEVE_{tag}", f"LOCALID_PRL_NEEVEE_{tag}")]
                self.assertEqual(len(actors), 2)
                self.assertEqual({actor["graphics_id"] for actor in actors}, {"OBJ_EVENT_GFX_TWIN"})
                self.assertEqual({actor["flag"] for actor in actors}, {f"FLAG_PRL_RECURRING_STEEVE_NEEVEE_{tag}"})
                self.assertEqual(len({actor["y"] for actor in actors}), 1)
                self.assertLessEqual(abs(actors[0]["x"] - actors[1]["x"]), 2)
                self.assertTrue(all(actor["trainer_type"] == "TRAINER_TYPE_NONE" for actor in actors))

    def test_coordinate_triggers_have_actual_npc_context_and_departure(self):
        for map_name, tag, scene_var, scene_val in CASES:
            with self.subTest(map=map_name):
                map_data = json.loads((ROOT / f"data/maps/{map_name}/map.json").read_text())
                triggered = [e for e in map_data["coord_events"] if e["script"].endswith("PRLSteeveNeeveeTrigger")]
                self.assertGreaterEqual(len(triggered), 1)
                self.assertTrue(all(e["var"] == scene_var and e["var_value"] == scene_val for e in triggered))
                s = (ROOT / f"data/maps/{map_name}/scripts.inc").read_text()
                self.assertRegex(s, rf"trainerbattle_double TRAINER_STEEVE_NEEVEE_{tag}[^\n]*localId=LOCALID_PRL_STEEVE_{tag}, continueScript=TRUE")
                for actor in (f"LOCALID_PRL_STEEVE_{tag}", f"LOCALID_PRL_NEEVEE_{tag}"):
                    self.assertIn(f"applymovement {actor}", s)
                    self.assertIn(f"removeobject {actor}", s)
                self.assertIn("special HasEnoughMonsForDoubleBattle", s)
                self.assertIn(f"goto_if_defeated TRAINER_STEEVE_NEEVEE_{tag}", s)
                self.assertIn(f"setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_{tag}", s)
                self.assertIn("SaveSync", s)

    def test_mt_moon_native_trainer_vision_and_warps(self):
        m=json.loads((ROOT/"data/maps/MtMoon_B1F_Frlg/map.json").read_text())
        self.assertEqual(m["coord_events"],[])
        for name in ("JESSIE","JAMES"):
            a=next(x for x in m["object_events"] if x.get("local_id")==f"LOCALID_PRL_{name}_MT_MOON")
            self.assertEqual(a["trainer_type"],"TRAINER_TYPE_NORMAL")
            self.assertEqual(a["movement_type"],"MOVEMENT_TYPE_FACE_DOWN")
        self.assertTrue(any(w["x"]==39 and w["y"]==4 and w["dest_map"]=="MAP_MT_MOON_B2F" for w in m["warp_events"]))
        s=(ROOT/"data/maps/MtMoon_B1F_Frlg/scripts.inc").read_text()
        for name in ("JESSIE","JAMES"):
            self.assertRegex(s,rf"trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON[^\n]*localId=LOCALID_PRL_{name}_MT_MOON")
        self.assertIn("MtMoon_B1F_Text_PRL_JessiePostBattle::",s)
        self.assertIn("MtMoon_B1F_Text_PRL_JamesPostBattle::",s)


if __name__ == "__main__":
    unittest.main()
