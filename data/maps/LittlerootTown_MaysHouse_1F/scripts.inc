LittlerootTown_MaysHouse_1F_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_MaysHouse_1F_OnWarp
	map_script MAP_SCRIPT_ON_LOAD, LittlerootTown_MaysHouse_1F_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_MaysHouse_1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_MaysHouse_1F_OnFrame
	.byte 0

LittlerootTown_MaysHouse_1F_OnWarp:
	map_script_2 VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLCleanNPCs
	map_script_2 VAR_PRL_REGION_STAGE, 3, LittlerootTown_MaysHouse_1F_EventScript_PRLCleanNPCs
	map_script_2 VAR_PRL_REGION_STAGE, 4, LittlerootTown_MaysHouse_1F_EventScript_PRLCleanNPCs
	.2byte 0

LittlerootTown_MaysHouse_1F_EventScript_PRLCleanNPCs::
	@ OnTransition flag edits can be too late for sprites already loaded.
	@ Erase the noninteractive Norman/moving-men objects unconditionally.
	setflag FLAG_HIDE_PLAYERS_HOUSE_DAD
	setflag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_1
	setflag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_2
	hideobjectat 2, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	hideobjectat 3, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	hideobjectat 5, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	removeobject 2
	removeobject 3
	removeobject 5
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_MaysHouse_1F_EventScript_PRLCleanRosieHome
	goto LittlerootTown_MaysHouse_1F_EventScript_PRLCleanBirchHome

LittlerootTown_MaysHouse_1F_EventScript_PRLCleanRosieHome::
	@ Rosie lives in the player's house. The rival family is not here.
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_MOM
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_SIBLING
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY
	hideobjectat 4, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	hideobjectat 6, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	hideobjectat 7, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	removeobject 4
	removeobject 6
	removeobject 7
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MOM
	addobject 1
	end

LittlerootTown_MaysHouse_1F_EventScript_PRLCleanBirchHome::
	@ In the other house, show Birch's family and talkable daughter/son.
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MOM
	hideobjectat 1, MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F
	removeobject 1
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_MOM
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_SIBLING
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY
	addobject 4
	addobject 6
	addobject 7
	end

LittlerootTown_MaysHouse_1F_OnLoad:
	@ PRL players arrived by Birch's truck, not the native unpacking scene.
	goto_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLNoMovingBoxes
	goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_MaysHouse_1F_EventScript_PRLNoMovingBoxes
	goto_if_eq VAR_PRL_REGION_STAGE, 4, LittlerootTown_MaysHouse_1F_EventScript_PRLNoMovingBoxes
	call_if_lt VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_MaysHouse_1F_EventScript_SetMovingBoxes
	call_if_set FLAG_RECEIVED_RUNNING_SHOES, LittlerootTown_MaysHouse_1F_EventScript_CheckShowShoesManual
	end

LittlerootTown_MaysHouse_1F_EventScript_PRLNoMovingBoxes::
	@ Preserve original layout furniture; suppress Emerald's moving-box tiles.
	end

LittlerootTown_MaysHouse_1F_EventScript_SetMovingBoxes::
	setmetatile 5, 4, METATILE_BrendansMaysHouse_MovingBox_Open, TRUE
	setmetatile 5, 2, METATILE_BrendansMaysHouse_MovingBox_Closed, TRUE
	return

LittlerootTown_MaysHouse_1F_EventScript_CheckShowShoesManual::
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_MaysHouse_1F_EventScript_ShowRunningShoesManual
	return

LittlerootTown_MaysHouse_1F_EventScript_ShowRunningShoesManual::
	setmetatile 6, 7, METATILE_BrendansMaysHouse_BookOnTable, TRUE
	return

