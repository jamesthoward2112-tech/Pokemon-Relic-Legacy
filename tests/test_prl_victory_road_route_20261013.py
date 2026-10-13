"""Save-safe PRL Victory Road entrances, mandatory duo scenes and battle art."""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(p):
    return (ROOT / p).read_text()

def mp(n):
    return json.loads(read(f"data/maps/{n}/map.json"))

class VictoryRoadRouteTests(unittest.TestCase):
    def test_stolen_badge_gate_does_not_teleport_after_eight_recoveries(self):
        s = read("src/overworld.c")
        block = s.split("static bool32 PRLIsBlockedVictoryRoadWarp(", 1)[1].split("\n}", 1)[0]
        self.assertIn("FlagGet(FLAG_PRL_BADGE_HEIST_STARTED)", block)
        self.assertIn("!FlagGet(FLAG_PRL_BADGE_HEIST_ALL_RECOVERED)", block)
        self.assertNotIn("!FlagGet(FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED)", block)
        self.assertIn("PRL_ROUTE23_SOUTH_WARP_ID", s)

    def test_cave_entrance_and_upper_exit_keep_correct_bidirectional_warps(self):
        route = mp("Route23_Frlg")["warp_events"]
        f1 = mp("VictoryRoad_1F_Frlg")["warp_events"]
        f2 = mp("VictoryRoad_2F_Frlg")["warp_events"]
        self.assertEqual((route[0]["dest_map"], route[0]["dest_warp_id"]), ("MAP_VICTORY_ROAD_1F_FRLG", "1"))
        self.assertEqual((route[1]["dest_map"], route[1]["dest_warp_id"]), ("MAP_VICTORY_ROAD_2F", "6"))
        self.assertEqual((f1[1]["dest_map"], f1[1]["dest_warp_id"]), ("MAP_ROUTE23", "0"))
        self.assertEqual((f2[6]["dest_map"], f2[6]["dest_warp_id"]), ("MAP_ROUTE23", "1"))

    def test_twins_spawn_on_clear_first_floor_tiles_not_the_outdoor_statues(self):
        ids = ("LOCALID_PRL_STEEVE_ROUTE23", "LOCALID_PRL_NEEVEE_ROUTE23")
        cave = mp("VictoryRoad_1F_Frlg")
        route = mp("Route23_Frlg")
        actors = [o for o in cave["object_events"] if o.get("local_id") in ids]
        self.assertEqual({(o["x"], o["y"]) for o in actors}, {(10,18), (12,18)})
        self.assertEqual(len(actors), 2)
        self.assertEqual([o for o in route["object_events"] if o.get("local_id") in ids], [])
        s = read("data/maps/VictoryRoad_1F_Frlg/scripts.inc")
        self.assertIn("MAP_SCRIPT_ON_FRAME_TABLE, VictoryRoad_1F_PRLTwinsOnFrame", s)
        self.assertIn("map_script_2 VAR_TEMP_1, 0, VictoryRoad_1F_EventScript_PRLTwinsEnter", s)
        for id in ids:
            self.assertIn("applymovement " + id + ", Common_Movement_ExclamationMark", s)
        self.assertIn("trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23", s)
        self.assertIn("goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23", s)
        self.assertIn("setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23", s)

    def test_rockets_confront_player_near_far_exit_with_approved_balloon(self):
        route = mp("Route23_Frlg")
        actors = {o.get("local_id"): o for o in route["object_events"]}
        self.assertEqual((actors["LOCALID_PRL_HEIST_JESSIE"]["x"], actors["LOCALID_PRL_HEIST_JESSIE"]["y"]), (16,31))
        self.assertEqual((actors["LOCALID_PRL_HEIST_JAMES"]["x"], actors["LOCALID_PRL_HEIST_JAMES"]["y"]), (20,31))
        # The base balloon stays put to protect the first approved badge heist.
        self.assertEqual((actors["LOCALID_PRL_HEIST_BALLOON"]["x"], actors["LOCALID_PRL_HEIST_BALLOON"]["y"]), (8,30))
        triggers = [x for x in route["coord_events"] if x["script"] == "Route23_EventScript_PRLFinalRocketTrigger"]
        self.assertGreaterEqual(len(triggers), 5)
        s = read("data/maps/Route23_Frlg/scripts.inc")
        self.assertIn("applymovement LOCALID_PRL_HEIST_JESSIE, Common_Movement_ExclamationMark", s)
        self.assertIn("applymovement LOCALID_PRL_HEIST_JAMES, Common_Movement_ExclamationMark", s)
        self.assertIn("goto Route23_EventScript_PRLFinalRocketBattle", s)
        self.assertIn("setobjectxy LOCALID_PRL_HEIST_BALLOON, 8, 151", s)
        self.assertIn("setobjectxy LOCALID_PRL_HEIST_BALLOON, 18, 30", s)
        self.assertIn("Route23_Movement_PRLBalloonEnter:\n\tfly_down", s)
        self.assertIn("Route23_Movement_PRLBalloonTakeoff:\n\tfly_up", s)
        f2 = read("data/maps/VictoryRoad_2F_Frlg/scripts.inc")
        self.assertIn("call_if_set FLAG_PRL_BADGE_HEIST_ALL_RECOVERED", f2)
        self.assertIn("VictoryRoad_2F_EventScript_PRL_RocketGateLetThrough", f2)

    def test_both_toxeon_and_nimbeon_are_six_pixels_lower(self):
        s = read("src/data/pokemon/species_info/prl_custom.h")
        for name in ("Toxeon", "Nimbeon"):
            with self.subTest(name=name):
                self.assertRegex(s, rf"gMonFrontPic_{name}, .frontPicSize = MON_COORDS_SIZE\\(64, 64\\), .frontPicYOffset = 6")

if __name__ == "__main__":
    unittest.main()
