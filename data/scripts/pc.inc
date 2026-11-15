EventScript_PC::
	lockall
	setvar VAR_0x8004, PC_LOCATION_OTHER
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox Text_BootUpPC, MSGBOX_DEFAULT
	goto EventScript_PCMainMenu
	end

EventScript_PCMainMenu::
	message gText_WhichPCShouldBeAccessed
	waitmessage
	special ScriptMenu_CreatePCMultichoice
	goto EventScript_AccessPC
	end

EventScript_AccessPC::
	switch VAR_RESULT
	case 0, EventScript_AccessPokemonStorage
	case 1, EventScript_AccessPlayersPC
	case 2, EventScript_AccessPRRCodes
	case 3, EventScript_AccessPRRHallOrOff
	case 4, EventScript_TurnOffPC
	case MULTI_B_PRESSED, EventScript_TurnOffPC
	end

@ When the Hall of Fame is unavailable, index 3 is LOG OFF.
EventScript_AccessPRRHallOrOff::
	goto_if_unset FLAG_SYS_GAME_CLEAR, EventScript_TurnOffPC
	goto EventScript_AccessHallOfFame

EventScript_AccessPRRCodes::
	playse SE_PC_LOGIN
	msgbox PRR_PcText_EnterCode, MSGBOX_DEFAULT
	special EnterCode
	waitstate
	special GetCodeFeedback
	goto_if_eq VAR_RESULT, 0, EventScript_PRRUnknownCode
	msgbox PRR_PcText_Recognized, MSGBOX_DEFAULT
	goto EventScript_PCMainMenu

EventScript_PRRUnknownCode::
	msgbox PRR_PcText_UnknownCode, MSGBOX_DEFAULT
	goto EventScript_PCMainMenu

PRR_PcText_EnterCode::
	.string "Enter a PRR code.\n"
	.string "Repeat OFF codes to disable.$"

PRR_PcText_Recognized::
	.string "PRR setting saved.\n"
	.string "You can change it later.$"

PRR_PcText_UnknownCode::
	.string "Unknown code. Check spelling.$"

EventScript_AccessPlayersPC::
	playse SE_PC_LOGIN
	msgbox gText_AccessedPlayersPC, MSGBOX_DEFAULT
	special PlayerPC
	goto EventScript_PCMainMenu
	end

EventScript_AccessPokemonStorage::
	playse SE_PC_LOGIN
	call_if_unset FLAG_SYS_PC_LANETTE, EventScript_AccessSomeonesPC
	call_if_set FLAG_SYS_PC_LANETTE, EventScript_AccessLanettesPC
	msgbox gText_StorageSystemOpened, MSGBOX_DEFAULT
	special ShowPokemonStorageSystemPC
	goto EventScript_PCMainMenu
	end

EventScript_AccessSomeonesPC::
	msgbox gText_AccessedSomeonesPC, MSGBOX_DEFAULT
	return

EventScript_AccessLanettesPC::
#if IS_FRLG
	msgbox gText_AccessedBillsPC, MSGBOX_DEFAULT
#else
	msgbox gText_AccessedLanettesPC, MSGBOX_DEFAULT
#endif
	return

EventScript_TurnOffPC::
	setvar VAR_0x8004, PC_LOCATION_OTHER
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	end

EventScript_AccessHallOfFame::
	goto_if_unset FLAG_SYS_GAME_CLEAR, EventScript_TurnOffPC
	playse SE_PC_LOGIN
	special AccessHallOfFamePC
	goto EventScript_AccessPC
	end

EventScript_AccessPokemonBoxLink::
	playse SE_PC_LOGIN
	msgbox gText_StorageSystemOpened, MSGBOX_DEFAULT
	special ShowPokemonStorageSystemPC
	goto EventScript_TurnOffPC
	end
