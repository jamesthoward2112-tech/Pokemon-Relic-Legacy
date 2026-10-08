"""Presentation integration gates for the three omissions found by player QA.

The approved binary assets are independent fixtures, recovered from the prior
PRL foundation and the saved title transfer. These are integrity/wiring checks,
not a claim that the player has completed gameplay QA.
"""
import hashlib
import re
import struct
import subprocess
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8-sig")


def body(source, name):
    match = re.search(r"\b" + re.escape(name) + r"\([^;{}]*\)\s*\{", source)
    if match is None:
        return ""
    start = match.end()
    depth = 1
    for pos in range(start, len(source)):
        depth += (source[pos] == "{") - (source[pos] == "}")
        if depth == 0:
            return source[start:pos]
    raise AssertionError("Unclosed function: " + name)


def indexed_png(data):
    """Decode the actual indexed PNG pixels used by the graphics converter."""
    pos, compressed, alpha = 8, b"", b""
    while pos < len(data):
        size = struct.unpack(">I", data[pos:pos + 4])[0]
        tag, chunk = data[pos + 4:pos + 8], data[pos + 8:pos + 8 + size]
        pos += 12 + size
        if tag == b"IHDR":
            width, height, bits, kind, _, _, interlace = struct.unpack(">IIBBBBB", chunk)
            assert (bits, kind, interlace) == (8, 3, 0)
        elif tag == b"PLTE":
            palette = [tuple(chunk[i:i + 3]) for i in range(0, len(chunk), 3)]
        elif tag == b"tRNS":
            alpha = chunk
        elif tag == b"IDAT":
            compressed += chunk
    raw, previous, pixels = zlib.decompress(compressed), [0] * width, []
    for y in range(height):
        start = y * (width + 1)
        mode, row = raw[start], list(raw[start + 1:start + 1 + width])
        for x in range(width):
            a, b, c = row[x - 1] if x else 0, previous[x], previous[x - 1] if x else 0
            if mode == 4:
                p = a + b - c
                predictor = min((a, b, c), key=lambda v: abs(p - v))
            else:
                predictor = (0, a, b, (a + b) // 2)[mode]
            row[x] = (row[x] + predictor) & 255
        pixels.extend(row)
        previous = row
    return width, height, palette, alpha, pixels


class PresentationIntegrationTests(unittest.TestCase):
    def contains(self, expected, text):
        self.assertTrue(expected in text, "Missing presentation integration: " + expected)

    def test_firered_entrypoint_enters_prl_title_before_stock_title(self):
        source = read("src/title_screen_frlg.c")
        self.assertTrue(re.search(r"^\s*#if defined\(FIRERED\)\s*InitPRLTitleScreen\(\);\s*return;\s*#endif",
                                  body(source, "CB2_InitTitleScreenFrlg")), "FireRed entrypoint bypasses PRL title")
        init = body(source, "InitPRLTitleScreen")
        self.contains("DISPCNT_MODE_3 | DISPCNT_BG2_ON", init)
        self.contains("CpuCopy16(sPRLTitleMode3, (void *)VRAM, sizeof(sPRLTitleMode3))", init)
        self.contains("PRLDrawPrompt(TRUE)", init)
        self.contains("gTasks[taskId].data[1] = TRUE", init)
        self.contains("SetMainCallback2(CB2_PRLTitleRun)", init)
        self.assertNotIn("PRLBlendTitlePixel", source, "Do not reinstate palette smoothing")

    def test_title_loader_consumes_final_option_b_rgb555_assets(self):
        source = read("src/title_screen_frlg.c")
        fixtures = {
            "sPRLTitleMode3": ("prl_option_b_title_mode3.bin", 76800,
                              "61d225c91f0c9139a9c02720b2df20d1e7eb37f20ee924ae7e69c0061d25b744"),
            "sPRLTitlePromptOff": ("prl_option_b_prompt_off_mode3.bin", 9600,
                                  "3df3735b0a4e97424225045028bb2b455bd176fbb0465c65263ef8199d4fcf4c"),
        }
        for symbol, (filename, size, digest) in fixtures.items():
            with self.subTest(asset=filename):
                path = "graphics/title_screen_prl/" + filename
                self.contains(f'{symbol}[] = INCBIN_U16("{path}")', source)
                data = (ROOT / path).read_bytes()
                self.assertEqual(len(data), size)
                self.assertEqual(hashlib.sha256(data).hexdigest(), digest)

    def test_prompt_pixel_bounds_are_inside_the_ornate_box(self):
        full = (ROOT / "graphics/title_screen_prl/prl_option_b_title_mode3.bin").read_bytes()
        off = (ROOT / "graphics/title_screen_prl/prl_option_b_prompt_off_mode3.bin").read_bytes()
        on_pixels = struct.unpack("<4800H", full[140 * 240 * 2:])
        off_pixels = struct.unpack("<4800H", off)
        changed = [(i % 240, i // 240 + 140) for i, (a, b)
                   in enumerate(zip(on_pixels, off_pixels)) if a != b]
        self.assertTrue(changed, "PRESS START is missing")
        self.assertGreaterEqual(min(x for x, y in changed), 85)
        self.assertLessEqual(max(x for x, y in changed), 155)
        self.assertGreaterEqual(min(y for x, y in changed), 144)
        self.assertLessEqual(max(y for x, y in changed), 152)

    def test_title_prompt_blinks_and_start_reaches_existing_menu(self):
        source = read("src/title_screen_frlg.c")
        task = body(source, "Task_PRLTitle")
        self.contains("JOY_NEW(A_BUTTON | START_BUTTON)", task)
        self.contains("SetMainCallback2(CB2_InitMainMenu)", task)
        self.contains("if (++data[0] >= 40)", task)
        self.contains("PRLDrawPrompt(data[1])", task)
        prompt = body(source, "PRLDrawPrompt")
        self.contains("sPRLTitleMode3", prompt)
        self.contains("sPRLTitlePromptOff", prompt)
        self.contains("PRL_TITLE_PROMPT_Y * DISPLAY_WIDTH", prompt)
        self.contains("sizeof(sPRLTitlePromptOff)", prompt)
        self.contains("RunTasks()", body(source, "CB2_PRLTitleRun"))

    def test_oak_intro_uses_vanilla_firered_oak(self):
        source = read("src/oak_speech.c")
        self.contains('sOakSpeech_Oak_Pal[] = INCGFX_U16("graphics/oak_speech/oak/pal.pal", ".gbapal")', source)
        self.contains('sOakSpeech_Oak_Tiles[] = INCGFX_U32("graphics/oak_speech/oak/pic.png", ".8bpp.smol")', source)
        self.assertFalse((ROOT / "graphics/oak_speech/oak/archoak_approved.png").exists(),
                         "ArchOak fixture must not return after the player reverted to vanilla Oak")
        blob = subprocess.check_output(
            ["git", "hash-object", "graphics/oak_speech/oak/pic.png"],
            cwd=ROOT,
            text=True,
        ).strip()
        self.assertEqual(blob, "2fb9cc863f487fb1e4fdc36f0310c6a3d0789785",
                         "Oak intro picture is not the vanilla FireRed Oak asset")
        lines = read("graphics/oak_speech/oak/pal.pal").splitlines()
        self.assertEqual(lines[:3], ["JASC-PAL", "0100", "32"])

    def test_omanyte_is_used_for_intro_picture_palette_release_and_cry(self):
        source = read("src/oak_speech.c")
        self.assertTrue(re.search(r"(?m)^#define INTRO_SPECIES SPECIES_OMANYTE$", source), "Oak intro no longer selects Omanyte")
        for call in ("LoadSpecialPokePic", "GetMonSpritePalFromSpeciesAndPersonality",
                     "SetMultiuseSpriteTemplateToPokemon", "CreatePokeballSpriteToReleaseMon", "PlayCry_Normal"):
            lines = [line for line in source.splitlines() if call + "(" in line]
            self.assertTrue(any("INTRO_SPECIES" in line for line in lines), call)

    def test_approved_oak_dialogue_is_preserved_byte_for_byte(self):
        data = (ROOT / "data/text/new_game_intro_frlg.inc").read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(), "94f8f711b1aa28e4ed0de64a82378c2b4a731f37b05e79ad0e0ba053d885d63c")


if __name__ == "__main__":
    unittest.main()
