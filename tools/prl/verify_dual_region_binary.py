#!/usr/bin/env python3
"""Validate real compiled Hoenn+Kanto map pointers, not just source strings."""
from pathlib import Path
import json
import re
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]
rom = (ROOT / "pokefirered.gba").read_bytes()
symbols = subprocess.check_output(["arm-none-eabi-nm", "pokefirered.elf"], cwd=ROOT, text=True)
symbol = {}
for line in symbols.splitlines():
    m = re.fullmatch(r"([0-9a-fA-F]{8})\s+\S\s+(\S+)", line)
    if m:
        symbol[m.group(2)] = int(m.group(1), 16)


def word(address):
    if not 0x08000000 <= address <= 0x08000000 + len(rom) - 4:
        raise AssertionError(f"out-of-ROM pointer: {address:#010x}")
    return struct.unpack_from("<I", rom, address - 0x08000000)[0]


def valid(name, ptr):
    assert 0x08000000 <= ptr < 0x08000000 + len(rom), f"{name} NULL or invalid: {ptr:#010x}"
    return ptr

groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
layouts = json.loads((ROOT / "data/layouts/layouts.json").read_text())
table = symbol["gMapGroups"]
layout_table = symbol["gMapLayouts"]

for group_name, map_name, expected_layout, emerald in [
    ("gMapGroup_TownsAndRoutes", "LittlerootTown", "LAYOUT_LITTLEROOT_TOWN", True),
    ("gMapGroup_TownsAndRoutes", "Route101", "LAYOUT_ROUTE101", True),
    ("gMapGroup_TownsAndRoutes", "OldaleTown", "LAYOUT_OLDALE_TOWN", True),
    ("gMapGroup_IndoorLittleroot", "LittlerootTown_BrendansHouse_1F",
     "LAYOUT_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F", True),
    ("gMapGroup_IndoorLittleroot", "LittlerootTown_MaysHouse_1F",
     "LAYOUT_LITTLEROOT_TOWN_MAYS_HOUSE_1F", True),
    ("gMapGroup_IndoorOldale", "OldaleTown_Mart", "LAYOUT_MART", True),
    ("gMapGroup_IndoorOldale", "OldaleTown_PokemonCenter_1F",
     "LAYOUT_POKEMON_CENTER_1F", True),
    ("gMapGroup_IndoorOldale", "OldaleTown_House1", "LAYOUT_HOUSE1", True),
    ("gMapGroup_IndoorOldale", "OldaleTown_House2", "LAYOUT_HOUSE2", True),
    ("gMapGroup_IndoorDynamic", "InsideOfTruck", "LAYOUT_INSIDE_OF_TRUCK", True),
    ("gMapGroup_IndoorLittleroot", "LittlerootTown_ProfessorBirchsLab",
     "LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB", True),
    ("gMapGroup_TownsAndRoutes_Frlg", "VermilionCity_Frlg", "LAYOUT_VERMILION_CITY", False),
]:
    group_index = groups["group_order"].index(group_name)
    map_index = groups[group_name].index(map_name)
    map_group = valid(group_name, word(table + group_index * 4))
    header = valid(map_name, word(map_group + map_index * 4))
    layout_id = next(i for i, x in enumerate(layouts["layouts"], 1) if x["id"] == expected_layout)
    layout = valid(expected_layout, word(layout_table + (layout_id - 1) * 4))
    assert word(header) == layout, f"{map_name} header layout mismatch"
    is_frlg = rom[layout - 0x08000000 + 0x18]
    assert is_frlg == (0 if emerald else 1), f"{map_name} wrong tileset mode {is_frlg}"
    primary = valid(map_name + " primary tileset", word(layout + 0x10))
    secondary = valid(map_name + " secondary tileset", word(layout + 0x14))
    valid(map_name + " primary gfx", word(primary + 4))
    valid(map_name + " secondary gfx", word(secondary + 4))
    print(f"PASS {map_name}: group {group_index}, map {map_index}, layout {layout_id}; "
          f"header={header:#x}, layout={layout:#x}, tilesets={primary:#x}/{secondary:#x}")

print("Compiled PRL both-region map tables valid.")

# PR102: verify newly opened family-house doors and Oldale, preserving original
# compiled map and warp indexes for older save files. Birch remains warp 2.
ROM_BASE = 0x08000000

def byte(address):
    valid("byte", address)
    return rom[address - ROM_BASE]


def header_for(group_name, map_name):
    group_no = groups["group_order"].index(group_name)
    map_no = groups[group_name].index(map_name)
    group_addr = valid(group_name, word(table + group_no * 4))
    return valid(map_name, word(group_addr + map_no * 4))


def warp_at(events, index):
    ptr = valid("map warps", word(events + 8))
    offset = ptr - ROM_BASE + index * 8
    return struct.unpack_from("<hhBBBB", rom, offset)


