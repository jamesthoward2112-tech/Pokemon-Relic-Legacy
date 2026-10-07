from pathlib import Path
import json, re

ROOT = Path(__file__).resolve().parents[2]
def read(rel): return (ROOT/rel).read_text(encoding="utf-8")
def write(rel, s):
    p=ROOT/rel; p.parent.mkdir(parents=True,exist_ok=True)
    p.write_text(s,encoding="utf-8",newline="\n")
def replace_once(rel, old, new):
    s=read(rel); n=s.count(old)
    if n != 1: raise SystemExit(f"{rel}: expected one match, got {n}: {old[:80]!r}")
    write(rel,s.replace(old,new,1))

# Resume exactly where the previous patch stopped.
battle_rel="src/battle_controller_player.c"
bc=read(battle_rel)
if "PRL: R-to-run" not in bc:
    marker="static void HandleInputChooseAction(enum BattlerId battler)\n{\n"
    if marker not in bc: raise SystemExit("HandleInputChooseAction signature not found")
    hook=marker+"""    // PRL: R-to-run. One press immediately chooses RUN in ordinary wild battles.
    // Trainer battles deliberately ignore this shortcut.
    if (JOY_NEW(R_BUTTON) && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
    {
        PlaySE(SE_SELECT);
        TryHideLastUsedBall();
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_RUN, 0);
        BtlController_Complete(battler);
        return;
    }

"""
    write(battle_rel,bc.replace(marker,hook,1))

# RELIC REPORTS presentation; internal Parcel ID/flags stay untouched.
items_rel="src/data/items.h"
items=read(items_rel)
if 'ITEM_NAME("Relic Reports")' not in items:
    pat=r"(\[ITEM_PARCEL\]\s*=\s*\{\s*\.name = ITEM_NAME\(\")Parcel(\"\),\s*\.price = 0,\s*\.description = COMPOUND_STRING\(\s*)\"A parcel for Prof\.\\n\"\s*\"Oak from a PokÃ©mon\\n\"\s*\"Mart's clerk\.\"\),"
    repl=r'\1Relic Reports\2\n        .price = 0,\n        .description = COMPOUND_STRING(\n            "Archaeology reports\\n"\n            "requested by Prof.\\n"\n            "Oak."),'
    items,n=re.subn(pat,repl,items,count=1,flags=re.S)
    if n!=1: raise SystemExit(f"Parcel item data patch count {n}")
    write(items_rel,items)

mart_rel="data/maps/ViridianCity_Mart_Frlg/scripts.inc"
mart=read(mart_rel)
mart=mart.replace(
    '\t.string "His order came in.\\n"\n\t.string "Can I get you to take it to him?$"',
    '\t.string "His RELIC REPORTS came in.\\n"\n\t.string "Can I get you to take them to him?$"')
mart=mart.replace(
    '\t.string "{PLAYER} received OAK\'S PARCEL\\n"\n\t.string "from the POKÃ©MON MART clerk.$"',
    '\t.string "{PLAYER} received RELIC REPORTS\\n"\n\t.string "from the POKÃ©MON MART clerk.$"')
write(mart_rel,mart)

lab_rel="data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc"
lab=read(lab_rel)
lab=lab.replace(
    '\t.string "What\'s that?\\n"\n\t.string "You have something for me?$"',
    '\t.string "What\'s that?\\n"\n\t.string "You have the RELIC REPORTS?$"')
lab=lab.replace(
    '\t.string "Ah! \\n"\n\t.string "It\'s the custom POKÃ© BALL!\\p"\n\t.string "I had it on order.\\n"\n\t.string "Thank you!$"',
    '\t.string "Ah! The archaeology reports!\\p"\n\t.string "Perfect timing. These findings may\\n"\n\t.string "explain KANTO\'s ancient RELICS.\\p"\n\t.string "Thank you, {PLAYER}!$"')
lab=lab.replace(
    '\t.string "{PLAYER} delivered OAK\'S PARCEL.$"',
    '\t.string "{PLAYER} delivered the RELIC REPORTS.$"')
