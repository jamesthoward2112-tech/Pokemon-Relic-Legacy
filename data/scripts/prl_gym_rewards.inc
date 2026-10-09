@ PRL Gym first-clear extras: held item then 10,000 Pokédollar cash, once.
@ Never award money or set flag until giveitem succeeds.
@ Full Bag -> return to Gym Leader to claim item AND bonus on next dialogue.

PRL_EventScript_GymExtra_KANTO_BROCK::
	goto_if_set FLAG_PRL_GYM_KANTO_BROCK_EXTRA, PRL_EventScript_GymExtra_KANTO_BROCK_Done
	checkitemspace ITEM_HARD_STONE
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_HARD_STONE
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_BROCK_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_BROCK_Done::
	return

PRL_EventScript_GymExtra_KANTO_MISTY::
	goto_if_set FLAG_PRL_GYM_KANTO_MISTY_EXTRA, PRL_EventScript_GymExtra_KANTO_MISTY_Done
	checkitemspace ITEM_MYSTIC_WATER
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_MYSTIC_WATER
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_MISTY_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_MISTY_Done::
	return

PRL_EventScript_GymExtra_KANTO_SURGE::
	goto_if_set FLAG_PRL_GYM_KANTO_SURGE_EXTRA, PRL_EventScript_GymExtra_KANTO_SURGE_Done
	checkitemspace ITEM_MAGNET
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_MAGNET
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_SURGE_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_SURGE_Done::
	return

PRL_EventScript_GymExtra_KANTO_ERIKA::
	goto_if_set FLAG_PRL_GYM_KANTO_ERIKA_EXTRA, PRL_EventScript_GymExtra_KANTO_ERIKA_Done
	checkitemspace ITEM_MIRACLE_SEED
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_MIRACLE_SEED
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_ERIKA_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_ERIKA_Done::
	return

PRL_EventScript_GymExtra_KANTO_KOGA::
	goto_if_set FLAG_PRL_GYM_KANTO_KOGA_EXTRA, PRL_EventScript_GymExtra_KANTO_KOGA_Done
	checkitemspace ITEM_POISON_BARB
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_POISON_BARB
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_KOGA_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_KOGA_Done::
	return

PRL_EventScript_GymExtra_KANTO_SABRINA::
	goto_if_set FLAG_PRL_GYM_KANTO_SABRINA_EXTRA, PRL_EventScript_GymExtra_KANTO_SABRINA_Done
	checkitemspace ITEM_TWISTED_SPOON
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_TWISTED_SPOON
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_SABRINA_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_SABRINA_Done::
	return

PRL_EventScript_GymExtra_KANTO_BLAINE::
	goto_if_set FLAG_PRL_GYM_KANTO_BLAINE_EXTRA, PRL_EventScript_GymExtra_KANTO_BLAINE_Done
	checkitemspace ITEM_CHARCOAL
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_CHARCOAL
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_BLAINE_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_BLAINE_Done::
	return

PRL_EventScript_GymExtra_KANTO_GIOVANNI::
	goto_if_set FLAG_PRL_GYM_KANTO_GIOVANNI_EXTRA, PRL_EventScript_GymExtra_KANTO_GIOVANNI_Done
	checkitemspace ITEM_SOFT_SAND
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_SOFT_SAND
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_KANTO_GIOVANNI_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_KANTO_GIOVANNI_Done::
	return

PRL_EventScript_GymExtra_HOENN_ROXANNE::
	goto_if_set FLAG_PRL_GYM_HOENN_ROXANNE_EXTRA, PRL_EventScript_GymExtra_HOENN_ROXANNE_Done
	checkitemspace ITEM_HARD_STONE
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_HARD_STONE
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_ROXANNE_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_ROXANNE_Done::
	return

PRL_EventScript_GymExtra_HOENN_BRAWLY::
	goto_if_set FLAG_PRL_GYM_HOENN_BRAWLY_EXTRA, PRL_EventScript_GymExtra_HOENN_BRAWLY_Done
	checkitemspace ITEM_BLACK_BELT
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_BLACK_BELT
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_BRAWLY_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_BRAWLY_Done::
	return

PRL_EventScript_GymExtra_HOENN_WATTSON::
	goto_if_set FLAG_PRL_GYM_HOENN_WATTSON_EXTRA, PRL_EventScript_GymExtra_HOENN_WATTSON_Done
	checkitemspace ITEM_MAGNET
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_MAGNET
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_WATTSON_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_WATTSON_Done::
	return

PRL_EventScript_GymExtra_HOENN_FLANNERY::
	goto_if_set FLAG_PRL_GYM_HOENN_FLANNERY_EXTRA, PRL_EventScript_GymExtra_HOENN_FLANNERY_Done
	checkitemspace ITEM_CHARCOAL
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_CHARCOAL
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_FLANNERY_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_FLANNERY_Done::
	return

PRL_EventScript_GymExtra_HOENN_NORMAN::
	goto_if_set FLAG_PRL_GYM_HOENN_NORMAN_EXTRA, PRL_EventScript_GymExtra_HOENN_NORMAN_Done
	checkitemspace ITEM_SILK_SCARF
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_SILK_SCARF
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_NORMAN_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_NORMAN_Done::
	return

PRL_EventScript_GymExtra_HOENN_WINONA::
	goto_if_set FLAG_PRL_GYM_HOENN_WINONA_EXTRA, PRL_EventScript_GymExtra_HOENN_WINONA_Done
	checkitemspace ITEM_SHARP_BEAK
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_SHARP_BEAK
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_WINONA_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_WINONA_Done::
	return

PRL_EventScript_GymExtra_HOENN_TATE_LIZA::
	goto_if_set FLAG_PRL_GYM_HOENN_TATE_LIZA_EXTRA, PRL_EventScript_GymExtra_HOENN_TATE_LIZA_Done
	checkitemspace ITEM_TWISTED_SPOON
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_TWISTED_SPOON
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_TATE_LIZA_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_TATE_LIZA_Done::
	return

PRL_EventScript_GymExtra_HOENN_JUAN::
	goto_if_set FLAG_PRL_GYM_HOENN_JUAN_EXTRA, PRL_EventScript_GymExtra_HOENN_JUAN_Done
	checkitemspace ITEM_MYSTIC_WATER
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	giveitem ITEM_MYSTIC_WATER
	goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom
	addmoney 10000
	setflag FLAG_PRL_GYM_HOENN_JUAN_EXTRA
	msgbox PRL_Text_GymExtraClaimed, MSGBOX_DEFAULT
PRL_EventScript_GymExtra_HOENN_JUAN_Done::
	return

PRL_EventScript_GymExtraNoRoom::
	msgbox PRL_Text_GymExtraNoRoom, MSGBOX_DEFAULT
	return

PRL_Text_GymExtraClaimed::
	.string "You earned an additional 10,000\n"
	.string "for winning this GYM battle!$"

PRL_Text_GymExtraNoRoom::
	.string "Your BAG is full. Please free a\n"
	.string "slot, then return for your bonus.$"
