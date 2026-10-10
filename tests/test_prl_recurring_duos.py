"""Regression checks for the eight mandatory PRL recurring double battles."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

ENCOUNTERS = {
    "STEEVE_NEEVEE_ROUTE3": ("Route3_Frlg", [("Eevee", 16), ("Eevee", 16)]),
    "STEEVE_NEEVEE_SS_ANNE": ("SSAnne_2F_Corridor_Frlg", [("Vaporeon", 26), ("Jolteon", 26), ("Eevee", 25), ("Eevee", 25)]),
    "STEEVE_NEEVEE_TOWER": ("PokemonTower_1F_Frlg", [("Flareon", 37), ("Vaporeon", 37), ("Jolteon", 36), ("Eevee", 36)]),
    "STEEVE_NEEVEE_ROUTE23": ("Route23_Frlg", [("Vaporeon", 52), ("Jolteon", 52), ("Flareon", 55), ("Toxeon", 55), ("Eevee", 51), ("Eevee", 53)]),
    "JESSIE_JAMES_MT_MOON": ("MtMoon_B1F_Frlg", [("Ekans", 16), ("Koffing", 16), ("Bellsprout", 16), ("Meowth", 17)]),
    "JESSIE_JAMES_HIDEOUT": ("RocketHideout_B4F_Frlg", [("Arbok", 29), ("Weezing", 29), ("Raticate", 30), ("Meowth", 30)]),
    "JESSIE_JAMES_SILPH": ("SilphCo_11F_Frlg", [("Arbok", 39), ("Weezing", 39), ("Drowzee", 40), ("Victreebel", 40), ("Pikachu", 41), ("Meowth", 41)]),
    "JESSIE_JAMES_VICTORY_ROAD": ("VictoryRoad_3F_Frlg", [("Arbok", 54), ("Weezing", 54), ("Hypno", 55), ("Victreebel", 55), ("Pikachu", 56), ("Meowth", 56)]),
}
GATES = {
    "STEEVE_NEEVEE_ROUTE3": "FLAG_BADGE01_GET",
    "STEEVE_NEEVEE_SS_ANNE": "FLAG_GOT_SS_TICKET",
    "STEEVE_NEEVEE_TOWER": "ITEM_SILPH_SCOPE",
    "STEEVE_NEEVEE_ROUTE23": "FLAG_BADGE08_GET",
    "JESSIE_JAMES_MT_MOON": "FLAG_GOT_FOSSIL_FROM_MT_MOON",
    "JESSIE_JAMES_HIDEOUT": "FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT",
    "JESSIE_JAMES_SILPH": "ITEM_SILPH_SCOPE",
    "JESSIE_JAMES_VICTORY_ROAD": "FLAG_BADGE08_GET",
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
                if name == "JESSIE_JAMES_MT_MOON":
                    self.assertIn("goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON",script)
                else:
                    if name.startswith("JESSIE_JAMES_"):
                    self.assertIn(f"goto_if_defeated TRAINER_{name}", script)
                else:
                    self.assertTrue(re.search(rf"goto_if_set FLAG_PRL_RECURRING_{name}\b", script), name)
                self.assertIn(GATES[name], script)
                if GATES[name] == "ITEM_SILPH_SCOPE":
                    self.assertIn("checkitem ITEM_SILPH_SCOPE", script)
                map_data = json.loads(read(f"data/maps/{map_name}/map.json"))
                if name == "JESSIE_JAMES_MT_MOON":
                    self.assertIn("MtMoon_B1F_EventScript_PRL_Jessie", script)
                    self.assertNotIn("call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON", read("data/maps/MtMoon_B2F_Frlg/scripts.inc"))
                elif name in ("JESSIE_JAMES_HIDEOUT", "JESSIE_JAMES_SILPH"):
                    self.assertIn("OnLoad_PRLRocketVisibility", script)
                    self.assertIn("EventScript_PRL_Jessie", script)
                    self.assertIn("EventScript_PRL_James", script)
                elif name == "JESSIE_JAMES_VICTORY_ROAD":
                    self.assertIn("MAP_SCRIPT_ON_TRANSITION", script)
                else:
                    self.assertTrue(any("PRLSteeveNeeveeTrigger" in event["script"] for event in map_data["coord_events"]), name)

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

    def test_victory_road_3f_duo_is_mandatory_before_route23_exit(self):
        floor2 = json.loads(read("data/maps/VictoryRoad_2F_Frlg/map.json"))
        exits = {(warp["x"], warp["y"]) for warp in floor2["warp_events"] if warp["dest_map"] == "MAP_ROUTE23"}
        self.assertTrue(exits, "Victory Road must retain its existing Route 23 exits")
        gated = {
            (event["x"], event["y"])
            for event in floor2["coord_events"]
            if event.get("script") == "VictoryRoad_2F_EventScript_PRL_RocketGate"
            and event.get("var") == "VAR_PRL_VICTORY_ROAD_ROCKET_GATE"
            and event.get("var_value") == "0"
        }
        self.assertTrue(exits <= gated, "every League exit warp must be intercepted until the 3F duo is beaten")
        floor2_script = read("data/maps/VictoryRoad_2F_Frlg/scripts.inc")
        self.assertIn("goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD", floor2_script)
        floor3_script = read("data/maps/VictoryRoad_3F_Frlg/scripts.inc")
        self.assertIn("setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1", floor3_script)

    def test_roster_is_limited_to_the_specified_kanto_species(self):
        banned = {"Espeon", "Umbreon", "Leafeon", "Glaceon", "Sylveon", "Noxichu", "Wobbuffet"}
        party = read("src/data/trainers_frlg.party")
        for name in ENCOUNTERS:
            self.assertIn(f"=== TRAINER_{name} ===", party)
            record = party.split(f"=== TRAINER_{name} ===", 1)[1].split("===", 1)[0]
            self.assertFalse(banned.intersection(re.findall(r"^([A-Za-z][A-Za-z -]+)$", record, re.M)))

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
            "Route23_Frlg": "From ROUTE 3 to the League",
            "RocketHideout_B4F_Frlg": "You ruined MT. MOON",
            "SilphCo_11F_Frlg": "After MT. MOON and",
            "VictoryRoad_3F_Frlg": "From SILPH to here",
        }
        for map_name, line in expected.items():
            with self.subTest(map=map_name):
                self.assertIn(line, read(f"data/maps/{map_name}/scripts.inc"))


    def test_rocket_event_labels_and_scene_flags_resolve_within_frlg_range(self):
        flags = read("include/constants/flags_frlg.h")
        self.assertNotRegex(flags, r"#define FLAG_HIDE_PRL_\w+\s+FLAG_0x90[0-9A-F]")
        for map_name, prefix in (("MtMoon_B1F_Frlg", "MtMoon_B1F"),
                                 ("RocketHideout_B4F_Frlg", "RocketHideout_B4F"),
                                 ("SilphCo_11F_Frlg", "SilphCo_11F"),
                                 ("VictoryRoad_3F_Frlg", "VictoryRoad_3F")):
            script = read(f"data/maps/{map_name}/scripts.inc")
            if prefix == "MtMoon_B1F":
                self.assertIn("MtMoon_B1F_EventScript_PRL_Jessie", script)
                continue
            self.assertIn(f"{prefix}_EventScript_PRL_Jessie::", script)
            self.assertIn(f"{prefix}_EventScript_PRL_James::", script)
            self.assertNotIn(f"{prefix}_EventScript_PRL_JESSIE_JAMES_", script)
            for ref in re.findall(r"\b(?:call|goto) (\w*PRL_JESSIE_JAMES_\w+)", script):
                self.assertRegex(script, rf"(?m)^{re.escape(ref)}::", msg=ref)



    def test_final_route23_preserves_existing_setup_for_returning_pokemon(self):
        party = read("src/data/trainers_frlg.party")
        record = party.split("=== TRAINER_STEEVE_NEEVEE_ROUTE23 ===", 1)[1].split("\n=== ", 1)[0]
        retained_setups = [
            "Vaporeon @ Sitrus Berry\nLevel: 52\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n- Surf\n- Aurora Beam\n- Bite\n- Acid Armor",
            "Jolteon @ Sitrus Berry\nLevel: 52\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n- Thunderbolt\n- Double Kick\n- Quick Attack\n- Thunder Wave",
            "Flareon @ Sitrus Berry\nLevel: 55\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n- Flamethrower\n- Fire Spin\n- Bite\n- Quick Attack",
            "Eevee\nLevel: 51\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n- Bite\n- Baton Pass\n- Quick Attack\n- Sand Attack",
            "Eevee @ Sitrus Berry\nLevel: 53\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n- Take Down\n- Bite\n- Quick Attack\n- Helping Hand",
        ]
        for setup in retained_setups:
            with self.subTest(setup=setup.splitlines()[0]):
                self.assertIn(setup, record)
        self.assertIn(f"Toxeon\nLevel: 55\n{zero_iv_for_test()}", record)


if __name__ == "__main__":
    unittest.main()
