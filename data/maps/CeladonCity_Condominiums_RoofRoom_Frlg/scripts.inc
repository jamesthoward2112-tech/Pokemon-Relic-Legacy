CeladonCity_Condominiums_RoofRoom_Frlg_MapScripts::
	.byte 0

CeladonCity_Condominiums_RoofRoom_EventScript_BlackBelt::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_TheresNothingIDontKnow, MSGBOX_NPC
	end

// PRL Celadon Eeveelutions Expert, accessible beside the rooftop Eevee gift.
// Canonical methods verified against gen_1_families.h Eevee evolution table.
CeladonCity_Condominiums_RoofRoom_EventScript_PRLEeveelutionsExpert::
	lock
	faceplayer
	msgbox CeladonCity_Condominiums_RoofRoom_Text_PRLExpertGreeting
	goto CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu::
	message CeladonCity_Condominiums_RoofRoom_Text_PRLExpertChoice
	waitmessage
	multichoice 0, 0, MULTI_PRL_EEVEE_EXPERT, FALSE
	switch VAR_RESULT
	case 0, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertStones
	case 1, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertHeldItems
	case 2, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertStoneGuide
	case 3, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertOtherGuide
	case 4, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertExit
	case 127, CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertExit
	end

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertStones::
	pokemart CeladonCity_Condominiums_RoofRoom_PRLEeveeStones
	goto CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertHeldItems::
	pokemart CeladonCity_Condominiums_RoofRoom_PRLEeveeHeldItems
	goto CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertStoneGuide::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_PRLExpertStoneGuide
	goto CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertOtherGuide::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_PRLExpertOtherGuide
	goto CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertMenu

CeladonCity_Condominiums_RoofRoom_EventScript_PRLExpertExit::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_PRLExpertGoodbye
	release
	end

	.align 2
CeladonCity_Condominiums_RoofRoom_PRLEeveeStones::
	.2byte ITEM_WATER_STONE
	.2byte ITEM_THUNDER_STONE
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_ICE_STONE
	.2byte ITEM_DUSK_STONE
	.2byte ITEM_ANCIENT_STONE
	.2byte ITEM_NONE

	.align 2
CeladonCity_Condominiums_RoofRoom_PRLEeveeHeldItems::
	.2byte ITEM_SHARP_BEAK
	.2byte ITEM_EXPERT_BELT
	.2byte ITEM_SILVER_POWDER
	.2byte ITEM_SOFT_SAND
	.2byte ITEM_METAL_COAT
	.2byte ITEM_HARD_STONE
	.2byte ITEM_POISON_BARB
	.2byte ITEM_DRAGON_FANG
	.2byte ITEM_NONE

CeladonCity_Condominiums_RoofRoom_Text_PRLExpertGreeting::
	.string "I'm the EEVEELUTIONS EXPERT!\n"
	.string "Even EEVEE's rare RELIC forms\n"
	.string "have their own methods.\p"
	.string "I sell useful catalysts and\n"
	.string "explain all eighteen forms.$"

CeladonCity_Condominiums_RoofRoom_Text_PRLExpertChoice::
	.string "What would you like to know?$"

CeladonCity_Condominiums_RoofRoom_Text_PRLExpertStoneGuide::
	.string "Water Stone: VAPOREON.\n"
	.string "Thunder Stone: JOLTEON.\n"
	.string "Fire Stone: FLAREON.\p"
	.string "Leaf Stone: LEAFEON.\n"
	.string "Ice Stone: GLACEON.\n"
	.string "Dusk Stone: OMEON.\p"
	.string "ANCIENT STONE: EEVEEON!\n"
	.string "Use the stone from your BAG\n"
	.string "on EEVEE to evolve it.$"

