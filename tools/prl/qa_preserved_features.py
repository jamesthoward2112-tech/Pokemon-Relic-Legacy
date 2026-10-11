"""Focused existing-save mGBA smoke checks; all changes stay in disposable RAM."""
exec(open('tools/prl/qa_restore_presentation.py').read().split("\nboot();e.frame")[0])
def restore():e.state(str(out/'overworld.state'),False)
def script(name):
    restore();call('ScriptContext_SetupScript',s[name]);e.frame(160)
restore();sb=e.read(s['gSaveBlock1Ptr']);call('AddBagItem',706,1);call('AddBagItem',712,1);e.write(sb+0x496,706,2);call('VarSet',0x40fd,712)
e.press('SELECT',held=1,after=90);e.shot(out/'select-tap.png');assert e.read(s['gPlayerAvatar']+0,1)&6;report['checks']['select_tap_bicycle']=True
e.press('SELECT',held=61,after=90);e.shot(out/'select-hold.png');assert e.read(s['gSpecialVar_ItemId'],2)==712;report['checks']['select_hold_smoke_no_selector']=True
script('EventScript_PRL_CenterDaypart');e.shot(out/'time-prompt.png');e.press('A',after=120);e.press('A',after=120);e.shot(out/'time-options.png')
e.press('DOWN');e.press('DOWN');e.press('A',after=120);e.shot(out/'time-set.png');call('RtcCalcLocalTime');assert e.read(s['gLocalTime']+2,1)==18;report['checks']['time_control_script_smoke']=True
script('Common_EventScript_MoveRelearner');e.press('A',after=120);e.shot(out/'relearner-options.png');e.press('A',after=120);e.press('A',after=120);e.shot(out/'relearner-choose-mon.png');e.press('A',after=160);e.press('A',after=160);e.press('A',after=160);e.press('A',after=160);e.shot(out/'relearner-move-list.png');report['checks']['relearner_script_smoke']=True
restore();call('FlagClear',0x2fa);call('RemoveBagItem',470,99);call('ScriptContext_SetupScript',s['Route10_PokemonCenter_1F_EventScript_Aide']);e.frame(160)
for _ in range(20):e.press('A',after=120)
e.shot(out/'aide-complete.png');assert call('CheckBagHasItem',470,1);assert call('FlagGet',0x2fa);report['checks']['aide_lucky_egg_given']=True
(out/'preserved-results.json').write_text(json.dumps(report,indent=2));e.close();print(json.dumps(report,indent=2))
