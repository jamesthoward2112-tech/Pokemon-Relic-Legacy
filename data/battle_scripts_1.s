#include "config/general.h"
#include "config/battle.h"
#include "constants/global.h"
#include "constants/battle.h"
#include "constants/pokemon.h"
#include "constants/battle_arena.h"
#include "constants/battle_move_resolution.h"
#include "constants/battle_stat_change.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_anim.h"
#include "constants/battle_string_ids.h"
#include "constants/abilities.h"
#include "constants/hold_effects.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/game_stat.h"
#include "constants/trainers.h"
#include "constants/species.h"
#include "constants/config_changes.h"
	.include "asm/macros.inc"
	.include "asm/macros/battle_script.inc"
	.include "constants/constants.inc"

	.section script_data, "aw", %progbits

BattleScript_TryRevertWeatherform:
	setbyte gEffectBattler, 0
	sortbattlers
BattleScript_TryRevertWeatherformLoop:
	tryrevertweatherform
	addbyte gEffectBattler, 1
	jumpifbytenotequal gEffectBattler, gBattlersCount, BattleScript_TryRevertWeatherformLoop
	return

BattleScript_FickleBeamMessage::
	pause B_WAIT_TIME_SHORTEST
	printstring STRINGID_FICKLEBEAMDOUBLED
	waitmessage B_WAIT_TIME_LONG
	return

BattleScript_MagnitudeMessage::
	pause B_WAIT_TIME_SHORT
	printstring STRINGID_MAGNITUDESTRENGTH
	waitmessage B_WAIT_TIME_LONG
	return

BattleScript_Terastallization::
	@ TODO: no string prints in S/V, but right now this helps with clarity
	flushtextbox
	printstring STRINGID_PKMNSTORINGENERGY
	playanimation BS_ATTACKER, B_ANIM_TERA_CHARGE
	waitanimation
	applyterastallization
	playanimation BS_ATTACKER, B_ANIM_TERA_ACTIVATE
	waitanimation
	printstring STRINGID_PKMNTERASTALLIZEDINTO
	waitmessage B_WAIT_TIME_LONG
	end3

BattleScript_TeraFormChange::
	@ TODO: no string prints in S/V, but right now this helps with clarity
	flushtextbox
	printstring STRINGID_PKMNSTORINGENERGY
	handleformchange BS_ATTACKER, 0, FALSE @ Prevent species name from overriting type name
	handleformchange BS_ATTACKER, 1
	playanimation BS_ATTACKER, B_ANIM_TERA_CHARGE
	waitanimation
	applyterastallization
	playanimation BS_ATTACKER, B_ANIM_TERA_ACTIVATE
	waitanimation
	printstring STRINGID_PKMNTERASTALLIZEDINTO
	waitmessage B_WAIT_TIME_LONG
	switchinabilities BS_ATTACKER
	abilityonformchange BS_ATTACKER
	effectsafterformchange
	end3

BattleScript_EffectStatChange::
	attackcanceler
	trymovestatchanges
	jumpifnotmove MOVE_SCARY_FACE, BattleScript_EffectStatChangeEnd
	seteffectprimary BS_ATTACKER, BS_TARGET, MOVE_EFFECT_FEAR
BattleScript_EffectStatChangeEnd::
	goto BattleScript_MoveEnd

BattleScript_EffectStatChangeHalfHp::
	attackcanceler
	trymovestatchanges
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER, ASSURANCE_DOUBLE
	goto BattleScript_MoveEnd

BattleScript_PlayMoveAnim::
    playmoveanimation MOVE_NONE
	waitanimation
    return

BattleScript_StatChangeFailed::
	pause B_WAIT_TIME_SHORT
    printstring STRINGID_BUTITFAILED
	waitmessage B_WAIT_TIME_LONG
	goto BattleScript_MoveEnd

BattleScript_PlayMoveAnimAndChangeHP::
	call BattleScript_PlayMoveAnim
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER, ASSURANCE_IGNORE
    return

BattleScript_PlayTidyUp::
	call BattleScript_PlayMoveAnim
	trytidyup TRUE, NULL
	printstring STRINGID_TIDYINGUPCOMPLETE
	waitmessage B_WAIT_TIME_LONG
    return

BattleScript_EffectDefog::
	attackcanceler
	trymovestatchanges
	trydefog TRUE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectMemento::
	attackcanceler
	trymovestatchanges
    tryfaintmon BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_Memento::
	setatkhptozero
	attackanimation
	waitanimation
    return

