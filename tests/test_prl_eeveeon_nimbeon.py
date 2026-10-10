import pathlib
import re
import struct
import unittest

R=pathlib.Path(__file__).resolve().parents[1]

class ApprovedEeveelutions(unittest.TestCase):
    def test_genuine_normal_sprite_dimensions(self):
        for mon in ("eeveeon","nimbeon"):
            for name,w,h in (("front.png",64,64),("back.png",64,64),("icon.png",32,64)):
                data=(R/"graphics"/"pokemon"/mon/name).read_bytes()
                self.assertEqual(data[:8],b"\x89PNG\r\n\x1a\n")
                self.assertEqual(struct.unpack(">II",data[16:24]),(w,h))
    def test_evolution_and_strongest_stats(self):
        s=(R/"src/data/pokemon/species_info/gen_1_families.h").read_text()
        self.assertIn("{EVO_ITEM, ITEM_ANCIENT_STONE, SPECIES_EEVEEON}",s)
        self.assertIn("{EVO_LEVEL, 0, SPECIES_NIMBEON",s)
        specs=(R/"src/data/pokemon/species_info/prl_custom.h").read_text()
        block=specs.split("[SPECIES_EEVEEON] =",1)[1].split("[SPECIES_NIMBEON] =",1)[0]
        for k,v in {"baseHP":100,"baseAttack":110,"baseDefense":85,"baseSpAttack":110,"baseSpDefense":85,"baseSpeed":110}.items():
            self.assertRegex(block,r"\."+k+r"\s*=\\s*"+str(v)+r"\b")
        self.assertIn("ABILITY_ADAPTABILITY",block)
        self.assertIn("ABILITY_NORMALIZE",block)
    def test_elevated_eeveeon_move(self):
        s=(R/"src/data/moves_info.h").read_text()
        b=s.split("[MOVE_EEVOBOOST] =",1)[1].split("    },",1)[0]
        self.assertIn(".priority = -2",b)
        self.assertIn(".pp = 2",b)
    def test_kanto_roster_has_signature_evolutions(self):
        s=(R/"src/data/trainers_frlg.party").read_text()
        for tag, mons in {"TOWER":("Omeon","Espeon","Umbreon"),"ROUTE23":("Toxeon","Eeveeon","Nimbeon")}.items():
            block=s.split("=== TRAINER_STEEVE_NEEVEE_"+tag+" ===",1)[1].split("=== TRAINER_",1)[0]
            for mon in mons:self.assertIn(mon,block)

if __name__=="__main__":
    unittest.main()
