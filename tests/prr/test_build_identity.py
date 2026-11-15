import os
import unittest
from pathlib import Path

from tools.prr.verify_gba import verify_rom


@unittest.skipUnless(
    os.environ.get("PRR_ROM_UNDER_TEST"),
    "set PRR_ROM_UNDER_TEST to verify a compiled PRR ROM",
)
class BuildIdentityTests(unittest.TestCase):
    def test_compiled_rom_has_prr_identity(self):
        result = verify_rom(Path(os.environ["PRR_ROM_UNDER_TEST"]))

        self.assertEqual(result["size"], 32 * 1024 * 1024)
        self.assertEqual(result["title"], "RADICAL RELC")
        self.assertEqual(result["game_code"], "BPRR")
        self.assertEqual(result["maker_code"], "01")
        self.assertEqual(result["version"], 0)
        self.assertTrue(result["header_checksum_valid"])


if __name__ == "__main__":
    unittest.main()
