VictoryRoad_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, VictoryRoad_1F_Frlg_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, VictoryRoad_1F_Frlg_OnTransition
	.byte 0

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

@ The final Kanto Eevee Twins battle uses native trainer line of sight.
@ Both stand on the upper ledge (elevation 4), safely clear of Strength.
@ Normal trainerbattle_double keeps them visible after victory, including old saves.
VictoryRoad_1F_EventScript_PRLSteevee::
	goto_if_unset FLAG_BADGE08_GET, VictoryRoad_1F_EventScript_PRLTwinsNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, VictoryRoad_1F_EventScript_PRLTwinsWon, localId=LOCALID_PRL_VR_TWIN_STEEVE
	msgbox VictoryRoad_1F_Text_PRLSteeveeAfter, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_PRLNeevee::
	goto_if_unset FLAG_BADGE08_GET, VictoryRoad_1F_EventScript_PRLTwinsNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, VictoryRoad_1F_EventScript_PRLTwinsWon, localId=LOCALID_PRL_VR_TWIN_NEEVEE
	msgbox VictoryRoad_1F_Text_PRLNeeveeAfter, MSGBOX_AUTOCLOSE
	end

VictoryRoad_1F_EventScript_PRLTwinsNotReady::
	end

VictoryRoad_1F_EventScript_PRLTwinsWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	msgbox VictoryRoad_1F_Text_PRLTwinsFarewell
	@ A native trainer can walk toward the player; put both back on the upper ledge.
	setobjectxy LOCALID_PRL_VR_TWIN_STEEVE, 9, 13
	setobjectxy LOCALID_PRL_VR_TWIN_NEEVEE, 10, 13
	release
	end

VictoryRoad_1F_Text_PRLTwinsFarewell::
	.string "STEEVEE: From ROUTE 3 to here…\p"
	.string "You've beaten us every time!\p"
	.string "NEEVEE: KANTO was incredible.\p"
	.string "We'll train even harder now!\p"
	.string "STEEVEE: This isn't goodbye.\p"
	.string "NEEVEE: See you in HOENN,\n"
	.string "{PLAYER}!$"

VictoryRoad_1F_Text_PRLSteeveeAfter::
	.string "STEEVEE: What a journey!\p"
	.string "We're heading to HOENN next.\n"
	.string "We'll be ready for you!$"

VictoryRoad_1F_Text_PRLNeeveeAfter::
	.string "NEEVEE: This was our last\n"
	.string "KANTO battle together.\p"
	.string "See you in HOENN, {PLAYER}!$"

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
