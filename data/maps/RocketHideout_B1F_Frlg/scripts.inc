RocketHideout_B1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RocketHideout_B1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, RocketHideout_B1F_OnTransition
	.byte 0

RocketHideout_B1F_OnLoad::
	call_if_not_defeated TRAINER_TEAM_ROCKET_GRUNT_12 RocketHideout_B1F_EventScript_SetBarrier
	end

RocketHideout_B1F_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_ROCKET_HIDEOUT_B1F
	end

RocketHideout_B1F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_8, RocketHideout_B1F_Text_Grunt1Intro, RocketHideout_B1F_Text_Grunt1Defeat
	msgbox RocketHideout_B1F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B1F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_9, RocketHideout_B1F_Text_Grunt2Intro, RocketHideout_B1F_Text_Grunt2Defeat
	msgbox RocketHideout_B1F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B1F_EventScript_Grunt3::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_10, RocketHideout_B1F_Text_Grunt3Intro, RocketHideout_B1F_Text_Grunt3Defeat
	msgbox RocketHideout_B1F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B1F_EventScript_Grunt4::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_11, RocketHideout_B1F_Text_Grunt4Intro, RocketHideout_B1F_Text_Grunt4Defeat
	msgbox RocketHideout_B1F_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B1F_EventScript_Grunt5::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_12, RocketHideout_B1F_Text_Grunt5Intro, RocketHideout_B1F_Text_Grunt5Defeat, RocketHideout_B1F_EventScript_DefeatedGrunt5
	msgbox RocketHideout_B1F_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B1F_EventScript_DefeatedGrunt5::
	call RocketHideout_B1F_EventScript_RemoveBarrier
	playse SE_UNLOCK
	special DrawWholeMapView
	waitse
	release
	end

RocketHideout_B1F_EventScript_SetBarrier::
	setmetatile 20, 19, METATILE_SilphCo_HideoutBarrier_TopLeft_Floor, 1
	setmetatile 21, 19, METATILE_SilphCo_HideoutBarrier_TopRight_Floor, 1
	setmetatile 20, 20, METATILE_SilphCo_HideoutBarrier_BottomLeft, 1
	setmetatile 21, 20, METATILE_SilphCo_HideoutBarrier_BottomRight, 1
	setmetatile 20, 21, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 21, 21, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	return

RocketHideout_B1F_EventScript_RemoveBarrier::
	setmetatile 20, 19, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 21, 19, METATILE_SilphCo_HideoutFloor, 0
	setmetatile 20, 20, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 21, 20, METATILE_SilphCo_HideoutFloor, 0
	setmetatile 20, 21, METATILE_SilphCo_HideoutFloor_ShadeFull, 0
	setmetatile 21, 21, METATILE_SilphCo_HideoutFloor, 0
	return

RocketHideout_B1F_Text_Grunt1Intro::
	.string "Hold it! This storage floor is\n"
	.string "for TEAM ROCKET's dig crew!\p"
	.string "Our RELIC crates aren't for\n"
	.string "nosy kids to examine!$"

RocketHideout_B1F_Text_Grunt1Defeat::
	.string "Oww!\n"
	.string "Beaten!$"

RocketHideout_B1F_Text_Grunt1PostBattle::
	.string "The fossils aren't treasure.\p"
	.string "Some contain a strange mark.\n"
	.string "The BOSS calls it a RELIC.\p"
	.string "We catalogue every fragment!$"

RocketHideout_B1F_Text_Grunt2Intro::
	.string "TEAM ROCKET's fossil cargo\n"
	.string "is none of your business!\p"
	.string "Turn around, kid!$"

RocketHideout_B1F_Text_Grunt2Defeat::
	.string "Boom!$"

RocketHideout_B1F_Text_Grunt2PostBattle::
	.string "Those crates came from\n"
	.string "MT. MOON and beyond.\p"
	.string "Even broken artifacts go\n"
	.string "straight to the scientists.$"

RocketHideout_B1F_Text_Grunt3Intro::
	.string "The excavation records\n"
	.string "are confidential!\p"
	.string "You won't find the specimen\n"
	.string "manifest on this floor!$"

RocketHideout_B1F_Text_Grunt3Defeat::
	.string "I can't do it!$"

RocketHideout_B1F_Text_Grunt3PostBattle::
	.string "A sealed specimen manifest...\p"
	.string "It lists MT. MOON,\n"
	.string "CERULEAN and other dig sites.\p"
	.string "All the marked fragments\n"
	.string "were sent to SILPH CO.$"

RocketHideout_B1F_Text_Grunt4Intro::
	.string "This floor stores every\n"
	.string "RELIC dig-site ledger!\p"
	.string "You can't reach the BOSS!$"

RocketHideout_B1F_Text_Grunt4Defeat::
	.string "This won't do!$"

RocketHideout_B1F_Text_Grunt4PostBattle::
	.string "Okay, I'll talk...\p"
	.string "Take the elevator to BOSS.\n"
	.string "He has the RELIC ledgers!$"

RocketHideout_B1F_Text_Grunt5Intro::
	.string "Are you lost, little mouse?\p"
	.string "One more guard and those\n"
	.string "RELIC vault doors open...$"

RocketHideout_B1F_Text_Grunt5Defeat::
	.string "Why…?$"

RocketHideout_B1F_Text_Grunt5PostBattle::
	.string "Uh-oh! You broke the guard!\p"
	.string "The vault's barrier has\n"
	.string "opened! BOSS will be furious!$"

