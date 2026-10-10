RocketHideout_B2F_Frlg_MapScripts::
	.byte 0

RocketHideout_B2F_EventScript_Grunt::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_13, RocketHideout_B2F_Text_GruntIntro, RocketHideout_B2F_Text_GruntDefeat
	msgbox RocketHideout_B2F_Text_GruntPostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B2F_Text_GruntIntro::
	.string "No one enters this archive!\p"
	.string "BOSS needs the SILPH SCOPE\n"
	.string "to examine unseen energy.\p"
	.string "Not even ghosts can hide\n"
	.string "ancient traces from us!$"

RocketHideout_B2F_Text_GruntDefeat::
	.string "I surrender!$"

RocketHideout_B2F_Text_GruntPostBattle::
	.string "Four floors underground...\p"
	.string "Still no room for all the\n"
	.string "fossil and RELIC catalogues!$"

