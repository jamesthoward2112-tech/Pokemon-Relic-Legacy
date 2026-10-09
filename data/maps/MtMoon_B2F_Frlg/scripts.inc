MtMoon_B2F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MtMoon_B2F_OnTransition
	.byte 0

MtMoon_B2F_OnTransition::
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	call_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B2F_EventScript_ShowFossils
	call_if_set FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B2F_EventScript_PRL_RetryRocketBattle
	end

MtMoon_B2F_EventScript_PRL_RetryRocketBattle::
	lockall
	call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON
	releaseall
	return

MtMoon_B2F_EventScript_ShowFossils::
	clearflag FLAG_HIDE_DOME_FOSSIL
	clearflag FLAG_HIDE_HELIX_FOSSIL
	return

MtMoon_B2F_EventScript_MiguelTrigger::
	lockall
	applymovement LOCALID_MIGUEL, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	call MtMoon_B2F_EventScript_BattleMiguel
	releaseall
	end

MtMoon_B2F_EventScript_Miguel::
	lock
	faceplayer
	goto_if_set FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B2F_EventScript_MiguelFossilPicked
	goto_if_defeated TRAINER_SUPER_NERD_MIGUEL, MtMoon_B2F_EventScript_MiguelGoPickFossil
	call MtMoon_B2F_EventScript_BattleMiguel
	release
	end

MtMoon_B2F_EventScript_BattleMiguel::
	textcolor NPC_TEXT_COLOR_MALE
	message MtMoon_B2F_Text_MiguelIntro
	waitmessage
	playbgm MUS_RG_ENCOUNTER_GYM_LEADER, 0
	waitbuttonpress
	setvar VAR_LAST_TALKED, LOCALID_MIGUEL
	trainerbattle_no_intro TRAINER_SUPER_NERD_MIGUEL, MtMoon_B2F_Text_MiguelDefeat
	setvar VAR_MAP_SCENE_MT_MOON_B2F, 1
	msgbox MtMoon_B2F_Text_WellEachTakeAFossil
	return

MtMoon_B2F_EventScript_MiguelFossilPicked::
	msgbox MtMoon_B2F_Text_LabOnCinnabarRegeneratesFossils
	release
	end

MtMoon_B2F_EventScript_MiguelGoPickFossil::
	msgbox MtMoon_B2F_Text_WellEachTakeAFossil
	release
	end

MtMoon_B2F_EventScript_DomeFossil::
	lock
	faceplayer
	msgbox MtMoon_B2F_Text_YouWantDomeFossil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MtMoon_B2F_EventScript_DontTakeFossil
	removeobject LOCALID_DOME_FOSSIL
	giveitem_msg MtMoon_B2F_Text_ObtainedDomeFossil, ITEM_DOME_FOSSIL, 1, MUS_RG_OBTAIN_KEY_ITEM
	closemessage
	delay 10
	applymovement LOCALID_MIGUEL, MtMoon_B2F_Movement_MiguelToHelixFossil
	waitmovement 0
	copyobjectxytoperm LOCALID_MIGUEL
	textcolor NPC_TEXT_COLOR_MALE
	playfanfare MUS_RG_OBTAIN_KEY_ITEM
	message MtMoon_B2F_Text_ThenThisFossilIsMine
	waitmessage
	waitfanfare
	removeobject LOCALID_HELIX_FOSSIL
	setflag FLAG_GOT_DOME_FOSSIL
	setflag FLAG_GOT_FOSSIL_FROM_MT_MOON
	call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON
	release
	end

MtMoon_B2F_Movement_MiguelToHelixFossil::
	walk_right
	walk_up
	walk_up
	walk_up
	step_end

