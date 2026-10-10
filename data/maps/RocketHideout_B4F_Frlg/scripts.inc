.equ NUM_DOOR_GRUNTS_DEFEATED, VAR_TEMP_1

RocketHideout_B4F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RocketHideout_B4F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, RocketHideout_B4F_OnTransition_PRLRocket
	.byte 0

RocketHideout_B4F_OnLoad::
	setflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	setvar NUM_DOOR_GRUNTS_DEFEATED, 0
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_16, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_defeated TRAINER_TEAM_ROCKET_GRUNT_17, RocketHideout_B4F_EventScript_CountGruntDefeated
	call_if_ne NUM_DOOR_GRUNTS_DEFEATED, 2, RocketHideout_B4F_EventScript_SetBarrier
	call RocketHideout_B4F_OnLoad_PRLRocketVisibility
	end

RocketHideout_B4F_EventScript_CountGruntDefeated::
	addvar NUM_DOOR_GRUNTS_DEFEATED, 1
	return

RocketHideout_B4F_EventScript_Giovanni::
	lock
	faceplayer
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
	msgbox RocketHideout_B4F_Text_PRL_StillGuarded
	release
	end

RocketHideout_B4F_Text_PRL_StillGuarded::
	.string "GIOVANNI: Defeat JESSIE and JAMES\n"
	.string "before approaching me!$"

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



@ Jessie and James behave as ordinary trainers, using native ! / walk-up.
@ One shared opponent ID makes the first alignment a single double battle.
@ Neither character is removed: each has separate post-battle dialogue.
RocketHideout_B4F_EventScript_PRL_Jessie::
	trainerbattle_double TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_Text_PRL_RocketIntro, RocketHideout_B4F_Text_PRL_RocketDefeat, RocketHideout_B4F_Text_PRL_RocketNotEnoughMons, RocketHideout_B4F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JESSIE_HIDEOUT
	msgbox RocketHideout_B4F_Text_PRL_JessiePostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B4F_EventScript_PRL_James::
	trainerbattle_double TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_Text_PRL_RocketIntro, RocketHideout_B4F_Text_PRL_RocketDefeat, RocketHideout_B4F_Text_PRL_RocketNotEnoughMons, RocketHideout_B4F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JAMES_HIDEOUT
	msgbox RocketHideout_B4F_Text_PRL_JamesPostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B4F_EventScript_PRL_Meowth::
	lock
	faceplayer
	goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_EventScript_PRL_MeowthAfter
	msgbox RocketHideout_B4F_Text_PRL_MeowthBefore
	release
	end
RocketHideout_B4F_EventScript_PRL_MeowthAfter::
	msgbox RocketHideout_B4F_Text_PRL_MeowthAfter
	release
	end

RocketHideout_B4F_EventScript_PRL_RocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT
	msgbox RocketHideout_B4F_Text_PRL_RocketPostBattle
	release
	end

RocketHideout_B4F_Text_PRL_JessiePostBattle::
	.string "JESSIE: Don't celebrate yet!\p"
	.string "TEAM ROCKET will still find\n"
	.string "the ultimate ancient RELIC!$"

RocketHideout_B4F_Text_PRL_JamesPostBattle::
	.string "JAMES: Every ancient specimen\n"
	.string "leads to another mystery.\p"
	.string "BOSS has a lot to learn!$"

RocketHideout_B4F_Text_PRL_MeowthBefore::
	.string "MEOWTH: Mrowr! Not another kid!$"

RocketHideout_B4F_Text_PRL_MeowthAfter::
	.string "MEOWTH: They lost AGAIN?!$"

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


@ Visibility is set before spawning map objects and refreshed on transition.
@ Neither function starts a battle or changes the native map's warp tiles.
RocketHideout_B4F_OnLoad_PRLRocketVisibility::
	goto_if_unset FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT, RocketHideout_B4F_PRLRocketVisibilityDone
	goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_PRLRocketAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
RocketHideout_B4F_PRLRocketVisibilityDone::
	return
RocketHideout_B4F_PRLRocketAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT
	clearflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	return

RocketHideout_B4F_OnTransition_PRLRocket::
	goto_if_unset FLAG_CAN_USE_ROCKET_HIDEOUT_LIFT, RocketHideout_B4F_PRLRocketTransitionDone
	goto_if_defeated TRAINER_JESSIE_JAMES_HIDEOUT, RocketHideout_B4F_PRLRocketTransitionAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
	goto RocketHideout_B4F_PRLRocketTransitionAdd
RocketHideout_B4F_PRLRocketTransitionAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_HIDEOUT
	clearflag FLAG_HIDE_PRL_ROCKET_HIDEOUT
RocketHideout_B4F_PRLRocketTransitionAdd::
	addobject LOCALID_PRL_JESSIE_HIDEOUT
	addobject LOCALID_PRL_JAMES_HIDEOUT
	addobject LOCALID_PRL_MEOWTH_HIDEOUT
RocketHideout_B4F_PRLRocketTransitionDone::
	return
