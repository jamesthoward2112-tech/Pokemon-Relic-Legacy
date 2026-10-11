SaffronCity_Dojo_Frlg_MapScripts::
	.byte 0

SaffronCity_Dojo_EventScript_TriggerMasterBattleLeft::
	lockall
	setvar VAR_MAP_SCENE_SAFFRON_CITY_DOJO, 1
	applymovement LOCALID_KARATE_MASTER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	releaseall
	end

SaffronCity_Dojo_EventScript_TriggerMasterBattleRight::
	lockall
	setvar VAR_MAP_SCENE_SAFFRON_CITY_DOJO, 1
	applymovement LOCALID_KARATE_MASTER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	releaseall
	end

@ The old FLAG_GOT_HITMON_FROM_DOJO was 0, so older ROMs never saved
@ which Hitmon was chosen. Reconcile once from caught records for
@ existing saves (including Pokémon deposited in the PC), after Koichi.
SaffronCity_Dojo_EventScript_EnsureGiftFlags::
	goto_if_set FLAG_PRL_DOJO_GIFT_MIGRATED, SaffronCity_Dojo_EventScript_EnsureGiftFlagsDone
	goto_if_defeated TRAINER_BLACK_BELT_KOICHI, SaffronCity_Dojo_EventScript_MigrateGiftFlags
	return

SaffronCity_Dojo_EventScript_MigrateGiftFlags::
	getcaughtmon SPECIES_HITMONLEE
	goto_if_eq VAR_RESULT, FALSE, SaffronCity_Dojo_EventScript_MigrateHitmonchan
	setflag FLAG_PRL_DOJO_HITMONLEE_CLAIMED
SaffronCity_Dojo_EventScript_MigrateHitmonchan::
	getcaughtmon SPECIES_HITMONCHAN
	goto_if_eq VAR_RESULT, FALSE, SaffronCity_Dojo_EventScript_MigrationDone
	setflag FLAG_PRL_DOJO_HITMONCHAN_CLAIMED
SaffronCity_Dojo_EventScript_MigrationDone::
	setflag FLAG_PRL_DOJO_GIFT_MIGRATED
SaffronCity_Dojo_EventScript_EnsureGiftFlagsDone::
	return

SaffronCity_Dojo_EventScript_HitmonleeBall::
	lock
	faceplayer
	goto_if_defeated TRAINER_BLACK_BELT_KOICHI, SaffronCity_Dojo_EventScript_CheckHitmonlee
	msgbox SaffronCity_Dojo_Text_DefeatMasterFirst
	release
	end

SaffronCity_Dojo_EventScript_CheckHitmonlee::
	call SaffronCity_Dojo_EventScript_EnsureGiftFlags
	goto_if_set FLAG_PRL_DOJO_HITMONLEE_CLAIMED, SaffronCity_Dojo_EventScript_AlreadyGotHitmon
	goto_if_unset FLAG_PRL_DOJO_HITMONCHAN_CLAIMED, SaffronCity_Dojo_EventScript_OfferHitmonlee
	goto_if_set FLAG_PRL_BADGE_KANTO_SABRINA, SaffronCity_Dojo_EventScript_OfferHitmonlee
	goto SaffronCity_Dojo_EventScript_SecondHitmonLocked

SaffronCity_Dojo_EventScript_OfferHitmonlee::
	showmonpic SPECIES_HITMONLEE, 10, 3
	setvar VAR_TEMP_1, SPECIES_HITMONLEE
	applymovement LOCALID_KARATE_MASTER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	textcolor NPC_TEXT_COLOR_MALE
	msgbox SaffronCity_Dojo_Text_YouWantHitmonlee, MSGBOX_YESNO
	call EventScript_RestorePrevTextColor
	goto_if_eq VAR_RESULT, YES, SaffronCity_Dojo_EventScript_GiveHitmon
	hidemonpic
	release
	end

SaffronCity_Dojo_EventScript_HitmonchanBall::
	lock
	faceplayer
	goto_if_defeated TRAINER_BLACK_BELT_KOICHI, SaffronCity_Dojo_EventScript_CheckHitmonchan
	msgbox SaffronCity_Dojo_Text_DefeatMasterFirst
	release
	end

