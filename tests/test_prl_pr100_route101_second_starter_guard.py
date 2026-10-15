"""PR100 video 1000042408: no duplicate Emerald Route 101 starter after Birch's lab.

Source-level checks only; copied-save emulator replay is still required.
"""
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

def script(src, label, stop="\n\n"):
    return src.split(label + "::", 1)[1].split(stop, 1)[0]

class Route101SecondStarterGuards(unittest.TestCase):
    def test_route_transition_repairs_champion_even_if_stage_stale(self):
        src = read("data/maps/Route101/scripts.inc")
        transition = src.split("Route101_OnTransition:", 1)[1].split("\n\n", 1)[0]
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET, Route101_EventScript_PRLRescueAlreadyComplete", transition)
        for stage in (1, 2, 3, 4):
            self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, "+str(stage)+", Route101_EventScript_PRLRescueAlreadyComplete", transition)
        repair = script(src, "Route101_EventScript_PRLRescueAlreadyComplete")
        self.assertIn("setvar VAR_ROUTE101_STATE, 3", repair)
        for flag in ("FLAG_RESCUED_BIRCH", "FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG", "FLAG_HIDE_ROUTE_101_ZIGZAGOON"):
            self.assertIn("setflag "+flag, repair)

    def test_all_legacy_rescue_entrypoints_guard_before_side_effects(self):
        src = read("data/maps/Route101/scripts.inc")
        for label in ("StartBirchRescue", "BirchsBag", "PreventExitSouth", "PreventExitWest", "PreventExitNorth"):
            block = script(src, "Route101_EventScript_"+label)
            self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET, Route101_EventScript_PRLRescueAlreadyComplete", block)
            self.assertIn("goto_if_ne VAR_PRL_REGION_STAGE, 0, Route101_EventScript_PRLRescueAlreadyComplete", block)
            self.assertLess(block.index("goto_if_ne VAR_PRL_REGION_STAGE"), block.index("lock"))
        bag = script(src, "Route101_EventScript_BirchsBag", "Route101_EventScript_HideMayInBedroom::")
        self.assertLess(bag.index("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET"), bag.index("special ChooseStarter"))

    def test_route_birch_and_bag_props_blocked_for_champion_not_emerald(self):
        src = read("src/event_object_movement.c")
        code = src.split("static bool8 PRLRejectHoennBridgeObject(", 1)[1].split("u8 TrySpawnObjectEventTemplate(", 1)[0]
        self.assertIn("#if IS_FRLG",code)
        block = code.split("if (mapGroup == MAP_GROUP(MAP_ROUTE101)", 1)[1].split("if (mapGroup == MAP_GROUP(MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB)", 1)[0]
        self.assertIn("stage != 0 || FlagGet(FLAG_PRL_POSTHOF_OAK_TICKET)",block)
        self.assertIn("obj->localId >= 2 && obj->localId <= 5",block)

if __name__ == "__main__":
    unittest.main()
