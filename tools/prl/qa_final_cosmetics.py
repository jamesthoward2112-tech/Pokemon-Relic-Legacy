#!/usr/bin/env python3
"""Exercise trainer-exclusive rendering inside a live mGBA battle.

Fixture selection changes enemy mon and trainer parameters to exercise every
approved renderer. This tests graphics, not defeating each trainer's party.
"""
import argparse,json,struct,hashlib
from pathlib import Path
from qa_great_badge_heist import QA,contact_sheet

ROOT=Path(__file__).resolve().parents[2]
CASES=[(632,112,5,'brock_rhydon'),(633,121,5,'misty_starmie'),(634,26,5,'surge_raichu'),(635,71,5,'erika_victreebel'),(636,31,5,'nidoqueen_y'),(637,65,5,'sabrina_alakazam'),(638,59,5,'blaine_arcanine'),(639,128,0,'tauros_x'),(639,127,1,'purakkusu_pinsir'),(639,141,2,'kabutops_p'),(639,142,3,'aerodactyl_p'),(639,31,4,'nidoqueen_y'),(639,34,5,'nidoking_x'),(310,131,5,'lorelei_lapras'),(311,68,5,'bruno_machamp'),(312,94,5,'shiny_mega_gengar'),(313,149,5,'lance_dragonite'),(338,18,0,'mega_pidgeot'),(338,65,3,'shiny_mega_alakazam'),(338,34,4,'nidogod'),(338,59,5,'champion_arcanine'),(630,110,1,'rocket_weezing_blue'),(631,110,1,'rocket_weezing_fire')]

def unpack(data):
 assert data[0]==16
 length=int.from_bytes(data[1:4],'little');out=bytearray();pos=4
 while len(out)<length:
  flags=data[pos];pos+=1
  for bit in range(8):
   if len(out)>=length:break
   if flags&(128>>bit):
    a,b=data[pos:pos+2];pos+=2;count=(a>>4)+3;distance=((a&15)<<8|b)+1
    for _ in range(count):out.append(out[-distance])
   else:out.append(data[pos]);pos+=1
 return bytes(out)

def main():
 p=argparse.ArgumentParser();p.add_argument('rom');p.add_argument('--elf',required=True);p.add_argument('--save',required=True);p.add_argument('--probe',required=True);p.add_argument('--out',required=True);p.add_argument('--nm',default='arm-none-eabi-nm')
 qa=QA(p.parse_args());emu=qa.open();frames=[];results={}
 try:
  qa.load_existing_save(emu);qa.setup_party(emu,(1574,1575))
  qa.call(emu,'ClearTrainerFlag',632);emu.write(qa.sym['gSelectedObjectEvent'],1,1);emu.write(qa.sym['gSpecialVar_LastTalked'],1,2)
  qa.call(emu,'ScriptContext_SetupScript',qa.sym['PewterCity_Gym_Frlg_EventScript_PRLBadgeRecovery'])
  for _ in range(30):emu.frame(80);emu.press('A',after=40)
  enemy=emu.read(qa.sym['gEnemyPartyPtr'],4)
  manager=emu.read(qa.sym['gMonSpritesGfxPtr'],4)
  assert 0x2000000<=manager<0x2040000
  position=emu.read(qa.sym['gBattlerPositions']+1,1)
  gfx=emu.read(manager+4+position*4,4)
  sprite=qa.sym['gSprites']+emu.read(qa.sym['gBattlerSpriteIds']+1,1)*68
  for trainer,species,index,name in CASES:
   emu.write(qa.sym['gTrainerBattleParameter']+2,trainer,2)
   qa.call(emu,'CreateRandomMon',enemy+index*100,species,68)
   emu.write(qa.sym['gBattlerPartyIndexes']+2,index,2)
   qa.call(emu,'BattleLoadMonSpriteGfx',enemy+index*100,1)
   asset=ROOT/'graphics/prl_kanto_bosses'/name
   expected=unpack(asset.with_suffix('.4bpp.lz').read_bytes())
   assert emu.readbytes(gfx,len(expected))==expected,(name,'wrong front tiles',emu.readbytes(gfx,16).hex(),expected[:16].hex(), hex(gfx), position)
   palette=asset.with_suffix('.gbapal').read_bytes()[:32]
   assert emu.readbytes(qa.sym['gPlttBufferUnfaded']+(256+16)*2,32)==palette,(name,'wrong palette',emu.readbytes(qa.sym['gPlttBufferUnfaded']+(256+16)*2,32).hex(),palette.hex())
   qa.call(emu,'StartSpriteAnim',sprite,0)
   emu.frame(12)
   frames.append((name,emu.image()))
   results[name]={'trainer':trainer,'species':species,'party_index':index,'decoded_tiles_match':True,'palette_match':True}
  # Only the duplicate-species aces receive their special artwork.
  for trainer,species,name in [(633,121,'misty_starmie'),(635,71,'erika_victreebel'),(638,59,'blaine_arcanine')]:
   emu.write(qa.sym['gTrainerBattleParameter']+2,trainer,2);emu.write(qa.sym['gBattlerPartyIndexes']+2,4,2)
   qa.call(emu,'CreateRandomMon',enemy+400,species,64);qa.call(emu,'BattleLoadMonSpriteGfx',enemy+400,1)
   assert emu.readbytes(gfx,2048)!=unpack((ROOT/'graphics/prl_kanto_bosses'/name).with_suffix('.4bpp.lz').read_bytes())[:2048]
  # The player side must always retain the underlying species' normal back.
  player=emu.read(qa.sym['gPlayerPartyPtr'],4);qa.call(emu,'CreateRandomMon',player,112,68)
  emu.write(qa.sym['gTrainerBattleParameter']+2,632,2);qa.call(emu,'BattleLoadMonSpriteGfx',player,0)
  playergfx=emu.read(manager+4,4)
  assert emu.readbytes(playergfx,2048)!=unpack((ROOT/'graphics/prl_kanto_bosses/brock_rhydon.4bpp.lz').read_bytes())[:2048]
  # Original Giovanni keeps both existing skeletons, separate from recovery.
  for species,name in [(141,'kabutops'),(142,'aerodactyl')]:
   trainerid=414
   # Resolve the actual original trainer constant from the approved source.
   import re
   trainerid=int(re.search(r'#define TRAINER_LEADER_GIOVANNI\s+(\d+)',(ROOT/'include/constants/opponents_frlg.h').read_text()).group(1))
   emu.write(qa.sym['gTrainerBattleParameter']+2,trainerid,2);qa.call(emu,'CreateRandomMon',enemy,species,68);emu.write(qa.sym['gBattlerPartyIndexes']+2,0,2);qa.call(emu,'BattleLoadMonSpriteGfx',enemy,1)
   expected=unpack((ROOT/f'graphics/battle_transitions/prl/giovanni_skeleton_{name}.4bpp.lz').read_bytes())
   assert emu.readbytes(gfx,len(expected))==expected
   qa.call(emu,'StartSpriteAnim',sprite,0);emu.frame(12);frames.append(('original skeleton '+name,emu.image()))
  contact_sheet(frames,qa.out/'boss-render-contact.png')
  (qa.out/'results.json').write_text(json.dumps({'renders':results,'ace_only_and_player_exclusion':True,'original_giovanni_skeletons_preserved':True},indent=2))
  print(json.dumps({'render_checks':len(CASES),'original_skeletons':2,'scope_checks':4}))
 finally:emu.close()
if __name__=='__main__':main()
