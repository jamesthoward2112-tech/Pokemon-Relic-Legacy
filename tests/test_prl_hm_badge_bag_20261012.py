"""PRL Kanto map HMs use the bag/badge, and party shortcuts check species compatibility."""
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

    def test_virtual_hm_menu_requires_badge_bag_and_species_compatibility(self):
        # Map field actions stay HM-in-bag only; the party menu must not
        # claim every Pokémon is able to Fly or Surf.
        body = read("src/party_menu.c").split(
            "// HM-in-bag and badge still unlock map-based field moves", 1
        )[1].split("if (!InBattlePike())", 1)[0]
        for action in ("FIELD_MOVE_FLASH", "FIELD_MOVE_FLY", "FIELD_MOVE_SURF"):
            self.assertIn(action, body)
        self.assertIn("IsFieldMoveUnlocked(fieldMove)", body)
        self.assertIn("CheckBagHasItem(hmItem, 1)", body)
        self.assertIn("GetSpeciesTeachableLearnset(species)", body)
        self.assertIn("teachableMoves[j] == move", body)
        self.assertIn("if (compatible)", body)
        menu = read("src/party_menu.c").split("static void CursorCb_FieldMove(u8 taskId)\\n{", 1)[1].split(
            "static void DisplayFieldMoveExitAreaMessage", 1
        )[0]
        self.assertIn("!CheckBagHasItem(GetTMHMItemIdFromMoveId(FieldMove_GetMoveId(fieldMove)), 1)", menu)

    def test_map_fly_needs_only_badge_and_hm_in_bag(self):
        src = read("src/field_region_map.c")
        block = src.split("static bool32 PRL_CanFlyDirectlyFromMap(void)",1)[1].split("static const struct BgTemplate",1)[0]
        self.assertIn("IsFieldMoveUnlocked(FIELD_MOVE_FLY)",block)
        self.assertIn("CheckBagHasItem(hm, 1)",block)
        self.assertNotIn("CanLearnTeachableMove",block)
        self.assertNotIn("GetMonData",block)
        self.assertIn("MAP_INPUT_R_BUTTON",src)

    def test_field_strings_credit_player_character(self):
        # {PLAYER} expands to the saved Trainer name; 'Trainer' is not a placeholder.
        files = [read("data/scripts/field_move_scripts.inc"), read("data/text/surf.inc")]
        for name in ("STRENGTH", "WATERFALL", "DIVE", "Rock Climb"):
            self.assertIn("{PLAYER} used " + name, files[0])
        self.assertIn("{PLAYER} used {STR_VAR_2}", files[0])
        self.assertIn("{PLAYER} used SURF", files[1])
        for contents in files:
            self.assertNotIn("Trainer used", contents)

    def test_a_button_water_edge_surf_uses_badge_and_bag(self):
        src = read("src/field_control_avatar.c")
        script = src.split("static const u8 *GetInteractedWaterScript(struct MapPosition *unused1, u8 metatileBehavior, enum Direction direction)\n{", 1)[1].split("static bool32 TrySetupDiveDownScript", 1)[0]
        self.assertIn("IsFieldMoveUnlocked(FIELD_MOVE_SURF)", script)
        self.assertIn("CheckBagHasItem(GetTMHMItemIdFromMoveId(MOVE_SURF), 1)", script)
        self.assertIn("IsPlayerFacingSurfableFishableWater()", script)
        self.assertIn("CheckFollowerNPCFlag(FOLLOWER_NPC_FLAG_CAN_SURF)", script)
        self.assertIn("return EventScript_UseSurf;", script)
        self.assertNotIn("PartyHasMonWithSurf()", script)
        self.assertIn("input->pressedAButton && TryStartInteractionScript(", src)
        self.assertIn("script = GetInteractedWaterScript(", src)

    def test_surf_rock_smash_and_strength_still_use_field_check(self):
        self.assertIn("checkfieldmove FIELD_MOVE_SURF",read("data/scripts/surf.inc"))
        s=read("data/scripts/field_move_scripts.inc")
        self.assertIn("checkfieldmove FIELD_MOVE_ROCK_SMASH, TRUE",s)
        self.assertIn("checkfieldmove FIELD_MOVE_STRENGTH, TRUE",s)
        self.assertIn("checkfieldmove FIELD_MOVE_CUT, TRUE",s)

if __name__ == "__main__":
    unittest.main()
