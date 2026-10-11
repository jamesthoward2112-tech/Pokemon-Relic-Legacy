"""Static regressions for PJR-style tap/hold registration in PRL.

These tests check integration wiring, not emulator behaviour.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class TestPjrRegisteredItems(unittest.TestCase):
    def test_no_failed_four_slot_selector(self):
        item = (ROOT / "src/item_menu.c").read_text()
        self.assertNotIn("PHR_REGISTERED_ITEM_SLOTS", item)
        self.assertNotIn("OpenRegisteredItemQuickMenu", item)
        self.assertIn("sRegisterOptions", item)
        self.assertIn("ACTION_REGISTER_HOLD", item)
        self.assertIn("ACTION_REGISTER_TAP", item)

    def test_tap_and_hold_dispatch(self):
        field = (ROOT / "src/field_control_avatar.c").read_text()
        self.assertIn("sPlayerSelectHoldFrames == 60", field)
        self.assertIn("UseRegisteredKeyItemOnField(FALSE)", field)
        self.assertIn("UseRegisteredKeyItemOnField(TRUE)", field)
        item = (ROOT / "src/item_menu.c").read_text()
        self.assertIn("CreateTask(GetItemFieldFunc(item), 8)", item)
        self.assertIn("GetRegisteredItemHold()", item)

    def test_existing_save_field_and_legacy_var_used(self):
        item = (ROOT / "src/item_menu.c").read_text()
        self.assertIn("gSaveBlock1Ptr->registeredItem", item)
        self.assertIn("VarGet(VAR_PRL_REGISTERED_ITEM_HOLD)", item)
        self.assertIn("VarSet(VAR_PRL_REGISTERED_ITEM_HOLD, itemId)", item)
        self.assertIn("ReplaceRegisteredItem(enum Item oldItem, enum Item newItem)", item)
        constants = (ROOT / "include/constants/vars.h").read_text()
        self.assertIn("#define VAR_PRL_REGISTERED_ITEM_HOLD", constants)
        self.assertIn("VAR_UNUSED_0x40FD", constants)

    def test_protected_prl_qol_stays_in_place(self):
        s = (ROOT / "src/start_menu.c").read_text()
        self.assertIn("AddStartMenuAction(MENU_ACTION_DEXNAV);", s)
        config = (ROOT / "include/config/dexnav.h").read_text()
        self.assertIn("#define DEXNAV_ENABLED                TRUE", config)
        relearner = (ROOT / "src/move_relearner.c").read_text()
        self.assertIn("Task_MoveRelearner_HandleInput", relearner)
        self.assertIn("PRL_IsExtraRelearnerMoveUnlocked", relearner)

if __name__ == "__main__":
    unittest.main()
