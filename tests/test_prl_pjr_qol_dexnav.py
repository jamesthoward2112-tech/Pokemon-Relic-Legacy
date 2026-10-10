"""Source regression checks for the PRL PJR QoL port, specifically Start-menu DexNav."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class TestPjrQolDexnav(unittest.TestCase):
    def test_dexnav_enabled_and_state_is_not_zero(self):
        config = read("include/config/dexnav.h")
        self.assertRegex(config, r"(?m)^#define DEXNAV_ENABLED\\s+TRUE\\b".replace("\\\\", "\\"))
        expected = {
            "DN_FLAG_SEARCHING": "0x8E5",
            "DN_FLAG_DEXNAV_GET": "0x8E6",
            "DN_FLAG_DETECTOR_MODE": "0x8E7",
            "DN_VAR_SPECIES": "0x40DB",
            "DN_VAR_STEP_COUNTER": "0x40DC",
        }
        for name, value in expected.items():
            self.assertRegex(config, rf"(?m)^#define {name}\\s+{value}\\b".replace("\\\\", "\\"))

    def test_start_menu_exposes_dexnav_after_pokedex_without_new_save_flags(self):
        start = read("src/start_menu.c")
        normal = start.split("static void BuildNormalStartMenu(void)\n{", 1)[1].split("\n}\n", 1)[0]
        self.assertIn("if (DEXNAV_ENABLED && FlagGet(FLAG_SYS_POKEDEX_GET))", normal)
        self.assertIn("AddStartMenuAction(MENU_ACTION_DEXNAV)", normal)
        self.assertNotIn("FlagGet(DN_FLAG_DEXNAV_GET)", normal)
        self.assertIn("Task_OpenDexNavFromStartMenu", start)
        self.assertIn("sCurrentStartMenuActions[9]", start)

    def test_no_pjr_item_wheel_ported(self):
        # PHR four-slot wheel is tracked separately and is not silently substituted.
        self.assertNotIn("PJR_QUICK_ITEM_WHEEL", read("src/start_menu.c"))


if __name__ == "__main__":
    unittest.main()
