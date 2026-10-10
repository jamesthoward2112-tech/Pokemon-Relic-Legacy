#!/usr/bin/env python3
"""Reconstruct approved Pokémon Mercury *art only* from verified raw GBA donor data.

The checked-in compact source records all three front/back sprites and normal/
shiny palettes. This script uses stdlib only and generates the exact 64x64
rendered pixels of the user's Mercury donor set at build time. No species or
Gigantamax mechanics are introduced here.
"""
import base64
import struct
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "assets/prl_mercury/mercury_sprite_payload.b64"
SPRITES = (
    ("Blastoise", "graphics/pokemon/fortotoise"),
    ("Charizard", "graphics/pokemon/charaxis"),
    ("Eevee", "graphics/pokemon/prl_eevee_twins"),
)
SIZE = 64
RAW_SPRITE_BYTES = SIZE * SIZE // 2
RAW_PALETTE_BYTES = 16 * 2


def rgb555_palette(raw):
    assert len(raw) == RAW_PALETTE_BYTES
    return [
        tuple(round(((value >> shift) & 31) * 255 / 31) for shift in (0, 5, 10))
        for (value,) in struct.iter_unpack("<H", raw)
    ]


def png_chunk(tag, data):
    return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)


def make_indexed_png(tiled_4bpp, colors):
    # gbagfx requires 8-bit indexed PNG (colour type 3), not RGBA (type 6).
    # Each source nibble is an *exact* index into the accompanying RGB555 pal.
    assert len(tiled_4bpp) == RAW_SPRITE_BYTES and len(colors) == 16
    scanlines = bytearray()
    for y in range(SIZE):
        scanlines.append(0)  # PNG filter type 0
        for x in range(SIZE):
            tile = (y // 8) * (SIZE // 8) + x // 8
            n = tile * 32 + (y % 8) * 4 + (x % 8) // 2
            index = (tiled_4bpp[n] >> 4) if (x & 1) else (tiled_4bpp[n] & 15)
            scanlines.append(index)
    header = struct.pack(">IIBBBBB", SIZE, SIZE, 8, 3, 0, 0, 0)
    palette = b"".join(bytes(rgb) for rgb in colors)
    return (b"\x89PNG\r\n\x1a\n"
            + png_chunk(b"IHDR", header)
            + png_chunk(b"PLTE", palette)
            + png_chunk(b"tRNS", bytes((0,)) + bytes((255,)) * 15)
            + png_chunk(b"IDAT", zlib.compress(scanlines, level=9))
            + png_chunk(b"IEND", b""))


def jasc_palette(colors):
    return ("JASC-PAL\n0100\n16\n" +
            "".join("%d %d %d\n" % rgb for rgb in colors)).encode("ascii")


def write_changed(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.is_file() or path.read_bytes() != data:
        path.write_bytes(data)


def materialize():
    payload = SOURCE.read_text(encoding="ascii").strip()
    raw = zlib.decompress(base64.b64decode(payload, validate=True))
    expected = len(SPRITES) * (2 * RAW_SPRITE_BYTES + 2 * RAW_PALETTE_BYTES)
    if len(raw) != expected:
        raise ValueError("Unexpected Mercury source length: %d != %d" % (len(raw), expected))

    offset = 0
    for donor_name, destination in SPRITES:
        fronts = []
        for _ in range(2):
            fronts.append(raw[offset:offset + RAW_SPRITE_BYTES])
            offset += RAW_SPRITE_BYTES
        palettes = []
        for _ in range(2):
            palettes.append(rgb555_palette(raw[offset:offset + RAW_PALETTE_BYTES]))
            offset += RAW_PALETTE_BYTES

        out = ROOT / destination
        for part, pixels in zip(("front", "back"), fronts):
            write_changed(out / (part + ".png"), make_indexed_png(pixels, palettes[0]))
        write_changed(out / "normal.pal", jasc_palette(palettes[0]))
        write_changed(out / "shiny.pal", jasc_palette(palettes[1]))

    assert offset == len(raw)


if __name__ == "__main__":
    materialize()