LittlerootTown_MaysHouse_1F_OnTransition:
	call_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLMakeAuntAvailable
	call_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_MaysHouse_1F_EventScript_PRLMakeAuntAvailable
	call_if_eq VAR_PRL_REGION_STAGE, 4, LittlerootTown_MaysHouse_1F_EventScript_PRLMakeAuntAvailable
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 3, LittlerootTown_MaysHouse_1F_EventScript_MoveMomToDoor
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 5, LittlerootTown_MaysHouse_1F_EventScript_MoveMomToStairs
	call_if_eq VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_MaysHouse_1F_EventScript_MoveMomToTV
	end

LittlerootTown_MaysHouse_1F_EventScript_MoveMomToStairs::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 2, 4
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_MaysHouse_1F_EventScript_MoveMomToTV::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 6, 5
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

LittlerootTown_MaysHouse_1F_EventScript_MoveMomToDoor::
	setobjectxyperm LOCALID_PLAYERS_HOUSE_1F_MOM, 1, 8
	setobjectmovementtype LOCALID_PLAYERS_HOUSE_1F_MOM, MOVEMENT_TYPE_FACE_UP
	return

@ Many of the below scripts have no gender check because they assume youre in the correct house
LittlerootTown_MaysHouse_1F_OnFrame:
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 3, LittlerootTown_MaysHouse_1F_EventScript_EnterHouseMovingIn
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 5, LittlerootTown_MaysHouse_1F_EventScript_GoUpstairsToSetClock
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 6, LittlerootTown_MaysHouse_1F_EventScript_PetalburgGymReport
	map_script_2 VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 1, LittlerootTown_MaysHouse_1F_EventScript_YoureNewNeighbor
	map_script_2 VAR_LITTLEROOT_HOUSES_STATE_MAY, 3, PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV
	.2byte 0

LittlerootTown_MaysHouse_1F_EventScript_GoUpstairsToSetClock::
	lockall
	msgbox PlayersHouse_1F_Text_GoSetTheClock, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, LittlerootTown_MaysHouse_1F_Movement_PushTowardStairs
	applymovement LOCALID_PLAYERS_HOUSE_1F_MOM, LittlerootTown_MaysHouse_1F_Movement_PushTowardStairs
	waitmovement 0
	warp MAP_LITTLEROOT_TOWN_MAYS_HOUSE_2F, 1, 1
	waitstate
	releaseall
	end

LittlerootTown_MaysHouse_1F_Movement_PushTowardStairs:
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_EventScript_EnterHouseMovingIn::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, FEMALE
	goto PlayersHouse_1F_EventScript_EnterHouseMovingIn
	end

LittlerootTown_MaysHouse_1F_EventScript_PetalburgGymReport::
	lockall
	setvar VAR_0x8004, FEMALE
	setvar VAR_0x8005, LOCALID_PLAYERS_HOUSE_1F_MOM
	goto PlayersHouse_1F_EventScript_PetalburgGymReportFemale
	end

LittlerootTown_MaysHouse_1F_EventScript_YoureNewNeighbor::
	lockall
	playse SE_PIN
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_RIVALS_HOUSE_1F_MOM, LittlerootTown_MaysHouse_1F_Movement_RivalMomApproach
	waitmovement 0
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_OhYoureTheNewNeighbor, MSGBOX_DEFAULT
	setflag FLAG_MET_RIVAL_MOM
	setvar VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 2
	releaseall
	end

LittlerootTown_MaysHouse_1F_Movement_RivalMomApproach:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

RivalsHouse_1F_EventScript_RivalMom::
	goto_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLBirchFamily
	goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_MaysHouse_1F_EventScript_PRLBirchFamily
	goto_if_eq VAR_PRL_REGION_STAGE, 4, LittlerootTown_MaysHouse_1F_EventScript_PRLBirchFamily
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RIVAL_ROUTE103, RivalsHouse_1F_EventScript_GoHomeEverySoOften
	goto_if_set FLAG_SYS_POKEMON_GET, RivalsHouse_1F_EventScript_RivalIsOnRoute103
	goto_if_eq VAR_LITTLEROOT_RIVAL_STATE, 3, RivalsHouse_1F_EventScript_RivalTooBusy
	special GetRivalSonDaughterString
	msgbox RivalsHouse_1F_Text_LikeChildLikeFather, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalTooBusy::
	msgbox RivalsHouse_1F_Text_TooBusyToNoticeVisit, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalIsOnRoute103::
	msgbox RivalsHouse_1F_Text_WentOutToRoute103, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_GoHomeEverySoOften::
	msgbox RivalsHouse_1F_Text_ShouldGoHomeEverySoOften, MSGBOX_DEFAULT
	release
	end

