"""Regression checks: PRL Noxichu polish and side-by-side Rocket staging."""
from pathlib import Path
import json, struct, hashlib, unittest
ROOT=Path(__file__).resolve().parents[1]
def png(path,w,h):
    data=(ROOT/path).read_bytes()
    assert data[:8]==b"\x89PNG\r\n\x1a\n"
    assert struct.unpack(">IIBB",data[16:26])==(w,h,4,3)
    assert b"tRNS" in data and data[data.index(b"tRNS")+4]==0
    return data
class NoxAndRocket(unittest.TestCase):
    def test_sprite_sizes_and_indexed_palette(self):
        for name,size in [("front",(64,64)),("back",(64,64)),("icon",(32,64))]:
            self.assertTrue(png("graphics/pokemon/noxichu/"+name+".png",*size))
    def test_noxichu_front_and_icon_palettes_match(self):
        normal=(ROOT/"graphics/pokemon/noxichu/normal.pal").read_text()
        icon=(ROOT/"graphics/pokemon/icon_palettes/pal6.pal").read_text()
        self.assertEqual(normal,icon)
        self.assertIn("49 47 55",normal)
    def test_approved_rocket_two_person_picture(self):
        j=png("graphics/trainers/front_pics/prl_jessie.png",64,64)
        self.assertEqual(j,png("graphics/trainers/front_pics/prl_james.png",64,64))
        self.assertEqual(hashlib.sha256(j).hexdigest(),"37321dc53d66139cb466971b75b9d5764f6148ea45f589617fa7bb4f316006a2")
    def test_rocket_actor_staging(self):
        for name,tag in [("MtMoon_B1F_Frlg","MT_MOON"),("RocketHideout_B4F_Frlg","HIDEOUT"),("SilphCo_11F_Frlg","SILPH"),("VictoryRoad_3F_Frlg","VICTORY_ROAD")]:
            with self.subTest(name=name):
                j=json.loads((ROOT/"data/maps"/name/"map.json").read_text())
                actors={o.get('local_id'):o for o in j['object_events']}
                x=actors['LOCALID_PRL_JESSIE_'+tag]
                y=actors['LOCALID_PRL_JAMES_'+tag]
                m=actors['LOCALID_PRL_MEOWTH_'+tag]
                self.assertEqual(x["y"],y["y"])
                self.assertNotEqual((m["x"],m["y"]),(x["x"],x["y"]))
                self.assertNotEqual((m["x"],m["y"]),(y["x"],y["y"]))
                script=(ROOT/"data/maps"/name/"scripts.inc").read_text()
                if tag == "MT_MOON":
                    self.assertIn("localId=LOCALID_PRL_JESSIE_MT_MOON",script)
                    self.assertIn("localId=LOCALID_PRL_JAMES_MT_MOON",script)
                else:
                    self.assertIn("localId=LOCALID_PRL_JESSIE_"+tag,script)
                    self.assertIn("localId=LOCALID_PRL_JAMES_"+tag,script)
                    self.assertIn("Text_PRL_JessiePostBattle::",script)
                    self.assertIn("Text_PRL_JamesPostBattle::",script)
    def test_mt_moon_revealed_before_object_spawn_for_existing_save(self):
        scene=(ROOT/"data/maps/MtMoon_B1F_Frlg/scripts.inc").read_text()
        self.assertIn("map_script MAP_SCRIPT_ON_LOAD, MtMoon_B1F_OnLoad_PRLJessieJames",scene)
        self.assertIn("goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON",scene)
        self.assertIn("goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON",scene)
        self.assertIn("clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON",scene)
        self.assertIn("setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON",scene)
        m=json.loads((ROOT/"data/maps/MtMoon_B1F_Frlg/map.json").read_text())
        rocket={a.get("local_id"):a for a in m["object_events"]}
        self.assertEqual((rocket["LOCALID_PRL_JESSIE_MT_MOON"]["x"],rocket["LOCALID_PRL_JESSIE_MT_MOON"]["y"]),(42,2))
        self.assertEqual((rocket["LOCALID_PRL_JAMES_MT_MOON"]["x"],rocket["LOCALID_PRL_JAMES_MT_MOON"]["y"]),(43,2))
        self.assertEqual((rocket["LOCALID_PRL_MEOWTH_MT_MOON"]["x"],rocket["LOCALID_PRL_MEOWTH_MT_MOON"]["y"]),(44,2))
        self.assertEqual(m["coord_events"],[])
        for who in ("JESSIE","JAMES"):
            self.assertEqual(rocket["LOCALID_PRL_"+who+"_MT_MOON"]["trainer_type"],"TRAINER_TYPE_NORMAL")
        for who in ("Jessie","James"):
            self.assertIn("MtMoon_B1F_EventScript_PRL_"+who+"::",scene)
            self.assertIn("MtMoon_B1F_Text_PRL_"+who+"PostBattle::",scene)
        self.assertNotIn("removeobject LOCALID_PRL_JESSIE_MT_MOON",scene)

    def test_opening_untouched(self):
        self.assertIn("CB2_InitPRLHwlScene0",(ROOT/"src/expansion_intro.c").read_text())
if __name__=="__main__": unittest.main()
