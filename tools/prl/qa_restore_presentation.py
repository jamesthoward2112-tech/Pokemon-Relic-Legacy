#!/usr/bin/env python3
"""Focused mGBA checks using a disposable old save. No ROM patch is used.
Native function calls set up test contexts, then real GBA input/frames test them.
"""
import argparse,json,subprocess,struct,re,shutil,sys
from pathlib import Path
from PIL import Image,ImageDraw
from mgba_qa import Emulator
p=argparse.ArgumentParser()
for n in ('rom','elf','save','probe','out'):p.add_argument('--'+n,required=True)
a=p.parse_args();out=Path(a.out);out.mkdir(parents=True,exist_ok=True);shutil.copyfile(a.save,out/'test.sav')
s={x.split()[-1]:int(x.split()[0],16) for x in subprocess.check_output(['arm-none-eabi-nm',a.elf],text=True).splitlines() if len(x.split())==3}
e=Emulator(a.rom,out/'test.sav',probe=a.probe);report={'save_size':Path(a.save).stat().st_size,'checks':{}}
def call(name,*args):
    import ctypes as C
    e.lib.prl_call.argtypes=[C.c_uint, C.POINTER(C.c_uint), C.c_uint]
    e.lib.prl_call.restype=C.c_uint
    values=(C.c_uint*len(args))(*args)
    return e.lib.prl_call(s[name],values,len(args))

def fields(address,box=False):
    getter='GetBoxMonData3' if box else 'GetMonData3'
    return {'moves':[call(getter,address,20+i,0) for i in range(4)],'pp':[call(getter,address,24+i,0) for i in range(4)],'bonus':call(getter,address,28,0)}
def swapped(b):
    c={k:list(v) if isinstance(v,list) else v for k,v in b.items()}
    for k in ('moves','pp'):c[k][0],c[k][1]=c[k][1],c[k][0]
    c['bonus']=(b['bonus']&~15)|((b['bonus']&3)<<2)|((b['bonus']>>2)&3);return c

def bonus(address,box=False):
    scratch=0x0203fff0;old=e.read(scratch);e.write(scratch,0xe4);call('SetBoxMonData' if box else 'SetMonData',address,28,scratch);e.write(scratch,old)
def boot():
    e.frame(2400);e.press('START',after=120);e.press('A',after=200)
def reorder():
    e.press('A');e.press('A');e.press('DOWN');e.press('A')
def save(name):
    e.press('B',after=120);e.frame(120);e.write(s['sStartMenuCursorPos'],0,1);e.press('START',after=100);e.shot(out/(name+'-menu.png'))
    for _ in range(5):e.press('DOWN')
    e.press('A',after=100);e.shot(out/(name+'-prompt.png'))

    for _ in range(5):e.press('A',after=120)
    e.frame(300);e.shot(out/(name+'-saved.png'))
    e.lib.prl_save(str(out/name).encode())
def restart(name):
    global e
    e.close();e=Emulator(a.rom,out/name,probe=a.probe);boot()