BattleScript_TakeHeart::
	attackanimation
	waitanimation
	updatestatusicon BS_ATTACKER
	printfromtable gCureStatusStringIds
	waitmessage B_WAIT_TIME_LONG
    return

BattleScript_ToxicThread::
	seteffectprimary BS_ATTACKER, BS_SCRIPTING, MOVE_EFFECT_POISON
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_SwaggerConfusion::
	seteffectprimary BS_ATTACKER, BS_SCRIPTING, MOVE_EFFECT_CONFUSION
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_SwaggerOwnTempoPrevents::
	call BattleScript_OwnTempoPreventsRet
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_NoRetreatMessage::
	printstring STRINGID_CANTESCAPEDUETOUSEDMOVE
	waitmessage B_WAIT_TIME_LONG
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_AutotomizeMessage::
	printstring STRINGID_BECAMENIMBLE
	waitmessage B_WAIT_TIME_LONG
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_TarShotMessage::
	printstring STRINGID_PKMNBECAMEWEAKERTOFIRE
	waitmessage B_WAIT_TIME_LONG
	trymovestatchanges
	goto BattleScript_MoveEnd

BattleScript_AbilityStatChange::
	call BattleScript_AbilityPopUp
	trystatchanges BS_EFFECT_BATTLER, STAT_CHANGE_IGNORE_SELF
	return

BattleScript_DefiantActivates::
	call BattleScript_AbilityPopUp
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_SECOND_QUEUE
	return

BattleScript_AdrenalineOrbActivates::
	call BattleScript_ItemPopUp_Scripting
	playanimation BS_SCRIPTING, B_ANIM_HELD_ITEM_EFFECT
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_SECOND_QUEUE
	removeitem BS_SCRIPTING
	return

BattleScript_MoveEffectStatChange::
	trystatchanges BS_ATTACKER, STAT_CHANGE_SILENT_FAILURE | STAT_CHANGE_IGNORE_SELF
	return

BattleScript_ItemStatChange::
	call BattleScript_ItemPopUp_Scripting
	playanimation BS_SCRIPTING, B_ANIM_HELD_ITEM_EFFECT
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_ITEM
	removeitem BS_SCRIPTING
	return

BattleScript_ConsumableBerryStatRaise::
	call BattleScript_ItemPopUp_Scripting
 	playanimation BS_SCRIPTING, B_ANIM_HELD_ITEM_BERRY
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_ITEM | STAT_CHANGE_CERTAIN
	removeitem BS_SCRIPTING
	return

BattleScript_ConsumableBerryStatRaiseRipen::
	call BattleScript_AbilityPopUp
	waitabilitypopup
	call BattleScript_ItemPopUp_Scripting
 	playanimation BS_SCRIPTING, B_ANIM_HELD_ITEM_BERRY
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_ITEM | STAT_CHANGE_CERTAIN
	removeitem BS_SCRIPTING
	return

BattleScript_ConsumableItemStatRaise::
	call BattleScript_ItemPopUp_Scripting
 	playanimation BS_SCRIPTING, B_ANIM_HELD_ITEM_EFFECT
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_ITEM | STAT_CHANGE_CERTAIN
	removeitem BS_SCRIPTING
	return

BattleScript_MirrorArmorReflect::
	pause B_WAIT_TIME_SHORT
	call BattleScript_AbilityPopUp
	trybattlerstatchange BS_SCRIPTING, STAT_CHANGE_SECOND_QUEUE | STAT_CHANGE_IGNORE_MIRROR_ARMOR
	return

BattleScript_EndTurnStatChange::
	trystatchanges BS_ATTACKER, STAT_CHANGE_IGNORE_MIRROR_ARMOR
	return

BattleScript_IncreaseStatChangeMessage::
	printfromtable gStatUpStringIds
	waitmessage B_WAIT_TIME_LONG
	tryadrenalineorb
	return

BattleScript_DecreaseStatChangeMessage::
	printfromtable gStatDownStringIds
	waitmessage B_WAIT_TIME_LONG
	trydefiantrattled
	tryadrenalineorb
	return

BattleScript_DecreaseStatChangeMessageMinStat::
	printfromtable gStatDownStringIds
	waitmessage B_WAIT_TIME_LONG
	return

BattleScript_StatDidntChangeMessag