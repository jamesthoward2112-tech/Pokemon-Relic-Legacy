"""Persistent four-slot move reordering regression gates (battle & Summary).

Source-level assertions verify that the real GBA code path is compiled in.
An emulator test must still exercise the input sequence and save persistence.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
BATTLE = ROOT / "src/battle_controller_player.c"
CONFIG = ROOT / "include/config/battle.h"
SUMMARY = ROOT / "src/pokemon_summary_screen.c"

class MoveReorderingRegression(unittest.TestCase):
    def test_gen3_select_battle_move_swap_is_enabled(self):
        config = CONFIG.read_text()
        self.assertRegex(config, r"(?m)^#define B_MOVE_REARRANGEMENT_IN_BATTLE\s+GEN_3\b")
        battle = BATTLE.read_text()
        self.assertIn("B_MOVE_REARRANGEMENT_IN_BATTLE < GEN_4 && JOY_NEW(SELECT_BUTTON)",battle)
        self.assertIn("gBattlerControllerFuncs[battler] = HandleMoveSwitching;",battle)
        self.assertIn("gNumberOfMovesToChoose > 1",battle)
        self.assertIn("!(gBattleTypeFlags & BATTLE_TYPE_LINK)",battle)

    def test_swapping_persists_move_pp_and_pp_up_fields_to_party(self):
        battle = BATTLE.read_text()
        start = battle.index("void HandleMoveSwitching(enum BattlerId battler)\n{")
        end = battle.index("\n}", start) + 2
        body = battle[start:end]
        for token in (
            "JOY_NEW(A_BUTTON | SELECT_BUTTON)",
            "moveInfo->moves[gMoveSelectionCursor[battler]]",
            "moveInfo->currentPP[gMoveSelectionCursor[battler]]",
            "moveInfo->maxPP[gMoveSelectionCursor[battler]]",
            "gBattleMons[battler].moves[i] = moveInfo->moves[i]",
            "gBattleMons[battler].pp[i] = moveInfo->currentPP[i]",
            "SetMonData(GetBattlerMon(battler), MON_DATA_MOVE1 + i",
            "SetMonData(GetBattlerMon(battler), MON_DATA_PP1 + i",
            "SetMonData(GetBattlerMon(battler), MON_DATA_PP_BONUSES",
            "if (!(gBattleMons[battler].volatiles.transformed))",
            "JOY_NEW(B_BUTTON | SELECT_BUTTON)",
        ):
            self.assertIn(token,body)

    def test_summary_supports_A_and_SELECT_without_unlocking_battle_summary(self):
        s = SUMMARY.read_text()
        self.assertIn("JOY_NEW(A_BUTTON) || (JOY_NEW(SELECT_BUTTON) && IS_MOVE_PAGE(",s)
        self.assertIn("JOY_NEW(A_BUTTON | SELECT_BUTTON)",s)
        self.assertIn("SwitchToMoveSelection(taskId);",s)
        self.assertIn("SwitchToMovePositionSwitchMode(taskId);",s)
        self.assertIn("SwapMonMoves(&mon[sMonSummaryScreen->curMonIndex]",s)
        self.assertIn("SwapBoxMonMoves(&boxMon[sMonSummaryScreen->curMonIndex]",s)
        self.assertIn("case SUMMARY_MODE_LOCK_MOVES:",s)
        self.assertIn("sMonSummaryScreen->lockMovesFlag = TRUE;",s)
        self.assertIn("if (sMonSummaryScreen->lockMovesFlag == TRUE",s)

    def test_overworld_pjr_select_is_not_replaced(self):
        pjr = (ROOT / "tests/test_prl_pjr_registered_items.py").read_text()
        self.assertIn("SELECT", pjr)
        cfg = CONFIG.read_text()
        self.assertIn("#define B_MOVE_REARRANGEMENT_IN_BATTLE", cfg)

if __name__=="__main__":
    unittest.main()