SaffronCity_Dojo_EventScript_CheckHitmonchan::
	call SaffronCity_Dojo_EventScript_EnsureGiftFlags
	goto_if_set FLAG_PRL_DOJO_HITMONCHAN_CLAIMED, SaffronCity_Dojo_EventScript_AlreadyGotHitmon
	goto_if_unset FLAG_PRL_DOJO_HITMONLEE_CLAIMED, SaffronCity_Dojo_EventScript_OfferHitmonchan
	goto_if_set FLAG_PRL_BADGE_KANTO_SABRINA, SaffronCity_Dojo_EventScript_OfferHitmonchan
	goto SaffronCity_Dojo_EventScript_SecondHitmonLocked

SaffronCity_Dojo_EventScript_OfferHitmonchan::
	showmonpic SPECIES_HITMONCHAN, 10, 3
	setvar VAR_TEMP_1, SPECIES_HITMONCHAN
	applymovement LOCALID_KARATE_MASTER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	textcolor NPC_TEXT_COLOR_MALE
	msgbox SaffronCity_Dojo_Text_YouWantHitmonchan, MSGBOX_YESNO
	call EventScript_RestorePrevTextColor
	goto_if_eq VAR_RESULT, YES, SaffronCity_Dojo_EventScript_GiveHitmon
	hidemonpic
	release
	end

SaffronCity_Dojo_EventScript_AlreadyGotHitmon::
	msgbox SaffronCity_Dojo_Text_AlreadyClaimedThisHitmon
	release
	end

SaffronCity_Dojo_EventScript_SecondHitmonLocked::
	msgbox SaffronCity_Dojo_Text_SecondHitmonRequiresSabrina
	release
	end

SaffronCity_Dojo_EventScript_GiveHitmon::
	hidemonpic
	givemon VAR_TEMP_1, 25
	goto_if_eq VAR_RESULT, 0, SaffronCity_Dojo_EventScript_ReceivedHitmonParty
	goto_if_eq VAR_RESULT, 1, SaffronCity_Dojo_EventScript_ReceivedHitmonPC
	goto_if_eq VAR_RESULT, 2, Common_EventScript_NoMoreRoomForPokemon
	release
	end

SaffronCity_Dojo_EventScript_RecordHitmonGift::
	goto_if_eq VAR_TEMP_1, SPECIES_HITMONLEE, SaffronCity_Dojo_EventScript_RecordHitmonlee
	setflag FLAG_PRL_DOJO_HITMONCHAN_CLAIMED
	return
SaffronCity_Dojo_EventScript_RecordHitmonlee::
	setflag FLAG_PRL_DOJO_HITMONLEE_CLAIMED
	return

SaffronCity_Dojo_EventScript_ReceivedHitmonParty::
	removeobject VAR_LAST_TALKED
	bufferspeciesname STR_VAR_1, VAR_TEMP_1
	playfanfare MUS_LEVEL_UP
	message SaffronCity_Dojo_Text_ReceivedMonFromKarateMaster
	waitmessage
	waitfanfare
	call SaffronCity_Dojo_EventScript_RecordHitmonGift
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SaffronCity_Dojo_EventScript_EndGiveMon
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto SaffronCity_Dojo_EventScript_EndGiveMon
	end

SaffronCity_Dojo_EventScript_ReceivedHitmonPC::
	removeobject VAR_LAST_TALKED
	bufferspeciesname STR_VAR_1, VAR_TEMP_1
	playfanfare MUS_LEVEL_UP
	message SaffronCity_Dojo_Text_ReceivedMonFromKarateMaster
	waitmessage
	waitfanfare
	call SaffronCity_Dojo_EventScript_RecordHitmonGift
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SaffronCity_Dojo_EventScript_TransferredHitmonToPC
	call Common_EventScript_NameReceivedBoxMon
	goto SaffronCity_Dojo_EventScript_TransferredHitmonToPC
	end

SaffronCity_Dojo_EventScript_TransferredHitmonToPC::
	call Common_EventScript_TransferredToPC
	goto SaffronCity_Dojo_EventScript_EndGiveMon
	end

SaffronCity_Dojo_EventScript_EndGiveMon::
	release
	end

SaffronCity_Dojo_EventScript_Statue::
	msgbox SaffronCity_Dojo_Text_FightingDojo, MSGBOX_SIGN
	end

SaffronCity_Dojo_EventScript_LeftScroll::
	msgbox SaffronCity_Dojo_Text_EnemiesOnEverySide, MSGBOX_SIGN
	end

SaffronCity_Dojo_EventScript_RightScroll::
	msgbox SaffronCity_Dojo_Text_GoesAroundComesAround, MSGBOX_SIGN
	end

