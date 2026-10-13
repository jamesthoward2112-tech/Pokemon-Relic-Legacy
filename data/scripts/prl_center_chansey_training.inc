@ PRL repeatable Chansey training battles at the former upstairs Move Deleter desk.
@ Distinct location teams, no defeated-flag persistence, preserves save layout.

EventScript_PRL_Chansey_VIRIDIAN::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_VIRIDIAN
	trainerbattle_single TRAINER_PRL_CHANSEY_VIRIDIAN, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_VIRIDIAN_After
	goto EventScript_PRL_Chansey_VIRIDIAN_After
EventScript_PRL_Chansey_VIRIDIAN_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_VIRIDIAN
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_PEWTER::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_PEWTER
	trainerbattle_single TRAINER_PRL_CHANSEY_PEWTER, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_PEWTER_After
	goto EventScript_PRL_Chansey_PEWTER_After
EventScript_PRL_Chansey_PEWTER_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_PEWTER
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_ROUTE4::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_ROUTE4
	trainerbattle_single TRAINER_PRL_CHANSEY_ROUTE4, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_ROUTE4_After
	goto EventScript_PRL_Chansey_ROUTE4_After
EventScript_PRL_Chansey_ROUTE4_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_ROUTE4
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_CERULEAN::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_CERULEAN
	trainerbattle_single TRAINER_PRL_CHANSEY_CERULEAN, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_CERULEAN_After
	goto EventScript_PRL_Chansey_CERULEAN_After
EventScript_PRL_Chansey_CERULEAN_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_CERULEAN
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_VERMILION::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_VERMILION
	trainerbattle_single TRAINER_PRL_CHANSEY_VERMILION, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_VERMILION_After
	goto EventScript_PRL_Chansey_VERMILION_After
EventScript_PRL_Chansey_VERMILION_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_VERMILION
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_ROUTE10::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_ROUTE10
	trainerbattle_single TRAINER_PRL_CHANSEY_ROUTE10, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_ROUTE10_After
	goto EventScript_PRL_Chansey_ROUTE10_After
EventScript_PRL_Chansey_ROUTE10_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_ROUTE10
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_LAVENDER::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_LAVENDER
	trainerbattle_single TRAINER_PRL_CHANSEY_LAVENDER, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_LAVENDER_After
	goto EventScript_PRL_Chansey_LAVENDER_After
EventScript_PRL_Chansey_LAVENDER_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_LAVENDER
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_CELADON::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_CELADON
	trainerbattle_single TRAINER_PRL_CHANSEY_CELADON, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_CELADON_After
	goto EventScript_PRL_Chansey_CELADON_After
EventScript_PRL_Chansey_CELADON_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_CELADON
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_SAFFRON::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_SAFFRON
	trainerbattle_single TRAINER_PRL_CHANSEY_SAFFRON, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_SAFFRON_After
	goto EventScript_PRL_Chansey_SAFFRON_After
EventScript_PRL_Chansey_SAFFRON_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_SAFFRON
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_FUCHSIA::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_FUCHSIA
	trainerbattle_single TRAINER_PRL_CHANSEY_FUCHSIA, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_FUCHSIA_After
	goto EventScript_PRL_Chansey_FUCHSIA_After
EventScript_PRL_Chansey_FUCHSIA_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_FUCHSIA
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_CINNABAR::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_CINNABAR
	trainerbattle_single TRAINER_PRL_CHANSEY_CINNABAR, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_CINNABAR_After
	goto EventScript_PRL_Chansey_CINNABAR_After
EventScript_PRL_Chansey_CINNABAR_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_CINNABAR
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_INDIGO::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_INDIGO
	trainerbattle_single TRAINER_PRL_CHANSEY_INDIGO, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_INDIGO_After
	goto EventScript_PRL_Chansey_INDIGO_After
EventScript_PRL_Chansey_INDIGO_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_INDIGO
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_ONE_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_ONE_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_ONE_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_ONE_ISLAND_After
	goto EventScript_PRL_Chansey_ONE_ISLAND_After
EventScript_PRL_Chansey_ONE_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_ONE_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_TWO_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_TWO_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_TWO_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_TWO_ISLAND_After
	goto EventScript_PRL_Chansey_TWO_ISLAND_After
EventScript_PRL_Chansey_TWO_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_TWO_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_THREE_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_THREE_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_THREE_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_THREE_ISLAND_After
	goto EventScript_PRL_Chansey_THREE_ISLAND_After
EventScript_PRL_Chansey_THREE_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_THREE_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_FOUR_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_FOUR_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_FOUR_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_FOUR_ISLAND_After
	goto EventScript_PRL_Chansey_FOUR_ISLAND_After
EventScript_PRL_Chansey_FOUR_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_FOUR_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_FIVE_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_FIVE_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_FIVE_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_FIVE_ISLAND_After
	goto EventScript_PRL_Chansey_FIVE_ISLAND_After
EventScript_PRL_Chansey_FIVE_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_FIVE_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_SIX_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_SIX_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_SIX_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_SIX_ISLAND_After
	goto EventScript_PRL_Chansey_SIX_ISLAND_After
EventScript_PRL_Chansey_SIX_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_SIX_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

EventScript_PRL_Chansey_SEVEN_ISLAND::
	lock
	faceplayer
	msgbox PRL_Chansey_TextOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PRL_Chansey_Decline
	cleartrainerflag TRAINER_PRL_CHANSEY_SEVEN_ISLAND
	trainerbattle_single TRAINER_PRL_CHANSEY_SEVEN_ISLAND, PRL_Chansey_TextIntro, PRL_Chansey_TextDefeat, EventScript_PRL_Chansey_SEVEN_ISLAND_After
	goto EventScript_PRL_Chansey_SEVEN_ISLAND_After
EventScript_PRL_Chansey_SEVEN_ISLAND_After::
	cleartrainerflag TRAINER_PRL_CHANSEY_SEVEN_ISLAND
	msgbox PRL_Chansey_TextAfter, MSGBOX_DEFAULT
	release
	end

PRL_Chansey_Decline::
	msgbox PRL_Chansey_TextDecline, MSGBOX_DEFAULT
	release
	end

PRL_Chansey_TextOffer::
	.string "Train against six CHANSEY?\n"
	.string "They only know two moves!$"

PRL_Chansey_TextIntro::
	.string "Time for Chansey training!$"

PRL_Chansey_TextDefeat::
	.string "A great training session!$"

PRL_Chansey_TextAfter::
	.string "Back for more EXP?\n"
	.string "I'll battle you again any time!$"

PRL_Chansey_TextDecline::
	.string "Come back when you're ready!$"
