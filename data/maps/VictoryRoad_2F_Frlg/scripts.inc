VictoryRoad_2F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, VictoryRoad_2F_EventScript_PRL_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, VictoryRoad_2F_OnLoad
	.byte 0

@ The final Rocket confrontation belongs to the exit corridor, not Route 23.
@ Both old saves and new saves retain the existing trainer and badge-heist flags.
VictoryRoad_2F_EventScript_PRL_OnTransition::
	setflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	setflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	goto_if_unset FLAG_PRL_BADGE_HEIST_STARTED, VictoryRoad_2F_EventScript_PRL_TransitionDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_ALL_RECOVERED, VictoryRoad_2F_EventScript_PRL_TransitionDone
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, VictoryRoad_2F_EventScript_PRL_TransitionDone
	goto_if_defeated TRAINER_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_2F_EventScript_PRL_TransitionSyncWon
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
VictoryRoad_2F_EventScript_PRL_TransitionDone::
	end

VictoryRoad_2F_EventScript_PRL_TransitionSyncWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED
	end

VictoryRoad_2F_OnLoad::
	call_if_ne VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER1, 100, VictoryRoad_2F_EventScript_SetRockBarrier1
	call_if_ne VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER2, 100, VictoryRoad_2F_EventScript_SetRockBarrier2
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 0
	call_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_2F_EventScript_PRL_RocketGateOpen
	end

@ Three approach lanes and the actual outgoing warp footprint are intercepted.
@ Unlike the old map-script gate, this launches the trainer battle automatically.
VictoryRoad_2F_EventScript_PRL_RocketGate::
	lockall
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, VictoryRoad_2F_EventScript_PRL_RocketAlreadyWon
	goto_if_defeated TRAINER_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_2F_EventScript_PRL_RocketAlreadyWon
	goto_if_unset FLAG_PRL_BADGE_HEIST_ALL_RECOVERED, VictoryRoad_2F_EventScript_PRL_RocketNotReady
	setvar VAR_TEMP_3, 1
	playse SE_PIN
	applymovement LOCALID_PRL_VR_ROCKET_JESSIE, Route23_Movement_PRLHeistExclamation
	applymovement LOCALID_PRL_VR_ROCKET_JAMES, Route23_Movement_PRLHeistExclamation
	waitmovement LOCALID_PRL_VR_ROCKET_JESSIE
	waitmovement LOCALID_PRL_VR_ROCKET_JAMES
	applymovement LOCALID_PRL_VR_ROCKET_JESSIE, VictoryRoad_2F_Movement_PRL_RocketStepIn
	applymovement LOCALID_PRL_VR_ROCKET_JAMES, VictoryRoad_2F_Movement_PRL_RocketStepIn
	waitmovement LOCALID_PRL_VR_ROCKET_JESSIE
	waitmovement LOCALID_PRL_VR_ROCKET_JAMES
	trainerbattle_double TRAINER_JESSIE_JAMES_VICTORY_ROAD, Route23_Text_PRLFinalRocketIntro, Route23_Text_PRLFinalRocketDefeat, Route23_Text_PRLFinalRocketNotEnoughMons, VictoryRoad_2F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_VR_ROCKET_JESSIE
	goto VictoryRoad_2F_EventScript_PRL_RocketAlreadyWon

VictoryRoad_2F_EventScript_PRL_Meowth::
	lock
	faceplayer
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, VictoryRoad_2F_EventScript_PRL_RocketMeowthGone
	msgbox Route23_Text_PRLFinalRocketMeowth
	release
	end

VictoryRoad_2F_EventScript_PRL_RocketMeowthGone::
	msgbox Route23_Text_PRLFinalRocketGone
	release
	end

VictoryRoad_2F_EventScript_PRL_RocketNotReady::
	msgbox VictoryRoad_2F_Text_PRL_RocketGateClosed
	releaseall
	end

VictoryRoad_2F_EventScript_PRL_RocketAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
	releaseall
	end

