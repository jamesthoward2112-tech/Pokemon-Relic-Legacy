"""Presentation integration gates for the three omissions found by player QA.

The approved binary assets are independent fixtures, recovered from the prior
PRL foundation and the saved title transfer. These are integrity/wiring checks,
not a claim that the player has completed gameplay QA.
"""
import hashlib
import re
import struct
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
        self.contains("DISPCNT_MODE_4 | DISPCNT_BG2_ON", init)
        self.contains("CpuCopy16(sPRLTitleBitmap, (void *)VRAM, sizeof(sPRLTitleBitmap))", init)
        self.contains("LoadPalette(sPRLTitlePalette, 0, sizeof(sPRLTitlePalette))", init)
        self.contains("SetMainCallback2(CB2_PRLTitleRun)", init)

    def test_title_loader_consumes_exact_approved_art_and_palette(self):
        source = read("src/title_screen_frlg.c")
        fixtures = {
            "sPRLTitlePalette": ("prl_title.pal", 512, "84be24641ff4b5916908d9b3bd1041bc16d2805f9c28e2b9902828e0f58481f0"),
            "sPRLTitleBitmap": ("prl_title_bitmap.bin", 38400, "088aa0cf45bc7138410f2d511a9101237935f6f2de7187e6455fb04c53f1b6b5"),
            "sPRLTitlePressBase": ("prl_title_press_base.bin", 4800, "acc4757de4264e2288e26e3f60ae60a3632f2e771f103c55059e160cac779b41"),
            "sPRLTitlePressOn": ("prl_title_press_on.bin", 4800, "c0c26fdf2b5bbbf3992a7b33a90b2a2851740640ea1c0baeef56b349b3f913ea"),
        }
        for symbol, (filename, size, digest) in fixtures.items():
            with self.subTest(asset=filename):
                path = "graphics/title_screen_prl/" + filename
                self.contains(f'{symbol}[] = INCBIN_U16("{path}")', source)
                self.assertTrue((ROOT / path).is_file(), "Missing approved title asset: " + path)
                data = (ROOT / path).read_bytes()
                self.assertEqual(len(data), size)
                self.assertEqual(hashlib.sha256(data).hexdigest(), digest)

    def test_title_prompt_blinks_and_start_reaches_existing_menu(self):
        source = read("src/title_screen_frlg.c")
        task = body(source, "Task_PRLTitle")
        self.contains("JOY_NEW(A_BUTTON | START_BUTTON)", task)
        self.contains("SetMainCallback2(CB2_InitMainMenu)", task)
        self.contains("if (++data[0] >= 32)", task)
        for symbol in ("sPRLTitlePressOn", "sPRLTitlePressBase"):
            self.contains(f"CpuCopy16({symbol}, (u8 *)VRAM + (140 * DISPLAY_WIDTH), sizeof({symbol}))", task)
        self.contains("RunTasks()", body(source, "CB2_PRLTitleRun"))

    def test_oak_intro_loads_archoak_with_its_own_palette(self):
        source = read("src/oak_speech.c")
        self.contains('sOakSpeech_Oak_Pal[] = INCGFX_U16("graphics/oak_speech/oak/pal.pal", ".gbapal")', source)
        self.contains('sOakSpeech_Oak_Tiles[] = INCGFX_U32("graphics/oak_speech/oak/pic.png", ".8bpp.smol")', source)
        oak_case = body(source, "LoadTrainerPic").split("case OAK_PIC:")[-1].split("break;")[0]
        self.contains("LoadPalette(sOakSpeech_Oak_Pal", oak_case)
        self.contains("DecompressDataWithHeaderVram(sOakSpeech_Oak_Tiles", oak_case)
        self.contains("LoadTrainerPic(OAK_PIC", source)
        approved = (ROOT / "graphics/oak_speech/oak/archoak_approved.png").read_bytes()
        self.assertEqual(hashlib.sha256(approved).hexdigest(), "85cac888de41237252410c08d3db2e6f36e06f07f34485bb20706c10a2c6f9bf")
        sw, sh, sp, sa, pixels = indexed_png(approved)
        self.assertEqual((sw, sh), (64, 64))
        width, height, _, _, actual = indexed_png((ROOT / "graphics/oak_speech/oak/pic.png").read_bytes())
        self.assertEqual((width, height), (64, 96))
        lines = read("graphics/oak_speech/oak/pal.pal").splitlines()
        colors = [tuple(map(int, line.split())) for line in lines[3:]]
        self.assertEqual(len(colors), int(lines[2]))
        self.assertLessEqual(96 + len(colors), 240, "ArchOak palette must not overwrite text palette")
        # Reconstruct BG2's 8bpp display using the palette actually loaded at 96.
        for y in range(96):
            for x in range(64):
                idx = actual[y * 64 + x]
                original = pixels[(y - 16) * 64 + x] if 16 <= y < 80 else 0
                transparent = not (16 <= y < 80) or (original < len(sa) and sa[original] == 0)
                if transparent:
                    self.assertEqual(idx, 0, (x, y, "transparency"))
                else:
                    self.assertTrue(96 <= idx < 96 + len(colors), (x, y, "palette index", idx))
                    self.assertEqual(tuple(v // 8 for v in colors[idx - 96]),
                                     tuple(v // 8 for v in sp[original]), (x, y, "approved pixel"))

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
