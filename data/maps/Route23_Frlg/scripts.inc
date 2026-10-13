Route23_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route23_OnTransition
	.byte 0

Route23_OnTransition::
	setflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	setflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	clearflag FLAG_HIDE_VICTORY_ROAD_3F_BOULDER
	setflag FLAG_HIDE_VICTORY_ROAD_2F_BOULDER
	setvar VAR_MAP_SCENE_VICTORY_ROAD_1F, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER1, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER2, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_3F, 0
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeSaveSyncWon
Route23_EventScript_PRLHeistVisibility::
	goto_if_unset FLAG_PRL_BADGE_HEIST_STARTED, Route23_EventScript_PRLHeistVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_ALL_RECOVERED, Route23_EventScript_PRLHeistVisibilityDone
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, Route23_EventScript_PRLHeistVisibilityDone
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	addobject LOCALID_PRL_HEIST_JESSIE
	addobject LOCALID_PRL_HEIST_JAMES
	addobject LOCALID_PRL_HEIST_MEOWTH
Route23_EventScript_PRLHeistVisibilityDone::
	end
Route23_EventScript_PRLSteeveNeeveeSaveSyncWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	goto Route23_EventScript_PRLHeistVisibility

Route23_EventScript_CascadeBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 2
	bufferstdstring STR_VAR_1, STDSTRING_CASCADE_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_ThunderBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 3
	bufferstdstring STR_VAR_1, STDSTRING_THUNDER_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_RainbowBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 4
	bufferstdstring STR_VAR_1, STDSTRING_RAINBOW_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_SoulBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 5
	bufferstdstring STR_VAR_1, STDSTRING_SOUL_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_MarshBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 6
	bufferstdstring STR_VAR_1, STDSTRING_MARSH_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_VolcanoBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 7
	bufferstdstring STR_VAR_1, STDSTRING_VOLCANO_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_EarthBadgeGuard::
	lock
	faceplayer
	setvar VAR_TEMP_1, 8
	bufferstdstring STR_VAR_1, STDSTRING_EARTH_BADGE
	goto Route23_EventScript_BadgeGuard
	end

Route23_EventScript_CascadeBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 2
	setvar VAR_0x8009, LOCALID_CASCADE_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_CASCADE_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_ThunderBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 3
	setvar VAR_0x8009, LOCALID_THUNDER_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_THUNDER_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_RainbowBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 4
	setvar VAR_0x8009, LOCALID_RAINBOW_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_RAINBOW_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_SoulBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 5
	setvar VAR_0x8009, LOCALID_SOUL_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_SOUL_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_MarshBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 6
	setvar VAR_0x8009, LOCALID_MARSH_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_MARSH_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_VolcanoBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 7
	setvar VAR_0x8009, LOCALID_VOLCANO_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_VOLCANO_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_EarthBadgeGuardTrigger::
	lockall
	setvar VAR_TEMP_1, 8
	setvar VAR_0x8009, LOCALID_EARTH_BADGE_GUARD
	bufferstdstring STR_VAR_1, STDSTRING_EARTH_BADGE
	goto Route23_EventScript_BadgeGuardTrigger
	end

Route23_EventScript_VictoryRoadGateSign::
	msgbox Route23_Text_VictoryRoadGateSign, MSGBOX_SIGN
	end

Route23_Text_VictoryRoadGateSign::
	.string "VICTORY ROAD GATE -\n"
	.string "POKéMON LEAGUE$"


@ Ordinary sight-based twin trainers. The engine owns the ! and approach.
@ Both share a trainer ID: a single double battle, then permanent postbattle dialogue.
Route23_EventScript_PRLSteeveNeeveeSteevee::
	goto_if_unset FLAG_BADGE08_GET, Route23_EventScript_PRLSteeveNeeveeNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, Route23_EventScript_PRLSteeveNeeveeTriggerWon, localId=LOCALID_PRL_STEEVE_ROUTE23
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	end

Route23_EventScript_PRLSteeveNeeveeNeevee::
	goto_if_unset FLAG_BADGE08_GET, Route23_EventScript_PRLSteeveNeeveeNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, Route23_EventScript_PRLSteeveNeeveeTriggerWon, localId=LOCALID_PRL_NEEVEE_ROUTE23
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	end

Route23_EventScript_PRLSteeveNeeveeNotReady::
	end

Route23_EventScript_PRLSteeveNeeveeTriggerWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle
	release
	end

Route23_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVEE: From ROUTE 3 to the League!\p"
	.string "NEEVEE: Let's give our best battle!$"

Route23_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVEE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

Route23_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVEE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

Route23_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"


