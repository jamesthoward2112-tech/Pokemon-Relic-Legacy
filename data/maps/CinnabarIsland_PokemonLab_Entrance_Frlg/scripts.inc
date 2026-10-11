CinnabarIsland_PokemonLab_Entrance_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CinnabarIsland_PokemonLab_Entrance_OnTransition
	.byte 0

CinnabarIsland_PokemonLab_Entrance_OnTransition::
	call_if_eq VAR_MAP_SCENE_CINNABAR_ISLAND_POKEMON_LAB_EXPERIMENT_ROOM_REVIVE_STATE, 1, CinnabarIsland_PokemonLab_Entrance_EventScript_ReadyFossil
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_ReadyFossil::
	setvar VAR_MAP_SCENE_CINNABAR_ISLAND_POKEMON_LAB_EXPERIMENT_ROOM_REVIVE_STATE, 2
	return

CinnabarIsland_PokemonLab_Entrance_EventScript_Scientist::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_StudyMonsExtensively, MSGBOX_NPC
	end

// Oak's aide returns the fossil the player LEFT behind at Mt. Moon.
// Both origin flags are persistent and work with pre-update save files.
// Never gate on Pokédex progress, never award both fossils twice.
CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAide::
	lock
	faceplayer
	goto_if_set FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilGiftDone
	goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNotYet
	goto_if_set FLAG_GOT_DOME_FOSSIL, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilCheckHelix
	goto_if_set FLAG_GOT_HELIX_FOSSIL, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilGiveDome
	goto CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNotYet

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilCheckHelix::
	goto_if_set FLAG_GOT_HELIX_FOSSIL, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAlreadyOwned
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakOffersHelix
	checkitemspace ITEM_HELIX_FOSSIL, 1
	goto_if_eq VAR_RESULT, FALSE, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace
	giveitem ITEM_HELIX_FOSSIL, 1
	goto_if_eq VAR_RESULT, FALSE, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace
	setflag FLAG_GOT_HELIX_FOSSIL
	setflag FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT
	goto CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAfterGift

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilGiveDome::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakOffersDome
	checkitemspace ITEM_DOME_FOSSIL, 1
	goto_if_eq VAR_RESULT, FALSE, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace
	giveitem ITEM_DOME_FOSSIL, 1
	goto_if_eq VAR_RESULT, FALSE, CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace
	setflag FLAG_GOT_DOME_FOSSIL
	setflag FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT
	goto CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAfterGift

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAlreadyOwned::
	setflag FLAG_PRL_CINNABAR_OAK_FOSSIL_GIFT
	goto CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilGiftDone

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilAfterGift::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilRevive
	release
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNoBagSpace::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilBagFull
	release
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilNotYet::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilNotYet
	release
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_PRL_FossilGiftDone::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilGiftDone
	release
	end

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakOffersHelix::
	.string "I'm one of PROF. OAK's aides!\n"
	.string "I brought the fossil you left\n"
	.string "behind at MT. MOON.\p"
	.string "You took the DOME FOSSIL,\n"
	.string "so this HELIX FOSSIL is yours!$"

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakOffersDome::
	.string "I'm one of PROF. OAK's aides!\n"
	.string "I brought the fossil you left\n"
	.string "behind at MT. MOON.\p"
	.string "You took the HELIX FOSSIL,\n"
	.string "so this DOME FOSSIL is yours!$"

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilRevive::
	.string "Our scientist can revive it!\n"
	.string "He's in the Testing Room,\n"
	.string "at the far end of the LAB.$"

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilBagFull::
	.string "Your BAG is full! Make room\n"
	.string "and come back for the fossil.$"

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilNotYet::
	.string "I'm PROF. OAK's aide.\n"
	.string "When you choose a fossil at\n"
	.string "MT. MOON, come find me here!$"

CinnabarIsland_PokemonLab_Entrance_Text_PRL_OakFossilGiftDone::
	.string "Hope your second fossil\n"
	.string "helps your research!\p"
	.string "Our LAB can revive it for you.$"


CinnabarIsland_PokemonLab_Entrance_EventScript_DrFujiPhoto::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_PhotoOfLabFounderDrFuji, MSGBOX_SIGN
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_MeetingRoomSign::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_MeetingRoomSign, MSGBOX_SIGN
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_RAndDRoomSign::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_RAndDRoomSign, MSGBOX_SIGN
	end

CinnabarIsland_PokemonLab_Entrance_EventScript_TestingRoomSign::
	msgbox CinnabarIsland_PokemonLab_Entrance_Text_TestingRoomSign, MSGBOX_SIGN
	end

CinnabarIsland_Gym_Text_PhotoOfBlaineAndFuji::
	.string "It's a photo of BLAINE and\n"
	.string "MR. FUJI.\p"
	.string "They're standing shoulder to\n"
	.string "shoulder with big grins.$"

CinnabarIsland_PokemonLab_Entrance_Text_StudyMonsExtensively::
	.string "We examine POKéMON and\n"
	.string "ancient fossils every day.\p"
	.string "Some of their genes have\n"
	.string "the same dormant RELIC mark.\p"
	.string "Our CINNABAR research calls\n"
	.string "the signal RELIC-01.$"

CinnabarIsland_PokemonLab_Entrance_Text_PhotoOfLabFounderDrFuji::
	.string "A photo of the LAB's\n"
	.string "founder, DR. FUJI.\p"
	.string "A faded note mentions\n"
	.string "BLAINE and RELIC-01 research.$"

CinnabarIsland_PokemonLab_Entrance_Text_MeetingRoomSign::
	.string "POKéMON LAB\n"
	.string "Meeting Room$"

CinnabarIsland_PokemonLab_Entrance_Text_RAndDRoomSign::
	.string "POKéMON LAB\n"
	.string "R & D Room$"

CinnabarIsland_PokemonLab_Entrance_Text_TestingRoomSign::
	.string "POKéMON LAB\n"
	.string "Testing Room$"

