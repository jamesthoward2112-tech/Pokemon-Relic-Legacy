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
	msgbox SilphCo_11F_Text_ThanksForRescuingUs, MSGBOX_NPC
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
	goto_if_unset FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH, SilphCo_11F_EventScript_GiovanniWaitForJessieJames
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
	.string "GIOVANNI: Face JESSIE and JAMES\n"
	.string "before you challenge me!$"

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

SilphCo_11F_Text_ThanksForSavingMeDearBoy::
	.string "PRESIDENT: Oh, dear boy!\n"
	.string "Thank you for saving SILPH.\p"
	.string "I will never forget you saved us in\n"
	.string "our moment of peril.\p"
	.string "I have to thank you in some way.\p"
	.string "Because I am rich, I can give you\n"
	.string "anything.\p"
	.string "Here, maybe this will do.$"

SilphCo_11F_Text_ThanksForSavingMeDearGirl::
	.string "PRESIDENT: Oh, dear girl!\n"
	.string "Thank you for saving SILPH.\p"
	.string "I will never forget you saved us in\n"
	.string "our moment of peril.\p"
	.string "I have to thank you in some way.\p"
	.string "Because I am rich, I can give you\n"
	.string "anything.\p"
	.string "Here, maybe this will do.$"

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
	.string "SECRETARY: Thank you for rescuing\n"
	.string "all of us.\p"
	.string "We, from the PRESIDENT down,\n"
	.string "are indebted to you.$"

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
	.string "Stop right there!\n"
	.string "Don't you move!$"

SilphCo_11F_Text_Grunt2Defeat::
	.string "Don't…\n"
	.string "Please!$"

SilphCo_11F_Text_Grunt2PostBattle::
	.string "So, you want to see my BOSS?$"

SilphCo_11F_Text_Grunt1Intro::
	.string "Halt! Do you have an appointment\n"
	.string "with my BOSS?$"

SilphCo_11F_Text_Grunt1Defeat::
	.string "Gaah!\n"
	.string "Demolished!$"

SilphCo_11F_Text_Grunt1PostBattle::
	.string "Watch your step…\n"
	.string "My BOSS likes his POKéMON tough!$"

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
	release
	end

SilphCo_11F_Text_PRL_JessiePostBattle::
	.string "JESSIE: Don't celebrate yet!\p"
	.string "TEAM ROCKET will still find\n"
	.string "the ultimate ancient RELIC!$"

SilphCo_11F_Text_PRL_JamesPostBattle::
	.string "JAMES: Every ancient specimen\n"
	.string "leads to another mystery.\p"
	.string "BOSS has a lot to learn!$"

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
	clearflag FLAG_HIDE_PRL_ROCKET_SILPH
	return

SilphCo_11F_OnTransition_PRLRocket::
	checkitem ITEM_SILPH_SCOPE
	goto_if_eq VAR_RESULT, FALSE, SilphCo_11F_PRLRocketTransitionDone
	goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_PRLRocketTransitionAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_SILPH
	goto SilphCo_11F_PRLRocketTransitionAdd
SilphCo_11F_PRLRocketTransitionAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH
	clearflag FLAG_HIDE_PRL_ROCKET_SILPH
SilphCo_11F_PRLRocketTransitionAdd::
	addobject LOCALID_PRL_JESSIE_SILPH
	addobject LOCALID_PRL_JAMES_SILPH
	addobject LOCALID_PRL_MEOWTH_SILPH
SilphCo_11F_PRLRocketTransitionDone::
	return