@ One-time outdoor theft scene. The real badge flags stay set; only the
@ separate recovery flags govern the quest and the Route 23 guards.
Route23_EventScript_PRLGreatBadgeHeist::
	lockall
	goto_if_set FLAG_PRL_BADGE_HEIST_STARTED, Route23_EventScript_PRLGreatBadgeHeistDone
	goto_if_unset FLAG_BADGE01_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE02_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE03_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE04_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE05_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE06_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE07_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	goto_if_unset FLAG_BADGE08_GET, Route23_EventScript_PRLGreatBadgeHeistNotReady
	setvar VAR_TEMP_0, 1
	setflag FLAG_PRL_BADGE_HEIST_STARTED
	cleartrainerflag TRAINER_JESSIE_JAMES_VICTORY_ROAD
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	addobject LOCALID_PRL_HEIST_JESSIE
	addobject LOCALID_PRL_HEIST_JAMES
	addobject LOCALID_PRL_HEIST_MEOWTH
	setobjectxy LOCALID_PRL_HEIST_JESSIE, 5, 149
	setobjectxy LOCALID_PRL_HEIST_JAMES, 11, 149
	setobjectxy LOCALID_PRL_HEIST_MEOWTH, 8, 149
	applymovement LOCALID_PRL_HEIST_JESSIE, Route23_Movement_PRLJessieApproach
	applymovement LOCALID_PRL_HEIST_JAMES, Route23_Movement_PRLJamesApproach
	applymovement LOCALID_PRL_HEIST_MEOWTH, Route23_Movement_PRLMeowthApproach
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_PRL_HEIST_JESSIE, Route23_Movement_PRLHeistExclamation
	applymovement LOCALID_PRL_HEIST_JAMES, Route23_Movement_PRLHeistExclamation
	waitmovement 0
	msgbox Route23_Text_PRLGreatBadgeHeist
	applymovement LOCALID_PRL_HEIST_JESSIE, Route23_Movement_PRLJessieBoard
	applymovement LOCALID_PRL_HEIST_JAMES, Route23_Movement_PRLJamesBoard
	applymovement LOCALID_PRL_HEIST_MEOWTH, Route23_Movement_PRLMeowthBoard
	waitmovement 0
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	addobject LOCALID_PRL_HEIST_BALLOON
	setobjectxy LOCALID_PRL_HEIST_BALLOON, 8, 151
	playse SE_WARP_IN
	delay 24
	removeobject LOCALID_PRL_HEIST_JESSIE
	removeobject LOCALID_PRL_HEIST_JAMES
	removeobject LOCALID_PRL_HEIST_MEOWTH
	msgbox Route23_Text_PRLGreatBadgeHeistEscape
	applymovement LOCALID_PRL_HEIST_BALLOON, Route23_Movement_PRLBalloonTakeoff
	waitmovement 0
	setflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	setflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	removeobject LOCALID_PRL_HEIST_BALLOON
Route23_EventScript_PRLGreatBadgeHeistDone::
	releaseall
	end
Route23_EventScript_PRLGreatBadgeHeistNotReady::
	releaseall
	end

Route23_EventScript_PRLFinalRocketBattle::
	goto_if_unset FLAG_PRL_BADGE_HEIST_ALL_RECOVERED, Route23_EventScript_PRLFinalRocketNotReady
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, Route23_EventScript_PRLFinalRocketGone
	trainerbattle_double TRAINER_JESSIE_JAMES_VICTORY_ROAD, Route23_Text_PRLFinalRocketIntro, Route23_Text_PRLFinalRocketDefeat, Route23_Text_PRLFinalRocketNotEnoughMons, Route23_EventScript_PRLFinalRocketWon, localId=LOCALID_PRL_HEIST_JESSIE
	goto Route23_EventScript_PRLFinalRocketGone

Route23_EventScript_PRLFinalRocketBattleJames::
	goto_if_unset FLAG_PRL_BADGE_HEIST_ALL_RECOVERED, Route23_EventScript_PRLFinalRocketNotReady
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, Route23_EventScript_PRLFinalRocketGone
	trainerbattle_double TRAINER_JESSIE_JAMES_VICTORY_ROAD, Route23_Text_PRLFinalRocketIntro, Route23_Text_PRLFinalRocketDefeat, Route23_Text_PRLFinalRocketNotEnoughMons, Route23_EventScript_PRLFinalRocketWon, localId=LOCALID_PRL_HEIST_JAMES
	goto Route23_EventScript_PRLFinalRocketGone

Route23_EventScript_PRLFinalRocketMeowth::
	lock
	faceplayer
	goto_if_set FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED, Route23_EventScript_PRLFinalRocketGone
	msgbox Route23_Text_PRLFinalRocketMeowth, MSGBOX_DEFAULT
	release
	end

Route23_EventScript_PRLFinalRocketNotReady::
	end

