ViridianCity_Gym_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ViridianCity_Gym_OnTransition_PRLBadgeRecovery
	.byte 0

ViridianCity_Gym_OnTransition_PRLBadgeRecovery::
	goto_if_unset FLAG_PRL_BADGE_HEIST_STARTED, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_BOULDER, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_CASCADE, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_THUNDER, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_RAINBOW, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_SOUL, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_MARSH, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_VOLCANO, ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone
	clearflag FLAG_HIDE_VIRIDIAN_GIOVANNI
	addobject LOCALID_VIRIDIAN_GIOVANNI
ViridianCity_Gym_EventScript_PRLGiovanniVisibilityDone::
	return

ViridianCity_Gym_EventScript_Giovanni::
	call ViridianCity_Gym_Frlg_EventScript_PRLBadgeRewardPending
	famechecker FAMECHECKER_GIOVANNI, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
	goto_if_unset FLAG_PRL_BADGE_HEIST_STARTED, ViridianCity_Gym_EventScript_GiovanniNormal
	goto_if_set FLAG_PRL_BADGE_HEIST_RECOVERED_EARTH, ViridianCity_Gym_EventScript_GiovanniNormal
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_BOULDER, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_CASCADE, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_THUNDER, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_RAINBOW, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_SOUL, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_MARSH, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_VOLCANO, ViridianCity_Gym_EventScript_PRLGiovanniNotReady
	goto ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecovery
ViridianCity_Gym_EventScript_GiovanniNormal::
	trainerbattle_single TRAINER_LEADER_GIOVANNI, ViridianCity_Gym_Text_GiovanniIntro, ViridianCity_Gym_Text_GiovanniDefeat, ViridianCity_Gym_EventScript_DefeatedGiovanni, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, ViridianCity_Gym_EventScript_GiovanniRematch
	call_if_unset FLAG_PRL_GYM_KANTO_GIOVANNI_EXTRA, PRL_EventScript_GymExtra_KANTO_GIOVANNI
	goto_if_unset FLAG_GOT_TM26_FROM_GIOVANNI, ViridianCity_Gym_EventScript_GiveTM26
	msgbox ViridianCity_Gym_Text_GiovanniPostBattle
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_VIRIDIAN_GIOVANNI
	fadescreen FADE_FROM_BLACK
	release
	end

ViridianCity_Gym_EventScript_PRLGiovanniNotReady::
	msgbox ViridianCity_Gym_Text_PRLGiovanniNotReady
	release
	end

ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecovery::
	trainerbattle_single TRAINER_PRL_BADGE_RECOVERY_GIOVANNI, ViridianCity_Gym_Text_PRLRecoveryIntro, ViridianCity_Gym_Text_PRLRecoveryDefeat, ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecoveryWon, NO_MUSIC
	msgbox ViridianCity_Gym_Text_PRLRecoveryAfter
	release
	end
ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecoveryWon::
	setflag FLAG_PRL_BADGE_HEIST_RECOVERED_EARTH
	setflag FLAG_PRL_BADGE_HEIST_ALL_RECOVERED
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox ViridianCity_Gym_Text_PRLRecoveryWon
	call PRL_EventScript_BadgeHeistReward_GIOVANNI
	release
	end

ViridianCity_Gym_Text_PRLGiovanniNotReady::
	.string "GIOVANNI: Reclaim the other seven\n"
	.string "Badges before you challenge me.$"
ViridianCity_Gym_Text_PRLRecoveryIntro::
	.string "So, you've reclaimed the other\n"
	.string "seven Badges? Impressive.\p"
	.string "But the EARTHBADGE will not be\n"
	.string "so easily recovered.$"
ViridianCity_Gym_Text_PRLRecoveryDefeat::
	.string "Your command is beyond question.$"
ViridianCity_Gym_Text_PRLRecoveryWon::
	.string "You recovered the EARTHBADGE!\p"
	.string "All eight stolen Badges have been\n"
	.string "reclaimed!$"
ViridianCity_Gym_Text_PRLRecoveryAfter::
	.string "GIOVANNI: Go. Settle this with\n"
	.string "those troublesome agents.$"

