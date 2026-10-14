"""Kanto Hall of Fame must lead into the Hoenn story without a dead/red end.

These checks protect vanilla-save compatibility and the explicit user's
postgame requirement to roam Kanto before any future Hoenn ferry trip.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(p):
    return (ROOT / p).read_text(encoding="utf-8")


class PRLHallOfFameHomecoming(unittest.TestCase):
    def test_short_credits_are_hoenn_teaser_not_original_staff_roll(self):
        credits = read("src/credits_frlg.c")
        script = credits.split("static const struct CreditsScrcmd sCreditsScript[] = {", 1)[1].split("};", 1)[0]
        self.assertLessEqual(script.count("CREDITS_PRINT("), 4)
        self.assertIn("CREDITS_PRINT(PRL_HOENN_TEASER,", script)
        self.assertIn("The Journey Continues...", credits)
        self.assertIn("In Hoenn!", credits)
        self.assertNotIn("CREDITS_THEENDGFX(", script)
        self.assertNotIn("CREDITS_MON(", script)
        self.assertIn("CREDITS_WAITBUTTON(", script)
        self.assertNotIn("SoftReset(RESET_ALL);", credits.split("static void CB2_Credits(void)\n{", 1)[1].split("static void SwitchWin1OffWin0On", 1)[0])
        self.assertIn("SetMainCallback2(CB2_LoadMap)", credits)
        self.assertIn("MAP_PALLET_TOWN_PLAYERS_HOUSE_1F", credits)
        self.assertIn("ResetInitialPlayerAvatarState()", credits)

    def test_mum_homecoming_happens_once_even_for_loaded_old_save(self):
        script = read("data/maps/PalletTown_PlayersHouse_1F_Frlg/scripts.inc")
        self.assertIn("MAP_SCRIPT_ON_FRAME_TABLE", script)
        self.assertIn("PalletTown_PlayersHouse_1F_EventScript_PRLHomecoming::", script)
        self.assertIn("goto_if_unset FLAG_SYS_GAME_CLEAR", script)
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_MOM_WELCOME", script)
        self.assertIn("setflag FLAG_PRL_POSTHOF_MOM_WELCOME", script)
        self.assertIn("PROF. OAK in his LAB.", script)
        self.assertIn("called HOENN!", script)
        self.assertIn("PalletTown_PlayersHouse_1F_EventScript_MomHeal::", script)

    def test_oak_one_time_ticket_with_existing_ticket_and_bag_full_handling(self):
        s = read("data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc")
        start = s.split("PalletTown_ProfessorOaksLab_EventScript_PRLHoennInvitationCheck::", 1)[1].split("PalletTown_ProfessorOaksLab_EventScript_OakJustShownCompleteDex::", 1)[0]
        self.assertIn("FLAG_PRL_POSTHOF_MOM_WELCOME", s)
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET", start)
        self.assertIn("checkitem ITEM_SS_TICKET", start)
        self.assertIn("checkitemspace ITEM_SS_TICKET", start)
        self.assertIn("giveitem_msg", start)
        self.assertIn("setflag FLAG_PRL_POSTHOF_OAK_TICKET", start)
        self.assertIn("TryStartNationalDexScene", start)
        self.assertIn("one way for now", s)
        self.assertIn("Find MEWTWO", s)
        self.assertIn("SEVII ISLANDS", s)
        self.assertNotIn("warp MAP_HOENN", s)

    def test_story_flags_stay_in_reserved_persistent_range(self):
        src = read("include/constants/flags.h")
        self.assertRegex(src, r"#define FLAG_PRL_POSTHOF_MOM_WELCOME\s+0x4D9\b")
        self.assertRegex(src, r"#define FLAG_PRL_POSTHOF_OAK_TICKET\s+0x4DA\b")
        self.assertIn("#define FLAG_PRL_ERIKA_BIG_ROOT_CORRECTION               0x4D8", src)

    def test_hall_of_fame_save_and_champion_remain_untouched(self):
        hof = read("data/maps/PokemonLeague_HallOfFame_Frlg/scripts.inc")
        self.assertIn("special EnterHallOfFame", hof)
        self.assertIn("EventScript_SetDefeatedEliteFourFlagsVars", hof)
        champ = read("data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc")
        for starter in ("SQUIRTLE", "BULBASAUR", "CHARMANDER"):
            self.assertIn(f"trainerbattle_no_intro TRAINER_CHAMPION_FIRST_{starter}", champ)


if __name__ == "__main__":
    unittest.main()
