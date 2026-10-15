"""PR104: early Hoenn route and first three Birch-aide Pokémon donors.

Build/source checks are not gameplay verification. Route 104, Rustboro, and
Dewford remain closed in PR104 until their connecting map and event scripts are
migrated and emulator-tested; donor scripts/sprites are staged there now.
"""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(p):
    return (ROOT / p).read_text(encoding="utf-8")


class TestPR104(unittest.TestCase):
    def test_true_region_name_on_hoenn_map(self):
        src = read("src/field_region_map.c")
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 2", src)
        self.assertIn("region = gText_Kanto;", src)
        self.assertIn("region = gText_Hoenn;", src)

    def test_gifts_are_level_ten_with_items_and_species_overworld_sprites(self):
        gifts = [
            ("PetalburgCity", "Rowan", "TREECKO", "MIRACLE_SEED", "FLAG_PRL_HOENN_ROWAN_TREECKO_GIFT"),
            ("RustboroCity", "Kiri", "TORCHIC", "CHARCOAL", "FLAG_PRL_HOENN_KIRI_TORCHIC_GIFT"),
            ("DewfordTown", "Neris", "MUDKIP", "MYSTIC_WATER", "FLAG_PRL_HOENN_NERIS_MUDKIP_GIFT"),
        ]
        flags = read("include/constants/flags.h")
        for town, name, species, item, flag in gifts:
            code = read(f"data/maps/{town}/scripts.inc")
            objects = json.loads(read(f"data/maps/{town}/map.json"))["object_events"]
            sprite = next(o for o in objects if o.get("local_id") == f"LOCALID_PRL_HOENN_{species}")
            self.assertEqual(sprite["graphics_id"], f"OBJ_EVENT_GFX_SPECIES({species})")
            self.assertEqual(sprite["flag"], flag)
            self.assertIn(f"givemon SPECIES_{species}, 10, ITEM_{item}", code)
            self.assertIn(f"setflag {flag}", code)
            self.assertIn(f"hideobjectat LOCALID_PRL_HOENN_{species}", code)
            self.assertIn(f"MON_GIVEN_TO_PC", code)
            self.assertNotIn("FLAG_DEFEATED_", code.split(f"{town}_EventScript_PRL{name}::", 1)[1].split(f"{town}_EventScript_PRL{name}Done::", 1)[0])
            self.assertIn(flag, flags)
            self.assertIn(f"{town}_Text_PRL{name}After::", code)

    def test_safe_path_from_oldale_to_petalburg(self):
        gen = read("tools/mapjson/mapjson.cpp")
        for name in ("Route102", "PetalburgCity", "PetalburgCity_Mart", "PetalburgCity_PokemonCenter_1F"):
            self.assertIn(f'"{name}"', gen)
        for name in ("Route102", "PetalburgCity", "OldaleTown"):
            obj = json.loads(read(f"data/maps/{name}/map.json"))
            self.assertIsNotNone(obj.get("connections"))
        oldale = json.loads(read("data/maps/OldaleTown/map.json"))
        self.assertIn("MAP_ROUTE102", [c["map"] for c in oldale["connections"]])
        petalburg = json.loads(read("data/maps/PetalburgCity/map.json"))
        self.assertIn("MAP_ROUTE102", [c["map"] for c in petalburg["connections"]])
        self.assertIn('!prlHoennDestinationEnabled(destination)', gen)
        self.assertNotIn('"Route104", "PetalburgWoods"', gen)

    def test_house_ghosts_hidden_and_rival_house_not_duplicate_aunt(self):
        for name in ("Brendans", "Mays"):
            s = read(f"data/maps/LittlerootTown_{name}House_1F/scripts.inc")
            normalize = s.split(f"LittlerootTown_{name}House_1F_EventScript_PRLMakeAuntAvailable::",1)[1].split(f"LittlerootTown_{name}House_1F_EventScript_PRLAunt::",1)[0]
            for flag in ("FLAG_HIDE_PLAYERS_HOUSE_DAD", "FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_1","FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_2"):
                self.assertIn(f"setflag {flag}",normalize)
            self.assertIn("PRLRosiesHouse::",normalize)
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_",normalize)
            self.assertIn("clearflag FLAG_HIDE_LITTLEROOT_TOWN_",normalize)


if __name__ == "__main__":
    unittest.main()
