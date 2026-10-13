#!/usr/bin/env python3
"""Inspect actual battle-created teams in mGBA against the approved manifest.

Each disposable save starts a normal shipped trainerbattle script. No enemy
party creation is called manually, and battle outcome/Hall of Fame flags are
never injected. This tool verifies team construction and battle mode only.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path
from qa_great_badge_heist import QA

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / 'tools/prl/final_kanto_parties.json'
GYMS = ['PewterCity', 'CeruleanCity', 'VermilionCity', 'CeladonCity', 'FuchsiaCity', 'SaffronCity', 'CinnabarIsland', 'ViridianCity']
ROOMS = ['LoreleisRoom', 'BrunosRoom', 'AgathasRoom', 'LancesRoom']

def cname(prefix, name):
    return prefix + '_' + name.upper().replace(' ', '_').replace('-', '_')

class PartyQA(QA):
    def __init__(self, args):
        super().__init__(args)
        self.manifest = json.loads(MANIFEST.read_text())
        (self.out/'runtime-provenance.json').write_text(json.dumps({'rom':str(Path(args.rom).resolve()),'rom_sha256':hashlib.sha256(Path(args.rom).read_bytes()).hexdigest(),'elf_sha256':hashlib.sha256(Path(args.elf).read_bytes()).hexdigest(),'manifest_sha256':hashlib.sha256(MANIFEST.read_bytes()).hexdigest()},indent=2)+'\n')
        names = {'MON_DATA_SPECIES', 'MON_DATA_HELD_ITEM', 'MON_DATA_LEVEL', 'MON_DATA_IS_SHINY', 'BATTLE_TYPE_DOUBLE', 'BATTLE_TYPE_TRAINER', 'VAR_STARTER_MON'}
        names.update(['MON_DATA_HP', 'MON_DATA_MAX_HP', 'MON_DATA_SPATK', 'MON_DATA_SPEED', 'MON_DATA_DEF', 'MON_DATA_SPDEF', 'MON_DATA_PP1', 'MON_DATA_PP2', 'MON_DATA_PP3', 'MON_DATA_PP4', 'MON_DATA_CHAMPION_RIBBON', 'MOVE_PSYCHIC', 'MOVE_DAZZLING_GLEAM', 'VAR_TEMP_1', 'MOVE_TAILWIND', 'MOVE_REFLECT', 'MOVE_SLEEP_POWDER', 'MOVE_PROTECT', 'MOVE_FAKE_OUT', 'MOVE_NUZZLE', 'MOVE_DRAGON_DANCE'])
        names.update('MON_DATA_MOVE' + str(i) for i in range(1, 5))
        names.update('MON_DATA_' + stat + '_IV' for stat in ['HP', 'ATK', 'DEF', 'SPATK', 'SPDEF', 'SPEED'])
        for trainer in self.manifest['trainers'].values():
            for mon in trainer['party']:
                names.update([cname('SPECIES', mon['species']), cname('ABILITY', mon['ability']), cname('ITEM', mon['item'])])
                names.update(cname('MOVE', move) for move in mon['moves'])
        # Resolve enum constants with the host C compiler from the same source
        # headers, avoiding assumptions about expansion item/move numbering.
        enum = re.search(r'enum MonData\s*\{.*?\};', (ROOT / 'include/pokemon.h').read_text(), re.S).group(0)
        c = '#include <stdio.h>\n' + ''.join(f'#include "constants/{header}.h"\n' for header in ['species', 'abilities', 'moves', 'items', 'pokemon', 'battle', 'vars']) + enum + '\nint main(void) {\n'
        c += ''.join(f'printf("{name}=%u\\n", (unsigned){name});\n' for name in sorted(names)) + 'return 0;}\n'
        path = self.out / 'party_constants.c'; path.write_text(c)
        binary = self.out / 'party_constants'
        subprocess.run(['gcc', '-I', str(ROOT / 'include'), str(path), '-o', str(binary)], check=True)
        self.ids = dict((name, int(value)) for name, value in (line.split('=') for line in subprocess.check_output([str(binary)], text=True).splitlines()))

    def get(self, emu, mon, field):
        return self.call(emu, 'GetMonData3', mon, self.ids[field], 0)

    def check(self, trainer_id, trainer, label, starter=None):
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.setup_party(emu, (150, 151, 143, 130, 6, 9))
            self.call(emu, 'ClearTrainerFlag', trainer['trainer_id'])
            self.set_flag(emu, 0x4D9)
            for flag in range(0x4DA, 0x4E1): self.set_flag(emu, flag)
            self.set_flag(emu, 0x4E1, False)
            self.set_flag(emu, 0x4E2, False)
            self.set_flag(emu, 0x055, False)
            # Use an inactive object slot for trainer command facing bookkeeping.
            emu.write(self.sym['gSelectedObjectEvent'], 1, 1)
            emu.write(self.sym['gSpecialVar_LastTalked'], 8 if 'GIOVANNI' in trainer_id else 1, 2)
            if starter is not None:
                self.call(emu, 'VarSet', self.ids['VAR_STARTER_MON'], starter)
            self.call(emu, 'ScriptContext_SetupScript', self.sym[label])
            enemy = emu.read(self.sym['gEnemyPartyPtr'], 4)
            for step in range(100):
                emu.frame(60)
                # Packed TrainerBattleParameter: flags byte, local ID byte, u16 opponentA.
                opponent = emu.read(self.sym['gTrainerBattleParameter'] + 2, 2)
                species = [self.get(emu, enemy + 100*i, 'MON_DATA_SPECIES') for i in range(6)]
                expected = [self.ids[cname('SPECIES', mon['species'])] for mon in trainer['party']]
                flags = emu.read(self.sym['gBattleTypeFlags'], 4)
                if opponent == trainer['trainer_id'] and species == expected and flags & self.ids['BATTLE_TYPE_TRAINER']:
                    break
                emu.press('A', after=12)
            else:
                raise AssertionError((trainer_id, 'battle party did not appear', opponent, species, flags))
            rows = []
            for i, expected in enumerate(trainer['party']):
                mon = enemy + 100*i
                observed = dict(
                    species=self.get(emu, mon, 'MON_DATA_SPECIES'),
                    level=self.get(emu, mon, 'MON_DATA_LEVEL'),
                    ability=self.call(emu, 'GetMonAbility', mon),
                    item=self.get(emu, mon, 'MON_DATA_HELD_ITEM'),
                    shiny=bool(self.get(emu, mon, 'MON_DATA_IS_SHINY')),
                    moves=[self.get(emu, mon, 'MON_DATA_MOVE'+str(j)) for j in range(1,5)],
                    ivs=[self.get(emu, mon, 'MON_DATA_'+stat+'_IV') for stat in ['HP','ATK','DEF','SPATK','SPDEF','SPEED']],
                )
                wanted=dict(expected)
                for key, prefix in [('species','SPECIES'),('ability','ABILITY'),('item','ITEM')]: wanted[key]=self.ids[cname(prefix, expected[key])]
                wanted['moves']=[self.ids[cname('MOVE', move)] for move in expected['moves']]
                assert observed == wanted, (trainer_id, i, observed, wanted)
                rows.append(observed)
            assert bool(flags & self.ids['BATTLE_TYPE_DOUBLE']) == trainer['double_battle'], (trainer_id, flags)
            # Advance only the normal introduction, then capture actual engine state.
            emu.frame(240)
            emu.shot(self.out / (trainer_id + '.png'))
            self.results[trainer_id]={'trainer_id':trainer['trainer_id'], 'battle_flags':flags, 'party_created_by_battle':rows, 'screenshot':trainer_id+'.png'}
            (self.out / 'results.json').write_text(json.dumps(self.results, indent=2)+'\n')
        finally: emu.close()

    def champion_win(self, starter=0):
        """Win through normal battle input, then observe the shipped HOF path."""
        emu=self.open()
        evidence={'won':False, 'hall_of_fame_map':False, 'hall_of_fame_callback':False, 'support_moves_observed':[]}
        try:
            self.load_existing_save(emu)
            self.call(emu, 'VarSet', self.ids['VAR_STARTER_MON'], starter)
            for flag in (0x844, 0x82C, 0x4BC): self.set_flag(emu, flag, False)
            self.call(emu, 'ClearTrainerFlag', 338+starter)
            party=emu.read(self.sym['gPlayerPartyPtr'],4)
            count=emu.read(self.sym['gPlayerPartyCountPtr'],4)
            scratch=0x0203FFF0
            def setmon(mon,field,value):
                emu.write(scratch,value,4)
                self.call(emu,'SetMonData',mon,self.ids[field],scratch)
            for i in range(6):
                mon=party+100*i
                self.call(emu,'CreateRandomMon',mon,150,100)
                for j in range(1,5):
                    setmon(mon,'MON_DATA_MOVE'+str(j),self.ids['MOVE_DAZZLING_GLEAM'])
                    setmon(mon,'MON_DATA_PP'+str(j),10)
                for field,value in [('MON_DATA_MAX_HP',999),('MON_DATA_HP',999),('MON_DATA_SPATK',999),('MON_DATA_SPEED',999)]:setmon(mon,field,value)
                setmon(mon,'MON_DATA_CHAMPION_RIBBON',0)
            emu.write(count,6,1)
            # The room's own entry script dispatches the first Champion and
            # carries the win into Oak's movement and the actual HOF warp.
            self.warp(emu,35,79,0)
            for i in range(6):
                for j in range(1,5):
                    setmon(party+100*i,'MON_DATA_MOVE'+str(j),self.ids['MOVE_DAZZLING_GLEAM'])
                    setmon(party+100*i,'MON_DATA_PP'+str(j),10)
                for field in ['MON_DATA_SPATK','MON_DATA_SPEED','MON_DATA_DEF','MON_DATA_SPDEF']:
                    setmon(party+100*i,field,9999)
            self.call(emu,'VarSet',self.ids['VAR_TEMP_1'],1)
            emu.write(self.sym['gSelectedObjectEvent'],1,1)
            emu.write(self.sym['gSpecialVar_LastTalked'],1,2)
            self.call(emu,'ScriptContext_SetupScript',self.sym['PokemonLeague_ChampionsRoom_EventScript_Battle'])
            support={self.ids['MOVE_'+name]:name for name in ['TAILWIND','REFLECT','SLEEP_POWDER','PROTECT','FAKE_OUT','NUZZLE','DRAGON_DANCE']}
            enemy_seen=False
            postwin_started=False
            for step in range(1600):
                emu.frame(24)
                flags=emu.read(self.sym['gBattleTypeFlags'],4)
                opponent=emu.read(self.sym['gTrainerBattleParameter']+2,2)
                if opponent==338+starter and flags & self.ids['BATTLE_TYPE_TRAINER']:
                    enemy_seen=True
                    for battler in (1,3):
                        move=emu.read(self.sym['gLastMoves']+2*battler,2)
                        if move in support and support[move] not in evidence['support_moves_observed']:
                            evidence['support_moves_observed'].append(support[move])
                    if emu.read(self.sym['gBattleOutcome'],1)==1:evidence['won']=True
                save=emu.read(self.sym['gSaveBlock1Ptr'],4)
                if emu.read(save+4,1)==35 and emu.read(save+5,1)==80:evidence['hall_of_fame_map']=True
                callback=emu.read(self.sym['gMain']+4,4)&~1
                if callback in [self.sym.get(name,0)&~1 for name in ['CB2_DoHallOfFameScreenFrlg','CB2_HofIdle']]:
                    evidence['hall_of_fame_callback']=True
                    emu.shot(self.out/f'champion-{starter}-hof-callback.png')
                if evidence['won'] and not postwin_started and callback==self.sym['CB2_Overworld']&~1:
                    postwin_started=True
                    self.call(emu,'ScriptContext_SetupScript',self.sym['PokemonLeague_ChampionsRoom_EventScript_EnterRoom'])
                if step%100==0:
                    emu.shot(self.out/f'champion-{starter}-win-{step:04}.png')
                    print(json.dumps({'step':step, 'opponent':opponent,'outcome':emu.read(self.sym['gBattleOutcome'],1),'callback':hex(callback),'flags':flags,'count':emu.read(count,1),'hp':[self.get(emu,party+100*i,'MON_DATA_HP') for i in range(2)],'species':[self.get(emu,party+100*i,'MON_DATA_SPECIES') for i in range(2)],'pp':[self.get(emu,party+100*i,'MON_DATA_PP1') for i in range(2)],'evidence':evidence}),flush=True)
                emu.press('A',held=8,after=30)
                if evidence['hall_of_fame_map'] and evidence['hall_of_fame_callback'] and self.flag(emu,0x82C):
                    evidence['game_clear_flag']=True
                    evidence['champion_flag']=self.flag(emu,0x844)
                    evidence['champion_ribbons']=[self.get(emu,party+100*i,'MON_DATA_CHAMPION_RIBBON') for i in range(6)]
                    emu.shot(self.out/f'champion-{starter}-real-win-hof.png')
                    break
            evidence['starter_index']=starter
            evidence['trainer_id']=338+starter
            evidence['postwin_script_resumed']=postwin_started
            evidence['real_battle_started']=enemy_seen
            evidence['final_outcome']=emu.read(self.sym['gBattleOutcome'],1)
            (self.out/f'champion-{starter}-win-results.json').write_text(json.dumps(evidence,indent=2)+'\n')
            assert evidence['won'] and evidence['hall_of_fame_map'] and evidence.get('game_clear_flag'),evidence
            print(json.dumps(evidence),flush=True)
        finally:emu.close()

    def run(self):
        trainers = self.manifest['trainers']
        for i, suffix in enumerate(['BROCK','MISTY','LT_SURGE','ERIKA','KOGA','SABRINA','BLAINE','GIOVANNI']):
            key='TRAINER_PRL_BADGE_RECOVERY_'+suffix
            self.check(key, trainers[key], GYMS[i]+'_Gym_Frlg_EventScript_PRLBadgeRecovery')
        for i, suffix in enumerate(['LORELEI','BRUNO','AGATHA','LANCE']):
            key='TRAINER_ELITE_FOUR_'+suffix
            self.check(key, trainers[key], 'PokemonLeague_'+ROOMS[i]+'_EventScript_Battle')
        for i, suffix in enumerate(['SQUIRTLE','BULBASAUR','CHARMANDER']):
            key='TRAINER_CHAMPION_FIRST_'+suffix
            # Actual starter branch dispatch, rather than direct branch labels.
            self.check(key, trainers[key], 'PokemonLeague_ChampionsRoom_EventScript_Battle', i)
        print(json.dumps({'actual_battle_parties_checked':len(self.results), 'results':str(self.out/'results.json')}))

if __name__ == '__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('rom'); parser.add_argument('--elf',required=True)
    parser.add_argument('--save',required=True); parser.add_argument('--probe',required=True)
    parser.add_argument('--nm',default='arm-none-eabi-nm'); parser.add_argument('--out',required=True)
    parser.add_argument('--champion-win', action='store_true')
    parser.add_argument('--starter',type=int,choices=[0,1,2])
    args=parser.parse_args()
    qa=PartyQA(args)
    if args.champion_win:
        for starter in ([args.starter] if args.starter is not None else range(3)):qa.champion_win(starter)
    else:qa.run()
