"""PKR V18 truck Mew regression coverage for PRL PR75.

These source assertions guard coordinates, retry and save compatibility.
Runtime verification still required for map-collision and battle-return behavior.
"""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class PKRTruckMewMigrationTests(unittest.TestCase):
    def test_broad_verified_truck_trigger_and_hidden_item_preserved(self):
        data = json.loads(read("data/maps/SSAnne_Exterior_Frlg/map.json"))
        trigger = [e for e in data["bg_events"] if e.get("script") == "SSAnne_Exterior_EventScript_PRLTruckMew"]
        expected = {(x, y) for y in range(8) for x in range(55, 64)}
        self.assertEqual({(e["x"], e["y"]) for e in trigger}, expected)
        self.assertEqual(len(trigger), 72)
        for e in trigger:
            self.assertEqual(e["type"], "sign")
            self.assertEqual(e["elevation"], 0)
            self.assertEqual(e["player_facing_dir"], "BG_EVENT_PLAYER_FACING_ANY")
        self.assertTrue(any(e.get("type") == "hidden_item" and
                            e.get("x") == 58 and e.get("y") == 28 and
                            e.get("item") == "ITEM_LAVA_COOKIE"
                            for e in data["bg_events"]))
        layout = json.loads(read("data/layouts/layouts.json"))
        layout_entry = next(x for x in layout["layouts"] if x["id"] == "LAYOUT_SSANNE_EXTERIOR")
        self.assertGreater(layout_entry["width"], 63)
        self.assertGreater(layout_entry["height"], 7)

    def test_level_fifty_retry_only_caught_sets_flag(self):
        script = read("data/maps/SSAnne_Exterior_Frlg/scripts.inc")
        battle = script.split("SSAnne_Exterior_EventScript_PRLTruckMew::", 1)[1].split("SSAnne_Exterior_EventScript_PRLTruckMewCaught::", 1)[0]
        self.assertIn("PRLHasKantoHallOfFameRecord", battle)
        self.assertIn("setwildbattle SPECIES_MEW, 50", battle)
        self.assertIn("playmoncry SPECIES_MEW", battle)
        self.assertIn("special BattleSetup_StartLegendaryBattle", battle)
        self.assertIn("goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SSAnne_Exterior_EventScript_PRLTruckMewCaught", battle)
        self.assertNotIn("setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT", battle)
        on_catch = script.split("SSAnne_Exterior_EventScript_PRLTruckMewCaught::", 1)[1].split("SSAnne_Exterior_EventScript_PRLTruckMewQuiet::", 1)[0]
        self.assertIn("setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT", on_catch)
        self.assertIn("setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED", on_catch)
        self.assertIn("goto_if_unset FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT, SSAnne_Exterior_EventScript_PRLTruckMewAvailable", battle)
        self.assertIn("goto_if_set FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED, SSAnne_Exterior_EventScript_PRLTruckMewQuiet", battle)
        self.assertNotIn("setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED", battle)

    def test_dedicated_flag_and_original_mew_flags_untouched(self):
        flags = read("include/constants/flags.h")
        self.assertIn("#define FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT                 0x4EE", flags)
        self.assertIn("#define FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED      0x4EF", flags)
        self.assertNotIn("#define FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT                 0x4DD", flags)
        self.assertIn("#define FLAG_CAUGHT_MEW", flags)
        self.assertIn("#define FLAG_DEFEATED_MEW", flags)
        self.assertIn("#define FLAG_HIDE_MEW", flags)
        self.assertNotIn("FLAG_CAUGHT_MEW", read("data/maps/SSAnne_Exterior_Frlg/scripts.inc"))

    def test_vermilion_pr74_sailor_preserved(self):
        s = read("data/maps/VermilionCity_Frlg/scripts.inc")
        self.assertIn("VermilionCity_EventScript_PRLHoennBoardingPreflight", s)
        self.assertIn("PRLHoennCheckPCSpaceForParty", s)
        self.assertIn("VermilionCity_EventScript_CheckSeagallopPresent", s)


if __name__ == "__main__":
    unittest.main()
