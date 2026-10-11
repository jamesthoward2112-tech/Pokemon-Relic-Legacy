
Route2_EastBuilding_Frlg_MapScripts::
	.byte 0

Route2_EastBuilding_EventScript_Aide::
	lock
	faceplayer
	call Route2_EastBuilding_EventScript_GetAideRequestInfo
	goto_if_set FLAG_GOT_HM05, Route2_EastBuilding_EventScript_AlreadyGotHM05
	msgbox Route2_EastBuilding_Text_GiveHM05IfSeen10Mons
	checkitemspace ITEM_HM05
	goto_if_eq VAR_RESULT, FALSE, Aide_EventScript_NoRoomForItem
	giveitem_msg Route2_EastBuilding_Text_ReceivedHM05FromAide, ITEM_HM05
	setflag FLAG_GOT_HM05
	msgbox Route2_EastBuilding_Text_ExplainHM05
	release
	end

Route2_EastBuilding_EventScript_AlreadyGotHM05::
	msgbox Route2_EastBuilding_Text_ExplainHM05
	release
	end

Route2_EastBuilding_EventScript_GetAideRequestInfo::
	bufferitemname STR_VAR_2, ITEM_HM05
	return

Route2_EastBuilding_EventScript_Rocker::
	msgbox Route2_EastBuilding_Text_CanGetThroughRockTunnel, MSGBOX_NPC
	end

Route2_EastBuilding_Text_GiveHM05IfSeen10Mons::
	.string "PROF. OAK has sent me to study
ancient relics in Kanto.pHe left HM05 FLASH for you.
It will help on your journey.$"

Route2_EastBuilding_Text_GreatHereYouGo::
	.string "PROF. OAK sent this for you.\n"
	.string "Here you go!$"

Route2_EastBuilding_Text_ReceivedHM05FromAide::
	.string "{PLAYER} received HM05\n"
	.string "from the AIDE.$"

Route2_EastBuilding_Text_ExplainHM05::
	.string "HM05 contains the hidden move\n"
	.string "FLASH.\p"
	.string "FLASH lights up even the darkest\n"
	.string "of caves and dungeons.$"

Route2_EastBuilding_Text_CanGetThroughRockTunnel::
	.string "Once a POKéMON learns FLASH, you\n"
	.string "can get through ROCK TUNNEL.$"

