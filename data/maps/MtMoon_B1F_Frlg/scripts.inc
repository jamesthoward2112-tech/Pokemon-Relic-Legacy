MtMoon_B1F_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, MtMoon_B1F_OnLoad_PRLJessieJames
	map_script MAP_SCRIPT_ON_TRANSITION, MtMoon_B1F_OnTransition_PRLJessieJames
	.byte 0

@ Reveal the duo after either fossil; leave them visible when already defeated.
MtMoon_B1F_OnLoad_PRLJessieJames::
	setflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B1F_OnLoad_PRLRocketDone
	goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B1F_OnLoad_PRLRocketAlreadyWon
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
MtMoon_B1F_OnLoad_PRLRocketDone::
	return
MtMoon_B1F_OnLoad_PRLRocketAlreadyWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	return

MtMoon_B1F_OnTransition_PRLJessieJames::
	goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON, MtMoon_B1F_OnTransition_PRLReturn
	goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B1F_OnTransition_PRLRocketWon
	goto MtMoon_B1F_OnTransition_PRLRocketShow
MtMoon_B1F_OnTransition_PRLRocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON
MtMoon_B1F_OnTransition_PRLRocketShow::
	clearflag FLAG_HIDE_PRL_ROCKET_MT_MOON
	addobject LOCALID_PRL_JESSIE_MT_MOON
	addobject LOCALID_PRL_JAMES_MT_MOON
	addobject LOCALID_PRL_MEOWTH_MT_MOON
MtMoon_B1F_OnTransition_PRLReturn::
	return

@ Ordinary line-of-sight trainers. Native trainer_see handles the !
@ and their walking approach when player is in the same column.
@ One shared trainer flag means only one double fight, never two rematches.
MtMoon_B1F_EventScript_PRL_Jessie::
	trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B2F_Text_PRL_RocketIntro, MtMoon_B2F_Text_PRL_RocketDefeat, MtMoon_B2F_Text_PRL_RocketNotEnoughMons, MtMoon_B1F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JESSIE_MT_MOON
	msgbox MtMoon_B1F_Text_PRL_JessiePostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B1F_EventScript_PRL_James::
	trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B2F_Text_PRL_RocketIntro, MtMoon_B2F_Text_PRL_RocketDefeat, MtMoon_B2F_Text_PRL_RocketNotEnoughMons, MtMoon_B1F_EventScript_PRL_RocketWon, localId=LOCALID_PRL_JAMES_MT_MOON
	msgbox MtMoon_B1F_Text_PRL_JamesPostBattle, MSGBOX_AUTOCLOSE
	end

MtMoon_B1F_EventScript_PRL_Meowth::
	lock
	faceplayer
	goto_if_defeated TRAINER_JESSIE_JAMES_MT_MOON, MtMoon_B1F_EventScript_PRL_MeowthAfter
	msgbox MtMoon_B1F_Text_PRL_MeowthBefore
	release
	end
MtMoon_B1F_EventScript_PRL_MeowthAfter::
	msgbox MtMoon_B1F_Text_PRL_MeowthAfter
	release
	end

@ Flag progression only; never remove the duo, so A-button dialogue works.
MtMoon_B1F_EventScript_PRL_RocketWon::
	setflag FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON
	msgbox MtMoon_B2F_Text_PRL_RocketPostBattle
	release
	end

MtMoon_B1F_Text_PRL_JessiePostBattle::
	.string "JESSIE: You may have won this time,\n"
	.string "but the BOSS wants every RELIC!\p"
	.string "We haven't finished our search.\n"
	.string "Count on that!$"

MtMoon_B1F_Text_PRL_JamesPostBattle::
	.string "JAMES: These ancient markings\n"
	.string "aren't like any fossils I've seen.\p"
	.string "We must report them to the BOSS.\n"
	.string "And keep an eye on you!$"

MtMoon_B1F_Text_PRL_MeowthBefore::
	.string "MEOWTH: Mrowr! Get ready, kid!$"

MtMoon_B1F_Text_PRL_MeowthAfter::
	.string "MEOWTH: We were supposed to win!$"
