SilphCo_8F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SilphCo_8F_OnLoad
	.byte 0

SilphCo_8F_OnLoad::
	call_if_unset FLAG_SILPH_8F_DOOR, EventScript_Close8FDoor
	end

SilphCo_8F_EventScript_WorkerM::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_8F_EventScript_WorkerMRocketsGone
	msgbox SilphCo_8F_Text_WonderIfSilphIsFinished
	release
	end

SilphCo_8F_EventScript_WorkerMRocketsGone::
	msgbox SilphCo_8F_Text_ThanksForSavingUs
	release
	end

SilphCo_8F_EventScript_FloorSign::
	msgbox SilphCo_8F_Text_FloorSign, MSGBOX_SIGN
	end

SilphCo_8F_EventScript_Parker::
	trainerbattle_single TRAINER_SCIENTIST_PARKER, SilphCo_8F_Text_ParkerIntro, SilphCo_8F_Text_ParkerDefeat
	msgbox SilphCo_8F_Text_ParkerPostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_8F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_32, SilphCo_8F_Text_Grunt1Intro, SilphCo_8F_Text_Grunt1Defeat
	msgbox SilphCo_8F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_8F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_36, SilphCo_8F_Text_Grunt2Intro, SilphCo_8F_Text_Grunt2Defeat
	msgbox SilphCo_8F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_8F_Text_WonderIfSilphIsFinished::
	.string "SILPH may be finished...\p"
	.string "ROCKET copied every report\n"
	.string "on those dormant RELIC traits.$"

SilphCo_8F_Text_ThanksForSavingUs::
	.string "Thanks for saving us!$"

SilphCo_8F_Text_Grunt1Intro::
	.string "Stop! No visitors near\n"
	.string "the archaeological data!\p"
	.string "TEAM ROCKET owns it now!$"

SilphCo_8F_Text_Grunt1Defeat::
	.string "Not enough grit!$"

SilphCo_8F_Text_Grunt1PostBattle::
	.string "Our team decoded an old\n"
	.string "RELIC diagram.\p"
	.string "It shows a birdlike shape\n"
	.string "painted in gold.\p"
	.string "But that doesn't mean\n"
	.string "the thing really exists!$"

SilphCo_8F_Text_ParkerIntro::
	.string "You found our RELIC lab?!\p"
	.string "Those ancient sketches\n"
	.string "must never leave SILPH!$"

SilphCo_8F_Text_ParkerDefeat::
	.string "Huh?\n"
	.string "I lost?$"

SilphCo_8F_Text_ParkerPostBattle::
	.string "One drawing shows wings\n"
	.string "outlined in faded gold.\p"
	.string "Not even the scientists\n"
	.string "know what species it is.$"

SilphCo_8F_Text_Grunt2Intro::
	.string "I'm one of the four\n"
	.string "ROCKET BROTHERS!\p"
	.string "The BOSS's golden specimen\n"
	.string "will make us unstoppable!$"

SilphCo_8F_Text_Grunt2Defeat::
	.string "Whoo!\n"
	.string "Oh, Brothers!$"

SilphCo_8F_Text_Grunt2PostBattle::
	.string "Even our experts haven't\n"
	.string "identified that gold figure.\p"
	.string "The BOSS calls it a prize!$"

SilphCo_8F_Text_FloorSign::
	.string "SILPH CO. HEAD OFFICE\n"
	.string "8F$"

SilphCo_8F_Text_ToRocketBossMonsAreTools::
	.string "To ROCKET's BOSS, POKéMON\n"
	.string "are nothing but tools.\p"
	.string "Now he wants to revive\n"
	.string "ancient species as weapons!$"
