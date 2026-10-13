#!/usr/bin/env python3
"""Inspect actual battle-created teams in mGBA against the approved manifest.

Each disposable save starts a normal shipped trainerbattle script. No enemy
party creation is called manually, and battle outcome/Hall of Fame flags are
never injected. This tool verifies team construction and battle mode only.
"""
import argparse
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
        names = {'MON_DATA_SPECIES', 'MON_DATA_HELD_ITEM', 'MON_DATA_LEVEL', 'MON_DATA_IS_SHINY', 'BATTLE_TYPE_DOUBLE', 'BATTLE_TYPE_TRAINER', 'VAR_STARTER_MON'}
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
    PartyQA(parser.parse_args()).run()
