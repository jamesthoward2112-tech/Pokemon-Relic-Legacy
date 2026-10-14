SilphCo_11F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SilphCo_11F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, SilphCo_11F_OnTransition_PRLRocket
	.byte 0

SilphCo_11F_OnLoad::
	setflag FLAG_HIDE_PRL_ROCKET_SILPH
	call_if_unset FLAG_SILPH_11F_DOOR, EventScript_Close11FDoor
	call SilphCo_11F_OnLoad_PRLRocketVisibility
	end

SilphCo_11F_EventScript_President::
	lock
	faceplayer
	goto_if_set FLAG_GOT_MASTER_BALL_FROM_SILPH, SilphCo_11F_EventScript_AlreadyGotMasterBall
	goto_if_eq VAR_MAP_SCENE_SILPH_CO_11F, 0, SilphCo_11F_EventScript_PresidentRocketOccupied
	checkplayergender
	call_if_eq VAR_RESULT, MALE, SilphCo_11F_EventScript_PresidentThanksMale
	call_if_eq VAR_RESULT, FEMALE, SilphCo_11F_EventScript_PresidentThanksFemale
	checkitemspace ITEM_MASTER_BALL
	goto_if_eq VAR_RESULT, FALSE, SilphCo_11F_EventScript_NoRoomForMasterBall
	giveitem_msg SilphCo_11F_Text_ObtainedMasterBallFromPresident, ITEM_MASTER_BALL, 1, MUS_RG_OBTAIN_KEY_ITEM
	msgbox SilphCo_11F_Text_ThatsOurSecretPrototype
	setflag FLAG_GOT_MASTER_BALL_FROM_SILPH
	release
	end

SilphCo_11F_EventScript_PresidentRocketOccupied::
	msgbox SilphCo_11F_Text_PresidentRocketOccupied
	release
	end

SilphCo_11F_EventScript_PresidentThanksMale::
	msgbox SilphCo_11F_Text_ThanksForSavingMeDearBoy
	return

SilphCo_11F_EventScript_PresidentThanksFemale::
	msgbox SilphCo_11F_Text_ThanksForSavingMeDearGirl
	return

SilphCo_11F_EventScript_NoRoomForMasterBall::
	msgbox SilphCo_11F_Text_YouHaveNoRoomForThis
	release
	end

SilphCo_11F_EventScript_AlreadyGotMasterBall::
	msgbox SilphCo_11F_Text_ThatsOurSecretPrototype
	release
	end

SilphCo_11F_EventScript_Secretary::
	goto_if_eq VAR_MAP_SCENE_SILPH_CO_11F, 0, SilphCo_11F_EventScript_SecretaryRocketOccupied
	msgbox SilphCo_11F_Text_ThanksForRescuingUs, MSGBOX_NPC
	end

SilphCo_11F_EventScript_SecretaryRocketOccupied::
	msgbox SilphCo_11F_Text_SecretaryRocketOccupied, MSGBOX_NPC
	end

SilphCo_11F_EventScript_GiovanniTriggerLeft::
	lockall
	setvar VAR_TEMP_1, 0
	goto SilphCo_11F_EventScript_BattleGiovanni
	end

SilphCo_11F_EventScript_GiovanniTriggerRight::
	lockall
	setvar VAR_TEMP_1, 1
	goto SilphCo_11F_EventScript_BattleGiovanni
	end

SilphCo_11F_EventScript_BattleGiovanni::
	@ Trainer defeated state works immediately after battle and for older saves.
	goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_EventScript_GiovanniBattleUnlocked
	goto SilphCo_11F_EventScript_GiovanniWaitForJessieJames

SilphCo_11F_EventScript_GiovanniBattleUnlocked::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH
	textcolor NPC_TEXT_COLOR_MALE
	applymovement LOCALID_SILPH_CO_GIOVANNI, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 25
	msgbox SilphCo_11F_Text_GiovanniIntro
	closemessage
	call_if_eq VAR_TEMP_1, 0, SilphCo_11F_EventScript_GiovanniApproachLeft
	call_if_eq VAR_TEMP_1, 1, SilphCo_11F_EventScript_GiovanniApproachRight
	setvar VAR_LAST_TALKED, LOCALID_SILPH_CO_GIOVANNI
	trainerbattle_no_intro TRAINER_BOSS_GIOVANNI_2, SilphCo_11F_Text_GiovanniDefeat
	msgbox SilphCo_11F_Text_GiovanniPostBattle
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_SILPH_CO_GIOVANNI
	removeobject LOCALID_SILPH_CO_11F_GRUNT1
	removeobject LOCALID_SILPH_CO_11F_GRUNT2
	fadescreen FADE_FROM_BLACK
	setvar VAR_MAP_SCENE_SILPH_CO_11F, 1
	setflag FLAG_HIDE_SAFFRON_ROCKETS
	clearflag FLAG_HIDE_SAFFRON_CIVILIANS
	releaseall
	end

