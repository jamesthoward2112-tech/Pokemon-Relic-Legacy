"""PR95 regression guards: one-way S.S. Legacy ferry and Emerald lorry arrival."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

def content(name):
    return (ROOT / name).read_text(encoding="utf-8")

class PR95VoyageTests(unittest.TestCase):
    def test_ship_sails_to_truck_not_uncompiled_slateport(self):
        scene = content("data/maps/VermilionCity_Frlg/scripts.inc")
        block = scene.split("VermilionCity_EventScript_PRLHoennBoardingPreflight::")[1].split("VermilionCity_EventScript_PRLHoennTransferFailed::")[0]
        for required in ("special DoSeagallopFerryScene", "setvar VAR_PRL_REGION_STAGE, 1",
                         "setvar VAR_LITTLEROOT_INTRO_STATE, 8",
                         "SEAGALLOP_PRL_HOENN_TRUCK",
                         "setdynamicwarp MAP_LITTLEROOT_TOWN, 3, 10",
                         "setdynamicwarp MAP_LITTLEROOT_TOWN, 12, 10"):
            self.assertIn(required, block)
        self.assertNotIn("warp MAP_LITTLEROOT_TOWN, 7, 18", block)
        ferry = content("src/seagallop.c")
        self.assertIn("ExecuteTruckSequence : FieldCB_DefaultWarpExit", ferry)
        self.assertIn("MAP_INSIDE_OF_TRUCK", ferry)
        self.assertIn("SEAGALLOP_PRL_HOENN_TRUCK", content("include/constants/seagallop.h"))

    def test_original_emerald_truck_and_safe_exit(self):
        self.assertIn("setstepcallback STEP_CB_TRUCK", content("data/maps/InsideOfTruck/scripts.inc"))
        town = content("data/maps/LittlerootTown/scripts.inc")
        seq = town.split("LittlerootTown_EventScript_PRLStepOffArrivalTruck::")[1].split("LittlerootTown_Text_PRLTruckArrived::")[0]
        for needle in ("LittlerootTown_Movement_PlayerStepOffTruck",
                       "setvar VAR_PRL_REGION_STAGE, 3",
                       "setvar VAR_LITTLEROOT_INTRO_STATE, 0",
                       "removeobject 5", "removeobject 6"):
            self.assertIn(needle, seq)
        self.assertNotIn("warpsilent MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F", seq)
        self.assertIn("SLATEPORT CITY", town)

    def test_map_ids_and_existing_regional_transfer_are_retained(self):
        gen = content("tools/mapjson/mapjson.cpp")
        self.assertIn('map_name == "InsideOfTruck"', gen)
        self.assertEqual(2, gen.count('json_to_string(layout, "id", true) != "LAYOUT_INSIDE_OF_TRUCK"'))
        self.assertIn('"InsideOfTruck", "LAYOUT_INSIDE_OF_TRUCK"',
                      content("tools/prl/verify_dual_region_binary.py"))
        self.assertIn("PRLHoennCommitKantoTransfer",
                      content("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc"))
        # Old saves entering at region stage 1 without the temporary truck
        # marker still use PR94's proven outdoor fallback.
        self.assertIn("LittlerootTown_Text_PRLSafeHoennArrival",
                      content("data/maps/LittlerootTown/scripts.inc"))

if __name__ == "__main__":
    unittest.main()
