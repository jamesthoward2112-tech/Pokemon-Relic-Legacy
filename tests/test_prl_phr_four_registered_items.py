"""Regression coverage for PHR's original four-Key-Item registered SELECT menu."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class TestPhrFourRegisteredKeyItems(unittest.TestCase):
    def test_donor_has_four_slots_and_preserves_existing_save_field(self):
        s = (ROOT / "src/item_menu.c").read_text()
        self.assertIn("#define PHR_REGISTERED_ITEM_SLOTS 4", s)
        self.assertIn("return gSaveBlock1Ptr->registeredItem;", s)
        for n in (2, 3, 4):
            self.assertIn(f"VAR_PHR_REGISTERED_ITEM_{n}", s)
        self.assertIn("bool8 IsItemRegistered(enum Item itemId)", s)
        self.assertIn("static bool8 ToggleRegisteredItem(enum Item itemId)", s)
        self.assertIn("OpenRegisteredItemQuickMenu(count);", s)
        self.assertIn("CreateTask(Task_RegisteredItemQuickMenu, 8);", s)
        self.assertIn("if (IsItemRegistered(gSpecialVar_ItemId))", s)
        self.assertIn("if (!ToggleRegisteredItem(gSpecialVar_ItemId))", s)
        self.assertNotIn("gSaveBlock1Ptr->registeredItem = gSpecialVar_ItemId;", s)

    def test_registered_bike_swap_updates_whichever_slot(self):
        s = (ROOT / "src/item.c").read_text()
        self.assertIn("if (IsItemRegistered(ITEM_MACH_BIKE))", s)
        self.assertIn("ReplaceRegisteredItem(ITEM_ACRO_BIKE, ITEM_MACH_BIKE)", s)
        constants = (ROOT / "include/constants/vars.h").read_text()
        for n, loc in ((2,"0x40FD"), (3,"0x40FE"), (4,"0x40FF")):
            self.assertIn(f"VAR_PHR_REGISTERED_ITEM_{n}", constants)
            self.assertIn(loc, constants)

    def test_dexnav_remains_in_start_menu_instead_of_phr_key_item(self):
        s = (ROOT / "src/start_menu.c").read_text()
        self.assertIn("AddStartMenuAction(MENU_ACTION_DEXNAV);", s)
        config = (ROOT / "include/config/dexnav.h").read_text()
        self.assertIn("#define DEXNAV_ENABLED                TRUE", config)

if __name__ == "__main__":
    unittest.main()