SilphCo_11F_EventScript_GiovanniWaitForJessieJames::
	msgbox SilphCo_11F_Text_PRL_StillGuarded
	releaseall
	end

SilphCo_11F_Text_PRL_StillGuarded::
	.string "GIOVANNI: My agents are\n"
	.string "guarding the western corridor!\p"
	.string "Defeat JESSIE and JAMES if\n"
	.string "you intend to challenge me!$"

SilphCo_11F_EventScript_GiovanniApproachLeft::
	applymovement LOCALID_SILPH_CO_GIOVANNI, SilphCo_11F_Movement_GiovanniApproachLeft
	applymovement LOCALID_PLAYER, SilphCo_11F_Movement_PlayerFaceGiovanni
	waitmovement 0
	return

SilphCo_11F_EventScript_GiovanniApproachRight::
	applymovement LOCALID_SILPH_CO_GIOVANNI, SilphCo_11F_Movement_GiovanniApproachRight
	waitmovement 0
	return

SilphCo_11F_Movement_GiovanniApproachLeft::
	walk_down
	walk_down
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

SilphCo_11F_Movement_GiovanniApproachRight::
	walk_down
	walk_down
	walk_down
	step_end

SilphCo_11F_Movement_PlayerFaceGiovanni::
	delay_16
	delay_16
	delay_16
	delay_8
	delay_4
	walk_in_place_faster_right
	step_end

@ Unused
SilphCo_11F_EventScript_Monitor::
	msgbox SilphCo_11F_Text_MonitorHasMonsOnIt, MSGBOX_SIGN
	end

SilphCo_11F_EventScript_FloorSign::
	msgbox SilphCo_11F_Text_FloorSign, MSGBOX_SIGN
	end

SilphCo_11F_EventScript_Grunt1::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_40, SilphCo_11F_Text_Grunt1Intro, SilphCo_11F_Text_Grunt1Defeat
	msgbox SilphCo_11F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_11F_EventScript_Grunt2::
	trainerbattle_single TRAINER_TEAM_ROCKET_GRUNT_41, SilphCo_11F_Text_Grunt2Intro, SilphCo_11F_Text_Grunt2Defeat
	msgbox SilphCo_11F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_11F_Text_PresidentRocketOccupied::
	.string "PRESIDENT: TEAM ROCKET is still\n"
	.string "holding SILPH hostage!\p"
	.string "Defeat GIOVANNI before I can\n"
	.string "entrust you with our MASTER BALL!$"

SilphCo_11F_Text_SecretaryRocketOccupied::
	.string "SECRETARY: Please stop\n"
	.string "GIOVANNI! The PRESIDENT and\n"
	.string "our research are in danger!$"

SilphCo_11F_Text_ThanksForSavingMeDearBoy::
	.string "PRESIDENT: Dear boy!\n"
	.string "Thank you for saving SILPH!\p"
	.string "ROCKET forced our scientists\n"
	.string "to revive ancient fossils.\p"
	.string "They wanted our MASTER BALL\n"
	.string "for a legendary golden bird.\p"
	.string "We do not know its name.\n"
	.string "Please accept my thanks!$"

SilphCo_11F_Text_ThanksForSavingMeDearGirl::
	.string "PRESIDENT: Dear girl!\n"
	.string "Thank you for saving SILPH!\p"
	.string "ROCKET forced our scientists\n"
	.string "to revive ancient fossils.\p"
	.string "They wanted our MASTER BALL\n"
	.string "for a legendary golden bird.\p"
	.string "We do not know its name.\n"
	.string "Please accept my thanks!$"

SilphCo_11F_Text_ObtainedMasterBallFromPresident::
	.string "{PLAYER} obtained a MASTER BALL\n"
	.string "from the PRESIDENT!$"

