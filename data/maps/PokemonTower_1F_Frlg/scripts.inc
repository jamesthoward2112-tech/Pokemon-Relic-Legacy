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


@ Ordinary sight-based twin trainers. The engine owns the ! and approach.
@ Both share a trainer ID: a single double battle, then permanent postbattle dialogue.
PokemonTower_1F_EventScript_PRLSteeveNeeveeSteevee::
	checkitem ITEM_SILPH_SCOPE
		goto_if_eq VAR_RESULT, FALSE, PokemonTower_1F_EventScript_PRLSteeveNeeveeNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_Text_PRL_SteeveNeeveeIntro, PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat, PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon, localId=LOCALID_PRL_STEEVE_TOWER
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeNeevee::
	checkitem ITEM_SILPH_SCOPE
		goto_if_eq VAR_RESULT, FALSE, PokemonTower_1F_EventScript_PRLSteeveNeeveeNotReady
	trainerbattle_double TRAINER_STEEVE_NEEVEE_TOWER, PokemonTower_1F_Text_PRL_SteeveNeeveeIntro, PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat, PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons, PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon, localId=LOCALID_PRL_NEEVEE_TOWER
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle, MSGBOX_AUTOCLOSE
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeNotReady::
	end

PokemonTower_1F_EventScript_PRLSteeveNeeveeTriggerWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_TOWER
	setvar VAR_PRL_RECURRING_TOWER_TRIGGER, 1
	msgbox PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle
	release
	end

PokemonTower_1F_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVEE: Remember us from the ship?\p"
	.string "NEEVEE: Your team keeps growing!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVEE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

PokemonTower_1F_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVEE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

PokemonTower_1F_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"
