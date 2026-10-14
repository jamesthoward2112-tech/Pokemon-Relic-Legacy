"""PR78: protect the approved animated forest and unobstructed Vermilion pier."""
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


class PR78Preservation(unittest.TestCase):
    def test_unskippable_full_celebi_jirachi_sequence(self):
        forest = (ROOT / "src/prl_intro_trial.c").read_text(encoding="utf-8")
        main = forest.split("static void Task_Scene0_Main(u8 taskId)\n{", 1)[1].split(
            "\n#undef tState", 1)[0]
        self.assertNotIn("gMain.newKeys", main)
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

    def test_dock_walkway_is_not_blocked_by_sailors(self):
        events = json.loads((ROOT / "data/maps/VermilionCity_Frlg/map.json").read_text(encoding="utf-8"))
        sailor = [o for o in events["object_events"]
                  if o.get("script") == "VermilionCity_EventScript_FerrySailor"]
        self.assertEqual(len(sailor), 1)
        self.assertEqual((sailor[0]["x"], sailor[0]["y"]), (31, 30))
        self.assertFalse([o for o in events["object_events"]
                          if o["x"] in (22, 23, 24) and o["y"] >= 30])
        for x in (22, 23, 24):
            self.assertIn((x, 34), [(w["x"], w["y"]) for w in events["warp_events"]])
            self.assertIn((x, 33), [(e["x"], e["y"]) for e in events["coord_events"]])
        script = (ROOT / "data/maps/VermilionCity_Frlg/scripts.inc").read_text(encoding="utf-8")
        self.assertIn("goto_if_eq VAR_MAP_SCENE_VERMILION_CITY, 3, VermilionCity_EventScript_PRLPierDepartedFallback", script)
        self.assertIn("goto_if_eq VAR_MAP_SCENE_VERMILION_CITY, 3, VermilionCity_EventScript_PRLPierAccess", script)


if __name__ == "__main__":
    unittest.main()
