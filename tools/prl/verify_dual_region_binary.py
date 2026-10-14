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