Route23_EventScript_PRLFinalRocketGone::
	msgbox Route23_Text_PRLFinalRocketGone, MSGBOX_AUTOCLOSE
	end

Route23_EventScript_PRLFinalRocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_VICTORY_ROAD
	setflag FLAG_PRL_BADGE_HEIST_FINAL_DEFEATED
	setvar VAR_PRL_VICTORY_ROAD_ROCKET_GATE, 1
	msgbox Route23_Text_PRLFinalRocketAfterBattle
	applymovement LOCALID_PRL_HEIST_JESSIE, Route23_Movement_PRLJessieFinalBoard
	applymovement LOCALID_PRL_HEIST_JAMES, Route23_Movement_PRLJamesFinalBoard
	applymovement LOCALID_PRL_HEIST_MEOWTH, Route23_Movement_PRLMeowthFinalBoard
	waitmovement 0
	clearflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	addobject LOCALID_PRL_HEIST_BALLOON
	playse SE_WARP_IN
	delay 24
	removeobject LOCALID_PRL_HEIST_JESSIE
	removeobject LOCALID_PRL_HEIST_JAMES
	removeobject LOCALID_PRL_HEIST_MEOWTH
	msgbox Route23_Text_PRLFinalRocketBlastOff
	applymovement LOCALID_PRL_HEIST_BALLOON, Route23_Movement_PRLBalloonTakeoff
	waitmovement 0
	setflag FLAG_HIDE_PRL_BADGE_HEIST_TRIO
	setflag FLAG_HIDE_PRL_BADGE_HEIST_BALLOON
	removeobject LOCALID_PRL_HEIST_BALLOON
	release
	end

Route23_Movement_PRLJessieApproach:
	walk_right
	walk_right
	face_down
	step_end

Route23_Movement_PRLJamesApproach:
	walk_left
	walk_left
	face_down
	step_end

Route23_Movement_PRLMeowthApproach:
	walk_down
	face_down
	step_end

Route23_Movement_PRLHeistExclamation:
	emote_exclamation_mark
	step_end

Route23_Movement_PRLJessieBoard:
	walk_up
	walk_up
	step_end

Route23_Movement_PRLJamesBoard:
	walk_up
	walk_up
	step_end

Route23_Movement_PRLMeowthBoard:
	walk_up
	walk_up
	step_end

Route23_Movement_PRLJessieFinalBoard:
	walk_right
	walk_up
	step_end

Route23_Movement_PRLJamesFinalBoard:
	walk_left
	walk_up
	step_end

Route23_Movement_PRLMeowthFinalBoard:
	walk_up
	walk_up
	step_end

Route23_Movement_PRLBalloonTakeoff:
	walk_slow_up
	walk_slow_up
	walk_slow_up
	walk_slow_up
	walk_slow_up
	step_end

Route23_Text_PRLGreatBadgeHeist::
	.string "JESSIE: TEAM ROCKET won't let you\n"
	.string "reach the POKéMON LEAGUE!\p"
	.string "JAMES: We'll be taking all eight\n"
	.string "of those shiny GYM BADGES!\p"
	.string "MEOWTH: A perfect crime!$"

Route23_Text_PRLGreatBadgeHeistEscape::
	.string "JESSIE: Want them back, twerp?\p"
	.string "JAMES: Then defeat all eight GYM LEADERS\n"
	.string "again!\p"
	.string "MEOWTH: Up, up, and away!$"

Route23_Text_PRLFinalRocketIntro::
	.string "JESSIE: You actually reclaimed\n"
	.string "every BADGE?!\p"
	.string "JAMES: Then this is TEAM ROCKET's\n"
	.string "final KANTO performance!\p"
	.string "MEOWTH: Nobody reaches the League!$"

Route23_Text_PRLFinalRocketDefeat::
	.string "JESSIE: Our grand finale!\p"
	.string "JAMES: Brought down by the twerp!$"

Route23_Text_PRLFinalRocketNotEnoughMons::
	.string "JAMES: Bring two POKéMON for our\n"
	.string "last double battle!$"

Route23_Text_PRLFinalRocketMeowth::
	.string "MEOWTH: We ain't lettin' you into\n"
	.string "VICTORY ROAD!$"

Route23_Text_PRLFinalRocketGone::
	.string "TEAM ROCKET's KANTO story is over.$"

Route23_Text_PRLFinalRocketAfterBattle::
	.string "JESSIE: You haven't seen the last\n"
	.string "of our ambition!\p"
	.string "JAMES: Balloon, don't fail us now!$"

Route23_Text_PRLFinalRocketBlastOff::
	.string "MEOWTH: TEAM ROCKET's blasting off\n"
	.string "from KANTO!$"
