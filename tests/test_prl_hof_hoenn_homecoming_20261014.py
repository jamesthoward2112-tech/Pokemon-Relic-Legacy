"""PKR 2.0 Kanto-only Champion regression checks.

PRL's earlier Hoenn departure expectations are intentionally superseded on
this separate PKR2 branch. The original PRL branches and tests are untouched.
"""
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class PKR2KantoChampionTests(unittest.TestCase):
    def test_credits_keep_short_safe_reset_and_no_hoenn_teaser(self):
        s = read("src/credits_frlg.c")
        script = s.split("static const struct CreditsScrcmd sCreditsScript[] = {", 1)[1].split("};", 1)[0]
        self.assertLessEqual(script.count("CREDITS_PRINT("), 4)
        self.assertIn("CREDITS_WAITBUTTON(", script)
        self.assertNotIn("CREDITS_THEENDGFX(", script)
        self.assertNotIn("CREDITS_MON(", script)
        self.assertIn('_("The Journey Continues...")', s)
        self.assertIn('_("In Kanto!")', s)
        self.assertNotIn('_("In Hoenn!")', s)
        self.assertIn("SoftReset(RESET_ALL);", s)

    def test_mum_welcomes_champion_once_then_can_heal(self):
        s = read("data/maps/PalletTown_PlayersHouse_1F_Frlg/scripts.inc")
        self.assertIn("PRLHasKantoHallOfFameRecord", s)
        self.assertIn("PalletTown_PlayersHouse_1F_EventScript_PRLHomecoming::", s)
        self.assertIn("setflag FLAG_PRL_POSTHOF_MOM_WELCOME", s)
        section = s.split("PalletTown_PlayersHouse_1F_EventScript_PRLMomPostLeague::", 1)[1].split("PalletTown_PlayersHouse_1F_EventScript_MomHeal::", 1)[0]
        self.assertIn("goto PalletTown_PlayersHouse_1F_EventScript_MomHeal", section)
        self.assertNotIn("setmoney", section)
        self.assertNotIn("removeitem", section)
        self.assertNotIn("PRLDeparturePrep", section)

    def test_oak_continues_kanto_and_unlocks_national_dex(self):
        s = read("data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc")
        entry = s.split("PalletTown_ProfessorOaksLab_EventScript_ProfOak::", 1)[1].split("PalletTown_ProfessorOaksLab_EventScript_PRLHoennInvitationCheck::", 1)[0]
        self.assertIn("PRLHasKantoHallOfFameRecord", entry)
        self.assertIn("goto_if_eq VAR_RESULT, TRUE, PalletTown_ProfessorOaksLab_EventScript_PKR2ChampionResearch", entry)
        self.assertIn("PalletTown_ProfessorOaksLab_EventScript_PKR2ChampionResearch::", entry)
        self.assertIn("special EnableNationalPokedex", entry)
        self.assertNotIn("giveitem_msg", entry)
        self.assertIn("SEVII ISLANDS", entry)

    def test_ferry_does_not_offer_hoenn_after_champion(self):
        s = read("data/maps/VermilionCity_Frlg/scripts.inc")
        ferry = s.split("VermilionCity_EventScript_FerrySailor::", 1)[1].split("VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[0]
        self.assertNotIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET", ferry)
        self.assertIn("VermilionCity_EventScript_CheckSeagallopPresent", ferry)
        self.assertNotIn("VAR_PRL_REGION_STAGE", ferry)

    def test_hall_of_fame_and_champion_remain(self):
        hof = read("data/maps/PokemonLeague_HallOfFame_Frlg/scripts.inc")
        self.assertIn("special EnterHallOfFame", hof)
        self.assertIn("EventScript_SetDefeatedEliteFourFlagsVars", hof)
        champ = read("data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc")
        for species in ("SQUIRTLE", "BULBASAUR", "CHARMANDER"):
            self.assertIn("trainerbattle_no_intro TRAINER_CHAMPION_FIRST_" + species, champ)

    def test_hof_detection_still_uses_saved_stat(self):
        s = read("src/field_specials.c")
        self.assertIn("u16 PRLHasKantoHallOfFameRecord(void)", s)
        self.assertIn("GetGameStat(GAME_STAT_ENTERED_HOF) != 0", s)


if __name__ == "__main__":
    unittest.main()
