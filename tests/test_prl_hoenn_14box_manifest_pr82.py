"""PR82: 14 save-compatible PC boxes, no write-before-transfer."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/field_specials.c").read_text(encoding="utf-8")
STORAGE = (ROOT / "include/pokemon_storage_system.h").read_text(encoding="utf-8")
SCRIPT = (ROOT / "data/maps/VermilionCity_Frlg/scripts.inc").read_text(encoding="utf-8")


class TestPR82FourteenBoxManifest(unittest.TestCase):
    def test_save_layout_not_expanded(self):
        self.assertRegex(STORAGE, r"(?m)^#define TOTAL_BOXES_COUNT\s+14$")
        self.assertRegex(STORAGE, r"(?m)^#define IN_BOX_ROWS\s+5\b")
        self.assertRegex(STORAGE, r"(?m)^#define IN_BOX_COLUMNS\s+6\b")

    def test_all_boxed_pokemon_party_and_eggs_included(self):
        manifest = SOURCE.split("static bool32 PRLHoennPlaceInManifest(", 1)[1]
        self.assertIn("PRL_HOENN_LEVEL_BOXES      10", SOURCE)
        self.assertIn("PRL_HOENN_OVERFLOW_START   10", SOURCE)
        self.assertIn("PRL_HOENN_OVERFLOW_END     13", SOURCE)
        self.assertIn("PRL_HOENN_LEGENDS_BOX      13", SOURCE)
        self.assertIn("gSpeciesInfo[species].isRestrictedLegendary", manifest)
        self.assertIn("gSpeciesInfo[species].isMythical", manifest)
        self.assertIn("for (box = 0; box < TOTAL_BOXES_COUNT; box++)", manifest)
        self.assertIn("for (slot = 0; slot < IN_BOX_COUNT; slot++)", manifest)
        self.assertIn("GetBoxMonData(mon, MON_DATA_SPECIES)", manifest)
        self.assertIn("GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES)", manifest)
        self.assertIn("partyCount > freeSlots", manifest)

    def test_fallback_uses_all_remaining_boxes(self):
        manifest = SOURCE.split("static bool32 PRLHoennPlaceInManifest(", 1)[1].split(
            "// Read-only boarding preflight", 1)[0]
        self.assertIn("for (box = PRL_HOENN_OVERFLOW_START;", manifest)
        self.assertIn("for (box = 0; box < TOTAL_BOXES_COUNT; box++)", manifest)
        self.assertIn("boxUsed[box]++", manifest)
        self.assertIn("return FALSE;", manifest)

    def test_boarding_stays_disabled_until_transaction_is_proven(self):
        board = SCRIPT.split("VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[1].split(
            "VermilionCity_EventScript_PRLHoennNoRoom::", 1)[0]
        self.assertIn("PRLHoennCheckPCSpaceForParty", board)
        self.assertIn("VermilionCity_Text_PRLHoennFinalConfirm", board)
        self.assertIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer", board)
        self.assertIn("VermilionCity_EventScript_PRLHoennTransferFailed", board)
        self.assertIn("warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB", board)
        for unsafe in ("\tremoveitem ", "\tsetmoney ", "\tgivemon "):
            self.assertNotIn(unsafe, board)

    def test_preflight_does_not_change_save_data(self):
        check = SOURCE.split("u16 PRLHoennCheckPCSpaceForParty(void)", 1)[1].split("u16 PRLHoennCommitKantoTransfer(void)", 1)[0]
        writes = ("ClearBag(", "ZeroBoxMonAt(", "SetBoxMonAt(", "FlagSet(", "VarSet(",
                  "gSaveBlock1Ptr->money =", "gParties[B_TRAINER_PLAYER][i] =")
        for unsafe in writes:
            self.assertNotIn(unsafe, check)


if __name__ == "__main__":
    unittest.main()
