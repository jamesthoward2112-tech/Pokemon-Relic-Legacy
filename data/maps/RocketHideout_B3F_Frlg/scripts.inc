RocketHideout_B3F_Frlg_MapScripts::
	.byte 0

RocketHideout_B3F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_14, RocketHideout_B3F_Text_Grunt1Intro, RocketHideout_B3F_Text_Grunt1Defeat
	msgbox RocketHideout_B3F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B3F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_15, RocketHideout_B3F_Text_Grunt2Intro, RocketHideout_B3F_Text_Grunt2Defeat
	msgbox RocketHideout_B3F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

RocketHideout_B3F_Text_Grunt1Intro::
	.string "Stop! These archives contain\n"
	.string "ancient fossil data!\p"
	.string "We cannot let you reach\n"
	.string "the BOSS's lab!$"

RocketHideout_B3F_Text_Grunt1Defeat::
	.string "Oof!\n"
	.string "Taken down!$"

RocketHideout_B3F_Text_Grunt1PostBattle::
	.string "The SILPH SCOPE sees energy\n"
	.string "that ordinary eyes miss.\p"
	.string "Our scientists detected\n"
	.string "the same mark in fossils.\p"
	.string "You weren't meant to know!$"

RocketHideout_B3F_Text_Grunt2Intro::
	.string "BOSS wants the ancient marks\n"
	.string "taken to the SILPH labs!\p"
	.string "He calls them RELIC traits.\n"
	.string "You won't spoil his research!$"

RocketHideout_B3F_Text_Grunt2Defeat::
	.string "What?\n"
	.string "I lost? No!$"

RocketHideout_B3F_Text_Grunt2PostBattle::
	.string "The fossil shipments are\n"
	.string "headed to SILPH CO.\p"
	.string "They can revive specimens\n"
	.string "and study dormant traits.$"