ViridianCity_Gym_EventScript_GiovanniRematch::
	trainerbattle_rematch TRAINER_LEADER_GIOVANNI, ViridianCity_Gym_Text_GiovanniIntro, ViridianCity_Gym_Text_GiovanniDefeat
	msgbox ViridianCity_Gym_Text_GiovanniPostBattle
	release
	end

ViridianCity_Gym_EventScript_DefeatedGiovanni::
	famechecker FAMECHECKER_GIOVANNI, 3
	setflag FLAG_HIDE_MISC_KANTO_ROCKETS
	setflag FLAG_DEFEATED_LEADER_GIOVANNI
	setflag FLAG_BADGE08_GET
	setflag FLAG_PRL_BADGE_KANTO_GIOVANNI
	call_if_unset FLAG_PRL_GYM_KANTO_GIOVANNI_EXTRA, PRL_EventScript_GymExtra_KANTO_GIOVANNI
	setvar VAR_MAP_SCENE_ROUTE22, 3
	set_gym_trainers_frlg 8
	goto ViridianCity_Gym_EventScript_GiveTM26
	end

ViridianCity_Gym_EventScript_GiveTM26::
	msgbox ViridianCity_Gym_Text_ExplainEarthBadgeTakeThis
	checkitemspace ITEM_TM26
	goto_if_eq VAR_RESULT, FALSE, ViridianCity_Gym_EventScript_NoRoomForTM26
	giveitem_msg ViridianCity_Gym_Text_ReceivedTM26FromGiovanni, ITEM_TM26
	setflag FLAG_GOT_TM26_FROM_GIOVANNI
	msgbox ViridianCity_Gym_Text_ExplainTM26
	release
	end

ViridianCity_Gym_EventScript_NoRoomForTM26::
	msgbox ViridianCity_Gym_Text_YouDoNotHaveSpace
	release
	end

ViridianCity_Gym_EventScript_Jason::
	trainerbattle_single TRAINER_TAMER_JASON, ViridianCity_Gym_Text_JasonIntro, ViridianCity_Gym_Text_JasonDefeat
	msgbox ViridianCity_Gym_Text_JasonPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Cole::
	trainerbattle_single TRAINER_TAMER_COLE, ViridianCity_Gym_Text_ColeIntro, ViridianCity_Gym_Text_ColeDefeat
	msgbox ViridianCity_Gym_Text_ColePostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Atsushi::
	trainerbattle_single TRAINER_BLACK_BELT_ATSUSHI, ViridianCity_Gym_Text_AtsushiIntro, ViridianCity_Gym_Text_AtsushiDefeat
	msgbox ViridianCity_Gym_Text_AtsushiPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Kiyo::
	trainerbattle_single TRAINER_BLACK_BELT_KIYO, ViridianCity_Gym_Text_KiyoIntro, ViridianCity_Gym_Text_KiyoDefeat
	msgbox ViridianCity_Gym_Text_KiyoPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Takashi::
	trainerbattle_single TRAINER_BLACK_BELT_TAKASHI, ViridianCity_Gym_Text_TakashiIntro, ViridianCity_Gym_Text_TakashiDefeat
	msgbox ViridianCity_Gym_Text_TakashiPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Samuel::
	trainerbattle_single TRAINER_COOLTRAINER_SAMUEL, ViridianCity_Gym_Text_SamuelIntro, ViridianCity_Gym_Text_SamuelDefeat
	msgbox ViridianCity_Gym_Text_SamuelPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Yuji::
	trainerbattle_single TRAINER_COOLTRAINER_YUJI, ViridianCity_Gym_Text_YujiIntro, ViridianCity_Gym_Text_YujiDefeat
	msgbox ViridianCity_Gym_Text_YujiPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_Warren::
	trainerbattle_single TRAINER_COOLTRAINER_WARREN, ViridianCity_Gym_Text_WarrenIntro, ViridianCity_Gym_Text_WarrenDefeat
	msgbox ViridianCity_Gym_Text_WarrenPostBattle, MSGBOX_AUTOCLOSE
	end