VictoryRoad_2F_EventScript_PRL_RocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
	msgbox Route23_Text_PRLFinalRocketAfterBattle
	msgbox Route23_Text_PRLFinalRocketBlastOff
	closemessage
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	addobject LOCALID_PRL_VR_ROCKET_BALLOON
	hideobjectat LOCALID_PRL_VR_ROCKET_BALLOON, MAP_VICTORY_ROAD_2F
	setobjectxy LOCALID_PRL_VR_ROCKET_BALLOON, 46, 8
	setobjectsubpriority LOCALID_PRL_VR_ROCKET_BALLOON, MAP_VICTORY_ROAD_2F, 1
	applymovement LOCALID_PRL_VR_ROCKET_BALLOON, Route23_Movement_PRLBalloonEnter
	waitmovement LOCALID_PRL_VR_ROCKET_BALLOON
	setobjectsubpriority LOCALID_PRL_VR_ROCKET_JESSIE, MAP_VICTORY_ROAD_2F, 0
	setobjectsubpriority LOCALID_PRL_VR_ROCKET_JAMES, MAP_VICTORY_ROAD_2F, 0
	setobjectsubpriority LOCALID_PRL_VR_ROCKET_MEOWTH, MAP_VICTORY_ROAD_2F, 0
	playse SE_LEDGE
	applymovement LOCALID_PRL_VR_ROCKET_JESSIE, Route23_Movement_PRLJessieFinalBoard
	applymovement LOCALID_PRL_VR_ROCKET_JAMES, Route23_Movement_PRLJamesFinalBoard
	applymovement LOCALID_PRL_VR_ROCKET_MEOWTH, Route23_Movement_PRLMeowthFinalBoard
	waitmovement LOCALID_PRL_VR_ROCKET_JESSIE
	waitmovement LOCALID_PRL_VR_ROCKET_JAMES
	waitmovement LOCALID_PRL_VR_ROCKET_MEOWTH
	removeobject LOCALID_PRL_VR_ROCKET_JESSIE
	removeobject LOCALID_PRL_VR_ROCKET_JAMES
	removeobject LOCALID_PRL_VR_ROCKET_MEOWTH
	applymovement LOCALID_PRL_VR_ROCKET_BALLOON, Route23_Movement_PRLBalloonTakeoff
	waitmovement LOCALID_PRL_VR_ROCKET_BALLOON
	setflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	setflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	removeobject LOCALID_PRL_VR_ROCKET_BALLOON
	releaseall
	end

VictoryRoad_2F_Movement_PRL_RocketStepIn:
	walk_down
	step_end

VictoryRoad_2F_EventScript_PRL_RocketGateOpen::
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
	return

VictoryRoad_2F_EventScript_SetRockBarrier1::
	setmetatile 13, 10, METATILE_Cave_RockBarrier_Top, 1
	setmetatile 13, 11, METATILE_Cave_RockBarrier_Bottom, 1
	return

VictoryRoad_2F_EventScript_SetRockBarrier2::
	setmetatile 33, 16, METATILE_Cave_RockBarrier_Top, 1
	setmetatile 33, 17, METATILE_Cave_RockBarrier_Bottom, 1
	return

VictoryRoad_2F_EventScript_FloorSwitch1::
	lockall
	goto_if_eq VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER1, 100, VictoryRoad_2F_EventScript_FloorSwitch1AlreadyPressed
	setmetatile 13, 10, METATILE_Cave_Floor_Ledge_Top, 0
	setmetatile 13, 11, METATILE_Cave_Floor_Ledge_Bottom, 0
	playse SE_ICE_BREAK
	special DrawWholeMapView
	waitse
	copyobjectxytoperm LOCALID_VICTORY_ROAD_2F_BOULDER1
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER1, 100
	releaseall
	end

VictoryRoad_2F_EventScript_FloorSwitch1AlreadyPressed::
	releaseall
	end

VictoryRoad_2F_EventScript_FloorSwitch2::
	lockall
	goto_if_eq VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER2, 100, VictoryRoad_2F_EventScript_FloorSwitch2AlreadyPressed
	setmetatile 33, 16, METATILE_Cave_Floor_Ledge_Top, 0
	setmetatile 33, 17, METATILE_Cave_Floor_Ledge_Bottom, 0
	playse SE_ICE_BREAK
	special DrawWholeMapView
	waitse
	copyobjectxytoperm LOCALID_VICTORY_ROAD_2F_BOULDER2
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER2, 100
	releaseall
	end

VictoryRoad_2F_EventScript_FloorSwitch2AlreadyPressed::
	releaseall
	end

