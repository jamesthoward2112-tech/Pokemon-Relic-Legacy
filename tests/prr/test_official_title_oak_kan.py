"""Guard the approved PRR title assets and Oak's PRU Kan introductory artwork."""
from pathlib import Path
import hashlib
import re
import unittest

R=Path(__file__).resolve().parents[2]
def read(path):return (R/path).read_text(encoding="utf-8")

class PRRTitleAndOakKan(unittest.TestCase):
    def test_mode4_artwork_is_exact_export_not_redrawn(self):
        bitmap=R/"graphics/title_screen/prr/official_title.8bpp"
        palette=R/"graphics/title_screen/prr/official_title.gbapal"
        self.assertEqual(38400,bitmap.stat().st_size)
        self.assertEqual(512,palette.stat().st_size)
        self.assertEqual("020f6c3abaa57ee327b1fbad61ec333001543a3bef8e5a299925956e2e2f9a1d",
                         hashlib.sha256(bitmap.read_bytes()).hexdigest())
        self.assertEqual("307f8453bf14d797beaacb1c3454c9bd85efabad5c0c7e78320cc9c2eb53219b",
                         hashlib.sha256(palette.read_bytes()).hexdigest())

    def test_title_draws_independent_flashing_press_start(self):
        code=read("src/title_screen.c")
        self.assertIn("CB2_InitPRRTitleScreen();",code)
        self.assertIn("DISPCNT_MODE_4",code)
        self.assertIn("CreatePressStartBanner(START_BANNER_X, 138)",code)
        self.assertIn("if (++sprite->sTimer & 16)",code)
        self.assertIn("JOY_NEW(START_BUTTON)",code)
        self.assertIn("SetMainCallback2(CB2_GoToMainMenu)",code)
        self.assertIn("sPRRTitleBitmap[]",code)
        self.assertIn("sPRRTitlePalette[]",code)

    def test_oak_displays_exact_kan_without_changing_species_roster(self):
        src=read("src/oak_speech.c")
        art=(R/"graphics/oak_speech/kan_intro.png").read_bytes()
        self.assertEqual("497cc11aab9bb6af5bf9767b68c958c01f6d8ffe033ad4be4684de575f41a72f",
                         hashlib.sha256(art).hexdigest())
        self.assertIn("INTRO_SPECIES SPECIES_KANGASKHAN",src)
        self.assertIn("LoadCompressedSpriteSheet(&sOakIntroKanSheet)",src)
        self.assertIn("LoadSpritePalette(&sOakIntroKanPalette)",src)
        self.assertIn("CreateSprite(&sOakIntroKanTemplate",src)
        self.assertIn("CreatePokeballSpriteToReleaseMon",src)
        self.assertNotIn("#define INTRO_SPECIES SPECIES_OMANYTE",src)
        # No newly assigned Kan index: this is intro artwork only.
        constants=read("include/constants/species.h")
        self.assertNotRegex(constants,r"(?m)^\s*SPECIES_KAN\s*[,=]")

if __name__=="__main__":
    unittest.main()
