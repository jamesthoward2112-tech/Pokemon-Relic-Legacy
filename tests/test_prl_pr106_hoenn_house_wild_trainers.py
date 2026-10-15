"""PR106: guards against reintroducing broken first-Hoenn gameplay.

Source tests do not replace testing compiled ROM in the emulator.
"""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(name):
    return (ROOT / name).read_text()

class EarlyHoennPR106(unittest.TestCase):
    def test_route102_trainers_do_not_alias_kanto(self):
        frlg = read("include/constants/opponents_frlg.h")
        party = read("src/data/trainers_frlg.party")
        scripts = read("data/maps/Route102/scripts.inc")
        for name, num, first, level in (
            ("CALVIN", 659, "Poochyena", 5),
            ("RICK", 660, "Wurmple", 4),
            ("TIANA", 661, "Zigzagoon", 4),
            ("ALLEN", 662, "Taillow", 3),
        ):
            key = f"TRAINER_PRL_HOENN_{name}_102"
            self.assertRegex(frlg, rf"#define\s+{key}\s+{num}\b")
            self.assertIn(f"=== {key} ===", party)
            self.assertIn(f"trainerbattle_single {key}", scripts)
            self.assertIn("Level: " + str(level), party.split(f"=== {key} ===", 1)[1].split("=== TRAINER_",1)[0])
        self.assertIn("TRAINERS_COUNT_FRLG                      663", frlg)
        self.assertIn("MAX_TRAINERS_COUNT_FRLG                  768", frlg)

    def test_route101_and_route102_emerald_wild_slots_compiled_for_firered(self):
        wild = json.loads(read("src/data/wild_encounters.json"))
        encounters = wild["wild_encounter_groups"][0]["encounters"]
        for route in ("101", "102"):
            data = next(e for e in encounters if e["map"] == "MAP_ROUTE" + route)
            self.assertGreater(data["land_mons"]["encounter_rate"], 0)
            self.assertEqual(len(data["land_mons"]["mons"]), 12)
        generator = read("tools/wild_encounters/wild_encounters_to_header.py")
        self.assertEqual(generator.count('shared_label in ("gRoute101", "gRoute102")'), 2)
        self.assertEqual(generator.count('defined(EMERALD) || defined(FIRERED)'), 2)

    def test_rosie_does_not_respawn_at_outdoor_door(self):
        s = read("data/maps/LittlerootTown/scripts.inc")
        stage = s.split("LittlerootTown_EventScript_SetMomStandingInFrontOfDoorPos::",1)[1].split("LittlerootTown_EventScript_SetMomInFrontOfDoorMale::",1)[0]
        for n in (2,3,4):
            self.assertIn(f"goto_if_eq VAR_PRL_REGION_STAGE, {n}, LittlerootTown_EventScript_PRLMomAlreadyInside",stage)
        self.assertIn("setobjectxyperm LOCALID_LITTLEROOT_MOM, 255, 255",s)

    def test_houses_remove_inert_and_doorway_npcs_after_load(self):
        generator = read("tools/mapjson/mapjson.cpp")
        self.assertIn("prlHouseInert",generator)
        self.assertIn("prlHouseRival",generator)
        for h in ("Brendans","Mays"):
            script = read(f"data/maps/LittlerootTown_{h}House_1F/scripts.inc")
            self.assertIn("MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE",script)
            self.assertIn(f"LittlerootTown_{h}House_1F_EventScript_PRLCleanNPCs",script)
            self.assertIn("hideobjectat 5,",script)
            self.assertIn("removeobject 7",script)
            self.assertIn(f"LittlerootTown_{h}House_1F_EventScript_PRLRivalChat::",script)

if __name__ == "__main__":
    unittest.main()
