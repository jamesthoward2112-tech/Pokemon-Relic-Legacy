#include "global.h"
#include "battle.h"
#include "constants/battle_ai.h"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "data.h"
#include "item.h"
#include "party_menu.h"
#include "pokemon.h"
#include "random.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/item_effects.h"
#include "constants/battle_move_effects.h"
#include "constants/items.h"
#include "constants/moves.h"

// this file's functions
struct IncomingHealInfo
{
    u16 healAmount:16;
    u16 wishCounter:8;
    u16 hasHealing:1;
    u16 healBeforeHazards:1;
    u16 healAfterHazards:1;
    u16 healEndOfTurn:1;
    u16 curesStatus:1;
};
static bool32 CanUseSuperEffectiveMoveAgainstOpponents(enum BattlerId battler, enum BattlerId opposingBattler);
static bool32 CanUseSuperEffectiveMoveAgainstOpponent(enum BattlerId battler, enum BattlerId opposingBattler);
static u32 GetSwitchinHazardsDamage(enum BattlerId battler);
static u32 GetSwitchinSingleUseItemHealing(enum BattlerId battler, enum BattlerId opposingBattler, s32 currentHP);
static bool32 AI_CanSwitchinAbilityTrapOpponent(enum Ability ability, enum BattlerId opposingBattler);
static uq4_12_t GetTypeMatchupAgainstTypes(enum BattlerId opposingBattler, enum Type defType1, enum Type defType2);
static enum Ability GetPartyMonAbilityForSwitchCalc(enum BattlerId battler, u32 monIndex, struct Pokemon *mon);
static uq4_12_t GetBattlerTypeMatchup(enum BattlerId opposingBattler, enum BattlerId battler);
static u32 GetSwitchinHitsToKO(s32 damageTaken, enum BattlerId battler, const struct IncomingHealInfo *healInfo, u32 originalHp);
static void GetIncomingHealInfo(enum BattlerId battler, struct IncomingHealInfo *healInfo);
static u32 GetWishHealAmountForBattler(enum BattlerId battler);
static void SetBattlerStatusForSwitchin(enum BattlerId battler);
static void SetBattlerStatStagesForSwitchin(enum BattlerId battler, enum BattlerId opposingBattler, u32 fieldStatus);
static void SetBattlerHPChangeForSwitch(enum BattlerId battler, enum BattlerId opposingBattler);
static void SetBattlerVolatilesForSwitchin(enum BattlerId battler, u32 weather, u32 fieldStatus);
bool32 IsSwitchinTSpikesAffected(enum BattlerId battler);
static bool32 IsOpponentPhysicalAttacker(enum BattlerId battler, enum BattlerId opposingBattler);
static bool32 CanIntimidateLowerOpponentAtk(enum BattlerId battler, enum BattlerId opposingBattler);
static bool32 ShouldSwitchIfIntimidateBenefit(struct SwitchAiContext *switchContext);
static bool32 DoesMostSuitableSwitchinBenefitFromWish(enum BattlerId battler);
static u32 GetSwitchinCandidate(u32 switchinCategory, enum BattlerId battler, int lastId, enum SwitchType switchType);

static enum Ability GetPartyMonAbilityForSwitchCalc(enum BattlerId battler, u32 monIndex, struct Pokemon *mon)
{
    enum Ability ability = GetMonAbility(mon);

#if TESTING
    if (gTestRunnerEnabled)
    {
        enum BattleTrainer trainer = !IsPartnerMonFromSameTrainer(battler) ? battler : GetBattlerSide(battler);
        enum Ability forcedAbility = TestRunner_Battle_GetForcedAbility(trainer, monIndex);
        if (forcedAbility != ABILITY_NONE)
            ability = forcedAbility;
    }
#endif

    return ability;
}

