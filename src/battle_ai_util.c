#include "global.h"
#include "battle_z_move.h"
#include "malloc.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_ai_field_statuses.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_stat_change.h"
#include "battle_controllers.h"
#include "battle_factory.h"
#include "battle_setup.h"
#include "event_data.h"
#include "data.h"
#include "item.h"
#include "move.h"
#include "pokemon.h"
#include "random.h"
#include "recorded_battle.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/items.h"

static u32 GetAIEffectGroup(enum BattleMoveEffects effect);
static u32 GetAIEffectGroupFromMove(enum BattlerId battler, enum Move move);

// Functions
enum Ability AI_GetMoldBreakerSanitizedAbility(enum BattlerId battlerAtk, enum Ability abilityAtk, enum Ability abilityDef, enum HoldEffect holdEffectDef, enum Move move)
{
    if (MoveIgnoresTargetAbility(move))
        return ABILITY_NONE;

    if (holdEffectDef != HOLD_EFFECT_ABILITY_SHIELD && IsMoldBreakerTypeAbility(battlerAtk, abilityAtk, move))
        return ABILITY_NONE;

    return abilityDef;
}

static bool32 AI_IsDoubleSpreadMove(enum BattlerId battlerAtk, enum Move move)
{
    u32 numOfTargets = 0;
    enum MoveTarget moveTargetType = AI_GetBattlerMoveTargetType(battlerAtk, move);

    if (!IsDoubleBattle() || !IsSpreadMove(moveTargetType))
        return FALSE;

    for (enum BattlerId battlerDef = 0; battlerDef < MAX_BATTLERS_COUNT; battlerDef++)
    {
        if (battlerAtk == battlerDef || !IsBattlerAlive(battlerDef))
            continue;

        if (moveTargetType == TARGET_BOTH && battlerAtk == GetPartnerBattler(battlerDef))
            continue;

        if (!IsSemiInvulnerable(battlerDef, CHECK_ALL)
         || CanBreakThroughSemiInvulnerablity(battlerAtk, battlerDef, gAiLogicData->abilities[battlerAtk], gAiLogicData->abilities[battlerDef], move))
            numOfTargets++;
    }

    if (numOfTargets > 1)
        return TRUE;

    return FALSE;
}

bool32 AI_IsBattlerGrounded(enum BattlerId battler)
{
    return IsBattlerGrounded(battler, gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler]);
}

static bool32 AI_CanBattlerHitBothFoesInTerrain(enum BattlerId battler, enum Move move, enum BattleMoveEffects effect)
{
    return effect == EFFECT_TERRAIN_BOOST
        && GetMoveTerrainBoost_HitsBothFoes(move)
        && IsBattlerTerrainAffected(battler, gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler], GetMoveTerrainBoost_Terrain(move), gFieldTimers.terrain);
}

enum MoveTarget AI_GetBattlerMoveTargetType(enum BattlerId battler, enum Move move)
{
    enum BattleMoveEffects effect = GetMoveEffect(move);
    if (effect == EFFECT_CURSE && !IS_BATTLER_OF_TYPE(battler, TYPE_GHOST))
        return TARGET_USER;
    if (AI_CanBattlerHitBothFoesInTerrain(battler, move, effect))
        return TARGET_BOTH;
    if (effect == EFFECT_TERA_STARSTORM && gBattleMons[battler].species == SPECIES_TERAPAGOS_STELLAR)
        return TARGET_BOTH;

    return GetMoveTarget(move);
}

u32 AI_GetDefaultDamageRollForContext(enum BattlerId battlerAtk, enum BattlerId battlerDef, u32 moveIndex, struct AiLogicData *aiData, u32 aiRoll)
{
    switch (aiRoll)
    {
    case AI_ROLL_MIN:
        return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].minimum;
    case AI_ROLL_MEDIAN:
        return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].median;
    case AI_ROLL_MAX:
        return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].maximum;
    case AI_ROLL_RANDOM:
        return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].random;
    default:
        return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].median; // Default assumes it deals median damage
    }
}

u32 AI_GetDamage(enum BattlerId battlerAtk, enum BattlerId battlerDef, u32 moveIndex, enum DamageCalcContext calcContext, struct AiLogicData *aiData)
{
    if (calcContext == AI_ATTACKING && BattlerHasAi(battlerAtk))
    {
        if ((gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_RISKY) && !(gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_CONSERVATIVE)) // Risky assumes it deals max damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].maximum;
        if ((gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_CONSERVATIVE) && !(gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_RISKY)) // Conservative assumes it deals min damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].minimum;
        return AI_GetDefaultDamageRollForContext(battlerAtk, battlerDef, moveIndex, aiData, GetConfig(AI_ROLL_ATTACKING));
    }
    else if (calcContext == AI_DEFENDING && BattlerHasAi(battlerDef))
    {
        if ((gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_RISKY) && !(gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_CONSERVATIVE)) // Risky assumes it takes min damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].minimum;
        if ((gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_CONSERVATIVE) && !(gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_RISKY)) // Conservative assumes it takes max damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].maximum;
        return AI_GetDefaultDamageRollForContext(battlerAtk, battlerDef, moveIndex, aiData, AI_ROLL_DEFENDING);
    }
    else if (calcContext == AI_SWITCHIN_ATTACKING && BattlerHasAi(battlerAtk))
    {
        if ((gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_RISKY) && !(gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_CONSERVATIVE)) // Risky assumes it deals max damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].maximum;
        if ((gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_CONSERVATIVE) && !(gAiThinkingStruct->aiFlags[battlerAtk] & AI_FLAG_RISKY)) // Conservative assumes it deals min damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].minimum;
        return AI_GetDefaultDamageRollForContext(battlerAtk, battlerDef, moveIndex, aiData, AI_ROLL_SWITCHIN_ATTACKING);
    }
    else if (calcContext == AI_SWITCHIN_DEFENDING && BattlerHasAi(battlerDef))
    {
        if ((gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_RISKY) && !(gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_CONSERVATIVE)) // Risky assumes it takes min damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].minimum;
        if ((gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_CONSERVATIVE) && !(gAiThinkingStruct->aiFlags[battlerDef] & AI_FLAG_RISKY)) // Conservative assumes it takes max damage
            return aiData->simulatedDmg[battlerAtk][battlerDef][moveIndex].maximum;
        return AI_GetDefaultDamageRollForContext(battlerAtk, battlerDef, moveIndex, aiData, AI_ROLL_SWITCHIN_DEFENDING);
    }
    else if (calcContext == AI_SHOUL