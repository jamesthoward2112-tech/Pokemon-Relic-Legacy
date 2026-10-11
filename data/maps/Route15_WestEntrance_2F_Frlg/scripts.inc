
Route15_WestEntrance_2F_Frlg_MapScripts::
	.byte 0

Route15_WestEntrance_2F_EventScript_LeftBinoculars::
	lockall
	msgbox Route15_WestEntrance_2F_Text_LargeShiningBird
	showmonpic SPECIES_ARTICUNO, 10, 3
	delay 20
	waitbuttonpress
	hidemonpic
	setvar VAR_0x8004, SPECIES_ARTICUNO
	special SetSeenMon
	releaseall
	end

Route15_WestEntrance_2F_EventScript_RightBinoculars::
	msgbox Route15_WestEntrance_2F_Text_SmallIslandOnHorizon, MSGBOX_SIGN
	end

Route15_WestEntrance_2F_EventScript_Aide::
	lock
	faceplayer
	call Route15_WestEntrance_2F_EventScript_GetAideRequestInfo
	goto_if_set FLAG_GOT_EXP_SHARE_FROM_OAKS_AIDE, Route15_WestEntrance_2F_EventScript_AlreadyGotExpShare
	msgbox Route15_WestEntrance_2F_Text_GiveItemIfCaughtEnough
	checkitemspace ITEM_LEFTOVERS
	goto_if_eq VAR_RESULT, FALSE, Aide_EventScript_NoRoomForItem
	giveitem_msg Route15_WestEntrance_2F_Text_ReceivedItemFromAide, ITEM_LEFTOVERS
	setflag FLAG_GOT_EXP_SHARE_FROM_OAKS_AIDE
	msgbox Route15_WestEntrance_2F_Text_ExplainExpShare
	release
	end

Route15_WestEntrance_2F_EventScript_AlreadyGotExpShare::
	msgbox Route15_WestEntrance_2F_Text_ExplainExpShare
	release
	end

Route15_WestEntrance_2F_EventScript_GetAideRequestInfo::
	bufferitemname STR_VAR_2, ITEM_LEFTOVERS
	return

Route15_WestEntrance_2F_Text_GiveItemIfCaughtEnough::
	.string "The relic search has taken
our team across Kanto.pPROF. OAK sent LEFTOVERS
for your long expeditions.$"

Route15_WestEntrance_2F_Text_GreatHereYouGo::
	.string "PROF. OAK sent this for you.\n"
	.string "Here you go!$"

Route15_WestEntrance_2F_Text_ReceivedItemFromAide::
	.string "{PLAYER} received LEFTOVERS\n"
	.string "from PROF. OAK's AIDE!$"

Route15_WestEntrance_2F_Text_ExplainExpShare::
	.string "LEFTOVERS restores HP each
turn during battle.pKeep it on a POKéMON for
those long relic searches.$"

Route15_WestEntrance_2F_Text_LargeShiningBird::
	.string "Let's see what the binoculars have\n"
	.string "to show…\p"
	.string "A large, shining bird is flying\n"
	.string "toward the sea.$"

Route15_WestEntrance_2F_Text_SmallIslandOnHorizon::
	.string "Let's see what the binoculars have\n"
	.string "to show…\p"
	.string "It looks like a small island on\n"
	.string "the horizon!$"

