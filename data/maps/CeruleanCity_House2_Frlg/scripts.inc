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
	.string "stole my DIG TM!\p"
	.string "They kept talking about\n"
	.string "an ancient layer underground.\p"
	.string "Why steal a TM to excavate\n"
	.string "beneath CERULEAN?$"

CeruleanCity_House2_Text_TeachDiglettDigWithoutTM::
	.string "I figure what's lost is lost.\p"
	.string "I decided to teach DIGLETT how to\n"
	.string "DIG without a TM.$"

CeruleanCity_House2_Text_TeamRocketTryingToDigIntoNoGood::
	.string "ROCKET's DIG plans weren't\n"
	.string "about buried treasure.\p"
	.string "I heard them mention\n"
	.string "RELIC fragments!$"

CeruleanCity_House2_Text_TeamRocketLeftWayOut::
	.string "TEAM ROCKET left a way out!$"

