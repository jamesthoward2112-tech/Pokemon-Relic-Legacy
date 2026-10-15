"""PR97: video 1000042380.mp4, sailor, ghost trucks, starter sprites, PC bucket repair."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class PR97HoennFixes(unittest.TestCase):
    def test_sailor_shortens_without_removing_safety_checks(self):
        script = read("data/maps/VermilionCity_Frlg/scripts.inc")
        text = script.split("VermilionCity_Text_PRLHoennFinalConfirm::",1)[1].split(
            "VermilionCity_Text_PRLHoennTransferFailed::",1)[0]
        self.assertIn("one-way",text)
        self.assertIn("SAVE",text)
        self.assertNotIn("Your KANTO POKéMON",text)
        self.assertNotIn("Your Bag items",text)
        self.assertNotIn("REMOTE PC",text)
        self.assertIn("specialvar VAR_RESULT, PRLHoennCheckPCSpaceForParty",script)
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("When you choose a partner",lab)
        self.assertIn("Your old Bag goes to the",lab)

    def test_firered_truck_objects_collide_at_original_locations(self):
        src = read("tools/mapjson/mapjson.cpp")
        self.assertIn('mapName == "LittlerootTown"',src)
        self.assertIn("i == 4 || i == 5",src)
        self.assertIn('bool prlArrivalTruck = version == "firered"',src)
        self.assertIn('(prlArrivalTruck ? "3" : json_to_string(obj_event, "elevation"))',src)
        self.assertIn('text << "\\tobject_event " << i + 1',src)
        # The animated interior remains compiled and uses the original engine.
        self.assertIn("ExecuteTruckSequence", read("src/seagallop.c"))
        self.assertIn("MAP_INSIDE_OF_TRUCK", read("data/maps/InsideOfTruck/map.json"))

    def test_lab_never_renders_fixed_ball_metatiles_in_prl_stage(self):
        sc = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        for name in ("PRLWaitForBirch", "PRLSetupStarterRoom", "PRLHideStarterRoom"):
            part = sc.split("LittlerootTown_ProfessorBirchsLab_EventScript_" + name + "::",1)[1].split("\n\n",1)[0]
            self.assertIn("setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB",part)
            self.assertNotIn("LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE",part)
        done = sc.split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete::",1)[1].split("\n\n",1)[0]
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 2",done)
        for ball in ("CYNDAQUIL","TOTODILE","CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_"+ball,done)

    def test_safe_sort_reserves_lowest_level_box_and_does_not_mislabel_spill(self):
        c = read("src/field_specials.c")
        fn = c.split("static bool32 PRLHoennPlaceInManifest(",1)[1].split(
            "// Read-only boarding preflight",1)[0]
        self.assertLess(fn.index("for (box = PRL_HOENN_OVERFLOW_START;"),
                        fn.index("for (box = PRL_HOENN_LEVEL_BOXES - 1;"))
        self.assertIn("if (box == PRL_HOENN_LEGENDS_BOX)",fn)
        self.assertIn("if (dest < PRL_HOENN_LEVEL_BOXES && preferred != dest)",c)
        self.assertIn("mixed[dest] = TRUE",c)
        self.assertIn("sPRLHoennMixedBoxName",c)
        self.assertIn("GetBoxMonData(&planned[laterBox][laterSlot], MON_DATA_LEVEL)",c)

    def test_birch_can_resort_existing_hoenn_save_without_touching_party_or_money(self):
        sc = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("specialvar VAR_RESULT, PRLHoennResortExistingPC",sc)
        self.assertIn("MSGBOX_YESNO",sc.split("EventScript_PRLBirchResearch::",1)[1].split(
            "EventScript_PRLSortDeclined::",1)[0])
        c = read("src/field_specials.c")
        op = c.split("u16 PRLHoennResortExistingPC(void)",1)[1].split(
            "// Repair one-time region inventory",1)[0]
        self.assertIn("AllocZeroed(sizeof(*planned) * TOTAL_BOXES_COUNT)",op)
        self.assertLess(op.index("PRLHoennSortEachBoxAscending(planned, used)"),
                        op.index("SetBoxMonAt(box, slot, &planned[box][slot])"))
        self.assertIn("Free(planned)",op)
        self.assertNotIn("ZeroMonData(",op)
        self.assertNotIn("ClearBag(",op)
        self.assertNotIn("SetMoney(",op)
        self.assertIn("def_special PRLHoennResortExistingPC",read("data/specials.inc"))
        self.assertIn("u16 PRLHoennResortExistingPC(void);",read("include/field_specials.h"))

if __name__ == "__main__":
    unittest.main()
