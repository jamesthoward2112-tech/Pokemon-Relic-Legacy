"""PR92 — reproduce PR90 starter visibility, duplicate gift and Bag restore issues."""
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
def read(p): return (ROOT/p).read_text(encoding="utf-8")
def block(src,label,next_label=None):
    after=src.split(label+"::",1)[1]
    return after.split("\n\n",1)[0] if next_label is None else after.split(next_label+"::",1)[0]

class PR92HoennRegressions(unittest.TestCase):
    def test_starter_ball_interaction_rejected_until_birch(self):
        s=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        for old,new in (("Cyndaquil","Ralts"),("Totodile","Poochyena"),("Chikorita","Gemlin")):
            v=block(s,"LittlerootTown_ProfessorBirchsLab_EventScript_"+old)
            self.assertIn("VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLAskBirchFirst",v)
            self.assertIn("VAR_PRL_REGION_STAGE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_PRL"+new+"Ball",v)
            v=block(s,"LittlerootTown_ProfessorBirchsLab_EventScript_PRL"+new+"Ball")
            self.assertIn("VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLAskBirchFirst",v)
        self.assertIn("Speak to him first",s)

    def test_post_starter_ball_visibility_is_persistently_hidden(self):
        s=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("map_script_2 VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLRemoveStarterBalls",s)
        self.assertIn("map_script_2 VAR_PRL_REGION_STAGE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_PRLRemoveStarterBalls",s)
        complete=block(s,"LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete")
        remove=block(s,"LittlerootTown_ProfessorBirchsLab_EventScript_PRLRemoveStarterBalls")
        for token in ("CYNDAQUIL","TOTODILE","CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_"+token,complete)
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_"+token,remove)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 2",complete)
        # PR99 re-enters the same map to purge the old rendered ball
        # metatiles after changing layout; waitstate safely terminates that warp.
        self.assertIn("warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB", complete)
        self.assertIn("waitstate", complete)
        self.assertIn("PRLHoennCommitKantoTransfer",s)

    def test_reset_kanto_inventory_in_live_bag_and_runtime_save_cache(self):
        c=block(read("src/field_specials.c"),"LittlerootTown_NotAFunction") if False else read("src/field_specials.c").split("u16 PRLHoennCommitKantoTransfer(void)",1)[1].split("\n}",1)[0]
        self.assertLess(c.index("ClearBag();"),c.index("AddBagItem(ITEM_EXP_SHARE, expShareQuantity)"))
        self.assertLess(c.index("AddBagItem(ITEM_EXP_SHARE, expShareQuantity)"),c.index("LoadPlayerBag();"))
        self.assertLess(c.index("LoadPlayerBag();"),c.index("SyncPrlInventoryLegacyBackup();"))
        loader=read("src/load_save.c")
        self.assertIn("memcpy(&gLoadedSaveData.bag, &(*GetPrlBag()), sizeof(struct Bag));",loader)
        self.assertIn("memcpy(&(*GetPrlBag()), &gLoadedSaveData.bag, sizeof(struct Bag));",loader)

    def test_birch_explicitly_explains_transfer_before_starter(self):
        s=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        intro=block(s,"LittlerootTown_ProfessorBirchsLab_Text_PRLHoennWelcome")
        for phrase in ("KANTO team","safe in your PC","old Bag","EXP SHARE","REMOTE PC","Lv1-10","P3000"):
            self.assertIn(phrase,intro)
        self.assertNotIn("uncle FRANK",intro)
        self.assertNotIn("remember uncle FRANK",s)
        mom=read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("AUNT ROSIE: {PLAYER}",mom)
        self.assertIn("Your MUM told me",mom)
        self.assertNotIn("Those Running Shoes",mom)

if __name__=="__main__": unittest.main()