RivalsHouse_1F_EventScript_RivalSibling::
	lock
	faceplayer
	special GetPlayerBigGuyGirlString
	msgbox RivalsHouse_1F_Text_DoYouHavePokemon, MSGBOX_DEFAULT
	release
	end

LittlerootTown_MaysHouse_1F_EventScript_GoSeeRoom::
	lockall
	setvar VAR_0x8004, LOCALID_PLAYERS_HOUSE_1F_MOM
	setvar VAR_0x8005, FEMALE
	applymovement VAR_0x8004, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	goto PlayersHouse_1F_EventScript_MomGoSeeRoom
	end

LittlerootTown_MaysHouse_1F_EventScript_MeetRival0::
	lockall
	setvar VAR_0x8008, 0
	goto LittlerootTown_MaysHouse_1F_EventScript_MeetRival
	end

LittlerootTown_MaysHouse_1F_EventScript_MeetRival1::
	lockall
	setvar VAR_0x8008, 1
	goto LittlerootTown_MaysHouse_1F_EventScript_MeetRival
	end

LittlerootTown_MaysHouse_1F_EventScript_MeetRival2::
	lockall
	setvar VAR_0x8008, 2
	goto LittlerootTown_MaysHouse_1F_EventScript_MeetRival
	end

LittlerootTown_MaysHouse_1F_EventScript_MeetRival::
	playse SE_EXIT
	delay 10
	addobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	delay 30
	playse SE_PIN
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, Common_Movement_Delay48
	waitmovement 0
	call_if_ne VAR_0x8008, 1, LittlerootTown_MaysHouse_1F_EventScript_PlayerFaceMay
	playbgm MUS_ENCOUNTER_MAY, TRUE
	call_if_eq VAR_0x8008, 0, LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer0
	call_if_eq VAR_0x8008, 1, LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer1
	call_if_eq VAR_0x8008, 2, LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer2
	msgbox RivalsHouse_1F_Text_MayWhoAreYou, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 0, LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs0
	call_if_eq VAR_0x8008, 1, LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs1
	call_if_eq VAR_0x8008, 2, LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs2
	playse SE_EXIT
	removeobject LOCALID_RIVALS_HOUSE_1F_RIVAL
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_2F_POKE_BALL
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_BEDROOM
	delay 30
	setvar VAR_LITTLEROOT_RIVAL_STATE, 3
	setvar VAR_LITTLEROOT_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

LittlerootTown_MaysHouse_1F_EventScript_PlayerFaceMay::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer0::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer0
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer1::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer1
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_EventScript_MayApproachPlayer2::
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer2
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer0:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer1:
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_Movement_MayApproachPlayer2:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs0::
	applymovement LOCALID_PLAYER, LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit0
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs0
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs1::
	applymovement LOCALID_PLAYER, LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit1
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs1
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_EventScript_MayGoUpstairs2::
	applymovement LOCALID_PLAYER, LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit2
	applymovement LOCALID_RIVALS_HOUSE_1F_RIVAL, LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs2
	waitmovement 0
	return

LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit0:
	delay_16
	delay_8
	walk_in_place_faster_right
	step_end

LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit1:
	delay_16
	delay_8
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

LittlerootTown_MaysHouse_1F_Movement_PlayerWatchMayExit2:
	delay_16
	delay_8
	walk_in_place_faster_left
	step_end

LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs0:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs1:
	walk_in_place_faster_right
	walk_right
	walk_in_place_faster_up
	walk_up
	walk_up
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	step_end

