PokemonTower_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PokemonTower_1F_OnTransition
	.byte 0

PokemonTower_1F_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_POKEMON_TOWER_1F
	goto_if_defeated TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeSaveSyncWon
	end
PokemonTower_1F_EventScript_PRLSteeveNeeveeSaveSyncWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
	setvar VAR_PRL_RECURRING_TOWER_TRIGGER, 1
	end

PokemonTower_1F_EventScript_Channeler::
	msgbox PokemonTower_1F_Text_SenseSpiritsUpToMischief, MSGBOX_NPC
	end

PokemonTower_1F_EventScript_Woman1::
	msgbox PokemonTower_1F_Text_GrowlitheWhyDidYouDie, MSGBOX_NPC
	end

PokemonTower_1F_EventScript_BaldingMan::
	msgbox PokemonTower_1F_Text_CameToPrayForDepartedClefairy, MSGBOX_NPC
	end

PokemonTower_1F_EventScript_Woman2::
	lock
	faceplayer
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, PokemonTower_1F_EventScript_Woman2MalePlayer
	msgbox PokemonTower_1F_Text_ComeToPayRespectsGirl
	release
	end

PokemonTower_1F_EventScript_Woman2MalePlayer::
	msgbox PokemonTower_1F_Text_ComeToPayRespectsSon
	release
	end

PokemonTower_1F_EventScript_WorkerF::
	msgbox PokemonTower_1F_Text_ErectedInMemoryOfDeadMons, MSGBOX_NPC
	end

PokemonTower_1F_Text_ErectedInMemoryOfDeadMons::
	.string "POKéMON TOWER was erected in the\n"
	.string "memory of POKéMON that died.$"

PokemonTower_1F_Text_ComeToPayRespectsSon::
	.string "Did you come to pay respects?\p"
	.string "Bless your POKéMON-loving heart,\n"
	.string "son.$"

PokemonTower_1F_Text_ComeToPayRespectsGirl::
	.string "Did you come to pay respects?\p"
	.string "Bless your POKéMON-loving heart,\n"
	.string "girl.$"

PokemonTower_1F_Text_CameToPrayForDepartedClefairy::
	.string "I came to pray for my dearly\n"
	.string "departed CLEFAIRY.\p"
	.string "Sniff!\n"
	.string "I'm awash in tears…$"

PokemonTower_1F_Text_GrowlitheWhyDidYouDie::
	.string "My GROWLITHE…\n"
	.string "Why did you die?$"

PokemonTower_1F_Text_SenseSpiritsUpToMischief::
	.string "I am a CHANNELER.\p"
	.string "There are spirits up to mischief.\n"
	.string "I sense them high in the TOWER.$"


@ Scripted, visible, NPC-associated double battle: never launch a trainer without an object context.
PokemonTower_1F_EventScript_PRLSteeveNeeveeTrigger::
	lockall
	goto_if_set FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd
	checkitem ITEM_SILPH_SCOPE
	goto_if_eq VAR_RESULT, FALSE, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_defeated TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerAlreadyWon
	@ Keep actors stationary if the player cannot field a double battle.
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, PokemonTower_1F_EventScript_PRLSteeveNeeveeNeedTwo
	applymovement LOCALID_PRL_STEEVE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Approach
	applymovement LOCALID_PRL_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Approach
	waitmovementall
	@ Passing localId selects the actual sprite instead of the player object.
	trainerbattle_double TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_Text_PRL_SteeveNeeveeIntro, PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat, PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons, localId=LOCALID_PRL_STEEVE_TOWER, continueScript=TRUE
	goto_if_defeated TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon
	releaseall
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeNeedTwo::
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons
	releaseall
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon::
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle
	applymovement LOCALID_PRL_STEEVE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Leave
	applymovement LOCALID_PRL_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Leave
	waitmovementall
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
	setvar VAR_PRL_RECURRING_TOWER_TRIGGER, 1
	removeobject LOCALID_PRL_STEEVE_TOWER
	removeobject LOCALID_PRL_NEEVEE_TOWER
	releaseall
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerAlreadyWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
	setvar VAR_PRL_RECURRING_TOWER_TRIGGER, 1
	removeobject LOCALID_PRL_STEEVE_TOWER
	removeobject LOCALID_PRL_NEEVEE_TOWER
PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd::
	releaseall
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Approach::
	walk_down
	step_end

PokemonTower_1F_EventScript_PRLSteeveNeeveeMovement_Leave::
	walk_up
	step_end

PokemonTower_1F_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVE: Remember us from the ship?\p"
	.string "NEEVEE: Your team keeps growing!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"
