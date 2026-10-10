"""Source-level guardrails for the Kanto Relic dialogue restoration.

These tests are NOT an emulator playthrough and do not establish event reachability.
"""
from pathlib import Path
import re
import unittest

MAP_DIR = Path("data/maps")
STORY_SCENES = {
    "PalletTown_ProfessorOaksLab_Frlg": ["PalletTown_ProfessorOaksLab_Text_OakCustomBallIOrdered"],
    "PewterCity_Museum_1F_Frlg": ["PewterCity_Museum_1F_Text_AmberContainsGeneticMatter"],
    "MtMoon_B2F_Frlg": ["MtMoon_B2F_Text_PRL_RocketIntro"],
    "CeruleanCity_Frlg": ["CeruleanCity_Text_GruntIntro", "CeruleanCity_Text_OkayIllReturnStolenTM", "CeruleanCity_Text_BetterGetMovingBye"],
    "CeruleanCity_House2_Frlg": ["CeruleanCity_House2_Text_RocketsStoleTMForDig", "CeruleanCity_House2_Text_TeachDiglettDigWithoutTM"],
    "Route25_SeaCottage_Frlg": ["Route25_SeaCottage_Text_CheckOutRareMonsOnPC"],
    "RocketHideout_B4F_Frlg": ["RocketHideout_B4F_Text_GiovanniIntro", "RocketHideout_B4F_Text_PRL_RocketIntro"],
    "SilphCo_7F_Frlg": ["SilphCo_7F_Text_RocketsAfterMasterBall"],
    "SilphCo_11F_Frlg": ["SilphCo_11F_Text_GiovanniIntro", "SilphCo_11F_Text_ThatsOurSecretPrototype"],
    "PokemonMansion_2F_Frlg": ["PokemonMansion_1F_Text_NewMonDiscoveredInGuyanaJungle"],
    "CinnabarIsland_PokemonLab_ResearchRoom_Frlg": ["CinnabarIsland_PokemonLab_ResearchRoom_Text_LegendaryBirdEmail"],
    "ViridianCity_Gym_Frlg": ["ViridianCity_Gym_Text_GiovanniIntro", "ViridianCity_Gym_Text_GiovanniPostBattle"],
    "VictoryRoad_3F_Frlg": ["VictoryRoad_3F_Text_PRL_RocketIntro", "VictoryRoad_3F_Text_PRL_RocketPostBattle"],
    "PokemonLeague_ChampionsRoom_Frlg": ["PokemonLeague_ChampionsRoom_Text_OakCongratulations"],
}
ROCKET_SITES = {
    "MtMoon_B2F_Frlg": "MtMoon_B2F",
    "RocketHideout_B4F_Frlg": "RocketHideout_B4F",
    "SilphCo_11F_Frlg": "SilphCo_11F",
    "VictoryRoad_3F_Frlg": "VictoryRoad_3F",
}


def load(map_name):
    return (MAP_DIR / map_name / "scripts.inc").read_text(encoding="utf-8")


def dialogue(source, label):
    match = re.search(
        rf"^{re.escape(label)}::\s*\n((?:[ \t]*\.string[^\n]*\n)+)",
        source, re.MULTILINE,
    )
    if match is None:
        raise AssertionError(f"Missing dialog: {label}")
    return match.group(1)


class KantoRelicStoryTests(unittest.TestCase):
    def test_core_story_scene_labels_present(self):
        for map_name, labels in STORY_SCENES.items():
            text = load(map_name)
            for label in labels:
                with self.subTest(map=map_name, dialogue=label):
                    self.assertIn('.string', dialogue(text, label))
                    self.assertIn('$', dialogue(text, label))

    def test_unknown_golden_bird_not_named(self):
        for map_name in STORY_SCENES:
            text = load(map_name).upper()
            with self.subTest(map=map_name):
                self.assertNotIn("RELICANTH", text)
                self.assertNotIn("HO-OH", text)
        self.assertIn("GOLD", load("SilphCo_11F_Frlg").upper())
        self.assertIn("UNKNOWN", load("CinnabarIsland_PokemonLab_ResearchRoom_Frlg").upper())

    def test_repeated_rocket_dialogue_is_distinct(self):
        intros = []
        for map_name, prefix in ROCKET_SITES.items():
            src = load(map_name)
            intro = dialogue(src, prefix + "_Text_PRL_RocketIntro")
            defeat = dialogue(src, prefix + "_Text_PRL_RocketDefeat")
            aftermath = dialogue(src, prefix + "_Text_PRL_RocketPostBattle")
            self.assertIn("JESSIE", intro)
            self.assertIn("JAMES", intro)
            self.assertIn("$", defeat)
            self.assertIn("$", aftermath)
            intros.append(intro)
        self.assertEqual(len(intros), len(set(intros)))

    def test_tm28_grunt_explains_relic_excavation_before_item_reward(self):
        city = load("CeruleanCity_Frlg")
        house = load("CeruleanCity_House2_Frlg")
        # Same dialogue label must be reached by direct talks and both coordinate triggers.
        self.assertRegex(
            city,
            r"CeruleanCity_EventScript_GruntDefeated::[\s\S]*?"
            r"msgbox CeruleanCity_Text_OkayIllReturnStolenTM[\s\S]*?"
            r"giveitem_msg CeruleanCity_Text_RecoveredTM28FromGrunt, ITEM_TM28",
        )
        for word in ("TM28 DIG", "TEAM ROCKET uses DIG", "fossil", "RELIC artifacts"):
            self.assertIn(word, dialogue(city, "CeruleanCity_Text_OkayIllReturnStolenTM"))
        self.assertIn(
            "RELIC fragments",
            dialogue(house, "CeruleanCity_House2_Text_TeachDiglettDigWithoutTM"),
        )
        self.assertIn("DIG", dialogue(house, "CeruleanCity_House2_Text_TeachDiglettDigWithoutTM"))

    def test_no_unsafe_text_line_overflow_in_changed_scenes(self):
        for map_name, labels in STORY_SCENES.items():
            src = load(map_name)
            for label in labels:
                block = dialogue(src, label)
                for s in re.findall(r'^\s*\.string "([^"]*)"', block, re.MULTILINE):
                    raw = re.split(r'\\[npl]', s)[0].rstrip("$")
                    # Variables expand at runtime, so check literal line lengths conservatively.
                    if "{" in raw:
                        continue
                    with self.subTest(dialogue=label, text=raw):
                        self.assertLessEqual(len(raw), 34)


if __name__ == "__main__":
    unittest.main()
