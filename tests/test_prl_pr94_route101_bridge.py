"""PR94: Champion bypass of vanilla Birch rescue and bounded Route 101 opening."""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT/path).read_text(encoding="utf-8")

class PR94Route101Bridge(unittest.TestCase):
    def test_firered_compiles_route_101_without_renumbering(self):
        src = read("tools/mapjson/mapjson.cpp")
        self.assertIn('"Route101",', src)
        self.assertIn('"LAYOUT_ROUTE101"', src)
        self.assertIn('!prlHoennDestinationEnabled(destination)', src)
        # The compiled binary verifier checks all real pointer and connection targets.
        check = read("tools/prl/verify_dual_region_binary.py")
        self.assertIn('("gMapGroup_TownsAndRoutes", "Route101", "LAYOUT_ROUTE101", True)', check)
        self.assertIn('Route101 must connect north to Oldale', check)

    def test_original_emerald_connections_are_not_mutated(self):
        route = json.loads(read("data/maps/Route101/map.json"))
        town = json.loads(read("data/maps/LittlerootTown/map.json"))
        self.assertEqual({link["map"] for link in route["connections"]},
                         {"MAP_OLDALE_TOWN", "MAP_LITTLEROOT_TOWN"})
        self.assertEqual(town["connections"][0]["map"], "MAP_ROUTE101")

    def test_champion_already_completed_birch_rescue(self):
        src = read("data/maps/Route101/scripts.inc")
        welcome = src.split("Route101_OnTransition:", 1)[1].split("Route101_OnFrame:", 1)[0]
        for stage in (1,2,3,4):
            self.assertIn(f"goto_if_eq VAR_PRL_REGION_STAGE, {stage}, Route101_EventScript_PRLRescueAlreadyComplete", welcome)
        self.assertIn("setvar VAR_ROUTE101_STATE, 3", welcome)
        self.assertIn("setflag FLAG_RESCUED_BIRCH", welcome)
        for flag in ("FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE",
                     "FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG",
                     "FLAG_HIDE_ROUTE_101_ZIGZAGOON",
                     "FLAG_HIDE_ROUTE_101_BIRCH"):
            self.assertIn("setflag " + flag, welcome)
        # Vanilla rescue code is retained for the separate Emerald build only.
        self.assertIn("Route101_EventScript_StartBirchRescue::", src)
        self.assertIn("call ProfBirch_EventScript_UpdateLocation", welcome)

    def test_lab_awards_one_starter_and_sets_done_state(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        done = lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete::", 1)[1].split("\n\n", 1)[0]
        self.assertIn("setflag FLAG_RESCUED_BIRCH", done)
        self.assertIn("setflag FLAG_ADVENTURE_STARTED", done)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 2", done)
        self.assertIn("setvar VAR_OLDALE_TOWN_STATE, 1", done)

if __name__ == "__main__":
    unittest.main()
