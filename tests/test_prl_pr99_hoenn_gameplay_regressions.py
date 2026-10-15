"""PR99 source safety checks for defects visible in Pizza Boy videos 2396/2397.

These tests prove code paths are wired, not that emulator gameplay is verified.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class PR99GameplayRegressions(unittest.TestCase):
    def test_trainer_card_toggle_frees_old_windows_before_reinit(self):
        card = read("src/trainer_card.c")
        toggle = card.split("else if (JOY_NEW(SELECT_BUTTON)", 1)[1].split("else if (JOY_NEW(B_BUTTON))", 1)[0]
        self.assertLess(toggle.index("FreeAllWindowBuffers();"), toggle.index("SetMainCallback2(CB2_InitTrainerCard);"))
        self.assertIn("sData->gfxLoadState = 0;", toggle)
        self.assertIn("sData->bgPalLoadState = 0;", toggle)
        self.assertIn("SetVBlankCallback(NULL);", toggle)

    def test_pc_resorts_after_normal_close_not_while_cursor_is_moving(self):
        storage = read("src/pokemon_storage_system.c")
        exit_fn = storage.split("static void Task_ChangeScreen(u8 taskId)\n{", 1)[1].split("\n}", 1)[0]
        self.assertIn("PRLHoennResortExistingPC();", exit_fn)
        self.assertLess(exit_fn.index("PRLHoennResortExistingPC();"),
                        exit_fn.index("FreePokeStorageData();"))
        self.assertNotIn("PRLHoennResortExistingPC();", storage.split("static void Task_PlaceMon(u8 taskId)\n{", 1)[1].split("static void Task_ShiftMon", 1)[0])
        # Guarded and transactional: nothing committed if the snapshot cannot be built.
        special = read("src/field_specials.c")
        sorter = special.split("u16 PRLHoennResortExistingPC(void)\n{", 1)[1].split("u16 PRLHoennAutoSortPCOnce", 1)[0]
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 2", sorter)
        self.assertIn("if (plan == NULL)", sorter)
        self.assertIn("plan->destToSource", sorter)
        self.assertIn("PRLHoennSortGroup", sorter)

    def test_one_truck_and_collision_only_during_arrival(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("LittlerootTown_EventScript_PRLRemoveOtherShuttle", town)
        self.assertIn("removeobject 5", town)
        self.assertIn("removeobject 6", town)
        self.assertIn("special PRLHoennBlockArrivalTruckFootprint", town)
        src = read("src/field_specials.c")
        helper = src.split("u16 PRLHoennBlockArrivalTruckFootprint(void)\n{", 1)[1].split("u16 PRLHoennBankArrivalFunds", 1)[0]
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 3", helper)
        self.assertIn("MapGridSetMetatileImpassabilityAt", helper)
        self.assertIn("MAP_OFFSET", helper)

    def test_outdoor_props_removed_after_objects_spawn_aunt_does_not_block_door(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("map_script_2 VAR_PRL_REGION_STAGE, 2, LittlerootTown_EventScript_PRLRepairHoennProps", town)
        self.assertIn("removeobject LOCALID_LITTLEROOT_RIVAL", town)
        self.assertIn("removeobject LOCALID_LITTLEROOT_BIRCH", town)
        self.assertIn("setobjectxyperm LOCALID_LITTLEROOT_MOM, 6, 9", town)
        self.assertIn("setobjectxyperm LOCALID_LITTLEROOT_MOM, 13, 9", town)
        self.assertIn("AUNT ROSIE:", town)

    def test_starter_table_and_reload_clear_baked_balls(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        before = lab.split("EventScript_PRLSetupStarterRoom::", 1)[1].split("\n\n", 1)[0]
        after = lab.split("EventScript_PRLStarterComplete::", 1)[1].split("LittlerootTown_ProfessorBirchsLab_Text_PRLHoennWelcome::", 1)[0]
        self.assertIn("LAB_WITH_TABLE", before)
        self.assertIn("setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE", after)
        self.assertIn("warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 7, 5", after)
        for ball in ("CYNDAQUIL", "TOTODILE", "CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_" + ball, after)
        storage = read("src/pokemon_storage_system.c")
        self.assertIn("PRLHoennStorageLevelLocked(GetBoxMonDataAt", storage)

if __name__ == "__main__":
    unittest.main()
