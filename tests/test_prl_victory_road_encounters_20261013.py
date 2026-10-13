"""Victory Road cave encounters and original badge-heist preservation."""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

def map_data(name):
    return json.loads(read(f"data/maps/{name}/map.json"))


class VictoryRoadCaveEncountersTests(unittest.TestCase):
    def test_twins_are_inside_and_not_obstructed_by_outdoor_statues(self):
        road = map_data("Route23_Frlg")
        old_twins = [o for o in road["object_events"]
                     if o.get("local_id") in ("LOCALID_PRL_STEEVE_ROUTE23", "LOCALID_PRL_NEEVEE_ROUTE23")]
        self.assertEqual(len(old_twins), 2)
        self.assertTrue(all(o["flag"] == "FLAG_HIDE_PRL_BADGE_HEIST_TRIO" for o in old_twins))
        inside = map_data("VictoryRoad_1F_Frlg")
        twins = [o for o in inside["object_events"] if o.get("local_id", "").startswith("LOCALID_PRL_VR_TWIN_")]
        self.assertEqual({(o["x"], o["y"]) for o in twins}, {(9, 13), (10, 13)})
        self.assertTrue(all(o["trainer_type"] == "TRAINER_TYPE_NORMAL" for o in twins))
        self.assertTrue(all(o["flag"] == "0" for o in twins))
        self.assertTrue(all(o["elevation"] == 4 for o in twins))
        self.assertTrue(all(o["movement_type"] == "MOVEMENT_TYPE_FACE_DOWN" for o in twins))
        self.assertEqual(abs(twins[0]["x"] - twins[1]["x"]), 1)
        self.assertTrue(all(o["y"] < 16 for o in twins))
        boulder = next(o for o in inside["object_events"] if o.get("local_id") == "LOCALID_VICTORY_ROAD_1F_BOULDER")
        self.assertEqual((boulder["x"], boulder["y"]), (7, 18))
        # Native trainer sight from the two adjacent upper-ledge twins.
        self.assertFalse(any("PRLSteeveNeeveeTrigger" in e["script"] for e in inside["coord_events"]))
        self.assertEqual({o["trainer_sight_or_berry_tree_id"] for o in twins}, {"3"})
        script = read("data/maps/VictoryRoad_1F_Frlg/scripts.inc")
        self.assertNotIn("VictoryRoad_1F_EventScript_PRLSteeveNeeveeTrigger::", script)
        for actor in ("Steevee", "Neevee"):
            self.assertIn(f"VictoryRoad_1F_EventScript_PRL{actor}::", script)
        for local_id in ("STEEVE", "NEEVEE"):
            self.assertIn(f"localId=LOCALID_PRL_VR_TWIN_{local_id}", script)
            self.assertNotIn(f"removeobject LOCALID_PRL_VR_TWIN_{local_id}", script)
        self.assertEqual(script.count("trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23"), 2)
        self.assertIn("VictoryRoad_1F_Text_PRLTwinsFarewell::", script)
        self.assertIn("See you in HOENN", script)
        self.assertIn("setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23", script)
        self.assertNotIn("FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23, VictoryRoad_1F_EventScript_PRLTwinsDone", script)

    def test_victory_road_trainers_match_the_post_badge_curve(self):
        party = read("src/data/trainers_frlg.party")
        maps = ["VictoryRoad_1F_Frlg", "VictoryRoad_2F_Frlg", "VictoryRoad_3F_Frlg"]
        by_map = {}
        for map_name in maps:
            scripts = read(f"data/maps/{map_name}/scripts.inc")
            trainers = set(re.findall(r"trainerbattle_(?:single|double|no_intro) (TRAINER_[A-Z_0-9]+)", scripts))
            by_map[map_name] = trainers
        floors = {
            "VictoryRoad_1F_Frlg": (58, 60),
            "VictoryRoad_2F_Frlg": (58, 65),
            "VictoryRoad_3F_Frlg": (60, 64),
        }
        for floor, (lo, hi) in floors.items():
            for name in by_map[floor]:
                marker = f"=== {name} ==="
                self.assertIn(marker, party)
                entry = party.split(marker, 1)[1].split("\\n=== TRAINER_", 1)[0]
                levels = [int(x) for x in re.findall(r"Level: (\\d+)", entry)]
                self.assertTrue(levels, name)
                for lv in levels:
                    self.assertGreaterEqual(lv, lo, name)
                    self.assertLessEqual(lv, hi, name)
        self.assertEqual(len(set.union(*by_map.values())), 14)

    def test_rocket_finale_occurs_at_cave_exit_and_cannot_be_clicked_past(self):
        cave = map_data("VictoryRoad_2F_Frlg")
        rocket = {o.get("local_id"): o for o in cave["object_events"]}
        for name in ("JESSIE", "JAMES", "MEOWTH", "BALLOON"):
            self.assertIn("LOCALID_PRL_VR_ROCKET_" + name, rocket)
        self.assertEqual((rocket["LOCALID_PRL_VR_ROCKET_JESSIE"]["x"], rocket["LOCALID_PRL_VR_ROCKET_JESSIE"]["y"]), (45, 9))
        self.assertEqual((rocket["LOCALID_PRL_VR_ROCKET_JAMES"]["x"], rocket["LOCALID_PRL_VR_ROCKET_JAMES"]["y"]), (47, 9))
        triggers = [e for e in cave["coord_events"] if e["script"] == "VictoryRoad_2F_EventScript_PRL_RocketGate"]
        self.assertTrue({(47, 12), (48, 11), (49, 12), (47, 13), (49, 13)} <=
                        {(e["x"], e["y"]) for e in triggers})
        for e in triggers:
            self.assertEqual(e["var"], "VAR_PRL_VICTORY_ROAD_ROCKET_GATE")
            self.assertEqual(e["var_value"], "0")
        s = read("data/maps/VictoryRoad_2F_Frlg/scripts.inc")
        for name in ("JESSIE", "JAMES"):
            self.assertIn(f"applymovement LOCALID_PRL_VR_ROCKET_{name}, Route23_Movement_PRLHeistExclamation", s)
            self.assertIn(f"applymovement LOCALID_PRL_VR_ROCKET_{name}, VictoryRoad_2F_Movement_PRL_RocketStepIn", s)
        self.assertIn("trainerbattle_double TRAINER_JESSIE_JAMES_VICTORY_ROAD", s)
        self.assertIn("FLAG_PRL_BADGE_HEIST_ALL_RECOVERED", s)
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", s)
        self.assertIn("Route23_Movement_PRLBalloonEnter", s)
        self.assertIn("Route23_Movement_PRLBalloonTakeoff", s)
        self.assertIn("MAP_VICTORY_ROAD_2F", s)

    def test_warp_no_longer_redirects_recovered_badge_player_to_south_route(self):
        src = read("src/overworld.c")
        start = src.split("static bool32 PRLIsBlockedVictoryRoadWarp", 1)[1].split("\n}", 1)[0]
        self.assertIn("FlagGet(FLAG_PRL_BADGE_HEIST_STARTED)", start)
        self.assertIn("!FlagGet(FLAG_PRL_BADGE_HEIST_ALL_RECOVERED)", start)
        self.assertNotIn("!FlagGet(FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED)", start)
        route = map_data("Route23_Frlg")
        cave = map_data("VictoryRoad_1F_Frlg")
        self.assertEqual(route["warp_events"][0]["dest_map"], "MAP_VICTORY_ROAD_1F_FRLG")
        self.assertEqual(str(route["warp_events"][0]["dest_warp_id"]), "1")
        self.assertEqual(cave["warp_events"][1]["dest_map"], "MAP_ROUTE23")
        self.assertEqual(str(cave["warp_events"][1]["dest_warp_id"]), "0")
        plateau = read("data/maps/IndigoPlateau_Exterior_Frlg/scripts.inc")
        self.assertIn("IndigoPlateau_Exterior_EventScript_PRLReturnToCave::", plateau)
        self.assertIn("goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", plateau)
        self.assertIn("warp MAP_VICTORY_ROAD_2F, 44, 10", plateau)

    def test_sprites_lowered_without_custom_image_replacement(self):
        src = read("src/data/pokemon/species_info/prl_custom.h")
        for name in ("TOXEON", "NIMBEON"):
            with self.subTest(name=name):
                block = src.split(f"[SPECIES_{name}] =", 1)[1].split("\n},", 1)[0]
                self.assertIn(".frontPicYOffset = 5", block)
                self.assertIn(f"gMonFrontPic_{name.title()}", block)

    def test_approved_initial_balloon_heist_remains_on_original_route(self):
        route = read("data/maps/Route23_Frlg/scripts.inc")
        heist = route.split("Route23_EventScript_PRLGreatBadgeHeist::", 1)[1].split("Route23_EventScript_PRLGreatBadgeHeistDone::", 1)[0]
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_STARTED", heist)
        self.assertIn("setobjectxy LOCALID_PRL_HEIST_BALLOON, 8, 151", heist)
        self.assertIn("Route23_Movement_PRLBalloonTakeoff", heist)
        visible = route.split("Route23_EventScript_PRLHeistVisibility::", 1)[1].split("Route23_EventScript_PRLHeistVisibilityDone::", 1)[0]
        self.assertNotIn("clearflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO", visible)


if __name__ == "__main__":
    unittest.main()
