"""Regression checks for approved PRU QoL without changing PRR's roster."""
from pathlib import Path
import re
import unittest

ROOT=Path(__file__).resolve().parents[2]
def src(path):
 return (ROOT/path).read_text(encoding="utf8")

class PRUCarryoverQoLTests(unittest.TestCase):
 def test_iv_ev_numeric_stats_and_in_summary_screen(self):
  s=src("include/config/summary_screen.h")
  self.assertRegex(s,r"(?m)^#define P_SUMMARY_SCREEN_IV_EV_INFO\s+TRUE\b")
  self.assertRegex(s,r"(?m)^#define P_SUMMARY_SCREEN_IV_EV_VALUES\s+TRUE\b")
  self.assertIn("#define P_SUMMARY_SCREEN_MOVE_RELEARNER  TRUE",s)

 def test_evolution_items_available_from_bag(self):
  s=src("include/config/item.h")
  self.assertRegex(s,r"(?m)^#define I_USE_EVO_HELD_ITEMS_FROM_BAG\s+TRUE\b")

 def test_dexnav_has_pru_timeout_but_keeps_search_mechanic(self):
  s=src("include/config/dexnav.h")
  self.assertRegex(s,r"(?m)^#define DEXNAV_TIMEOUT\s+30\b")
  self.assertRegex(s,r"(?m)^#define DEXNAV_ENABLED\s+TRUE\b")

 def test_one_button_fishing_no_follower_bonus(self):
  s=src("include/config/fishing.h")
  self.assertRegex(s,r"(?m)^#define I_FISHING_MINIGAME\s+GEN_2\b")
  self.assertRegex(s,r"(?m)^#define I_FISHING_FOLLOWER_BOOST\s+FALSE\b")
  self.assertIn("case GEN_2:",src("src/fishing.c"))
  self.assertIn("task->tStep = FISHING_A_PRESS_NO_MINIGAME;",src("src/fishing.c"))

 def test_nurse_heals_immediately_and_whiteout_still_animates(self):
  s=src("data/scripts/pkmn_center_nurse_frlg.inc")
  normal=s.split("EventScript_PkmnCenterNurse_Frlg::",1)[1].split("EventScript_PkmnCenterNurse_HealPkmn_Frlg::",1)[0]
  self.assertIn("special HealPlayerParty",normal)
  self.assertNotIn("dofieldeffect",normal)
  self.assertNotIn("msgbox",normal)
  self.assertIn("call EventScript_PkmnCenterNurse_TakeAndHealPkmn_Frlg",s.split("EventScript_AfterWhiteOutHeal_Frlg::",1)[1].split("EventScript_PkmnCenterNurse_Frlg::",1)[0])

 def test_following_pokemon_are_explicitly_disabled(self):
  s=src("include/config/overworld.h")
  self.assertRegex(s,r"(?m)^#define OW_FOLLOWERS_ENABLED\s+FALSE\b")

 def test_evo_expert_and_repel_prompt_already_present(self):
  s=src("data/maps/CeladonCity_Condominiums_RoofRoom_Frlg/scripts.inc")
  self.assertIn("EventScript_PRLEeveelutionsExpert",s)
  self.assertIn("ITEM_ANCIENT_STONE",s)
  p=src("include/config/item.h")
  self.assertRegex(p,r"(?m)^#define I_REPEL_LURE_MENU\s+TRUE\b")

 def test_existing_town_map_fly_is_gated_and_reuses_prl(self):
  s=src("src/field_region_map.c")
  self.assertIn("PRL_CanFlyDirectlyFromMap(void)",s)
  self.assertIn("case MAP_INPUT_R_BUTTON:",s)
  self.assertIn("SetFlyDestination(&sFieldRegionMapHandler->regionMap)",s)
  self.assertIn("IsFieldMoveUnlocked(FIELD_MOVE_FLY)",s)
  self.assertIn("CheckBagHasItem(hm, 1)",s)

if __name__=="__main__":
 unittest.main()
