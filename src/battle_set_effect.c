#include "global.h"
#include "battle.h"
#include "battle_set_effect.h"
#include "battle_util.h"
#include "battle_script_commands.h"
#include "battle_hold_effects.h"
#include "battle_message.h"
#include "battle_ai_record.h"
#include "battle_scripts.h"
#include "battle_environment.h"
#include "battle_stat_change.h"
#include "battle_controllers.h"
#include "battle_dynamax.h"
#include "battle_gimmick.h"
#include "battle_terastal.h"
#include "item.h"
#include "pokemon.h"
#include "util.h"
#include "move.h"
#include "random.h"
#include "string_util.h"
#include "config/battle.h"

static inline bool32 IgnoreTargetingForMoveEffect(enum MoveEffect moveEffect);
static bool32 DoesSubstituteBlockMoveEffectOnTarget(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum MoveEffect moveEffect);
static bool32 IsFinalStrikeEffect(enum MoveEffect moveEffect);

static void HandleSetEffectNone(struct BattleCalcValues *cv, struct SetEffect *se)
{
    gBattlescriptCurrInstr = se->script;

    assertf(se->moveEffect != MOVE_EFFECT_FLORAL_HEALING, "no effect assigned to MOVE_EFFECT_FLORAL_HEALING");
}

static void HandleSetEffectNonVolatile(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (IsSafeguardProtected(cv->battlerAtk, se->effectBattler, cv->abilities[cv->battlerAtk]) && !se->primary)
    {
        gBattlescriptCurrInstr = se->script;
    }
    else if (CanSetNonVolatileStatus(
                cv->battlerAtk,
                se->effectBattler,
                cv->abilities[cv->battlerAtk],
                cv->abilities[se->effectBattler],
                se->moveEffect,
                CHECK_TRIGGER))
    {
        SetNonVolatileStatus(cv->battlerAtk, se->effectBattler, se->moveEffect, se->script, TRIGGER_ON_MOVE);
    }
    else
    {
        gBattlescriptCurrInstr = se->script;
    }
}

static void HandleSetEffectInfatuation(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (gBattleMons[se->effectBattler].volatiles.infatuation
     || !AreBattlersOfOppositeGender(cv->battlerAtk, se->effectBattler)
     || cv->abilities[se->effectBattler] == ABILITY_OBLIVIOUS
     || IsAbilityOnSide(se->effectBattler, ABILITY_AROMA_VEIL))
        gBattlescriptCurrInstr = se->script;
    else
    {
        gBattleMons[se->effectBattler].volatiles.infatuation = INFATUATED_WITH(cv->battlerAtk);
        BattleScriptPush(se->script);
        gBattlescriptCurrInstr = BattleScript_MoveEffectInfatuation;
    }
}

static void HandleSetEffectCurse(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (gBattleMons[se->effectBattler].volatiles.cursed)
        gBattlescriptCurrInstr = se->script;
    else
    {
        gBattleMons[se->effectBattler].volatiles.cursed = TRUE;
        BattleScriptPush(se->script);
        gBattlescriptCurrInstr = BattleScript_MoveEffectCurse;
    }
}

static void HandleSetEffectFear(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (gBattleMons[se->effectBattler].volatiles.fearTimer != 0)
    {
        gBattlescriptCurrInstr = se->script;
        return;
    }

    gBattleMons[se->effectBattler].volatiles.fearTimer = 2;
    BattleScriptPush(se->script);
    gBattlescriptCurrInstr = BattleScript_MoveEffectFear;
}

static void HandleSetEffectConfusion(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (!CanBeConfused(cv->battlerAtk, se->effectBattler))
    {
        gBattlescriptCurrInstr = se->script;
    }
    else
    {
        gBattleMons[se->effectBattler].volatiles.confusionTimer = RandomUniform(RNG_CONFUSION_TURNS, 2, B_CONFUSION_TURNS); // 2-5 turns
        BattleScriptPush(se->script);
        gBattlescriptCurrInstr = BattleScript_MoveEffectConfusion;
    }
}