town = header_for("gMapGroup_TownsAndRoutes", "LittlerootTown")
route = header_for("gMapGroup_TownsAndRoutes", "Route101")
lab = header_for("gMapGroup_IndoorLittleroot", "LittlerootTown_ProfessorBirchsLab")
assert word(town + 12) != 0, "Littleroot should now connect north to compiled Route 101"
assert word(route + 12) != 0, "Route 101 should connect south to compiled Littleroot"
assert word(lab + 12) == 0, "Birch's lab should have no map connections"
def connection_dest(header, index):
    conn = valid("connection table", word(header + 12))
    count = word(conn)
    ptr = valid("connection entries", word(conn + 4))
    entry = ptr + 12 * index
    return count, (byte(entry + 8), byte(entry + 9))
route_group = groups["group_order"].index("gMapGroup_TownsAndRoutes")
town_map_num = groups["gMapGroup_TownsAndRoutes"].index("LittlerootTown")
route_map_num = groups["gMapGroup_TownsAndRoutes"].index("Route101")
assert connection_dest(town, 0) == (1, (route_group, route_map_num)), "Littleroot's only connection must target Route101"
oldale_map_num = groups["gMapGroup_TownsAndRoutes"].index("OldaleTown")
assert connection_dest(route, 0) == (2, (route_group, oldale_map_num)), "Route101 must connect north to Oldale"
assert connection_dest(route, 1) == (2, (route_group, town_map_num)), "Route101 must connect south to Littleroot"
oldale = header_for("gMapGroup_TownsAndRoutes", "OldaleTown")
# PR104: Route 102 opens west of Oldale. The compiled connection count
# increases from 1 to 2, while Route 101 remains at its original slot 0.
route102_map_num = groups["gMapGroup_TownsAndRoutes"].index("Route102")
assert connection_dest(oldale, 0) == (2, (route_group, route_map_num)), "Oldale south must still connect to Route101"
assert connection_dest(oldale, 1) == (2, (route_group, route102_map_num)), "Oldale west must connect to Route102"
route102 = header_for("gMapGroup_TownsAndRoutes", "Route102")
petalburg_map_num = groups["gMapGroup_TownsAndRoutes"].index("PetalburgCity")
assert connection_dest(route102, 0) == (2, (route_group, petalburg_map_num)), "Route102 west must connect to Petalburg"
assert connection_dest(route102, 1) == (2, (route_group, oldale_map_num)), "Route102 east must connect to Oldale"
town_events = valid("Littleroot events", word(town + 4))
lab_events = valid("Birch lab events", word(lab + 4))
if byte(town_events + 1) != 3:
    generated = ROOT / "data/maps/LittlerootTown/events.inc"
    if generated.exists():
        print("Generated Littleroot warp_defs:",
              [line.strip() for line in generated.read_text().splitlines()
               if "warp_def " in line])
    raise AssertionError(
        f"Littleroot compiled warp count {byte(town_events + 1)}, expected 3; "
        f"counts {[byte(town_events + i) for i in range(4)]}; "
        f"town={town:#x} events={town_events:#x} warp_ptr={word(town_events + 8):#x}")
assert byte(lab_events + 1) == 2, "Birch lab return warp count changed"
town_group = groups["group_order"].index("gMapGroup_TownsAndRoutes")
town_num = groups["gMapGroup_TownsAndRoutes"].index("LittlerootTown")
lab_group = groups["group_order"].index("gMapGroup_IndoorLittleroot")
lab_num = groups["gMapGroup_IndoorLittleroot"].index("LittlerootTown_ProfessorBirchsLab")
house_group = groups["group_order"].index("gMapGroup_IndoorLittleroot")
for index, map_name, coords in (
    (0, "LittlerootTown_MaysHouse_1F", (14, 8)),
    (1, "LittlerootTown_BrendansHouse_1F", (5, 8)),
):
    x, y, elevation, warp_id, map_num, group_num = warp_at(town_events, index)
    assert (x, y) == coords, f"house door {index} unexpectedly disabled"
    assert (group_num, map_num) == (
        house_group, groups["gMapGroup_IndoorLittleroot"].index(map_name)
    ), f"house door {index} points to wrong destination"
assert warp_at(town_events, 2)[4:6] == (lab_num, lab_group), "Birch lab entry changed"
for index in (0, 1):
    assert warp_at(lab_events, index)[4:6] == (town_num, town_group), "Birch lab return changed"

