"""Keep the approved Mt Moon Jessie/James trainer behaviour across Kanto.

Static checks confirm the event wiring. Emulator testing remains necessary.
"""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SCENES = (
    ("MtMoon_B1F_Frlg", "MtMoon_B1F", "MT_MOON", "TRAINER_JESSIE_JAMES_MT_MOON"),
    ("RocketHideout_B4F_Frlg", "RocketHideout_B4F", "HIDEOUT", "TRAINER_JESSIE_JAMES_HIDEOUT"),
    ("SilphCo_11F_Frlg", "SilphCo_11F", "SILPH", "TRAINER_JESSIE_JAMES_SILPH"),
    ("VictoryRoad_3F_Frlg", "VictoryRoad_3F", "VICTORY_ROAD", "TRAINER_JESSIE_JAMES_VICTORY_ROAD"),
)

class RocketNativeTrainerRegression(unittest.TestCase):
    def test_every_duo_uses_native_approach_and_individual_dialogue(self):
        for folder, prefix, tag, trainer in SCENES:
            with self.subTest(scene=tag):
                mapdata = json.loads((ROOT / "data/maps" / folder / "map.json").read_text())
                script = (ROOT / "data/maps" / folder / "scripts.inc").read_text()
                actors = {a.get("local_id"): a for a in mapdata["object_events"]}
                for name in ("JESSIE", "JAMES"):
                    npc = actors["LOCALID_PRL_" + name + "_" + tag]
                    self.assertEqual(npc["trainer_type"], "TRAINER_TYPE_NORMAL")
                    self.assertEqual(npc["movement_type"], "MOVEMENT_TYPE_FACE_DOWN")
                    self.assertEqual(int(npc["trainer_sight_or_berry_tree_id"]), 4)
                    self.assertEqual(npc["script"], prefix + "_EventScript_PRL_" + name.title())
                    self.assertEqual(npc["flag"], "FLAG_HIDE_PRL_ROCKET_" + ("MT_MOON" if tag == "MT_MOON" else tag))
                    self.assertRegex(script, rf"trainerbattle_double {trainer}[^\n]*localId=LOCALID_PRL_{name}_{tag}")
                    self.assertIn(prefix + "_Text_PRL_" + name.title() + "PostBattle::", script)
                    self.assertNotIn("removeobject LOCALID_PRL_" + name + "_" + tag, script)
                meowth = actors["LOCALID_PRL_MEOWTH_" + tag]
                self.assertEqual(meowth["trainer_type"], "TRAINER_TYPE_NONE")
                self.assertIn("EventScript_PRL_Meowth", meowth["script"])
                self.assertNotIn("removeobject LOCALID_PRL_MEOWTH_" + tag, script)
                self.assertIn("goto_if_defeated " + trainer, script)
                self.assertIn("setflag FLAG_PRL_RECURRING_JESSIE_JAMES_" + tag, script)

    def test_no_forced_battles_on_transition_or_warp(self):
        for folder, prefix, tag, _ in SCENES:
            with self.subTest(scene=tag):
                mapdata = json.loads((ROOT / "data/maps" / folder / "map.json").read_text())
                script = (ROOT / "data/maps" / folder / "scripts.inc").read_text()
                for trig in mapdata["coord_events"]:
                    self.assertNotIn("PRL_JESSIE_JAMES", trig["script"])
                self.assertNotIn(prefix + "_EventScript_PRL_JESSIE_JAMES_", script)
                if tag != "MT_MOON":
                    self.assertIn(prefix + "_OnLoad_PRLRocketVisibility::", script)
                    self.assertIn(prefix + "_OnTransition_PRLRocket::", script)

    def test_boss_and_league_progression_remain_gated(self):
        hideout = (ROOT / "data/maps/RocketHideout_B4F_Frlg/scripts.inc").read_text()
        silph = (ROOT / "data/maps/SilphCo_11F_Frlg/scripts.inc").read_text()
        road = (ROOT / "data/maps/VictoryRoad_2F_Frlg/scripts.inc").read_text()
        self.assertIn("goto_if_unset FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT", hideout)
        self.assertIn("goto_if_unset FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH", silph)
        self.assertIn("goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD", road)
        self.assertIn("FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT", hideout)
        self.assertIn("checkitem ITEM_SILPH_SCOPE", silph)

if __name__ == "__main__":
    unittest.main()
