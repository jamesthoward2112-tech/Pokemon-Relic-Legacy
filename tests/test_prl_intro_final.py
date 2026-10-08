"""Regression gate for PRL's user-approved, final forest-to-title opening."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8-sig")


class FinalPrlIntroTests(unittest.TestCase):
    def test_porygon_splash_enters_original_forest_without_capture(self):
        intro = source("src/expansion_intro.c")
        self.assertIn("SetMainCallback2(CB2_InitPRLHwlScene0);", intro)
        self.assertNotIn("SetMainCallback2(CB2_InitPRLIntroTrial);", intro)

    def test_deprecated_capture_is_only_a_forwarder(self):
        old = source("src/prl_vanadium_capture.c")
        self.assertIn("CB2_InitPRLHwlScene0();", old)
        self.assertNotIn("noxichu_capture.png", old)
        self.assertNotIn("Task_Scene1_FadeIn", old)

    def test_original_forest_and_title_handoff_stay_intact(self):
        forest = source("src/prl_intro_trial.c")
        for sprite in ("celebi", "jirachi", "celebi2", "jirachi2"):
            self.assertIn(f'graphics/intro/prl_hwl/{sprite}.png', forest)
        self.assertIn("SetMainCallback2(CB2_InitTitleScreen);", forest)

    def test_title_mode3_fade_and_gold_sparkles_present(self):
        title = source("src/title_screen_frlg.c")
        self.assertIn("sPRLTitleMode3", title)
        self.assertIn("BLDCNT_TGT1_BG2 | BLDCNT_EFFECT_DARKEN", title)
        self.assertIn("SetGpuReg(REG_OFFSET_BLDY, 16);", title)
        self.assertIn("PRLUpdateTitleSparkles(data[3]++);", title)
        self.assertIn("PRLDrawPrompt(data[1]);", title)
        self.assertIn("JOY_NEW(A_BUTTON | START_BUTTON)", title)
        self.assertIn("SetMainCallback2(CB2_InitMainMenu);", title)

    def test_hosted_ci_does_not_generate_retired_capture(self):
        workflow = source(".github/workflows/prl-build.yml")
        self.assertIn("prepare_hwl_scene0.py", workflow)
        self.assertNotIn("prepare_vanadium_capture.py", workflow)
        self.assertNotIn("NOX_CAPTURE_13_FRAME_PASS", workflow)


if __name__ == "__main__":
    unittest.main()