LittlerootTown_MaysHouse_1F_Movement_MayGoUpstairs2:
	walk_in_place_faster_left
	walk_left
	walk_in_place_faster_up
	walk_up
	walk_up
	step_end

RivalsHouse_1F_Text_OhYoureTheNewNeighbor:
	.string "Oh, hello. And you are?\p"
	.string "… … … … … … … … …\n"
	.string "… … … … … … … … …\p"
	.string "Oh, you're {PLAYER}{KUN}, our new next-door\n"
	.string "neighbor! Hi!\p"
	.string "We have a {STR_VAR_1} about the same\n"
	.string "age as you.\p"
	.string "Our {STR_VAR_1} was excited about making\n"
	.string "a new friend.\p"
	.string "Our {STR_VAR_1} is upstairs, I think.$"

RivalsHouse_1F_Text_LikeChildLikeFather:
	.string "Like child, like father.\p"
	.string "My husband is as wild about POKéMON\n"
	.string "as our child.\p"
	.string "If he's not at his LAB, he's likely\n"
	.string "scrabbling about in grassy places.$"

RivalsHouse_1F_Text_TooBusyToNoticeVisit:
	.string "That {RIVAL}!\p"
	.string "I guess our child is too busy with\n"
	.string "POKéMON to notice that you came\l"
	.string "to visit, {PLAYER}{KUN}.$"

RivalsHouse_1F_Text_WentOutToRoute103:
	.string "Oh, {RIVAL} went out to ROUTE 103\n"
	.string "just a little while ago.\p"
	.string "Like father, like child.\n"
	.string "{RIVAL} can't stay quietly at home.$"

RivalsHouse_1F_Text_ShouldGoHomeEverySoOften:
	.string "I think it's wonderful for people to\n"
	.string "travel with POKéMON.\p"
	.string "But you should go home every so often\n"
	.string "to let your mother know you're okay.\p"
	.string "She might not say it, but I'm sure she\n"
	.string "worries about you, {PLAYER}{KUN}.$"

RivalsHouse_1F_Text_MayWhoAreYou:
	.string "Huh?\n"
	.string "Who… Who are you?\p"
	.string "… … … … … … … …\n"
	.string "… … … … … … … …\p"
	.string "Oh, you're {PLAYER}{KUN}.\n"
	.string "So your move was today.\p"
	.string "Um… I'm MAY.\n"
	.string "Glad to meet you!\p"
	.string "I…\n"
	.string "I have this dream of becoming friends\l"
	.string "with POKéMON all over the world.\p"
	.string "I… I heard about you, {PLAYER}{KUN}, from\n"
	.string "my dad, PROF. BIRCH.\p"
	.string "I was hoping that you would be nice,\n"
	.string "{PLAYER}{KUN}, and that we could be friends.\p"
	.string "Oh, this is silly, isn't it?\n"
	.string "I… I've just met you, {PLAYER}{KUN}.\p"
	.string "Eheheh…\p"
	.string "Oh, no! I forgot!\p"
	.string "I was supposed to go help Dad catch\n"
	.string "some wild POKéMON!\p"
	.string "{PLAYER}{KUN}, I'll catch you later!$"

RivalsHouse_1F_Text_BrendanWhoAreYou:
	.string "Hey!\n"
	.string "You…\p"
	.string "Who are you?\p"
	.string "Oh, you're {PLAYER}, aren't you?\n"
	.string "Moved in next door, right?\p"
	.string "I didn't know that you're a girl.\p"
	.string "Dad, PROF. BIRCH, said that our new\n"
	.string "next-door neighbor is a GYM LEADER's\l"
	.string "kid, so I assumed you'd be a guy.\p"
	.string "My name's BRENDAN.\n"
	.string "So, hi, neighbor!\p"
	.string "Huh? Hey, {PLAYER}, don't you have\n"
	.string "a POKéMON?\p"
	.string "Do you want me to go catch you one?\p"
	.string "Aw, darn, I forgot…\p"
	.string "I'm supposed to go help my dad catch\n"
	.string "some wild POKéMON.\p"
	.string "Some other time, okay?$"

