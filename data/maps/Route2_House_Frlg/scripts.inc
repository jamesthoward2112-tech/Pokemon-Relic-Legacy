Route2_House_Frlg_MapScripts::
	.byte 0

Route2_House_EventScript_Scientist::
	lock
	faceplayer
	goto_if_set FLAG_PRL_ROUTE2_HOUSE_RESEARCH_GIFT, Route2_House_EventScript_ResearchGiftAlreadyGiven
	msgbox Route2_House_Text_OakResearchAideIntro
	checkitemspace ITEM_RARE_CANDY
	goto_if_eq VAR_RESULT, FALSE, Aide_EventScript_NoRoomForItem
	giveitem_msg Route2_House_Text_OakResearchAideGift, ITEM_RARE_CANDY
	setflag FLAG_PRL_ROUTE2_HOUSE_RESEARCH_GIFT
	msgbox Route2_House_Text_OakResearchAideAfter
	release
	end

Route2_House_EventScript_ResearchGiftAlreadyGiven::
	msgbox Route2_House_Text_OakResearchAideAfter
	release
	end

Route2_House_EventScript_Reyley::
	lock
	faceplayer
	setvar VAR_0x8008, INGAME_TRADE_MR_MIME
	call EventScript_GetInGameTradeSpeciesInfo
	goto_if_set FLAG_DID_MIMIEN_TRADE, Route2_House_EventScript_AlreadyTraded
	msgbox Trade_Text_LookingForMonWannaTradeForMon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route2_House_EventScript_DeclineTrade
	call EventScript_ChooseMonForInGameTrade
	goto_if_ge VAR_0x8004, PARTY_SIZE, Route2_House_EventScript_DeclineTrade
	call EventScript_GetInGameTradeSpecies
	goto_if_ne VAR_RESULT, VAR_0x8009, Route2_House_EventScript_NotRequestedMon
	call EventScript_DoInGameTrade
	msgbox Trade_Text_HeyThanks
	setflag FLAG_DID_MIMIEN_TRADE
	release
	end

Route2_House_EventScript_DeclineTrade::
	msgbox Trade_Text_AwwOhWell
	release
	end

Route2_House_EventScript_NotRequestedMon::
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox Trade_Text_WhatThatsNoMon
	release
	end

Route2_House_EventScript_AlreadyTraded::
	msgbox Trade_Text_IsntMyOldMonGreat
	release
	end

Route2_House_Text_OakResearchAideIntro::
	.string "I'm assisting PROF. OAK with\n"
	.string "his Kanto relic research.\p"
	.string "We found something useful for\n"
	.string "trainers exploring the region.\p"
	.string "Please take this RARE CANDY.$"

Route2_House_Text_OakResearchAideGift::
	.string "{PLAYER} received a RARE CANDY\n"
	.string "from OAK's researcher!$"

Route2_House_Text_OakResearchAideAfter::
	.string "PROF. OAK is investigating\n"
	.string "ancient POKéMON relics.\p"
	.string "Come back if you find anything\n"
	.string "unusual in Kanto.$
