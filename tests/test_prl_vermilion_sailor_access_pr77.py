"""PR77: post-Hall-of-Fame pier must be reachable on legacy Kanto saves."""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class PR77SailorAccess(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.script = (ROOT / "data/maps/VermilionCity_Frlg/scripts.inc").read_text(encoding="utf-8")
        cls.map = json.loads((ROOT / "data/maps/VermilionCity_Frlg/map.json").read_text(encoding="utf-8"))

    def test_sailor_does_not_occupy_last_pier_entry(self):
        sailor = next(x for x in self.map["object_events"]
                      if x.get("local_id") == "LOCALID_VERMILION_FERRY_SAILOR")
        self.assertEqual((sailor["x"], sailor["y"]), (24, 31))
        for x in (22, 23, 24):
            self.assertFalse(any(o["x"] == x and o["y"] == 33
                                 for o in self.map["object_events"]),
                             f"Entry tile {x},33 must remain open")
            entry = [e for e in self.map["coord_events"]
                     if e["x"] == x and e["y"] == 33
                     and e["var"] == "VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER"
                     and e["var_value"] == "0"]
            self.assertEqual(len(entry), 1)
            exit_events = [e for e in self.map["coord_events"]
                           if e["x"] == x and e["y"] == 32
                           and e["script"] == "VermilionCity_EventScript_ExitedTicketCheck"]
            self.assertEqual(len(exit_events), 1)

    def test_sailor_uses_champion_record_even_if_old_flag_missing(self):
        block = self.script.split("VermilionCity_EventScript_FerrySailor::", 1)[1].split(
            "VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[0]
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET", block)
        self.assertIn("specialvar VAR_RESULT, PRLHasKantoHallOfFameRecord", block)
        self.assertIn("goto_if_eq VAR_RESULT, TRUE, VermilionCity_EventScript_PRLHoennBoardingPreflight", block)
        self.assertLess(block.index("PRLHasKantoHallOfFameRecord"),
                        block.index("VermilionCity_EventScript_CheckSeagallopPresent"))

    def test_gate_allows_champion_without_requiring_ticket_flag(self):
        block = self.script.split("VermilionCity_EventScript_CheckTicket::", 1)[1].split(
            "VermilionCity_EventScript_PRLPierAccess::", 1)[0]
        self.assertIn("specialvar VAR_RESULT, PRLHasKantoHallOfFameRecord", block)
        self.assertIn("goto_if_eq VAR_RESULT, TRUE, VermilionCity_EventScript_PRLPierAccess", block)
        self.assertLess(block.index("PRLHasKantoHallOfFameRecord"),
                        block.index("VermilionCity_Text_DoYouHaveATicket"))

    def test_existing_sevii_and_non_destructive_preflight_unchanged(self):
        block = self.script.split("VermilionCity_EventScript_PRLHoennBoardingPreflight::", 1)[1].split(
            "VermilionCity_Text_PRLHoennCheckDeparture::", 1)[0]
        self.assertIn("specialvar VAR_RESULT, PRLHoennCheckPCSpaceForParty", block)
        self.assertIn("VermilionCity_EventScript_CheckSeagallopPresent", block)
        self.assertNotIn("\twarp ", block)
        self.assertNotIn("\tremoveitem ", block)


if __name__ == "__main__":
    unittest.main()
