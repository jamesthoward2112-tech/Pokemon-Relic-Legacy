"""Guardrails for the PKR-derived Kanto Relic dialogue pass.
Source checks, NOT emulator reachability proof.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1] / "data" / "maps"
SCENES = {
    "RocketHideout_B1F_Frlg": "Grunt1Intro Grunt1PostBattle Grunt2PostBattle Grunt3Intro Grunt4PostBattle",
    "RocketHideout_B2F_Frlg": "GruntIntro GruntPostBattle",
    "RocketHideout_B3F_Frlg": "Grunt2Intro Grunt2PostBattle",
    "RocketHideout_B4F_Frlg": "GiovanniIntro GiovanniPostBattle Grunt2PostBattle Grunt3PostBattle",
    "SilphCo_2F_Frlg": "ConnorPostBattle Grunt1PostBattle Grunt2Intro Grunt2PostBattle",
    "SilphCo_3F_Frlg": "GruntIntro GruntPostBattle JosePostBattle",
    "SilphCo_4F_Frlg": "Grunt1PostBattle Grunt2Intro Grunt2PostBattle",
    "SilphCo_5F_Frlg": "Grunt1Intro Grunt1PostBattle BeauPostBattle Grunt2PostBattle",
    "SilphCo_6F_Frlg": "Grunt1Intro Grunt1PostBattle Grunt2PostBattle TargetedSilphForOurMonProducts",
    "SilphCo_7F_Frlg": "Grunt1PostBattle Grunt2PostBattle",
    "SilphCo_8F_Frlg": "Grunt1Intro Grunt1PostBattle Grunt2Intro",
    "SilphCo_9F_Frlg": "Grunt1Intro Grunt2PostBattle",
    "SilphCo_10F_Frlg": "GruntIntro GruntPostBattle",
    "SilphCo_11F_Frlg": "Grunt2PostBattle Grunt1Intro Grunt1PostBattle ThanksForSavingMeDearBoy ThanksForSavingMeDearGirl",
    "PalletTown_ProfessorOaksLab_Frlg": "OakComeSeeMeSometime PRLAideResearch",
    "PewterCity_Museum_2F_Frlg": "WhatsSpecialAboutMoonStone MeteoriteThatFellOnMtMoon",
    "Route25_SeaCottage_Frlg": "ThanksBudTakeThis ThanksLadyTakeThis CheckOutRareMonsOnPC",
    "Route10_Frlg": "HeidiPostBattle MarkPostBattle",
    "RockTunnel_1F_Frlg": "OliverPostBattle LucasPostBattle",
    "RockTunnel_B1F_Frlg": "AllenPostBattle CooperPostBattle",
    "LavenderTown_Frlg": "GhostsAppearedInTower",
    "PokemonTower_7F_Frlg": "Grunt1Intro Grunt2PostBattle",
    "LavenderTown_VolunteerPokemonHouse_Frlg": "MrFujiWasPrayingForCubonesMother HasPokeFluteHelpedYou",
    "FuchsiaCity_Frlg": "OmanyteSign KabutoSign",
    "FuchsiaCity_Gym_Frlg": "KogaPostBattle",
    "FuchsiaCity_SafariZone_Office_Frlg": "WardenIsVeryKnowledgeable",
    "SafariZone_SecretHouse_Frlg": "CongratsYouveWon",
    "CinnabarIsland_Frlg": "ScientistsExperimentInMansion",
    "CinnabarIsland_PokemonLab_Entrance_Frlg": "StudyMonsExtensively PhotoOfLabFounderDrFuji",
    "CinnabarIsland_PokemonLab_Lounge_Frlg": "FoundFossilInMtMoon",
    "CinnabarIsland_PokemonLab_ResearchRoom_Frlg": "EeveeCanEvolveIntroThreeMons",
    "CinnabarIsland_PokemonLab_ExperimentRoom_Frlg": "HaveYouAFossilForMe FossilMonBackToLife ThatFossilIsOfMonMakeItLiveAgain YouComeAgain",
    "PokemonMansion_2F_Frlg": "NewMonDiscoveredInGuyanaJungle ChristenedDiscoveredMonMew",
    "PokemonMansion_3F_Frlg": "MewGaveBirthToMewtwo",
    "PokemonMansion_B1F_Frlg": "MewtwoIsFarTooPowerful IvanPostBattle",
    "Route20_Frlg": "ShirleyPostBattle IrenePostBattle MelissaPostBattle",
    "Route19_Frlg": "ConniePostBattle",
    "ViridianCity_Gym_Frlg": "YujiPostBattle WarrenPostBattle SamuelPostBattle",
    "VictoryRoad_1F_Frlg": "NaomiPostBattle",
    "VictoryRoad_2F_Frlg": "DawsonPostBattle NelsonPostBattle",
    "VictoryRoad_3F_Frlg": "GeorgePostBattle ColbyPostBattle",
    "PokemonLeague_ChampionsRoom_Frlg": "PostBattle",
    "CeruleanCity_Frlg": "GruntIntro GruntDefeat OkayIllReturnStolenTM BetterGetMovingBye PeopleHereWereRobbed",
    "CeruleanCity_House2_Frlg": "RocketsStoleTMForDig TeachDiglettDigWithoutTM"
}

def source(map_name):
    return (ROOT / map_name / "scripts.inc").read_text(encoding="utf-8")

def speech(map_name, suffix):
    src = source(map_name)
    regex = r"^([A-Za-z0-9_]+_Text_" + re.escape(suffix) + r")::\n((?:[ \t]*\\.string[^\n]*\n)+)"
    hit = re.search(regex, src, flags=re.MULTILINE)
    if hit is None:
        raise AssertionError("Missing story dialogue: " + map_name + "/" + suffix)
    return hit.group(2)

class PKRStoryMigrationTests(unittest.TestCase):
    def test_all_story_blocks_are_gba_safe(self):
        for name, suffixes in SCENES.items():
            for suffix in suffixes.split():
                with self.subTest(map=name, dialogue=suffix):
                    block = speech(name, suffix)
                    self.assertTrue(block.strip().endswith('"$'))
                    for text in re.findall(r'^[ \t]*\\.string "([^"]*)"', block, re.MULTILINE):
                        self.assertLessEqual(len(text.replace(r"\n", "").replace(r"\p", "").rstrip("$")), 34)
                        self.assertTrue(text.endswith((r"\n", r"\p", "$")))

    def test_no_retired_legend_named_in_restored_scenes(self):
        for name, suffixes in SCENES.items():
            for suffix in suffixes.split():
                dialog = speech(name, suffix).upper()
                self.assertNotIn("RELICANTH", dialog)
                self.assertNotIn("HO-OH", dialog)

    def test_rocket_dig_and_bills_transporter_are_story_events(self):
        cerulean = source("CeruleanCity_Frlg")
        self.assertIn("msgbox CeruleanCity_Text_OkayIllReturnStolenTM", cerulean)
        self.assertIn("giveitem_msg CeruleanCity_Text_RecoveredTM28FromGrunt, ITEM_TM28", cerulean)
        self.assertIn("TEAM ROCKET uses DIG", speech("CeruleanCity_Frlg", "OkayIllReturnStolenTM"))
        bill = source("Route25_SeaCottage_Frlg")
        for suffix in ("ThanksBudTakeThis", "ThanksLadyTakeThis"):
            self.assertIn("Route25_SeaCottage_Text_" + suffix, bill)
            self.assertIn("ancient RELIC energy", speech("Route25_SeaCottage_Frlg", suffix))

    def test_rocket_silph_cinnabar_giovanni_and_league_progression(self):
        self.assertIn("SILPH", speech("RocketHideout_B4F_Frlg", "GiovanniIntro"))
        self.assertIn("MASTER BALL", speech("SilphCo_11F_Frlg", "ThanksForSavingMeDearBoy"))
        self.assertIn("golden bird", speech("SilphCo_11F_Frlg", "ThanksForSavingMeDearGirl"))
        self.assertIn("RELIC-01", speech("PokemonMansion_2F_Frlg", "ChristenedDiscoveredMonMew"))
        self.assertIn("RELIC-01", speech("PokemonMansion_B1F_Frlg", "MewtwoIsFarTooPowerful"))
        self.assertIn("RELIC", speech("ViridianCity_Gym_Frlg", "GiovanniPostBattle"))
        self.assertIn("RELIC REPORTS", speech("PokemonLeague_ChampionsRoom_Frlg", "PostBattle"))

if __name__ == "__main__":
    unittest.main()