MtMoon_B2F_EventScript_HelixFossil::
	lock
	faceplayer
	msgbox MtMoon_B2F_Text_YouWantHelixFossil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MtMoon_B2F_EventScript_DontTakeFossil
	removeobject LOCALID_HELIX_FOSSIL
	giveitem_msg MtMoon_B2F_Text_ObtainedHelixFossil, ITEM_HELIX_FOSSIL, 1, MUS_RG_OBTAIN_KEY_ITEM
	closemessage
	delay 10
	applymovement LOCALID_MIGUEL, MtMoon_B2F_Movement_MiguelToDomeFossil
	waitmovement 0
	copyobjectxytoperm LOCALID_MIGUEL
	textcolor NPC_TEXT_COLOR_MALE
	playfanfare MUS_RG_OBTAIN_KEY_ITEM
	message MtMoon_B2F_Text_ThenThisFossilIsMine
	waitmessage
	waitfanfare
	removeobject LOCALID_DOME_FOSSIL
	setflag FLAG_GOT_HELIX_FOSSIL
	setflag FLAG_GOT_FOSSIL_FROM_MT_MOON
	call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON
	release
	end

MtMoon_B2F_EventScript_DontTakeFossil::
	release
	end

MtMoon_B2F_Movement_MiguelToDomeFossil::
	walk_up
	walk_up
	walk_up
	step_end

MtMoon_B2F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT, MtMoon_B2F_Text_Grunt1Intro, MtMoon_B2F_Text_Grunt1Defeat
	msgbox MtMoon_B2F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B2F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_2, MtMoon_B2F_Text_Grunt2Intro, MtMoon_B2F_Text_Grunt2Defeat
	msgbox MtMoon_B2F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B2F_EventScript_Grunt3::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_3, MtMoon_B2F_Text_Grunt3Intro, MtMoon_B2F_Text_Grunt3Defeat
	msgbox MtMoon_B2F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B2F_EventScript_Grunt4::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_4, MtMoon_B2F_Text_Grunt4Intro, MtMoon_B2F_Text_Grunt4Defeat
	msgbox MtMoon_B2F_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B2F_Text_MiguelIntro::
	.string "Hey, stop!\p"
	.string "I found these fossils!\n"
	.string "They're both mine!$"

MtMoon_B2F_Text_MiguelDefeat::
	.string "Okay!\n"
	.string "I'll share!$"

MtMoon_B2F_Text_WellEachTakeAFossil::
	.string "TEAM ROCKET almost took both!\p"
	.string "We'll each take a fossil.\n"
	.string "No being greedy!$"

MtMoon_B2F_Text_ThenThisFossilIsMine::
	.string "All right.\n"
	.string "Then this fossil is mine!$"

MtMoon_B2F_Text_LabOnCinnabarRegeneratesFossils::
	.string "Far away, on CINNABAR ISLAND,\n"
	.string "there's a POKéMON LAB.\p"
	.string "They do research on regenerating\n"
	.string "fossils.$"

MtMoon_B2F_Text_Grunt1Intro::
	.string "We, TEAM ROCKET, are taking\n"
	.string "every fossil in MT. MOON!\p"
	.string "BOSS wants the whole dig\n"
	.string "catalogued. Hand it over!$"

MtMoon_B2F_Text_Grunt1Defeat::
	.string "The excavation…!$"

MtMoon_B2F_Text_Grunt1PostBattle::
	.string "This was supposed to be a simple\n"
	.string "recovery job!\p"
	.string "Fossils, stones, old relics…\n"
	.string "BOSS wants all of it.$"

MtMoon_B2F_Text_Grunt2Intro::
	.string "This cave is TEAM ROCKET's\n"
	.string "recovery site!\p"
	.string "Anything ancient here belongs\n"
	.string "to BOSS!$"

MtMoon_B2F_Text_Grunt2Defeat::
	.string "The relics…!$"

MtMoon_B2F_Text_Grunt2PostBattle::
	.string "MT. MOON is only the beginning.\n"
	.string "We're searching all over KANTO.$"

MtMoon_B2F_Text_Grunt3Intro::
	.string "We're pulling a big job here!\p"
	.string "The fossils go straight to our\n"
	.string "research team. Get lost, kid!$"

MtMoon_B2F_Text_Grunt3Defeat::
	.string "You ruined the dig!$"

