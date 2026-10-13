#!/usr/bin/env python3
"""Combined-ROM mGBA checks: real shop UI, bell evolution and Return learning.

Native calls prepare reproducible saved-game fixtures and invoke the game's
normal scripts; purchases and evolution run through frame-driven game UI.
"""
import argparse
import json
from pathlib import Path
from qa_great_badge_heist import QA, contact_sheet

KANGASKHAN, KHANG = 115, 1577
BELL, PP_MAX, MAX_REVIVE = 463, 72, 34
SPECIES, HELD, MOVE1, FRIENDSHIP = 18, 19, 20, 39
SCRATCH = 0x0203FFF0

class FinalQA(QA):
    def mon_set(self, emu, mon, field, value):
        emu.write(SCRATCH, value, 4)
        self.call(emu, 'SetMonData', mon, field, SCRATCH)

    def mon_get(self, emu, mon, field):
        return self.call(emu, 'GetMonData3', mon, field, 0)

    def quantity(self, emu, item):
        return self.call(emu, 'CountTotalItemQuantityInBag', item)

    def money(self, emu):
        save = emu.read(self.sym['gSaveBlock1Ptr'], 4)
        return self.call(emu, 'GetMoney', save + 0x490)

    def league_purchase(self, item, price, row):
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.warp(emu, 47, 0, 0)
            save = emu.read(self.sym['gSaveBlock1Ptr'], 4)
            self.call(emu, 'SetMoney', save + 0x490, 50000)
            before = self.quantity(emu, item)
            self.call(emu, 'ScriptContext_SetupScript', self.sym['IndigoPlateau_PokemonCenter_1F_EventScript_Clerk'])
            emu.frame(180)
            emu.press('A', after=120) # Finish the greeting and show Buy/Sell/Quit.
            emu.press('A', after=240) # Buy opens the actual buy list.
            for _ in range(row): emu.press('DOWN', after=12)
            emu.shot(self.out / f'league-{item}-listed.png')
            emu.press('A', after=180) # quantity prompt, default one
            emu.shot(self.out / f'league-{item}-quantity-price.png')
            emu.press('A', after=180) # purchase confirmation
            emu.shot(self.out / f'league-{item}-confirm.png')
            emu.press('A', after=180) # Advance confirmation.
            for _ in range(4):
                if self.money(emu) != 50000: break
                emu.press('A', after=180)
            after_money = self.money(emu)
            after_quantity = self.quantity(emu, item)
            emu.shot(self.out / f'league-{item}-purchased.png')
            assert after_money == 50000 - price, (item, after_money)
            assert after_quantity == before + 1, (item, before, after_quantity)
            self.results[f'purchase_{item}'] = {'money_before': 50000, 'money_after': after_money, 'quantity_before': before, 'quantity_after': after_quantity}
        finally: emu.close()

    def evolution(self):
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.setup_party(emu, (KANGASKHAN,))
            mon = emu.read(self.sym['gPlayerPartyPtr'], 4)
            # Blank moves allow the evolution move to be learned automatically.
            for field in range(MOVE1, MOVE1+4): self.mon_set(emu, mon, field, 0)
            self.mon_set(emu, mon, FRIENDSHIP, 0)
            self.mon_set(emu, mon, HELD, 0)
            assert self.call(emu, 'GetEvolutionTargetSpecies', mon, 0, 0, 0, SCRATCH, 0) == 0
            self.mon_set(emu, mon, FRIENDSHIP, 255)
            assert self.call(emu, 'GetEvolutionTargetSpecies', mon, 0, 0, 0, SCRATCH, 0) == 0
            self.mon_set(emu, mon, FRIENDSHIP, 0)
            self.mon_set(emu, mon, HELD, BELL)
            assert self.call(emu, 'GetEvolutionTargetSpecies', mon, 0, 0, 0, SCRATCH, 0) == KHANG
            self.results['bell_required_independent_of_friendship'] = True
            self.call(emu, 'BeginEvolutionScene', mon, KHANG, 1, 0)
            frames=[]
            for i in range(30):
                emu.frame(100)
                frames.append((f'evolution {i}', emu.image()))
                emu.press('A', after=1)
                if self.mon_get(emu, mon, SPECIES) == KHANG and self.mon_get(emu, mon, MOVE1) == 216: break
            contact_sheet(frames, self.out / 'khang-evolution-contact.png')
            assert self.mon_get(emu, mon, SPECIES) == KHANG
            moves = [self.mon_get(emu, mon, field) for field in range(MOVE1, MOVE1+4)]
            assert 216 in moves, moves
            assert self.mon_get(emu, mon, HELD) == BELL
            self.results['actual_evolution_scene_and_return_learned'] = {'species': KHANG, 'moves': moves, 'held_item': BELL}
            emu.frame(180)
            emu.shot(self.out / 'khang-evolved-return.png')
        finally: emu.close()

    def return_battle(self):
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.setup_party(emu, (KHANG,))
            mon = emu.read(self.sym['gPlayerPartyPtr'], 4)
            self.mon_set(emu, mon, FRIENDSHIP, 255)
            self.mon_set(emu, mon, MOVE1, 216)
            self.mon_set(emu, mon, 24, 20) # PP1
            self.mon_set(emu, mon, 68, 500) # Fixture speed: attack before random paralysis.
            self.set_flag(emu, 0x4D9)
            self.set_flag(emu, 0x4E1, False)
            self.set_flag(emu, 0x055, False)
            self.warp(emu, 39, 1, 0)
            emu.write(self.sym['gSelectedObjectEvent'], 1, 1)
            emu.write(self.sym['gSpecialVar_LastTalked'], 8, 2)
            self.call(emu, 'ScriptContext_SetupScript', self.sym['ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecovery'])
            # Advance trainer introduction and battle menus, then inspect every
            # frame for Return's actual damage-calculation power at max friendship.
            found = False
            observed = set()
            frames = []
            for i in range(100):
                emu.press('A', after=1)
                for j in range(39):
                    emu.frame()
                    observed.add((emu.read(self.sym['gCurrentMove'], 2), emu.read(self.sym['gBattleMovePower'], 2)))
                    if emu.read(self.sym['gCurrentMove'], 2) == 216 and emu.read(self.sym['gBattleMovePower'], 2) == 102:
                        found = True
                if i % 5 == 0: frames.append((f'Return battle {i}', emu.image()))
                if found:
                    emu.shot(self.out / 'khang-return-102-power.png')
                    break
            contact_sheet(frames, self.out / 'khang-return-battle-contact.png')
            (self.out / 'return-observed-powers.json').write_text(json.dumps(sorted(observed)))
            assert found, 'Return never reached its expected 102 base power in the live battle'
            self.results['actual_return_battle_max_friendship_power'] = 102
        finally: emu.close()

    def safari_existing_save(self):
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.set_flag(emu, 0x8FC, False)
            self.warp(emu, 35, 66, 0)
            emu.shot(self.out / 'safari-west-existing-save.png')
            # Test compiled event table as well as the old-save reserved flag.
            address = self.sym['SafariZone_West_Frlg_ObjectEvents']
            source = json.loads((Path(__file__).resolve().parents[2] / 'data/maps/SafariZone_West_Frlg/map.json').read_text())
            names = [o['script'] for o in source['object_events']]
            assert len(names) == 4, names
            for i, name in enumerate(names):
                # struct ObjectEventTemplate: script is a word at offset 16.
                assert emu.read(address + i * 24 + 16, 4) == self.sym[name]
            assert 'SafariZone_West_EventScript_ItemSilkScarf' not in self.sym
            assert not self.flag(emu, 0x8FC)
            self.results['safari_old_save_only_extra_object_removed'] = names
        finally: emu.close()

    def run(self):
        self.league_purchase(PP_MAX, 5000, 11)
        self.league_purchase(MAX_REVIVE, 3000, 7)
        self.evolution()
        self.return_battle()
        self.safari_existing_save()
        (self.out / 'results.json').write_text(json.dumps(self.results, indent=2) + '\n')
        print(json.dumps(self.results))

def main():
    p=argparse.ArgumentParser()
    p.add_argument('rom'); p.add_argument('--elf',required=True)
    p.add_argument('--save',required=True); p.add_argument('--probe',required=True)
    p.add_argument('--nm',default='arm-none-eabi-nm'); p.add_argument('--out',required=True)
    FinalQA(p.parse_args()).run()
if __name__ == '__main__': main()
