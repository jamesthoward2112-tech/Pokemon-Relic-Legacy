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
	.string "SILPH SCOPE?\n"
	.string "Oh, that machine the BOSS stole.\p"
	.string "It's here somewhere.$"

RocketHideout_B3F_Text_Grunt2Intro::
	.string "You again?! The BOSS has\n"
	.string "ordered the old stone marks\n"
	.string "sent for laboratory testing!\p"
	.string "You're not stopping us!$"

RocketHideout_B3F_Text_Grunt2Defeat::
	.string "What?\n"
	.string "I lost? No!$"

RocketHideout_B3F_Text_Grunt2PostBattle::
	.string "The fossil shipments are\n"
	.string "headed to SILPH CO.\p"
	.string "They can revive specimens\n"
	.string "and study dormant traits.$"

