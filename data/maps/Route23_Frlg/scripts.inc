Route23_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route23_OnTransition
	.byte 0

Route23_OnTransition::
	clearflag FLAG_HIDE_VICTORY_ROAD_3F_BOULDER
	setflag FLAG_HIDE_VICTORY_ROAD_2F_BOULDER
	setvar VAR_MAP_SCENE_VICTORY_ROAD_1F, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER1, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_2F_BOULDER2, 0
	setvar VAR_MAP_SCENE_VICTORY_ROAD_3F, 0
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeSaveSyncWon
	end
Route23_EventScript_PRLSteeveNeeveeSaveSyncWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	end

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


@ Scripted, visible, NPC-associated double battle: never launch a trainer without an object context.
Route23_EventScript_PRLSteeveNeeveeTrigger::
	lockall
	goto_if_set FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_unset FLAG_BADGE08_GET, Route23_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeTriggerAlreadyWon
	@ Keep actors stationary if the player cannot field a double battle.
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, Route23_EventScript_PRLSteeveNeeveeNeedTwo
	applymovement LOCALID_PRL_STEEVE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeMovement_Approach
	applymovement LOCALID_PRL_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeMovement_Approach
	waitmovementall
	@ Passing localId selects the actual sprite instead of the player object.
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_Text_PRL_SteeveNeeveeIntro, Route23_Text_PRL_SteeveNeeveeDefeat, Route23_Text_PRL_SteeveNeeveeNotEnoughMons, localId=LOCALID_PRL_STEEVE_ROUTE23, continueScript=TRUE
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeTriggerWon
	releaseall
	end

Route23_EventScript_PRLSteeveNeeveeNeedTwo::
	msgbox Route23_Text_PRL_SteeveNeeveeNotEnoughMons
	releaseall
	end

Route23_EventScript_PRLSteeveNeeveeTriggerWon::
	msgbox Route23_Text_PRL_SteeveNeeveePostBattle
	applymovement LOCALID_PRL_STEEVE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeMovement_Leave
	applymovement LOCALID_PRL_NEEVEE_ROUTE23, Route23_EventScript_PRLSteeveNeeveeMovement_Leave
	waitmovementall
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	removeobject LOCALID_PRL_STEEVE_ROUTE23
	removeobject LOCALID_PRL_NEEVEE_ROUTE23
	releaseall
	end

Route23_EventScript_PRLSteeveNeeveeTriggerAlreadyWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE23
	setvar VAR_MAP_SCENE_ROUTE23, 9
	removeobject LOCALID_PRL_STEEVE_ROUTE23
	removeobject LOCALID_PRL_NEEVEE_ROUTE23
Route23_EventScript_PRLSteeveNeeveeTriggerEnd::
	releaseall
	end

Route23_EventScript_PRLSteeveNeeveeMovement_Approach::
	walk_down
	step_end

Route23_EventScript_PRLSteeveNeeveeMovement_Leave::
	walk_up
	step_end

Route23_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVE: From ROUTE 3 to the League!\p"
	.string "NEEVEE: Let's give our best battle!$"

Route23_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

Route23_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

Route23_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"
