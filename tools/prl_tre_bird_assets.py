#!/usr/bin/env python3
"""Restore APPROVED Team Rocket Edition Primal bird battle artwork on clean builds.

Donor: PJR_TRE_Primal_Sprite_Bank.zip (IDs 955 Articuno-P, 956 Zapdos-P,
957 Moltres-P). Source records are 64x64 exact indexed 4bpp battle graphics
plus RGB555-compatible palettes. Stdlib only; no species or battle changes.

The donor Icons/ files are NOT the birds (they show unrelated green creatures).
Leave valid existing Pokémon icons untouched pending an authentic matched set.
The donor Articuno/Moltres back art is identical to front; this is a documented
original-source limitation, not an edit or substituted illustration.
"""
import base64
import hashlib
import struct
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PAYLOAD = ROOT / "assets/prl_tre_birds/tre_primal_birds.b64"
BIRDS = ("articuno", "zapdos", "moltres")
SPRITE_SIZE = 64
FOURBPP_LENGTH = 64 * 64 // 2
PALETTE_LENGTH = 16 * 3
EXPECTED_SOURCE_BYTES = len(BIRDS) * (2 * FOURBPP_LENGTH + 2 * PALETTE_LENGTH)
EXPECTED_SHA256 = "011c0368c41940e2ef9deb7bd7247fed191ff3e9711808ce61012dc2a57008cf"


def png_chunk(tag, data):
    return (struct.pack(">I", len(data)) + tag + data
            + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF))


def untile(tiled):
    assert len(tiled) == FOURBPP_LENGTH
    indices = bytearray(64 * 64)
    for y in range(64):
        for x in range(64):
            tile = (y // 8) * 8 + x // 8
            src = tile * 32 + (y % 8) * 4 + (x % 8) // 2
            val = tiled[src]
            indices[y * 64 + x] = val >> 4 if x % 2 else val & 15
    return indices


def indexed_png(tiled, palette, frames=1):
    # 8-bit indexed PNG is required by gbagfx. Palette index zero transparent.
    assert len(palette) == PALETTE_LENGTH
    pixels = untile(tiled)
    height = SPRITE_SIZE * frames
    scan = bytearray()
    for y in range(height):
        scan.append(0)
        start = (y % SPRITE_SIZE) * SPRITE_SIZE
        scan.extend(pixels[start:start + SPRITE_SIZE])
    ihdr = struct.pack(">IIBBBBB", SPRITE_SIZE, height, 8, 3, 0, 0, 0)
    return (b"\x89PNG\r\n\x1a\n"
            + png_chunk(b"IHDR", ihdr)
            + png_chunk(b"PLTE", palette)
            + png_chunk(b"tRNS", bytes([0] + [255] * 15))
            + png_chunk(b"IDAT", zlib.compress(scan, 9))
            + png_chunk(b"IEND", b""))


def jasc(palette):
    colors = [palette[i:i + 3] for i in range(0, PALETTE_LENGTH, 3)]
    return ("JASC-PAL\n0100\n16\n"
            + "".join("%s %s %s\n" % tuple(x) for x in colors)).encode("ascii")


def write_if_changed(path, raw):
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.exists() or path.read_bytes() != raw:
        path.write_bytes(raw)


def restore():
    compressed = base64.b64decode(PAYLOAD.read_text("ascii").strip(), validate=True)
    data = zlib.decompress(compressed)
    if len(data) != EXPECTED_SOURCE_BYTES:
        raise ValueError("Unexpected TRE bird payload size")
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA256:
        raise ValueError("TRE bird donor digest changed")
    cursor = 0
    for species in BIRDS:
        raw_front = data[cursor:cursor + FOURBPP_LENGTH]
        cursor += FOURBPP_LENGTH
        raw_back = data[cursor:cursor + FOURBPP_LENGTH]
        cursor += FOURBPP_LENGTH
        pal = data[cursor:cursor + PALETTE_LENGTH]
        cursor += PALETTE_LENGTH
        shiny_pal = data[cursor:cursor + PALETTE_LENGTH]
        cursor += PALETTE_LENGTH
        directory = ROOT / "graphics/pokemon" / species
        front = indexed_png(raw_front, pal, frames=2)
        back = indexed_png(raw_back, pal)
        for path in ("anim_front.png", "anim_front_gba.png"):
            write_if_changed(directory / path, front)
        for path in ("back.png", "back_gba.png"):
            write_if_changed(directory / path, back)
        for path in ("normal.pal", "normal_gba.pal"):
            write_if_changed(directory / path, jasc(pal))
        for path in ("shiny.pal", "shiny_gba.pal"):
            write_if_changed(directory / path, jasc(shiny_pal))
    assert cursor == len(data)


if __name__ == "__main__":
    restore()
