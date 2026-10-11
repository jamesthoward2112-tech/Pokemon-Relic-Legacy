"""PRL B4F Jessie & James placement: right-facing sight intercepts Giovanni approach.

Source-only map and progression invariants. Needs emulator verification of LoS,
visible positions, and save compatibility before marking in-game tested.
"""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "data/maps/RocketHideout_B4F_Frlg/map.json"
SCRIPT = ROOT / "data/maps/RocketHideout_B4F_Frlg/scripts.inc"
LOCATIONS = {
    "LOCALID_PRL_JESSIE_HIDEOUT": (15, 20),
    "LOCALID_PRL_JAMES_HIDEOUT": (15, 21),
    "LOCALID_PRL_MEOWTH_HIDEOUT": (14, 21),
}

class HideoutApproachTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = json.loads(MAP.read_text(encoding="utf8"))
        cls.script = SCRIPT.read_text(encoding="utf8")
        cls.trio = {
            o["local_id"]: o
            for o in cls.data["object_events"]
            if o.get("local_id") in LOCATIONS
        }

    def test_trio_positions_match_user_marked_b4f_corridor(self):
        self.assertEqual(set(self.trio), set(LOCATIONS))
        self.assertEqual(
            {n: (o["x"], o["y"]) for n, o in self.trio.items()}, LOCATIONS
        )
        self.assertEqual(
            len({(o["x"], o["y"]) for o in self.data["object_events"]}),
            len(self.data["object_events"]), "No occupied coordinates collide"
        )
        self.assertEqual(self.data["coord_events"], [], "Keep native trainer sight")

    def test_both_trainers_face_east_and_see_the_giovanni_corridor(self):
        # Player's screenshot shows main approach at x=17.
        corridor_x = 17
        for name in ("LOCALID_PRL_JESSIE_HIDEOUT", "LOCALID_PRL_JAMES_HIDEOUT"):
            with self.subTest(trainer=name):
                a = self.trio[name]
                self.assertEqual(a["movement_type"], "MOVEMENT_TYPE_FACE_RIGHT")
                self.assertEqual(a["trainer_type"], "TRAINER_TYPE_NORMAL")
                self.assertLessEqual(corridor_x-a["x"], int(a["trainer_sight_or_berry_tree_id"]))
                self.assertGreater(corridor_x, a["x"])
                self.assertEqual(a["flag"], "FLAG_HIDE_PRL_ROCKET_HIDEOUT")
        m = self.trio["LOCALID_PRL_MEOWTH_HIDEOUT"]
        self.assertEqual(m["movement_type"], "MOVEMENT_TYPE_FACE_RIGHT")
        self.assertEqual(m["trainer_type"], "TRAINER_TYPE_NONE")

    def test_existing_save_events_and_giovanni_block_remain(self):
        for part in (
            "goto_if_unset FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT",
            "goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT",
            "addobject LOCALID_PRL_JESSIE_HIDEOUT",
            "addobject LOCALID_PRL_JAMES_HIDEOUT",
            "addobject LOCALID_PRL_MEOWTH_HIDEOUT",
            "TRAINER_JESSIE_JAMES_HIDEOUT",
            "goto_if_unset FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT",
            "FLAG_HIDE_CELADON_ROCKETS",
            "ITEM_SILPH_SCOPE",
        ):
            self.assertIn(part, self.script)
        for name in ("LOCALID_PRL_JESSIE_HIDEOUT", "LOCALID_PRL_JAMES_HIDEOUT"):
            self.assertIn(f"localId={name}", self.script)
        self.assertEqual(self.data["warp_events"][0]["dest_map"], "MAP_ROCKET_HIDEOUT_B3F")
        self.assertEqual(
            {tuple((e["x"],e["y"])) for e in self.data["warp_events"] if e["dest_map"]=="MAP_ROCKET_HIDEOUT_ELEVATOR"},
            {(20,23),(21,23)}
        )

if __name__ == "__main__":
    unittest.main()