lab=lab.replace(
    '\tgiveitem_msg PalletTown_ProfessorOaksLab_Text_ReceivedFivePokeBalls, ITEM_POKE_BALL, 5',
    '\tgiveitem_msg PalletTown_ProfessorOaksLab_Text_ReceivedFivePokeBalls, ITEM_POKE_BALL, 20')
lab=lab.replace('\tsetvar VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 1',
                '\tsetvar VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 2')
lab=lab.replace('\t.string "{PLAYER} received five POKÃ© BALLS\\n"',
                '\t.string "{PLAYER} received twenty POKÃ© BALLS\\n"')
lab=lab.replace(", RIVAL_BATTLE_TUTORIAL, PalletTown_ProfessorOaksLab_Text_RivalDefeat",
                ", 0, PalletTown_ProfessorOaksLab_Text_RivalDefeat")

# PRL starter trio: physical left=NidoranM, middle=Growlithe, right=Pikachu.
lab=lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_BULBASAUR\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_CHARMANDER\n\tsetvar RIVAL_STARTER_ID, LOCALID_CHARMANDER_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_GROWLITHE\n\tsetvar RIVAL_STARTER_ID, LOCALID_SQUIRTLE_BALL")
lab=lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_SQUIRTLE\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_BULBASAUR\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_GROWLITHE\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL")
lab=lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_CHARMANDER\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_SQUIRTLE\n\tsetvar RIVAL_STARTER_ID, LOCALID_SQUIRTLE_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_PIKACHU\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL")

if "PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter" not in lab:
    pat=r"PalletTown_ProfessorOaksLab_EventScript_ConfirmStarterChoice::.*?\n\tend\n\nPalletTown_ProfessorOaksLab_EventScript_ConfirmBulbasaur::"
    repl="""PalletTown_ProfessorOaksLab_EventScript_ConfirmStarterChoice::
\tapplymovement LOCALID_OAKS_LAB_PROF_OAK, Common_Movement_FaceRight
\twaitmovement 0
\tshowmonpic PLAYER_STARTER_SPECIES, 10, 3
\tbufferspeciesname STR_VAR_1, PLAYER_STARTER_SPECIES
\ttextcolor NPC_TEXT_COLOR_MALE
\tmsgbox PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter, MSGBOX_YESNO
\tgoto_if_eq VAR_RESULT, YES, PalletTown_ProfessorOaksLab_EventScript_ChoseStarter
\tgoto_if_eq VAR_RESULT, NO, PalletTown_ProfessorOaksLab_EventScript_DeclinedStarter
\tend

PalletTown_ProfessorOaksLab_EventScript_ConfirmBulbasaur::"""
    lab,n=re.subn(pat,repl,lab,count=1,flags=re.S)
    if n!=1: raise SystemExit(f"starter confirmation patch count {n}")

old="""\tgoto_if_eq PLAYER_STARTER_NUM, 0, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToCharmander
\tgoto_if_eq PLAYER_STARTER_NUM, 1, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur
\tgoto_if_eq PLAYER_STARTER_NUM, 2, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToSquirtle"""
new="""\tgoto_if_eq PLAYER_STARTER_NUM, 0, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToSquirtle
\tgoto_if_eq PLAYER_STARTER_NUM, 1, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur
\tgoto_if_eq PLAYER_STARTER_NUM, 2, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur"""
if old in lab: lab=lab.replace(old,new,1)
elif new not in lab: raise SystemExit("rival initial-ball dispatch missing")

parcel_marker="\tmsgbox PalletTown_ProfessorOaksLab_Text_RivalWhatDidYouCallMeFor\n\tclosemessage\n"
if "\tmsgbox PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter\n" not in lab:
    ins=parcel_marker+"""\tmsgbox PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter
\tremoveobject LOCALID_BULBASAUR_BALL
\tremoveobject LOCALID_SQUIRTLE_BALL
\tremoveobject LOCALID_CHARMANDER_BALL
"""
    if parcel_marker not in lab: raise SystemExit("parcel rival marker missing")
    lab=lab.replace(parcel_marker,ins,1)

