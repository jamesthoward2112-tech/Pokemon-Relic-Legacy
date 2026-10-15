"""PR90: Hoenn intro, Birch interaction, map-placed balls and shoe protection."""
import json
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def read(p): return (ROOT / p).read_text(encoding="utf-8")
class HoennPHRFoundation(unittest.TestCase):
    def test_birch_only_greets_when_interacted_with(self):
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        on_frame=lab.split("LittlerootTown_ProfessorBirchsLab_OnFrame:",1)[1].split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLHoennArrival::",1)[0]
        self.assertNotIn("map_script_2 VAR_PRL_REGION_STAGE, 3",on_frame)
        npc=lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_Birch::",1)[1].split("\n\n",1)[0]
        self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLHoennArrival",npc)
        self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_PRLBirchResearch",npc)
        self.assertIn("A strange RESONANCE",lab)
        self.assertNotIn("uncle FRANK",lab)
        self.assertIn("KANTO team",lab)
        self.assertIn("safe in your PC",lab)
        self.assertIn("Lv1-10",lab)
    def test_three_starters_stay_on_lab_table_and_only_after_birch(self):
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("EventScript_PRLWaitForBirch::",lab)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 4",lab)
        m=json.loads(read("data/maps/LittlerootTown_ProfessorBirchsLab/map.json"))
        ids=["LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL","LOCALID_BIRCHS_LAB_TOTODILE_BALL","LOCALID_BIRCHS_LAB_CHIKORITA_BALL"]
        positions={o.get("local_id"):(o["x"],o["y"]) for o in m["object_events"]}
        self.assertEqual([positions[i] for i in ids],[(8,4),(9,4),(10,4)])
        self.assertNotIn("map_script_2 VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLPlaceStarterBalls",lab)
    def test_running_shoes_do_not_retrigger_even_on_existing_pr89_save(self):
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        town=read("data/maps/LittlerootTown/scripts.inc")
        complete=lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete::",1)[1].split("\n\n",1)[0]
        for phrase in ("setvar VAR_LITTLEROOT_TOWN_STATE, 4","setflag FLAG_SYS_B_DASH","setflag FLAG_RECEIVED_RUNNING_SHOES","setflag FLAG_HIDE_LITTLEROOT_TOWN_MOM_OUTSIDE"):
            self.assertIn(phrase,complete)
        self.assertIn("call_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_EventScript_PRLKeepKantoRunningShoes",town)
        self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_EventScript_PRLMomAlreadyHasShoes",town)
        self.assertNotIn("S.S. LEGACY: HOENN AT LAST!",town)
    def test_atomic_transfer_still_one_time_and_gemlin(self):
        c=read("src/field_specials.c")
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 4",c)
        self.assertIn("starterSpecies = SPECIES_GEMLIN;",c)
        self.assertIn("PRLHoennCheckPCSpaceForParty()",c)
        self.assertIn("VarSet(VAR_PRL_REGION_STAGE, 2);",c)
if __name__=="__main__": unittest.main()