RivalsHouse_1F_Text_DoYouHavePokemon:
	.string "Hi, neighbor!\p"
	.string "Do you already have your\n"
	.string "own POKéMON?$"


@ PRL continuity: Hoenn's guardian is the player's AUNT.
@ Kanto MUM remains back in Pallet Town, watching the saved money.
LittlerootTown_MaysHouse_1F_EventScript_PRLMakeAuntAvailable::
	@ On every Hoenn house entry remove the vanilla moving-in props, the
	@ inert father, and the rival sprite which blocked the door tile.
	setflag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_1
	setflag FLAG_HIDE_LITTLEROOT_TOWN_PLAYERS_HOUSE_VIGOROTH_2
	setflag FLAG_HIDE_PLAYERS_HOUSE_DAD
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_MaysHouse_1F_EventScript_PRLRosiesHouse
	@ Opposite house belongs to Birch's family, never a duplicate Rosie.
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MOM
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_MOM
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_SIBLING
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY
	return

LittlerootTown_MaysHouse_1F_EventScript_PRLRosiesHouse::
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MOM
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_MOM
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_SIBLING
	setflag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY
	return

LittlerootTown_MaysHouse_1F_EventScript_PRLAunt::
	goto_if_eq VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntTalk
	goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntTalk
	goto_if_eq VAR_PRL_REGION_STAGE, 4, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntTalk
	goto PlayersHouse_1F_EventScript_Mom

LittlerootTown_MaysHouse_1F_EventScript_PRLAuntTalk::
	lock
	faceplayer
	msgbox LittlerootTown_MaysHouse_1F_Text_PRLAuntHome
	@ Give the Hoenn Town Map only after Birch's starter transaction.
	goto_if_ne VAR_PRL_REGION_STAGE, 2, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntFinished
	checkitem ITEM_TOWN_MAP
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntFinished
	msgbox LittlerootTown_MaysHouse_1F_Text_PRLAuntMap
	giveitem ITEM_TOWN_MAP
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_MaysHouse_1F_EventScript_PRLAuntMapFull
LittlerootTown_MaysHouse_1F_EventScript_PRLAuntFinished::
	release
	end

LittlerootTown_MaysHouse_1F_EventScript_PRLAuntMapFull::
	msgbox gText_TooBadBagIsFull
	release
	end

LittlerootTown_MaysHouse_1F_Text_PRLAuntMap::
	.string "AUNT ROSIE: I've got an\n"
	.string "old Town Map for you!\p"
	.string "It shows all of HOENN.\n"
	.string "Keep it handy, dear!$"

LittlerootTown_MaysHouse_1F_Text_PRLAuntHome::
	.string "AUNT ROSIE: Welcome to\n"
	.string "HOENN, dear!\p"
	.string "{PLAYER}! Make yourself at home.\p"
	.string "Your MUM is keeping your\n"
	.string "KANTO savings safe.\p"
	.string "Uncle FRANK is over in\n"
	.string "PETALBURG CITY.\p"
	.string "I'll be here if you ever\n"
	.string "want to talk, dear!$"


LittlerootTown_MaysHouse_1F_EventScript_PRLBirchFamily::
	msgbox LittlerootTown_MaysHouse_1F_Text_PRLBirchFamily, MSGBOX_NPC
	end

LittlerootTown_MaysHouse_1F_Text_PRLBirchFamily::
	.string "BIRCH's family: The Professor\n"
	.string "is usually at his LAB.\p"
	.string "We're glad to welcome\n"
	.string "another researcher to HOENN!$"

LittlerootTown_MaysHouse_1F_EventScript_PRLRivalChat::
	msgbox LittlerootTown_MaysHouse_1F_Text_PRLRivalChat, MSGBOX_NPC
	end

LittlerootTown_MaysHouse_1F_Text_PRLRivalChat::
	.string "MAY: Dad spends\n"
	.string "so much time researching!\p"
	.string "Are you studying the\n"
	.string "relics with him, too?$"
