"""Check all current PRL Pokemon Centers expose the reusable settable clock."""
from pathlib import Path
import json
import unittest

ROOT = Path(__file__).resolve().parents[1]
CLOCK = "EventScript_PRL_PokemonCenterWallClock"


class TestPjrCenterClock(unittest.TestCase):
    def test_every_existing_pokemon_center_floor_has_one_wall_clock(self):
        maps = sorted((ROOT / "data/maps").glob("*PokemonCenter_1F*/map.json"))
        self.assertEqual(len(maps), 35, "Review new center maps when regions expand")
        for path in maps:
            with self.subTest(center=path.parent.name):
                j = json.loads(path.read_text(encoding="utf-8"))
                hits = [e for e in j["bg_events"] if e.get("script") == CLOCK]
                self.assertEqual(len(hits), 1)
                self.assertEqual(hits[0]["type"], "sign")
                self.assertEqual(hits[0]["player_facing_dir"], "BG_EVENT_PLAYER_FACING_ANY")
                self.assertFalse(any(e["x"] == hits[0]["x"] and e["y"] == hits[0]["y"]
                                     for e in j["object_events"]), "don't shadow an NPC")
                self.assertFalse(any(e is not hits[0] and e["x"] == hits[0]["x"] and e["y"] == hits[0]["y"]
                                     for e in j["bg_events"]), "don't shadow an existing sign")

    def test_common_clock_can_be_changed_more_than_once(self):
        script = (ROOT / "data/scripts/players_house.inc").read_text(encoding="utf-8")
        self.assertEqual(script.count(CLOCK + "::"), 1)
        content = script.split(CLOCK + "::", 1)[1].split("\n\n", 1)[0]
        self.assertIn("special StartWallClock", content)
        self.assertNotIn("goto_if_set FLAG_SET_WALL_CLOCK", content)
        self.assertIn("copyvar VAR_0x8004, VAR_RESULT", content)
        specials = (ROOT / "data/specials.inc").read_text(encoding="utf-8")
        self.assertIn("def_special StartWallClock", specials)


if __name__ == "__main__":
    unittest.main()
