"""PRR code-entry and PRU-style Evolution UI regression contracts.

These source-level tests do not replace full interactive emulator checks.
"""
from pathlib import Path
import re
import unittest

ROOT=Path(__file__).resolve().parents[2]

def source(path):
    return (ROOT/path).read_text(encoding="utf8")

class PRRQoLTests(unittest.TestCase):
    def test_hgss_dex_is_enabled_with_visible_methods(self):
        c=source("include/config/pokedex_plus_hgss.h")
        self.assertRegex(c,r"(?m)^#define POKEDEX_PLUS_HGSS\s+TRUE")
        self.assertRegex(c,r"(?m)^#define HGSS_HIDE_UNSEEN_EVOLUTION_NAMES\s+FALSE")
        self.assertRegex(c,r"(?m)^#define HGSS_HIDE_UNOWNED_EVOLUTION_METHODS\s+FALSE")
        d=source("src/pokedex_plus_hgss.c")
        self.assertIn("PrintEvolutionTargetSpeciesAndMethod(",d)
        self.assertIn("case EVO_ITEM:",d)
        self.assertIn("case EVO_LEVEL:",d)

    def test_pc_keeps_storage_player_and_hall_services(self):
        c=source("src/script_menu.c")
        s=source("data/scripts/pc.inc")
        for code in ("0, EventScript_AccessPokemonStorage", "1, EventScript_AccessPlayersPC",
                     "2, EventScript_AccessPRRCodes", "3, EventScript_AccessPRRHallOrOff",
                     "4, EventScript_TurnOffPC"):
            self.assertIn(code,s)
        self.assertIn("numChoices = 5",c)
        self.assertIn("numChoices = 4",c)
        self.assertIn("gText_HallOfFame",c)
        self.assertIn("special EnterCode",s)
        self.assertIn("special GetCodeFeedback",s)
        self.assertIn("waitstate",s)

    def test_codes_are_saved_and_reversible(self):
        code=source("src/field_specials.c")
        for token in ('_("DexAll")','_("DexOff")','_("TeamPreview")','_("TeamOff")'):
            self.assertIn(token,code)
        self.assertIn("FlagSet(FLAG_PRR_DEX_ALL)",code)
        self.assertIn("FlagClear(FLAG_PRR_DEX_ALL)",code)
        self.assertIn("FlagSet(FLAG_PRR_TEAM_PREVIEW)",code)
        self.assertIn("FlagClear(FLAG_PRR_TEAM_PREVIEW)",code)
        self.assertNotIn("FLAG_SET_CAUGHT",code[code.index("void GetCodeFeedback(void)"):code.index("void SetHiddenNature(void)")])

    def test_dexall_reveals_only_local_encounters_without_editing_seen_flags(self):
        d=source("src/dexnav.c")
        block=d.split("static bool8 PRR_DexNavCanDisplaySpecies",1)[1].split("// Defines",1)[0]
        self.assertIn("FlagGet(FLAG_PRR_DEX_ALL)",block)
        self.assertIn("FLAG_GET_SEEN",block)
        self.assertNotIn("FLAG_SET_SEEN",block)
        self.assertNotIn("FLAG_SET_CAUGHT",block)
        self.assertGreaterEqual(d.count("PRR_DexNavCanDisplaySpecies("),5)

    def test_battle_preview_has_a_real_ui_and_avoids_multiplayer(self):
        code=source("src/battle_controller_player.c")
        self.assertIn("FlagGet(FLAG_PRR_TEAM_PREVIEW)",code)
        self.assertIn("JOY_NEW(START_BUTTON)",code)
        self.assertIn("gParties[B_TRAINER_OPPONENT_A][slot]",code)
        self.assertIn("GetSpeciesName(species)",code)
        self.assertIn("BattlePutTextOnWindow(gStringVar4, B_WIN_ACTION_PROMPT)",code)
        for restrict in ("BATTLE_TYPE_LINK","BATTLE_TYPE_MULTI","BATTLE_TYPE_TWO_OPPONENTS","BATTLE_TYPE_RECORDED"):
            self.assertIn(restrict,code)
        self.assertIn("HandleInputPRRTeamPreview",code)
        self.assertIn("BattlePutTextOnWindow(gDisplayedStringBattle, B_WIN_ACTION_PROMPT)",code)

    def test_flags_do_not_overlap_prl_story(self):
        f=source("include/constants/flags_frlg.h")
        self.assertIn("#define FLAG_PRR_DEX_ALL                 0x4AE",f)
        self.assertIn("#define FLAG_PRR_TEAM_PREVIEW            0x4AF",f)
        self.assertIn("#define FLAG_PRR_FOREST_MINIBOSS_REWARD  0x4AD",f)
        self.assertIn("FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED",f)
        self.assertNotIn("FLAG_UNUSED_0x4AE",f)
        self.assertNotIn("FLAG_UNUSED_0x4AF",f)

if __name__=="__main__":
    unittest.main()
