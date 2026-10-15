"""PR108 must not shift Kanto saves or reintroduce cross-region trainer IDs."""
import json
import re
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text()

class RegionTrainerNamespaceTests(unittest.TestCase):
    def test_locked_region_registry_is_stable(self):
        completed = subprocess.run(
            [sys.executable, str(ROOT / "tools/prl/check_trainer_registry.py"), "--check"],
            cwd=ROOT, capture_output=True, text=True, check=False
        )
        self.assertEqual(completed.returncode, 0, completed.stdout + completed.stderr)
        lock = json.loads(read("tools/prl/trainer_ids.lock.json"))
        self.assertEqual(lock["kanto"]["TRAINER_SWIMMER_FEMALE_MELISSA_2"], 615)
        self.assertEqual(lock["hoenn"]["TRAINER_RICK"], 615)
        self.assertIn("TRAINER_HOENN_RICK (0x1000 + TRAINER_RICK)", read("include/constants/opponents_prl.h"))

    def test_trainer_data_and_flags_are_region_aware(self):
        data = read("src/data.c")
        setup = read("src/battle_setup.c")
        accessor = read("include/data.h")
        difficulty = read("src/difficulty.c")
        self.assertIn("gPrlHoennTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT_EMERALD]", data)
        self.assertIn('#include "data/trainers.h"', data)
        self.assertIn("PRL_IS_HOENN_TRAINER(trainerId)", accessor)
        self.assertIn("gPrlHoennTrainers[difficulty][trainerId - PRL_TRAINER_HOENN_BASE]", accessor)
        self.assertIn("PRL_IS_HOENN_TRAINER(trainerId)", difficulty)
        self.assertIn("PRLGetTrainerFlagId", setup)
        self.assertIn("PRL_HOENN_TRAINER_FLAGS_START + trainerId - PRL_TRAINER_HOENN_BASE", setup)
        self.assertIn("FlagGet(PRLGetTrainerFlagId(trainerId))", setup)
        for key in ("RICK", "CALVIN_1", "TIANA", "ALLEN"):
            self.assertIn("TRAINER_HOENN_" + key, read("data/maps/Route102/scripts.inc"))
        self.assertNotIn("TRAINER_PRL_HOENN_RICK_102", read("data/maps/Route102/scripts.inc"))

    def test_six_hoenn_gift_progress_flags_do_not_overlap_kanto(self):
        flags = read("include/constants/flags.h")
        heist = read("include/constants/flags_frlg.h")
        for name, num in (
            ("FLAG_PRL_HOENN_ESSENTIALS_READY", 0xA000),
            ("FLAG_PRL_HOENN_FUNDS_BANKED", 0xA001),
            ("FLAG_PRL_HOENN_PC_LEVEL_SORTED", 0xA002),
            ("FLAG_PRL_HOENN_ROWAN_TREECKO_GIFT", 0xA003),
            ("FLAG_PRL_HOENN_KIRI_TORCHIC_GIFT", 0xA004),
            ("FLAG_PRL_HOENN_NERIS_MUDKIP_GIFT", 0xA005),
        ):
            self.assertRegex(flags, rf"#define\s+{name}\s+0x{num:04X}\b")
        self.assertIn("FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED       FLAG_0x4E3", heist)

    def test_region_state_is_versioned_and_does_not_resize_legacy_saveblocks(self):
        save = read("src/save.c")
        helper = read("src/prl_region_save.c")
        defs = read("include/prl_region_save.h")
        flags = read("src/event_data.c")
        legacy = read("include/global.h")
        self.assertIn("PrlWriteRegionSaveSector", save)
        self.assertIn("PrlReadRegionSaveSector", save)
        self.assertIn("PrlClearRegionSave", save)
        self.assertIn("PrlMigrateLegacyHoennRoute102Flags", save)
        self.assertIn("SECTOR_ID_PKMN_STORAGE_END", save)
        self.assertIn("PrlRegionSaveFitsPokemonStorageTail", helper)
        self.assertIn("GetPrlRegionSaveChecksum", helper)
        self.assertIn("PRL_REGION_SAVE_MAGIC", defs)
        self.assertIn("return PrlGetRegionFlagPointer(id);", flags)
        self.assertIn("u8 flags[NUM_FLAG_BYTES]", legacy)
        self.assertIn("u16 vars[VARS_COUNT]", legacy)
        self.assertIn("u32 prlInventoryStorage[399]", legacy)

if __name__ == "__main__":
    unittest.main()
