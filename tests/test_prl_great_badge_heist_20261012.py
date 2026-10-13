"""Regression contract for PRL's Great Badge Heist storyline."""

from __future__ import annotations

import hashlib
import json
import re
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


RECOVERY = {
    "BROCK": ("PewterCity_Gym_Frlg", "BOULDER"),
    "MISTY": ("CeruleanCity_Gym_Frlg", "CASCADE"),
    "LT_SURGE": ("VermilionCity_Gym_Frlg", "THUNDER"),
    "ERIKA": ("CeladonCity_Gym_Frlg", "RAINBOW"),
    "KOGA": ("FuchsiaCity_Gym_Frlg", "SOUL"),
    "SABRINA": ("SaffronCity_Gym_Frlg", "MARSH"),
    "BLAINE": ("CinnabarIsland_Gym_Frlg", "VOLCANO"),
    "GIOVANNI": ("ViridianCity_Gym_Frlg", "EARTH"),
}


class GreatBadgeHeistTests(unittest.TestCase):
    def test_dedicated_persistent_flags_do_not_replace_real_badges(self):
        flags = text("include/constants/flags_frlg.h")
        expected = [
            "FLAG_PRL_BADGE_HEIST_STARTED",
            *(f"FLAG_PRL_BADGE_HEIST_RECOVERED_{badge}" for _, badge in RECOVERY.values()),
            "FLAG_PRL_BADGE_HEIST_ALL_RECOVERED",
            "FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED",
            "FLAG_HIDE_PRL_BADGE_HEIST_TRIO",
            "FLAG_HIDE_PRL_BADGE_HEIST_BALLOON",
        ]
        values = []
        for name in expected:
            match = re.search(rf"^#define {name}\s+FLAG_(0x[0-9A-F]+)$", flags, re.M)
            self.assertIsNotNone(match, name)
            values.append(int(match.group(1), 16))
        self.assertEqual(len(values), len(set(values)))
        self.assertTrue(all(0x4D5 <= value <= 0x4E6 for value in values))

        route = text("data/scripts/route23.inc")
        for number in range(1, 9):
            self.assertNotIn(f"clearflag FLAG_BADGE0{number}_GET", route)
        field_move = text("src/field_move.c")
        self.assertIn("FLAG_BADGE01_GET", field_move)
        self.assertNotIn("FLAG_PRL_BADGE_HEIST", field_move)

    def test_route23_contains_one_time_heist_and_two_balloon_escapes(self):
        script = text("data/maps/Route23_Frlg/scripts.inc")
        map_json = text("data/maps/Route23_Frlg/map.json")
        for actor in ("JESSIE", "JAMES", "MEOWTH", "BALLOON"):
            self.assertIn(f"LOCALID_PRL_HEIST_{actor}", map_json)
        self.assertIn("OBJ_EVENT_GFX_PRL_MEOWTH_BALLOON", map_json)
        self.assertIn("Route23_EventScript_PRLGreatBadgeHeist", script)
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_STARTED", script)
        self.assertIn("Route23_EventScript_PRLFinalRocketBattle", script)
        self.assertIn("TRAINER_JESSIE_JAMES_VICTORY_ROAD", script)
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", script)
        self.assertGreaterEqual(script.count("Route23_Movement_PRLBalloonTakeoff"), 2)
        self.assertIn("JESSIE: TEAM ROCKET", script)
        self.assertIn("defeat all eight GYM LEADERS", script)

    def test_heist_trigger_uses_surface_elevation_and_temp_scene_latch(self):
        route_map = json.loads(text("data/maps/Route23_Frlg/map.json"))
        triggers = [
            trigger for trigger in route_map["coord_events"]
            if trigger["script"] == "Route23_EventScript_PRLGreatBadgeHeist"
        ]
        self.assertEqual(len(triggers), 2)
        for trigger in triggers:
            self.assertEqual(trigger["elevation"], 0)
            self.assertEqual(trigger["var"], "VAR_TEMP_0")
            self.assertEqual(trigger["var_value"], "0")

        script = text("data/maps/Route23_Frlg/scripts.inc")
        self.assertIn("setvar VAR_TEMP_0, 1", script)
        self.assertNotIn("VAR_PRL_BADGE_HEIST_SCENE", script)
        self.assertNotIn("VAR_PRL_BADGE_HEIST_SCENE", text("include/constants/vars.h"))

    def test_balloon_spawns_in_the_route23_scene_before_takeoff(self):
        route_map = json.loads(text("data/maps/Route23_Frlg/map.json"))
        balloon = next(
            event for event in route_map["object_events"]
            if event.get("local_id") == "LOCALID_PRL_HEIST_BALLOON"
        )
        self.assertEqual((balloon["x"], balloon["y"]), (8, 30))
        script = text("data/maps/Route23_Frlg/scripts.inc")
        self.assertIn("setobjectxy LOCALID_PRL_HEIST_BALLOON, 8, 150", script)

    def test_balloon_is_visible_before_boarding_in_both_escapes(self):
        script = text("data/maps/Route23_Frlg/scripts.inc")
        for label in ("Route23_EventScript_PRLGreatBadgeHeist", "Route23_EventScript_PRLFinalRocketWon"):
            block = script.split(label + "::", 1)[1].split("\nRoute23_", 1)[0]
            spawn = block.index("addobject LOCALID_PRL_HEIST_BALLOON")
            board = block.index("applymovement LOCALID_PRL_HEIST_JESSIE, Route23_Movement_PRLJessie", block.index("msgbox"))
            self.assertLess(spawn, board, label)
            for actor in ("JESSIE", "JAMES", "MEOWTH"):
                self.assertLess(spawn, block.index("removeobject LOCALID_PRL_HEIST_" + actor), label)
            self.assertIn("setobjectsubpriority LOCALID_PRL_HEIST_BALLOON, MAP_ROUTE23, 0", block)

    def test_approved_meowth_balloon_bytes_are_integrated(self):
        gfx = ROOT / "graphics/object_events/pics/misc/prl_meowth_balloon.4bpp"
        pal = ROOT / "graphics/object_events/palettes/prl_meowth_balloon.rgb555"
        self.assertTrue(gfx.is_file())
        self.assertTrue(pal.is_file())
        self.assertEqual(gfx.stat().st_size, 2048)
        self.assertEqual(pal.stat().st_size, 32)
        self.assertEqual(hashlib.sha256(gfx.read_bytes()).hexdigest(),
                         "95d971316f21fcc393a3052e8897644997e65f51d7ad66dcd13fafab64552a68")
        self.assertEqual(hashlib.sha256(pal.read_bytes()).hexdigest(),
                         "32a16f0cde2dcd8ba91b9f60f0d2d66115345fb009a8ada21bc27ceda82c6241")

    def test_badge_guards_use_recovery_flags_without_changing_hm_badges(self):
        route = text("data/scripts/route23.inc")
        self.assertIn("FLAG_PRL_BADGE_HEIST_STARTED", route)
        for _, badge in RECOVERY.values():
            self.assertIn(f"FLAG_PRL_BADGE_HEIST_RECOVERED_{badge}", route)
        self.assertIn("Text_PRLBadgeHeistBadgeStillMissing", route)

    def test_victory_road_cannot_be_bypassed_by_warp_or_existing_save(self):
        overworld = text("src/overworld.c")
        self.assertIn("PRLRedirectBlockedVictoryRoadWarp", overworld)
        for map_name in ("MAP_VICTORY_ROAD_1F_FRLG", "MAP_VICTORY_ROAD_2F", "MAP_VICTORY_ROAD_3F"):
            self.assertIn(map_name, overworld)
        self.assertIn("FLAG_PRL_BADGE_HEIST_STARTED", overworld)
        self.assertIn("FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", overworld)
        self.assertIn("MAP_ROUTE23", overworld)
        self.assertIn("PRL_ROUTE23_SOUTH_WARP_ID", overworld)
        self.assertIn("PRLRedirectBlockedVictoryRoadSavedLocation();", overworld)
        for helper in (
            "SetWarpDestinationToDynamicWarp",
            "SetWarpDestinationToLastHealLocation",
            "SetWarpDestinationForTeleport",
            "SetWarpDestinationToEscapeWarp",
            "SetWarpDestinationToDiveWarp",
            "SetWarpDestinationToContinueGameWarp",
        ):
            marker = f"void {helper}(" if helper != "SetWarpDestinationToDiveWarp" else f"static void {helper}("
            block = overworld.split(marker, 1)[1].split("\n}", 1)[0]
            self.assertIn("PRLRedirectBlockedVictoryRoadWarp", block, helper)

    def test_final_rocket_encounter_is_on_route23_not_victory_road_3f(self):
        old_map = text("data/maps/VictoryRoad_3F_Frlg/map.json")
        old_script = text("data/maps/VictoryRoad_3F_Frlg/scripts.inc")
        self.assertNotIn("LOCALID_PRL_JESSIE_VICTORY_ROAD", old_map)
        self.assertNotIn("TRAINER_JESSIE_JAMES_VICTORY_ROAD", old_script)
        graphics = text("src/battle_gfx_sfx_util.c")
        self.assertIn("TRAINER_JESSIE_JAMES_VICTORY_ROAD", graphics)

    def test_each_leader_has_isolated_recovery_battle_and_no_first_clear_rewards(self):
        for leader, (map_name, badge) in RECOVERY.items():
            with self.subTest(leader=leader):
                script = text(f"data/maps/{map_name}/scripts.inc")
                label = f"{map_name}_EventScript_PRLBadgeRecovery"
                self.assertIn(label, script)
                self.assertIn(f"TRAINER_PRL_BADGE_RECOVERY_{leader}", script)
                self.assertIn(f"setflag FLAG_PRL_BADGE_HEIST_RECOVERED_{badge}", script)
                # Match the definition rather than an earlier branch reference.
                block = script.split(f"{label}::", 1)[1].split("\n\n", 1)[0]
                for forbidden in ("additem", "addmoney", "setflag FLAG_BADGE", "GiveTM", "GymExtra"):
                    self.assertNotIn(forbidden, block)

    def test_first_seven_are_any_order_and_giovanni_requires_all_seven(self):
        giovanni = text("data/maps/ViridianCity_Gym_Frlg/scripts.inc")
        for _, badge in list(RECOVERY.values())[:7]:
            self.assertIn(f"goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_{badge}", giovanni)
        self.assertIn("ViridianCity_Gym_EventScript_PRLGiovanniNotReady", giovanni)
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_ALL_RECOVERED", giovanni)

    def test_all_recovery_teams_are_new_late_game_six_mon_parties(self):
        parties = text("src/data/trainers_frlg.party")
        for leader in RECOVERY:
            marker = f"=== TRAINER_PRL_BADGE_RECOVERY_{leader} ==="
            self.assertIn(marker, parties)
            block = parties.split(marker, 1)[1].split("=== ", 1)[0]
            levels = [int(value) for value in re.findall(r"^Level: (\d+)$", block, re.M)]
            self.assertEqual(len(levels), 6, leader)
            self.assertGreaterEqual(min(levels), 54, leader)
            self.assertLessEqual(max(levels), 65, leader)

    def test_giovanni_recovery_team_retains_skeletal_species_and_trainer_art(self):
        parties = text("src/data/trainers_frlg.party")
        block = parties.split("=== TRAINER_PRL_BADGE_RECOVERY_GIOVANNI ===", 1)[1].split("=== ", 1)[0]
        self.assertIn("Pic: Leader Giovanni Frlg", block)
        self.assertIn("Kabutops", block)
        self.assertIn("Aerodactyl", block)
        graphics = text("src/battle_gfx_sfx_util.c")
        predicate = graphics.split("static u8 PRLGetGiovanniSkeletonGfx", 1)[1].split("static const u32 *", 1)[0]
        self.assertIn("TRAINER_PRL_BADGE_RECOVERY_GIOVANNI", predicate)
        self.assertIn("SPECIES_KABUTOPS", predicate)
        self.assertIn("SPECIES_AERODACTYL", predicate)

    def test_exact_back_sprite_offsets_and_front_art_are_preserved(self):
        species = text("src/data/pokemon/species_info/prl_custom.h")
        eden = species.split("[SPECIES_EDENSAUR]", 1)[1].split("[SPECIES_CHARAXIS]", 1)[0]
        charaxis = species.split("[SPECIES_CHARAXIS]", 1)[1].split("[SPECIES_FORTOTOISE]", 1)[0]
        self.assertIn(".backPicYOffset = 7", eden)
        self.assertIn(".backPicYOffset = 1", charaxis)
        self.assertIn(".frontPicYOffset = 0", eden)
        self.assertIn(".frontPicYOffset = 0", charaxis)
        self.assertIn("gMonFrontPic_Edensaur", eden)
        self.assertIn("gMonFrontPic_Charaxis", charaxis)


if __name__ == "__main__":
    unittest.main()