# Useful Oak lab aides.
aides={
"PalletTown_ProfessorOaksLab_EventScript_Aide1":"""\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_EXP_SHARE, PalletTown_ProfessorOaksLab_EventScript_Aide1Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideExpShare
\tgiveitem ITEM_EXP_SHARE
\tsetflag FLAG_PRL_GOT_LAB_EXP_SHARE
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide1Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend""",
"PalletTown_ProfessorOaksLab_EventScript_Aide2":"""\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_QUICK_CLAW, PalletTown_ProfessorOaksLab_EventScript_Aide2Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideQuickClaw
\tgiveitem ITEM_QUICK_CLAW
\tsetflag FLAG_PRL_GOT_LAB_QUICK_CLAW
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide2Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend""",
"PalletTown_ProfessorOaksLab_EventScript_Aide3":"""\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_SOOTHE_BELL, PalletTown_ProfessorOaksLab_EventScript_Aide3Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideSootheBell
\tgiveitem ITEM_SOOTHE_BELL
\tsetflag FLAG_PRL_GOT_LAB_SOOTHE_BELL
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide3Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend"""
}
for label,body in aides.items():
    if label+"Repeat::" in lab: continue
    pat=rf"{label}::.*?(?=\nPalletTown_ProfessorOaksLab_EventScript_[A-Za-z0-9_]+::)"
    lab,n=re.subn(pat,label+"::\n"+body,lab,count=1,flags=re.S)
    if n!=1: raise SystemExit(f"aide patch {label} count {n}")

if "PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter::" not in lab:
    lab += r'''
PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter::
	.string "So, {PLAYER}, you want {STR_VAR_1}?$"

PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter::
	.string "{RIVAL}: Gramps, I'm taking the last\n"
	.string "one too.\p"
	.string "I won't leave a good POKÃ©MON sitting\n"
	.string "around while {PLAYER} gets ahead!$"

PalletTown_ProfessorOaksLab_Text_PRLAideExpShare::
	.string "OAK wants every field researcher\n"
	.string "properly equipped.\p"
	.string "Take this EXP. SHARE. It'll keep your\n"
	.string "whole team moving forward.$"

PalletTown_ProfessorOaksLab_Text_PRLAideQuickClaw::
	.string "Fieldwork rewards preparation.\p"
	.string "Take this QUICK CLAW. A slower\n"
	.string "POKÃ©MON can still surprise you.$"

PalletTown_ProfessorOaksLab_Text_PRLAideSootheBell::
	.string "Some ancient traits only surface\n"
	.string "when a POKÃ©MON truly trusts you.\p"
	.string "This SOOTHE BELL should help.$"

PalletTown_ProfessorOaksLab_Text_PRLAideResearch::
	.string "Keep an eye out for unusual changes.\n"
	.string "OAK's relic research is just starting.$"
'''
write(lab_rel,lab)

flags_rel="include/constants/flags.h"
flags=read(flags_rel)
if "FLAG_PRL_GOT_LAB_EXP_SHARE" not in flags:
    marker="#define FLAG_UNUSED_0x02B    0x2B // Unused Flag\n"
    aliases="""
// PokÃ©mon Relic Legacy early-Kanto flags.
#define FLAG_PRL_GOT_LAB_EXP_SHARE FLAG_UNUSED_0x020
#define FLAG_PRL_GOT_LAB_QUICK_CLAW FLAG_UNUSED_0x021
#define FLAG_PRL_GOT_LAB_SOOTHE_BELL FLAG_UNUSED_0x022
#define FLAG_PRL_GOT_BULBASAUR_GIFT FLAG_UNUSED_0x023
#define FLAG_PRL_GOT_CHARMANDER_GIFT FLAG_UNUSED_0x024
#define FLAG_PRL_GOT_SQUIRTLE_GIFT FLAG_UNUSED_0x025
"""
    if marker not in flags: raise SystemExit("unused flag marker missing")
    write(flags_rel,flags.replace(marker,marker+aliases,1))

