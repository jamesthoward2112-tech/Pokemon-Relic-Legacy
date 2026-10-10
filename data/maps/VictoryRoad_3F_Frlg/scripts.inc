VictoryRoad_3F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, VictoryRoad_3F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, VictoryRoad_3F_OnTransition
	.byte 0

VictoryRoad_3F_OnTransition::
	call_if_set FLAG_BADGE08_GET, VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROAD_Trigger
	end

VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROAD_Trigger::
	lockall
	call VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROAD
	releaseall
	end

VictoryRoad_3F_OnLoad::
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	call_if_ne VAR_MAP_SCENE_VICTORY_ROAD_3F, 100, VictoryRoad_3F_EventScript_SetRockBarrier
	end

VictoryRoad_3F_EventScript_SetRockBarrier::
	setmetatile 12, 12, METATILE_Cave_RockBarrier_Top, 1
	setmetatile 12, 13, METATILE_Cave_RockBarrier_Bottom, 1
	return

VictoryRoad_3F_EventScript_FloorSwitch::
	lockall
	goto_if_eq VAR_MAP_SCENE_VICTORY_ROAD_3F, 100, VictoryRoad_3F_EventScript_FloorSwitchAlreadyPressed
	setmetatile 12, 12, METATILE_Cave_Floor_Ledge_Top, 0
	setmetatile 12, 13, METATILE_Cave_Floor_Ledge_Bottom, 0
	playse SE_ICE_BREAK
	special DrawWholeMapView
	waitse
	copyobjectxytoperm LOCALID_VICTORY_ROAD_3F_BOULDER1
	copyobjectxytoperm LOCALID_VICTORY_ROAD_3F_BOULDER2
	setvar VAR_MAP_SCENE_VICTORY_ROAD_3F, 100
	releaseall
	end

VictoryRoad_3F_EventScript_FloorSwitchAlreadyPressed::
	releaseall
	end

VictoryRoad_3F_EventScript_George::
	trainerbattle_single TRAINER_COOLTRAINER_GEORGE, VictoryRoad_3F_Text_GeorgeIntro, VictoryRoad_3F_Text_GeorgeDefeat
	msgbox VictoryRoad_3F_Text_GeorgePostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_3F_EventScript_Colby::
	trainerbattle_single TRAINER_COOLTRAINER_COLBY, VictoryRoad_3F_Text_ColbyIntro, VictoryRoad_3F_Text_ColbyDefeat
	msgbox VictoryRoad_3F_Text_ColbyPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_3F_EventScript_Caroline::
	trainerbattle_single TRAINER_COOLTRAINER_CAROLINE, VictoryRoad_3F_Text_CarolineIntro, VictoryRoad_3F_Text_CarolineDefeat
	msgbox VictoryRoad_3F_Text_CarolinePostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_3F_EventScript_Alexa::
	trainerbattle_single TRAINER_COOLTRAINER_ALEXA, VictoryRoad_3F_Text_AlexaIntro, VictoryRoad_3F_Text_AlexaDefeat
	msgbox VictoryRoad_3F_Text_AlexaPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_3F_EventScript_Ray::
	trainerbattle_double TRAINER_COOL_COUPLE_RAY_TYRA, VictoryRoad_3F_Text_RayIntro, VictoryRoad_3F_Text_RayDefeat, VictoryRoad_3F_Text_RayNotEnoughMons
	msgbox VictoryRoad_3F_Text_RayPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_3F_EventScript_Tyra::
	trainerbattle_double TRAINER_COOL_COUPLE_RAY_TYRA, VictoryRoad_3F_Text_TyraIntro, VictoryRoad_3F_Text_TyraDefeat, VictoryRoad_3F_Text_TyraNotEnoughMons
	msgbox VictoryRoad_3F_Text_TyraPostBattle, MSGBOX_AUTOCLOSE
	end

Text_DoubleEdgeTeach::
	.string "You should be proud of yourself,\n"
	.string "having battled your way through\l"
	.string "VICTORY ROAD so courageously.\p"
	.string "In recognition of your feat,\n"
	.string "I'll teach you DOUBLE-EDGE.\p"
	.string "Would you like me to teach that\n"
	.string "technique?$"

Text_DoubleEdgeDeclined::
	.string "I'll teach you the technique\n"
	.string "anytime.$"

Text_DoubleEdgeWhichMon::
	.string "Which POKéMON should I teach\n"
	.string "DOUBLE-EDGE?$"

Text_DoubleEdgeTaught::
	.string "Keep that drive going for the\n"
	.string "POKéMON LEAGUE!\p"
	.string "Take a run at them and knock 'em\n"
	.string "out!$"

VictoryRoad_3F_Text_GeorgeIntro::
	.string "I heard rumors of a child prodigy.$"

VictoryRoad_3F_Text_GeorgeDefeat::
	.string "The rumors were true!$"

VictoryRoad_3F_Text_GeorgePostBattle::
	.string "So, it was you who beat GIOVANNI\n"
	.string "of TEAM ROCKET?$"

VictoryRoad_3F_Text_AlexaIntro::
	.string "TRAINERS live to seek stronger\n"
	.string "opponents.$"

VictoryRoad_3F_Text_AlexaDefeat::
	.string "Oh!\n"
	.string "So strong!$"

