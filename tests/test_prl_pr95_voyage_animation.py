"""PR95: Kanto boat sequence, Slateport connection and Emerald truck arrival."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class PR95Voyage(unittest.TestCase):
    def test_sailor_uses_boat_cutscene_and_does_not_warp_to_disabled_slateport(self):
        sailor = read("data/maps/VermilionCity_Frlg/scripts.inc")
        boarding = sailor.split("VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[1].split(
            "VermilionCity_EventScript_PRLHoennTransferFailed::", 1)[0]
        self.assertIn("special DoSeagallopFerryScene", boarding)
        self.assertIn("SEAGALLOP_PRL_SLATEPORT_TRANSFER", boarding)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 1", boarding)
        self.assertNotIn("warp MAP_LITTLEROOT_TOWN", boarding)
        self.assertNotIn("warp MAP_SLATEPORT_CITY", boarding)
        c = read("src/seagallop.c")
        self.assertIn("MAP_GROUP(MAP_INSIDE_OF_TRUCK)", c)
        self.assertIn("if (gSpecialVar_0x8006 == SEAGALLOP_PRL_SLATEPORT_TRANSFER)", c)
        self.assertIn("gFieldCallback = ExecuteTruckSequence", c)
        self.assertIn("gFieldCallback = FieldCB_DefaultWarpExit", c)

    def test_truck_map_layout_compiled_without_reindexing(self):
        gen = read("tools/mapjson/mapjson.cpp")
        self.assertIn('"InsideOfTruck",', gen)
        self.assertIn('"LAYOUT_INSIDE_OF_TRUCK"', gen)
        check = read("tools/prl/verify_dual_region_binary.py")
        self.assertIn('("gMapGroup_IndoorDynamic", "InsideOfTruck", "LAYOUT_INSIDE_OF_TRUCK", True)', check)
        source = read("data/maps/InsideOfTruck/scripts.inc")
        self.assertIn("setvar VAR_LITTLEROOT_INTRO_STATE, 8", source)
        self.assertIn("setdynamicwarp MAP_LITTLEROOT_TOWN, 3, 10", source)
        self.assertIn("setdynamicwarp MAP_LITTLEROOT_TOWN, 12, 10", source)
        self.assertNotIn("setrespawn HEAL_LOCATION_LITTLEROOT", source.split(
            "InsideOfTruck_EventScript_PRLSlateportShuttle::", 1)[1].split("InsideOfTruck_OnLoad:", 1)[0])

    def test_player_steps_off_once_without_original_house_intro(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        end = town.split("LittlerootTown_EventScript_PRLFinishShuttleArrival::", 1)[1].split(
            "LittlerootTown_Text_PRLShuttleArrival::", 1)[0]
        self.assertIn("applymovement LOCALID_PLAYER, LittlerootTown_Movement_PlayerStepOffTruck", end)
        self.assertIn("setvar VAR_LITTLEROOT_INTRO_STATE, 0", end)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 3", end)
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE", end)
        self.assertNotIn("LittlerootTown_EventScript_GoInsideWithMom", end)
        self.assertNotIn("warp MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F", end)
        self.assertNotIn("warp MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F", end)
        self.assertIn("LittlerootTown_EventScript_PRLPrepareShuttleTruck::", town)

    def test_birch_starter_transfer_remains_atomic(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer", lab)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 4", lab)

if __name__ == "__main__":
    unittest.main()
