.equ NUM_DOOR_GRUNTS_DEFEATED, VAR_TEMP_1

RocketHideout_B4F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RocketHideout_B4F_OnLoad
	.byte 0

RocketHideout_B4F_OnLoad::
	setflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	setvar NUM_DOOR_GRUNTS_DEFEATED, 0
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_16, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_17, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_ne NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_SetBarrier
	end

RocketHideout_B4F_EventScript_CountGruntDefeated::
	addvar NUM_DOOR_GRUNTS_DEFEATED, 1
	return

RocketHideout_B4F_EventScript_Giovanni::
	lock
	faceplayer
	call RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUT
	goto_if_unset FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_EventScript_GiovanniWaitForJessieJames
	famechecker FAMECHECKER_GIOVANNI, 0
	message RocketHideout_B4F_Text_GiovanniIntro
	waitmessage
	playbgm MUS_RG_ENCOUNTER_ROCKET, 0
	waitbuttonpress
	trainerbattle_no_intro TRAINER_BOSS_GIOVANNI, RocketHideout_B4F_Text_GiovanniDefeat
	msgbox RocketHideout_B4F_Text_GiovanniPostBattle
	fadescreen FADE_TO_BLACK
	closemessage
	removeobject LOCALID_HIDEOUT_GIOVANNI
	addobject LOCALID_SILPH_SCOPE
	clearflag FLAG_HIDE_SILPH_SCOPE
	setflag FLAG_HIDE_CELADON_ROCKETS
	famechecker FAMECHECKER_GIOVANNI, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
	fadescreen FADE_FROM_BLACK
	release
	end

RocketHideout_B4F_EventScript_GiovanniWaitForJessieJames::
	release
	end

RocketHideout_B4F_EventScript_SilphScope::
	lock
	faceplayer
	removeobject LOCALID_SILPH_SCOPE
	giveitem ITEM_SILPH_SCOPE
	goto_if_eq VAR_RESULT, FALSE, EventScript_BagIsFull
	release
	end

RocketHideout_B4F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_18, RocketHideout_B4F_Text_Grunt1Intro, RocketHideout_B4F_Text_Grunt1Defeat, RocketHideout_B4F_EventScript_DefeatedGrunt1
	msgbox RocketHideout_B4F_Text_Grunt1PostBattle
	release
	end

RocketHideout_B4F_EventScript_DefeatedGrunt1::
	msgbox RocketHideout_B4F_Text_Grunt1PostBattle
	addobject LOCALID_LIFT_KEY
	clearflag FLAG_HIDE_LIFT_KEY
	release
	end

RocketHideout_B4F_EventScript_LiftKey::
	lock
	faceplayer
	setflag FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT
	removeobject LOCALID_LIFT_KEY
	giveitem ITEM_LIFT_KEY
	goto_if_eq VAR_RESULT, FALSE, EventScript_BagIsFull
	release
	end

RocketHideout_B4F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_16, RocketHideout_B4F_Text_Grunt2Intro, RocketHideout_B4F_Text_Grunt2Defeat, RocketHideout_B4F_EventScript_DefeatedGrunt2
	msgbox RocketHideout_B4F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B4F_EventScript_DefeatedGrunt2::
	setvar NUM_DOOR_GRUNTS_DEFEATED, 0
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_16, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_17, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_eq NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_RemoveBarrier
	call_if_eq NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_DrawMapForBarrierRemoval
	release
	end

RocketHideout_B4F_EventScript_Grunt3::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_17, RocketHideout_B4F_Text_Grunt3Intro, RocketHideout_B4F_Text_Grunt3Defeat, RocketHideout_B4F_EventScript_DefeatedGrunt3
	msgbox RocketHideout_B4F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B4F_EventScript_DefeatedGrunt3::
	setvar NUM_DOOR_GRUNTS_DEFEATED, 0
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_16, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_17, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_eq NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_RemoveBarrier
	call_if_eq NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_DrawMapForBarrierRemoval
	release
	end

RocketHideout_B4F_EventScript_DrawMapForBarrierRemoval::
	playse SE_UNLOCK
	special DrawWholeMapView
	waitse
	return

RocketHideout_B4F_EventScript_SetBarrier::
	setmetatile 17, 12, METATILE_SilphCo_HideoutBarrier_TopLeft_Floor, 1
	setmetatile 18, 12, METATILE_SilphCo_HideoutBarrier_TopRight_Floor, 1
	setmetatile 17, 13, METATILE_SilphCo_HideoutBarrier_BottomLeft, 1
	setmetatile 18, 13, METATILE_SilphCo_HideoutBarrier_BottomRight, 1
	setmetatile 17, 14, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 18, 14, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	return

RocketHideout_B4F_EventScript_RemoveBarrier::
	setmetatile 17, 12, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 18, 12, METATILE_SilphCo_HideoutFloor, 0
	setmetatile 17, 13, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 18, 13, METATILE_SilphCo_HideoutFloor, 0
	setmetatile 17, 14, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 18, 14, METATILE_SilphCo_HideoutFloor, 0
	return

