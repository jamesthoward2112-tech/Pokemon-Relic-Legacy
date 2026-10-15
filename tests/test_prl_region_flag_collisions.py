"""Prevent Kanto/Hoenn event-flag aliasing before regional trainer migration."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
KANTO = ROOT / "include/constants/flags_frlg.h"
SHARED = ROOT / "include/constants/flags.h"

class RegionalFlagIsolation(unittest.TestCase):
    def test_hoenn_progression_does_not_alias_badge_heist(self):
        a = KANTO.read_text()
        b = SHARED.read_text()
        kanto = dict((name, int(value, 16)) for name, value in re.findall(
            r"^#define\s+(FLAG_PRL_BADGE_HEIST_\w+|FLAG_HIDE_PRL_BADGE_HEIST_\w+)\s+FLAG_0x([0-9A-Fa-f]+)", a, re.M))
        hoenn = dict((name, int(value, 16)) for name, value in re.findall(
            r"^#define\s+(FLAG_PRL_HOENN_\w+)\s+0x([0-9A-Fa-f]+)", b, re.M))
        overlaps = [(h, k, hex(v)) for h, v in hoenn.items() for k, w in kanto.items() if v == w]
        self.assertFalse(overlaps, f"Save-corrupting regional flag aliases: {overlaps}")

if __name__ == "__main__":
    unittest.main()
