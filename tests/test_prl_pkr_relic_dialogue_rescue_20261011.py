"""PKR-to-PRL continuity regression gates for the Celadon/Rocket/Silph story.
Existing gameplay, trainers, saves and map event dispatch remain unchanged.
These source tests are not a substitute for emulator dialogue playthroughs.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1] / "data" / "maps"

RESTORED = {
    "CeladonCity_GameCorner_Frlg": (
        "RumoredTeamRocketRunsThisPlace", "ThinkMachinesHaveDifferentOdds",
        "RareMonsForCoins", "GruntIntro", "GruntPostBattle",
    ),
    "CeladonCity_GameCorner_PrizeRoom_Frlg": (
        "FancyThatPorygon", "RakedItInToday",
    ),
    "CeladonCity_Frlg": (
        "GameCornerIsBadForCitysImage", "KeepOutOfTeamRocketsWay",
        "SomeoneStoleSilphScope",
    ),
    "RocketHideout_B1F_Frlg": (
        "Grunt3PostBattle", "Grunt4Intro", "Grunt5Intro", "Grunt5PostBattle",
    ),
    "RocketHideout_B2F_Frlg": ("GruntPostBattle",),
    "RocketHideout_B3F_Frlg": ("Grunt1PostBattle", "Grunt2Intro"),
    "RocketHideout_B4F_Frlg": (
        "GiovanniIntro", "GiovanniPostBattle", "Grunt2PostBattle",
        "Grunt3PostBattle", "PRL_JessiePostBattle", "PRL_JamesPostBattle",
        "PRL_RocketIntro", "PRL_RocketPostBattle",
    ),
    "SilphCo_2F_Frlg": ("JerryIntro", "JerryPostBattle"),
    "SilphCo_3F_Frlg": ("WhatAmIToDo", "YouAndYourMonsSavedUs"),
    "SilphCo_4F_Frlg": ("RodneyIntro", "RodneyPostBattle", "CantYouSeeImHiding"),
    "SilphCo_5F_Frlg": ("DaltonIntro", "DaltonPostBattle", "Grunt2Intro"),
    "SilphCo_6F_Frlg": (
        "RocketsTryingToConquerWorld", "RocketsTookOverBuilding",
        "ComeWorkForSilphWhenYoureOlder",
    ),
    "SilphCo_7F_Frlg": (
        "Grunt3Intro", "Grunt3PostBattle", "Grunt2Intro",
        "RocketsAfterMasterBall",
    ),
    "SilphCo_8F_Frlg": (
        "ParkerIntro", "ParkerPostBattle", "WonderIfSilphIsFinished",
    ),
    "SilphCo_9F_Frlg": ("Grunt2Intro", "DontGiveUp"),
    "SilphCo_10F_Frlg": ("TravisIntro", "TravisPostBattle"),
    "SilphCo_11F_Frlg": (
        "Grunt2Intro", "ThanksForRescuingUs",
        "PRL_JamesPostBattle", "PRL_JessiePostBattle",
    ),
}

class PRLRelicStoryRescue(unittest.TestCase):
    def test_reachable_relic_story_texts_are_valid(self):
        self.assertEqual(sum(map(len, RESTORED.values())), 53)
        for name, labels in RESTORED.items():
            source = (ROOT / name / "scripts.inc").read_text(encoding="utf-8")
            for suffix in labels:
                with self.subTest(map=name, label=suffix):
                    label = name.removesuffix("_Frlg") + "_Text_" + suffix
                    pattern = rf"^{re.escape(label)}::\n((?:[ \t]*\.string[^\n]*\n)+)"
                    match = re.search(pattern, source, re.MULTILINE)
                    self.assertIsNotNone(match, label)
                    if match is None:
                        continue
                    strings = re.findall(r'^\s*\.string "([^"]*)"', match.group(1), re.MULTILINE)
                    self.assertTrue(strings)
                    self.assertTrue(strings[-1].endswith("$"), label)
                    for sentence in strings:
                        self.assertTrue(sentence.endswith((r"\n", r"\p", "$")), label)
                        self.assertLessEqual(len(sentence.replace(r"\n", "").replace(r"\p", "").rstrip("$")), 34, label)
                    story = match.group(1).upper()
                    self.assertNotIn("RELICANTH", story)
                    self.assertNotIn("HO-OH", story)
                    self.assertGreater(source.count(label), 1, "Dialogue must be referenced by a live script: " + label)

    def test_hideout_preserves_rocket_events_and_silph_bridge(self):
        hideout = (ROOT / "RocketHideout_B4F_Frlg" / "scripts.inc").read_text()
        for command in (
            "TRAINER_BOSS_GIOVANNI", "TRAINER_JESSIE_JAMES_HIDEOUT",
            "ITEM_SILPH_SCOPE", "ITEM_LIFT_KEY",
            "FLAG_HIDE_CELADON_ROCKETS",
            "FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT",
        ):
            self.assertIn(command, hideout)
        self.assertIn("a bird outlined in gold", hideout)
        self.assertIn("SILPH CO.", hideout)

    def test_golden_bird_is_a_mystery_and_relic_01_remains_for_cinnabar(self):
        silph = (ROOT / "SilphCo_11F_Frlg" / "scripts.inc").read_text()
        self.assertIn("No one knows its name.", silph)
        mansion = (ROOT / "PokemonMansion_2F_Frlg" / "scripts.inc").read_text()
        self.assertIn("RELIC-01", mansion)
        self.assertIn("fossil", (ROOT / "RocketHideout_B1F_Frlg" / "scripts.inc").read_text().lower())

if __name__ == "__main__":
    unittest.main()