RocketHideout_B4F_Text_GiovanniIntro::
	.string "GIOVANNI: So you found us.\p"
	.string "MT. MOON was merely the first\n"
	.string "of our excavation sites.\p"
	.string "These ancient specimens may\n"
	.string "conceal a power long dormant.\p"
	.string "TEAM ROCKET will awaken it!$"

RocketHideout_B4F_Text_GiovanniDefeat::
	.string "WHAT!\n"
	.string "This can't be!$"

RocketHideout_B4F_Text_GiovanniPostBattle::
	.string "You fight with conviction.\p"
	.string "But our fossil records are\n"
	.string "already on their way to SILPH.\p"
	.string "You have delayed my research,\n"
	.string "not ended it. Farewell!$"

RocketHideout_B4F_Text_Grunt2Intro::
	.string "I know you!\n"
	.string "You ruined our plans at MT. MOON!$"

RocketHideout_B4F_Text_Grunt2Defeat::
	.string "Burned again!$"

RocketHideout_B4F_Text_Grunt2PostBattle::
	.string "MT. MOON's fossils are only\n"
	.string "one part of the operation!$"

RocketHideout_B4F_Text_Grunt3Intro::
	.string "The BOSS has crates of\n"
	.string "fossils from across KANTO.\p"
	.string "You won't see his findings!$"

RocketHideout_B4F_Text_Grunt3Defeat::
	.string "Ayaya!$"

RocketHideout_B4F_Text_Grunt3PostBattle::
	.string "BOSS!\n"
	.string "I'm sorry I failed you!$"

RocketHideout_B4F_Text_Grunt1Intro::
	.string "The elevator doesn't work?\n"
	.string "Who has the LIFT KEY?$"

RocketHideout_B4F_Text_Grunt1Defeat::
	.string "No!$"

RocketHideout_B4F_Text_Grunt1PostBattle::
	.string "Oh, no!\n"
	.string "I dropped the LIFT KEY!$"


RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUT::
	goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTEnd
	goto_if_unset FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT, RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTEnd
	goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	addobject LOCALID_PRL_JESSIE_HIDEOUT
	addobject LOCALID_PRL_JAMES_HIDEOUT
	addobject LOCALID_PRL_MEOWTH_HIDEOUT
	applymovement LOCALID_PRL_JESSIE_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_JAMES_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketApproach
	waitmovementall
	turnobject LOCALID_PRL_JESSIE_HIDEOUT, DIR_SOUTH
	turnobject LOCALID_PRL_JAMES_HIDEOUT, DIR_SOUTH
	turnobject LOCALID_PRL_MEOWTH_HIDEOUT, DIR_SOUTH
	trainerbattle_double TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_Text_PRL_RocketIntro, RocketHideout_B4F_Text_PRL_RocketDefeat, RocketHideout_B4F_Text_PRL_RocketNotEnoughMons, localId=LOCALID_PRL_JESSIE_HIDEOUT, continueScript=TRUE
	goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTWon
	goto RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTCleanup
RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT
	msgbox RocketHideout_B4F_Text_PRL_RocketPostBattle
RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTCleanup::
	applymovement LOCALID_PRL_JESSIE_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_JAMES_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_HIDEOUT, RocketHideout_B4F_Movement_PRL_RocketExit
	waitmovementall
	setflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	removeobject LOCALID_PRL_JESSIE_HIDEOUT
	removeobject LOCALID_PRL_JAMES_HIDEOUT
	removeobject LOCALID_PRL_MEOWTH_HIDEOUT
RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT
RocketHideout_B4F_EventScript_PRL_JESSIE_JAMES_HIDEOUTEnd::
	return

RocketHideout_B4F_Movement_PRL_RocketApproach::
	walk_up
	step_end

RocketHideout_B4F_Movement_PRL_RocketExit::
	walk_down
	step_end

RocketHideout_B4F_Text_PRL_RocketIntro::
	.string "JESSIE: You ruined MT. MOON!\p"
	.string "JAMES: These fossil ledgers\n"
	.string "are TOP SECRET!\p"
	.string "JESSIE: You read nothing!$"

RocketHideout_B4F_Text_PRL_RocketDefeat::
	.string "JAMES: The specimen ledger!\p"
	.string "JESSIE: Don't let that brat\n"
	.string "see the ancient diagrams!$"

RocketHideout_B4F_Text_PRL_RocketNotEnoughMons::
	.string "JESSIE: Bring at least two POKéMON\n"
	.string "if you want to stop TEAM ROCKET!$"

RocketHideout_B4F_Text_PRL_RocketPostBattle::
	.string "JAMES: SILPH has our samples.\p"
	.string "JESSIE: The BOSS expects them\n"
	.string "to wake an ancient power!$"
