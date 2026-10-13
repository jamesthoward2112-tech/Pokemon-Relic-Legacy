#!/usr/bin/env python3
"""Real mGBA script QA on a disposable save; callbacks are invoked explicitly.
This verifies shipped script execution and persistence, not battle AI or route traversal.
"""
from __future__ import annotations
import argparse
import json
import shutil
from pathlib import Path
from qa_great_badge_heist import QA, contact_sheet

LEADERS = [('PewterCity','Brock','BROCK',496), ('CeruleanCity','Misty','MISTY',472), ('VermilionCity','LtSurge','SURGE',443), ('CeladonCity','Erika','ERIKA',474), ('FuchsiaCity','Koga','KOGA',487), ('SaffronCity','Sabrina','SABRINA',476), ('CinnabarIsland','Blaine','BLAINE',477), ('ViridianCity','Giovanni','GIOVANNI',442)]
HEALERS = [(1,'Rolando',296),(2,'Gregory',190),(3,'George',293)]

class FinalQA(QA):
    def money(self,e):
        return self.call(e,'GetMoney',e.read(self.sym['gSaveBlock1Ptr'],4)+0x490)

    def quantity(self,e,item):
        return self.call(e,'CountTotalItemQuantityInBag',item)

    def script(self,e,label,name,presses=14):
        frames=self.run_script_frames(e,label,presses,name)
        contact_sheet(frames,self.out/(name+'.png'))

    def pocket(self,e,item):
        index=self.call(e,'GetItemPocket',item)
        ptr=self.sym['gBagPockets']+8*index
        return ptr,e.read(ptr+4,2)&0x3ff

    def slot(self,e,pocket,index,item,quantity):
        self.call(e,'BagPocket_SetSlotData',pocket,index,(quantity<<16)|item)

    def persist(self,e,name):
        assert self.call(e,'TrySavingData',0)==1, 'GBA save failed'
        path=self.out/(name+'.sav')
        assert e.lib.prl_save(str(path).encode()), 'mGBA export failed'
        e.close()
        original=self.args.save
        self.args.save=str(path)
        try: e=self.open()
        finally: self.args.save=original
        self.load_existing_save(e)
        return e

    def rewards(self):
        e=self.open()
        try:
            self.load_existing_save(e)
            self.set_flag(e,0x4D9)
            # Sufficient headroom avoids the game's money cap obscuring bonuses.
            save=e.read(self.sym['gSaveBlock1Ptr'],4)
            self.call(e,'SetMoney',save+0x490,1000)
            for i,(city,leader,key,item) in enumerate(LEADERS):
                recovered,claimed=0x4DA+i,0x4E6+i
                self.set_flag(e,recovered,False); self.set_flag(e,claimed,False)
                pocket,capacity=self.pocket(e,item)
                # Every slot holds a distinct valid item; target item is absent.
                filler=[j for j in range(1,800) if j!=item and self.call(e,'GetItemPocket',j)==self.call(e,'GetItemPocket',item)]
                assert len(filler)>=capacity
                for pos in range(capacity):self.slot(e,pocket,pos,filler[pos],1)
                assert not self.call(e,'CheckBagHasSpace',item,1)
                before=self.money(e)
                self.script(e,city+'_Gym_Frlg_EventScript_PRLBadgeRecoveryWon',key+'-full')
                assert self.flag(e,recovered) and not self.flag(e,claimed)
                assert self.money(e)==before and self.quantity(e,item)==0
                # Pending badge/reward flags must survive an actual save and cold reload.
                if i==0:
                    e=self.persist(e,'pending-reward')
                    assert self.flag(e,recovered) and not self.flag(e,claimed)
                    assert self.money(e)==before
                    pocket,capacity=self.pocket(e,item)
                self.slot(e,pocket,0,0,0)
                self.script(e,city+'_Gym_EventScript_'+leader,key+'-pending')
                assert self.flag(e,claimed) and self.quantity(e,item)==1
                assert self.money(e)==before+10000
                # Invoke the guarded gift a second time via a real overworld script.
                # PendingDone returns, so use the victory callback to provide an end.
                self.script(e,city+'_Gym_Frlg_EventScript_PRLBadgeRecoveryWon',key+'-duplicate')
                assert self.quantity(e,item)==1 and self.money(e)==before+10000
                if i==0:
                    e=self.persist(e,'claimed-reward')
                    assert self.flag(e,claimed) and self.quantity(e,item)==1
                    self.script(e,city+'_Gym_Frlg_EventScript_PRLBadgeRecoveryWon',key+'-reloaded')
                    assert self.money(e)==before+10000 and self.quantity(e,item)==1
                self.results[key]={'full_bag_badge_preserved':True,'pending_claim':True,'no_duplicate':True}
            # Also test direct normal victory with room on a clean claimed flag.
            city,leader,key,item=LEADERS[0]
            pocket,capacity=self.pocket(e,item)
            for pos in range(capacity):self.slot(e,pocket,pos,0,0)
            self.set_flag(e,0x4E6,False)
            before=self.money(e)
            self.script(e,city+'_Gym_Frlg_EventScript_PRLBadgeRecoveryWon','normal-victory')
            assert self.quantity(e,item)==1 and self.money(e)==before+10000 and self.flag(e,0x4E6)
            self.results['normal_victory']=True
        finally:e.close()

    def mon(self,e,address,field):return self.call(e,'GetMonData3',address,field,0)

    def set_mon(self,e,address,field,value):
        scratch=self.sym['gSpecialVar_0x8000']
        old=e.read(scratch,4);e.write(scratch,value,4)
        self.call(e,'SetMonData',address,field,scratch)
        e.write(scratch,old,4)

    def healers(self):
        e=self.open()
        try:
            self.load_existing_save(e)
            self.setup_party(e,(3,6,9,25,130,143))
            party=e.read(self.sym['gPlayerPartyPtr'],4)
            invariant_fields=[18,19,20,21,22,23,28,32,33,34,35,36,37,38,64]
            baseline=[[self.mon(e,party+100*i,f) for f in invariant_fields] for i in range(6)]
            healthy_pp=[[self.mon(e,party+100*i,24+j) for j in range(4)] for i in range(6)]
            def damage():
                for i,status in enumerate([8,16,64,1,32,128]):
                    self.set_mon(e,party+100*i,10,1)
                    self.set_mon(e,party+100*i,1,status)
                    for j in range(4):self.set_mon(e,party+100*i,24+j,0)
            def healed():
                for i in range(6):
                    address=party+100*i
                    assert self.mon(e,address,10)==self.mon(e,address,65)
                    assert self.mon(e,address,1)==0
                    assert [self.mon(e,address,24+j) for j in range(4)]==healthy_pp[i]
                    assert [self.mon(e,address,f) for f in invariant_fields]==baseline[i]
            for floor,name,trainer in HEALERS:
                self.call(e,'SetTrainerFlag',trainer)
                damage()
                self.script(e,f'VictoryRoad_{floor}F_EventScript_{name}','old-save-'+name)
                healed();assert self.flag(e,0x500+trainer)
                damage()
                self.script(e,f'VictoryRoad_{floor}F_EventScript_{name}Heal','victory-callback-'+name)
                healed();assert self.flag(e,0x500+trainer)
                self.results[name]={'old_save_repeat_heal':True,'callback_heal':True,'hp_pp_status':True,'invariants':True}
            e=self.persist(e,'healed-party')
            party=e.read(self.sym['gPlayerPartyPtr'],4)
            healed()
            self.results['healed_save_reload']=True
        finally:e.close()

if __name__=='__main__':
    p=argparse.ArgumentParser()
    for name in ['rom','elf','save','probe','out']:p.add_argument('--'+name,required=True)
    p.add_argument('--nm',default='arm-none-eabi-nm')
    args=p.parse_args()
    out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    assert Path(args.save).stat().st_size == 128 * 1024, 'Use a verified 128 KiB base save'
    copied=out/'input-copy.sav';shutil.copyfile(args.save,copied);args.save=str(copied)
    qa=FinalQA(args)
    qa.rewards();qa.healers()
    (out/'results.json').write_text(json.dumps(qa.results,indent=2))
    print(json.dumps(qa.results,indent=2))