boot();e.frame(800);e.shot(out/'old-save-loaded.png');e.state(str(out/'overworld.state'))
party=s['gParties'];bonus(party);b=fields(party)
e.press('START');e.press('DOWN');e.press('DOWN');e.press('A',after=100);e.press('A');e.press('A',after=100);e.press('RIGHT');e.press('RIGHT');e.shot(out/'party-before.png');reorder();e.shot(out/'party-after.png');c=fields(party);assert c==swapped(b),(b,c)
e.press('B');e.press('B',after=100);e.press('A',after=100);e.press('RIGHT');e.press('RIGHT');assert fields(party)==c
report['checks']['party_moves_pp_and_pp_ups_swap']=True
# Return to the field, save with the game's standard save routine, then cold boot.
e.press('B',after=100);e.press('B',after=100);e.press('B',after=100);save('reordered.sav');restart('reordered.sav');assert fields(party)==c
report['checks']['party_swap_survives_save_and_cold_restart']=True
box=call('GetBoxedMonPtr',0,0);stride=call('GetBoxedMonPtr',0,1)-box
index=next(i for i in range(30) if call('GetBoxMonData3',box+i*stride,18,0));mon=box+index*stride;bonus(mon,True);b=fields(mon,True);assert b['moves'][1]
# The actual boxed Summary, actual stored BoxPokemon, normal box mode and GBA keys.
call('ShowPokemonStorageSystemPC');e.frame(60);e.press('A',after=180);e.shot(out/'pc-storage.png');e.press('A');e.press('DOWN');e.press('A',after=120);e.shot(out/'pc-summary-open.png');e.press('RIGHT');e.press('RIGHT');e.shot(out/'pc-before.png');reorder();e.shot(out/'pc-after.png');c=fields(mon,True);assert c==swapped(b),(b,c)
e.press('B');e.press('B',after=160);e.shot(out/'pc-storage-returned.png');e.press('B',after=100);e.shot(out/'pc-exit-prompt.png');e.press('DOWN');e.press('A',after=240);e.shot(out/'pc-exit-menu.png');e.press('B',after=160);save('pc-reordered.sav');restart('pc-reordered.sav');assert fields(call('GetBoxedMonPtr',0,index),True)==c
report['checks']['pc_moves_pp_and_pp_ups_swap_and_restart']=True
for tag,trainer in [('standard',1),('rocket',251),('jessie_james',628),('eevee_twins',624)]:
    e.state(str(out/'overworld.state'),False);e.writebytes(s['gTrainerBattleParameter'],bytes(44));e.write(s['gTrainerBattleParameter']+2,trainer,2);print(tag,'transition',call('GetTrainerBattleTransition'),flush=True);call('BattleSetup_StartTrainerBattle')
    for f in range(420):
        e.frame()
        if f in tuple(range(0,161,10))+(180,230,360,419):e.shot(out/f'{tag}-{f}.png')
    e.press('A',after=120);e.shot(out/f'{tag}-battle.png');e.state(str(out/f'{tag}-battle.state'));report['checks'][tag+'_native_battle_started']=True
# Verify all donor assignments and all decoded VRAM/palette bytes in a running battle.
e.state(str(out/'standard-battle.state'),False);h=Path('src/data/prl_crystal_battle_visuals.h').read_text();defaults=list(map(int,re.search(r'sCrystalDefaultVisuals.*?\{(.*?)\}',h,re.S)[1].replace('\n','').split(',')[:-1]));rows=[tuple(map(int,m)) for m in re.findall(r'^    \{ (\d+), (\d+), (\d+) \},',h,re.M)]
contexts={d:(env,34,0) for env,d in enumerate(defaults)}
for g,n,d in rows:contexts.setdefault(d,(7,g,n))
assert set(contexts)==set(range(38));sb=e.read(s['gSaveBlock1Ptr']);images=[];sys.path.insert(0,'tools');from extract_crystal_redux_battle_envs import decompress
for donor,(env,g,n) in sorted(contexts.items()):
    e.state(str(out/'standard-battle.state'),False)
    e.write(s['gBattleTypeFlags'],0);e.write(sb+4,g,1);e.write(sb+5,n,1);visual=call('GetCrystalBattleVisual',env);assert visual==s['sCrystalBattleVisuals']+donor*64,(donor,hex(visual))
    e.write(s['gMapHeader']+0x1b,0,1);e.write(s['gBattleEnvironment'],env,1);call('DrawMainBattleBackground');call('DrawBattleEntryBackground');folder=Path('graphics/battle_environment/crystal_redux')/f'{donor:02}'
    for file,address in [('tiles.4bpp.lz',0x06008000),('tilemap.bin.lz',0x0600d000),('entry_tiles.4bpp.lz',0x06004000),('entry_tilemap.bin.lz',0x0600e000)]:
        data,_=decompress((folder/file).read_bytes(),0);assert e.readbytes(address,len(data))==data,(donor,file)
    assert e.readbytes(s['gPlttBufferUnfaded']+64,96)==(folder/'palette_48colors.gbapal').read_bytes();call('SetGpuReg',0,(e.read(0x04000000,2)|0x800)&~0x600);e.frame(2);e.shot(out/f'background-{donor:02}.png');images.append(e.image())
report['checks']['all_38_backgrounds_entry_tiles_tilemaps_and_palettes_loaded_in_mgba']=True
sheet=Image.new('RGB',(240*5,180*8));draw=ImageDraw.Draw(sheet)
for j,i in enumerate(images):x=j%5*240;y=j//5*180;sheet.paste(i,(x,y));draw.text((x+4,y+162),f'Donor {j:02}',fill='white')
sheet.save(out/'all-38-backgrounds.png');(out/'results.json').write_text(json.dumps(report,indent=2));e.close();print(json.dumps(report,indent=2))
