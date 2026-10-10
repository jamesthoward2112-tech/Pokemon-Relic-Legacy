CinnabarIsland_PokemonLab_ResearchRoom_Frlg_MapScripts::
	.byte 0

CinnabarIsland_PokemonLab_ResearchRoom_EventScript_MetronomeTutor::
	goto EventScript_MetronomeTutor
	end

CinnabarIsland_PokemonLab_ResearchRoom_EventScript_Scientist::
	lock
	faceplayer
	msgbox CinnabarIsland_PokemonLab_ResearchRoom_Text_EeveeCanEvolveIntroThreeMons
	goto EventScript_PRL_TutorHeatWave
	end

CinnabarIsland_PokemonLab_ResearchRoom_EventScript_Computer::
	msgbox CinnabarIsland_PokemonLab_ResearchRoom_Text_LegendaryBirdEmail, MSGBOX_SIGN
	end

CinnabarIsland_PokemonLab_ResearchRoom_EventScript_AmberPipe::
	msgbox CinnabarIsland_PokemonLab_ResearchRoom_Text_AnAmberPipe, MSGBOX_SIGN
	end

CinnabarIsland_PokemonLab_ResearchRoom_Text_EeveeCanEvolveIntroThreeMons::
	.string "EEVEE can evolve into one of three\n"
	.string "kinds of POKéMON.$"

CinnabarIsland_PokemonLab_ResearchRoom_Text_LegendaryBirdEmail::
	.string "There's an e-mail message.\p"
	.string "KANTO's known legendary\n"
	.string "birds are ARTICUNO, ZAPDOS\n"
	.string "and MOLTRES.\p"
	.string "A separate old sketch shows\n"
	.string "a fourth bird, coloured gold.\p"
	.string "No species matches it.\n"
	.string "Identity: UNKNOWN.\p"
	.string "File reference: RELIC-01.$"

CinnabarIsland_PokemonLab_ResearchRoom_Text_AnAmberPipe::
	.string "An old AMBER specimen.\n"
	.string "Its label reads RELIC-01.\p"
	.string "FOSSIL DNA and modern genes\n"
	.string "show a curious shared trait.$"

