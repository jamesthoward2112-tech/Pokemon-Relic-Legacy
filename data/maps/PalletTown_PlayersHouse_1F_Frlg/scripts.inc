PalletTown_PlayersHouse_1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PalletTown_PlayersHouse_1F_OnFrame
	.byte 0

@ If the player returns home after the Hall of Fame (including older saves),
@ start this scene once. It must not interrupt the beginning of the game.
PalletTown_PlayersHouse_1F_OnFrame::
	map_script_2 VAR_TEMP_2, 0, PalletTown_PlayersHouse_1F_EventScript_PRLHomecoming
	.2byte 0

PalletTown_PlayersHouse_1F_EventScript_PRLHomecoming::
	setvar VAR_TEMP_2, 1
	goto_if_unset FLAG_SYS_GAME_CLEAR, PalletTown_PlayersHouse_1F_EventScript_PRLHomecomingDone
	goto_if_set FLAG_PRL_POSTHOF_MOM_WELCOME, PalletTown_PlayersHouse_1F_EventScript_PRLHomecomingDone
	lockall
	msgbox PalletTown_PlayersHouse_1F_Text_PRLHomecoming
	setflag FLAG_PRL_POSTHOF_MOM_WELCOME
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_PRLHomecomingDone::
	end

PalletTown_PlayersHouse_1F_EventScript_Mom::
	lock
	faceplayer
	goto_if_set FLAG_SYS_GAME_CLEAR, PalletTown_PlayersHouse_1F_EventScript_PRLMomPostLeague
	goto_if_set FLAG_BEAT_RIVAL_IN_OAKS_LAB, PalletTown_PlayersHouse_1F_EventScript_MomHeal
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale
	closemessage
	applymovement LOCALID_PLAYERS_HOUSE_1F_MOM, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouMale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllBoysLeaveOakLookingForYou
	return

PalletTown_PlayersHouse_1F_EventScript_MomOakLookingForYouFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_AllGirlsLeaveOakLookingForYou
	return

@ Direct talk fallback for anyone loading a save at Pallet Town instead
@ of reaching the house through the credits.
PalletTown_PlayersHouse_1F_EventScript_PRLMomPostLeague::
	goto_if_set FLAG_PRL_POSTHOF_MOM_WELCOME, PalletTown_PlayersHouse_1F_EventScript_PRLCheckDeparturePrep
	msgbox PalletTown_PlayersHouse_1F_Text_PRLHomecoming
	setflag FLAG_PRL_POSTHOF_MOM_WELCOME
	goto PalletTown_PlayersHouse_1F_EventScript_PRLCheckDeparturePrep

PalletTown_PlayersHouse_1F_EventScript_PRLCheckDeparturePrep::
	@ This is information only. NEVER touch bag, money or PC until boarding.
	goto_if_unset FLAG_PRL_POSTHOF_OAK_TICKET, PalletTown_PlayersHouse_1F_EventScript_MomHeal
	goto_if_set FLAG_PRL_POSTHOF_MOM_PREP_BRIEFED, PalletTown_PlayersHouse_1F_EventScript_MomHeal
	msgbox PalletTown_PlayersHouse_1F_Text_PRLDeparturePrep
	setflag FLAG_PRL_POSTHOF_MOM_PREP_BRIEFED
	goto PalletTown_PlayersHouse_1F_EventScript_MomHeal

PalletTown_PlayersHouse_1F_EventScript_MomHeal::
	msgbox PalletTown_PlayersHouse_1F_Text_YouShouldTakeQuickRest
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox PalletTown_PlayersHouse_1F_Text_LookingGreatTakeCare
	release
	end

@ Displays special text if interacted with from side or back (which are normally inaccessible)
PalletTown_PlayersHouse_1F_EventScript_TV::
	lockall
	goto_if_eq VAR_FACING, DIR_NORTH, PalletTown_PlayersHouse_1F_EventScript_TVScreen
	msgbox PalletTown_PlayersHouse_1F_Text_OopsWrongSide
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreen::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenMale
	call_if_eq VAR_RESULT, FEMALE, PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TVScreenMale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVFourBoysOnRailroad
	return

PalletTown_PlayersHouse_1F_EventScript_TVScreenFemale::
	msgbox PalletTown_PlayersHouse_1F_Text_MovieOnTVGirlOnBrickRoad
	return

PalletTown_PlayersHouse_1F_Text_AllBoysLeaveOakLookingForYou::
	.string "MOM: …Right.\n"
	.string "All boys leave home someday.\l"
	.string "It said so on TV.\p"
	.string "Oh, yes. PROF. OAK, next door, was\n"
	.string "looking for you.$"

PalletTown_PlayersHouse_1F_Text_AllGirlsLeaveOakLookingForYou::
	.string "MOM: …Right.\n"
	.string "All girls dream of traveling.\l"
	.string "It said so on TV.\p"
	.string "Oh, yes. PROF. OAK, next door, was\n"
	.string "looking for you.$"

PalletTown_PlayersHouse_1F_Text_YouShouldTakeQuickRest::
	.string "MOM: {PLAYER}!\n"
	.string "You should take a quick rest.$"

PalletTown_PlayersHouse_1F_Text_LookingGreatTakeCare::
	.string "MOM: Oh, good! You and your\n"
	.string "POKéMON are looking great.\l"
	.string "Take care now!$"

PalletTown_PlayersHouse_1F_Text_MovieOnTVFourBoysOnRailroad::
	.string "There's a movie on TV.\n"
	.string "Four boys are walking on railroad\l"
	.string "tracks.\p"
	.string "…I better go, too.$"

PalletTown_PlayersHouse_1F_Text_MovieOnTVGirlOnBrickRoad::
	.string "There's a movie on TV.\n"
	.string "A girl with her hair in pigtails is\l"
	.string "walking up a brick road.\p"
	.string "…I better go, too.$"

PalletTown_PlayersHouse_1F_Text_OopsWrongSide::
	.string "Oops, wrong side…$"

PalletTown_PlayersHouse_1F_Text_PRLHomecoming::
	.string "MOM: {PLAYER}! My new CHAMPION!\p"
	.string "I knew you'd make it. I'm so\n"
	.string "proud of you and your POKéMON.\p"
	.string "You've earned time to explore\n"
	.string "all that KANTO has left.\p"
	.string "When you're ready for more,\n"
	.string "visit PROF. OAK in his LAB.\p"
	.string "He has news of a faraway place\n"
	.string "called HOENN!$"


PalletTown_PlayersHouse_1F_Text_PRLDeparturePrep::
	.string "MOM: OAK told me about HOENN!\p"
	.string "When you're ready to sail,\n"
	.string "I'll look after your money.\p"
	.string "You'll get it all back when\n"
	.string "you return to KANTO.\p"
	.string "Every BAG item, even TMs,\n"
	.string "goes to the TRAINING SCHOOL.\p"
	.string "Only EXP SHARE and REMOTE PC\n"
	.string "Held items stay with POKéMON.\p"
	.string "Nothing will be taken until\n"
	.string "you actually board the ship!$"