static void HandleSetEffectFlinch(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (cv->abilities[se->effectBattler] == ABILITY_INNER_FOCUS)
    {
        if (se->primary || se->certain)
        {
            gLastUsedAbility = ABILITY_INNER_FOCUS;
            gBattlerAbility = se->effectBattler;
            RecordAbilityBattle(se->effectBattler, ABILITY_INNER_FOCUS);
            gBattlescriptCurrInstr = se->script;
        }
        else
        {
            gBattlescriptCurrInstr = se->script;
        }
    }
    else if (gBattleMons[se->effectBattler].volatiles.flinched)
    {
        gBattlescriptCurrInstr = se->script;
    }
    else if (!HasBattlerActedThisTurn(se->effectBattler)
          && GetActiveGimmick(se->effectBattler) != GIMMICK_DYNAMAX)
    {
        gBattleMons[se->effectBattler].volatiles.flinched = TRUE;
        gBattlescriptCurrInstr = se->script;
    }
    else
    {
        gBattlescriptCurrInstr = se->script;
    }
}

static void HandleSetEffectAbsorb(struct BattleCalcValues *cv, struct SetEffect *se)
{
    if (gBattleStruct->moveDamage[cv->battlerDef] > 0
     && IsBattlerTurnDamaged(cv->battlerDef, INCLUDING_SUBSTITUTES)
     && IsBattlerAlive(cv->battlerAtk))
    {
        u32 absorbPercentage = se->additionalEffect->argument.absorbPercentage;

        assertf(absorbPercentage != 0, "Missing absorb percentage for %S", gMovesInfo[cv->move].name)
        {
            return;
        }

        s32 healAmount = (gBattleStruct->moveDamage[cv->battlerDef] * absorbPercentage / 100);
        healAmount = GetDrainedBigRootHp(cv->battlerAtk, healAmount);
        gEffectBattler = cv->battlerAtk;
        gBattlerAbility = gBattleScripting.battler = cv->battlerDef;

        if (cv->abilities[cv->battlerDef] == ABILITY_LIQUID_OOZE
         && (GetMoveEffect(cv->move)!= EFFECT_DREAM_EATER || GetConfig(B_DREAM_EATER_LIQUID_OOZE) >= GEN_5))
        {
            SetPassiveDamageAmount(cv->battlerAtk, healAmount);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABSORB_OOZE;
            BattleScriptPush(se->script);
            gBattlescriptCurrInstr = BattleScript_EffectAbsorbLiquidOoze;
        }
        else if (!IsBattlerAtMaxHp(cv->battlerAtk) || GetConfig(B_ABSORB_MESSAGE) < GEN_5)
        {
            SetHealAmount(cv->battlerAtk, healAmount);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABSORB;
            BattleScriptPush(se->script);
            gBattlescriptCurrInstr = BattleScript_EffectAbsorb;
        }
    }
}

static void HandleSetEffectRandomFromList(struct BattleCalcValues *cv, struct SetEffect *se)
{
    const enum MoveEffect *sRandomFromListEffects = se->additionalEffect->argument.randomMoveEffects;
    u32 validEffectCount = 0;

    while (validEffectCount < MAX_RANDOM_ADDITIONAL_EFFECTS && sRandomFromListEffects[validEffectCount] != MOVE_EFFECT_NONE)
    {
        validEffectCount++;
    }

    assertf(validEffectCount != 0, "Missing or empty randomMoveEffects array for move %S", gMovesInfo[gCurrentMove].name)
    {
        return;
    }

    u32 chosenMoveEffect = RandomUniform(RNG_RANDOM_FROM_LIST, 0, validEffectCount - 1);
    if (sRandomFromListEffects[chosenMoveEffect] == MOVE_EFFECT_BURN)
        gBattleStruct->triAttackBurn = TRUE;

    se->moveEffect = sRandomFromListEffects[chosenMoveEffec