# Check every compiled map connection, including existing Kanto connections.
# A NULL map group, header or layout cannot be dereferenced during the map
# grid's border construction, even when the player never crosses the edge.
connection_count = 0
for group_index, group_name in enumerate(groups["group_order"]):
    group_ptr = word(table + group_index * 4)
    if group_ptr == 0:
        continue
    valid(group_name, group_ptr)
    for map_index, map_name in enumerate(groups[group_name]):
        map_ptr = word(group_ptr + map_index * 4)
        if map_ptr == 0:
            continue
        valid(map_name, map_ptr)
        conn_ptr = word(map_ptr + 12)
        if conn_ptr == 0:
            continue
        valid(map_name + " connections", conn_ptr)
        n = word(conn_ptr)
        assert 0 < n < 10, f"{map_name} invalid connection count {n}"
        entries = valid(map_name + " connection list", word(conn_ptr + 4))
        for i in range(n):
            # struct MapConnection: byte direction, padding, s32 offset,
            # byte mapGroup, byte mapNum, then padding (12 bytes total).
            dst_group, dst_map = (byte(entries + i * 12 + 8),
                                  byte(entries + i * 12 + 9))
            assert dst_group < len(groups["group_order"]), f"{map_name} bad destination group"
            dst_name = groups["group_order"][dst_group]
            assert dst_map < len(groups[dst_name]), f"{map_name} bad destination map"
            dst_group_ptr = valid(dst_name, word(table + dst_group * 4))
            dst_header = valid(groups[dst_name][dst_map],
                               word(dst_group_ptr + dst_map * 4))
            valid(groups[dst_name][dst_map] + " destination layout", word(dst_header))
            connection_count += 1

# Prevent regressing the save/respawn and Fly guards in a later PR.
region_map_source = (ROOT / "src/region_map.c").read_text()
transfer_source = (ROOT / "src/field_specials.c").read_text()
arrival_source = (ROOT / "data/maps/LittlerootTown/scripts.inc").read_text()
assert "PRLIsUnavailableHoennFlyMapSec" in region_map_source
assert "return WARP_ID_NONE;" in region_map_source.split("case MAPSEC_LITTLEROOT_TOWN:", 2)[-1]
assert "lastHealLocation.mapGroup = MAP_GROUP(MAP_LITTLEROOT_TOWN)" in transfer_source
arrival = arrival_source.split("LittlerootTown_EventScript_PRLOutdoorDockArrival::", 1)[1].split("LittlerootTown_Text_PRLSafeHoennArrival::", 1)[0]
assert "setvar VAR_LITTLEROOT_INTRO_STATE, 0" in arrival
assert "setvar VAR_LITTLEROOT_TOWN_STATE, 0" in arrival
assert "setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 0" in arrival

# Keep all script warps within reviewed destinations. The family houses are
# now enabled in PR102, though the original moving-truck house cutscene is
# still bypassed; Route 103 and Route 102 stay closed on this slice.
script_warp = re.compile(r"\b(?:warp|warpsilent|setwarp|setdynamicwarp|setescapewarp|setholewarp)\s+(MAP_[A-Z0-9_]+)")
town_scripts = (ROOT / "data/maps/LittlerootTown/scripts.inc").read_text()
lab_scripts = (ROOT / "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc").read_text()
town_targets = set(script_warp.findall(town_scripts))
lab_targets = set(script_warp.findall(lab_scripts))
assert town_targets <= {
    "MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB",
    "MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F",  # original Emerald truck cutscene
    "MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F",      # original Emerald truck cutscene
}, f"unsafe Littleroot script warp: {town_targets}"
# PR99 intentionally reloads this *same enabled map* once on starter
# confirmation to discard already-rendered old starter-ball metatiles.
# Keep every other Hoenn destination forbidden and require the reload to
# be scoped to the successful starter-completion script.
safe_lab_warps = {"MAP_VERMILION_CITY", "MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB"}
assert lab_targets <= safe_lab_warps, f"unsafe Birch lab script warp: {lab_targets}"
if "MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB" in lab_targets:
    complete = lab_scripts.split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete::", 1)[1]
    complete = complete.split("LittlerootTown_ProfessorBirchsLab_Text_PRLHoennWelcome::", 1)[0]
    required = (
        "setvar VAR_PRL_REGION_STAGE, 2",
        "setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB",
        "setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_CYNDAQUIL",
        "setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_TOTODILE",
        "setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_CHIKORITA",
        "warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 7, 5",
        "waitstate",
    )
    assert all(token in complete for token in required), "unsafe starter-room reload sequence"
    assert lab_scripts.count("warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB,") == 1, "unreviewed extra lab reload"
assert not (byte(town + 0x1A) & 0x02), "Dig/Escape Rope unexpectedly enabled in town"
assert not (byte(lab + 0x1A) & 0x02), "Dig/Escape Rope unexpectedly enabled in lab"
route_source = (ROOT / "data/maps/Route101/scripts.inc").read_text()
assert "Route101_EventScript_PRLRescueAlreadyComplete::" in route_source
assert "setvar VAR_ROUTE101_STATE, 3" in route_source
for prop in ("FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE", "FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG",
             "FLAG_HIDE_ROUTE_101_ZIGZAGOON", "FLAG_HIDE_ROUTE_101_BIRCH"):
    assert "setflag " + prop in route_source
print(f"PASS PR94 bridge: Littleroot/Route101 connected, Oldale remains disabled, rescue bypassed; checked {connection_count} compiled connections.")