SaffronCity_Dojo_EventScript_Hitoshi::
	trainerbattle_single TRAINER_BLACK_BELT_HITOSHI, SaffronCity_Dojo_Text_HitoshiIntro, SaffronCity_Dojo_Text_HitoshiDefeat
	msgbox SaffronCity_Dojo_Text_HitoshiPostBattle, MSGBOX_AUTOCLOSE
	goto SaffronCity_Dojo_EventScript_PowerUpPunch
	end

SaffronCity_Dojo_EventScript_PowerUpPunch::
	lock
	faceplayer
	goto EventScript_PRL_TutorPowerUpPunch
	end

SaffronCity_Dojo_EventScript_Hideki::
	trainerbattle_single TRAINER_BLACK_BELT_HIDEKI, SaffronCity_Dojo_Text_HidekiIntro, SaffronCity_Dojo_Text_HidekiDefeat
	msgbox SaffronCity_Dojo_Text_HidekiPostBattle, MSGBOX_AUTOCLOSE
	goto SaffronCity_Dojo_EventScript_IcePunch
	end

SaffronCity_Dojo_EventScript_IcePunch::
	lock
	faceplayer
	goto EventScript_PRL_TutorIcePunch
	end

SaffronCity_Dojo_EventScript_Aaron::
	trainerbattle_single TRAINER_BLACK_BELT_AARON, SaffronCity_Dojo_Text_AaronIntro, SaffronCity_Dojo_Text_AaronDefeat
	msgbox SaffronCity_Dojo_Text_AaronPostBattle, MSGBOX_AUTOCLOSE
	goto SaffronCity_Dojo_EventScript_BlazeKick
	end

SaffronCity_Dojo_EventScript_BlazeKick::
	lock
	faceplayer
	goto EventScript_PRL_TutorBlazeKick
	end

SaffronCity_Dojo_EventScript_Mike::
	trainerbattle_single TRAINER_BLACK_BELT_MIKE, SaffronCity_Dojo_Text_MikeIntro, SaffronCity_Dojo_Text_MikeDefeat
	msgbox SaffronCity_Dojo_Text_MikePostBattle, MSGBOX_AUTOCLOSE
	goto SaffronCity_Dojo_EventScript_HighJumpKick
	end

SaffronCity_Dojo_EventScript_HighJumpKick::
	lock
	faceplayer
	goto EventScript_PRL_TutorHighJumpKick
	end

SaffronCity_Dojo_EventScript_MasterKoichi::
	trainerbattle_single TRAINER_BLACK_BELT_KOICHI, SaffronCity_Dojo_Text_MasterKoichiIntro, SaffronCity_Dojo_Text_MasterKoichiDefeat, SaffronCity_Dojo_EventScript_DefeatedMasterKoichi
	call SaffronCity_Dojo_EventScript_EnsureGiftFlags
	goto_if_set FLAG_PRL_DOJO_HITMONLEE_CLAIMED, SaffronCity_Dojo_EventScript_KoichiAfterFirst
	goto_if_set FLAG_PRL_DOJO_HITMONCHAN_CLAIMED, SaffronCity_Dojo_EventScript_KoichiAfterFirst
	msgbox SaffronCity_Dojo_Text_ChoosePrizedFightingMon
	release
	end

SaffronCity_Dojo_EventScript_KoichiAfterFirst::
	goto_if_unset FLAG_PRL_DOJO_HITMONLEE_CLAIMED, SaffronCity_Dojo_EventScript_KoichiSecondPending
	goto_if_set FLAG_PRL_DOJO_HITMONCHAN_CLAIMED, SaffronCity_Dojo_EventScript_MasterKoichiAlreadyGotHitmon
SaffronCity_Dojo_EventScript_KoichiSecondPending::
	goto_if_set FLAG_PRL_BADGE_KANTO_SABRINA, SaffronCity_Dojo_EventScript_KoichiSecondReady
	msgbox SaffronCity_Dojo_Text_SecondHitmonRequiresSabrina
	release
	end

SaffronCity_Dojo_EventScript_KoichiSecondReady::
	msgbox SaffronCity_Dojo_Text_SecondHitmonUnlocked
	release
	end

SaffronCity_Dojo_EventScript_MasterKoichiAlreadyGotHitmon::
	msgbox SaffronCity_Dojo_Text_StayAndTrainWithUs
	release
	end

SaffronCity_Dojo_EventScript_DefeatedMasterKoichi::
	setvar VAR_MAP_SCENE_SAFFRON_CITY_DOJO, 1
	release
	end

SaffronCity_Dojo_Text_MasterKoichiIntro::
	.string "Grunt!\p"
	.string "I am the KARATE MASTER!\n"
	.string "I am the LEADER here!\p"
	.string "You wish to challenge us?\n"
	.string "Expect no mercy!\p"
	.string "Fwaaa!$"

