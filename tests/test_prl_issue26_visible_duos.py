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
    ("VictoryRoad_1F_Frlg", "ROUTE23", "VAR_TEMP_1", "0"),
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
                # Native trainers remain visible; defeated/sync scripts gate repeat battles.
                self.assertEqual({actor["flag"] for actor in actors}, {"0"})
                axis = "x" if map_name == "SSAnne_2F_Corridor_Frlg" else "y"
                self.assertEqual(len({actor[axis] for actor in actors}), 1)
                gap = abs(actors[0]["x"] - actors[1]["x"]) + abs(actors[0]["y"] - actors[1]["y"])
                self.assertIn(gap, (1, 2))
                expected_type = ("TRAINER_TYPE_SEE_ALL_DIRECTIONS" if tag == "TOWER" else "TRAINER_TYPE_NORMAL")
                self.assertTrue(all(actor["trainer_type"] == expected_type for actor in actors))

    def test_native_trainer_sight_has_both_npc_contexts_and_save_sync(self):
        # The approved native sight handlers replaced the earlier coordinate/departure prototype.
        for map_name, tag, _, _ in CASES:
            with self.subTest(map=map_name):
                map_data = json.loads((ROOT / f"data/maps/{map_name}/map.json").read_text())
                self.assertFalse(any(e["script"].endswith("PRLSteeveNeeveeTrigger") for e in map_data["coord_events"]))
                actors = [o for o in map_data["object_events"] if o.get("local_id") in
                          (f"LOCALID_PRL_STEEVE_{tag}", f"LOCALID_PRL_NEEVEE_{tag}")]
                self.assertTrue(all(int(o["trainer_sight_or_berry_tree_id"]) > 0 for o in actors))
                s = (ROOT / f"data/maps/{map_name}/scripts.inc").read_text()
                for actor in (f"LOCALID_PRL_STEEVE_{tag}", f"LOCALID_PRL_NEEVEE_{tag}"):
                    self.assertRegex(s, rf"trainerbattle_double TRAINER_STEEVE_NEEVEE_{tag}[^\n]*localId={actor}")
                self.assertIn(f"goto_if_defeated TRAINER_STEEVE_NEEVEE_{tag}", s)
                self.assertIn(f"setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_{tag}", s)
                self.assertTrue("SaveSync" in s or (tag == "ROUTE23" and "PRLTwinsPreviouslyWon" in s))

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
