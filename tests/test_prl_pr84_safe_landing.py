"""PR84: safeguard crossing after the user video showed a black-screen crash.

The source tests aren't a substitute for running the ROM on a Champion save.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def read(p):
    return (ROOT / p).read_text(encoding="utf-8")


class PR84Safety(unittest.TestCase):
    def test_sailor_keeps_original_party_before_warp(self):
        sailor = read("data/maps/VermilionCity_Frlg/scripts.inc")
        s = sailor.split("VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[1].split(
            "VermilionCity_EventScript_PRLHoennNoRoom::", 1)[0]
        self.assertIn("PRLHoennCheckPCSpaceForParty", s)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 1", s)
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("map_script_2 VAR_PRL_REGION_STAGE, 1, LittlerootTown_EventScript_PRLOutdoorDockArrival", town)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 3", town)
        self.assertNotIn("PRLHoennCommitKantoTransfer", s)
        self.assertLess(s.index("setvar VAR_PRL_REGION_STAGE, 1"), s.index("warp MAP_LITTLEROOT_TOWN"))

    def test_sailor_pages_use_at_most_two_lines(self):
        sailor = read("data/maps/VermilionCity_Frlg/scripts.inc")
        msg = sailor.split("VermilionCity_Text_PRLHoennFinalConfirm::", 1)[1].split(
            "VermilionCity_Text_PRLHoennTransferFailed::", 1)[0]
        import re
        text = "".join(re.findall(r'\\.string "([^"]*)"', msg))
        for page in text.split(r"\p"):
            self.assertLessEqual(page.count(r"\n"), 1, page)
        self.assertIn("EXP SHARE", msg)

    def test_map_arrival_precedes_atomic_transfer_and_starter(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLHoennArrival", lab)
        self.assertIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer", lab)
        self.assertIn("warp MAP_VERMILION_CITY, 24, 32", lab)
        source = read("src/field_specials.c").split("u16 PRLHoennCommitKantoTransfer(void)", 1)[1]
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 4", source)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 4", lab)
        self.assertIn("u16 starterChoice = gSpecialVar_0x8004;", source)
        self.assertIn("CreateRandomMon(&gParties[B_TRAINER_PLAYER][0], starterSpecies, 5);", source)
        self.assertIn("gPartiesCount[B_TRAINER_PLAYER] = 1;", source)
        self.assertNotIn("gPartiesCount[B_TRAINER_PLAYER] = 0;", source)
        self.assertLess(source.index("PRLHoennSortEachBoxAscending("),
                        source.index("CreateRandomMon(&gParties[B_TRAINER_PLAYER][0]"))
        self.assertIn("VarSet(VAR_PRL_REGION_STAGE, 2)", source)


if __name__ == "__main__":
    unittest.main()
