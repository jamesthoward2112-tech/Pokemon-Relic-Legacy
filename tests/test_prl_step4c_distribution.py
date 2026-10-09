"""Guard first-batch PRL Relic TM/Tutor/ritual distribution (Step 4C)."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

LEGAL = {
    "SOIL_DRAIN": {"EDENSAUR", "FAERANIUM", "CACTOMB", "SPHYNXEON"},
    "TAKE_FLIGHT": {"CHARAXIS", "LEPIDEON", "SKARMET", "GRIMFOWL", "GHOULBAT", "TROPISAUR"},
    "IRON_FANGS": {"GUARDEON", "MAWYRM"},
    "SEISMIC_SLAM": {"KHANG", "HERACURION", "FERALODON", "SWAMPLITH", "COELOSSUS"},
    "BRAVADO": {"KHANG", "NOLAX", "MILTITAN", "SKULBERUS"},
    "BEATDOWN": {"CHAMPEON", "SKULBERUS"},
    "INSECT_IMPACT": {"SOLAZIKEN"},
    "RAGING_SOULS": {"PYROCLAST", "GHOULBAT", "OMEON"},
}
class Step4CDistributionTests(unittest.TestCase):
    def test_each_custom_move_is_whitelisted_by_species_and_progress(self):
        src = read("src/pokemon.c")
        for move, names in LEGAL.items():
            with self.subTest(move=move):
                match = re.search(r"case MOVE_" + move + r":(.*?)(?=\n    case MOVE_|\n    default:)", src, re.S)
                self.assertIsNotNone(match)
                self.assertEqual(set(re.findall(r"SPECIES_([A-Z0-9_]+)", match.group(1))), names)
                gate = ("FLAG_BADGE03_GET" if move in ("SOIL_DRAIN", "TAKE_FLIGHT", "IRON_FANGS")
                        else "FLAG_SYS_GAME_CLEAR" if move == "RAGING_SOULS"
                        else "FLAG_BADGE05_GET")
                self.assertIn("FlagGet(" + gate + ")", match.group(1))

    def test_existing_fifty_tms_are_not_replaced(self):
        src = read("include/constants/tms_hms.h")
        first = src.split("#define FOREACH_TM(F)",1)[1].split("#define FOREACH_HM(F)",1)[0]
        seq = re.findall(r"F\(([A-Z0-9_]+)\)",first)
        self.assertEqual(len(seq),53)
        self.assertEqual(seq[-3:],["SOIL_DRAIN","TAKE_FLIGHT","IRON_FANGS"])
        items = read("src/data/items.h")
        for n, move in ((51,"SOIL_DRAIN"),(52,"TAKE_FLIGHT"),(53,"IRON_FANGS")):
            self.assertIn("[ITEM_TM_"+move+"]",items)
            self.assertNotIn("[ITEM_TM"+str(n)+"]",items)

    def test_script_connects_shop_tutor_and_postleague_ritual(self):
        scripts = read("data/scripts/move_tutors.inc")
        for sym in ("PRL_EventScript_RelicMaster::","pokemart PRL_RelicTMStock",
                    "special ChooseMonForMoveTutor","goto_if_unset FLAG_SYS_GAME_CLEAR",
                    "setvar VAR_0x8005, MOVE_RAGING_SOULS"):
            self.assertIn(sym,scripts)
        for move in ("SOIL_DRAIN","TAKE_FLIGHT","IRON_FANGS"):
            self.assertIn("ITEM_TM_"+move,scripts)
        for move in ("SEISMIC_SLAM","BRAVADO","BEATDOWN","INSECT_IMPACT"):
            self.assertIn("setvar VAR_0x8005, MOVE_"+move,scripts)
        for ident in ("MULTI_PRL_RELIC_SERVICES","MULTI_PRL_RELIC_TUTOR_MOVES"):
            self.assertIn(ident,read("include/constants/script_menu.h"))
            self.assertIn("["+ident+"]",read("src/data/script_menu.h"))

    def test_npcs_in_two_present_regions(self):
        for path in ("data/maps/PewterCity_Mart_Frlg/map.json","data/maps/FallarborTown_Mart/map.json"):
            m=json.loads(read(path))
            npc=[x for x in m["object_events"] if x["script"]=="PRL_EventScript_RelicMaster"]
            self.assertEqual(len(npc),1)
            p=(npc[0]["x"],npc[0]["y"])
            self.assertFalse(any((x["x"],x["y"])==p for x in m["warp_events"]))
        self.assertFalse((ROOT/"data/maps/EcruteakCity").exists())

if __name__ == "__main__":
    unittest.main()
