Route3_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route3_EventScript_PRLSteeveNeeveeSaveSync
	.byte 0

Route3_EventScript_PRLSteeveNeeveeSaveSync::
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeSaveSyncWon
	return
Route3_EventScript_PRLSteeveNeeveeSaveSyncWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE3
	setvar VAR_PRL_RECURRING_ROUTE3_TRIGGER, 1
	return

Route3_EventScript_Youngster::
	msgbox Route3_Text_TunnelFromCeruleanTiring, MSGBOX_NPC
	end

Route3_EventScript_RouteSign::
	msgbox Route3_Text_RouteSign, MSGBOX_SIGN
	end

Route3_Text_TunnelFromCeruleanTiring::
	.string "Whew… I better take a rest…\n"
	.string "Groan…\p"
	.string "That tunnel from CERULEAN takes a\n"
	.string "lot out of you!$"

Route3_Text_ColtonIntro::
	.string "Hey!\n"
	.string "I saw you in VIRIDIAN FOREST!$"

Route3_Text_ColtonDefeat::
	.string "You beat me again!$"

Route3_Text_ColtonPostBattle::
	.string "There are other kinds of POKéMON\n"
	.string "than the ones you find in forests.$"

Route3_Text_BenIntro::
	.string "Hi!\n"
	.string "I like shorts!\p"
	.string "They're delightfully comfy and\n"
	.string "easy to wear!$"

Route3_Text_BenDefeat::
	.string "I don't believe it!$"

Route3_Text_BenPostBattle::
	.string "Are you using a POKéMON CENTER's\n"
	.string "PC for storing your POKéMON?\p"
	.string "Each BOX can hold up to\n"
	.string "30 POKéMON.$"

Route3_Text_JaniceIntro::
	.string "Excuse me!\n"
	.string "You looked at me, didn't you?$"

Route3_Text_JaniceDefeat::
	.string "You're mean!$"

Route3_Text_JanicePostBattle::
	.string "You shouldn't be staring if you\n"
	.string "don't want to battle!$"

Route3_Text_GregIntro::
	.string "Are you a TRAINER?\n"
	.string "Let's get with it right away!$"

Route3_Text_GregDefeat::
	.string "If I had new POKéMON, I would've\n"
	.string "won!$"

Route3_Text_GregPostBattle::
	.string "If a POKéMON BOX on the PC gets\n"
	.string "full, just switch to another BOX.$"

Route3_Text_SallyIntro::
	.string "That look you gave me…\n"
	.string "It's so intriguing!$"

Route3_Text_SallyDefeat::
	.string "Be nice!$"

Route3_Text_SallyPostBattle::
	.string "You can avoid battles by not\n"
	.string "letting TRAINERS see you.$"

Route3_Text_CalvinIntro::
	.string "Hey! You're not wearing shorts!\n"
	.string "What's wrong with you?$"

Route3_Text_CalvinDefeat::
	.string "Lost!\n"
	.string "Lost! Lost!$"

Route3_Text_CalvinPostBattle::
	.string "I always wear shorts, even in\n"
	.string "winter. That's my policy.$"

Route3_Text_JamesIntro::
	.string "I'll battle you with the POKéMON\n"
	.string "I just caught.$"

Route3_Text_JamesDefeat::
	.string "Done like dinner!$"

Route3_Text_JamesPostBattle::
	.string "Trained POKéMON are stronger than\n"
	.string "the wild ones.$"

Route3_Text_RobinIntro::
	.string "Eek!\n"
	.string "Did you touch me?$"

Route3_Text_RobinDefeat::
	.string "That's it?$"

Route3_Text_RobinPostBattle::
	.string "ROUTE 4 is at the foot of\n"
	.string "MT. MOON.$"

Route3_Text_RouteSign::
	.string "ROUTE 3\n"
	.string "MT. MOON AHEAD$"

Route3_EventScript_PRLCharmanderAide::
	lock
	faceplayer
	goto_if_set FLAG_PRL_GOT_CHARMANDER_GIFT, Route3_EventScript_PRLCharmanderAide_Repeat
	msgbox Route3_EventScript_PRLCharmanderAide_Text_Intro
	setvar VAR_TEMP_1, SPECIES_CHARMANDER
	givemon SPECIES_CHARMANDER, 10
	goto_if_eq VAR_RESULT, 2, Common_EventScript_NoMoreRoomForPokemon
	msgbox Route3_EventScript_PRLCharmanderAide_Text_Received
	setflag FLAG_PRL_GOT_CHARMANDER_GIFT
	removeobject LOCALID_PRL_CHARMANDER
	msgbox Route3_EventScript_PRLCharmanderAide_Text_After
	giveitem ITEM_SHARP_BEAK
	release
	end

Route3_EventScript_PRLCharmanderAide_Repeat::
	msgbox Route3_EventScript_PRLCharmanderAide_Text_Repeat
	release
	end