# Remove low-value opening clutter and turn remaining Viridian boy into Noxichu clue.
vir_map_rel="data/maps/ViridianCity_Frlg/map.json"
vm=json.loads(read(vir_map_rel))
vm["object_events"]=[o for o in vm["object_events"]
                     if o.get("script") not in ("ViridianCity_EventScript_Woman","ViridianCity_EventScript_Youngster")]
write(vir_map_rel,json.dumps(vm,indent=2)+"\n")

gate_map_rel="data/maps/Route2_ViridianForest_SouthEntrance_Frlg/map.json"
gm=json.loads(read(gate_map_rel))
gm["object_events"]=[o for o in gm["object_events"]
                     if o.get("script")!="Route2_ViridianForest_SouthEntrance_EventScript_Woman2"]
write(gate_map_rel,json.dumps(gm,indent=2)+"\n")

vir_script_rel="data/maps/ViridianCity_Frlg/scripts.inc"
vs=read(vir_script_rel)
old='''ViridianCity_Text_CanCarryMonsAnywhere::
\t.string "Those POKÃ© BALLS at your waist!\\n"
\t.string "You have POKÃ©MON, don't you?\\p"
\t.string "It's great that you can carry and\\n"
\t.string "use POKÃ©MON anytime, anywhere.$"'''
newtxt='''ViridianCity_Text_CanCarryMonsAnywhere::
\t.string "VIRIDIAN FOREST is just ahead.\\p"
\t.string "OAK says PIKACHU may hide an\\n"
\t.string "ancient trait of its own.\\p"
\t.string "He mentioned a MOON STONEâ€¦$"'''
if old in vs: vs=vs.replace(old,newtxt,1)
elif newtxt not in vs: raise SystemExit("Viridian hint text target missing")
write(vir_script_rel,vs)

# Visible Level-10 Kanto classic-starter gifts beside aides.
gift_specs=[
("data/maps/ViridianForest_Frlg/map.json","data/maps/ViridianForest_Frlg/scripts.inc",
 "ViridianForest_EventScript_Youngster","ViridianForest_EventScript_PRLBulbasaurAide",
 "LOCALID_PRL_BULBASAUR","BULBASAUR","FLAG_PRL_GOT_BULBASAUR_GIFT","ITEM_MIRACLE_SEED",30,58,"BULBASAUR","MIRACLE SEED","GRASS-type"),
("data/maps/Route3_Frlg/map.json","data/maps/Route3_Frlg/scripts.inc",
 "Route3_EventScript_Youngster","Route3_EventScript_PRLCharmanderAide",
 "LOCALID_PRL_CHARMANDER","CHARMANDER","FLAG_PRL_GOT_CHARMANDER_GIFT","ITEM_SHARP_BEAK",71,13,"CHARMANDER","SHARP BEAK","FIRE-type"),
("data/maps/Route4_Frlg/map.json","data/maps/Route4_Frlg/scripts.inc",
 "Route4_EventScript_Woman","Route4_EventScript_PRLSquirtleAide",
 "LOCALID_PRL_SQUIRTLE","SQUIRTLE","FLAG_PRL_GOT_SQUIRTLE_GIFT","ITEM_METAL_COAT",10,8,"SQUIRTLE","METAL COAT","WATER-type"),
]
for map_rel,script_rel,old_script,aide_script,localid,species,flag,item,px,py,disp,item_disp,type_disp in gift_specs:
    m=json.loads(read(map_rel))
    target=next((o for o in m["object_events"] if o.get("script")==old_script or o.get("script")==aide_script),None)
    if target is None: raise SystemExit(f"{map_rel}: aide donor object missing")
    target["graphics_id"]="OBJ_EVENT_GFX_SCIENTIST"; target["script"]=aide_script
    if not any(o.get("local_id")==localid for o in m["object_events"]):
        m["object_events"].append({
            "local_id":localid,"type":"object","graphics_id":f"OBJ_EVENT_GFX_SPECIES({species})",
            "x":px,"y":py,"elevation":target.get("elevation",3),
            "movement_type":"MOVEMENT_TYPE_FACE_DOWN","movement_range_x":0,"movement_range_y":0,
            "trainer_type":"TRAINER_TYPE_NONE","trainer_sight_or_berry_tree_id":"0",
            "script":aide_script+"_Pokemon","flag":flag})
    write(map_rel,json.dumps(m,indent=2)+"\n")
    sc=read(script_rel)
    if aide_script+"::" not in sc:
        add=f'''
{aide_script}::
\tlock
\tfaceplayer
\tgoto_if_set {flag}, {aide_script}_Repeat
\tmsgbox {aide_script}_Text_Intro
\tsetvar VAR_TEMP_1, SPECIES_{species}
\tgivemon SPECIES_{species}, 10
\tgoto_if_eq VAR_RESULT, 2, Common_EventScript_NoMoreRoomForPokemon
\tsetflag {flag}
\tremoveobject {localid}
\tgiveitem {item}
\tmsgbox {aide_script}_Text_After
\trelease
\tend

{aide_script}_Repeat::
\tmsgbox {aide_script}_Text_Repeat
\trelease
\tend

{aide_script}_Pokemon::
\tmsgbox {aide_script}_Text_Pokemon, MSGBOX_NPC
\tend

{aide_script}_Text_Intro::
\t.string "PROF. OAK asked me to entrust this\\n"
\t.string "POKÃ©MON to a promising TRAINER.\\p"
\t.string "It may look ordinary nowâ€¦ but OAK\\n"
\t.string "believes something ancient sleeps\\l"
\t.string "within it.$"

{aide_script}_Text_After::
\t.string "Take this {item_disp}, too.\\p"
\t.string "OAK thinks certain {type_disp}\\n"
\t.string "POKÃ©MON may respond to it as they\\l"
\t.string "mature. Take good care of {disp}.$"

{aide_script}_Text_Repeat::
\t.string "Keep training {disp}. OAK's relic\\n"
\t.string "theory may take time to prove.$"

{aide_script}_Text_Pokemon::
\t.string "{disp} looks ready to travel.$"
'''
        write(script_rel,sc.rstrip()+"\n"+add)

