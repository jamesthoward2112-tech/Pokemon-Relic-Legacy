"""Regression checks for the eight mandatory PRL recurring double battles."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

ENCOUNTERS = {
    "STEEVE_NEEVEE_ROUTE3": ("Route3_Frlg", [("Eevee", 16), ("Eevee", 16)]),
    "STEEVE_NEEVEE_SS_ANNE": ("SSAnne_2F_Corridor_Frlg", [("Vaporeon", 26), ("Jolteon", 26), ("Eevee", 25), ("Eevee", 25)]),
    "STEEVE_NEEVEE_TOWER": ("PokemonTower_1F_Frlg", [("Flareon", 37), ("Omeon", 37), ("Espeon", 37), ("Umbreon", 36)]),
    "STEEVE_NEEVEE_ROUTE23": ("Route23_Frlg", [("Vaporeon", 52), ("Jolteon", 52), ("Flareon", 55), ("Toxeon", 55), ("Nimbeon", 54), ("Eeveeon", 55)]),
    "JESSIE_JAMES_MT_MOON": ("MtMoon_B1F_Frlg", [("Ekans", 16), ("Koffing", 16), ("Bellsprout", 16), ("Meowth", 17)]),
    "JESSIE_JAMES_HIDEOUT": ("RocketHideout_B4F_Frlg", [("Arbok", 29), ("Weezing", 29), ("Raticate", 30), ("Meowth", 30)]),
    "JESSIE_JAMES_SILPH": ("SilphCo_11F_Frlg", [("Arbok", 39), ("Weezing", 39), ("Drowzee", 40), ("Victreebel", 40), ("Pikachu", 41), ("Meowth", 41)]),
    "JESSIE_JAMES_VICTORY_ROAD": ("Route23_Frlg", [("Arbok", 58), ("Weezing", 58), ("Victreebel", 59), ("Gyarados", 59), ("Pikachu", 60), ("Meowth", 60)]),
}
GATES = {
    "STEEVE_NEEVEE_ROUTE3": "FLAG_BADGE01_GET",
    "STEEVE_NEEVEE_SS_ANNE": "FLAG_GOT_SS_TICKET",
    "STEEVE_NEEVEE_ROUTE23": "FLAG_BADGE08_GET",
    "JESSIE_JAMES_MT_MOON": "FLAG_GOT_FOSSIL_FROM_MT_MOON",
    "JESSIE_JAMES_HIDEOUT": "FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT",
    "JESSIE_JAMES_SILPH": "ITEM_SILPH_SCOPE",
    "JESSIE_JAMES_VICTORY_ROAD": "FLAG_PRL_BADGE_HEIST_ALL_RECOVERED",
}


def read(path):
    return (ROOT / path).read_text(encoding="utf-8", errors="surrogateescape")




def zero_iv_for_test():
    return "IVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe"

class PRLRecurringDuos(unittest.TestCase):
    def test_each_encounter_has_a_persistent_trainer_and_double_battle(self):
        party = read("src/data/trainers_frlg.party")
        constants = read("include/constants/opponents_frlg.h")
        for name, (_, team) in ENCOUNTERS.items():
            with self.subTest(encounter=name):
                self.assertIsNotNone(re.search(rf"^#define TRAINER_{name}\s+\d+", constants, re.M), name)
                record = party.split(f"=== TRAINER_{name} ===", 1)[1].split("===", 1)[0]
                self.assertIn("Double Battle: Yes", record)
                parsed = [(species.split(" @", 1)[0], level) for species, level in re.findall(r"^(.*?)\s*$\nLevel: (\d+)", record, re.M)]
                self.assertEqual(parsed, [(species, str(level)) for species, level in team])
                self.assertEqual(record.count("Level:"), len(team))

    def test_each_map_contains_a_story_gated_persistent_battle_event(self):
        flags = read("include/constants/flags_frlg.h")
        for name, (map_name, _) in ENCOUNTERS.items():
            with self.subTest(encounter=name):
                self.assertIsNotNone(re.search(rf"^#define FLAG_PRL_RECURRING_{name}\s+FLAG_0x[0-9A-F]+", flags, re.M), name)
                script = read(f"data/maps/{map_name}/scripts.inc")
                self.assertTrue(re.search(rf"trainerbattle_double TRAINER_{name}\b", script), name)
                self.assertTrue(re.search(rf"setflag FLAG_PRL_RECURRING_{name}\b", script), name)
                if name == "JESSIE_JAMES_VICTORY_ROAD":
                    self.assertIn("setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", script)
                elif name.startswith("JESSIE_JAMES_"):
                    self.assertIn(f"goto_if_defeated TRAINER_{name}", script)
                else:
                    self.assertIn(f"trainerbattle_double TRAINER_{name}", script)  # native trainer flag detection
                if name in GATES:
                    self.assertIn(GATES[name], script)
                if name == "STEEVE_NEEVEE_TOWER":
                    for actor in ("Steevee", "Neevee"):
                        label = f"PokemonTower_1F_EventScript_PRLSteeveNeevee{actor}::"
                        body = script.split(label, 1)[1]
                        self.assertTrue(body.lstrip().startswith("trainerbattle_double"),
                                        f"Tower {actor} must start with trainerbattle for native trainer sight")
                    self.assertNotIn("PokemonTower_1F_EventScript_PRLSteeveNeeveeNotReady", script)
                map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
                if name == "JESSIE_JAMES_MT_MOON":
                    self.assertIn("MtMoon_B1F_EventScript_PRL_Jessie", script)
                    self.assertNotIn("call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON", read("data/maps/MtMoon_B2F_Frlg/scripts.inc"))
                elif name in ("JESSIE_JAMES_HIDEOUT", "JESSIE_JAMES_SILPH"):
                    self.assertIn("OnLoad_PRLRocketVisibility", script)
                    self.assertIn("EventScript_PRL_Jessie", script)
                    self.assertIn("EventScript_PRL_James", script)
                elif name == "JESSIE_JAMES_VICTORY_ROAD":
                    self.assertIn("Route23_EventScript_PRLFinalRocketBattle", script)
                else:
                    # All four twin encounters use native object-event sight (and its !).
                    self.assertFalse(any("PRLSteeveNeeveeTrigger" in event["script"] for event in map_data["coord_events"]), name)
                    actor_suffix = name.split("STEEVE_NEEVEE_", 1)[1]
                    actors = [o for o in map_data["object_events"] if o.get("local_id") in
                              (f"LOCALID_PRL_STEEVE_{actor_suffix}", f"LOCALID_PRL_NEEVEE_{actor_suffix}")]
                    self.assertEqual(len(actors), 2)
                    if actor_suffix == "SS_ANNE":
                        self.assertEqual(actors[0]["x"], actors[1]["x"])
                        self.assertEqual(abs(actors[0]["y"] - actors[1]["y"]), 1)
                        self.assertEqual(actors[0]["movement_type"], "MOVEMENT_TYPE_FACE_LEFT")
                        self.assertEqual(actors[1]["movement_type"], "MOVEMENT_TYPE_FACE_LEFT")
                    else:
                        self.assertEqual(actors[0]["y"], actors[1]["y"])
                        self.assertEqual(abs(actors[0]["x"] - actors[1]["x"]), 1)
                    for o in actors:
                        expected_type = ("TRAINER_TYPE_SEE_ALL_DIRECTIONS"
                                         if name == "STEEVE_NEEVEE_TOWER" else "TRAINER_TYPE_NORMAL")
                        self.assertEqual(o["trainer_type"], expected_type)
                        self.assertGreaterEqual(int(o["trainer_sight_or_berry_tree_id"]), 3)

    def test_mt_moon_battle_follows_both_fossil_choices_without_replacing_miguel(self):
        script = read("data/maps/MtMoon_B2F_Frlg/scripts.inc")
        for fossil_script in ("MtMoon_B2F_EventScript_DomeFossil", "MtMoon_B2F_EventScript_HelixFossil"):
            body = script.split(f"{fossil_script}::", 1)[1].split("\nMtMoon_B2F_", 1)[0]
            self.assertIn("setflag FLAG_GOT_FOSSIL_FROM_MT_MOON", body, fossil_script)
            self.assertNotIn("call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON", body, fossil_script)
        self.assertIn("MtMoon_B2F_EventScript_MiguelTrigger", script)
        self.assertIn("MtMoon_B2F_EventScript_Miguel::", script)
        b1 = read("data/maps/MtMoon_B1F_Frlg/scripts.inc")
        self.assertIn("trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON", b1)
        self.assertIn("FLAG_GOT_FOSSIL_FROM_MT_MOON", b1)

    def test_route23_finale_is_mandatory_before_victory_road(self):
        route = read("data/maps/Route23_Frlg/scripts.inc")
        self.assertIn("FLAG_PRL_BADGE_HEIST_ALL_RECOVERED", route)
        self.assertIn("setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", route)
        overworld = read("src/overworld.c")
        self.assertIn("PRLRedirectBlockedVictoryRoadWarp", overworld)
        self.assertIn("FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED", overworld)

    def test_roster_is_limited_to_the_specified_kanto_species(self):
        banned = {"Noxichu", "Wobbuffet"}
        party = read("src/data/trainers_frlg.party")
        for name in ENCOUNTERS:
            self.assertIn(f"=== TRAINER_{name} ===", party)
            record = party.split(f"=== TRAINER_{name} ===", 1)[1].split("===", 1)[0]
            self.assertFalse(banned.intersection(re.findall(r"^([A-Za-z][A-Za-z -]+)$", record, re.M)))


    def test_eevee_twins_remain_on_map_after_victory_and_can_be_spoken_to(self):
        """A defeated pair behaves like ordinary trainers: visible, friendly and never rematched."""
        for name, (map_name, _) in ENCOUNTERS.items():
            if not name.startswith("STEEVE_NEEVEE_"):
                continue
            with self.subTest(encounter=name):
                map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
                twins = [obj for obj in map_data["object_events"]
                         if obj.get("local_id") in (f"LOCALID_PRL_STEEVE_{name.split('STEEVE_NEEVEE_', 1)[1]}",
                                                    f"LOCALID_PRL_NEEVEE_{name.split('STEEVE_NEEVEE_', 1)[1]}")]
                self.assertEqual(len(twins), 2, "both twins must be visible")
                for obj in twins:
                    self.assertEqual(obj["flag"], "0", "defeat flags must not hide the NPC")
                    self.assertEqual(obj["graphics_id"], "OBJ_EVENT_GFX_TWIN")
                    expected_facing = "MOVEMENT_TYPE_FACE_LEFT" if name.endswith("_SS_ANNE") else "MOVEMENT_TYPE_FACE_DOWN"
                    self.assertEqual(obj["movement_type"], expected_facing)
                if name.endswith("_SS_ANNE"):
                    self.assertEqual([(obj["x"], obj["y"]) for obj in twins], [(31, 9), (31, 8)])
                script = read(f"data/maps/{map_name}/scripts.inc")
                suffix = name.split("STEEVE_NEEVEE_", 1)[1]
                prefix = {"ROUTE3": "Route3", "SS_ANNE": "SSAnne_2F_Corridor",
                          "TOWER": "PokemonTower_1F", "ROUTE23": "Route23"}[suffix]
                self.assertIn(f"{prefix}_EventScript_PRLSteeveNeeveeSteevee::", script)
                self.assertIn(f"{prefix}_EventScript_PRLSteeveNeeveeNeevee::", script)
                self.assertIn(f"{prefix}_EventScript_PRLSteeveNeeveeTriggerWon::", script)
                self.assertEqual(script.count(f"trainerbattle_double TRAINER_{name}"), 2)
                for actor in ("STEEVE", "NEEVEE"):
                    self.assertIn(f"localId=LOCALID_PRL_{actor}_{suffix}", script)
                self.assertIn(f"msgbox {prefix}_Text_PRL_SteeveNeeveePostBattle", script)
                self.assertNotIn("continueScript=TRUE", script.split(f"{prefix}_EventScript_PRLSteeveNeeveeSteevee::",1)[1].split(f"{prefix}_Text_PRL_SteeveNeeveeIntro::",1)[0])
                for actor in ("STEEVE", "NEEVEE"):
                    self.assertNotIn(f"removeobject LOCALID_PRL_{actor}_{suffix}", script)
                self.assertNotIn(f"{prefix}_EventScript_PRLSteeveNeeveeMovement_Leave::", script)

    def test_route3_eevee_moves_are_available_by_level_16_in_gen3(self):
        party = read("src/data/trainers_frlg.party")
        record = party.split("=== TRAINER_STEEVE_NEEVEE_ROUTE3 ===", 1)[1].split("===", 1)[0]
        moves = re.findall(r"^- (.+)$", record, re.M)
        legal_level_16 = {"Tackle", "Tail Whip", "Helping Hand", "Sand Attack", "Growl"}
        self.assertEqual(len(moves), 8)
        self.assertTrue(set(moves) <= legal_level_16)

    def test_hideout_meowth_does_not_use_level_41_move_at_level_30(self):
        party = read("src/data/trainers_frlg.party")
        record = party.split("=== TRAINER_JESSIE_JAMES_HIDEOUT ===", 1)[1].split("===", 1)[0]
        meowth = record.split("Meowth\nLevel: 30", 1)[1].split("===", 1)[0]
        self.assertNotIn("- Fury Swipes", meowth)

    def test_rocket_scenes_have_hidden_three_character_actors_and_follow_choreography(self):
        for name, (map_name, _) in ENCOUNTERS.items():
            if not name.startswith("JESSIE_JAMES_"):
                continue
            with self.subTest(encounter=name):
                map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
                graphics = {obj["graphics_id"] for obj in map_data["object_events"]}
                for actor in ("OBJ_EVENT_GFX_PRL_JESSIE", "OBJ_EVENT_GFX_PRL_JAMES", "OBJ_EVENT_GFX_MEOWTH"):
                    self.assertIn(actor, graphics, actor)
                script = read(f"data/maps/{map_name}/scripts.inc")
                self.assertIn("trainerbattle_double", script)
                if name == "JESSIE_JAMES_VICTORY_ROAD":
                    self.assertIn("Route23_EventScript_PRLFinalRocketBattle", script)
                    self.assertIn("Route23_Movement_PRLBalloonTakeoff", script)
                    self.assertIn("removeobject LOCALID_PRL_HEIST_JESSIE", script)
                    continue
                self.assertIn("EventScript_PRL_Jessie::", script)
                self.assertIn("EventScript_PRL_James::", script)
                self.assertIn("Text_PRL_JessiePostBattle::", script)
                self.assertIn("Text_PRL_JamesPostBattle::", script)
                self.assertNotIn("removeobject LOCALID_PRL_JESSIE", script)
                self.assertNotIn("removeobject LOCALID_PRL_JAMES", script)
                if name != "JESSIE_JAMES_MT_MOON":
                    self.assertIn("addobject LOCALID_PRL_MEOWTH", script)
                    for actor in ("JESSIE","JAMES"):
                        npc=next(x for x in map_data["object_events"] if x.get("local_id","").startswith("LOCALID_PRL_"+actor+"_"))
                        self.assertEqual(npc["trainer_type"],"TRAINER_TYPE_NORMAL")


    def test_recurring_duos_acknowledge_prior_kanto_encounters(self):
        expected = {
            "SSAnne_2F_Corridor_Frlg": "We met on ROUTE 3",
            "PokemonTower_1F_Frlg": "Remember us from the ship",
            "RocketHideout_B4F_Frlg": "You ruined MT. MOON",
            "SilphCo_11F_Frlg": "After MT. MOON and",
        }
        for map_name, line in expected.items():
            with self.subTest(map=map_name):
                self.assertIn(line, read(f"data/maps/{map_name}/scripts.inc"))
        route23 = read("data/maps/Route23_Frlg/scripts.inc")
        self.assertIn("From ROUTE 3 to the League", route23)
        self.assertIn("final KANTO performance", route23)


    def test_rocket_event_labels_and_scene_flags_resolve_within_frlg_range(self):
        flags = read("include/constants/flags_frlg.h")
        self.assertNotRegex(flags, r"#define FLAG_HIDE_PRL_\w+\s+FLAG_0x90[0-9A-F]")
        for map_name, prefix in (("MtMoon_B1F_Frlg", "MtMoon_B1F"),
                                 ("RocketHideout_B4F_Frlg", "RocketHideout_B4F"),
                                 ("SilphCo_11F_Frlg", "SilphCo_11F"),
                                 ):
            script = read(f"data/maps/{map_name}/scripts.inc")
            if prefix == "MtMoon_B1F":
                self.assertIn("MtMoon_B1F_EventScript_PRL_Jessie", script)
                continue
            self.assertIn(f"{prefix}_EventScript_PRL_Jessie::", script)
            self.assertIn(f"{prefix}_EventScript_PRL_James::", script)
            self.assertNotIn(f"{prefix}_EventScript_PRL_JESSIE_JAMES_", script)
            for ref in re.findall(r"\b(?:call|goto) (\w*PRL_JESSIE_JAMES_\w+)", script):
                self.assertRegex(script, rf"(?m)^{re.escape(ref)}::", msg=ref)



    def test_route23_has_balanced_eeveelutions_and_signature(self):
        party = read("src/data/trainers_frlg.party")
        rec = party.split("=== TRAINER_STEEVE_NEEVEE_ROUTE23 ===", 1)[1].split("=== ", 1)[0]
        for name in ("Vaporeon", "Jolteon", "Flareon", "Toxeon", "Nimbeon", "Eeveeon"):
            self.assertIn(name, rec)
        self.assertIn("- Eevoboost", rec)
        self.assertEqual(rec.count("Level:"), 6)

    def test_prl_hm_and_harbour_source_regressions(self):
        script = read("data/maps/VermilionCity_Frlg/scripts.inc")
        self.assertIn("VermilionCity_EventScript_PRLPierAccess::", script)
        self.assertIn("setvar VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER, 1", script)
        self.assertIn("VermilionCity_Text_PRLPierAccess:", script)
        self.assertIn("The pier is open to Trainers now.", script)
        hm = read("src/scrcmd.c")
        self.assertIn("(doUnlockedCheck || useFieldHm) && !IsFieldMoveUnlocked(fieldMove)", hm)
        self.assertIn("CheckBagHasItem(hmItem, 1)", hm)
        self.assertIn("if (useFieldHm || MonKnowsMove(", hm)
        self.assertNotIn("CanLearnTeachableMove(species, move)", hm)
        fly = read("src/field_region_map.c")
        self.assertIn("PRL_CanFlyDirectlyFromMap()", fly)
        self.assertIn("CheckBagHasItem(hm, 1)", fly)
        self.assertNotIn("CanLearnTeachableMove(species, MOVE_FLY)", fly)

    def test_select_relearner_and_field_hm_text_regression(self):
        field = read("src/field_control_avatar.c")
        self.assertIn("sPlayerSelectHoldFrames", field)
        self.assertIn("heldKeys & SELECT_BUTTON", field)
        self.assertIn("sPlayerSelectHoldFrames == 60", field)
        item = read("src/item_menu.c")
        self.assertNotIn("OpenRegisteredItemQuickMenu", item)
        self.assertIn("GetRegisteredItemHold()", item)
        relearner = read("src/move_relearner.c")
        self.assertIn("PRL_IsExtraRelearnerMoveUnlocked", relearner)
        self.assertIn("gMovesInfo[move].power > maxPower", relearner)
        for category in ("EggMoves", "TMMoves", "TutorMoves"):
            self.assertIn("GetRelearner" + category, relearner)
        text = read("data/scripts/field_move_scripts.inc")
        self.assertIn("{PLAYER} used {STR_VAR_2}!", text)
        self.assertIn("{PLAYER} used STRENGTH!", text)


if __name__ == "__main__":
    unittest.main()
