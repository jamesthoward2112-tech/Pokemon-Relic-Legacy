PokemonTower_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PokemonTower_1F_OnTransition
	.byte 0

PokemonTower_1F_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_POKEMON_TOWER_1F
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


PokemonTower_1F_EventScript_PRLSteeveNeeveeTrigger::
	lockall
	goto_if_set FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_unset FLAG_GOT_SILPH_SCOPE, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_defeated TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerAlreadyWon
	trainerbattle_double TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_Text_PRL_SteeveNeeveeIntro, PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat, PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons
	goto_if_defeated TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon
	releaseall
	end
PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle
PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerAlreadyWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerEnd::
	releaseall
	end

PokemonTower_1F_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVE: Our EEVEE are ready!\p"
	.string "NEEVEE: Let's battle together!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"
