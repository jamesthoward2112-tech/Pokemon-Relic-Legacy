CeruleanCity_House2_Frlg_MapScripts::
	.byte 0

CeruleanCity_House2_EventScript_Hiker::
	lock
	faceplayer
	goto_if_set FLAG_GOT_TM28_FROM_ROCKET, CeruleanCity_House2_EventScript_HikerGotTM28
	msgbox CeruleanCity_House2_Text_RocketsStoleTMForDig
	release
	end

CeruleanCity_House2_EventScript_HikerGotTM28::
	msgbox CeruleanCity_House2_Text_TeachDiglettDigWithoutTM
	release
	end

CeruleanCity_House2_EventScript_Lass::
	msgbox CeruleanCity_House2_Text_TeamRocketTryingToDigIntoNoGood, MSGBOX_NPC
	end

CeruleanCity_House2_EventScript_WallHole::
	msgbox CeruleanCity_House2_Text_TeamRocketLeftWayOut, MSGBOX_SIGN
	end

CeruleanCity_House2_Text_RocketsStoleTMForDig::
	.string "Those wretched ROCKETS!\p"
	.string "They smashed my wall and\n"
	.string "stole TM28 DIG!\p"
	.string "They said they'd use DIG to\n"
	.string "reach ancient fossil beds.\p"
	.string "What are they looking for\n"
	.string "beneath CERULEAN?$"

CeruleanCity_House2_Text_TeachDiglettDigWithoutTM::
	.string "You got TM28 DIG back?\n"
	.string "You earned it, so keep it!\p"
	.string "Those ROCKETS were hunting\n"
	.string "fossil RELIC fragments.\p"
	.string "I'll teach DIGLETT DIG myself.$"

CeruleanCity_House2_Text_TeamRocketTryingToDigIntoNoGood::
	.string "ROCKET's DIG plans weren't\n"
	.string "about buried treasure.\p"
	.string "I heard them mention\n"
	.string "RELIC fragments!$"

CeruleanCity_House2_Text_TeamRocketLeftWayOut::
	.string "TEAM ROCKET left a way out!$"

