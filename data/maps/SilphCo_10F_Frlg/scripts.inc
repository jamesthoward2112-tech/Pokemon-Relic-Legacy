SilphCo_10F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SilphCo_10F_OnLoad
	.byte 0

SilphCo_10F_OnLoad::
	call_if_unset FLAG_SILPH_10F_DOOR, EventScript_Close10FDoor
	end

SilphCo_10F_EventScript_WorkerF::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_10F_EventScript_WorkerFRocketsGone
	msgbox SilphCo_10F_Text_WaaaImScared
	release
	end

SilphCo_10F_EventScript_WorkerFRocketsGone::
	msgbox SilphCo_10F_Text_KeepMeCryingASecret
	release
	end

SilphCo_10F_EventScript_FloorSign::
	msgbox SilphCo_10F_Text_FloorSign, MSGBOX_SIGN
	end

SilphCo_10F_EventScript_Travis::
	trainerbattle_single TRAINER_SCIENTIST_TRAVIS, SilphCo_10F_Text_TravisIntro, SilphCo_10F_Text_TravisDefeat
	msgbox SilphCo_10F_Text_TravisPostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_10F_EventScript_Grunt::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_39, SilphCo_10F_Text_GruntIntro, SilphCo_10F_Text_GruntDefeat
	msgbox SilphCo_10F_Text_GruntPostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_10F_Text_GruntIntro::
	.string "Welcome to 10F!\p"
	.string "The MASTER BALL research\n"
	.string "is nearly complete.\p"
	.string "You won't reach the BOSS\n"
	.string "or his ancient prize!$"

SilphCo_10F_Text_GruntDefeat::
	.string "I'm stunned!$"

SilphCo_10F_Text_GruntPostBattle::
	.string "The MASTER BALL records\n"
	.string "went to the top floor.\p"
	.string "A golden bird? That's just\n"
	.string "an unproven old legend!$"

SilphCo_10F_Text_TravisIntro::
	.string "No one gets to the BOSS!\p"
	.string "Especially not a kid who\n"
	.string "knows about the RELIC files!$"

SilphCo_10F_Text_TravisDefeat::
	.string "No continues left!$"

SilphCo_10F_Text_TravisPostBattle::
	.string "You're nearing the boardroom.\p"
	.string "The MASTER BALL prototype\n"
	.string "and golden-sketch reports\n"
	.string "are on the top floor.$"

SilphCo_10F_Text_WaaaImScared::
	.string "Waaaaa!\n"
	.string "I'm scared!$"

SilphCo_10F_Text_KeepMeCryingASecret::
	.string "About me crying…\n"
	.string "Please keep it a secret.$"

SilphCo_10F_Text_FloorSign::
	.string "SILPH CO. HEAD OFFICE\n"
	.string "10F$"