SilphCo_11F_Text_ThatsOurSecretPrototype::
	.string "PRESIDENT: This prototype\n"
	.string "is our MASTER BALL.\p"
	.string "ROCKET wanted it for an\n"
	.string "unknown ancient POKéMON.\p"
	.string "Old drawings show a golden\n"
	.string "bird. No one knows its name.\p"
	.string "Keep the ball safe from them!$"

SilphCo_11F_Text_YouHaveNoRoomForThis::
	.string "You have no room for this.$"

SilphCo_11F_Text_ThanksForRescuingUs::
	.string "SECRETARY: Thank you!\p"
	.string "ROCKET seized our research\n"
	.string "on ancient evolution traits.\p"
	.string "Please keep our findings\n"
	.string "from becoming weapons!$"

SilphCo_11F_Text_GiovanniIntro::
	.string "GIOVANNI: You persist, child.\p"
	.string "SILPH has restored my ancient\n"
	.string "specimens to living strength.\p"
	.string "Their records tell of a bird\n"
	.string "whose feathers gleam like gold.\p"
	.string "Perhaps the ultimate RELIC...\p"
	.string "With the MASTER BALL, I will\n"
	.string "seize it if it truly exists!$"

SilphCo_11F_Text_GiovanniDefeat::
	.string "Arrgh!!\n"
	.string "I lost again!?$"

SilphCo_11F_Text_GiovanniPostBattle::
	.string "My fossil army... defeated?\p"
	.string "You have ruined this phase\n"
	.string "of TEAM ROCKET's operation.\p"
	.string "But ancient legends remain.\n"
	.string "I have another plan!$"

SilphCo_11F_Text_Grunt2Intro::
	.string "Stay back! The BOSS is\n"
	.string "claiming our fossil army!\p"
	.string "The MASTER BALL is for\n"
	.string "something even older!$"

SilphCo_11F_Text_Grunt2Defeat::
	.string "Don't…\n"
	.string "Please!$"

SilphCo_11F_Text_Grunt2PostBattle::
	.string "You defeated me, but\n"
	.string "BOSS already has the files!\p"
	.string "The MASTER BALL is for an\n"
	.string "unknown ancient POKéMON!$"

SilphCo_11F_Text_Grunt1Intro::
	.string "Halt! Do you have an\n"
	.string "appointment with the BOSS?\p"
	.string "He is reviewing the\n"
	.string "RELIC research with SILPH!$"

SilphCo_11F_Text_Grunt1Defeat::
	.string "Gaah!\n"
	.string "Demolished!$"

SilphCo_11F_Text_Grunt1PostBattle::
	.string "The fossil army is real.\p"
	.string "SILPH revived those species.\n"
	.string "BOSS wants the final prize!$"

SilphCo_11F_Text_MonitorHasMonsOnIt::
	.string "The screen lists revived\n"
	.string "FOSSIL POKéMON specimens.\p"
	.string "A file reads: RELIC SIGNAL -\n"
	.string "ORIGIN UNKNOWN.$"

SilphCo_11F_Text_FloorSign::
	.string "SILPH CO. HEAD OFFICE\n"
	.string "11F$"



@ Jessie and James behave as ordinary trainers, using native ! / walk-up.
@ One shared opponent ID makes the first alignment a single double battle.
@ Neither character is removed: each has separate post-battle dialogue.
SilphCo_11F_EventScript_PRL_Jessie::
	trainerbattle_double TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_Text_PRL_RocketIntro, SilphCo_11F_Text_PRL_RocketDefeat, SilphCo_11F_Text_PRL_RocketNotEnoughMons, SilphCo_11F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JESSIE_SILPH
	msgbox SilphCo_11F_Text_PRL_JessiePostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_11F_EventScript_PRL_James::
	trainerbattle_double TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_Text_PRL_RocketIntro, SilphCo_11F_Text_PRL_RocketDefeat, SilphCo_11F_Text_PRL_RocketNotEnoughMons, SilphCo_11F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JAMES_SILPH
	msgbox SilphCo_11F_Text_PRL_JamesPostBattle, MSGBOX_AUTOCLOSE
	end

SilphCo_11F_EventScript_PRL_Meowth::
	lock
	faceplayer
	goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_EventScript_PRL_MeowthAfter
	msgbox SilphCo_11F_Text_PRL_MeowthBefore
	release
	end
SilphCo_11F_EventScript_PRL_MeowthAfter::
	msgbox SilphCo_11F_Text_PRL_MeowthAfter
	release
	end

