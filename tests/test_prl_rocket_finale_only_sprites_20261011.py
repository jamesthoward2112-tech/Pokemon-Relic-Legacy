"""Jessie & James sprite progression: ordinary Pokémon art before Victory Road.

Source-level checks; the native trainer party, save and move data are unchanged.
Emulator smoke tests remain required before declaring the visuals playtested.
"""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
MAP = ROOT / "data/maps/RocketHideout_B4F_Frlg/map.json"
GFX = ROOT / "src/battle_gfx_sfx_util.c"

class PRLRocketFinaleOnly(unittest.TestCase):
    def test_b4f_trio_all_shift_down_one_tile_without_other_map_edits(self):
        map_data = json.loads(MAP.read_text(encoding="utf-8"))
        expected = {
            "LOCALID_PRL_JESSIE_HIDEOUT": (15, 20),
            "LOCALID_PRL_JAMES_HIDEOUT": (15, 21),
            "LOCALID_PRL_MEOWTH_HIDEOUT": (14, 21),
        }
        actual = {o.get("local_id"): o for o in map_data["object_events"]
                  if o.get("local_id") in expected}
        self.assertEqual(set(actual), set(expected))
        for label, coords in expected.items():
            with self.subTest(actor=label):
                npc = actual[label]
                self.assertEqual((npc["x"],npc["y"]), coords)
                self.assertEqual(npc["movement_type"], "MOVEMENT_TYPE_FACE_RIGHT")
                self.assertEqual(npc["flag"], "FLAG_HIDE_PRL_ROCKET_HIDEOUT")
                if "MEOWTH" not in label:
                    self.assertEqual(npc["trainer_type"], "TRAINER_TYPE_NORMAL")
                    self.assertEqual(npc["trainer_sight_or_berry_tree_id"], "4")
        self.assertEqual(map_data["coord_events"], [])
        self.assertEqual(len({(o["x"],o["y"]) for o in map_data["object_events"]}),
                         len(map_data["object_events"]))

    def test_weezing_galarian_art_is_final_encounter_only(self):
        text = GFX.read_text(encoding="utf-8")
        block = text.split("static bool32 PRLIsJessieJamesWeezingVisual(",1)[1].split("enum PRLRocketDarkGfx",1)[0]
        self.assertIn("species != SPECIES_WEEZING",block)
        self.assertIn("IsOnPlayerSide(battler)",block)
        self.assertRegex(block, r"TRAINER_BATTLE_PARAM\.opponentA\s*==\s*TRAINER_JESSIE_JAMES_VICTORY_ROAD")
        for stage in ("MT_MOON","HIDEOUT","SILPH"):
            self.assertNotIn("TRAINER_JESSIE_JAMES_"+stage,block)
        self.assertIn("prlRocketWeezing ? SPECIES_WEEZING_GALAR : species",text)

    def test_rocket_dark_forms_are_final_encounter_only(self):
        text = GFX.read_text(encoding="utf-8")
        block = text.split("static u8 PRLGetRocketDarkGfx(",1)[1].split("static const u32 *PRLGetRocketDarkFront(",1)[0]
        self.assertRegex(block, r"TRAINER_BATTLE_PARAM\.opponentA\s*!=\s*TRAINER_JESSIE_JAMES_VICTORY_ROAD")
        for stage in ("MT_MOON","HIDEOUT","SILPH"):
            self.assertNotIn("TRAINER_JESSIE_JAMES_"+stage,block)
        for species in ("ARBOK","RATICATE","VICTREEBEL","GYARADOS"):
            self.assertIn("SPECIES_"+species,block)
            self.assertIn("PRL_ROCKET_DARK_"+species,block)
        self.assertIn("PRLGetRocketDarkFront(prlRocketDarkGfx)",text)
        self.assertIn("PRLGetRocketDarkPalette(prlRocketDarkGfx)",text)

    def test_regular_species_and_existing_pokemon_data_remain_unmodified(self):
        party = (ROOT / "src/data/trainers_frlg.party").read_text()
        for name in ("JESSIE_JAMES_HIDEOUT","JESSIE_JAMES_SILPH","JESSIE_JAMES_VICTORY_ROAD"):
            self.assertIn("=== TRAINER_"+name+" ===",party)
        self.assertIn("Arbok @ Oran Berry",party)
        self.assertIn("Weezing @ Oran Berry",party)
        self.assertIn("Meowth",party)
        self.assertIn("Weezing @ Black Sludge",party)

if __name__=="__main__":
    unittest.main()
