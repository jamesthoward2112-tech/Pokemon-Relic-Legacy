"""PR100: targeted regressions after 1000042400.mp4, not emulator substitutes."""
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def read(p): return (ROOT / p).read_text(encoding="utf-8")

class PR100HoennRegressions(unittest.TestCase):
    def test_region_labels_beneath_id_and_not_badge_row(self):
        card=read("src/trainer_card.c")
        text=card.split("case 6:\n        if (!sData->isLink && IS_FRLG)",1)[1].split("default:",1)[0]
        self.assertIn("FONT_SMALL, 174, 27", text)
        self.assertNotIn("23, 113",text)
        self.assertIn("sData->isHoenn ? sTrainerCardTextColors : sPRLKantoRegionColors",text)
        self.assertIn("TEXT_COLOR_RED, TEXT_COLOR_LIGHT_RED", card)
        self.assertIn("FreeAllWindowBuffers();",card.split("else if (JOY_NEW(SELECT_BUTTON)",1)[1].split("else if (JOY_NEW(B_BUTTON))",1)[0])

    def test_spawn_guard_stops_late_props_and_balls_without_kanto_changes(self):
        code=read("src/event_object_movement.c")
        helper=code.split("static bool8 PRLRejectHoennBridgeObject(",1)[1].split("u8 TrySpawnObjectEventTemplate",1)[0]
        self.assertIn("#if IS_FRLG",helper)
        self.assertIn("MAP_LITTLEROOT_TOWN",helper)
        self.assertIn("obj->localId == 7 || obj->localId == 8",helper)
        self.assertIn("obj->localId == 5 || obj->localId == 6",helper)
        self.assertIn("stage == 2 || stage == 3",helper)
        self.assertIn("obj->localId >= 4 && obj->localId <= 6",helper)
        self.assertIn("PRLRejectHoennBridgeObject(&objectEventTemplateLocal, mapNum, mapGroup)",code)
        self.assertIn("!PRLRejectHoennBridgeObject(template, gSaveBlock1Ptr->location.mapNum",code)

    def test_starter_balls_aligned_to_furniture_and_post_choice_blocked(self):
        gen=read("tools/mapjson/mapjson.cpp")
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn('mapName == "LittlerootTown_ProfessorBirchsLab"',gen)
        self.assertIn('prlHoennStarterBall ? to_string(9 + i - 3)',gen)
        for localid,x in (("CYNDAQUIL",9),("TOTODILE",10),("CHIKORITA",11)):
            self.assertIn("setobjectxy LOCALID_BIRCHS_LAB_"+localid+"_BALL, "+str(x)+", 4",lab)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 2", lab)
        self.assertIn("hideobjectat LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL",lab)

    def test_no_large_storage_snapshot_and_no_midmove_sort(self):
        src=read("src/field_specials.c")
        sort=src.split("u16 PRLHoennResortExistingPC(void)\n{",1)[1].split("// PR97 saves are already past",1)[0]
        self.assertIn("struct PRLHoennSortPlan",src)
        self.assertIn("plan = AllocZeroed(sizeof(*plan));",sort)
        self.assertNotIn("AllocZeroed(sizeof(*planned) * TOTAL_BOXES_COUNT)",sort)
        self.assertIn("plan->destToSource",sort)
        self.assertIn("PRLHoennPlaceInManifest(used, species, level",sort)
        self.assertLess(sort.index("PRLHoennPlaceInManifest(used, species, level"), sort.index("// Phase 4:"))
        self.assertNotIn("ZeroMonData(",sort)
        self.assertNotIn("ClearBag(",sort)
        self.assertNotIn("SetMoney(",sort)
        storage=read("src/pokemon_storage_system.c")
        entry=storage.split("static void EnterPokeStorage(u8 boxOption)\n{",1)[1].split("ResetTasks();",1)[0]
        self.assertIn("PRLHoennResortExistingPC();",entry)
        exit=storage.split("static void Task_ChangeScreen(u8 taskId)\n{",1)[1].split("static void",1)[0]
        self.assertIn("PRLHoennResortExistingPC();",exit)
        self.assertLess(exit.index("PRLHoennResortExistingPC();"),exit.index("FreePokeStorageData();"))
        self.assertIn("sStorage->boxOption != OPTION_SELECT_MON",exit)
        self.assertIn("PRLHoennStorageLevelLocked(GetBoxMonDataAt",storage)

if __name__=="__main__":
    unittest.main()