MtMoon_B2F_Text_Grunt3PostBattle::
	.string "You think this is about money?\n"
	.string "BOSS is hunting something older.$"

MtMoon_B2F_Text_Grunt4Intro::
	.string "Keep away from the specimens!\p"
	.string "Some of what we found predates\n"
	.string "KANTO's towns by centuries!$"

MtMoon_B2F_Text_Grunt4Defeat::
	.string "The specimens!$"

MtMoon_B2F_Text_Grunt4PostBattle::
	.string "If BOSS learns you wrecked our\n"
	.string "MT. MOON operation, we're done!$"

MtMoon_B2F_Text_YouWantDomeFossil::
	.string "Do you want the DOME FOSSIL?$"

MtMoon_B2F_Text_YouWantHelixFossil::
	.string "Do you want the HELIX FOSSIL?$"

MtMoon_B2F_Text_ObtainedHelixFossil::
	.string "Obtained the HELIX FOSSIL!$"

MtMoon_B2F_Text_ObtainedDomeFossil::
	.string "Obtained the DOME FOSSIL!$"


MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON::
	goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON, MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONEnd
	goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONEnd
	goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	addobject LOCALID_PRL_JESSIE_MT_MOON
	addobject LOCALID_PRL_JAMES_MT_MOON
	addobject LOCALID_PRL_MEOWTH_MT_MOON
	applymovement LOCALID_PRL_JESSIE_MT_MOON, MtMoon_B2F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_JAMES_MT_MOON, MtMoon_B2F_Movement_PRL_RocketApproach
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_MT_MOON, MtMoon_B2F_Movement_PRL_RocketApproach
	waitmovementall
	turnobject LOCALID_PRL_JESSIE_MT_MOON, DIR_SOUTH
	turnobject LOCALID_PRL_JAMES_MT_MOON, DIR_SOUTH
	turnobject LOCALID_PRL_MEOWTH_MT_MOON, DIR_SOUTH
	trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B2F_Text_PRL_RocketIntro, MtMoon_B2F_Text_PRL_RocketDefeat, MtMoon_B2F_Text_PRL_RocketNotEnoughMons
	goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONWon
	goto MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONCleanup
MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON
	msgbox MtMoon_B2F_Text_PRL_RocketPostBattle
MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONCleanup::
	applymovement LOCALID_PRL_JESSIE_MT_MOON, MtMoon_B2F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_JAMES_MT_MOON, MtMoon_B2F_Movement_PRL_RocketExit
	waitmovementall
	applymovement LOCALID_PRL_MEOWTH_MT_MOON, MtMoon_B2F_Movement_PRL_RocketExit
	waitmovementall
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	removeobject LOCALID_PRL_JESSIE_MT_MOON
	removeobject LOCALID_PRL_JAMES_MT_MOON
	removeobject LOCALID_PRL_MEOWTH_MT_MOON
MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON
MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOONEnd::
	return

MtMoon_B2F_Movement_PRL_RocketApproach::
	walk_up
	step_end

MtMoon_B2F_Movement_PRL_RocketExit::
	walk_down
	step_end

MtMoon_B2F_Text_PRL_RocketIntro::
	.string "JESSIE: Prepare for trouble!\p"
	.string "JAMES: And make it double!\p"
	.string "This Kanto operation is only one\n"
	.string "piece of a much older mystery.$"

MtMoon_B2F_Text_PRL_RocketDefeat::
	.string "JESSIE: This can't be!\p"
	.string "JAMES: We'll report back to the\n"
	.string "BOSS about the relic research!$"

MtMoon_B2F_Text_PRL_RocketNotEnoughMons::
	.string "JESSIE: Bring at least two POKéMON\n"
	.string "if you want to stop TEAM ROCKET!$"

MtMoon_B2F_Text_PRL_RocketPostBattle::
	.string "JAMES: The fossils, the relics…\p"
	.string "JESSIE: This Kanto project is just\n"
	.string "the beginning!$"
