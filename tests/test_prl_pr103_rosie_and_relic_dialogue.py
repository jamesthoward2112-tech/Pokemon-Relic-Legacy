"""PR103: Hoenn arrival dialogue, Aunt Rosie persistence and accessible homes.

Source-level safety checks only. Passing these is NOT a substitute for a
copied-save mGBA/Pizza Boy playthrough through Littleroot -> Oldale.
"""
import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class HoennRosieAndDialogue(unittest.TestCase):
    def test_entrance_animation_reaches_correct_door_and_disappears(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("setobjectxy LOCALID_LITTLEROOT_MOM, 5, 9", town)
        self.assertIn("setobjectxy LOCALID_LITTLEROOT_MOM, 14, 9", town)
        self.assertIn("LittlerootTown_Movement_PRLRosieEnterDoor:", town)
        anim = town.split("LittlerootTown_Movement_PRLRosieEnterDoor:", 1)[1].split("step_end", 1)[0]
        self.assertIn("walk_up", anim)
        self.assertIn("set_invisible", anim)
        arrival = town.split("LittlerootTown_EventScript_PRLRosieWelcomeOnArrival::", 1)[1].split("LittlerootTown_EventScript_PRLRosieAlignMaleDoor::", 1)[0]
        self.assertIn("removeobject LOCALID_LITTLEROOT_MOM", arrival)
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE", arrival)

    def test_aunt_hidden_on_return_to_town_and_visible_in_own_home(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        home = town.split("LittlerootTown_EventScript_PRLKeepRosieInside::", 1)[1].split("return", 1)[0]
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE", home)
        for stage in (2, 3, 4):
            self.assertIn(
                f"call_if_eq VAR_PRL_REGION_STAGE, {stage}, LittlerootTown_EventScript_PRLKeepRosieInside",
                town,
            )
        exterior = town.split("LittlerootTown_EventScript_PRLRepairHoennProps::", 1)[1].split("end", 1)[0]
        self.assertIn("removeobject LOCALID_LITTLEROOT_MOM", exterior)
        for name in ("Brendans", "Mays"):
            scripts = read(f"data/maps/LittlerootTown_{name}House_1F/scripts.inc")
            for stage in (2, 3, 4):
                self.assertIn(
                    f"call_if_eq VAR_PRL_REGION_STAGE, {stage}, LittlerootTown_{name}House_1F_EventScript_PRLMakeAuntAvailable",
                    scripts,
                )
            aunt = scripts.split(f"LittlerootTown_{name}House_1F_EventScript_PRLAuntTalk::", 1)[1]
            self.assertIn("checkitem ITEM_TOWN_MAP", aunt)
            self.assertIn("giveitem ITEM_TOWN_MAP", aunt)
            self.assertIn("goto_if_ne VAR_PRL_REGION_STAGE, 2", aunt)

    def test_birch_no_spoilers_and_aide_uses_relic_dialogue(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        intro = lab.split("LittlerootTown_ProfessorBirchsLab_Text_PRLHoennWelcome::", 1)[1].split("LittlerootTown_ProfessorBirchsLab_Text_PRLWantRalts::", 1)[0]
        self.assertIn("potential", intro)
        self.assertIn("mystery", intro)
        self.assertNotIn("VALKYVOIR", intro)
        self.assertNotIn("SKULBERUS", intro)
        aide = lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_Aide::", 1)[1].split("LittlerootTown_ProfessorBirchsLab_EventScript_AideAlreadyMet::", 1)[0]
        for stage in (2, 3, 4):
            self.assertIn(f"goto_if_eq VAR_PRL_REGION_STAGE, {stage}, LittlerootTown_ProfessorBirchsLab_EventScript_PRLAide", aide)
        self.assertIn("BIRCH's AIDE: We're studying", lab)

    def test_two_littleroot_houses_and_oldale_warps_open(self):
        gen = read("tools/mapjson/mapjson.cpp")
        for name in ("OldaleTown", "LittlerootTown_BrendansHouse_1F", "LittlerootTown_MaysHouse_1F"):
            self.assertIn(f'"{name}"', gen)
        town = json.loads(read("data/maps/LittlerootTown/map.json"))
        self.assertEqual(town["warp_events"][0]["dest_map"], "MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F")
        self.assertEqual(town["warp_events"][1]["dest_map"], "MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F")
        self.assertEqual((town["warp_events"][0]["x"], town["warp_events"][0]["y"]), (14, 8))
        self.assertEqual((town["warp_events"][1]["x"], town["warp_events"][1]["y"]), (5, 8))
        for name in ("Brendans", "Mays"):
            house = json.loads(read(f"data/maps/LittlerootTown_{name}House_1F/map.json"))
            self.assertEqual(house["warp_events"][0]["dest_map"], "MAP_LITTLEROOT_TOWN")
        route = json.loads(read("data/maps/Route101/map.json"))
        self.assertIn("MAP_OLDALE_TOWN", [c["map"] for c in route["connections"]])


if __name__ == "__main__":
    unittest.main()
