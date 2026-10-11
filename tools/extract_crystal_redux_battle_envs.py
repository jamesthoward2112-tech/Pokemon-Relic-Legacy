#!/usr/bin/env python3
"""Extract original Crystal Advance Redux battle environments into PRL source assets.

Usage: python3 tools/extract_crystal_redux_battle_envs.py "Crystal Redux.gba"

This extracts art only; it never changes the donor ROM or modifies PRL saves,
battle rules or the SELECT selector.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import struct
from pathlib import Path

TABLE = 0x4090F0
COUNT = 38
EXPECTED_SHA256 = "242e40954879f61ca95897f6880009bf65d1c30d07e8086658f7019f46024720"
NAMES = ("tiles.4bpp.lz", "tilemap.bin.lz", "entry_tiles.4bpp.lz",
         "entry_tilemap.bin.lz", "palette.gbapal.lz")


def decompress(rom: bytes, pos: int) -> tuple[bytes, bytes]:
    assert rom[pos] == 0x10, f"Expected GBA LZ77 at 0x{pos:X}"
    size = rom[pos + 1] | rom[pos + 2] << 8 | rom[pos + 3] << 16
    assert 0 < size < 0x100000, (pos, size)
    p = pos + 4
    output = bytearray()
    while len(output) < size:
        flags = rom[p]
        p += 1
        for bit in range(8):
            if flags & (0x80 >> bit):
                hi, lo = rom[p:p+2]
                p += 2
                count = (hi >> 4) + 3
                distance = (((hi & 15) << 8) | lo) + 1
                assert distance <= len(output)
                for _ in range(count):
                    output.append(output[-distance])
            else:
                output.append(rom[p])
                p += 1
            if len(output) >= size:
                break
    return bytes(output[:size]), rom[pos:p]


def extract(donor: Path, output: Path) -> None:
    rom = donor.read_bytes()
    assert len(rom) >= 0x2000000, "Expected a full 32 MiB GBA ROM"
    sha = hashlib.sha256(rom).hexdigest()
    if sha != EXPECTED_SHA256:
        raise ValueError(f"Wrong donor ROM SHA-256: {sha}. No assets extracted.")
    output.mkdir(parents=True, exist_ok=True)
    records = []
    for i in range(COUNT):
        ptrs = struct.unpack_from("<5I", rom, TABLE + 20 * i)
        offsets = [p - 0x08000000 for p in ptrs]
        assert all(0 < p < len(rom) for p in offsets)
        parts = [decompress(rom, p) for p in offsets]
        raw = [entry[0] for entry in parts]
        assert len(raw[1]) == 4096 and len(raw[3]) == 896
        assert len(raw[4]) in (32, 64, 96)
        folder = output / f"{i:02d}"
        folder.mkdir(exist_ok=True)
        for name, (_, compressed) in zip(NAMES, parts):
            (folder / name).write_bytes(compressed)
        for name, data in zip(("tiles.4bpp", "tilemap.bin",
                               "entry_tiles.4bpp", "entry_tilemap.bin"), raw[:4]):
            (folder / name).write_bytes(data)
        palette = raw[4]
        while len(palette) < 96:
            palette += raw[4][:min(32, 96-len(palette))]
        (folder / "palette_48colors.gbapal").write_bytes(palette)
        records.append({
            "index": i, "table_offset": hex(TABLE + 20*i),
            "asset_rom_offsets": list(map(hex, offsets)),
            "decompressed_lengths": list(map(len, raw)),
            "compressed_lengths": [len(part[1]) for part in parts],
        })
    (output / "manifest.json").write_text(json.dumps({
        "donor_sha256": sha, "count": COUNT,
        "palette_note": "Tilemap palette banks start at 2; preserve Nature Power/Camouflage terrain independently.",
        "entries": records
    }, indent=2))
    print(f"Verified and extracted {COUNT} Crystal Redux environments to {output}")


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("donor_rom", type=Path)
    p.add_argument("--output", type=Path, default=Path("graphics/prl_crystal_redux"))
    a = p.parse_args()
    extract(a.donor_rom, a.output)
