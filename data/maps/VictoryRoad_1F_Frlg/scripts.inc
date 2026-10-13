VictoryRoad_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, VictoryRoad_1F_Frlg_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, VictoryRoad_1F_Frlg_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, VictoryRoad_1F_PRLTwinsOnFrame
	.byte 0


@ The final Eevee Twins battle happens inside the cave, just after the
@ Route 23 entrance (11,20), not behind the outdoor route's stone statues.
@ A map-frame event forces the encounter without relying on only one trainer
@ detecting the player. Their NPCs remain for friendly postbattle dialogue.
VictoryRoad_1F_PRLTwinsOnFrame::
	map_script_2 VAR_TEMP_1, 0, VictoryRoad_1F_EventScript_PRLTwinsEnter
	.2byte 0

VictoryRoad_1F_EventScript_PRLTwinsEnter::
	lockall
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23, VictoryRoad_1F_EventScript_PRLTwinsPreviouslyWon
	goto_if_unset FLAG_BADGE08_GET, VictoryRoad_1F_EventScript_PRLTwinsNoBadge
	setvar VAR_TEMP_1, 1
	playse SE_PIN
	applymovement LOCALID_PRL_STEEVE_ROUTE23, Common_Movement_ExclamationMark
	applymovement LOCALID_PRL_NEEVEE_ROUTE23, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_PRL_STEEVE_ROUTE23, VictoryRoad_1F_Movement_PRLSteeveApproach
	applymovement LOCALID_PRL_NEEVEE_ROUTE23, VictoryRoad_1F_Movement_PRLNeeveeApproach
	waitmovement 0
	goto VictoryRoad_1F_EventScript_PRLTwinsSteevee

VictoryRoad_1F_EventScript_PRLTwinsPreviouslyWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
VictoryRoad_1F_EventScript_PRLTwinsNoBadge::
	setvar VAR_TEMP_1, 1
	releaseall
	end

VictoryRoad_1F_EventScript_PRLTwinsSteevee::
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, VictoryRoad_1F_EventScript_PRLTwinsWon, localId=LOCALID_PRL_STEEVE_ROUTE23
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	releaseall
	end

VictoryRoad_1F_EventScript_PRLTwinsNeevee::
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, VictoryRoad_1F_EventScript_PRLTwinsWon, localId=LOCALID_PRL_NEEVEE_ROUTE23
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	releaseall
	end

VictoryRoad_1F_EventScript_PRLTwinsWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle
	releaseall
	end

VictoryRoad_1F_Movement_PRLSteeveApproach:
	walk_down
	face_down
	step_end

VictoryRoad_1F_Movement_PRLNeeveeApproach:
	walk_down
	face_down
	step_end

VictoryRoad_1F_Frlg_OnLoad::
	call_if_ne VAR_MAP_SCENE_VICTORY_ROAD_1F, 100, VictoryRoad_1F_EventScript_SetRockBarrier
	end

VictoryRoad_1F_EventScript_SetRockBarrier::
	setmetatile 12, 14, METATILE_Cave_RockBarrier_Top, 1
	setmetatile 12, 15, METATILE_Cave_RockBarrier_Bottom, 1
	return

VictoryRoad_1F_Frlg_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_VICTORY_ROAD_1F
	end

VictoryRoad_1F_EventScript_FloorSwitch::
	lockall
	goto_if_eq VAR_MAP_SCENE_VICTORY_ROAD_1F, 100, VictoryRoad_1F_EventScript_FloorSwitchAlreadyPressed
	setmetatile 12, 14, METATILE_Cave_Floor_Ledge_Top, 0
	setmetatile 12, 15, METATILE_Cave_Floor_Ledge_Bottom, 0
	playse SE_ICE_BREAK
	special DrawWholeMapView
	waitse
	copyobjectxytoperm LOCALID_VICTORY_ROAD_1F_BOULDER
	setvar VAR_MAP_SCENE_VICTORY_ROAD_1F, 100
	releaseall
	end

VictoryRoad_1F_EventScript_FloorSwitchAlreadyPressed::
	releaseall
	end

VictoryRoad_1F_EventScript_Naomi::
	trainerbattle_single TRAINER_COOLTRAINER_NAOMI, VictoryRoad_1F_Text_NaomiIntro, VictoryRoad_1F_Text_NaomiDefeat
	msgbox VictoryRoad_1F_Text_NaomiPostBattle, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_Rolando::
	trainerbattle_single TRAINER_COOLTRAINER_ROLANDO, VictoryRoad_1F_Text_RolandoIntro, VictoryRoad_1F_Text_RolandoDefeat, VictoryRoad_1F_EventScript_RolandoHeal
	goto VictoryRoad_1F_EventScript_RolandoHeal

VictoryRoad_1F_EventScript_RolandoHeal::
	lock
	faceplayer
	special HealPlayerParty
	msgbox VictoryRoad_1F_Text_RolandoHealing, MSGBOX_DEFAULT
	release
	end

VictoryRoad_1F_Text_NaomiIntro::
	.string "I wonder if you are good enough\n"
	.string "for me?$"

VictoryRoad_1F_Text_NaomiDefeat::
	.string "I lost out…$"

VictoryRoad_1F_Text_NaomiPostBattle::
	.string "I trained near old cliffs.\p"
	.string "Some stones show marks\n"
	.string "like those at MT. MOON.\p"
	.string "Maybe KANTO's ancient\n"
	.string "mystery isn't finished!$"

VictoryRoad_1F_Text_RolandoIntro::
	.string "I can see you're good.\n"
	.string "Let me see exactly how good!$"

VictoryRoad_1F_Text_RolandoDefeat::
	.string "I had a chance…$"

VictoryRoad_1F_Text_RolandoPostBattle::
	.string "Tch!\n"
	.string "I concede, you are better than I!$"


VictoryRoad_1F_Text_RolandoHealing::
	.string "That was a terrific battle!\p"
	.string "Let me restore your POKéMON\n"
	.string "before you carry on.\p"
	.string "Come back any time for a rest!$"
