"""Audit 24 reserved Gym claim flags and all 16 existing Gym scripts."""
import pathlib,re,unittest
R=pathlib.Path(__file__).resolve().parents[1]
K=[("BROCK","PewterCity_Gym_Frlg","HARD_STONE","ITEM_TM39",1),("MISTY","CeruleanCity_Gym_Frlg","MYSTIC_WATER","ITEM_TM03",2),("SURGE","VermilionCity_Gym_Frlg","MAGNET","ITEM_TM34",3),("ERIKA","CeladonCity_Gym_Frlg","MIRACLE_SEED","ITEM_TM19",4),("KOGA","FuchsiaCity_Gym_Frlg","POISON_BARB","ITEM_TM06",5),("SABRINA","SaffronCity_Gym_Frlg","TWISTED_SPOON","ITEM_TM04",6),("BLAINE","CinnabarIsland_Gym_Frlg","CHARCOAL","ITEM_TM38",7),("GIOVANNI","ViridianCity_Gym_Frlg","SOFT_SAND","ITEM_TM26",8)]
H=[("ROXANNE","RustboroCity_Gym","HARD_STONE","ITEM_TM_ROCK_TOMB",1),("BRAWLY","DewfordTown_Gym","BLACK_BELT","ITEM_TM_BULK_UP",2),("WATTSON","MauvilleCity_Gym","MAGNET","ITEM_TM_SHOCK_WAVE",3),("FLANNERY","LavaridgeTown_Gym_1F","CHARCOAL","ITEM_TM_OVERHEAT",4),("NORMAN","PetalburgCity_Gym","SILK_SCARF","ITEM_TM_FACADE",5),("WINONA","FortreeCity_Gym","SHARP_BEAK","ITEM_TM_AERIAL_ACE",6),("TATE_LIZA","MossdeepCity_Gym","TWISTED_SPOON","ITEM_TM_CALM_MIND",7),("JUAN","SootopolisCity_Gym_1F","MYSTIC_WATER","ITEM_TM_WATER_PULSE",8)]
J=("FALKNER","BUGSY","WHITNEY","MORTY","CHUCK","JASMINE","PRYCE","CLAIR")
def read(x): return (R/x).read_text(encoding="utf-8")
class PRLGymRewards(unittest.TestCase):
 def test_distinct_flags(self):
    flags=read("include/constants/flags.h")
    names=["KANTO_"+n for n,*_ in K]+["JOHTO_"+n for n in J]+["HOENN_"+n for n,*_ in H]
    vals=[]
    for n in names:
        m=re.search(r"^#define FLAG_PRL_GYM_"+n+r"_EXTRA\s+(0x[0-9A-F]+)",flags,re.M)
        self.assertIsNotNone(m,n)
        vals.append(int(m.group(1),16))
    self.assertEqual(len(set(vals)),24)
 def test_item_first_money_second_flag_last(self):
    txt=read("data/scripts/prl_gym_rewards.inc")
    for region,gyms in (("KANTO",K),("HOENN",H)):
        for name,_,item,_,_ in gyms:
            segment=txt.split(f"PRL_EventScript_GymExtra_{region}_{name}::",1)[1].split("::",1)[0]
            tokens=[f"goto_if_set FLAG_PRL_GYM_{region}_{name}_EXTRA",f"checkitemspace ITEM_{item}",f"giveitem ITEM_{item}","addmoney 10000",f"setflag FLAG_PRL_GYM_{region}_{name}_EXTRA"]
            locs=[segment.index(t) for t in tokens]
            self.assertEqual(locs,sorted(locs))
            self.assertIn("PRL_EventScript_GymExtraNoRoom",segment)
    self.assertEqual(txt.count("addmoney 10000"),16)
 def test_each_gym_has_retry_and_original_tm(self):
    for region,gyms in (("KANTO",K),("HOENN",H)):
        for name,path,item,tm,badge in gyms:
            with self.subTest(region=region,leader=name):
                s=read(f"data/maps/{path}/scripts.inc")
                call=f"call_if_unset FLAG_PRL_GYM_{region}_{name}_EXTRA, PRL_EventScript_GymExtra_{region}_{name}"
                self.assertEqual(s.count(call),2)
                self.assertIn(f"setflag FLAG_BADGE0{badge}_GET",s)
                self.assertIn(tm,s)
    self.assertEqual(read("data/event_scripts.s").count('.include "data/scripts/prl_gym_rewards.inc"'),1)
 def test_johto_not_present(self):
    self.assertFalse((R/"data/maps/VioletCity_Gym").exists())
if __name__=="__main__":unittest.main()