SilphCo_11F_EventScript_PRL_RocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH
	msgbox SilphCo_11F_Text_PRL_RocketPostBattle
	closemessage
	@ Running away on foot after this encounter; no further balloon departures.
	applymovement LOCALID_PRL_JESSIE_SILPH, SilphCo_11F_Movement_PRL_JessieRetreat
	applymovement LOCALID_PRL_JAMES_SILPH, SilphCo_11F_Movement_PRL_JamesRetreat
	applymovement LOCALID_PRL_MEOWTH_SILPH, SilphCo_11F_Movement_PRL_MeowthRetreat
	waitmovement LOCALID_PRL_JESSIE_SILPH
	waitmovement LOCALID_PRL_JAMES_SILPH
	waitmovement LOCALID_PRL_MEOWTH_SILPH
	setflag FLAG_HIDE_PRL_ROCKET_SILPH
	removeobject LOCALID_PRL_JESSIE_SILPH
	removeobject LOCALID_PRL_JAMES_SILPH
	removeobject LOCALID_PRL_MEOWTH_SILPH
	release
	end

SilphCo_11F_Movement_PRL_JessieRetreat:
	walk_fast_right
	walk_fast_right
	step_end

SilphCo_11F_Movement_PRL_JamesRetreat:
	walk_fast_right
	walk_fast_right
	step_end

SilphCo_11F_Movement_PRL_MeowthRetreat:
	walk_fast_right
	walk_fast_right
	step_end

SilphCo_11F_Text_PRL_JessiePostBattle::
	.string "JESSIE: The BOSS spent so\n"
	.string "much on that fossil army!\p"
	.string "And now you've ruined\n"
	.string "his MASTER BALL plans!$"

SilphCo_11F_Text_PRL_JamesPostBattle::
	.string "JAMES: Those sketches might\n"
	.string "only be ancient legends.\p"
	.string "But the signal from the\n"
	.string "fossils was definitely real!$"

SilphCo_11F_Text_PRL_MeowthBefore::
	.string "MEOWTH: Mrowr! Not another kid!$"

SilphCo_11F_Text_PRL_MeowthAfter::
	.string "MEOWTH: They lost AGAIN?!$"

SilphCo_11F_Text_PRL_RocketIntro::
	.string "JESSIE: After MT. MOON and\n"
	.string "CELADON, you STILL follow us?!\p"
	.string "JAMES: The golden-bird files\n"
	.string "belong to TEAM ROCKET!$"

SilphCo_11F_Text_PRL_RocketDefeat::
	.string "JAMES: The MASTER BALL!\p"
	.string "JESSIE: We were so close to\n"
	.string "seizing the ancient prize!$"

SilphCo_11F_Text_PRL_RocketNotEnoughMons::
	.string "JESSIE: Bring at least two POKéMON\n"
	.string "if you want to stop TEAM ROCKET!$"

SilphCo_11F_Text_PRL_RocketPostBattle::
	.string "JAMES: A golden bird...\p"
	.string "JESSIE: Just an old rumour!\n"
	.string "The BOSS will be furious.$"


@ Visibility is set before spawning map objects and refreshed on transition.
@ Neither function starts a battle or changes the native map's warp tiles.
SilphCo_11F_OnLoad_PRLRocketVisibility::
	checkitem ITEM_SILPH_SCOPE
	goto_if_eq VAR_RESULT, FALSE, SilphCo_11F_PRLRocketVisibilityDone
	goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_PRLRocketAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_SILPH
SilphCo_11F_PRLRocketVisibilityDone::
	return
SilphCo_11F_PRLRocketAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH
	setflag FLAG_HIDE_PRL_ROCKET_SILPH
	return

SilphCo_11F_OnTransition_PRLRocket::
	checkitem ITEM_SILPH_SCOPE
	goto_if_eq VAR_RESULT, FALSE, SilphCo_11F_PRLRocketTransitionDone
	goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_PRLRocketTransitionAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_SILPH
	goto SilphCo_11F_PRLRocketTransitionAdd
SilphCo_11F_PRLRocketTransitionAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH
	setflag FLAG_HIDE_PRL_ROCKET_SILPH
	goto SilphCo_11F_PRLRocketTransitionDone
SilphCo_11F_PRLRocketTransitionAdd::
	addobject LOCALID_PRL_JESSIE_SILPH
	addobject LOCALID_PRL_JAMES_SILPH
	addobject LOCALID_PRL_MEOWTH_SILPH
SilphCo_11F_PRLRocketTransitionDone::
	return