# Rival teams: both unchosen PRL starters by Route 22/Cerulean.
party_rel="src/data/trainers_frlg.party"
party=read(party_rel)
def trainer_block(label,mons):
    lines=[f"=== {label} ===","Name: TERRY","Class: Rival Early Frlg","Pic: Rival Early Frlg",
           "Gender: Male","Music: Male","Double Battle: No",
           "AI: Check Bad Move / Try To Faint / Check Viability",""]
    for species,level,iv in mons:
        lines += [species,f"Level: {level}",f"IVs: {iv} HP / {iv} Atk / {iv} Def / {iv} SpA / {iv} SpD / {iv} Spe",""]
    return "\n".join(lines).rstrip()+"\n"
teams={
"TRAINER_RIVAL_OAKS_LAB_SQUIRTLE":[("NidoranM",5,0)],
"TRAINER_RIVAL_OAKS_LAB_BULBASAUR":[("NidoranM",5,0)],
"TRAINER_RIVAL_OAKS_LAB_CHARMANDER":[("Growlithe",5,0)],
"TRAINER_RIVAL_ROUTE22_EARLY_SQUIRTLE":[("Pidgey",9,6),("NidoranM",9,6),("Growlithe",9,6)],
"TRAINER_RIVAL_ROUTE22_EARLY_BULBASAUR":[("Pidgey",9,6),("NidoranM",9,6),("Pikachu",9,6)],
"TRAINER_RIVAL_ROUTE22_EARLY_CHARMANDER":[("Pidgey",9,6),("Growlithe",9,6),("Pikachu",9,6)],
"TRAINER_RIVAL_CERULEAN_SQUIRTLE":[("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Nidorino",18,12),("Growlithe",18,12)],
"TRAINER_RIVAL_CERULEAN_BULBASAUR":[("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Nidorino",18,12),("Pikachu",18,12)],
"TRAINER_RIVAL_CERULEAN_CHARMANDER":[("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Growlithe",18,12),("Pikachu",18,12)],
}
for label,mons in teams.items():
    pat=rf"=== {re.escape(label)} ===\n.*?(?=\n=== |\Z)"
    party,n=re.subn(pat,trainer_block(label,mons),party,count=1,flags=re.S)
    if n!=1: raise SystemExit(f"trainer block {label} count {n}")