Route3_EventScript_PRLCharmanderAide_Pokemon::
	msgbox Route3_EventScript_PRLCharmanderAide_Text_Pokemon, MSGBOX_NPC
	end

Route3_EventScript_PRLCharmanderAide_Text_Received::
	.string "{PLAYER} received CHARMANDER\n"
	.string "from PROF. OAK's AIDE!$"

Route3_EventScript_PRLCharmanderAide_Text_Intro::
	.string "PROF. OAK asked me to entrust this\n"
	.string "POKéMON to a promising TRAINER.\p"
	.string "It may look ordinary now… but OAK\n"
	.string "believes something ancient sleeps\l"
	.string "within it.$"

Route3_EventScript_PRLCharmanderAide_Text_After::
	.string "Take this SHARP BEAK, too.\p"
	.string "OAK thinks certain FIRE-type\n"
	.string "POKéMON may respond to it as they\l"
	.string "mature. Take good care of CHARMANDER.$"

Route3_EventScript_PRLCharmanderAide_Text_Repeat::
	.string "Keep training CHARMANDER. OAK's relic\n"
	.string "theory may take time to prove.$"

Route3_EventScript_PRLCharmanderAide_Text_Pokemon::
	.string "CHARMANDER looks ready to travel.$"

@ Scripted, visible, NPC-associated double battle: never launch a trainer without an object context.
Route3_EventScript_PRLSteeveNeeveeTrigger::
	lockall
	goto_if_set FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_unset FLAG_BADGE01_GET, Route3_EventScript_PRLSteeveNeeveeTriggerEnd
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeTriggerAlreadyWon
	@ Keep actors stationary if the player cannot field a double battle.
	special HasEnoughMonsForDoubleBattle
	goto_if_ne VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, Route3_EventScript_PRLSteeveNeeveeNeedTwo
	applymovement LOCALID_PRL_STEEVE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeMovement_Approach
	applymovement LOCALID_PRL_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeMovement_Approach
	waitmovementall
	@ Passing localId selects the actual sprite instead of the player object.
	trainerbattle_double TRAINER_STEEVE_NEEVEE_ROUTE3, Route3_Text_PRL_SteeveNeeveeIntro, Route3_Text_PRL_SteeveNeeveeDefeat, Route3_Text_PRL_SteeveNeeveeNotEnoughMons, localId=LOCALID_PRL_STEEVE_ROUTE3, continueScript=TRUE
	goto_if_defeated TRAINER_STEEVE_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeTriggerWon
	releaseall
	end

Route3_EventScript_PRLSteeveNeeveeNeedTwo::
	msgbox Route3_Text_PRL_SteeveNeeveeNotEnoughMons
	releaseall
	end

Route3_EventScript_PRLSteeveNeeveeTriggerWon::
	msgbox Route3_Text_PRL_SteeveNeeveePostBattle
	applymovement LOCALID_PRL_STEEVE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeMovement_Leave
	applymovement LOCALID_PRL_NEEVEE_ROUTE3, Route3_EventScript_PRLSteeveNeeveeMovement_Leave
	waitmovementall
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE3
	setvar VAR_PRL_RECURRING_ROUTE3_TRIGGER, 1
	removeobject LOCALID_PRL_STEEVE_ROUTE3
	removeobject LOCALID_PRL_NEEVEE_ROUTE3
	releaseall
	end

Route3_EventScript_PRLSteeveNeeveeTriggerAlreadyWon::
	setflag FLAG_PRL_RECURRING_STEEVE_NEEVEE_ROUTE3
	setvar VAR_PRL_RECURRING_ROUTE3_TRIGGER, 1
	removeobject LOCALID_PRL_STEEVE_ROUTE3
	removeobject LOCALID_PRL_NEEVEE_ROUTE3
Route3_EventScript_PRLSteeveNeeveeTriggerEnd::
	releaseall
	end

Route3_EventScript_PRLSteeveNeeveeMovement_Approach::
	walk_down
	step_end

Route3_EventScript_PRLSteeveNeeveeMovement_Leave::
	walk_up
	step_end

Route3_Text_PRL_SteeveNeeveeIntro::
	.string "STEEVE: Our EEVEE are ready!\p"
	.string "NEEVEE: Let's battle together!$"

Route3_Text_PRL_SteeveNeeveeDefeat::
	.string "STEEVE: Your team is amazing!\p"
	.string "NEEVEE: We'll keep training!$"

Route3_Text_PRL_SteeveNeeveeNotEnoughMons::
	.string "STEEVE: We need a fair double\n"
	.string "battle. Bring two POKéMON!$"

Route3_Text_PRL_SteeveNeeveePostBattle::
	.string "NEEVEE: Our EEVEE will keep\n"
	.string "growing with us. See you ahead!$"