CeladonCity_Condominiums_RoofRoom_Text_PRLExpertOtherGuide::
	.string "Hold SHARP BEAK and level up\n"
	.string "by day for NIMBEON.\p"
	.string "Hold EXPERT BELT by day:\n"
	.string "CHAMPEON.\p"
	.string "Hold SILVER POWDER by day:\n"
	.string "LEPIDEON.\p"
	.string "Hold SOFT SAND by day:\n"
	.string "SPHYNXEON.\p"
	.string "Hold METAL COAT at night:\n"
	.string "GUARDEON.\p"
	.string "Hold HARD STONE at night:\n"
	.string "OBSIDEON.\p"
	.string "Hold POISON BARB at night:\n"
	.string "TOXEON.\p"
	.string "Hold DRAGON FANG and reach\n"
	.string "Lv. 25: DRAKEON.\p"
	.string "High friendship by day:\n"
	.string "ESPEON; at night: UMBREON.\p"
	.string "High friendship and a FAIRY\n"
	.string "move: SYLVEON.\p"
	.string "LEAFEON can also evolve in\n"
	.string "PETALBURG WOODS; GLACEON in\n"
	.string "SHOAL CAVE's icy room.$"

CeladonCity_Condominiums_RoofRoom_Text_PRLExpertGoodbye::
	.string "Treat your EEVEE well!\n"
	.string "Every form has potential.$"


CeladonCity_Condominiums_RoofRoom_EventScript_EeveeBall::
	lock
	faceplayer
	setvar VAR_TEMP_1, SPECIES_EEVEE
	givemon SPECIES_EEVEE, 25
	goto_if_eq VAR_RESULT, 0, CeladonCity_Condominiums_RoofRoom_EventScript_GetEeveeParty
	goto_if_eq VAR_RESULT, 1, CeladonCity_Condominiums_RoofRoom_EventScript_GetEeveePC
	goto_if_eq VAR_RESULT, 2, Common_EventScript_NoMoreRoomForPokemon
	release
	end

CeladonCity_Condominiums_RoofRoom_EventScript_GetEeveeParty::
	removeobject LOCALID_EEVEE_POKEBALL
	playfanfare MUS_LEVEL_UP
	message CeladonCity_Condominiums_RoofRoom_Text_ObtainedAnEevee
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_EEVEE
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_Condominiums_RoofRoom_EventScript_SetGotEevee
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto CeladonCity_Condominiums_RoofRoom_EventScript_SetGotEevee
	end

CeladonCity_Condominiums_RoofRoom_EventScript_GetEeveePC::
	removeobject LOCALID_EEVEE_POKEBALL
	playfanfare MUS_LEVEL_UP
	message CeladonCity_Condominiums_RoofRoom_Text_ObtainedAnEevee
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_EEVEE
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CeladonCity_Condominiums_RoofRoom_EventScript_TransferEeveeToPC
	call Common_EventScript_NameReceivedBoxMon
	goto CeladonCity_Condominiums_RoofRoom_EventScript_TransferEeveeToPC
	end

CeladonCity_Condominiums_RoofRoom_EventScript_TransferEeveeToPC::
	call Common_EventScript_TransferredToPC
	goto CeladonCity_Condominiums_RoofRoom_EventScript_SetGotEevee
	end

CeladonCity_Condominiums_RoofRoom_EventScript_SetGotEevee::
	setflag FLAG_GOT_EEVEE
	release
	end

CeladonCity_Condominiums_RoofRoom_EventScript_TMsPamphlet::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_PamphletOnTMs, MSGBOX_SIGN
	end

CeladonCity_Condominiums_RoofRoom_EventScript_Blackboard::
	lockall
	msgbox CeladonCity_Condominiums_RoofRoom_Text_WirelessAdapterLecture
	message CeladonCity_Condominiums_RoofRoom_Text_ReadWhichHeading
	waitmessage
	setvar VAR_0x8004, 4
	multichoice 0, 0, MULTI_LINKED_DIRECT_UNION, FALSE
	switch VAR_RESULT
	case 0, CeladonCity_Condominiums_RoofRoom_EventScript_WirelessClub
	case 1, CeladonCity_Condominiums_RoofRoom_EventScript_DirectCorner
	case 2, CeladonCity_Condominiums_RoofRoom_EventScript_UnionRoom
	case 3, CeladonCity_Condominiums_RoofRoom_EventScript_ExitBlackboard
	case 127, CeladonCity_Condominiums_RoofRoom_EventScript_ExitBlackboard
	end

