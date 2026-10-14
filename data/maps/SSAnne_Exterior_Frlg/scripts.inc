SSAnne_Exterior_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SSAnne_Exterior_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SSAnne_Exterior_OnFrame
	.byte 0

SSAnne_Exterior_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_SSANNE_EXTERIOR
	end

SSAnne_Exterior_OnFrame::
	map_script_2 VAR_MAP_SCENE_VERMILION_CITY, 1, SSAnne_Exterior_ExitSSAnne
	.2byte 0

SSAnne_Exterior_ExitSSAnne::
	lockall
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_le VAR_0x8005, 6, SSAnne_Exterior_WalkDown
	call_if_ge VAR_0x8005, 7, SSAnne_Exterior_WalkInPlaceDown
	fadenewbgm MUS_RG_SURF
	delay 50
	special DoSSAnneDepartureCutscene
	waitstate
	removeobject LOCALID_SS_ANNE
	applymovement LOCALID_PLAYER, SSAnne_Exterior_Movement_Exit
	waitmovement 0
	setvar VAR_MAP_SCENE_VERMILION_CITY, 2
	setvar VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER, 0
	warp MAP_VERMILION_CITY, 23, 34
	waitstate
	releaseall
	end

SSAnne_Exterior_WalkDown::
	applymovement LOCALID_PLAYER, SSAnne_Exterior_Movement_WalkDown
	waitmovement 0
	return

SSAnne_Exterior_WalkInPlaceDown::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	return

SSAnne_Exterior_Movement_Exit::
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SSAnne_Exterior_Movement_WalkDown::
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

@ PKR V18 (user-confirmed 14 Sep): truck A-button event at x55-63 / y0-7.
@ Avoid guessed tile coordinates and NEVER overwrite Lava Cookie x58/y28.
@ This is the Vermilion S.S. Anne truck, NOT Hoenn's moving truck.
SSAnne_Exterior_EventScript_PRLTruckMew::
	lockall
	@ A single stale save bit must never suppress Mew for an older Champion.
	@ Only this encounter's successful capture sets BOTH fresh markers.
	goto_if_unset FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT, SSAnne_Exterior_EventScript_PRLTruckMewAvailable
	goto_if_set FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED, SSAnne_Exterior_EventScript_PRLTruckMewQuiet

SSAnne_Exterior_EventScript_PRLTruckMewAvailable::
	@ Keep the PKR post-League unlock, recognising old and new PRL HoF saves.
	specialvar VAR_RESULT, PRLHasKantoHallOfFameRecord
	goto_if_eq VAR_RESULT, FALSE, SSAnne_Exterior_EventScript_PRLTruckMewDormant
	msgbox SSAnne_Exterior_Text_PRLTruckMewShakes
	waitse
	playmoncry SPECIES_MEW, CRY_MODE_ENCOUNTER
	waitmoncry
	delay 20
	setwildbattle SPECIES_MEW, 50
	special BattleSetup_StartLegendaryBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SSAnne_Exterior_EventScript_PRLTruckMewCaught
	@ KO, run and all teleport/flee outcomes preserve the encounter.
	msgbox SSAnne_Exterior_Text_PRLTruckMewEscaped
	releaseall
	end

SSAnne_Exterior_EventScript_PRLTruckMewCaught::
	setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT
	setflag FLAG_PRL_SSANNE_TRUCK_MEW_CAPTURE_CONFIRMED
	msgbox SSAnne_Exterior_Text_PRLTruckMewQuiet
	releaseall
	end

SSAnne_Exterior_EventScript_PRLTruckMewQuiet::
	msgbox SSAnne_Exterior_Text_PRLTruckMewQuiet
	releaseall
	end

SSAnne_Exterior_EventScript_PRLTruckMewDormant::
	msgbox SSAnne_Exterior_Text_PRLTruckMewDormant
	releaseall
	end

SSAnne_Exterior_Text_PRLTruckMewShakes::
	.string "The truck suddenly began to\n"
	.string "shake...!$"

SSAnne_Exterior_Text_PRLTruckMewEscaped::
	.string "The mysterious POKéMON slipped\n"
	.string "away. The truck is still...$"

SSAnne_Exterior_Text_PRLTruckMewQuiet::
	.string "The truck is quiet now.$"

SSAnne_Exterior_Text_PRLTruckMewDormant::
	.string "An old truck. Something about\n"
	.string "it feels strangely familiar.$"