ViridianCity_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_LEADER_GIOVANNI, ViridianCity_Gym_EventScript_GymGuyPostVictory
	msgbox ViridianCity_Gym_Text_GymGuyAdvice
	release
	end

ViridianCity_Gym_EventScript_GymGuyPostVictory::
	famechecker FAMECHECKER_GIOVANNI, 4
	msgbox ViridianCity_Gym_Text_GymGuyPostVictory
	release
	end

ViridianCity_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_PRL_BADGE_KANTO_GIOVANNI, ViridianCity_Gym_EventScript_GymStatuePostVictory
	msgbox ViridianCity_Gym_Text_GymStatue
	releaseall
	end

ViridianCity_Gym_EventScript_GymStatuePostVictory::
	lockall
	warpteleport MAP_VIRIDIAN_CITY_GYM, 2, 3
	releaseall
	end

ViridianCity_Gym_Text_GiovanniIntro::
	.string "GIOVANNI: Here we meet again.\p"
	.string "The fossils from MT. MOON,\n"
	.string "the lab at SILPH, the relics...\p"
	.string "All were steps toward an\n"
	.string "ancient power I sought.\p"
	.string "A bird of gold? A mere myth?\n"
	.string "I still don't know.\p"
	.string "But my fossil army is real.\n"
	.string "Now face its full force!$"

ViridianCity_Gym_Text_GiovanniDefeat::
	.string "Ha!\n"
	.string "That was a truly intense fight.\l"
	.string "You have won!\p"
	.string "As proof, here is the EARTHBADGE!\n"
	.string "{PAUSE_MUSIC}{PLAY_BGM}{MUS_OBTAIN_BADGE}{PAUSE 0xFE}{PAUSE 0x56}{RESUME_MUSIC}$"

ViridianCity_Gym_Text_GiovanniPostBattle::
	.string "My research. My fossil army.\n"
	.string "All brought down by you.\p"
	.string "I chased a legend, but lost\n"
	.string "sight of my own POKéMON.\p"
	.string "As of today, TEAM ROCKET\n"
	.string "is disbanded.\p"
	.string "The ancient mystery remains.\n"
	.string "It is yours to uncover now.$"

ViridianCity_Gym_Text_ExplainEarthBadgeTakeThis::
	.string "The EARTHBADGE makes POKéMON of\n"
	.string "any level obey without question.\p"
	.string "It is evidence of your mastery as\n"
	.string "a POKéMON TRAINER.\p"
	.string "With it, you can challenge the\n"
	.string "POKéMON LEAGUE.\p"
	.string "Also, take this TM.\p"
	.string "Consider it a gift for your POKéMON\n"
	.string "LEAGUE challenge.$"

ViridianCity_Gym_Text_ReceivedTM26FromGiovanni::
	.string "{PLAYER} received TM26\n"
	.string "from GIOVANNI.$"

ViridianCity_Gym_Text_ExplainTM26::
	.string "TM26 contains EARTHQUAKE.\p"
	.string "It is a powerful attack that causes\n"
	.string "a massive tremor.\p"
	.string "I made it when I ran the GYM here,\n"
	.string "far too long ago… $"

ViridianCity_Gym_Text_YouDoNotHaveSpace::
	.string "You do not have space for this!$"

ViridianCity_Gym_Text_YujiIntro::
	.string "Heh!\n"
	.string "You must be running out of steam\l"
	.string "by now!$"

ViridianCity_Gym_Text_YujiDefeat::
	.string "I ran out of gas!$"

ViridianCity_Gym_Text_YujiPostBattle::
	.string "The LEADER's fossil teams\n"
	.string "are stronger than ever.\p"
	.string "He says ancient strength\n"
	.string "is there for the taking!$"

ViridianCity_Gym_Text_AtsushiIntro::
	.string "Rrrroar!\n"
	.string "I'm working myself into a rage!$"

ViridianCity_Gym_Text_AtsushiDefeat::
	.string "Wargh!$"

ViridianCity_Gym_Text_AtsushiPostBattle::
	.string "I'm still not worthy!$"

ViridianCity_Gym_Text_JasonIntro::
	.string "POKéMON and I, we make wonderful\n"
	.string "music together!$"

