"""Saffron Fighting Dojo approach and Relic party/battle sprite regressions."""
import re
import struct
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def block(src, label):
    match = re.search(r"^" + re.escape(label) + r"::\n", src, re.M)
    assert match, label
    next_label = re.search(r"^[A-Za-z0-9_]+::\n", src[match.end():], re.M)
    end = match.end() + next_label.start() if next_label else len(src)
    return src[match.start():end]


def png_header(path, expected_dimensions):
    raw = (ROOT / path).read_bytes()
    assert raw.startswith(b"\x89PNG\r\n\x1a\n")
    assert struct.unpack(">II", raw[16:24]) == expected_dimensions
    assert raw[24:26] == bytes((4, 3)), "GBA sprites require indexed 4-bpp PNG"
    assert b"tRNS" in raw
    cursor = 8
    while cursor < len(raw):
        length = struct.unpack(">I", raw[cursor:cursor + 4])[0]
        typ = raw[cursor + 4:cursor + 8]
        data = raw[cursor + 8:cursor + 8 + length]
        crc = struct.unpack(">I", raw[cursor + 8 + length:cursor + 12 + length])[0]
        assert zlib.crc32(typ + data) == crc, path + " corrupt PNG"
        cursor += length + 12
    assert cursor == len(raw)


class DojoTutorAndRelicSprites(unittest.TestCase):
    def test_dojo_sight_scanner_stops_after_victory_without_forced_tutoring(self):
        scripts = read("data/maps/SaffronCity_Dojo_Frlg/scripts.inc")
        tutors = read("data/scripts/move_tutors_frlg.inc")
        for person, trainer, tutor in (
            ("Hitoshi", "TRAINER_BLACK_BELT_HITOSHI", "PowerUpPunch"),
            ("Hideki", "TRAINER_BLACK_BELT_HIDEKI", "IcePunch"),
            ("Aaron", "TRAINER_BLACK_BELT_AARON", "BlazeKick"),
            ("Mike", "TRAINER_BLACK_BELT_MIKE", "HighJumpKick"),
        ):
            trainer_block = block(scripts, f"SaffronCity_Dojo_EventScript_{person}")
            body = trainer_block.splitlines()[1:]
            self.assertTrue(body[0].strip().startswith("trainerbattle_single " + trainer))
            self.assertNotIn("goto_if_defeated", trainer_block)
            self.assertIn(f"goto SaffronCity_Dojo_EventScript_{tutor}", trainer_block)
            self.assertIn(f"goto EventScript_PRL_Tutor{tutor}", block(scripts, f"SaffronCity_Dojo_EventScript_{tutor}"))
            self.assertIn("MSGBOX_YESNO", block(tutors, f"EventScript_PRL_Tutor{tutor}"))
        self.assertIn("goto_if_eq VAR_RESULT, NO, EventScript_PRL_TutorDeclined", tutors)
        self.assertIn("release\n\tend", block(tutors, "EventScript_PRL_TutorDeclined"))

    def test_party_icons_derive_from_approved_species_and_fit_gba_frames(self):
        for mon in ("nolax", "noxichu"):
            png_header(f"graphics/pokemon/{mon}/icon.png", (32, 64))

    def test_back_sprites_are_gba_sized_and_lowered(self):
        png_header("graphics/pokemon/nolax/back.png", (64, 64))
        species = read("src/data/pokemon/species_info/prl_custom.h")
        nolax = species.split("[SPECIES_NOLAX] =", 1)[1].split("[SPECIES_NUMEW]", 1)[0]
        self.assertIn(".backPicYOffset = 8,", nolax)
        nox = read("src/data/pokemon/species_info.h")
        nox = nox.split("[SPECIES_NOXICHU] =", 1)[1].split('#include "species_info/prl_custom.h"', 1)[0]
        self.assertIn(".backPicYOffset = 20,", nox)
        self.assertIn(".iconPalIndex = 6,", nox)


if __name__ == "__main__":
    unittest.main()
