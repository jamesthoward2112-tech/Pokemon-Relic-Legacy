"""PR78: protect the approved animated forest and unobstructed Vermilion pier."""
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


class PR78Preservation(unittest.TestCase):
    def test_manual_skip_and_original_full_celebi_jirachi_sequence(self):
        forest = (ROOT / "src/prl_intro_trial.c").read_text(encoding="utf-8")
        main = forest.split("static void Task_Scene0_Main(u8 taskId)\n{", 1)[1].split(
            "\n#undef tState", 1)[0]
        # Only newly pressed A/B/START after the Porygon splash guard may skip.
        self.assertIn("gMain.newKeys & (A_BUTTON | B_BUTTON | START_BUTTON)", main)
        self.assertIn("task->tSkipReady < 12", main)
        self.assertIn("task->tState != S0_SKIP_FADE", main)
        self.assertIn("BeginNormalPaletteFade(PALETTES_ALL, 3, 0, 16, RGB_BLACK)", main)
        self.assertIn("case S0_SKIP_FADE:", main)
        self.assertIn("SetMainCallback2(CB2_InitTitleScreen)", main)
        for name in ("celebi", "jirachi", "celebi2", "jirachi2"):
            self.assertIn('INCGFX_U32("graphics/intro/prl_hwl/' + name + '.png"', forest)
            self.assertIn('INCGFX_U16("graphics/intro/prl_hwl/' + name + '.png"', forest)
        for event in ("S0_LAUNCH_SPRITES", "S0_LAUNCH_SPRITES2",
                      "SetMainCallback2(CB2_InitTitleScreen)"):
            self.assertIn(event, main)
        intro = (ROOT / "src/expansion_intro.c").read_text(encoding="utf-8")
        self.assertIn("SetMainCallback2(CB2_InitPRLHwlScene0)", intro)
        title = (ROOT / "src/title_screen_frlg.c").read_text(encoding="utf-8")
        self.assertIn('INCBIN_U16("graphics/title_screen_prl/prl_option_b_title_mode3.bin")', title)

    def test_sailor_exactly_matches_approved_pr75_layout_and_dialogue(self):
        events = json.loads((ROOT / "data/maps/VermilionCity_Frlg/map.json").read_text(encoding="utf-8"))
        sailor = next(o for o in events["object_events"]
                      if o.get("local_id") == "LOCALID_VERMILION_FERRY_SAILOR")
        self.assertEqual((sailor["x"], sailor["y"]), (24, 33))
        flavor = next(o for o in events["object_events"]
                      if o["script"] == "VermilionCity_EventScript_Sailor")
        self.assertEqual((flavor["x"], flavor["y"]), (31, 30))
        # Exactly the two original ticket-entry/reset lanes, no experimental third lane.
        triggers = [e for e in events["coord_events"]
                    if e["var"] == "VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER"]
        self.assertEqual({(e["x"], e["y"]) for e in triggers}, {(22, 33), (23, 33)})
        script = (ROOT / "data/maps/VermilionCity_Frlg/scripts.inc").read_text(encoding="utf-8")
        sailor_script = script.split("VermilionCity_EventScript_FerrySailor::", 1)[1].split(
            "VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[0]
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET, VermilionCity_EventScript_PRLHoennBoardingPreflight", sailor_script)
        self.assertIn("goto_if_eq VAR_MAP_SCENE_VERMILION_CITY, 3, VermilionCity_EventScript_CheckSeagallopPresent", sailor_script)
        self.assertNotIn("PRLPierDepartedFallback", script)
        self.assertNotIn("PRLHasKantoHallOfFameRecord", sailor_script)


if __name__ == "__main__":
    unittest.main()
