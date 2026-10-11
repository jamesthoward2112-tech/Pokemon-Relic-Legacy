"""Validate approved 2026-10-11 duo trainer presentation assets and final Kanto J&J roster."""
import hashlib
import pathlib
import struct
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class PRLDuoApprovedArt(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        import sys
        sys.path.insert(0, str(ROOT / "tools"))
        import prl_duo_20261011_assets
        cls.expected_paths = prl_duo_20261011_assets.unpack()

    def test_packs_have_full_bg0_tiles_and_correct_palette_bank(self):
        for name in ("jessie_james", "eevee_twins"):
            with self.subTest(name=name):
                root = ROOT / "graphics/battle_transitions/prl" / name
                self.assertEqual(len(root.with_suffix(".4bpp").read_bytes()), 512 * 32)
                self.assertEqual(len(root.with_suffix(".gbapal").read_bytes()), 16 * 2)
                m = root.with_suffix(".bin").read_bytes()
                self.assertEqual(len(m), 32 * 32 * 2)
                tiles = struct.unpack("<1024H", m)
                self.assertTrue(all((t >> 12) == 13 for t in tiles))
                self.assertTrue(all((t & 0x3ff) < 512 for t in tiles))

    def test_exclusive_routing_uses_original_kanto_trainer_ids(self):
        setup = (ROOT / "src/battle_setup.c").read_text()
        for token in (
            "TRAINER_JESSIE_JAMES_MT_MOON",
            "TRAINER_JESSIE_JAMES_HIDEOUT",
            "TRAINER_JESSIE_JAMES_SILPH",
            "TRAINER_JESSIE_JAMES_VICTORY_ROAD",
            "TRAINER_STEEVE_NEEVEE_ROUTE3",
            "TRAINER_STEEVE_NEEVEE_SS_ANNE",
            "TRAINER_STEEVE_NEEVEE_TOWER",
            "TRAINER_STEEVE_NEEVEE_ROUTE23",
        ):
            self.assertIn("case " + token + ":", setup)
        self.assertIn("B_TRANSITION_JESSIE_JAMES", setup)
        self.assertIn("B_TRANSITION_EEVEE_TWINS", setup)

    def test_patternweave_sprite_data_uses_matching_bank_13(self):
        code = (ROOT / "src/battle_transition.c").read_text()
        self.assertIn("Task_PRL_JessieJames", code)
        self.assertIn("Task_PRL_EeveeTwins", code)
        self.assertIn("BG_PLTT_ID(13)", code)
        self.assertIn("sPRL_jessie_james_Tiles", code)
        self.assertIn("sPRL_eevee_twins_Tiles", code)

    def test_original_dark_rocket_sprite_bank_is_gba_valid(self):
        import sys
        sys.path.insert(0, str(ROOT / "tools"))
        import prl_rocket_dark_20261011_assets
        for path in prl_rocket_dark_20261011_assets.unpack():
            f = ROOT / path
            self.assertTrue(f.exists(), path)
            payload = f.read_bytes()
            if path.endswith(".gbapal"):
                self.assertEqual(len(payload), 32)
            else:
                self.assertEqual(payload[:4], bytes((0x10, 0, 8, 0)))
                self.assertEqual(len(payload) % 4, 0)

    def test_rocket_dark_visuals_are_scoped_without_species_replacement(self):
        gfx = (ROOT / "src/battle_gfx_sfx_util.c").read_text()
        self.assertIn("PRLGetRocketDarkGfx", gfx)
        self.assertIn("PRLGetRocketDarkFront(prlRocketDarkGfx)", gfx)
        self.assertIn("PRLGetRocketDarkPalette(prlRocketDarkGfx)", gfx)
        for x in ("ARBOK", "RATICATE", "VICTREEBEL", "GYARADOS"):
            self.assertIn("PRL_ROCKET_DARK_" + x, gfx)
        self.assertIn("IsOnPlayerSide(battler)", gfx)
        self.assertIn("TRAINER_JESSIE_JAMES_VICTORY_ROAD", gfx)

    def test_galarian_weezing_skin_is_scoped_to_rocket_only(self):
        graphics = (ROOT / "src/battle_gfx_sfx_util.c").read_text()
        self.assertIn("PRLIsJessieJamesWeezingVisual", graphics)
        self.assertIn("SPECIES_WEEZING_GALAR", graphics)
        self.assertIn("species != SPECIES_WEEZING", graphics)
        self.assertIn("IsOnPlayerSide(battler)", graphics)
        self.assertIn("TRAINER_JESSIE_JAMES_VICTORY_ROAD", graphics)
        self.assertIn("prlRocketWeezing ? SPECIES_WEEZING_GALAR : species", graphics)
        self.assertIn("GetMonSpritePalFromSpeciesAndPersonality(SPECIES_WEEZING_GALAR", graphics)

    def test_all_standard_frlg_pokemon_center_upstairs_match_vermilion(self):
        import json
        kanto_centers = ("ViridianCity", "PewterCity", "CeruleanCity", "LavenderTown",
                         "VermilionCity", "CeladonCity", "FuchsiaCity", "CinnabarIsland",
                         "IndigoPlateau", "SaffronCity", "Route4", "Route10",
                         "SevenIsland", "TwoIsland", "ThreeIsland", "FourIsland",
                         "FiveIsland", "SixIsland")
        base = json.loads((ROOT / "data/maps/VermilionCity_PokemonCenter_2F_Frlg/map.json").read_text())
        for area in kanto_centers:
            with self.subTest(center=area):
                path = ROOT / f"data/maps/{area}_PokemonCenter_2F_Frlg/map.json"
                room = json.loads(path.read_text())
                self.assertEqual(room["layout"], "LAYOUT_POKEMON_CENTER_2F_FRLG")
                self.assertEqual(room["object_events"], base["object_events"],
                                 "Keep Vermilion's three upstairs services and Mystery Gift")
                self.assertEqual(len(room["warp_events"]), len(base["warp_events"]))
                self.assertEqual(room["warp_events"][0]["dest_map"],
                                 f"MAP_{self._center_constant(area)}_POKEMON_CENTER_1F")
        # One Island has its own original, intentional upstairs map layout.

    @staticmethod
    def _center_constant(area):
        import re
        return re.sub(r"(?<!^)(?=[A-Z][a-z])", "_", area).upper()

    def test_omeon_uses_grounded_front_sprite_y_offset(self):
        data = (ROOT / "src/data/pokemon/species_info/prl_custom.h").read_text()
        block = data.split("[SPECIES_OMEON] =", 1)[1].split("[SPECIES_DRAKEON] =", 1)[0]
        self.assertIn(".frontPic = gMonFrontPic_Omeon", block)
        self.assertIn(".frontPicYOffset = 7", block)
        self.assertIn(".backPicYOffset = 0", block)


    def test_ancient_stone_is_distinct_and_correctly_named_in_celadon_shop(self):
        items = (ROOT / "src/data/items.h").read_text()
        old = items.split("[ITEM_SHINY_STONE] =", 1)[1].split("[ITEM_DUSK_STONE] =", 1)[0]
        ancient = items.split("[ITEM_ANCIENT_STONE] =", 1)[1].split("// PRL STEP10 ITEM DATA END", 1)[0]
        self.assertIn('.name = ITEM_NAME("Shiny Stone")', old)
        self.assertIn('.name = ITEM_NAME("Ancient Stone")', ancient)
        self.assertNotIn('.name = ITEM_NAME("Shiny Stone")', ancient)
        self.assertIn("ItemUseOutOfBattle_EvolutionStone", ancient)
        self.assertIn("gItemEffect_EvoItem", ancient)
        shop = (ROOT / "data/maps/CeladonCity_DepartmentStore_4F_Frlg/scripts.inc").read_text()
        self.assertIn(".2byte ITEM_ANCIENT_STONE", shop)

    def test_cinnabar_oak_aide_grants_the_other_mt_moon_fossil_once(self):
        import json
        flags = (ROOT / "include/constants/flags.h").read_text()
        self.assertIn("FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT                 0x4AC", flags)
        data = json.loads((ROOT / "data/maps/CinnabarIsland_PokemonLab_Entrance_Frlg/map.json").read_text())
        matching = [o for o in data["object_events"] if o["script"].endswith("EventScript_PRL_FossilAide")]
        self.assertEqual(len(matching), 1)
        self.assertEqual(matching[0]["graphics_id"], "OBJ_EVENT_GFX_SCIENTIST")
        self.assertEqual(matching[0]["flag"], "0")
        script = (ROOT / "data/maps/CinnabarIsland_PokemonLab_Entrance_Frlg/scripts.inc").read_text()
        gift = script.split("CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAide::", 1)[1]
        gift = gift.split("CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilGiftDone::", 1)[0]
        self.assertIn("goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON", gift)
        self.assertIn("goto_if_set FLAG_GOT_DOME_FOSSIL", gift)
        self.assertIn("goto_if_set FLAG_GOT_HELIX_FOSSIL", gift)
        self.assertIn("checkitemspace ITEM_HELIX_FOSSIL, 1", gift)
        self.assertIn("checkitemspace ITEM_DOME_FOSSIL, 1", gift)
        self.assertEqual(gift.count("setflag FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT"), 3)
        self.assertIn("giveitem ITEM_HELIX_FOSSIL, 1", gift)
        self.assertIn("giveitem ITEM_DOME_FOSSIL, 1", gift)
        self.assertNotIn("pokedex", gift.lower())
        self.assertIn("CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace", gift)


    def test_rooftop_eevee_expert_shop_and_all_18_methods(self):
        import json
        room = json.loads((ROOT / "data/maps/CeladonCity_Condominiums_RoofRoom_Frlg/map.json").read_text())
        expert = [x for x in room["object_events"]
                  if x["script"] == "CeladonCity_Condominiums_RoofRoom_EventScript_PRLEeveelutionsExpert"]
        self.assertEqual(len(expert), 1)
        self.assertEqual((expert[0]["x"], expert[0]["y"]), (8, 5))
        self.assertEqual(expert[0]["trainer_type"], "TRAINER_TYPE_NONE")
        self.assertEqual(expert[0]["flag"], "0")
        self.assertEqual(len([x for x in room["object_events"]
                              if x.get("local_id") == "LOCALID_EEVEE_POKEBALL"]), 1)
        self.assertEqual(len(room["warp_events"]), 3)
        menu_enum = (ROOT / "include/constants/script_menu.h").read_text()
        menu_def = (ROOT / "src/data/script_menu.h").read_text()
        self.assertIn("MULTI_PRL_EEVEE_EXPERT", menu_enum)
        self.assertIn("MULTICHOICE(sMultichoiceList_PRLEeveeExpert)", menu_def)
        for action in ("BUY STONES", "BUY HELD ITEMS", "STONE GUIDE", "OTHER EVOLUTIONS"):
            self.assertIn(action, menu_def)
        src = (ROOT / "data/maps/CeladonCity_Condominiums_RoofRoom_Frlg/scripts.inc").read_text()
        stones = src.split("CeladonCity_Condominiums_RoofRoom_PRLEeveeStones::", 1)[1]
        stones = stones.split("CeladonCity_Condominiums_RoofRoom_PRLEeveeHeldItems::", 1)[0]
        for stone in ("WATER_STONE", "THUNDER_STONE", "FIRE_STONE", "LEAF_STONE",
                      "ICE_STONE", "DUSK_STONE", "ANCIENT_STONE"):
            self.assertIn(".2byte ITEM_" + stone, stones)
        self.assertEqual(stones.count(".2byte ITEM_"), 8)  # includes terminator
        held = src.split("CeladonCity_Condominiums_RoofRoom_PRLEeveeHeldItems::", 1)[1]
        held = held.split("CeladonCity_Condominiums_RoofRoom_Text_PRLExpertGreeting::", 1)[0]
        for item in ("SHARP_BEAK", "EXPERT_BELT", "SILVER_POWDER", "SOFT_SAND",
                     "METAL_COAT", "HARD_STONE", "POISON_BARB", "DRAGON_FANG"):
            self.assertIn(".2byte ITEM_" + item, held)
        self.assertEqual(held.count(".2byte ITEM_"), 9)
        self.assertIn("multichoice 0, 0, MULTI_PRL_EEVEE_EXPERT", src)
        evo = (ROOT / "src/data/pokemon/species_info/gen_1_families.h").read_text()
        evolution = evo.split("[SPECIES_EEVEE] =", 1)[1].split("[SPECIES_EEVEE_GMAX]", 1)[0]
        target_forms = ("VAPOREON", "JOLTEON", "FLAREON", "LEAFEON", "GLACEON",
                        "OMEON", "EEVEEON", "NIMBEON", "CHAMPEON", "LEPIDEON",
                        "SPHYNXEON", "GUARDEON", "OBSIDEON", "TOXEON", "DRAKEON",
                        "ESPEON", "UMBREON", "SYLVEON")
        for species in target_forms:
            self.assertIn("SPECIES_" + species, evolution)
            self.assertIn(species, src, "Expert should describe every current evolution")

    def test_kanto_finale_has_strong_unchanged_mascots(self):
        party = (ROOT / "src/data/trainers_frlg.party").read_text()
        section = party.split("=== TRAINER_JESSIE_JAMES_VICTORY_ROAD ===", 1)[1].split("===", 1)[0]
        self.assertEqual(section.count("Level:"), 6)
        self.assertNotIn("Hypno\n", section)
        for line in ("Gyarados @ Sitrus Berry", "Pikachu @ Light Ball", "Meowth @ Focus Sash",
                     "Ability: Technician", "- Fake Out", "- Feint", "- Protect"):
            self.assertIn(line, section)


if __name__ == "__main__":
    unittest.main()