VictoryRoad_2F_EventScript_Dawson::
	trainerbattle_single TRAINER_POKEMANIAC_DAWSON, VictoryRoad_2F_Text_DawsonIntro, VictoryRoad_2F_Text_DawsonDefeat
	msgbox VictoryRoad_2F_Text_DawsonPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_2F_EventScript_Daisuke::
	trainerbattle_single TRAINER_BLACK_BELT_DAISUKE, VictoryRoad_2F_Text_DaisukeIntro, VictoryRoad_2F_Text_DaisukeDefeat
	msgbox VictoryRoad_2F_Text_DaisukePostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_2F_EventScript_Nelson::
	trainerbattle_single TRAINER_JUGGLER_NELSON, VictoryRoad_2F_Text_NelsonIntro, VictoryRoad_2F_Text_NelsonDefeat
	msgbox VictoryRoad_2F_Text_NelsonPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_2F_EventScript_Gregory::
	trainerbattle_single TRAINER_JUGGLER_GREGORY, VictoryRoad_2F_Text_GregoryIntro, VictoryRoad_2F_Text_GregoryDefeat, VictoryRoad_2F_EventScript_GregoryHeal
	goto VictoryRoad_2F_EventScript_GregoryHeal

VictoryRoad_2F_EventScript_GregoryHeal::
	lock
	faceplayer
	special HealPlayerParty
	msgbox VictoryRoad_2F_Text_GregoryHealing, MSGBOX_DEFAULT
	release
	end

VictoryRoad_2F_EventScript_Vincent::
	trainerbattle_single TRAINER_TAMER_VINCENT, VictoryRoad_2F_Text_VincentIntro, VictoryRoad_2F_Text_VincentDefeat
	msgbox VictoryRoad_2F_Text_VincentPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_2F_Text_DawsonIntro::
	.string "If you can get through here,\n"
	.string "you can go meet the ELITE FOUR.$"

VictoryRoad_2F_Text_DawsonDefeat::
	.string "No!\n"
	.string "Unbelievable!$"

VictoryRoad_2F_Text_DawsonPostBattle::
	.string "I study POKéMON history.\p"
	.string "MT. MOON, SILPH and\n"
	.string "CINNABAR shared records.\p"
	.string "That RELIC signal remains\n"
	.string "unexplained even now!$"

VictoryRoad_2F_Text_DaisukeIntro::
	.string "This here is VICTORY ROAD.\n"
	.string "It's the final test for TRAINERS!$"

VictoryRoad_2F_Text_DaisukeDefeat::
	.string "Atcho!$"

VictoryRoad_2F_Text_DaisukePostBattle::
	.string "If you get stuck, try moving some\n"
	.string "boulders around.$"

VictoryRoad_2F_Text_NelsonIntro::
	.string "Ah, so you wish to challenge the\n"
	.string "ELITE FOUR?$"

VictoryRoad_2F_Text_NelsonDefeat::
	.string "You got me!$"

VictoryRoad_2F_Text_NelsonPostBattle::
	.string "Your RIVAL came through.\p"
	.string "And I heard you defeated\n"
	.string "GIOVANNI's fossil army!\p"
	.string "The LEAGUE is waiting!$"

VictoryRoad_2F_Text_VincentIntro::
	.string "Come on!\n"
	.string "I'll whip you!$"

VictoryRoad_2F_Text_VincentDefeat::
	.string "I got whipped!$"

VictoryRoad_2F_Text_VincentPostBattle::
	.string "You earned the right to be on\n"
	.string "VICTORY ROAD…$"

VictoryRoad_2F_Text_GregoryIntro::
	.string "Is VICTORY ROAD too tough?$"

VictoryRoad_2F_Text_GregoryDefeat::
	.string "Well done!$"

VictoryRoad_2F_Text_GregoryPostBattle::
	.string "These old rocks predate\n"
	.string "KANTO's earliest fossils.\p"
	.string "The same pattern was found\n"
	.string "in SILPH's RELIC files!$"

VictoryRoad_2F_Text_PRL_RocketGateClosed::
	.string "A voice echoes from above…\p"
	.string "TEAM ROCKET is waiting on the\n"
	.string "upper floor. The path is blocked!$"

@ Unused, old text for Moltres
VictoryRoad_2F_Text_Gyaoo::
	.string "ギヤーオ！$"


VictoryRoad_2F_Text_GregoryHealing::
	.string "That was a terrific battle!\p"
	.string "Let me restore your POKéMON\n"
	.string "before you carry on.\p"
	.string "Come back any time for a rest!$"