VictoryRoad_3F_Text_AlexaPostBattle::
	.string "By fighting tough battles, you get\n"
	.string "stronger.$"

VictoryRoad_3F_Text_CarolineIntro::
	.string "I'll show you just how good you\n"
	.string "are. Not!$"

VictoryRoad_3F_Text_CarolineDefeat::
	.string "I'm furious!$"

VictoryRoad_3F_Text_CarolinePostBattle::
	.string "You showed me just how good\n"
	.string "I was…$"

VictoryRoad_3F_Text_ColbyIntro::
	.string "Only the chosen can pass here!$"

VictoryRoad_3F_Text_ColbyDefeat::
	.string "I don't believe it!$"

VictoryRoad_3F_Text_ColbyPostBattle::
	.string "All TRAINERS here are headed to\n"
	.string "the POKéMON LEAGUE.\p"
	.string "Don't let down your guard.$"

VictoryRoad_3F_Text_RayIntro::
	.string "RAY: Together, the two of us are\n"
	.string "destined for greatness!$"

VictoryRoad_3F_Text_RayDefeat::
	.string "RAY: Ludicrous!\n"
	.string "This can't be!$"

VictoryRoad_3F_Text_RayPostBattle::
	.string "RAY: You've beaten us.\n"
	.string "Greatness remains elusive…$"

VictoryRoad_3F_Text_RayNotEnoughMons::
	.string "RAY: Together, the two of us are\n"
	.string "striving for the pinnacle.\p"
	.string "We need you to bring two POKéMON\n"
	.string "into battle with us.$"

VictoryRoad_3F_Text_TyraIntro::
	.string "TYRA: We're trying to become\n"
	.string "champions together.$"

VictoryRoad_3F_Text_TyraDefeat::
	.string "TYRA: Oh, but…$"

VictoryRoad_3F_Text_TyraPostBattle::
	.string "TYRA: You've taught me that power\n"
	.string "can be infinite in shape and form.$"

VictoryRoad_3F_Text_TyraNotEnoughMons::
	.string "TYRA: You can't battle with us if\n"
	.string "you have only one POKéMON.$"


VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROAD::
	goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADEnd
	goto_if_unset FLAG_BADGE08_GET, VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADEnd
	goto_if_defeated TRAINER_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	clearflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	clearflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	addobject LOCALID_PRL_JESSIE_VICTORY_ROAD
	addobject LOCALID_PRL_JAMES_VICTORY_ROAD
	addobject LOCALID_PRL_MEOWTH_VICTORY_ROAD
	applymovement LOCALID_PRL_JESSIE_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_JAMES_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketApproach
	waitmovementall
	turnobject LOCALID_PRL_JESSIE_VICTORY_ROAD, DIR_SOUTH
	turnobject LOCALID_PRL_JAMES_VICTORY_ROAD, DIR_SOUTH
	turnobject LOCALID_PRL_MEOWTH_VICTORY_ROAD, DIR_SOUTH
	trainerbattle_double TRAINER_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_3F_Text_PRL_RocketIntro, VictoryRoad_3F_Text_PRL_RocketDefeat, VictoryRoad_3F_Text_PRL_RocketNotEnoughMons, localId=LOCALID_PRL_JESSIE_VICTORY_ROAD, continueScript=TRUE
	goto_if_defeated TRAINER_JESSIE_JAMES_VICTORY_ROAD, VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADWon
	goto VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADCleanup
VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
	msgbox VictoryRoad_3F_Text_PRL_RocketPostBattle
VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADCleanup::
	applymovement LOCALID_PRL_JESSIE_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_JAMES_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_VICTORY_ROAD, VictoryRoad_3F_Movement_PRL_RocketExit
	waitmovementall
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	setflag FLAG_HIDE_PRL_ROCKET_VICTORY_ROAD
	removeobject LOCALID_PRL_JESSIE_VICTORY_ROAD
	removeobject LOCALID_PRL_JAMES_VICTORY_ROAD
	removeobject LOCALID_PRL_MEOWTH_VICTORY_ROAD
VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
VictoryRoad_3F_EventScript_PRL_JESSIE_JAMES_VICTORY_ROADEnd::
	return

VictoryRoad_3F_Movement_PRL_RocketApproach::
	walk_up
	step_end

VictoryRoad_3F_Movement_PRL_RocketExit::
	walk_down
	step_end

VictoryRoad_3F_Text_PRL_RocketIntro::
	.string "JESSIE: From SILPH to here?!\p"
	.string "JAMES: GIOVANNI disbanded\n"
	.string "TEAM ROCKET...\p"
	.string "JESSIE: But we still have\n"
	.string "unfinished business with YOU!$"

VictoryRoad_3F_Text_PRL_RocketDefeat::
	.string "JAMES: Even our last plan!\p"
	.string "JESSIE: This is NOT how our\n"
	.string "grand finale should end!$"

VictoryRoad_3F_Text_PRL_RocketNotEnoughMons::
	.string "JESSIE: Bring at least two POKéMON\n"
	.string "if you want to stop TEAM ROCKET!$"

VictoryRoad_3F_Text_PRL_RocketPostBattle::
	.string "JAMES: The old clues point\n"
	.string "far beyond this mountain.\p"
	.string "JESSIE: Keep the gold-bird\n"
	.string "rumour to yourself, twerp!$"