write(party_rel,party)

# PKR Route1/2 encounter rates and Route2 equal quarter species weights.
wild_rel="src/data/wild_encounters.json"
wild=json.loads(read(wild_rel))
def walk(x):
    if isinstance(x,dict):
        yield x
        for v in x.values(): yield from walk(v)
    elif isinstance(x,list):
        for v in x: yield from walk(v)
r1=r2=0
for d in walk(wild):
    if d.get("map")=="MAP_ROUTE1" and isinstance(d.get("land_mons"),dict):
        lm=d["land_mons"]; spp={x.get("species") for x in lm.get("mons",[])}
        if spp and spp <= {"SPECIES_PIDGEY","SPECIES_RATTATA"}:
            lm["encounter_rate"]=15; r1+=1
    if d.get("map")=="MAP_ROUTE2" and isinstance(d.get("land_mons"),dict):
        lm=d["land_mons"]; spp={x.get("species") for x in lm.get("mons",[])}
        target={"SPECIES_PIDGEY","SPECIES_RATTATA","SPECIES_CATERPIE","SPECIES_WEEDLE"}
        if spp and spp <= target and len(lm.get("mons",[]))==12:
            lm["encounter_rate"]=15
            rows=[(3,3,"SPECIES_PIDGEY"),(3,3,"SPECIES_RATTATA"),
                  (4,4,"SPECIES_CATERPIE"),(4,4,"SPECIES_WEEDLE"),
                  (2,2,"SPECIES_CATERPIE"),(2,2,"SPECIES_WEEDLE"),
                  (5,5,"SPECIES_PIDGEY"),(5,5,"SPECIES_RATTATA"),
                  (4,4,"SPECIES_CATERPIE"),(4,4,"SPECIES_WEEDLE"),
                  (5,5,"SPECIES_CATERPIE"),(5,5,"SPECIES_WEEDLE")]
            lm["mons"]=[{"min_level":a,"max_level":b,"species":s} for a,b,s in rows]
            r2+=1
if r1<1 or r2<1: raise SystemExit(f"safe FRLG encounter match failed route1={r1} route2={r2}")
write(wild_rel,json.dumps(wild,indent=2)+"\n")

manifest_rel="tools/prl/pkr_port_manifest.json"
manifest=json.loads(read(manifest_rel))
layer=next(x for x in manifest["layers"] if x["id"]=="pkr_story_kanto")
ported=layer.setdefault("ported",[])
entries=[
"Cerulean milestone: Relic Reports replaces visible Oak Parcel presentation while retaining the safe internal story item.",
"Cerulean milestone: Viridian forced Weedle tutorial skipped after Relic Reports; low-value early NPC clutter removed/replaced with a Noxichu Moon Stone hint.",
"Cerulean milestone: Kanto starters changed to NidoranM / Growlithe / Pikachu with rival owning both unchosen lines by Route 22 and Cerulean.",
"Cerulean milestone: visible Lv10 Bulbasaur/Charmander/Squirtle gifts beside Oak aides with Miracle Seed/Sharp Beak/Metal Coat.",
"Cerulean milestone: Noxichu added as fresh PRL custom species ID with locked Electric/Dark, Infiltrator, 70/110/70/110/80/125 and Moon Stone evolution.",
"Cerulean milestone QoL: direct R-to-run in wild battles, L last Ball, reusable TMs, Gen6-style Exp Share key item and useful Oak lab aides."
]
for e in entries:
    if e not in ported: ported.append(e)
layer["status"]="in_progress"
manifest.setdefault("milestones",{})["PRL_PKR_PORT_003_CERULEAN"]={
    "status":"implementation_complete_awaiting_build","boundary":"New game through Cerulean/Bill approach","requires_fresh_save":True}
write(manifest_rel,json.dumps(manifest,indent=2)+"\n")

print("Resume patch complete.")
print(f"Encounter matches: Route1={r1}, Route2={r2}")
