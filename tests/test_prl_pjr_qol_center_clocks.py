"""Regression tests for upstairs Trainer Services in every defined Pokemon Center."""
from pathlib import Path
import json
import unittest

ROOT = Path(__file__).resolve().parents[1]

class TestPjrCenterTrainerServices(unittest.TestCase):
    def test_all_centres_have_services_upstairs_instead_of_invisible_clocks(self):
        centers = sorted((ROOT / "data/maps").glob("*PokemonCenter_1F*/map.json"))
        self.assertEqual(len(centers), 35)
        for downstairs in centers:
            with self.subTest(center=downstairs.parent.name):
                first = json.loads(downstairs.read_text(encoding="utf-8"))
                self.assertFalse(any(e.get("script") == "EventScript_PRL_PokemonCenterWallClock"
                                     for e in first["bg_events"]))
                second_path = downstairs.parent.parent / downstairs.parent.name.replace("_1F","_2F") / "map.json"
                second = json.loads(second_path.read_text(encoding="utf-8"))
                for x, script in ((2, "Common_EventScript_MoveRelearner"),
                                  (6, "EventScript_PRL_CenterDaypart"),
                                  (10, "EventScript_PRL_CenterMoveDeleter")):
                    npc = [o for o in second["object_events"] if o["x"] == x and o["y"] == 2]
                    self.assertEqual(len(npc), 1)
                    if x == 10 and "_Frlg" in downstairs.parent.name:
                        # The former Move Deleter desk now hosts a location-scaled Chansey coach.
                        self.assertTrue(npc[0]["script"].startswith("EventScript_PRL_Chansey_"))
                    else:
                        self.assertEqual(npc[0]["script"], script)

    def test_four_relearner_categories_use_level_and_badge_gates(self):
        src = (ROOT / "data/scripts/move_relearner.inc").read_text()
        for name in ("MoveRelearner_Text_LevelUpMoves", "MoveRelearner_Text_EggMoves",
                     "MoveRelearner_Text_TMMoves", "MoveRelearner_Text_TutormoveMoves"):
            self.assertIn(name, src)
        code = (ROOT / "src/move_relearner.c").read_text()
        self.assertIn("PRL_IsExtraRelearnerMoveUnlocked", code)
        self.assertIn("gMovesInfo[move].power > maxPower", code)
        self.assertIn("u32 level = GetLevelFromBoxMonExp(mon);", code)
        self.assertIn("u32 level = GetLevelFromBoxMonExp(boxMon);", code)

    def test_daypart_is_persistent_and_repeatable(self):
        clock = (ROOT / "src/clock.c").read_text()
        self.assertIn("void PRL_SetDaypart(void)", clock)
        self.assertIn("RtcCalcLocalTimeOffset(gLocalTime.days", clock)
        self.assertIn("FormChangeTimeUpdate();", clock)
        scripts = (ROOT / "data/scripts/players_house.inc").read_text()
        self.assertIn("EventScript_PRL_CenterDaypart::", scripts)
        self.assertIn("special PRL_SetDaypart", scripts)
        self.assertIn("EventScript_PRL_CenterMoveDeleter::", scripts)
        self.assertIn("def_special PRL_SetDaypart",
                      (ROOT / "data/specials.inc").read_text())

if __name__ == "__main__":
    unittest.main()