static void InitializeSwitchinCandidate(enum BattlerId switchinBattler, u32 monIndex, struct Pokemon *mon)
{
    u32 storeCurrBattlerPartyIndex = gBattlerPartyIndexes[switchinBattler]; // Rage Fist fix
    PokemonToBattleMon(mon, &gBattleMons[switchinBattler]);
    gBattlerPartyIndexes[switchinBattler] = monIndex;
    CopyMonAbilityAndTypesToBattleMon(switchinBattler, mon);
    // Setup switchin battler data
    gAiThinkingStruct->saved[switchinBattler].saved = TRUE;
    SetBattlerAiData(switchinBattler, gAiLogicData);
    u32 switchinWeather = AI_GetSwitchinWeather(switchinBattler);
    u32 switchinTerrain = AI_GetSwitchinTerrain(switchinBattler);
    SetBattlerVolatilesForSwitchin(switchinBattler, switchinWeather, switchinTerrain);

    SetBattlerStatusForSwitchin(switchinBattler);
    gBattlerPartyIndexes[switchinBattler] = monIndex;
    gAiLogicData->switchInCalc = TRUE;

    for (enum BattlerId battlerIndex = 0; battlerIndex < gBattlersCount; battlerIndex++)
    {
        if (switchinBattler == battlerIndex || !IsBattlerAlive(battlerIndex))
            continue;
        SetBattlerStatStagesForSwitchin(switchinBattler, battlerIndex, switchinTerrain);
        SetBattlerHPChangeForSwitch(switchinBattler, battlerIndex);
        CalcBattlerAiMovesData(gAiLogicData, switchinBattler, battlerIndex, switchinWeather, switchinTerrain);
        CalcBattlerAiMovesData(gAiLogicData, battlerIndex, switchinBattler, switchinWeather, switchinTerrain);
    }

    gAiLogicData->switchInCalc = FALSE;
    gBattlerPartyIndexes[switchinBattler] = storeCurrBattlerPartyIndex;
    gAiThinkingStruct->saved[switchinBattler].saved = FALSE;
}

static u32 GetWishHealAmountForBattler(enum BattlerId battler)
{
    u32 wishHeal = 0;

    if (gBattleStruct->wish[battler].counter == 0)
        return wishHeal;

    if (B_WISH_HP_SOURCE >= GEN_5)
    {
        wishHeal = GetMonData(&GetBattlerParty(battler)[gBattleStruct->wish[battler].partyId], MON_DATA_MAX_HP) / 2;
    }
    else
    {
        wishHeal = GetNonDynamaxMaxHP(battler) / 2;
    }

    return wishHeal;
}

static void GetIncomingHealInfo(enum BattlerId battler, struct IncomingHealInfo *healInfo)
{
    memset(healInfo, 0, sizeof(*healInfo));

    // Healing Wish / Lunar Dance heal to full and clear status before hazards
    if (gBattleStruct->battlerState[battler].storedHealingWish)
    {
        healInfo->hasHealing = TRUE;
        healInfo->healBeforeHazards = TRUE;
        healInfo->curesStatus = TRUE;
    }
    if (gBattleStruct->battlerState[battler].storedLunarDance)
    {
        healInfo->hasHealing = TRUE;
        healInfo->healBeforeHazards = TRUE;
        healInfo->curesStatus = TRUE;
    }

    // Z-Parting Shot / Z-Memento heal after hazards on switch-in
    if (gBattleStruct->zmove.healReplacement)
    {
        healInfo->hasHealing = TRUE;
        healInfo->healAfterHazards = TRUE;
    }

    // Wish heals at end of turn
    if (gBattleStruct->wish[battler].counter > 0)
    {
        healInfo->hasHealing = TRUE;
        healInfo->healEndOfTurn = TRUE;
        healInfo->wishCounter = gBattleStruct->wish[battler].counter;
        healInfo->healAmount = GetWishHealAmountForBattler(battler);
    }
}

u32 GetSwitchChance(enum ShouldSwitchScenario shouldSwitchScenario)
{
    // Modify these cases if you want unique behaviour based on other data (trainer class, difficulty, etc.)
    switch (shouldSwitchScenario)
    {
    case SHOULD_SWITCH_WONDER_GUARD:
        return SHOULD_SWITCH_WONDER_GUARD_PERCENTAGE;
    case SHOULD_SWITCH_ABSORBS_MOVE:
        return SHOULD_SWITCH_ABSORBS_MOVE_PERCENTAGE;
    case SHOULD_SWITCH_TRAPPER:
