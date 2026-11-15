"""PRU Chansey Twins port uses exact approved art in PRR's existing Kanto upstairs."""
from pathlib import Path
import hashlib,re
import unittest
ROOT=Path(__file__).resolve().parents[2]
SITES="VIRIDIAN PEWTER ROUTE4 CERULEAN VERMILION ROUTE10 LAVENDER CELADON SAFFRON FUCHSIA CINNABAR INDIGO".split()
SCRIPT=(ROOT/"data/scripts/prl_center_chansey_training.inc").read_text(encoding="utf8")
PARTIES=(ROOT/"src/data/trainers_frlg.party").read_text(encoding="utf8")
class ChanseyTwinsTests(unittest.TestCase):
 def test_all_kanto_upstairs_coaches_are_repeatable_double_chansey(self):
  for site in SITES:
   tag="TRAINER_PRL_CHANSEY_"+site
   section=PARTIES.split("=== "+tag+" ===",1)[1].split("\n===",1)[0]
   self.assertIn("Double Battle: Yes",section,site)
   self.assertIn("Pic: PRR Chansey Twins",section,site)
   self.assertEqual(2,len(re.findall(r"(?m)^Chansey$",section)),site)
   self.assertEqual(2,len(re.findall(r"(?m)^Level: \d+$",section)),site)
   self.assertIn("cleartrainerflag "+tag,SCRIPT)
   self.assertIn("trainerbattle_double "+tag,SCRIPT)
   self.assertIn("PRL_Chansey_TextNeedTwo",SCRIPT)

 def test_sevii_is_not_silently_changed(self):
  for site in "ONE_ISLAND TWO_ISLAND THREE_ISLAND FOUR_ISLAND FIVE_ISLAND SIX_ISLAND SEVEN_ISLAND".split():
   self.assertIn("trainerbattle_single TRAINER_PRL_CHANSEY_"+site,SCRIPT)

 def test_original_custom_sprite_bytes_and_registry(self):
  pic=ROOT/"graphics/trainers/front_pics/pru_chansey_twins.png"
  self.assertEqual("aabcca878b1e775d67886aa869e386fc9af99176945af1c377924654393123b1",
                   hashlib.sha256(pic.read_bytes()).hexdigest())
  reg=(ROOT/"src/data/graphics/trainers.h").read_text(encoding="utf8")
  self.assertIn("TRAINER_PIC_PRR_CHANSEY_TWINS",reg)
  self.assertIn("gTrainerFrontPic_PRRChanseyTwins",reg)
  self.assertIn("gTrainerPalette_PRRChanseyTwins",reg)

 def test_original_kanto_upstairs_encounters_preserved(self):
  import json
  folders=['ViridianCity','PewterCity','CeruleanCity','VermilionCity',
           'LavenderTown','CeladonCity','SaffronCity','FuchsiaCity','CinnabarIsland','IndigoPlateau','Route4','Route10']
  for loc in folders:
   mapfile=ROOT/"data/maps"/(loc+"_PokemonCenter_2F_Frlg")/"map.json"
   obj=json.loads(mapfile.read_text(encoding="utf8"))
   npcs=[o for o in obj["object_events"] if o["script"].startswith("EventScript_PRL_Chansey_")]
   self.assertEqual(1,len(npcs),loc)
   self.assertEqual([10,2],[npcs[0]["x"],npcs[0]["y"]],loc)

if __name__=="__main__":unittest.main()