CeladonCity_Condominiums_RoofRoom_EventScript_ReadAnotherHeading::
	message CeladonCity_Condominiums_RoofRoom_Text_ReadWhichHeading
	waitmessage
	multichoice 0, 0, MULTI_LINKED_DIRECT_UNION, FALSE
	switch VAR_RESULT
	case 0, CeladonCity_Condominiums_RoofRoom_EventScript_WirelessClub
	case 1, CeladonCity_Condominiums_RoofRoom_EventScript_DirectCorner
	case 2, CeladonCity_Condominiums_RoofRoom_EventScript_UnionRoom
	case 3, CeladonCity_Condominiums_RoofRoom_EventScript_ExitBlackboard
	case 127, CeladonCity_Condominiums_RoofRoom_EventScript_ExitBlackboard
	end

CeladonCity_Condominiums_RoofRoom_EventScript_WirelessClub::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_ExplainWirelessClub
	goto CeladonCity_Condominiums_RoofRoom_EventScript_ReadAnotherHeading
	end

CeladonCity_Condominiums_RoofRoom_EventScript_DirectCorner::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_ExplainDirectCorner
	goto CeladonCity_Condominiums_RoofRoom_EventScript_ReadAnotherHeading
	end

CeladonCity_Condominiums_RoofRoom_EventScript_UnionRoom::
	msgbox CeladonCity_Condominiums_RoofRoom_Text_ExplainUnionRoom
	goto CeladonCity_Condominiums_RoofRoom_EventScript_ReadAnotherHeading
	end

CeladonCity_Condominiums_RoofRoom_EventScript_ExitBlackboard::
	releaseall
	end

CeladonCity_Condominiums_RoofRoom_Text_TheresNothingIDontKnow::
	.string "There is nothing that I don't know,\n"
	.string "like I wrote on the blackboard.\p"
	.string "I know about the world of POKéMON\n"
	.string "in your GAME BOY ADVANCE!\p"
	.string "Get together with your friends and\n"
	.string "enjoy trading POKéMON!$"

CeladonCity_Condominiums_RoofRoom_Text_ObtainedAnEevee::
	.string "{PLAYER} obtained an EEVEE!$"

CeladonCity_Condominiums_RoofRoom_Text_BoxIsFull::
	.string "ポケモンが　いっぱいだ\n"
	.string "ボックスを　かえて　きなさい$"

CeladonCity_Condominiums_RoofRoom_Text_WirelessAdapterLecture::
	.string "POKéMON Lecture\p"
	.string "Playing with the Wireless Adapter$"

CeladonCity_Condominiums_RoofRoom_Text_ReadWhichHeading::
	.string "Which heading do you want to read?$"

CeladonCity_Condominiums_RoofRoom_Text_ExplainWirelessClub::
	.string "The POKéMON WIRELESS CLUB is\n"
	.string "upstairs at any POKéMON CENTER.\p"
	.string "Visit one to link up with friend(s)\n"
	.string "using your Wireless Adapter.$"

CeladonCity_Condominiums_RoofRoom_Text_ExplainDirectCorner::
	.string "To link only with a friend, go to\n"
	.string "the DIRECT CORNER.\p"
	.string "It is the right counter at the\n"
	.string "POKéMON WIRELESS CLUB.\p"
	.string "Go into the TRADE CORNER or the\n"
	.string "COLOSSEUM with your friend.$"

CeladonCity_Condominiums_RoofRoom_Text_ExplainUnionRoom::
	.string "To link and communicate with\n"
	.string "anyone, go to the UNION ROOM.\p"
	.string "It is the left counter at the\n"
	.string "POKéMON WIRELESS CLUB.\p"
	.string "Go to the UNION ROOM and enjoy\n"
	.string "meeting other TRAINERS.$"

CeladonCity_Condominiums_RoofRoom_Text_PamphletOnTMs::
	.string "It's a pamphlet on TMs.\p"
	.string "… …\p"
	.string "There are fifty TMs in all.\p"
	.string "There are also seven HMs that\n"
	.string "can be used repeatedly.\p"
	.string "SILPH CO.$"

