"""PRL Kanto HM field use: badge + HM in Bag, no taught or compatible Pokémon needed."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class TestHMBagBadge(unittest.TestCase):
    def test_field_script_requires_badge_and_hm_only(self):
        src = read("src/scrcmd.c")
        body = src.split("bool8 ScrCmd_checkfieldmove(struct ScriptContext *ctx)", 1)[1].split("bool8 ScrCmd_addmoney", 1)[0]
        self.assertIn("(doUnlockedCheck || useFieldHm) && !IsFieldMoveUnlocked(fieldMove)", body)
        self.assertIn("useFieldHm && !CheckBagHasItem(hmItem, 1)", body)
        self.assertIn("GetItemTMHMIndex(hmItem) > NUM_TECHNICAL_MACHINES", body)
        self.assertIn("MON_DATA_IS_EGG", body)
        self.assertIn("if (useFieldHm || MonKnowsMove(", body)
        self.assertNotIn("CanLearnTeachableMove", body)
        self.assertIn("gSpecialVar_Result = PARTY_SIZE;", body)
        self.assertIn("gSpecialVar_Result = i;", body)

    def test_virtual_flash_fly_require_hm_badge_not_learnset(self):
        body = read("src/party_menu.c").split("// Flash and Fly remain optional untaught field actions;",1)[1].split("if (!InBattlePike())",1)[0]
        self.assertIn("FIELD_MOVE_FLASH", body)
        self.assertIn("FIELD_MOVE_FLY", body)
        self.assertIn("IsFieldMoveUnlocked(fieldMove)",body)
        self.assertIn("CheckBagHasItem(hmItem, 1)",body)
        self.assertNotIn("CanLearnTeachableMove",body)
        menu = read("src/party_menu.c").split("static void CursorCb_FieldMove(u8 taskId)\n{",1)[1].split("static void DisplayFieldMoveExitAreaMessage",1)[0]
        self.assertIn("!CheckBagHasItem(GetTMHMItemIdFromMoveId(FieldMove_GetMoveId(fieldMove)), 1)",menu)

    def test_field_strings_credit_trainer(self):
        files = [read("data/scripts/field_move_scripts.inc"),read("data/text/surf.inc")]
        for name in ("STRENGTH", "WATERFALL", "DIVE"):
            self.assertIn("Trainer used "+name, files[0])
        self.assertIn("Trainer used {STR_VAR_2}", files[0])
        self.assertIn("Trainer used SURF", files[1])
        self.assertNotIn("{PLAYER} used",files[0])
        self.assertNotIn("{STR_VAR_1} used SURF",files[1])

    def test_surf_rock_smash_and_strength_still_use_field_check(self):
        self.assertIn("checkfieldmove FIELD_MOVE_SURF",read("data/scripts/surf.inc"))
        s=read("data/scripts/field_move_scripts.inc")
        self.assertIn("checkfieldmove FIELD_MOVE_ROCK_SMASH, TRUE",s)
        self.assertIn("checkfieldmove FIELD_MOVE_STRENGTH, TRUE",s)
        self.assertIn("checkfieldmove FIELD_MOVE_CUT, TRUE",s)

if __name__ == "__main__":
    unittest.main()