SaffronCity_Dojo_Text_MasterKoichiDefeat::
	.string "Hwa!\n"
	.string "Arrgh! Beaten!$"

SaffronCity_Dojo_Text_ChoosePrizedFightingMon::
	.string "Indeed, I have lost!\p"
	.string "But, I beseech you, do not take\n"
	.string "our emblem as your trophy!\p"
	.string "In return, I will give you a prized\n"
	.string "FIGHTING POKéMON!\p"
	.string "Choose whichever one you like!\p"
	.string "Beat SABRINA in the GYM next door,\n"
	.string "and the other is yours, too!$"

SaffronCity_Dojo_Text_StayAndTrainWithUs::
	.string "Grunt!\n"
	.string "Stay and train at karate with us!$"

SaffronCity_Dojo_Text_DefeatMasterFirst::
	.string "The KARATE MASTER must approve\n"
	.string "your choice first.$"
SaffronCity_Dojo_Text_SecondHitmonRequiresSabrina::
	.string "You've earned one of our prized\n"
	.string "FIGHTING POKéMON.\p"
	.string "Defeat SABRINA at the GYM next\n"
	.string "door, then return for the other!$"
SaffronCity_Dojo_Text_SecondHitmonUnlocked::
	.string "You defeated SABRINA!\p"
	.string "Our second prized FIGHTING\n"
	.string "POKéMON is yours. Take it!$"
SaffronCity_Dojo_Text_AlreadyClaimedThisHitmon::
	.string "You've already claimed this\n"
	.string "FIGHTING POKéMON!$"

SaffronCity_Dojo_Text_MikeIntro::
	.string "Hoargh!\n"
	.string "Take your shoes off!$"

SaffronCity_Dojo_Text_MikeDefeat::
	.string "I give up!$"

SaffronCity_Dojo_Text_MikePostBattle::
	.string "You wait 'til you see our MASTER!\n"
	.string "I'm a small fry compared to him!$"

SaffronCity_Dojo_Text_HidekiIntro::
	.string "I hear you're good!\n"
	.string "Show me!$"

SaffronCity_Dojo_Text_HidekiDefeat::
	.string "Judge!\n"
	.string "One point!$"

SaffronCity_Dojo_Text_HidekiPostBattle::
	.string "Our MASTER is a pro fighter.\n"
	.string "Be prepared to lose!$"

SaffronCity_Dojo_Text_AaronIntro::
	.string "Hiyah!\n"
	.string "I have no fear for anything tough!\p"
	.string "I shatter boulders for training\n"
	.string "every day!$"

SaffronCity_Dojo_Text_AaronDefeat::
	.string "Yow!\n"
	.string "Stubbed fingers!$"

SaffronCity_Dojo_Text_AaronPostBattle::
	.string "The only thing that frightens us is\n"
	.string "psychic power!$"

SaffronCity_Dojo_Text_HitoshiIntro::
	.string "Hwaah!\p"
	.string "You're trespassing in our\n"
	.string "FIGHTING DOJO!$"

SaffronCity_Dojo_Text_HitoshiDefeat::
	.string "Oof!\n"
	.string "I give up!$"

SaffronCity_Dojo_Text_HitoshiPostBattle::
	.string "The prime fighters across the land\n"
	.string "train here.$"

SaffronCity_Dojo_Text_YouWantHitmonlee::
	.string "You want the hard-kicking\n"
	.string "HITMONLEE?$"

SaffronCity_Dojo_Text_ReceivedMonFromKarateMaster::
	.string "{PLAYER} received the {STR_VAR_1}\n"
	.string "from the KARATE MASTER.$"

SaffronCity_Dojo_Text_YouWantHitmonchan::
	.string "You want the piston-punching\n"
	.string "HITMONCHAN?$"

SaffronCity_Dojo_Text_ReceivedMonFromKarateMaster2::
	.string "{PLAYER}は　カラテ　だいおう　から\n"
	.string "{STR_VAR_1}を　もらった！$"

SaffronCity_Dojo_Text_BetterNotGetGreedy::
	.string "Better not get greedy…$"

SaffronCity_Dojo_Text_EnemiesOnEverySide::
	.string "Enemies on every side!$"

SaffronCity_Dojo_Text_GoesAroundComesAround::
	.string "What goes around comes around.$"

SaffronCity_Dojo_Text_FightingDojo::
	.string "FIGHTING DOJO$"

