"""Regression checks for the approved PKR Safari and Cerulean Cave changes."""
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def read(path): return (ROOT / path).read_text(encoding="utf-8")

def test_safari_entry():
    code = read("src/safari_zone.c")
    entrance = read("data/maps/FuchsiaCity_SafariZone_Entrance_Frlg/scripts.inc")
    assert "gNumSafariBalls = IS_FRLG ? 99 : 30;" in code
    assert "gSafariZoneStepCounter = 996;" in code
    assert "received 99 SAFARI BALLS" in entrance
    assert "996 steps to explore" in entrance

def test_safari_catch_and_kangaskhan():
    assert "(IS_FRLG && (gBattleTypeFlags & BATTLE_TYPE_SAFARI)) ? 20" in read("src/battle_main.c")
    wild = json.loads(read("src/data/wild_encounters.json"))
    checked = []
    for g in wild["wild_encounter_groups"]:
        land = next((f for f in g.get("fields", []) if f["type"] == "land_mons"), None)
        for row in g.get("encounters", []):
            if row.get("base_label") not in ("sSafariZoneWest_FireRed","sSafariZoneWest_LeafGreen"): continue
            rate = sum(land["encounter_rates"][i] for i,mon in enumerate(row["land_mons"]["mons"]) if mon["species"]=="SPECIES_KANGASKHAN")
            assert rate == 7
            checked.append(row["base_label"])
    assert len(checked) == 2

def test_extra_silk_scarf():
    objs = json.loads(read("data/maps/SafariZone_West_Frlg/map.json"))["object_events"]
    assert not any(x["script"] == "SafariZone_West_EventScript_ItemSilkScarf" for x in objs)
    assert "SafariZone_West_EventScript_ItemSilkScarf::" not in read("data/scripts/item_ball_scripts_frlg.inc")
    assert "FLAG_HIDE_PRL_SAFARI_WEST_SILK_SCARF" in read("include/constants/flags_frlg.h")

def test_first_champion_victory_opens_cave():
    city = read("data/maps/CeruleanCity_Frlg/scripts.inc")
    assert "call_if_set FLAG_SYS_GAME_CLEAR, CeruleanCity_EventScript_RemoveCaveGuard" in city
    assert "CeruleanCity_EventScript_RemoveCaveGuard::\n\tsetflag FLAG_HIDE_CERULEAN_CAVE_GUARD\n\treturn" in city


def test_kanto_one_save_version_exclusive_encounters():
    data = json.loads(read("src/data/wild_encounters.json"))
    wanted = {
        "SANDSHREW": "MAP_ROUTE4",
        "BELLSPROUT": "MAP_ROUTE5",
        "VULPIX": "MAP_ROUTE7",
        "SLOWPOKE": "MAP_ROUTE6",
        "STARYU": "MAP_PALLET_TOWN",
        "PINSIR": "MAP_SAFARI_ZONE_EAST",
        "MAGMAR": "MAP_POKEMON_MANSION_1F",
        "PORYGON": "MAP_POWER_PLANT",
        "TAUROS": "MAP_ROUTE13",
    }
    seen = set()
    for group in data["wild_encounter_groups"]:
        for e in group.get("encounters", []):
            if not e.get("base_label", "").endswith("_FireRed"):
                continue
            for container in ("land_mons", "water_mons", "fishing_mons"):
                for mon in e.get(container, {}).get("mons", []):
                    key = mon["species"].removeprefix("SPECIES_")
                    if wanted.get(key) == e["map"]:
                        seen.add(key)
    assert seen == set(wanted), sorted(set(wanted) - seen)


def test_legendaries_return_when_defeated_until_caught():
    for p,name,flag in (
        ("data/maps/PowerPlant_Frlg/scripts.inc", "Zapdos", "FLAG_FOUGHT_ZAPDOS"),
        ("data/maps/MtEmber_Summit_Frlg/scripts.inc", "Moltres", "FLAG_FOUGHT_MOLTRES"),
        ("data/maps/CeruleanCave_B1F_Frlg/scripts.inc", "Mewtwo", "FLAG_FOUGHT_MEWTWO"),
        ("data/maps/SeafoamIslands_B4F_Frlg/scripts.inc", "Articuno", "FLAG_FOUGHT_ARTICUNO"),
    ):
        script = read(p)
        defeated = script.split("EventScript_Defeated" + name + "::", 1)[1].split("\n\tend", 1)[0]
        assert "goto EventScript_RemoveStaticMon" in defeated
        assert "setflag " + flag not in defeated
        assert "call_if_unset " + flag in script
