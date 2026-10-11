SilphCo_6F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SilphCo_6F_OnLoad
	.byte 0

SilphCo_6F_OnLoad::
	call_if_unset FLAG_SILPH_6F_DOOR, EventScript_Close6FDoor
	end

SilphCo_6F_EventScript_WorkerM2::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_6F_EventScript_WorkerM2RocketsGone
	msgbox SilphCo_6F_Text_TargetedSilphForOurMonProducts
	release
	end

SilphCo_6F_EventScript_WorkerM2RocketsGone::
	msgbox SilphCo_6F_Text_ComeWorkForSilphWhenYoureOlder
	release
	end

SilphCo_6F_EventScript_WorkerM3::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_6F_EventScript_WorkerM3RocketsGone
	msgbox SilphCo_6F_Text_RocketsTookOverBuilding
	release
	end

SilphCo_6F_EventScript_WorkerM3RocketsGone::
	msgbox SilphCo_6F_Text_BetterGetBackToWork
	release
	end

SilphCo_6F_EventScript_WorkerM1::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_6F_EventScript_WorkerM1RocketsGone
	msgbox SilphCo_6F_Text_HelpMePlease
	release
	end

SilphCo_6F_EventScript_WorkerM1RocketsGone::
	msgbox SilphCo_6F_Text_WeGotEngaged
	release
	end

SilphCo_6F_EventScript_WorkerF1::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_6F_EventScript_WorkerF1RocketsGone
	msgbox SilphCo_6F_Text_ThatManIsSuchACoward
	release
	end

SilphCo_6F_EventScript_WorkerF1RocketsGone::
	msgbox SilphCo_6F_Text_NeedsMeToLookAfterHim
	release
	end

SilphCo_6F_EventScript_WorkerF2::
	lock
	faceplayer
	goto_if_ge VAR_MAP_SCENE_SILPH_CO_11F, 1, SilphCo_6F_EventScript_WorkerF2RocketsGone
	msgbox SilphCo_6F_Text_RocketsTryingToConquerWorld
	release
	end

SilphCo_6F_EventScript_WorkerF2RocketsGone::
	msgbox SilphCo_6F_Text_RocketsRanAwayBecauseOfYou
	release
	end

SilphCo_6F_EventScript_FloorSign::
	msgbox SilphCo_6F_Text_FloorSign, MSGBOX_SIGN
	end

SilphCo_6F_EventScript_Taylor::
	trainerbattle_single TRAINER_SCIENTIST_TAYLOR, SilphCo_6F_Text_TaylorIntro, SilphCo_6F_Text_TaylorDefeat
	msgbox SilphCo_6F_Text_TaylorPostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_6F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_30, SilphCo_6F_Text_Grunt1Intro, SilphCo_6F_Text_Grunt1Defeat
	msgbox SilphCo_6F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_6F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_31, SilphCo_6F_Text_Grunt2Intro, SilphCo_6F_Text_Grunt2Defeat
	msgbox SilphCo_6F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_6F_Text_RocketsTookOverBuilding::
	.string "The ROCKETS seized SILPH!\p"
	.string "They searched every floor\n"
	.string "for RELIC and fossil data.$"

SilphCo_6F_Text_BetterGetBackToWork::
	.string "Well, better get back to work.$"

SilphCo_6F_Text_HelpMePlease::
	.string "Oh dear, oh dear.\n"
	.string "Help me, please!$"

SilphCo_6F_Text_WeGotEngaged::
	.string "We got engaged.\n"
	.string "Heheh!$"

SilphCo_6F_Text_ThatManIsSuchACoward::
	.string "That man next to me…\n"
	.string "He's such a coward!$"

SilphCo_6F_Text_NeedsMeToLookAfterHim::
	.string "He's so helpless, he needed\n"
	.string "someone like me to look after him.$"

SilphCo_6F_Text_RocketsTryingToConquerWorld::
	.string "ROCKET plans to control KANTO\n"
	.string "with revived fossil POKéMON.\p"
	.string "They're looking for an\n"
	.string "older power behind them!$"

SilphCo_6F_Text_RocketsRanAwayBecauseOfYou::
	.string "TEAM ROCKET ran away, and it's all\n"
	.string "because of you!$"

SilphCo_6F_Text_TargetedSilphForOurMonProducts::
	.string "ROCKET wanted our POKéMON\n"
	.string "technology, yes.\p"
	.string "But they stole the archive\n"
	.string "on ancient RELIC specimens!\p"
	.string "Some samples came from\n"
	.string "far beneath KANTO.$"

SilphCo_6F_Text_ComeWorkForSilphWhenYoureOlder::
	.string "When you're older, perhaps\n"
	.string "you could work for SILPH.\p"
	.string "We need someone who studies\n"
	.string "RELICS without exploiting them.$"

SilphCo_6F_Text_Grunt1Intro::
	.string "I am one of the four\n"
	.string "ROCKET BROTHERS!\p"
	.string "Our family's guarding\n"
	.string "the ancient-specimen data!$"

SilphCo_6F_Text_Grunt1Defeat::
	.string "Flame out!$"

SilphCo_6F_Text_Grunt1PostBattle::
	.string "My brothers will avenge me!\p"
	.string "They've sealed the RELIC\n"
	.string "files on higher floors!$"

SilphCo_6F_Text_TaylorIntro::
	.string "That rotten PRESIDENT!\p"
	.string "Serves him right for shipping me\n"
	.string "off to the TIKSI BRANCH!\p"
	.string "That's why TEAM ROCKET came after\n"
	.string "us, I'm sure of it!$"

SilphCo_6F_Text_TaylorDefeat::
	.string "Shoot!$"

SilphCo_6F_Text_TaylorPostBattle::
	.string "TIKSI BRANCH?\n"
	.string "It's in Russian no-man's-land!$"

SilphCo_6F_Text_Grunt2Intro::
	.string "We've revived an entire\n"
	.string "collection of fossil POKéMON!\p"
	.string "You're not stopping us!$"

SilphCo_6F_Text_Grunt2Defeat::
	.string "You traitor!$"

SilphCo_6F_Text_Grunt2PostBattle::
	.string "SILPH revived our fossil\n"
	.string "POKéMON as ordered.\p"
	.string "BOSS wants living proof\n"
	.string "those ancient traits return!$"

SilphCo_6F_Text_FloorSign::
	.string "SILPH CO. HEAD OFFICE\n"
	.string "6F$"

