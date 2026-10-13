import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class FinalBossCosmetics(unittest.TestCase):
 def test_cosmetics_are_scoped_to_boss_species_and_ace_index(self):
  s=(ROOT/'src/data/prl_kanto_boss_graphics.h').read_text()
  self.assertIn('IsOnPlayerSide(battler)',s)
  self.assertIn('BATTLE_TYPE_TRAINER',s)
  for name in ['BROCK','MISTY','LT_SURGE','ERIKA','KOGA','SABRINA','BLAINE','GIOVANNI']:
   self.assertIn('TRAINER_PRL_BADGE_RECOVERY_'+name,s)
  self.assertIn('gBattlerPartyIndexes[battler] == 5',s)
  self.assertIn('TRAINER_JESSIE_JAMES_SILPH',s)
  self.assertIn('rocket_weezing_blue',s)
  self.assertIn('rocket_weezing_fire',s)
 def test_skeletons_are_preserved_only_for_original_giovanni(self):
  s=(ROOT/'src/battle_gfx_sfx_util.c').read_text()
  p=s.split('static u8 PRLGetGiovanniSkeletonGfx',1)[1].split('static const u32 *',1)[0]
  self.assertIn('TRAINER_LEADER_GIOVANNI',p)
  self.assertNotIn('TRAINER_PRL_BADGE_RECOVERY_GIOVANNI',p)
 def test_assets_have_two_front_frames_and_one_palette(self):
  folder=ROOT/'graphics/prl_kanto_bosses'
  for front in folder.glob('*.4bpp.lz'):
   with self.subTest(asset=front.name):
    data=front.read_bytes()
    self.assertEqual(data[0],16)
    self.assertEqual(int.from_bytes(data[1:4],'little'),4096)
    self.assertEqual(len(front.with_suffix('').with_suffix('.gbapal').read_bytes()),32)
  self.assertIn('$(wildcard graphics/prl_kanto_bosses/*.4bpp.lz)', (ROOT/'graphics_file_rules.mk').read_text())