ViridianCity_Gym_Text_JasonDefeat::
	.string "You are in perfect harmony!$"

ViridianCity_Gym_Text_JasonPostBattle::
	.string "Do you know the identity of our\n"
	.string "GYM LEADER?$"

ViridianCity_Gym_Text_KiyoIntro::
	.string "Karate is the ultimate form of\n"
	.string "martial arts!$"

ViridianCity_Gym_Text_KiyoDefeat::
	.string "Aiyah!$"

ViridianCity_Gym_Text_KiyoPostBattle::
	.string "If my POKéMON were as good at\n"
	.string "karate as I…$"

ViridianCity_Gym_Text_WarrenIntro::
	.string "The truly talented win with style.$"

ViridianCity_Gym_Text_WarrenDefeat::
	.string "I lost my grip!$"

ViridianCity_Gym_Text_WarrenPostBattle::
	.string "The LEADER demanded\n"
	.string "our fossil battle records.\p"
	.string "His search for ancient\n"
	.string "power has consumed him!$"

ViridianCity_Gym_Text_TakashiIntro::
	.string "I'm the KARATE KING!\n"
	.string "Your fate rests with me!$"

ViridianCity_Gym_Text_TakashiDefeat::
	.string "Ayah!$"

ViridianCity_Gym_Text_TakashiPostBattle::
	.string "The POKéMON LEAGUE?\n"
	.string "You? Don't get cocky!$"

ViridianCity_Gym_Text_ColeIntro::
	.string "Your POKéMON will cower at the\n"
	.string "crack of my whip!$"

ViridianCity_Gym_Text_ColeDefeat::
	.string "Yowch!\n"
	.string "Whiplash!$"

ViridianCity_Gym_Text_ColePostBattle::
	.string "Wait!\n"
	.string "I was just careless!$"

ViridianCity_Gym_Text_SamuelIntro::
	.string "VIRIDIAN GYM was closed for a\n"
	.string "long time.\p"
	.string "But now, our LEADER is back!$"

ViridianCity_Gym_Text_SamuelDefeat::
	.string "I was beaten?$"

ViridianCity_Gym_Text_SamuelPostBattle::
	.string "Our LEADER reopened the GYM\n"
	.string "after his RELIC operation\n"
	.string "at SILPH fell apart.\p"
	.string "Can you beat him in battle?$"

ViridianCity_Gym_Text_GymGuyAdvice::
	.string "Yo!\n"
	.string "Champ in the making!\p"
	.string "Even I don't know the VIRIDIAN\n"
	.string "LEADER's identity.\p"
	.string "But one thing's certain.\n"
	.string "This will be the toughest of all\l"
	.string "the GYM LEADERS.\p"
	.string "Also, I heard that the TRAINERS\n"
	.string "here like GROUND-type POKéMON.$"

ViridianCity_Gym_Text_GymGuyPostVictory::
	.string "Blow me away! GIOVANNI was the\n"
	.string "GYM LEADER of VIRIDIAN?$"

ViridianCity_Gym_Text_GymStatue::
	.string "VIRIDIAN POKéMON GYM\n"
	.string "LEADER: ?\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

ViridianCity_Gym_Text_GymStatuePlayerWon::
	.string "VIRIDIAN POKéMON GYM\n"
	.string "LEADER: GIOVANNI\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"


@ A recovered badge remains valid even while its full-Bag gift is pending.
ViridianCity_Gym_Frlg_EventScript_PRLBadgeRewardPending::
	goto_if_unset FLAG_PRL_BADGE_HEIST_RECOVERED_EARTH, ViridianCity_Gym_Frlg_EventScript_PRLBadgeRewardPendingDone
	goto_if_set FLAG_PRL_BADGE_HEIST_REWARD_GIOVANNI, ViridianCity_Gym_Frlg_EventScript_PRLBadgeRewardPendingDone
	lock
	faceplayer
	call PRL_EventScript_BadgeHeistReward_GIOVANNI
	release
	end
ViridianCity_Gym_Frlg_EventScript_PRLBadgeRewardPendingDone::
	return
