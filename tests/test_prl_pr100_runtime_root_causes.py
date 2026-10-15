"""PR100 continuation: guard root causes found during gameplay investigation.

These are source regression assertions, not emulator-playthrough evidence.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(relative):
    return (ROOT / relative).read_text(encoding="utf-8")

class PR100RuntimeRootCauses(unittest.TestCase):
    def test_every_box_level_read_uses_xp_not_party_level_field(self):
        code = read("src/field_specials.c")
        self.assertNotRegex(code, r"GetBoxMonData\([^;]*?MON_DATA_LEVEL\)")
        self.assertIn("GetLevelFromBoxMonExp(mon)", code)
        self.assertIn("GetLevelFromBoxMonExp(&planned[box][right])", code)
        self.assertIn("GetLevelFromBoxMonExp(&planned[laterBox][laterSlot])", code)
        self.assertIn("level = GetLevelFromBoxMonExp(mon);", code)
        self.assertIn("plan->destToSource", code)

    def test_parked_truck_blocks_exact_three_by_three_sprite(self):
        code = read("src/field_specials.c").split(
            "u16 PRLHoennBlockArrivalTruckFootprint(void)\n{", 1)[1].split(
            "u16 PRLHoennBankArrivalFunds", 1)[0]
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 3", code)
        self.assertIn("dx < 3", code)
        self.assertIn("dy < 3", code)
        self.assertIn("MAP_OFFSET + 8 + dy", code)
        self.assertNotIn("dx < 4", code)

    def test_delivery_truck_goes_on_first_lab_entry_not_after_starter(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        transition = lab.split("LittlerootTown_ProfessorBirchsLab_OnTransition:", 1)[1].split("\n\n", 1)[0]
        self.assertLess(
            transition.index("call_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLTruckDeparted"),
            transition.index("goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLWaitForBirch"))
        depart = lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLTruckDeparted::", 1)[1].split("return", 1)[0]
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK", depart)
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_TRUCK", depart)
        spawn = read("src/event_object_movement.c")
        gate = spawn.split("static bool8 PRLRejectHoennBridgeObject", 1)[1].split("u8 TrySpawnObjectEventTemplate(", 1)[0]
        self.assertIn("if (stage != 1)", gate)
        self.assertIn("return TRUE;", gate)

    def test_birch_dialogue_replaces_three_ball_table_all_hoenn_stages(self):
        code = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        for name in ("PRLWaitForBirch", "PRLSetupStarterRoom",
                     "PRLHideStarterRoom", "PRLStarterComplete"):
            block = code.split("EventScript_" + name + "::", 1)[1]
            block = block.split("\n\n", 1)[0] if name != "PRLStarterComplete" else block.split("LittlerootTown_ProfessorBirchsLab_Text_PRLHoennWelcome", 1)[0]
            self.assertIn("setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB\n", block)
            self.assertNotIn("_WITH_TABLE", block)
        for species in ("RALTS","POOCHYENA","GEMLIN"):
            self.assertIn("showmonpic SPECIES_"+species,code)
        self.assertIn("LittlerootTown_ProfessorBirchsLab_EventScript_PRLBirchWaiting::",code)
        self.assertIn("goto LittlerootTown_ProfessorBirchsLab_EventScript_PRLOfferRalts",code)
        self.assertNotIn("LittlerootTown_ProfessorBirchsLab_EventScript_PRLPlaceStarterBalls::",code)
        for ball in ("CYNDAQUIL","TOTODILE","CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_"+ball,code)


    def test_aunt_rosie_text_never_exceeds_two_lines_or_28_chars_per_page(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        dialogue = town.split("LittlerootTown_Text_PRLRosieArrival::", 1)[1].split(
            "LittlerootTown_Text_PRLShuttleArrival::", 1)[0]
        chunks = re.findall(r'\.string "([^"]+)"', dialogue)
        self.assertTrue(chunks)
        message = "".join(chunks).replace("{PLAYER}", "ABCDEFG")
        self.assertTrue(message.endswith("$"))
        for page in message.rstrip("$").split(r"\p"):
            lines = page.split(r"\n")
            self.assertLessEqual(len(lines), 2, lines)
            for line in lines:
                self.assertLessEqual(len(line), 28, line)



if __name__ == "__main__":
    unittest.main()
