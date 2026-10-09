Warning: truncated output (original token count: 107201)
Total output lines: 11350

#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_anim_scripts.h"
#include "battle_arena.h"
#include "battle_environment.h"
#include "battle_pyramid.h"
#include "battle_util.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_setup.h"
#include "battle_z_move.h"
#include "battle_gimmick.h"
#include "battle_hold_effects.h"
#include "battle_stat_change.h"
#include "config_changes.h"
#include "party_menu.h"
#include "pokemon.h"
#include "international_string_util.h"
#include "item.h"
#include "util.h"
#include "battle_scripts.h"
#include "random.h"
#include "text.h"
#include "safari_zone.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "test_runner.h"
#include "trig.h"
#include "trainer_slide.h"
#include "window.h"
#include "battle_message.h"
#include "battle_ai_record.h"
#include "battle_ai_util.h"
#include "event_data.h"
#include "link.h"
#include "malloc.h"
#include "berry.h"
#include "pokedex.h"
#include "mail.h"
#include "field_weather.h"
#include "constants/abilities.h"
#include "constants/battle_anim.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_string_ids.h"
#include "constants/items.h"
#include "constants/item_effects.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/weather.h"
#include "constants/pokemon.h"
#include "test/battle.h"

static bool32 TryRemoveScreens(enum BattlerId battler);
static bool32 IsUnnerveAbilityOnOpposingSide(enum BattlerId battler);
static u32 GetFlingPowerFromItemId(enum Item itemId);
static bool32 IsNonVolatileStatusBlocked(enum BattlerId battlerDef, enum Ability abilityDef, bool32 abilityAffected, const u8 *battleScript, enum ResultOption option);
static bool32 CanSleepDueToSleepClause(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum ResultOption option);
static bool32 IsOpposingSideEmpty(enum BattlerId battler);
static void ResetParadoxWeatherStat(enum BattlerId battler);
static void ResetParadoxTerrainStat(enum BattlerId battler);
static bool32 CanBattlerFormChange(enum BattlerId battler, enum FormChanges method);
const u8 *AbsorbedByDrainHpAbility(enum BattlerId battlerDef);
const u8 *AbsorbedByStatIncreaseAbility(struct DamageContext *ctx, enum Stat statId, u32 statAmount);
const u8 *AbsorbedByFlashFire(struct DamageContext *ctx);
static bool32 IsCriticalHit(struct DamageContext *ctx);

ARM_FUNC NOINLINE static uq4_12_t PercentToUQ4_12(u32 percent);
ARM_FUNC NOINLINE static uq4_12_t PercentToUQ4_12_Floored(u32 percent);

extern const u8 *const gBattlescriptsForRunningByItem[];
extern const u8 *const gBattlescriptsForUsingItem[];
extern const u8 *const gBattlescriptsForSafariActions[];

enum BattlerId GetBattlerAtPosition(enum BattlerPosition position)
{
    enum BattlerId battler;
    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerPosition(battler) == position)
            break;
    }
    return battler;
}

enum BattlerPosition GetPartnerPosition(enum BattlerPosition position)
{
    return (position ^ BIT_FLANK);
}

enum BattlerPosition GetOppositePosition(enum BattlerPosition position)
{
    return (position ^ BIT_SIDE);
}

enum BattlerId GetPartnerBattler(enum BattlerId battler)
{
    return GetBattlerAtPosition(GetPartnerPosition(GetBattlerPosition(battler)));
}

enum BattlerId GetOppositeBattler(enum BattlerId battler)
{
    return GetBattlerAtPosition(GetOppositePosition(GetBattlerPosition(battler)));
}

// Left and right are determined by how they're referred to in tests and everywhere else.
// Left is battlers 0 and 1, right 2 and 3; if you assume the battler referencing them is south, left is to the northeast and right to the northwest.
enum BattlerId GetBattlerLeftFoe(enum BattlerId battler)
{
    return GetBattlerAtPosition(GetOppositePosition((enum BattlerPosition)GetBattlerSide(battler)));
}

enum BattlerId GetBattlerRightFoe(enum BattlerId battler)
{
    return GetPartnerBattler(GetBattlerLeftFoe(battler));
}

enum BattlerId GetDefaultSelectionTarget(enum BattlerId battler, enum MoveTarget moveTarget)
{
    switch (moveTarget)
    {
    case TARGET_USER:
    case TARGET_USER_OR_ALLY:
    case TARGET_USER_AND_ALLY:
        return battler;
    case TARGET_ALLY:
        return GetPartnerBattler(battler);
    default:
        return GetBattlerLeftFoe(battler);
    }
}

static const u8 sPkblToEscapeFactor[][3] = {
    {
        [B_MSG_MON_CURIOUS]    = 0,
        [B_MSG_MON_ENTHRALLED] = 0,
        [B_MSG_MON_IGNORED]    = 0
    },{
        [B_MSG_MON_CURIOUS]    = 3,
        [B_MSG_MON_ENTHRALLED] = 5,
        [B_MSG_MON_IGNORED]    = 0
    },{
        [B_MSG_MON_CURIOUS]    = 2,
        [B_MSG_MON_ENTHRALLED] = 3,
        [B_MSG_MON_IGNORED]    = 0
    },{
        [B_MSG_MON_CURIOUS]    = 1,
        [B_MSG_MON_ENTHRALLED] = 2,
        [B_MSG_MON_IGNORED]    = 0
    },{
        [B_MSG_MON_CURIOUS]    = 1,
        [B_MSG_MON_ENTHRALLED] = 1,
        [B_MSG_MON_IGNORED]    = 0
    }
};
static const u8 sGoNearCounterToCatchFactor[] = {4, 3, 2, 1};
static const u8 sGoNearCounterToEscapeFactor[] = {4, 4, 4, 4};

const struct BattleWeatherInfo gBattleWeatherInfo[BATTLE_WEATHER_COUNT] = {
    [BATTLE_WEATHER_RAIN] =
    {
        .flag = B_WEATHER_RAIN_NORMAL,
        .rock = HOLD_EFFECT_DAMP_ROCK,
        .abilityStartMessage = B_MSG_STARTED_DRIZZLE,
        .moveStartMessage = B_MSG_STARTED_RAIN,
        .endMessage = B_MSG_WEATHER_END_RAIN,
        .continuesMessage = B_MSG_WEATHER_TURN_RAIN,
        .animation = B_ANIM_RAIN_CONTINUES,
        .type = BATTLE_WEATHER_RAIN,
    },

    [BATTLE_WEATHER_RAIN_PRIMAL] =
    {
        .flag = B_WEATHER_RAIN_PRIMAL,
        .rock = HOLD_EFFECT_DAMP_ROCK,
        .abilityStartMessage = B_MSG_STARTED_PRIMORDIAL_SEA,
        .moveStartMessage = B_MSG_STARTED_RAIN, // Placeholder
        .endMessage = B_MSG_WEATHER_END_RAIN,
        .continuesMessage = B_MSG_WEATHER_TURN_RAIN,
        .animation = B_ANIM_RAIN_CONTINUES,
        .type = BATTLE_WEATHER_RAIN,
    },

    [BATTLE_WEATHER_RAIN_DOWNPOUR] =
    {
        .flag = B_WEATHER_RAIN_NORMAL,
        .rock = HOLD_EFFECT_DAMP_ROCK,
        .abilityStartMessage = B_MSG_STARTED_DRIZZLE,
        .moveStartMessage = B_MSG_STARTED_RAIN,
        .endMessage = B_MSG_WEATHER_END_RAIN,
        .continuesMessage = B_MSG_WEATHER_TURN_DOWNPOUR,
        .animation = B_ANIM_RAIN_CONTINUES,
        .type = BATTLE_WEATHER_RAIN,
    },

    [BATTLE_WEATHER_SUN] =
    {
        .flag = B_WEATHER_SUN_NORMAL,
        .rock = HOLD_EFFECT_HEAT_ROCK,
        .abilityStartMessage = B_MSG_STARTED_DROUGHT,
        .moveStartMessage = B_MSG_STARTED_SUNLIGHT,
        .endMessage = B_MSG_WEATHER_END_SUN,
        .continuesMessage = B_MSG_WEATHER_TURN_SUN,
        .animation = B_ANIM_SUN_CONTINUES,
        .type = BATTLE_WEATHER_SUN,
    },

    [BATTLE_WEATHER_SUN_PRIMAL] =
    {
        .flag = B_WEATHER_SUN_PRIMAL,
        .rock = HOLD_EFFECT_HEAT_ROCK,
        .abilityStartMessage = B_MSG_STARTED_DESOLATE_LAND,
        .moveStartMessage = B_MSG_STARTED_SUNLIGHT, // Placeholder
        .endMessage = B_MSG_WEATHER_END_SUN,
        .continuesMessage = B_MSG_WEATHER_TURN_SUN,
        .animation = B_ANIM_SUN_CONTINUES,
        .type = BATTLE_WEATHER_SUN,
    },

    [BATTLE_WEATHER_SANDSTORM] =
    {
        .flag = B_WEATHER_SANDSTORM,
        .rock = HOLD_EFFECT_SMOOTH_ROCK,
        .abilityStartMessage = B_MSG_STARTED_SAND_STREAM,
        .moveStartMessage = B_MSG_STARTED_SANDSTORM,
        .endMessage = B_MSG_WEATHER_END_SANDSTORM,
        .continuesMessage = B_MSG_WEATHER_TURN_SANDSTORM,
        .animation = B_ANIM_SANDSTORM_CONTINUES,
        .type = BATTLE_WEATHER_SANDSTORM,
    },

    [BATTLE_WEATHER_HAIL] =
    {
        .flag = B_WEATHER_HAIL,
        .rock = HOLD_EFFECT_ICY_ROCK,
        .abilityStartMessage = B_MSG_STARTED_HAIL_WARNING,
        .moveStartMessage = B_MSG_STARTED_HAIL,
        .endMessage = B_MSG_WEATHER_END_HAIL,
        .continuesMessage = B_MSG_WEATHER_TURN_HAIL,
        .animation = B_ANIM_HAIL_CONTINUES,
        .type = BATTLE_WEATHER_SNOW,
    },

    [BATTLE_WEATHER_SNOW] =
    {
        .flag = B_WEATHER_SNOW,
        .rock = HOLD_EFFECT_ICY_ROCK,
        .abilityStartMessage = B_MSG_STARTED_SNOW_WARNING,
        .moveStartMessage = B_MSG_STARTED_SNOW,
        .endMessage = B_MSG_WEATHER_END_SNOW,
        .continuesMessage = B_MSG_WEATHER_TURN_SNOW,
        .animation = B_ANIM_SNOW_CONTINUES,
        .type = BATTLE_WEATHER_SNOW,
    },

    [BATTLE_WEATHER_FOG] =
    {
        .flag = B_WEATHER_FOG,
        .rock = HOLD_EFFECT_NONE,
        .abilityStartMessage = B_MSG_STARTED_DRIZZLE, // Placeholder
        .moveStartMessage = B_MSG_STARTED_FOG, // Placeholder
        .endMessage = B_MSG_WEATHER_END_FOG,
        .continuesMessage = B_MSG_WEATHER_TURN_FOG,
        .animation = B_ANIM_FOG_CONTINUES,
        .type = BATTLE_WEATHER_FOG,
    },

    [BATTLE_WEATHER_STRONG_WINDS] =
    {
        .flag = B_WEATHER_STRONG_WINDS,
        .rock = HOLD_EFFECT_NONE,
        .abilityStartMessage = B_MSG_STARTED_STRONG_WINDS,
        .moveStartMessage = B_MSG_STARTED_RAIN, // Placeholder
        .endMessage = B_MSG_WEATHER_END_STRONG_WINDS,
        .continuesMessage = B_MSG_WEATHER_TURN_STRONG_WINDS,
        .animation = B_ANIM_STRONG_WINDS,
        .type = BATTLE_WEATHER_STRONG_WINDS,
    },
};

enum BattleWeather GetBattleWeather(u32 weather)
{
    u32 currBattleWeather = BATTLE_WEATHER_NONE;

    for (u32 i = 0; i < ARRAY_COUNT(gBattleWeatherInfo); i++)
    {
        if (weather & gBattleWeatherInfo[i].flag)
        {
            currBattleWeather = i;
            break;
        }
    }

    return currBattleWeather;
}

const struct TerrainInfo gBattleTerrainInfo[B_TERRAIN_COUNT] = {
    [B_TERRAIN_GRASSY] = {
        .type = TYPE_GRASS,
        .secretPowerAnimation = gBattleAnimMove_NeedleArm,
        .secretPowerEffect = MOVE_EFFECT_SLEEP,
        .naturePowerMove = MOVE_ENERGY_BALL,
        .battleBackground = BG_GRASSY_TERRAIN,
        .seedStat = STAT_DEF,
        .seedHoldEffect = HOLD_EFFECT_PARAM_GRASSY_TERRAIN,
        .startMessage = B_MSG_TERRAIN_SET_GRASSY,
        .endMessage = B_MSG_TERRAIN_END_GRASSY,
    },

    [B_TERRAIN_MISTY] = {
        .type = TYPE_FAIRY,
        .secretPowerAnimation = gBattleAnimMove_FairyWind,
        .secretPowerEffect = SECRET_POWER_SP_ATK_MINUS_1,
        .naturePowerMove = MOVE_MOONBLAST,
        .battleBackground = BG_MISTY_TERRAIN,
        .seedStat = STAT_SPDEF,
        .seedHoldEffect = HOLD_EFFECT_PARAM_MISTY_TERRAIN,
        .startMessage = B_MSG_TERRAIN_SET_MISTY,
        .endMessage = B_MSG_TERRAIN_END_MISTY,
    },

    [B_TERRAIN_ELECTRIC] = {
        .type = TYPE_ELECTRIC,
        .secretPowerAnimation = gBattleAnimMove_ThunderShock,
        .secretPowerEffect = MOVE_EFFECT_PARALYSIS,
        .naturePowerMove = MOVE_THUNDERBOLT,
        .battleBackground = BG_ELECTRIC_TERRAIN,
        .seedStat = STAT_DEF,
        .seedHoldEffect = HOLD_EFFECT_PARAM_ELECTRIC_TERRAIN,
        .startMessage = B_MSG_TERRAIN_SET_ELECTRIC,
        .endMessage = B_MSG_TERRAIN_END_ELECTRIC,
    },

    [B_TERRAIN_PSYCHIC] = {
        .type = TYPE_PSYCHIC,
        .secretPowerAnimation = gBattleAnimMove_Confusion,
        .secretPowerEffect = SECRET_POWER_SPD_MINUS_1,
        .naturePowerMove = MOVE_PSYCHIC,
        .battleBackground = BG_PSYCHIC_TERRAIN,
        .seedStat = STAT_SPDEF,
        .seedHoldEffect = HOLD_EFFECT_PARAM_PSYCHIC_TERRAIN,
        .startMessage = B_MSG_TERRAIN_SET_PSYCHIC,
        .endMessage = B_MSG_TERRAIN_END_PSYCHIC,
    },
};

bool32 EndOrContinueWeather(void)
{
    enum BattleWeather currBattleWeather = GetBattleWeather(gBattleWeather);

    if (currBattleWeather == BATTLE_WEATHER_NONE)
        return FALSE;

    if (gBattleStruct->weatherDuration > 0 && --gBattleStruct->weatherDuration == 0)
    {
        gBattleWeather = B_WEATHER_NONE;
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            gBattleMons[battler].volatiles.weatherAbilityDone = FALSE;
            ResetParadoxWeatherStat(battler);
        }
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleWeatherInfo[currBattleWeather].endMessage;
        BattleScriptCall(BattleScript_WeatherFaded);
        return TRUE;
    }
    else
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleWeatherInfo[currBattleWeather].continuesMessage;
        gBattleScripting.animArg1 = gBattleWeatherInfo[currBattleWeather].animation;
        BattleScriptCall(BattleScript_WeatherContinues);
        return TRUE;
    }

    return FALSE;
}

// Gen5+
static u32 CalcBeatUpPower(void)
{
    enum Species species = gBattleStruct->beatUpSpecies[gBattleStruct->beatUpSlot++];
    // FIXME: Why call CalcBeatUpPower when 'beatUpSlot' is OOB?
    if (species == 0xFFFF)
        return 0;
    return (GetSpeciesBaseAttack(species) / 10) + 5;
}

// Gen 3/4
static s32 CalcBeatUpDamage(struct DamageContext *ctx)
{
    u32 partyIndex = gBattleStruct->beatUpSpecies[gBattleStruct->beatUpSlot++];
    struct Pokemon *party = GetBattlerParty(ctx->battlerAtk);
    enum Species species = GetMonData(&party[partyIndex], MON_DATA_SPECIES);
    u32 levelFactor = GetMonData(&party[partyIndex], MON_DATA_LEVEL) * 2 / 5 + 2;
    s32 dmg = GetSpeciesBaseAttack(species);

    dmg *= GetMovePower(ctx->move);
    dmg *= levelFactor;
    dmg /= GetSpeciesBaseDefense(gBattleMons[ctx->battlerDef].species);
    dmg = (dmg / 50) + 2;

    if (gProtectStructs[ctx->battlerAtk].helpingHand)
        dmg = dmg * 15 / 10;
    if (ctx->isCrit)
        dmg *= 2;

    return dmg;
}

enum DamageCategory GetReflectDamageMoveDamageCategory(enum BattlerId battler, enum Move move)
{
    u32 damageCategories = GetMoveReflectDamage_DamageCategories(move);

    if (damageCategories == 1u << DAMAGE_CATEGORY_PHYSICAL) // Counter
        return DAMAGE_CATEGORY_PHYSICAL;
    if (damageCategories == 1u << DAMAGE_CATEGORY_SPECIAL) // Mirror Coat
        return DAMAGE_CATEGORY_SPECIAL;

    // Metal Burst / Comeuppance
    if (gProtectStructs[battler].lastHitBySpecialMove)
        return DAMAGE_CATEGORY_SPECIAL;
    else
        return DAMAGE_CATEGORY_PHYSICAL;
}

bool32 ShouldTeraShellDistortTypeMatchups(struct DamageContext *ctx)
{
    if (ctx->abilities[ctx->battlerDef] == ABILITY_TERA_SHELL
     && gBattleMons[ctx->battlerDef].species == SPECIES_TERAPAGOS_TERASTAL
     && gBattleMons[ctx->battlerDef].hp == gBattleMons[ctx->battlerDef].maxHP
     && !IsBattleMoveStatus(ctx->move))
        return TRUE;

    return FALSE;
}

bool32 IsUnnerveBlocked(enum BattlerId battler, enum Item itemId)
{
    if (GetItemPocket(itemId) != POCKET_BERRIES)
        return FALSE;

    if (gBattleScripting.overrideBerryRequirements > 0) // Berries that aren't eaten naturally ignore unnerve
        return FALSE;

    if (IsUnnerveAbilityOnOpposingSide(battler))
        return TRUE;

    return FALSE;
}

static bool32 IsUnnerveAbilityOnOpposingSide(enum BattlerId battler)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (battler == battlerDef || IsBattlerAlly(battler, battlerDef))
            continue;

        enum Ability ability = GetBattlerAbility(battlerDef);
        switch (ability)
        {
        case ABILITY_UNNERVE:
        case ABILITY_AS_ONE_ICE_RIDER:
        case ABILITY_AS_ONE_SHADOW_RIDER:
            return TRUE;
        default:
            break;
        }
    }

    return FALSE;
}

// Functions
void HandleAction_UseMove(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    if (!IsBattlerAlive(gBattlerAttacker)
     || gBattleStruct->battlerState[gBattlerAttacker].commandingDondozo)
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }

    gCurrMovePos = gChosenMovePos = gBattleStruct->chosenMovePositions[gBattlerAttacker];

    // choose move
    if (gProtectStructs[gBattlerAttacker].noValidMoves)
    {
        gProtectStructs[gBattlerAttacker].noValidMoves = FALSE;
        gCurrentMove = gChosenMove = MOVE_STRUGGLE;
        gBattleStruct->moveTarget[gBattlerAttacker] = GetBattleMoveTarget(MOVE_STRUGGLE, TARGET_NONE);
    }
    else if (gBattleMons[gBattlerAttacker].volatiles.multipleTurns || gBattleMons[gBattlerAttacker].volatiles.rechargeTimer > 0)
    {
        gCurrentMove = gChosenMove = gLockedMoves[gBattlerAttacker];
    }
    // encore forces you to use the same move
    else if (GetActiveGimmick(gBattlerAttacker) != GIMMICK_Z_MOVE && gBattleMons[gBattlerAttacker].volatiles.encoredMove != MOVE_NONE
             && gBattleMons[gBattlerAttacker].volatiles.encoredMove == gBattleMons[gBattlerAttacker].moves[gBattleMons[gBattlerAttacker].volatiles.encoredMovePos])
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].volatiles.encoredMove;
        gCurrMovePos = gChosenMovePos = gBattleMons[gBattlerAttacker].volatiles.encoredMovePos;
        if (GetConfig(B_ENCORE_TARGET) < GEN_5)
            gBattleStruct->moveTarget[gBattlerAttacker] = GetBattleMoveTarget(gCurrentMove, TARGET_NONE);
    }
    // check if the encored move wasn't overwritten
    else if (GetActiveGimmick(gBattlerAttacker) != GIMMICK_Z_MOVE && gBattleMons[gBattlerAttacker].volatiles.encoredMove != MOVE_NONE
          && gBattleMons[gBattlerAttacker].volatiles.encoredMove != gBattleMons[gBattlerAttacker].moves[gBattleMons[gBattlerAttacker].volatiles.encoredMovePos])
    {
        gCurrMovePos = gChosenMovePos = gBattleMons[gBattlerAttacker].volatiles.encoredMovePos;
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gBattleMons[gBattlerAttacker].volatiles.encoredMove = MOVE_NONE;
        gBattleMons[gBattlerAttacker].volatiles.encoredMovePos = 0;
        gBattleMons[gBattlerAttacker].volatiles.encoreTimer = 0;
        gBattleStruct->moveTarget[gBattlerAttacker] = GetBattleMoveTarget(gCurrentMove, TARGET_NONE);
    }
    else if (gBattleMons[gBattlerAttacker].moves[gCurrMovePos] != gChosenMoveByBattler[gBattlerAttacker])
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gBattleStruct->moveTarget[gBattlerAttacker] = GetBattleMoveTarget(gCurrentMove, TARGET_NONE);
    }
    else
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
    }

    if (IsBattlerAlive(gBattlerAttacker))
    {
        if (IsOnPlayerSide(gBattlerAttacker))
            gBattleResults.lastUsedMovePlayer = gCurrentMove;
        else
            gBattleResults.lastUsedMoveOpponent = gCurrentMove;
    }

    SetTypeBeforeUsingMove(gChosenMove, gBattlerAttacker, GetBattlerAbility(gBattlerAttacker), GetBattlerHoldEffect(gBattlerAttacker));
    gBattleStruct->baseMove = gCurrentMove;

    // check Z-Move used
    if (GetActiveGimmick(gBattlerAttacker) == GIMMICK_Z_MOVE
     && GetMoveCategory(gCurrentMove) != DAMAGE_CATEGORY_STATUS // Check the actual type, not the dynamic one
     && !IsZMove(gCurrentMove))
    {
        gCurrentMove = gChosenMove = GetUsableZMove(gBattlerAttacker, gCurrentMove);
    }
    // check Max Move used
    else if (GetActiveGimmick(gBattlerAttacker) == GIMMICK_DYNAMAX)
    {
        gCurrentMove = gChosenMove = GetMaxMove(gBattlerAttacker, gCurrentMove);
    }

    gBattleStruct->eventState.atkCanceler = 0;
    ClearDamageCalcResults();
    ClearBothStatChangeQueues();
    gMultiHitCounter = 0;
    gBattlerTarget = gBattleStruct->moveTarget[gBattlerAttacker];

    if (gBattleTypeFlags & BATTLE_TYPE_PALACE && gProtectStructs[gBattlerAttacker].palaceUnableToUseMove)
    {
        // Battle Palace, select battle script for failure to use move
        if (!IsBattlerAlive(gBattlerAttacker))
        {
            gCurrentActionFuncId = B_ACTION_FINISHED;
            return;
        }
        else if (gPalaceSelectionBattleScripts[gBattlerAttacker] != NULL)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_INCAPABLE_OF_POWER;
            gBattlescriptCurrInstr = gPalaceSelectionBattleScripts[gBattlerAttacker];
            gPalaceSelectionBattleScripts[gBattlerAttacker] = NULL;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_INCAPABLE_OF_POWER;
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
        }
    }
    else
    {
        gBattlescriptCurrInstr = GetMoveBattleScript(gCurrentMove);
    }

    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        BattleArena_AddMindPoints(gBattlerAttacker);

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        gBattleStruct->battlerState[battler].wasAboveHalfHp = gBattleMons[battler].hp > gBattleMons[battler].maxHP / 2;
        gBattleMons[battler].volatiles.activateDancer = FALSE;
    }

    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_Switch(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];

    enum BattlerId partnerAtk = GetPartnerBattler(gBattlerAttacker);
    // if switching to a mon that is already on field, cancel switch
    if (!(gAbsentBattlerFlags & (1u << partnerAtk))
     && IsBattlerAlive(partnerAtk)
     && gBattlerPartyIndexes[partnerAtk] == gBattleStruct->monToSwitchIntoId[gBattlerAttacker]
     && BattlersShareParty(gBattlerAttacker, partnerAtk))
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gActionSelectionCursor[gBattlerAttacker] = 0;
    gMoveSelectionCursor[gBattlerAttacker] = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattleStruct->battlerPartyIndexes[gBattlerAttacker]);

    gBattleScripting.battler = gBattlerAttacker;
    gBattlescriptCurrInstr = BattleScript_ActionSwitch;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;

    if (gBattleResults.playerSwitchesCounter < 255)
        gBattleResults.playerSwitchesCounter++;

    TryBattleFormChange(gBattlerAttacker, FORM_CHANGE_BATTLE_SWITCH_OUT, GetBattlerAbility(gBattlerAttacker));
}

void HandleAction_UseItem(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    ClearVariousBattlerFlags(gBattlerAttacker);

    gLastUsedItem = gBattleResources->bufferB[gBattlerAttacker][1] | (gBattleResources->bufferB[gBattlerAttacker][2] << 8);
    if (X_ITEM_FRIENDSHIP_INCREASE > 0
        && GetItemEffectType(gLastUsedItem) == ITEM_EFFECT_X_ITEM
        && !ShouldSkipFriendshipChange())
        UpdateFriendshipFromXItem(gBattlerAttacker);

    gBattlescriptCurrInstr = gBattlescriptsForUsingItem[GetItemBattleUsage(gLastUsedItem) - 1];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

bool32 TryRunFromBattle(enum BattlerId battler)
{
    bool32 effect = FALSE;
    u8 holdEffect;
    u8 pyramidMultiplier;
    u8 speedVar;

    // If this flag is set, running will never be successful under any circumstances.
    if (FlagGet(WE_FLAG_NO_RUNNING))
        return effect;

    if (gBattleMons[battler].item == ITEM_ENIGMA_BERRY_E_READER)
        holdEffect = gEnigmaBerries[battler].holdEffect;
    else
        holdEffect = GetItemHoldEffect(gBattleMons[battler].item);

    gPotentialItemEffectBattler = battler;

    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
    {
        gLastUsedItem = gBattleMons[battler].item;
        gProtectStructs[battler].fleeType = FLEE_ITEM;
        effect = TRUE;
    }
    else if (GetConfig(B_GHOSTS_ESCAPE) >= GEN_6 && IS_BATTLER_OF_TYPE(battler, TYPE_GHOST))
    {
        effect = TRUE;
    }
    else if (GetBattlerAbility(battler) == ABILITY_RUN_AWAY)
    {
        if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        {
            gBattleStruct->runTries++;
            pyramidMultiplier = GetPyramidRunMultiplier();
            speedVar = (gBattleMons[battler].speed * pyramidMultiplier) / (gBattleMons[GetOppositeBattler(battler)].speed) + (gBattleStruct->runTries * 30);
            if (speedVar > (Random() & 0xFF))
            {
                gLastUsedAbility = ABILITY_RUN_AWAY;
                gProtectStructs[battler].fleeType = FLEE_ABILITY;
                effect = TRUE;
            }
        }
        else
        {
            gLastUsedAbility = ABILITY_RUN_AWAY;
            gProtectStructs[battler].fleeType = FLEE_ABILITY;
            effect = TRUE;
        }
    }
    else if (IsGhostBattleWithoutScope())
    {
        if (GetBattlerSide(battler) == B_SIDE_PLAYER)
            effect = TRUE;
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL) && gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        effect = TRUE;
    }
    else if (CanPlayerForfeitNormalTrainerBattle())
    {
        effect = TRUE;
    }
    else
    {
        enum BattlerId runningFromBattler = GetOppositeBattler(battler);
        if (!IsBattlerAlive(runningFromBattler))
            runningFromBattler |= BIT_FLANK;

        if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        {
            pyramidMultiplier = GetPyramidRunMultiplier();
            speedVar = (gBattleMons[battler].speed * pyramidMultiplier) / (gBattleMons[runningFromBattler].speed) + (gBattleStruct->runTries * 30);
            if (speedVar > (Random() & 0xFF))
                effect = TRUE;
        }
        else if (gBattleMons[battler].speed < gBattleMons[runningFromBattler].speed)
        {
            speedVar = (gBattleMons[battler].speed * 128) / (gBattleMons[runningFromBattler].speed) + (gBattleStruct->runTries * 30);
            if (speedVar > (Random() & 0xFF))
                effect = TRUE;
        }
        else // same speed or faster
        {
            effect = TRUE;
        }

        gBattleStruct->runTries++;
    }

    if (effect)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        gBattleOutcome = B_OUTCOME_RAN;
    }

    return effect;
}

void HandleAction_Run(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
    {
        gCurrentTurnActionNumber = gBattlersCount;

        for (enum BattlerId i = 0; i < gBattlersCount; i++)
        {
            if (IsOnPlayerSide(i))
            {
                if (gChosenActionByBattler[i] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_LOST;
            }
            else
            {
                if (gChosenActionByBattler[i] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_WON;
            }
        }

        gBattleOutcome |= B_OUTCOME_LINK_BATTLE_RAN;
        gSaveBlock2Ptr->frontier.disableRecordBattle = TRUE;
    }
    else
    {
        if (IsOnPlayerSide(gBattlerAttacker))
        {
            if (!TryRunFromBattle(gBattlerAttacker)) // failed to run away
            {
                ClearVariousBattlerFlags(gBattlerAttacker);
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_ESCAPE_2;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
        }
        else
        {
            if (GetBattlerHoldEffect(gBattlerAttacker) != HOLD_EFFECT_CAN_ALWAYS_RUN
             && GetBattlerAbility(gBattlerAttacker) != ABILITY_RUN_AWAY
             && !CanBattlerEscape(gBattlerAttacker))
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ATTACKER_CANT_ESCAPE;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
            else
            {
                gCurrentTurnActionNumber = gBattlersCount;
                gBattleOutcome = B_OUTCOME_MON_FLED;
            }
        }
    }
}

#define safariBaitThrowCounter safariPkblThrowCounter
#define safariRockThrowCounter safariGoNearCounter

void HandleAction_WatchesCarefully(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    if (IS_FRLG)
    {
        if (gBattleStruct->safariRockThrowCounter != 0)
        {
            gBattleStruct->safariRockThrowCounter--;
            if (gBattleStruct->safariRockThrowCounter == 0)
            {
                gBattleStruct->safariCatchFactor = gSpeciesInfo[GetMonData(gParties[B_TRAINER_OPPONENT_A], MON_DATA_SPECIES)].catchRate * 100 / 1275;
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
            }
            else
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_ANGRY;
            }
        }
        else
        {
            if (gBattleStruct->safariBaitThrowCounter != 0)
            {
                --gBattleStruct->safariBaitThrowCounter;
                if (gBattleStruct->safariBaitThrowCounter == 0)
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
                else
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_EATING;
            }
            else
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
            }
        }
        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[0];
    }
    else
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_MON_WATCHING;
        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[0];
    }
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_SafariZoneBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gNumSafariBalls--;
    gLastUsedItem = ITEM_SAFARI_BALL;
    gBattlescriptCurrInstr = BattleScript_SafariBallThrow;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_ThrowBall(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gLastUsedItem = gBallToDisplay;
    if (!GetItemImportance(gLastUsedItem))
        RemoveBagItem(gLastUsedItem, 1);
    gBattlescriptCurrInstr = BattleScript_BallThrow;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_ThrowPokeblock(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    if (IS_FRLG)
    {
        // throw bait
        gBattleStruct->safariBaitThrowCounter += Random() % 5 + 2;
        if (gBattleStruct->safariBaitThrowCounter > 6)
            gBattleStruct->safariBaitThrowCounter = 6;

        gBattleStruct->safariRockThrowCounter = 0;
        gBattleStruct->safariCatchFactor >>= 1;

        if (gBattleStruct->safariCatchFactor <= 2)
            gBattleStruct->safariCatchFactor = 3;

        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[5];
    }
    else
    {
        // throw pokeblock
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleResources->bufferB[gBattlerAttacker][1] - 1;
        gLastUsedItem = gBattleResources->bufferB[gBattlerAttacker][2];

        if (gBattleResults.pokeblockThrows < 255)
            gBattleResults.pokeblockThrows++;
        if (gBattleStruct->safariPkblThrowCounter < 3)
            gBattleStruct->safariPkblThrowCounter++;
        if (gBattleStruct->safariEscapeFactor > 1)
        {
            // BUG: safariEscapeFactor can become 0 below. This causes the pokeblock throw glitch.
            #ifdef BUGFIX
            if (gBattleStruct->safariEscapeFactor <= sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]])
            #else
            if (gBattleStruct->safariEscapeFactor < sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]])
            #endif
                gBattleStruct->safariEscapeFactor = 1;
            else
                gBattleStruct->safariEscapeFactor -= sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]];
        }

        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[2];
    }

    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_GoNear(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    if (IS_FRLG)
    {
        // throw rock
        gBattleStruct->safariRockThrowCounter += Random() % 5 + 2;
        if (gBattleStruct->safariRockThrowCounter > 6)
            gBattleStruct->safariRockThrowCounter = 6;

        gBattleStruct->safariBaitThrowCounter = 0;
        gBattleStruct->safariCatchFactor <<= 1;

        if (gBattleStruct->safariCatchFactor > 20)
            gBattleStruct->safariCatchFactor = 20;

        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[4];
    }
    else
    {
        // go near
        gBattleStruct->safariCatchFactor += sGoNearCounterToCatchFactor[gBattleStruct->safariGoNearCounter];
        if (gBattleStruct->safariCatchFactor > 20)
            gBattleStruct->safariCatchFactor = 20;

        gBattleStruct->safariEscapeFactor += sGoNearCounterToEscapeFactor[gBattleStruct->safariGoNearCounter];
        if (gBattleStruct->safariEscapeFactor > 20)
            gBattleStruct->safariEscapeFactor = 20;

        if (gBattleStruct->safariGoNearCounter < 3)
        {
            gBattleStruct->safariGoNearCounter++;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CREPT_CLOSER;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CANT_GET_CLOSER;
        }
        gBattlescriptCurrInstr = gBattlescriptsForSafariActions[1];
    }

    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

#undef safariBaitThrowCounter
#undef safariRockThrowCounter

void HandleAction_SafariZoneRun(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    PlaySE(SE_FLEE);
    gCurrentTurnActionNumber = gBattlersCount;
    gBattleOutcome = B_OUTCOME_RAN;
}

void HandleAction_WallyBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker])

    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[3];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
    gActionsByTurnOrder[1] = B_ACTION_FINISHED;
}

void HandleAction_TryFinish(void)
{
    if (!HandleFaintedMonActions())
    {
        gBattleStruct->eventState.faintedAction = 0;
        gCurrentActionFuncId = B_ACTION_FINISHED;
    }
}

void HandleAction_NothingIsFainted(void)
{
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
}

void HandleAction_ActionFinished(void)
{
    u32 i, j;
    bool32 afterYouActive = gSpecialStatuses[gBattlerByTurnOrder[gCurrentTurnActionNumber + 1]].afterYou;
    gBattleStruct->monToSwitchIntoId[gBattlerByTurnOrder[gCurrentTurnActionNumber]] = gSelectedMonPartyId = PARTY_SIZE;
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    memset(&gSpecialStatuses, 0, sizeof(gSpecialStatuses));

    gCurrentMove = MOVE_NONE;
    ClearDamageCalcResults(); // Relies on gCurrentMove
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gBattleStruct->dynamicMoveType = 0;
    gBattleStruct->bouncedMoveIsUsed = FALSE;
    gBattleStruct->snatchedMoveIsUsed = FALSE;
    gBattleScripting.moveendState = 0;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleResources->battleScriptsStack->size = 0;

    if (GetConfig(B_RECALC_TURN_AFTER_ACTIONS) >= GEN_8
     && !afterYouActive
     && gBattleStruct->pledgeState != PLEDGE_COMBO_WAITING
     && !IsPursuitTargetSet())
    {
        // i starts at `gCurrentTurnActionNumber` because we don't want to recalculate turn order for mon that have already
        // taken action. It's been previously increased, which we want in order to not recalculate the turn of the mon that just finished its action

        struct BattleCalcValues calcValues = {0};
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            calcValues.abilities[battler] = GetBattlerAbility(battler);
            calcValues.holdEffects[battler] = GetBattlerHoldEffect(battler);
        }
        for (i = gCurrentTurnActionNumber; i < gBattlersCount - 1; i++)
        {
            for (j = i + 1; j < gBattlersCount; j++)
            {
                calcValues.battlerAtk = gBattlerByTurnOrder[i];
                calcValues.battlerDef = gBattlerByTurnOrder[j];

                if (gProtectStructs[calcValues.battlerAtk].quash || gProtectStructs[calcValues.battlerDef].quash
                    || gProtectStructs[calcValues.battlerAtk].shellTrap || gProtectStructs[calcValues.battlerDef].shellTrap)
                    continue;

                // We recalculate order only for action of the same priority. If any action other than switch/move has been taken, they should
                // have been executed before. The only recalculation needed is for moves/switch. Mega evolution is handled in src/battle_main.c/TryChangeOrder
                if ((gActionsByTurnOrder[i] == B_ACTION_USE_MOVE && gActionsByTurnOrder[j] == B_ACTION_USE_MOVE))
                {
                    if (GetWhichBattlerFaster(&calcValues, FALSE) == -1)
                        SwapTurnOrder(i, j);
                }
                else if ((gActionsByTurnOrder[i] == B_ACTION_SWITCH && gActionsByTurnOrder[j] == B_ACTION_SWITCH))
                {
                    if (GetWhichBattlerFaster(&calcValues, TRUE) == -1) // If the actions chosen are switching, we recalc order but ignoring the moves
                        SwapTurnOrder(i, j);
                }
            }
        }
    }
}

// code

ARM_FUNC NOINLINE static uq4_12_t PercentToUQ4_12(u32 percent)
{
    return (4096 * percent + 50) / 100;
}

ARM_FUNC NOINLINE static uq4_12_t PercentToUQ4_12_Floored(u32 percent)
{
    return (4096 * percent) / 100;
}

static inline uq4_12_t PercentToUQ4_12AddOne(u32 percent)
{
    return uq4_12_add(UQ_4_12(1.0), PercentToUQ4_12(percent));
}

enum BattlerId GetBattlerForBattleScript(u8 caseId)
{
    enum BattlerId ret = 0;
    switch (caseId)
    {
    case BS_TARGET:
        ret = gBattlerTarget;
        break;
    case BS_ATTACKER:
        ret = gBattlerAttacker;
        break;
    case BS_ATTACKER_PARTNER:
        ret = GetPartnerBattler(gBattlerAttacker);
        break;
    case BS_EFFECT_BATTLER:
        ret = gEffectBattler;
        break;
    case BS_BATTLER_0:
        ret = 0;
        break;
    case BS_SCRIPTING:
        ret = gBattleScripting.battler;
        break;
    case BS_FAINTED:
        ret = gBattlerFainted;
        break;
    case BS_FAINTED_MULTIPLE_1:
        ret = gBattlerFainted;
        break;
    case BS_FAINTED_MULTIPLE_2:
        ret = GetPartnerBattler(gBattlerFainted);
        break;
    case BS_ATTACKER_WITH_PARTNER:
    case BS_ATTACKER_SIDE:
    case BS_TARGET_SIDE:
    case BS_PLAYER1:
        ret = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        break;
    case BS_OPPONENT1:
        ret = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        break;
    case BS_PLAYER2:
        ret = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        break;
    case BS_OPPONENT2:
        ret = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        break;
    case BS_ABILITY_BATTLER:
        ret = gBattlerAbility;
        break;
    }
    return ret;
}

static void UNUSED MarkAllBattlersForControllerExec(void)
{
    enum BattlerId i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        for (i = 0; i < gBattlersCount; i++)
            MarkBattleControllerMessageOutboundOverLink(i);
    }
    else
    {
        for (i = 0; i < gBattlersCount; i++)
            MarkBattleControllerActiveOnLocal(i);
    }
}

bool32 IsBattlerMarkedForControllerExec(enum BattlerId battler)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        return IsBattleControllerMessageSynchronizedOverLink(battler);
    else
        return IsBattleControllerActiveOnLocal(battler);
}

void MarkBattlerForControllerExec(enum BattlerId battler)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        MarkBattleControllerMessageOutboundOverLink(battler);
    else
        MarkBattleControllerActiveOnLocal(battler);
}

void MarkBattlerReceivedLinkData(enum BattlerId battler)
{
    s32 i;

    for (i = 0; i < GetLinkPlayerCount(); i++)
        MarkBattleControllerActiveForPlayer(battler, i);

    MarkBattleControllerMessageSynchronizedOverLink(battler);
}

void CancelMultiTurnMoves(enum BattlerId battler)
{
    gBattleMons[battler].volatiles.uproarTurns = 0;
    gBattleMons[battler].volatiles.bideTurns = 0;
    gBattleMons[battler].volatiles.rolloutTimer = 0;
    gBattleMons[battler].volatiles.furyCutterCounter = 0;

    if (B_RAMPAGE_CONFUSION < GEN_5
     || gBattleMons[battler].volatiles.rampageTurns != 1) // Will be confused at the end of the turn
    {
        gLockedMoves[battler] = MOVE_NONE;
        gBattleMons[battler].volatiles.multipleTurns = 0;
        gBattleMons[battler].volatiles.rampageTurns = 0;
    }

    // Clear battler's semi-invulnerable bits if they are not held by Sky Drop.
    if (gBattleMons[battler].volatiles.semiInvulnerable != STATE_SKY_DROP_TARGET)
        gBattleMons[battler].volatiles.semiInvulnerable = STATE_NONE;

}

// Returns TRUE if no other battler after this one in turn order will use a move
bool32 IsLastMonToMove(enum BattlerId battler)
{
    u32 i;
    u32 battlerTurnOrderNum = GetBattlerTurnOrderNum(battler);

    if (battlerTurnOrderNum >= gBattlersCount - 1)
        return TRUE;

    for (i = battlerTurnOrderNum + 1; i < gBattlersCount; i++)
    {
        enum BattlerId otherBattler = gBattlerByTurnOrder[i];
        if (!IsBattlerAlive(otherBattler))
            continue;
        if (gActionsByTurnOrder[i] == B_ACTION_USE_MOVE)
            return FALSE;
    }
    return TRUE;
}

static u32 GetAiTurnOrder(u8 *aiTurnOrder, enum BattlerId battler)
{
    for (u32 i = 0; i < gBattlersCount; i++)
    {
        if (aiTurnOrder[i] == battler)
            return i;
    }
    return 0;
}

static bool32 Ai_AttackerMovesAfterTarget(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    u8 aiTurnOrder[4] = {0};
    AI_SetBattlerTurnOrder(aiTurnOrder);

    return GetAiTurnOrder(aiTurnOrder, battlerAtk) > GetAiTurnOrder(aiTurnOrder, battlerDef);
}

static bool32 Ai_AttackerMovesLast(enum BattlerId battlerAtk)
{
    u8 aiTurnOrder[4] = {0};
    AI_SetBattlerTurnOrder(aiTurnOrder);
    u32 numAliveBattlers = 0;
    u32 battlerTurnOrder = GetAiTurnOrder(aiTurnOrder, battlerAtk);

    for (enum BattlerId battler = B_BATTLER_0; battler < gBattlersCount; battler++)
    {
        if (IsBattlerAlive(battler))
            numAliveBattlers++;
    }

    if (battlerTurnOrder >= numAliveBattlers - 1)
        return TRUE;

    return FALSE;
}

void PrepareStringBattleWithWait(enum StringID stringId, enum BattlerId battler)
{
    PrepareStringBattle(stringId, battler);
    gBattleCommunication[MSG_DISPLAY] = MSG_DISPLAY_WAIT;
}

void PrepareStringBattle(enum StringID stringId, enum BattlerId battler)
{
    switch (stringId)
    {
    case STRINGID_ITDOESNTAFFECT:
    case STRINGID_PKMNUNAFFECTED:
        TryInitializeTrainerSlideMonUnaffected(gBattlerTarget, gBattlerAttacker);
        break;
    default:
        break;
    }

    BtlController_EmitPrintString(battler, B_COMM_TO_CONTROLLER, stringId);
    MarkBattlerForControllerExec(battler);
}

void ResetSentPokesToOpponentValue(void)
{
    s32 i;
    u32 bits = 0;

    gSentPokesToOpponent[0] = 0;
    gSentPokesToOpponent[1] = 0;

    for (i = 0; i < gBattlersCount; i += 2)
        bits |= 1u << gBattlerPartyIndexes[i];

    for (i = 1; i < gBattlersCount; i += 2)
        gSentPokesToOpponent[(i & BIT_FLANK) >> 1] = bits;
}

void OpponentSwitchInResetSentPokesToOpponentValue(enum BattlerId battler)
{
    s32 i = 0;
    u32 bits = 0;

    if (!IsOnPlayerSide(battler))
    {
        u8 flank = ((battler & BIT_FLANK) >> 1);
        gSentPokesToOpponent[flank] = 0;

        for (i = 0; i < gBattlersCount; i += 2)
        {
            if (!(gAbsentBattlerFlags & (1u << i)))
                bits |= 1u << gBattlerPartyIndexes[i];
        }
        gSentPokesToOpponent[flank] = bits;
    }
}

void UpdateSentPokesToOpponentValue(enum BattlerId battler)
{
    if (!IsOnPlayerSide(battler))
    {
        OpponentSwitchInResetSentPokesToOpponentValue(battler);
    }
    else
    {
        s32 i;
        for (i = 1; i < gBattlersCount; i++)
            gSentPokesToOpponent[(i & BIT_FLANK) >> 1] |= 1u << gBattlerPartyIndexes[battler];
    }
}

void BattleScriptPush(const u8 *bsPtr)
{
    assertf(gBattleResources->battleScriptsStack->size < ARRAY_COUNT(gBattleResources->battleScriptsStack->ptr), "attempted to push a battle script, but battleScriptsStack is full!")
    {
        return;
    }
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = bsPtr;
}

void BattleScriptPushCursor(void)
{
    assertf(gBattleResources->battleScriptsStack->size < ARRAY_COUNT(gBattleResources->battleScriptsStack->ptr), "attempted to push cursor, but battleScriptsStack is full!")
    {
        return;
    }
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = gBattlescriptCurrInstr;
}

void BattleScriptCall(const u8 *bsPtr)
{
    BattleScriptPushCursor();
    gBattlescriptCurrInstr = bsPtr;
}

void BattleScriptPop(void)
{
    if (gBattleResources->battleScriptsStack->size != 0)
        gBattlescriptCurrInstr = gBattleResources->battleScriptsStack->ptr[--gBattleResources->battleScriptsStack->size];
}

void BattleScriptExecute(const u8 *BS_ptr)
{
    gBattlescriptCurrInstr = BS_ptr;
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands_PopCallbacksStack;
    gCurrentActionFuncId = 0;
}

void BattleScriptPushCursorAndCallback(const u8 *BS_ptr)
{
    BattleScriptCall(BS_ptr);
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands;
}

bool32 IsGravityPreventingMove(enum Move move)
{
    if (!(gFieldStatuses & STATUS_FIELD_GRAVITY))
        return FALSE;

    return IsMoveGravityBanned(move);
}

bool32 IsHealBlockPreventingMove(enum BattlerId battler, enum Move move)
{
    if (!gBattleMons[battler].volatiles.healBlockTimer)
        return FALSE;

    return IsHealingMove(move);
}

bool32 IsBelchPreventingMove(enum BattlerId battler, enum Move move)
{
    if (GetMoveEffect(move) != EFFECT_BELCH)
        return FALSE;

    return (!GetBattlerPartyState(battler)->ateBerry && GetConfig(B_BELCH_SELECTABLE) < GEN_CHAMPIONS);
}

static bool32 SetCantSelectScript(enum BattlerId battler, enum Move move, const u8 *palaceScript, const u8 *script)
{
    gCurrentMove = move;

    if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
    {
        gPalaceSelectionBattleScripts[battler] = palaceScript;
        gProtectStructs[battler].palaceUnableToUseMove = TRUE;
        return FALSE;
    }
    else
    {
        gSelectionBattleScripts[battler] = script;
        return TRUE;
    }
}

static bool32 IsGimmickChosenForAction(enum BattlerId battler, enum Gimmick gimmick)
{
    return (gBattleResources->bufferB[battler][2] & RET_GIMMICK)
         && gBattleStruct->gimmick.usableGimmick[battler] == gimmick;
}

u32 TrySetCantSelectMoveBattleScript(enum BattlerId battler)
{
    u32 limitations = 0;
    u8 moveId = gBattleResources->bufferB[battler][2] & ~RET_GIMMICK;
    enum Move move = gBattleMons[battler].moves[moveId];
    enum HoldEffect holdEffect = GetBattlerHoldEffect(battler);
    u16 *choicedMove = &gBattleStruct->choicedMove[battler];
    enum BattleMoveEffects moveEffect = GetMoveEffect(move);

    // Dynamax bypasses all selection prevention except Taunt and Assault Vest.
    bool32 dynamaxBypassCheck = (!IsGimmickChosenForAction(battler, GIMMICK_DYNAMAX) && GetActiveGimmick(battler) != GIMMICK_DYNAMAX);

    // Z-Moves bypass the effects of disruption moves like Encore, Taunt, Disable
    bool32 zMoveBypassCheck = (!IsGimmickChosenForAction(battler, GIMMICK_Z_MOVE) && GetActiveGimmick(battler) != GIMMICK_Z_MOVE);

    if (GetConfig(B_ENCORE_TARGET) >= GEN_5
     && dynamaxBypassCheck && zMoveBypassCheck && gBattleMons[battler].volatiles.encoredMove != move && gBattleMons[battler].volatiles.encoredMove != MOVE_NONE)
    {
        gBattleScripting.battler = battler;
        limitations = SetCantSelectScript(battler, gBattleMons[battler].volatiles.encoredMove, BattleScript_EncoredMoveInPalace, BattleScript_EncoredMove);
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && gBattleMons[battler].volatiles.disabledMove == move && move != MOVE_NONE)
    {
        gBattleScripting.battler = battler;
        if (SetCantSelectScript(battler, gBattleMons[battler].volatiles.disabledMove, BattleScript_SelectingDisabledMoveInPalace, BattleScript_SelectingDisabledMove))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && move == gLastMoves[battler] && move != MOVE_STRUGGLE && (gBattleMons[battler].volatiles.torment == TRUE))
    {
        CancelMultiTurnMoves(battler);
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[battler] = BattleScript_SelectingTormentedMoveInPalace;
            gProtectStructs[battler].palaceUnableToUseMove = TRUE;
        }
        else
        {
            gSelectionBattleScripts[battler] = BattleScript_SelectingTormentedMove;
            limitations++;
        }
    }

    if (zMoveBypassCheck
     && gBattleMons[battler].volatiles.tauntTimer != 0
     && IsBattleMoveStatus(move)
     && (GetConfig(B_TAUNT_ME_FIRST) < GEN_5 || moveEffect != EFFECT_ME_FIRST))
    {
        if ((GetActiveGimmick(battler) == GIMMICK_DYNAMAX))
            gCurrentMove = MOVE_MAX_GUARD;
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveTauntInPalace, BattleScript_SelectingNotAllowedMoveTaunt))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && gBattleMons[battler].volatiles.throatChopTimer > 0 && IsSoundMove(move))
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveThroatChopInPalace, BattleScript_SelectingNotAllowedMoveThroatChop))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && GetImprisonedMovesCount(battler, move))
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingImprisonedMoveInPalace, BattleScript_SelectingImprisonedMove))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && IsGravityPreventingMove(move))
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveGravityInPalace, BattleScript_SelectingNotAllowedMoveGravity))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && IsHealBlockPreventingMove(battler, move))
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveHealBlockInPalace, BattleScript_SelectingNotAllowedMoveHealBlock))
            limitations++;
    }

    if (dynamaxBypassCheck && zMoveBypassCheck && IsBelchPreventingMove(battler, move))
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedBelchInPalace, BattleScript_SelectingNotAllowedBelch))
            limitations++;
    }

    if (dynamaxBypassCheck && moveEffect == EFFECT_STUFF_CHEEKS && GetItemPocket(gBattleMons[battler].item) != POCKET_BERRIES && GetConfig(B_STUFF_CHEEKS_SELECTABLE) < GEN_CHAMPIONS)
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedStuffCheeksInPalace, BattleScript_SelectingNotAllowedStuffCheeks))
            limitations++;
    }

    if (MoveCantBeUsedTwice(move) && move == gLastResultingMoves[battler])
    {
        PREPARE_MOVE_BUFFER(gBattleTextBuff1, gCurrentMove);
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedCurrentMoveInPalace, BattleScript_SelectingNotAllowedCurrentMove))
            limitations++;
    }

    // Unconfirmed: We're making an assumption that a Max Move variant of moves that otherwise result in
    // "This move can't be used!" can be used while Dynamaxed
    if (dynamaxBypassCheck
     && moveEffect == EFFECT_FIRST_TURN_ONLY
     && !IsBattlersFirstTurn(battler)
     && GetConfig(B_FIRST_TURN_MOVE) >= GEN_CHAMPIONS)
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingCantUseMoveInPalace, BattleScript_SelectingCantUseMove))
            limitations++;
    }

    if (dynamaxBypassCheck
     && moveEffect == EFFECT_SPIT_UP
     && gBattleMons[battler].volatiles.stockpileCounter == 0
     && GetConfig(B_SPIT_UP_SELECTABLE) >= GEN_CHAMPIONS)
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingCantUseMoveInPalace, BattleScript_SelectingCantUseMove))
            limitations++;
    }

    if (dynamaxBypassCheck
     && zMoveBypassCheck
     && moveEffect == EFFECT_FAIL_IF_NOT_ARG_TYPE
     && !IS_BATTLER_OF_TYPE(battler, GetMoveArgType(move))
     && GetConfig(B_MOVES_THAT_REMOVE_TYPE) >= GEN_CHAMPIONS)
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingCantUseMoveInPalace, BattleScript_SelectingCantUseMove))
            limitations++;
    }

    if (dynamaxBypassCheck
     && zMoveBypassCheck
     && moveEffect == EFFECT_LAST_RESORT
     && !CanUseLastResort(battler)
     && GetConfig(B_LAST_RESORT_SELECTABLE) >= GEN_CHAMPIONS)
    {
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingCantUseMoveInPalace, BattleScript_SelectingCantUseMove))
            limitations++;
    }

    gPotentialItemEffectBattler = battler;
    if (dynamaxBypassCheck && IsHoldEffectChoice(holdEffect) && *choicedMove != MOVE_NONE && *choicedMove != MOVE_UNAVAILABLE && *choicedMove != move)
    {
        gCurrentMove = *choicedMove;
        gLastUsedItem = gBattleMons[battler].item;
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveChoiceItemInPalace, BattleScript_SelectingNotAllowedMoveChoiceItem))
            limitations++;
    }
    else if (holdEffect == HOLD_EFFECT_ASSAULT_VEST && IsBattleMoveStatus(move) && moveEffect != EFFECT_ME_FIRST)
    {
        if ((GetActiveGimmick(battler) == GIMMICK_DYNAMAX))
            gCurrentMove = MOVE_MAX_GUARD;
        gLastUsedItem = gBattleMons[battler].item;
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveAssaultVestInPalace, BattleScript_SelectingNotAllowedMoveAssaultVest))
            limitations++;
    }
    if (dynamaxBypassCheck && (GetBattlerAbility(battler) == ABILITY_GORILLA_TACTICS) && *choicedMove != MOVE_NONE
              && *choicedMove != MOVE_UNAVAILABLE && *choicedMove != move)
    {
        gCurrentMove = *choicedMove;
        gLastUsedItem = gBattleMons[battler].item;
        if (SetCantSelectScript(battler, gCurrentMove, BattleScript_SelectingNotAllowedMoveGorillaTacticsInPalace, BattleScript_SelectingNotAllowedMoveGorillaTactics))
            limitations++;
    }

    if (gBattleMons[battler].pp[moveId] == 0)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gProtectStructs[battler].palaceUnableToUseMove = TRUE;
        }
        else
        {
            gSelectionBattleScripts[battler] = BattleScript_SelectingMoveWithNoPP;
            limitations++;
        }
    }

    if (moveEffect == EFFECT_PLACEHOLDER)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[battler] = BattleScript_SelectingNotAllowedPlaceholderInPalace;
            gProtectStructs[battler].palaceUnableToUseMove = TRUE;
        }
        else
        {
            gSelectionBattleScripts[battler] = BattleScript_SelectingNotAllowedPlaceholder;
            limitations++;
        }
    }

    return limitations;
}

u32 CheckMoveLimitations(enum BattlerId battler, u8 unusableMoves, u32 check)
{
    enum Move move;
    enum BattleMoveEffects moveEffect;
    enum HoldEffect holdEffect = GetBattlerHoldEffect(battler);
    u16 *choicedMove = &gBattleStruct->choicedMove[battler];
    s32 i;

    gPotentialItemEffectBattler = battler;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = gBattleMons[battler].moves[i];
        moveEffect = GetMoveEffect(move);
        // No move
        if (check & MOVE_LIMITATION_ZEROMOVE && move == MOVE_NONE)
            unusableMoves |= 1u << i;
        // No PP
        else if (check & MOVE_LIMITATION_PP && gBattleMons[battler].pp[i] == 0)
            unusableMoves |= 1u << i;
        // Placeholder
        else if (check & MOVE_LIMITATION_PLACEHOLDER && moveEffect == EFFECT_PLACEHOLDER)
            unusableMoves |= 1u << i;
        // Disable
        else if (check & MOVE_LIMITATION_DISABLED && move == gBattleMons[battler].volatiles.disabledMove)
            unusableMoves |= 1u << i;
        // Torment
        else if (check & MOVE_LIMITATION_TORMENTED && move == gLastMoves[battler] && gBattleMons[battler].volatiles.torment == TRUE)
            unusableMoves |= 1u << i;
        // Taunt
        else if (check & MOVE_LIMITATION_TAUNT
              && gBattleMons[battler].volatiles.tauntTimer
              && IsBattleMoveStatus(move)
              && (GetConfig(B_TAUNT_ME_FIRST) < GEN_5 || moveEffect != EFFECT_ME_FIRST))
            unusableMoves |= 1u << i;
        // Imprison
        else if (check & MOVE_LIMITATION_IMPRISON && GetImprisonedMovesCount(battler, move))
            unusableMoves |= 1u << i;
        // Encore
        else if (check & MOVE_LIMITATION_ENCORE && gBattleMons[battler].volatiles.encoreTimer && gBattleMons[battler].volatiles.encoredMove != move)
            unusableMoves |= 1u << i;
        // Choice Items
        else if (check & MOVE_LIMITATION_CHOICE_ITEM && IsHoldEffectChoice(holdEffect) && *choicedMove != MOVE_NONE && *choicedMove != MOVE_UNAVAILABLE && *choicedMove != move)
            unusableMoves |= 1u << i;
        // Assault Vest
        else if (check & MOVE_LIMITATION_ASSAULT_VEST && holdEffect == HOLD_EFFECT_ASSAULT_VEST && IsBattleMoveStatus(move) && moveEffect != EFFECT_ME_FIRST)
            unusableMoves |= 1u << i;
        // Gravity
        else if (check & MOVE_LIMITATION_GRAVITY && IsGravityPreventingMove(move))
            unusableMoves |= 1u << i;
        // Heal Block
        else if (check & MOVE_LIMITATION_HEAL_BLOCK && IsHealBlockPreventingMove(battler, move))
            unusableMoves |= 1u << i;
        // Belch
        else if (check & MOVE_LIMITATION_BELCH && IsBelchPreventingMove(battler, move))
            unusableMoves |= 1u << i;
        // Throat Chop
        else if (check & MOVE_LIMITATION_THROAT_CHOP && gBattleMons[battler].volatiles.throatChopTimer > 0 && IsSoundMove(move))
            unusableMoves |= 1u << i;
        // Stuff Cheeks
        else if (check & MOVE_LIMITATION_STUFF_CHEEKS && moveEffect == EFFECT_STUFF_CHEEKS && GetItemPocket(gBattleMons[battler].item) != POCKET_BERRIES && GetConfig(B_STUFF_CHEEKS_SELECTABLE) < GEN_CHAMPIONS)
            unusableMoves |= 1u << i;
        // Gorilla Tactics
        else if (check & MOVE_LIMITATION_CHOICE_ITEM && GetBattlerAbility(battler) == ABILITY_GORILLA_TACTICS && *choicedMove != MOVE_NONE && *choicedMove != MOVE_UNAVAILABLE && *choicedMove != move)
            unusableMoves |= 1u << i;
        // Can't Use Twice flag
        else if (check & MOVE_LIMITATION_CANT_USE_TWICE && MoveCantBeUsedTwice(move) && move == gLastResultingMoves[battler])
            unusableMoves |= 1u << i;
        else if (check & MOVE_LIMITATION_UNUSABLE)
        {
            switch (moveEffect)
            {
                // Fake Out, First Impression
                case EFFECT_FIRST_TURN_ONLY:
                    if (!IsBattlersFirstTurn(battler) && GetConfig(B_FIRST_TURN_MOVE) >= GEN_CHAMPIONS)
                        unusableMoves |= 1u << i;
                    break;
                // Spit Up
                case EFFECT_SPIT_UP:
                    if (gBattleMons[battler].volatiles.stockpileCounter == 0 && GetConfig(B_SPIT_UP_SELECTABLE) >= GEN_CHAMPIONS)
                        unusableMoves |= 1u << i;
                    break;
                // Burn Up, Double Shock
                case EFFECT_FAIL_IF_NOT_ARG_TYPE:
                    if (!IS_BATTLER_OF_TYPE(battler, GetMoveArgType(move)) && GetConfig(B_MOVES_THAT_REMOVE_TYPE) >= GEN_CHAMPIONS)
                        unusableMoves |= 1u << i;
                    break;
                // Last Resort
                case EFFECT_LAST_RESORT:
                    if (!CanUseLastResort(battler) && GetConfig(B_LAST_RESORT_SELECTABLE) >= GEN_CHAMPIONS)
                        unusableMoves |= 1u << i;
                    break;
                default:
                    break;
            }
        }
    }
    return unusableMoves;
}

bool32 AreAllMovesUnusable(enum BattlerId battler)
{
    u32 unusable = CheckMoveLimitations(battler, 0, MOVE_LIMITATIONS_ALL);
    u32 allMovesMask = ((1 << MAX_MON_MOVES) - 1);

    if (unusable == allMovesMask) // All moves are unusable.
    {
        gProtectStructs[battler].noValidMoves = TRUE;
        gSelectionBattleScripts[battler] = BattleScript_NoMovesLeft;
    }
    else
    {
        gProtectStructs[battler].noValidMoves = FALSE;
    }

    return (unusable == allMovesMask);
}

u8 GetImprisonedMovesCount(enum BattlerId battler, enum Move move)
{
    s32 i;
    u8 imprisonedMoves = 0;
    u32 battlerSide = GetBattlerSide(battler);

    for (i = 0; i < gBattlersCount; i++)
    {
        if (battlerSide != GetBattlerSide(i) && gBattleMons[i].volatiles.imprison)
        {
            s32 j;
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (move == gBattleMons[i].moves[j])
                    break;
            }
            if (j < MAX_MON_MOVES)
                imprisonedMoves++;
        }
    }

    return imprisonedMoves;
}

u32 GetBattlerAffectionHearts(enum BattlerId battler)
{
    if (!IsOnPlayerSide(battler)
     || gBattleStruct->battlerState[battler].notOnField
     || gSpecialStatuses[battler].attackerInParty)
        return AFFECTION_NO_HEARTS;

    if (gSpeciesInfo[gBattleMons[battler].species].isMegaEvolution
          || (gBattleTypeFlags & (BATTLE_TYPE_EREADER_TRAINER
                                | BATTLE_TYPE_FRONTIER
                                | BATTLE_TYPE_LINK
                                | BATTLE_TYPE_RECORDED_LINK
                                | BATTLE_TYPE_SECRET_BASE)))
        return AFFECTION_NO_HEARTS;

    return gBattleMons[battler].affectionHearts;
}

// gBattlerAttacker is the battler that's trying to raise their stats and due to limitations of RandomUniformExcept, cannot be an argument
bool32 MoodyCantRaiseStat(u32 stat)
{
    return CompareStat(gBattlerAttacker, stat, MAX_STAT_STAGE, CMP_EQUAL, GetBattlerAbility(gBattlerAttacker));
}

// gBattlerAttacker is the battler that's trying to lower their stats and due to limitations of RandomUniformExcept, cannot be an argument
bool32 MoodyCantLowerStat(u32 stat)
{
    return stat == gSpecialStatuses[gBattlerAttacker].statStageQueue[0].stat
        || CompareStat(gBattlerAttacker, stat, MIN_STAT_STAGE, CMP_EQUAL, GetBattlerAbility(gBattlerAttacker));
}

void TryToRevertMimicryAndFlags(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        gBattleMons[battler].volatiles.terrainAbilityDone = FALSE;
        ResetParadoxTerrainStat(battler);
        if (IsAbilityAndRecord(battler, GetBattlerAbility(battler), ABILITY_MIMICRY))
            RESTORE_BATTLER_TYPE(battler);
    }
}

// Ingrain, Leech Seed, Strength Sap and Aqua Ring
s32 GetDrainedBigRootHp(enum BattlerId battler, s32 hp)
{
    if (GetBattlerHoldEffect(battler) == HOLD_EFFECT_BIG_ROOT)
        hp = (hp * 1300) / 1000;
    if (hp == 0)
        hp = 1;

    return hp;
}

// Should always be the last check. Otherwise the ability might be wrongly recorded.
bool32 IsAbilityAndRecord(enum BattlerId battler, enum Ability battlerAbility, enum Ability abilityToCheck)
{
    if (battlerAbility != abilityToCheck)
        return FALSE;

    RecordAbilityBattle(battler, abilityToCheck);
    return TRUE;
}

bool32 HandleFaintedMonActions(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
        return FALSE;

    do
    {
        switch (gBattleStruct->eventState.faintedAction)
        {
        case FAINTED_ACTIONS_NO_MONS_TO_SWITCH:
            gBattleStruct->eventState.faintedActionBattler = 0;
            gBattleStruct->eventState.faintedAction++;
            for (enum BattlerId i = 0; i < gBattlersCount; i++)
            {
                if (gAbsentBattlerFlags & (1u << i) && !HasNoMonsToSwitch(i, PARTY_SIZE, PARTY_SIZE))
                    gAbsentBattlerFlags &= ~(1u << i);
            }
            // fall through
        case FAINTED_ACTIONS_GIVE_EXP:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->eventState.faintedActionBattler;
                if (gBattleMons[gBattlerFainted].hp == 0
                 && !(gBattleStruct->givenExpMons[GetBattlerTrainer(gBattlerFainted) & BIT_FLANK] & (1u << gBattlerPartyIndexes[gBattlerFainted]))
                 && !(gAbsentBattlerFlags & (1u << gBattlerFainted)))
                {
                    BattleScriptExecute(BattleScript_GiveExp);
                    gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_SET_ABSENT_FLAGS;
                    return TRUE;
                }
            } while (++gBattleStruct->eventState.faintedActionBattler != gBattlersCount);
            gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_WAIT_STATE;
            break;
        case FAINTED_ACTIONS_SET_ABSENT_FLAGS:
            OpponentSwitchInResetSentPokesToOpponentValue(gBattlerFainted);
            if (++gBattleStruct->eventState.faintedActionBattler == gBattlersCount)
                gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_WAIT_STATE;
            else
                gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_GIVE_EXP;
            // Don't switch mons until all Pokémon performed their actions or the battle's over.
            if (B_FAINT_SWITCH_IN >= GEN_4
                && gBattleOutcome == 0
                && !NoAliveMonsForEitherParty()
                && gCurrentTurnActionNumber != gBattlersCount)
            {
                gAbsentBattlerFlags |= 1u << gBattlerFainted;
                if (gBattleStruct->eventState.faintedAction != FAINTED_ACTIONS_GIVE_EXP)
                    return FALSE;
            }
            break;
        case FAINTED_ACTIONS_WAIT_STATE:
            // Don't switch mons until all Pokémon performed their actions or the battle's over.
            if (B_FAINT_SWITCH_IN >= GEN_4
                && gBattleOutcome == 0
                && !NoAliveMonsForEitherParty()
                && gCurrentTurnActionNumber != gBattlersCount)
            {
                return FALSE;
            }
            gBattleStruct->eventState.faintedActionBattler = 0;
            gBattleStruct->eventState.faintedAction++;
            // fall through
        case FAINTED_ACTIONS_HANDLE_FAINTED_MON:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->eventState.faintedActionBattler;
                if (gBattleMons[gBattleStruct->eventState.faintedActionBattler].hp == 0
                 && !(gAbsentBattlerFlags & (1u << gBattleStruct->eventState.faintedActionBattler)))
                {
                    BattleScriptExecute(BattleScript_HandleFaintedMon);
                    gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_HANDLE_NEXT_BATTLER;
                    return TRUE;
                }
            } while (++gBattleStruct->eventState.faintedActionBattler != gBattlersCount);
            gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_MAX_CASE;
            break;
        case FAINTED_ACTIONS_HANDLE_NEXT_BATTLER:
            if (++gBattleStruct->eventState.faintedActionBattler == gBattlersCount)
                gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_MAX_CASE;
            else
                gBattleStruct->eventState.faintedAction = FAINTED_ACTIONS_HANDLE_FAINTED_MON;
            break;
        case FAINTED_ACTIONS_MAX_CASE:
            break;
        }
    } while (gBattleStruct->eventState.faintedAction != FAINTED_ACTIONS_MAX_CASE);
    return FALSE;
}

bool32 HasNoMonsToSwitch(enum BattlerId battler, u8 partyIdBattlerOn1, u8 partyIdBattlerOn2)
{
    u32 i, playerId, flankId;
    s32 lastId = GetAILastPartyIndex(battler); // + 1
    struct Pokemon *party = GetBattlerParty(battler);


    if (!IsDoubleBattle())
        return FALSE;

    bool32 isPlayerSide = IsOnPlayerSide(battler);

    if (BATTLE_TWO_VS_ONE_OPPONENT && !isPlayerSide)
    {
        flankId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        playerId = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);

        // Edge case: If both opposing Pokemon were knocked out on the same turn,
        // make sure opponent only sends out the final Pokemon once.
        if (battler == playerId
         && (gHitMarker & HITMARKER_FAINTED(flankId))
         && (gHitMarker & HITMARKER_FAINTED(playerId)))
        {
            u8 count = 0;
            for (i = 0; i < lastId; i++)
                if (IsValidForBattle(&party[i]))
                    count++;

            if (count < 2)
                return TRUE;
        }

        if (partyIdBattlerOn1 == PARTY_SIZE)
            partyIdBattlerOn1 = gBattlerPartyIndexes[flankId];
        if (partyIdBattlerOn2 == PARTY_SIZE)
            partyIdBattlerOn2 = gBattlerPartyIndexes[playerId];

        for (i = 0; i < lastId; i++)
        {
            if (IsValidForBattle(&party[i])
             && i != partyIdBattlerOn1 && i != partyIdBattlerOn2
             && i != gBattleStruct->monToSwitchIntoId[flankId] && i != playerId[gBattleStruct->monToSwitchIntoId])
                break;
        }
        return (i == lastId);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
    {
        if (!isPlayerSide && WILD_DOUBLE_BATTLE)
        {
            flankId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            playerId = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);

            if (partyIdBattlerOn1 == PARTY_SIZE)
                partyIdBattlerOn1 = gBattlerPartyIndexes[flankId];
            if (partyIdBattlerOn2 == PARTY_SIZE)
                partyIdBattlerOn2 = gBattlerPartyIndexes[playerId];

            for (i = 0; i < lastId; i++)
            {
                if (IsValidForBattle(&party[i])
                 && i != partyIdBattlerOn1 && i != partyIdBattlerOn2
                 && i != gBattleStruct->monToSwitchIntoId[flankId] && i != playerId[gBattleStruct->monToSwitchIntoId])
                    break;
            }
            return (i == lastId);
        }
        else
        {
            for (i = 0; i < lastId; i++)
            {
                if (IsValidForBattle(&party[i]))
                    break;
            }
            return (i == lastId);
        }
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        for (i = 0; i < lastId; i++)
        {
            if (IsValidForBattle(&party[i]))
                break;
        }
        return (i == lastId);
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) && !isPlayerSide)
    {
        for (i = 0; i < lastId; i++)
        {
            if (IsValidForBattle(&party[i]))
                break;
        }
        return (i == lastId);
    }
    else
    {
        if (!isPlayerSide)
        {
            flankId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            playerId = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        }
        else
        {
            flankId = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            playerId = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        }

        if (partyIdBattlerOn1 == PARTY_SIZE)
            partyIdBattlerOn1 = gBattlerPartyIndexes[flankId];
        if (partyIdBattlerOn2 == PARTY_SIZE)
            partyIdBattlerOn2 = gBattlerPartyIndexes[playerId];

        for (i = 0; i < lastId; i++)
        {
            if (IsValidForBattle(&party[i])
             && i != partyIdBattlerOn1 && i != partyIdBattlerOn2
             && i != gBattleStruct->monToSwitchIntoId[flankId] && i != playerId[gBattleStruct->monToSwitchIntoId])
                break;
        }
        return (i == lastId);
    }
}

static bool32 TryChangeWeatherWithAbility(enum BattlerId battler, u32 battleWeather, enum Ability ability)
{
    switch (TryChangeBattleWeather(battler, battleWeather, ability))
    {
    case WEATHER_FAILURE_SAME_WEATHER:
        if (ability == ABILITY_ORICHALCUM_PULSE)
        {
            BattleScriptCall(BattleScript_OrichalcumPulseActivatesInSun);
            return TRUE;
        }
        return FALSE;
    case WEATHER_FAILURE_OVERWORLD:
        BattleScriptCall(BattleScript_BlockedByOverworldWeather);
        return TRUE;
    case WEATHER_FAILURE_PRIMAL:
        BattleScriptCall(BattleScript_BlockedByPrimalWeather);
        return TRUE;
    case WEATHER_FAILURE_SUCCESS:
        if (ability == ABILITY_ORICHALCUM_PULSE)
            BattleScriptCall(BattleScript_OrichalcumPulseActivates);
        else
            BattleScriptCall(BattleScript_WeatherAbilityActivates);
        return TRUE;
    }
    return FALSE;
}

enum WeatherFailure TryChangeBattleWeather(enum BattlerId battler, u32 battleWeatherId, enum Ability ability)
{
    if (gBattleWeather & gBattleWeatherInfo[battleWeatherId].flag)
        return WEATHER_FAILURE_SAME_WEATHER;

    if (gBattleStruct->overworldWeatherPresent)
        return WEATHER_FAILURE_OVERWORLD;

    if (gBattleWeather & B_WEATHER_PRIMAL_ANY
          && ability != ABILITY_DESOLATE_LAND
          && ability != ABILITY_PRIMORDIAL_SEA
          && ability != ABILITY_DELTA_STREAM)
    {
        return WEATHER_FAILURE_PRIMAL;
    }

    if (GetConfig(B_ABILITY_WEATHER) < GEN_6 && ability != ABILITY_NONE)
    {
        gBattleWeather = gBattleWeatherInfo[battleWeatherId].flag;
    }
    else
    {
        u32 rock = gBattleWeatherInfo[battleWeatherId].rock;
        gBattleWeather = gBattleWeatherInfo[battleWeatherId].flag;

        if (gBattleWeather & B_WEATHER_PRIMAL_ANY)
            gBattleStruct->weatherDuration = 0;
        else if (rock != 0 && GetBattlerHoldEffect(battler) == rock)
            gBattleStruct->weatherDuration = 8;
        else
            gBattleStruct->weatherDuration = 5;
    }

    if (ability != ABILITY_NONE) // Weather started by Ability
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleWeatherInfo[battleWeatherId].abilityStartMessage;
        gBattleScripting.animArg1 = gBattleWeatherInfo[battleWeatherId].animation;
    }
    else // Weather started by Move
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleWeatherInfo[battleWeatherId].moveStartMessage;
    }

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        gBattleMons[i].volatiles.weatherAbilityDone = FALSE;
        ResetParadoxWeatherStat(i);
    }

    return WEATHER_FAILURE_SUCCESS;
}

bool32 TryChangeBattleTerrain(enum BattlerId battler, enum BattleTerrain terrain)
{
    if (terrain == B_TERRAIN_NONE || terrain == gFieldTimers.terrain)
        return FALSE;

    if (gBattleStruct->isSkyBattle)
        return FALSE;

    if (gFieldTimers.terrain != terrain)
    {
        gFieldTimers.terrain = terrain;
        gBattleCommunication[MULTISTRING_CHOOSER] = gBattleTerrainInfo[terrain].startMessage;

        for (enum BattlerId i = 0; i < gBattlersCount; i++)
        {
            gBattleMons[i].volatiles.terrainAbilityDone = FALSE;
            ResetParadoxTerrainStat(i);
        }
        if (GetBattlerHoldEffect(battler) == HOLD_EFFECT_TERRAIN_EXTENDER)
            gFieldTimers.terrainTimer = 8;
        else
            gFieldTimers.terrainTimer = 5;
        gBattleScripting.battler = battler;
        return TRUE;
    }

    return FALSE;
}

static void ForewarnChooseMove(enum BattlerId battler)
{
    struct Forewarn {
        enum BattlerId battler;
        u8 power;
        enum Move moveId;
    };
    u32 i, j, bestId, count;
    struct Forewarn *data = Alloc(sizeof(struct Forewarn) * MAX_BATTLERS_COUNT * MAX_MON_MOVES);

    // Put all moves
    for (count = 0, i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        if (IsBattlerAlive(i) && !IsBattlerAlly(i, battler))
        {
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (gBattleMons[i].moves[j] == MOVE_NONE)
                    continue;
                data[count].moveId = gBattleMons[i].moves[j];
                data[count].battler = i;
                switch (GetMoveEffect(data[count].moveId))
                {
                case EFFECT_OHKO:
                    data[count].power = 150;
                    break;
                case EFFECT_REFLECT_DAMAGE:
                    data[count].power = 120;
                    break;
                default:
                {
                    u32 movePower = GetMovePower(data[count].moveId);
                    if (movePower == 1)
                        data[count].power = 80;
                    else
                        data[count].power = movePower;
                    break;
                }
                }
                count++;
            }
        }
    }

    if (count == 0)
    {
        Free(data);
        return;
    }

    u32 tieCount = 1;
    u8 bestPower = data[0].power;

    bestId = 0;
    for (i = 1; i < count; i++)
    {
        if (data[i].power > bestPower)
        {
            bestPower = data[i].power;
            bestId = i;
            tieCount = 1;
        }
        else if (data[i].power == bestPower)
        {
            tieCount++;
        }
    }

    if (tieCount > 1)
    {
        u32 tieIndex = RandomUniform(RNG_FOREWARN, 0, tieCount - 1);
        for (i = 0, bestId = 0; i < count; i++)
        {
            if (data[i].power != bestPower)
                continue;
            if (tieIndex-- == 0)
            {
                bestId = i;
                break;
            }
        }
    }

    gEffectBattler = data[bestId].battler;
    PREPARE_MOVE_BUFFER(gBattleTextBuff1, data[bestId].moveId)
    RecordKnownMove(data[bestId].battler, data[bestId].moveId);

    Free(data);
}

bool32 ChangeTypeBasedOnTerrain(enum BattlerId battler)
{
    enum Type battlerType = gBattleTerrainInfo[gFieldTimers.terrain].type;

    SET_BATTLER_TYPE(battler, battlerType);
    PREPARE_TYPE_BUFFER(gBattleTextBuff1, battlerType);
    return TRUE;
}

// Supreme Overlord adds a x0.1 damage boost for each fainted ally.
static inline uq4_12_t GetSupremeOverlordModifier(enum BattlerId battler)
{
    return UQ_4_12(1.0) + (PercentToUQ4_12(gBattleMons[battler].volatiles.supremeOverlordCounter * 10));
}

bool32 HadMoreThanHalfHpNowDoesnt(enum BattlerId battler)
{
    // Had more than half of hp before, now has less
    return gBattleStruct->battlerState[battler].wasAboveHalfHp
 …67201 tokens truncated…tler].state != ILLUSION_ON)
        return FALSE;
    if (ability == ABILITY_ILLUSION && !IsBattlerTurnDamaged(battler, EXCLUDING_SUBSTITUTES))
        return FALSE;

    gBattleScripting.battler = battler;
    BattleScriptCall(BattleScript_IllusionOff);
    return TRUE;
}

struct Pokemon *GetIllusionMonPtr(enum BattlerId battler)
{
    if (gBattleStruct->illusion[battler].state == ILLUSION_NOT_SET)
        SetIllusionMon(GetBattlerMon(battler), battler);
    if (gBattleStruct->illusion[battler].state != ILLUSION_ON)
        return NULL;

    return gBattleStruct->illusion[battler].mon;
}

void ClearIllusionMon(enum BattlerId battler)
{
    memset(&gBattleStruct->illusion[battler], 0, sizeof(gBattleStruct->illusion[battler]));
}

enum Species GetIllusionMonSpecies(enum BattlerId battler)
{
    struct Pokemon *illusionMon = GetIllusionMonPtr(battler);
    if (illusionMon != NULL)
        return GetMonData(illusionMon, MON_DATA_SPECIES);
    return SPECIES_NONE;
}

u32 GetIllusionMonPartyId(struct Pokemon *party, struct Pokemon *mon, struct Pokemon *partnerMon, enum BattlerId battler)
{
    // Find last alive non-egg Pokémon.
    for (s32 id = PARTY_SIZE - 1; id >= 0; id--)
    {
        if (GetMonData(&party[id], MON_DATA_SANITY_HAS_SPECIES)
            && GetMonData(&party[id], MON_DATA_HP)
            && !GetMonData(&party[id], MON_DATA_IS_EGG))
        {
            enum Species species = GetMonData(&party[id], MON_DATA_SPECIES);
            if (species == SPECIES_TERAPAGOS_STELLAR || (species >= SPECIES_OGERPON_TEAL_TERA && species <= SPECIES_OGERPON_CORNERSTONE_TERA))
                continue;
            if (&party[id] != mon && &party[id] != partnerMon)
                return id;
            else // If this Pokémon or its partner is last in the party, ignore Illusion.
                return PARTY_SIZE;
        }
    }
    return PARTY_SIZE;
}

void SetIllusionMon(struct Pokemon *mon, enum BattlerId battler)
{
    struct Pokemon *party, *partnerMon;
    u32 id;

    gBattleStruct->illusion[battler].state = ILLUSION_OFF;
    if (GetMonAbility(mon) != ABILITY_ILLUSION)
        return;

    party = GetBattlerParty(battler);

    if (IsBattlerAlive(GetPartnerBattler(battler)))
        partnerMon = GetBattlerMon(GetPartnerBattler(battler));
    else
        partnerMon = mon;

    id = GetIllusionMonPartyId(party, mon, partnerMon, battler);
    if (id != PARTY_SIZE)
    {
        gBattleStruct->illusion[battler].state = ILLUSION_ON;
        gBattleStruct->illusion[battler].mon = &party[id];
    }
}

enum ImmunityHealStatusOutcome TryImmunityAbilityHealStatus(enum BattlerId battler)
{
    enum ImmunityHealStatusOutcome outcome = IMMUNITY_NO_EFFECT;
    switch (GetBattlerAbilityIgnoreMoldBreaker(battler))
    {
    case ABILITY_IMMUNITY:
    case ABILITY_PASTEL_VEIL:
        if (gBattleMons[battler].status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON | STATUS1_TOXIC_COUNTER))
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_POISON;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        break;
    case ABILITY_OWN_TEMPO:
        if (gBattleMons[battler].volatiles.confusionTimer > 0)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_CONFUSION;
            outcome = IMMUNITY_CONFUSION_CLEARED;
        }
        break;
    case ABILITY_LIMBER:
        if (gBattleMons[battler].status1 & STATUS1_PARALYSIS)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_PARALYSIS;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        break;
    case ABILITY_INSOMNIA:
    case ABILITY_VITAL_SPIRIT:
        if (gBattleMons[battler].status1 & STATUS1_SLEEP)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_SLEEP;
            TryDeactivateSleepClause(battler, gBattlerPartyIndexes[battler]);
            gBattleMons[battler].volatiles.nightmare = FALSE;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        break;
    case ABILITY_WATER_VEIL:
    case ABILITY_WATER_BUBBLE:
    case ABILITY_THERMAL_EXCHANGE:
        if (gBattleMons[battler].status1 & STATUS1_BURN)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_BURN;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        break;
    case ABILITY_MAGMA_ARMOR:
        if (gBattleMons[battler].status1 & STATUS1_FREEZE)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FREEZE;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        else if (gBattleMons[battler].status1 & STATUS1_FROSTBITE)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FROSTBITE;
            outcome = IMMUNITY_STATUS_CLEARED;
        }
        break;
    case ABILITY_OBLIVIOUS:
        if (gBattleMons[battler].volatiles.infatuation)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_INFATUATION;
            outcome = IMMUNITY_INFATUATION_CLEARED;
        }
        else if (GetConfig(B_OBLIVIOUS_TAUNT) >= GEN_6 && gBattleMons[battler].volatiles.tauntTimer != 0)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_TAUNT;
            outcome = IMMUNITY_TAUNT_CLEARED;
        }
        break;
    default:
        break;
    }

    switch (outcome)
    {
    case IMMUNITY_STATUS_CLEARED:
        gBattleMons[battler].status1 = 0;
        BattleScriptCall(BattleScript_AbilityCuredStatus);
        break;
    case IMMUNITY_CONFUSION_CLEARED:
        gBattleMons[battler].volatiles.confusionTimer = 0;
        BattleScriptCall(BattleScript_AbilityCuredStatus);
        break;
    case IMMUNITY_INFATUATION_CLEARED:
        gBattleMons[battler].volatiles.infatuation = 0;
        BattleScriptCall(BattleScript_AbilityCuredStatus);
        break;
    case IMMUNITY_TAUNT_CLEARED:
        gBattleMons[battler].volatiles.tauntTimer = 0;
        BattleScriptCall(BattleScript_AbilityCuredStatus);
        break;
    case IMMUNITY_NO_EFFECT:
        return IMMUNITY_NO_EFFECT;
    }

    gBattleScripting.battler = gBattlerAbility = battler;
    if (outcome == IMMUNITY_STATUS_CLEARED)
    {
        BtlController_EmitSetMonData(battler, B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battler].status1);
        MarkBattlerForControllerExec(battler);
    }
    return outcome;
}

uq4_12_t GetBadgeBoostModifier(void)
{
    if (GetConfig(B_BADGE_BOOST) < GEN_3)
        return UQ_4_12(1.125);
    else
        return UQ_4_12(1.1);
}

bool32 ShouldGetStatBadgeBoost(u16 badgeFlag, enum BattlerId battler)
{
    if (GetConfig(B_BADGE_BOOST) <= GEN_3 && badgeFlag != 0)
    {
        if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_FRONTIER))
            return FALSE;
        else if (!IsOnPlayerSide(battler))
            return FALSE;
        else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER && TRAINER_BATTLE_PARAM.opponentA == TRAINER_SECRET_BASE)
            return FALSE;
        else if (FlagGet(badgeFlag))
            return TRUE;
    }
    return FALSE;
}

enum DamageCategory GetBattleMoveCategory(enum Move move)
{
    bool32 inBattle = gBattleStruct != NULL;

    if (inBattle && gBattleStruct->dynamicMoveCategory != DAMAGE_CATEGORY_NONE)
        return gBattleStruct->dynamicMoveCategory;

    if (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS)
        return DAMAGE_CATEGORY_STATUS;

    if (B_PHYSICAL_SPECIAL_SPLIT < GEN_4)
    {
        if (inBattle)
            return gTypesInfo[GetBattleMoveType(move)].damageCategory;
        else
            return gTypesInfo[GetMoveType(move)].damageCategory;
    }

    return GetMoveCategory(move);
}

void SetDynamicMoveCategory(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Move move)
{
    gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;

    switch (GetMoveEffect(move))
    {
    case EFFECT_PHOTON_GEYSER:
        if (GetCategoryBasedOnStats(battlerAtk) == DAMAGE_CATEGORY_PHYSICAL)
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_PHYSICAL;
        else
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_SPECIAL;
        break;
    case EFFECT_SHELL_SIDE_ARM:
        gBattleStruct->dynamicMoveCategory = gBattleStruct->shellSideArmCategory[battlerAtk][battlerDef];
        break;
    case EFFECT_TERA_BLAST:
        if (GetActiveGimmick(battlerAtk) == GIMMICK_TERA)
        {
            if (GetCategoryBasedOnStats(battlerAtk) == DAMAGE_CATEGORY_PHYSICAL)
                gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_PHYSICAL;
            else
                gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_SPECIAL;
        }
        else
        {
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;
        }
        break;
    case EFFECT_TERA_STARSTORM:
        if (GetActiveGimmick(battlerAtk) == GIMMICK_TERA && GET_BASE_SPECIES_ID(GetMonData(GetBattlerMon(battlerAtk), MON_DATA_SPECIES)) == SPECIES_TERAPAGOS)
        {
            if (GetCategoryBasedOnStats(battlerAtk) == DAMAGE_CATEGORY_PHYSICAL)
                gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_PHYSICAL;
            else
                gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_SPECIAL;
        }
        else
        {
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;
        }
        break;
    case EFFECT_PRESENT:
    {
        gBattleStruct->presentBasePower = 0;
        u32 rand = RandomUniform(RNG_PRESENT, 0, 0xFF);
        if (rand < 102)
            gBattleStruct->presentBasePower = 40;
        else if (rand < 178)
            gBattleStruct->presentBasePower = 80;
        else if (rand < 204)
            gBattleStruct->presentBasePower = 120;
        else
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_STATUS;
        break;
    }
    default:
        if (GetActiveGimmick(battlerAtk) == GIMMICK_DYNAMAX)
            gBattleStruct->dynamicMoveCategory = GetMoveCategory(GetBattlerChosenMove(battlerAtk));
        break;
    }
}

static bool32 TryRemoveScreens(enum BattlerId battler)
{
    bool32 removed = FALSE;
    u32 battlerSide = GetBattlerSide(battler);
    u8 enemySide = GetBattlerSide(GetOppositeBattler(battler));

    // try to remove from battler's side
    if (gSideStatuses[battlerSide] & SIDE_STATUS_SCREEN_ANY)
    {
        gSideStatuses[battlerSide] &= ~SIDE_STATUS_SCREEN_ANY;
        removed = TRUE;
    }

    // try to remove from battler opponent's side
    if (gSideStatuses[enemySide] & SIDE_STATUS_SCREEN_ANY)
    {
        gSideStatuses[enemySide] &= ~SIDE_STATUS_SCREEN_ANY;
        removed = TRUE;
    }

    return removed;
}

// Photon Geyser, Light That Burns the Sky, Tera Blast
enum DamageCategory GetCategoryBasedOnStats(enum BattlerId battler)
{
    u32 attack = gBattleMons[battler].attack;
    u32 spAttack = gBattleMons[battler].spAttack;

    attack = attack * gStatStageRatios[gBattleMons[battler].statStages[STAT_ATK]][0];
    attack = attack / gStatStageRatios[gBattleMons[battler].statStages[STAT_ATK]][1];

    spAttack = spAttack * gStatStageRatios[gBattleMons[battler].statStages[STAT_SPATK]][0];
    spAttack = spAttack / gStatStageRatios[gBattleMons[battler].statStages[STAT_SPATK]][1];

    if (spAttack >= attack)
        return DAMAGE_CATEGORY_SPECIAL;
    else
        return DAMAGE_CATEGORY_PHYSICAL;
}

static u32 GetFlingPowerFromItemId(enum Item itemId)
{
    if (gItemsInfo[itemId].pocket == POCKET_TM_HM)
    {
        u32 power = GetMovePower(ItemIdToBattleMoveId(itemId));
        if (power > 1)
            return power;
        return 10; // Status moves and moves with variable power always return 10 power.
    }
    else
        return GetItemFlingPower(itemId);
}

bool32 CanFling(enum BattlerId battlerAtk, enum Ability abilityAtk)
{
    enum Item item = gBattleMons[battlerAtk].item;

    if (item == ITEM_NONE
      || (GetConfig(B_KLUTZ_FLING_INTERACTION) >= GEN_5 && abilityAtk == ABILITY_KLUTZ)
      || gFieldStatuses & STATUS_FIELD_MAGIC_ROOM
      || gBattleMons[battlerAtk].volatiles.embargoTimer
      || (GetItemTMHMIndex(item) != 0 && GetItemImportance(item) == 1) // don't fling reusable TMs
      || GetFlingPowerFromItemId(item) == 0
      || !CanBattlerGetOrLoseItem(battlerAtk, battlerAtk, item)) // defender being a paradox mon doesn't matter
        return FALSE;

    return TRUE;
}

void SortBattlersByRawSpeed(u8 battlers[])
{
    for (u32 i = 0; i < gBattlersCount; i++)
        battlers[i] = i;

    for (u32 i = 0; i < gBattlersCount; i++)
    {
        for (u32 j = 0; j < gBattlersCount; j++)
        {
            if (gBattleMons[battlers[i]].speed >= gBattleMons[battlers[j]].speed)
            {
                u32 temp = battlers[i];
                battlers[i] = battlers[j];
                battlers[j] = temp;
            }
        }
    }
}

// Sort an array of battlers by speed
// Useful for effects like pickpocket, eject button, red card, dancer
void SortBattlersBySpeed(enum BattlerId *battlers, bool32 slowToFast)
{
    int i, j, currSpeed;
    enum BattlerId currBattler;
    u16 speeds[MAX_BATTLERS_COUNT] = {0};

    for (i = 0; i < gBattlersCount; i++)
    {
        enum BattlerId battler = battlers[i];
        speeds[i] = GetBattlerTotalSpeedStat(battler, GetBattlerAbility(battler), GetBattlerHoldEffect(battler));
    }

    for (i = 1; i < gBattlersCount; i++)
    {
        currBattler = battlers[i];
        currSpeed = speeds[i];
        j = i - 1;

        if (slowToFast)
        {
            while (j >= 0 && speeds[j] > currSpeed)
            {
                battlers[j + 1] = battlers[j];
                speeds[j + 1] = speeds[j];
                j = j - 1;
            }
        }
        else
        {
            while (j >= 0 && speeds[j] < currSpeed)
            {
                battlers[j + 1] = battlers[j];
                speeds[j + 1] = speeds[j];
                j = j - 1;
            }
        }

        battlers[j + 1] = currBattler;
        speeds[j + 1] = currSpeed;
    }
}

void TryRestoreHeldItems(void)
{
    if (!B_TRAINERS_KNOCK_OFF_ITEMS && B_RESTORE_HELD_BATTLE_ITEMS < GEN_9)
        return;

    bool32 returnNPCItems = B_RETURN_STOLEN_NPC_ITEMS >= GEN_5 && gBattleTypeFlags & BATTLE_TYPE_TRAINER;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (gBattleStruct->itemLost[B_TRAINER_PLAYER][i].stolen || returnNPCItems)
        {
            enum Item lostItem = gBattleStruct->itemLost[B_TRAINER_PLAYER][i].originalItem;

            if (GetItemPocket(lostItem) == POCKET_BERRIES && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM) != lostItem)
                lostItem = ITEM_NONE;

            if ((lostItem != ITEM_NONE || returnNPCItems) && GetItemPocket(lostItem) != POCKET_BERRIES)
                SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM, &lostItem);
        }
    }
}

bool32 CanStealItem(enum BattlerId battlerStealing, enum BattlerId battlerItem, enum Item item)
{
    enum BattleSide stealerSide = GetBattlerSide(battlerStealing);

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        return FALSE;

    // Check if the battler trying to steal should be able to
    if (stealerSide == B_SIDE_OPPONENT
        && !(gBattleTypeFlags &
             (BATTLE_TYPE_EREADER_TRAINER
              | BATTLE_TYPE_FRONTIER
              | BATTLE_TYPE_LINK
              | BATTLE_TYPE_RECORDED_LINK
              | BATTLE_TYPE_SECRET_BASE
              | (B_TRAINERS_KNOCK_OFF_ITEMS == TRUE ? BATTLE_TYPE_TRAINER : 0)
              )))
    {
        return FALSE;
    }
    else if (!(gBattleTypeFlags &
          (BATTLE_TYPE_EREADER_TRAINER
           | BATTLE_TYPE_FRONTIER
           | BATTLE_TYPE_LINK
           | BATTLE_TYPE_RECORDED_LINK
           | BATTLE_TYPE_SECRET_BASE))
        && GetBattlerPartyState(battlerStealing)->isKnockedOff)
    {
        return FALSE;
    }

    if (!CanBattlerGetOrLoseItem(battlerItem, battlerStealing, item)  // Battler with item cannot have it stolen
     || !CanBattlerGetOrLoseItem(battlerStealing, battlerItem, item)) // Stealer cannot take the item
        return FALSE;

    return TRUE;
}

void TrySaveExchangedItem(enum BattlerId battler, enum Item stolenItem)
{
    // Because BtlController_EmitSetMonData does SetMonData, we need to save the stolen item only if it matches the battler's original
    // So, if the player steals an item during battle and has it stolen from it, it will not end the battle with it (naturally)
    if (B_TRAINERS_KNOCK_OFF_ITEMS == FALSE)
        return;

    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER) || gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        return;

    if (GetBattlerTrainer(battler) != B_TRAINER_PLAYER)
        return;

    // If regular trainer battle and mon's original item matches what is being stolen, save it to be restored at end of battle
    if (stolenItem == gBattleStruct->itemLost[B_TRAINER_PLAYER][gBattlerPartyIndexes[battler]].originalItem)
        gBattleStruct->itemLost[B_TRAINER_PLAYER][gBattlerPartyIndexes[battler]].stolen = TRUE;
}

bool32 IsBattlerAffectedByHazards(enum BattlerId battler, enum HoldEffect holdEffect, bool32 toxicSpikes)
{
    bool32 ret = TRUE;
    if (!IsBattlerAlive(battler))
    {
        ret = FALSE;
    }
    else if (toxicSpikes && holdEffect == HOLD_EFFECT_HEAVY_DUTY_BOOTS && !IS_BATTLER_OF_TYPE(battler, TYPE_POISON))
    {
        ret = FALSE;
        RecordItemEffectBattle(battler, holdEffect);
    }
    else if (holdEffect == HOLD_EFFECT_HEAVY_DUTY_BOOTS)
    {
        ret = FALSE;
        RecordItemEffectBattle(battler, holdEffect);
    }
    return ret;
}

bool32 IsSheerForceAffected(enum Move move, enum Ability ability)
{
    return ability == ABILITY_SHEER_FORCE && MoveIsAffectedBySheerForce(move);
}

bool32 BlocksPrankster(enum Move move, enum BattlerId battlerPrankster, enum BattlerId battlerDef, bool32 checkTarget)
{
    if (GetConfig(B_PRANKSTER_DARK_TYPES) < GEN_7)
        return FALSE;
    if (!gProtectStructs[battlerPrankster].pranksterElevated)
        return FALSE;
    if (IsBattlerAlly(battlerPrankster, battlerDef))
        return FALSE;
    if (checkTarget && GetBattlerMoveTargetType(battlerPrankster, move) == TARGET_DEPENDS)
        return FALSE;
    if (checkTarget && GetBattlerMoveTargetType(battlerPrankster, move) == TARGET_OPPONENTS_FIELD)
        return FALSE;
    if (!IS_BATTLER_OF_TYPE(battlerDef, TYPE_DARK))
        return FALSE;
    if (IsSemiInvulnerable(battlerDef, CHECK_ALL))
        return FALSE;

    return TRUE;
}

// Not enum BattlerId to allow using it with RandomUniformExcept
bool32 CantPickupItem(u32 _battler)
{
    enum BattlerId battler = _battler;
    // Used by RandomUniformExcept() for RNG_PICKUP
    if (battler == gBattlerAttacker && (GetConfig(B_PICKUP_WILD) < GEN_9 || gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK)))
        return TRUE;
    return !(IsBattlerAlive(battler) && GetBattlerPartyState(battler)->usedHeldItem && gBattleStruct->battlerState[battler].canPickupItem);
}

bool32 PickupHasValidTarget(enum BattlerId battler)
{
    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        if (!CantPickupItem(i))
            return TRUE;
    }
    return FALSE;
}

u32 GetWeather(void)
{
    if (gBattleWeather == B_WEATHER_NONE || !HasWeatherEffect())
        return B_WEATHER_NONE;
    return gBattleWeather;
}

u32 GetAttackerWeather(enum HoldEffect holdEffect, enum Ability ability, u32 weather)
{
    if (ability == ABILITY_MEGA_SOL)
        return B_WEATHER_SUN;

    if (holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA)
        return weather & ~(B_WEATHER_SUN | B_WEATHER_RAIN); // This was assumed not to block mega sol, like cloud nine doesn't.

    return weather;
}


bool32 IsBattlerWeatherAffected(enum HoldEffect holdEffect, u32 weather, u32 weatherFlags)
{
    if (weather & (B_WEATHER_SUN | B_WEATHER_RAIN) && holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA)
        return FALSE;

    if (weather == B_WEATHER_NONE || !(gBattleWeather & weatherFlags))
        return FALSE;

    return TRUE;
}

static u32 CanBattlerHitBothFoesInTerrain(enum BattlerId battler, enum Move move, enum BattleMoveEffects effect)
{
    return effect == EFFECT_TERRAIN_BOOST
        && GetMoveTerrainBoost_HitsBothFoes(move)
        && IsBattlerTerrainAffected(battler, GetBattlerAbility(battler), GetBattlerHoldEffect(battler), GetMoveTerrainBoost_Terrain(move), gFieldTimers.terrain);
}

enum MoveTarget GetBattlerMoveSelectionTargetType(enum BattlerId battler, enum Move move)
{
    enum BattleMoveEffects effect = GetMoveEffect(move);
    if (effect == EFFECT_CURSE && !IS_BATTLER_OF_TYPE(battler, TYPE_GHOST))
        return TARGET_USER;
    if (effect == EFFECT_TERA_STARSTORM && gBattleMons[battler].species == SPECIES_TERAPAGOS_STELLAR)
        return TARGET_BOTH;

    return GetMoveTarget(move);
}

enum MoveTarget GetBattlerMoveTargetType(enum BattlerId battler, enum Move move)
{
    if (CanBattlerHitBothFoesInTerrain(battler, move, GetMoveEffect(move)))
        return TARGET_BOTH;

    return GetBattlerMoveSelectionTargetType(battler, move);
}

bool32 CanTargetBattler(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Move move)
{
    if (GetMoveEffect(move) == EFFECT_HIT_ENEMY_HEAL_ALLY
    &&  IsBattlerAlly(battlerAtk, battlerDef)
    &&  gBattleMons[battlerAtk].volatiles.healBlockTimer)
        return FALSE;   // Pokémon affected by Heal Block cannot target allies with Pollen Puff
    if (!IsBattlerAlive(battlerDef))
        return FALSE;
    if (IsBattlerAlly(battlerAtk, battlerDef) && (GetActiveGimmick(battlerAtk) == GIMMICK_DYNAMAX
                                               || IsGimmickSelected(battlerAtk, GIMMICK_DYNAMAX)))
        return FALSE;

    return TRUE;
}

u32 GetNextTarget(u32 moveTarget, bool32 excludeCurrent)
{
    enum BattlerId battler;
    for (battler = B_BATTLER_0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        if (excludeCurrent && battler == gBattlerTarget)
            continue;
        if (gBattleStruct->battlerState[gBattlerAttacker].targetsDone[battler])
            continue;
        if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_NO_EFFECT)
            continue;
        break;
    }
    return battler;
}

void CopyMonLevelAndBaseStatsToBattleMon(enum BattlerId battler, struct Pokemon *mon, bool32 updateSpeedStat)
{
    gBattleMons[battler].level = GetMonData(mon, MON_DATA_LEVEL);
    gBattleMons[battler].hp = GetMonData(mon, MON_DATA_HP);
    gBattleMons[battler].maxHP = GetMonData(mon, MON_DATA_MAX_HP);
    gBattleMons[battler].attack = GetMonData(mon, MON_DATA_ATK);
    gBattleMons[battler].defense = GetMonData(mon, MON_DATA_DEF);
    if (updateSpeedStat)
        gBattleMons[battler].speed = GetMonData(mon, MON_DATA_SPEED);
    gBattleMons[battler].spAttack = GetMonData(mon, MON_DATA_SPATK);
    gBattleMons[battler].spDefense = GetMonData(mon, MON_DATA_SPDEF);
}

void CopyMonAbilityAndTypesToBattleMon(enum BattlerId battler, struct Pokemon *mon)
{
    gBattleMons[battler].ability = GetMonAbility(mon);
    #if TESTING
    if (gTestRunnerEnabled)
    {
        u32 array = (!IsPartnerMonFromSameTrainer(battler)) ? battler : GetBattlerSide(battler);
        u32 partyIndex = gBattlerPartyIndexes[battler];
        if (TestRunner_Battle_GetForcedAbility(array, partyIndex))
            gBattleMons[battler].ability = TestRunner_Battle_GetForcedAbility(array, partyIndex);
    }
    #endif
    gBattleMons[battler].types[0] = GetSpeciesType(gBattleMons[battler].species, 0);
    gBattleMons[battler].types[1] = GetSpeciesType(gBattleMons[battler].species, 1);
    gBattleMons[battler].types[2] = TYPE_MYSTERY;
}

void RecalcBattlerStats(enum BattlerId battler, struct Pokemon *mon, bool32 isDynamaxing)
{
    u32 hp = GetMonData(mon, MON_DATA_HP);
    u32 oldMaxHp = GetMonData(mon, MON_DATA_MAX_HP);
    if (gBattleMons[battler].volatiles.speedSwapped && GetConfig(B_MEGA_EVO_SPEED_SWAP) >= GEN_CHAMPIONS)
        CalculateMonStatsCont(mon, FALSE);
    else
        CalculateMonStats(mon);

    if (GetActiveGimmick(battler) == GIMMICK_DYNAMAX && gChosenActionByBattler[battler] != B_ACTION_SWITCH)
    {
        ApplyDynamaxHPMultiplier(mon);
        u32 newMaxHp = GetMonData(mon, MON_DATA_MAX_HP);
        if (!isDynamaxing)
        {
            if (newMaxHp > oldMaxHp) // restore hp gained from changing form, without this, dynamaxed form changes are calculated incorrectly
            {
                hp += (newMaxHp - oldMaxHp);
                SetMonData(mon, MON_DATA_HP, &hp);
            }
            else
            {
                SetMonData(mon, MON_DATA_HP, &hp);
            }
        }
    }
    if (gBattleMons[battler].volatiles.speedSwapped && GetConfig(B_MEGA_EVO_SPEED_SWAP) >= GEN_CHAMPIONS)
        CopyMonLevelAndBaseStatsToBattleMon(battler, mon, FALSE);
    else
        CopyMonLevelAndBaseStatsToBattleMon(battler, mon, TRUE);
    CopyMonAbilityAndTypesToBattleMon(battler, mon);
}

u32 GetBattlerGender(enum BattlerId battler)
{
    return GetGenderFromSpeciesAndPersonality(gBattleMons[battler].species,
                                              gBattleMons[battler].personality);
}

bool32 AreBattlersOfOppositeGender(enum BattlerId battler1, enum BattlerId battler2)
{
    u32 gender1 = GetBattlerGender(battler1);
    u32 gender2 = GetBattlerGender(battler2);

    return (gender1 != MON_GENDERLESS && gender2 != MON_GENDERLESS && gender1 != gender2);
}

bool32 AreBattlersOfSameGender(enum BattlerId battler1, enum BattlerId battler2)
{
    u32 gender1 = GetBattlerGender(battler1);
    u32 gender2 = GetBattlerGender(battler2);

    return (gender1 != MON_GENDERLESS && gender2 != MON_GENDERLESS && gender1 == gender2);
}

u32 CalcSecondaryEffectChance(enum BattlerId battler, enum Ability battlerAbility, const struct AdditionalEffect *additionalEffect)
{
    bool8 hasSereneGrace = (battlerAbility == ABILITY_SERENE_GRACE);
    bool8 hasRainbow = (gSideStatuses[GetBattlerSide(battler)] & SIDE_STATUS_RAINBOW) != 0;
    u16 secondaryEffectChance = additionalEffect->chance;

    if (hasRainbow && hasSereneGrace && additionalEffect->moveEffect == MOVE_EFFECT_FLINCH)
        return secondaryEffectChance * 2;

    if (hasSereneGrace)
        secondaryEffectChance *= 2;
    if (hasRainbow)
        secondaryEffectChance *= 2;

    return secondaryEffectChance;
}

bool32 MoveEffectIsGuaranteed(enum BattlerId battler, enum Ability battlerAbility, const struct AdditionalEffect *additionalEffect)
{
    return additionalEffect->chance == 0 || CalcSecondaryEffectChance(battler, battlerAbility, additionalEffect) >= 100;
}

bool32 IsGen6ExpShareEnabled(void)
{
    if (I_EXP_SHARE_FLAG <= TEMP_FLAGS_END)
        return FALSE;

    return FlagGet(I_EXP_SHARE_FLAG);
}


bool32 MoveHasAdditionalEffect(enum Move move, enum MoveEffect moveEffect)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        if (additionalEffect->moveEffect == moveEffect && additionalEffect->self == FALSE)
            return TRUE;
    }
    return FALSE;
}

bool32 MoveHasAdditionalOnSideEffect(enum Move move)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        if (additionalEffect->onSide)
            return TRUE;
    }
    return FALSE;
}

bool32 MoveHasAdditionalEffectWithChance(enum Move move, enum MoveEffect moveEffect, u32 chance)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        if (additionalEffect->moveEffect == moveEffect && additionalEffect->chance == chance)
            return TRUE;
    }
    return FALSE;
}

bool32 MoveHasAdditionalEffectSelf(enum Move move, enum MoveEffect moveEffect)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        if (additionalEffect->moveEffect == moveEffect && additionalEffect->self == TRUE)
            return TRUE;
    }
    return FALSE;
}

bool32 IsMoveEffectRemoveSpeciesType(enum Move move, enum MoveEffect moveEffect, enum Type type)
{
    return (MoveHasAdditionalEffectSelf(move, moveEffect) && GetMoveArgType(move) == type);
}

bool32 MoveHasChargeTurnAdditionalEffect(enum Move move)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        if (GetMoveAdditionalEffectById(move, i)->onChargeTurnOnly)
            return TRUE;
    }
    return FALSE;
}

bool32 MoveIsAffectedBySheerForce(enum Move move)
{
    u32 i;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(move);
    for (i = 0; i < numAdditionalEffects; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        if ((additionalEffect->chance > 0) != additionalEffect->sheerForceOverride)
            return TRUE;
    }
    return FALSE;
}

bool32 CanMonParticipateInSkyBattle(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 monAbilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    enum Ability ability = GetSpeciesAbility(species, monAbilityNum);

    bool32 hasLevitateAbility = (ability == ABILITY_LEVITATE || ability == ABILITY_EELEVATE);
    bool32 isFlyingType = GetSpeciesType(species, 0) == TYPE_FLYING || GetSpeciesType(species, 1) == TYPE_FLYING;
    bool32 monIsValidAndNotEgg = GetMonData(mon, MON_DATA_SANITY_HAS_SPECIES) && !GetMonData(mon, MON_DATA_IS_EGG);

    if (monIsValidAndNotEgg)
    {
        if ((hasLevitateAbility || isFlyingType) && !gSpeciesInfo[species].isSkyBattleBanned)
            return TRUE;
    }
    return FALSE;
}

void GetBattlerTypes(enum BattlerId battler, bool32 ignoreTera, enum Type types[static 3])
{
    // Terastallization.
    bool32 isTera = GetActiveGimmick(battler) == GIMMICK_TERA;
    if (!ignoreTera && isTera)
    {
        enum Type teraType = GetBattlerTeraType(battler);
        if (teraType != TYPE_STELLAR)
        {
            types[0] = types[1] = types[2] = teraType;
            return;
        }
    }

    types[0] = gBattleMons[battler].types[0];
    types[1] = gBattleMons[battler].types[1];
    types[2] = gBattleMons[battler].types[2];

    // Roost.
    if (!isTera && gBattleMons[battler].volatiles.roostActive)
    {
        if (types[0] == TYPE_FLYING && types[1] == TYPE_FLYING)
            types[0] = types[1] = B_ROOST_PURE_FLYING >= GEN_5 ? TYPE_NORMAL : TYPE_MYSTERY;
        else if (types[0] == TYPE_FLYING)
            types[0] = TYPE_MYSTERY;
        else if (types[1] == TYPE_FLYING)
            types[1] = TYPE_MYSTERY;
    }
}

void RemoveBattlerType(enum BattlerId battler, enum Type type)
{
    u32 i;
    if (GetActiveGimmick(battler) == GIMMICK_TERA) // don't remove type if Terastallized
        return;
    for (i = 0; i < 3; i++)
    {
        if (*(u8 *)(&gBattleMons[battler].types[0] + i) == type)
            *(u8 *)(&gBattleMons[battler].types[0] + i) = TYPE_MYSTERY;
    }
}

void SetShellSideArmCategory(void)
{
    enum BattlerId battlerAtk, battlerDef;
    u32 attackerAtkStat;
    u32 targetDefStat;
    u32 attackerSpAtkStat;
    u32 targetSpDefStat;
    u8 statStage;
    u32 physical;
    u32 special;
    u32 power = GetMovePower(MOVE_SHELL_SIDE_ARM);

    // Don't run this check for Safari Battles. Because player's stats are zeroed out, this performs division by zero which previously would crash on certain emulators in Safari Zone.
    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
        return;

    for (battlerAtk = 0; battlerAtk < gBattlersCount; battlerAtk++)
    {
        attackerAtkStat = gBattleMons[battlerAtk].attack;
        statStage = gBattleMons[battlerAtk].statStages[STAT_ATK];
        attackerAtkStat *= gStatStageRatios[statStage][0];
        attackerAtkStat /= gStatStageRatios[statStage][1];

        attackerSpAtkStat = gBattleMons[battlerAtk].spAttack;
        statStage = gBattleMons[battlerAtk].statStages[STAT_SPATK];
        attackerSpAtkStat *= gStatStageRatios[statStage][0];
        attackerSpAtkStat /= gStatStageRatios[statStage][1];

        for (battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
        {
            if (battlerAtk == battlerDef)
                continue;

            targetDefStat = gBattleMons[battlerDef].defense;
            statStage = gBattleMons[battlerDef].statStages[STAT_DEF];
            targetDefStat *= gStatStageRatios[statStage][0];
            targetDefStat /= gStatStageRatios[statStage][1];
            if (targetDefStat == 0)
                targetDefStat = 1;

            physical = ((((2 * gBattleMons[battlerAtk].level / 5 + 2) * power * attackerAtkStat) / targetDefStat) / 50);

            targetSpDefStat = gBattleMons[battlerDef].spDefense;
            statStage = gBattleMons[battlerDef].statStages[STAT_SPDEF];
            targetSpDefStat *= gStatStageRatios[statStage][0];
            targetSpDefStat /= gStatStageRatios[statStage][1];
            if (targetSpDefStat == 0)
                targetSpDefStat = 1;

            special = ((((2 * gBattleMons[battlerAtk].level / 5 + 2) * power * attackerSpAtkStat) / targetSpDefStat) / 50);

            if ((physical > special) || (physical == special && RandomPercentage(RNG_SHELL_SIDE_ARM, 50)))
                gBattleStruct->shellSideArmCategory[battlerAtk][battlerDef] = DAMAGE_CATEGORY_PHYSICAL;
            else
                gBattleStruct->shellSideArmCategory[battlerAtk][battlerDef] = DAMAGE_CATEGORY_SPECIAL;
        }
    }
}

bool32 CanTargetPartner(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    return (IsDoubleBattle()
         && IsBattlerAlive(GetPartnerBattler(battlerDef))
         && battlerDef != GetPartnerBattler(battlerAtk));
}

bool32 IsBattlerUnaffectedByMove(enum BattlerId battler)
{
    return gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_NO_EFFECT;
}

enum Type GetBattleMoveType(enum Move move)
{
    if (gMain.inBattle)
    {
        if (gBattleStruct->dynamicMoveType != TYPE_NONE)
            return gBattleStruct->dynamicMoveType;

        enum BattleMoveEffects effect = GetMoveEffect(move);
        if ((effect == EFFECT_BEAT_UP || effect == EFFECT_FUTURE_SIGHT)
         && GetConfig(B_UPDATED_MOVE_TYPES) < GEN_5)
            return TYPE_MYSTERY;
    }
    return GetMoveType(move);
}

void TryActivateSleepClause(enum BattlerId battler, u32 indexInParty)
{
    if (gBattleStruct->battlerState[battler].sleepClauseEffectExempt)
    {
        gBattleStruct->battlerState[battler].sleepClauseEffectExempt = FALSE;
        return;
    }

    if (IsSleepClauseEnabled())
    {
        enum BattleSide side = GetBattlerSide(battler);
        struct SleepClause *monCausingSleepClause = &gBattleStruct->monCausingSleepClause[side];
        monCausingSleepClause->partyIndex = indexInParty;
        monCausingSleepClause->trainer = GetBattlerTrainer(battler);
    }
}

void TryDeactivateSleepClause(enum BattlerId battler, u32 indexInParty)
{
    enum BattleSide side = GetBattlerSide(battler);
    struct SleepClause *monCausingSleepClause = &gBattleStruct->monCausingSleepClause[side];
    // If the Pokémon on the given side and trainer party at the given index in the party is the one causing Sleep Clause to be
    // active, set monCausingSleepClause->partyIndex = PARTY_SIZE, which means Sleep Clause is not active for the given side
    if (IsSleepClauseEnabled()
     && monCausingSleepClause->partyIndex == indexInParty
     && monCausingSleepClause->trainer == GetBattlerTrainer(battler))
    {
        monCausingSleepClause->partyIndex = PARTY_SIZE;
        monCausingSleepClause->trainer = MAX_BATTLE_TRAINERS;
    }
}

bool32 IsSleepClauseActiveForSide(enum BattleSide battlerSide)
{
    // If monCausingSleepClause[battlerSide].partyIndex == PARTY_SIZE, Sleep Clause is not active for the given side.
    // If monCausingSleepClause[battlerSide].partyIndex < PARTY_SIZE, it means it is storing the index of the mon that is causing Sleep Clause to be active,
    // from which it follows that Sleep Clause is active.
    return (IsSleepClauseEnabled() && (gBattleStruct->monCausingSleepClause[battlerSide].partyIndex < PARTY_SIZE));
}

bool32 IsSleepClauseEnabled(void)
{
    if (B_SLEEP_CLAUSE)
        return TRUE;
    if (FlagGet(B_FLAG_SLEEP_CLAUSE))
        return TRUE;
    return FALSE;
}

bool32 AreMultiPartiesFullTeams(void)
{
#if TESTING
    u8 *partySizes = gBattleTestRunnerState->data.partySizes;
    bool32 fullTeam = FALSE;

    if (partySizes[B_TRAINER_PLAYER] && partySizes[B_TRAINER_PARTNER]
        && (partySizes[B_TRAINER_PLAYER] > MULTI_PARTY_SIZE || partySizes[B_TRAINER_PARTNER] > MULTI_PARTY_SIZE))
    {
        fullTeam = TRUE;
    }
    if (partySizes[B_TRAINER_OPPONENT_A] && partySizes[B_TRAINER_OPPONENT_B]
        && (partySizes[B_TRAINER_OPPONENT_A] > MULTI_PARTY_SIZE || partySizes[B_TRAINER_OPPONENT_B] > MULTI_PARTY_SIZE))
    {
        fullTeam = TRUE;
    }

    if (!fullTeam)
    {
        gSpecialVar_Result = FALSE;
        return FALSE;
    }
#else
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();

    if (B_MULTI_HALF_TEAMS)
    {
        gSpecialVar_Result = FALSE;
        return FALSE;
    }

    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
    {
        gSpecialVar_Result = TRUE;
        return TRUE;
    }

    if (TRAINER_BATTLE_PARAM.opponentA == TRAINER_LINK_OPPONENT
     || gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI
     || (gTrainers[difficulty][TRAINER_BATTLE_PARAM.opponentA].multiTeamSize == MULTI_TEAM_SIZE_HALF)
     || (gTrainers[difficulty][TRAINER_BATTLE_PARAM.opponentB].multiTeamSize == MULTI_TEAM_SIZE_HALF))
    {
        gSpecialVar_Result = FALSE;
        return FALSE;
    }
#endif

    gSpecialVar_Result = TRUE;
    return TRUE;
}

void ClearDamageCalcResults(void)
{
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        gBattleStruct->moveDamage[battler] = 0;
        gBattleStruct->innardsOutHpLost[battler] = 0;
        gBattleStruct->moveResultFlags[battler] = 0;
        gBattleStruct->passiveHpUpdate[battler] = 0;
        gSpecialStatuses[battler].criticalHit = FALSE;
        gSpecialStatuses[battler].damagedByAttack = FALSE;
    }

    gBattleStruct->numSpreadTargets = 0;
    gBattleStruct->unableToUseMove = FALSE;
    gBattleStruct->attackAnimPlayed = FALSE;
    gBattleScripting.savedDmg = 0;
    if (gCurrentMove != MOVE_NONE)
        gBattleStruct->moldBreakerActive = IsMoldBreakerTypeAbility(gBattlerAttacker, GetBattlerAbility(gBattlerAttacker), gCurrentMove) || MoveIgnoresTargetAbility(gCurrentMove);
    else
        gBattleStruct->moldBreakerActive = FALSE;
}

bool32 DoesDestinyBondFail(enum BattlerId battler)
{
    return GetConfig(B_DESTINY_BOND_FAIL) >= GEN_7 && gBattleMons[battler].volatiles.destinyBond;
}

// This check has always to be the last in a condtion statement because of the recording of AI data.
bool32 IsMoveEffectBlockedByTarget(enum Ability ability)
{
    if (ability == ABILITY_SHIELD_DUST)
    {
        RecordAbilityBattle(gBattlerTarget, ability);
        return TRUE;
    }
    else if (GetBattlerHoldEffect(gBattlerTarget) == HOLD_EFFECT_COVERT_CLOAK)
    {
        RecordItemEffectBattle(gBattlerTarget, HOLD_EFFECT_COVERT_CLOAK);
        return TRUE;
    }

    return FALSE;
}

bool32 SetTargetToNextPursuiter(enum BattlerId battlerDef)
{
    u32 i;
    for (i = gCurrentTurnActionNumber + 1; i < gBattlersCount; i++)
    {
        enum BattlerId battler = gBattlerByTurnOrder[i];
        if (gChosenActionByBattler[battler] == B_ACTION_USE_MOVE
        && GetMoveEffect(gChosenMoveByBattler[battler]) == EFFECT_PURSUIT
        && IsBattlerAlive(battlerDef)
        && IsBattlerAlive(battler)
        && !IsBattlerAlly(battler, battlerDef)
        && (B_PURSUIT_TARGET >= GEN_4 || gBattleStruct->moveTarget[battler] == battlerDef)
        && !IsGimmickSelected(battler, GIMMICK_Z_MOVE)
        && !IsGimmickSelected(battler, GIMMICK_DYNAMAX)
        && GetActiveGimmick(battler) != GIMMICK_DYNAMAX)
        {
            gBattlerTarget = battler;
            return TRUE;
        }
    }
    return FALSE;
}

bool32 IsPursuitTargetSet(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gBattleStruct->battlerState[battler].pursuitTarget)
            return TRUE;
    }
    return FALSE;
}

void ClearPursuitValues(void)
{
    for (enum BattlerId i = 0; i < gBattlersCount; i++)
        gBattleStruct->battlerState[i].pursuitTarget = FALSE;
    gBattleStruct->pursuitStoredSwitch = PARTY_SIZE;
}

void ClearPursuitValuesIfSet(enum BattlerId battler)
{
    if (gBattleStruct->battlerState[battler].pursuitTarget)
        ClearPursuitValues();
}

bool32 HasWeatherEffect(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        enum Ability ability = GetBattlerAbility(battler);
        switch (ability)
        {
        case ABILITY_CLOUD_NINE:
        case ABILITY_AIR_LOCK:
            return FALSE;
        default:
            break;
        }
    }

    return TRUE;
}

bool32 TrySwitchInEjectPack(enum EjectPackTiming timing)
{
    // Because sorting the battlers by speed takes lots of cycles, it's better to just check if any of the battlers has the Eject items.
    u32 ejectPackBattlers = 0;
    u32 numEjectPackBattlers = 0;

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].volatiles.tryEjectPack
         && IsBattlerAlive(i)
         && !IsBattlerInvolvedInSkyDrop(i)
         && GetBattlerHoldEffect(i) == HOLD_EFFECT_EJECT_PACK
         && gBattleMons[i].volatiles.semiInvulnerable != STATE_COMMANDER
         && gBattleStruct->battlerState[i].commanderSpecies == SPECIES_NONE
         && CanBattlerSwitch(i))
        {
            ejectPackBattlers |= 1u << i;
            numEjectPackBattlers++;
        }
    }

    if (numEjectPackBattlers == 0)
        return FALSE;

    enum BattlerId battlers[MAX_BATTLERS_COUNT] = {0, 1, 2, 3};
    if (numEjectPackBattlers > 1)
        SortBattlersBySpeed(battlers, FALSE);

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
        gBattleMons[i].volatiles.tryEjectPack = FALSE;

    for (u32 i = 0; i < gBattlersCount; i++)
    {
        enum BattlerId battler = battlers[i];

        if (!(ejectPackBattlers & 1u << battler))
            continue;

        gBattleScripting.battler = battler;
        gLastUsedItem = gBattleMons[battler].item;
        if (timing != END_TURN)
        {
            BattleScriptCall(BattleScript_EjectPackActivates_SendReplacement);
        }
        else
        {
            gSpecialStatuses[battler].queuedSwitch = QUEUED_SWITCH_OPEN_PARTY_SCREEN;
            BattleScriptCall(BattleScript_EjectItemActivates);
        }
        gAiLogicData->ejectPackSwitch = TRUE;
        return TRUE;
    }

    return FALSE;
}

bool32 EmergencyExitCanBeTriggered(enum BattlerId battler, enum Ability ability)
{
    if (ability != ABILITY_EMERGENCY_EXIT && ability != ABILITY_WIMP_OUT)
        return FALSE;

    if (IsBattlerAlive(battler)
     && !IsPursuitTargetSet()
     && gBattleStruct->battlerState[battler].commanderSpecies == SPECIES_NONE
     && (HadMoreThanHalfHpNowDoesnt(battler) || gSpecialStatuses[battler].shellBellEmergencyExit)
     && (CanBattlerSwitch(battler) || !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
     && !(gBattleTypeFlags & BATTLE_TYPE_ARENA)
     && gBattleMons[battler].volatiles.semiInvulnerable != STATE_SKY_DROP_TARGET)
        return TRUE;

    return FALSE;
}

bool32 TryTriggerSymbiosis(enum BattlerId battler, u32 ally)
{
    return GetBattlerAbility(ally) == ABILITY_SYMBIOSIS
        && gBattleMons[battler].item == ITEM_NONE
        && gBattleMons[ally].item != ITEM_NONE
        && CanBattlerGetOrLoseItem(battler, ally, gBattleMons[ally].item)
        && CanBattlerGetOrLoseItem(ally, battler, gBattleMons[ally].item)
        && IsBattlerAlive(battler)
        && IsBattlerAlive(ally);
}

// itemId represents the item that was removed, not the item being given.
bool32 TrySymbiosis(enum BattlerId battler, enum Item itemId, const u8 *nextInstr)
{
    if (GetItemHoldEffect(itemId) != HOLD_EFFECT_EJECT_BUTTON
     && GetItemHoldEffect(itemId) != HOLD_EFFECT_EJECT_PACK
     && (GetConfig(B_SYMBIOSIS_GEMS) < GEN_7 || !(gSpecialStatuses[battler].gemBoost))
     && !gSpecialStatuses[battler].berryReduced //Fling and damage-reducing berries are handled separately.
     && TryTriggerSymbiosis(battler, GetPartnerBattler(battler)))
    {
        BestowItem(GetPartnerBattler(battler), battler);
        gLastUsedAbility = gBattleMons[GetPartnerBattler(battler)].ability;
        gEffectBattler = battler;
        gBattleScripting.battler = gBattlerAbility = GetPartnerBattler(battler);
        if (nextInstr == NULL)
            BattleScriptPushCursor();
        else
            BattleScriptPush(nextInstr);
        gBattlescriptCurrInstr = BattleScript_SymbiosisActivates;
        return TRUE;
    }
    return FALSE;
}

// Used by Bestow and Symbiosis to take an item from one battler and give to another.
void BestowItem(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    gLastUsedItem = gBattleMons[battlerAtk].item;

    gBattleMons[battlerAtk].item = ITEM_NONE;
    BtlController_EmitSetMonData(battlerAtk, B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[battlerAtk].item), &gBattleMons[battlerAtk].item);
    MarkBattlerForControllerExec(battlerAtk);
    CheckSetUnburden(battlerAtk);

    gBattleMons[battlerDef].item = gLastUsedItem;
    BtlController_EmitSetMonData(battlerDef, B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[battlerDef].item), &gBattleMons[battlerDef].item);
    MarkBattlerForControllerExec(battlerDef);
    gBattleMons[battlerDef].volatiles.unburdenActive = FALSE;
}

#define UNPACK_VOLATILE_GETTERS(_enum, _fieldName, _typeMaxValue, ...) case _enum: return gBattleMons[battler].volatiles._fieldName;

// Gets the value of a volatile status flag for a certain battler
// Primarily used for the debug menu and scripts. Outside of it explicit references are preferred
// Uses Arm because there is a compiler bug when it tries to compile in thumb
ARM_FUNC u32 GetBattlerVolatile(enum BattlerId battler, enum Volatile _volatile)
{
    switch (_volatile)
    {
    VOLATILE_DEFINITIONS(UNPACK_VOLATILE_GETTERS)
    /* Expands to:
    case VOLATILE_CONFUSION:
        return gBattleMons[battler].volatiles.confusionTimer;
    */
    default: // Invalid volatile status
        return 0;
    }
}

#define UNPACK_VOLATILE_SETTERS(_enum, _fieldName, _typeMaxValue, ...) case _enum: gBattleMons[battler].volatiles._fieldName = min(GET_VOLATILE_MAXIMUM(_typeMaxValue), newValue); break;

// Sets the value of a volatile status flag for a certain battler
// Primarily used for the debug menu and scripts. Outside of it explicit references are preferred
void SetMonVolatile(enum BattlerId battler, enum Volatile _volatile, u32 newValue)
{
    switch (_volatile)
    {
        VOLATILE_DEFINITIONS(UNPACK_VOLATILE_SETTERS)
        /* Expands to:
    case VOLATILE_CONFUSION:
            gBattleMons[battler].volatiles.confusionTimer = min(MAX_BITS(3), newValue);
            break;
        */
    default: // Invalid volatile status
            return;
    }
}

bool32 ItemHealMonVolatile(enum BattlerId battler, enum Item itemId)
{
    bool32 statusChanged = FALSE;
    const u8 *effect = GetItemEffect(itemId);
    if (effect[3] & ITEM3_STATUS_ALL)
    {
        statusChanged = (gBattleMons[battler].volatiles.infatuation || gBattleMons[battler].volatiles.confusionTimer > 0);
        gBattleMons[battler].volatiles.infatuation = 0;
        gBattleMons[battler].volatiles.confusionTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_CONFUSION;
    }
    else if (effect[0] & ITEM0_INFATUATION)
    {
        statusChanged = !!gBattleMons[battler].volatiles.infatuation;
        gBattleMons[battler].volatiles.infatuation = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_INFATUATION;
    }
    else if (effect[3] & ITEM3_CONFUSION)
    {
        statusChanged = (gBattleMons[battler].volatiles.confusionTimer > 0);
        gBattleMons[battler].volatiles.confusionTimer = 0;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_CONFUSION;
    }

    return statusChanged;
}

// Hazards are added to a queue and applied based in order (FIFO)
void PushHazardTypeToQueue(enum BattleSide side, enum Hazards hazardType)
{
    if (!IsHazardOnSide(side, hazardType)) // Failsafe
        gBattleStruct->hazardsQueue[side][gBattleStruct->numHazards[side]++] = hazardType;
}

bool32 IsHazardOnSide(enum BattleSide side, enum Hazards hazardType)
{
    for (u32 i = 0; i < HAZARDS_MAX_COUNT; i++)
    {
        if (gBattleStruct->hazardsQueue[side][i] == hazardType)
            return TRUE;
    }
    return FALSE;
}

bool32 AreAnyHazardsOnSide(enum BattleSide side)
{
    return gBattleStruct->numHazards[side] > 0;
}

bool32 IsHazardOnSideAndClear(enum BattleSide side, enum Hazards hazardType)
{
    for (u32 i = 0; i < HAZARDS_MAX_COUNT; i++)
    {
        if (gBattleStruct->hazardsQueue[side][i] == hazardType)
        {
            gBattleStruct->hazardsQueue[side][i] = HAZARDS_NONE;
            if (hazardType == HAZARDS_SPIKES)
                gSideTimers[side].spikesAmount = 0;
            else if (hazardType == HAZARDS_TOXIC_SPIKES)
                gSideTimers[side].toxicSpikesAmount = 0;
            return TRUE;
        }
    }
    return FALSE;
}

void RemoveHazardFromField(enum BattleSide side, enum Hazards hazardType)
{
    u32 i;
    for (i = 0; i < HAZARDS_MAX_COUNT; i++)
    {
        if (gBattleStruct->hazardsQueue[side][i] == hazardType)
        {
            gBattleStruct->hazardsQueue[side][i] = HAZARDS_NONE;
            gBattleStruct->numHazards[side]--;
            if (hazardType == HAZARDS_SPIKES)
                gSideTimers[side].spikesAmount = 0;
            else if (hazardType == HAZARDS_TOXIC_SPIKES)
                gSideTimers[side].toxicSpikesAmount = 0;
            break;
        }
    }
    while (i < HAZARDS_MAX_COUNT)
    {
        if (i + 1 == HAZARDS_MAX_COUNT)
        {
            gBattleStruct->hazardsQueue[side][i] = HAZARDS_NONE;
            break;
        }
        gBattleStruct->hazardsQueue[side][i] = gBattleStruct->hazardsQueue[side][i+1];
        i++;
    }
}

static bool32 CanMoveSkipAccuracyCheck(enum BattlerId battlerAtk, enum Move move)
{
    return MoveAlwaysHitsOnSameType(move) && IS_BATTLER_OF_TYPE(battlerAtk, GetMoveType(move));
}

bool32 CanMoveSkipAccuracyCalc(struct BattleCalcValues *cv, u32 weather, enum ResultOption option)
{
    bool32 effect = FALSE;
    enum BattlerId battlerAtk = cv->battlerAtk;
    enum BattlerId battlerDef = cv->battlerDef;
    enum Ability ability = ABILITY_NONE;
    enum BattlerId abilityBattler = battlerAtk;
    enum BattleMoveEffects moveEffect = GetMoveEffect(cv->move);
    bool32 shouldAvoidSkyDropBattlers = (moveEffect != EFFECT_SKY_DROP && IsBattlerInvolvedInSkyDrop(battlerDef));

    if (gBattleMons[battlerAtk].volatiles.battlerWithSureHit == battlerDef + 1
     || CanMoveSkipAccuracyCheck(battlerAtk, cv->move)
     || gBattleMons[battlerDef].volatiles.glaiveRush)
    {
        effect = TRUE;
    }
    else if (cv->abilities[battlerAtk] == ABILITY_NO_GUARD
          && gBattleMons[battlerDef].volatiles.semiInvulnerable != STATE_COMMANDER
          && !shouldAvoidSkyDropBattlers)
    {
        effect = TRUE;
        ability = ABILITY_NO_GUARD;
        abilityBattler = battlerAtk;
    }
    else if (cv->abilities[battlerDef] == ABILITY_NO_GUARD && !shouldAvoidSkyDropBattlers)
    {
        effect = TRUE;
        ability = ABILITY_NO_GUARD;
        abilityBattler = battlerDef;
    }
    // If the target is under the effects of Telekinesis, and the move isn't a OH-KO move, move hits.
    else if (gBattleMons[battlerDef].volatiles.telekinesis
          && !IsSemiInvulnerable(battlerDef, CHECK_ALL)
          && moveEffect != EFFECT_OHKO)
    {
        effect = TRUE;
    }
    else if (gBattleStruct->battlerState[battlerDef].pursuitTarget)
    {
        effect = TRUE;
    }
    else if (GetActiveGimmick(battlerAtk) == GIMMICK_Z_MOVE && !IsSemiInvulnerable(battlerDef, CHECK_ALL))
    {
        effect = TRUE;
    }
    else if (B_MINIMIZE_DMG_ACC >= GEN_6
     && gBattleMons[battlerDef].volatiles.minimize
     && MoveIncreasesPowerToMinimizedTargets(cv->move))
    {
        effect = TRUE;
    }
    else if (GetMoveAccuracy(cv->move) == 0)
    {
        effect = TRUE;
    }
    else
    {
        u32 attackerWeather = GetAttackerWeather(cv->holdEffects[battlerAtk], cv->abilities[battlerAtk], weather);

        if ((attackerWeather & B_WEATHER_RAIN) && MoveAlwaysHitsInRain(cv->move))
            effect = TRUE;
        else if ((attackerWeather & B_WEATHER_ICY_ANY) && MoveAlwaysHitsInHailSnow(cv->move))
            effect = TRUE;

        if (effect)
            return effect;
    }

    if (ability != ABILITY_NONE && option == RUN_SCRIPT)
        RecordAbilityBattle(abilityBattler, ability);

    return effect;
}

u32 GetTotalAccuracy(struct BattleCalcValues *cv, u32 weather)
{
    s32 buff, accStage, evasionStage;

    enum BattlerId battlerAtk = cv->battlerAtk;
    enum BattlerId battlerDef = cv->battlerDef;

    u32 atkParam = GetBattlerHoldEffectParam(battlerAtk);
    u32 defParam = GetBattlerHoldEffectParam(battlerDef);

    gPotentialItemEffectBattler = battlerDef;

    accStage = gBattleMons[battlerAtk].statStages[STAT_ACC];
    evasionStage = gBattleMons[battlerDef].statStages[STAT_EVASION];

    if (cv->abilities[battlerAtk] == ABILITY_UNAWARE
     || cv->abilities[battlerAtk] == ABILITY_KEEN_EYE
     || cv->abilities[battlerAtk] == ABILITY_MINDS_EYE
     || (GetConfig(B_ILLUMINATE_EFFECT) >= GEN_9 && cv->abilities[battlerAtk] == ABILITY_ILLUMINATE))
        evasionStage = DEFAULT_STAT_STAGE;
    if (MoveIgnoresDefenseEvasionStages(cv->move))
        evasionStage = DEFAULT_STAT_STAGE;
    if (cv->abilities[battlerDef] == ABILITY_UNAWARE)
        accStage = DEFAULT_STAT_STAGE;

    if (gBattleMons[battlerDef].volatiles.foresight || gBattleMons[battlerDef].volatiles.miracleEye)
        buff = accStage;
    else
        buff = accStage + DEFAULT_STAT_STAGE - evasionStage;

    if (buff < MIN_STAT_STAGE)
        buff = MIN_STAT_STAGE;
    if (buff > MAX_STAT_STAGE)
        buff = MAX_STAT_STAGE;

    u32 moveAcc = GetMoveAccuracy(cv->move);
    u32 attackerWeather = GetAttackerWeather(cv->holdEffects[battlerAtk], cv->abilities[battlerAtk], weather);

    // Check Thunder and Hurricane on sunny weather.
    if ((attackerWeather & B_WEATHER_SUN) && MoveHas50AccuracyInSun(cv->move))
        moveAcc = 50;
    // Check Wonder Skin.
    if (cv->abilities[battlerDef] == ABILITY_WONDER_SKIN && IsBattleMoveStatus(cv->move) && moveAcc > 50)
        moveAcc = 50;

    u32 calc = gAccuracyStageRatios[buff].dividend * moveAcc;
    calc /= gAccuracyStageRatios[buff].divisor;

    // Attacker's ability
    switch (cv->abilities[battlerAtk])
    {
    case ABILITY_COMPOUND_EYES:
        calc = (calc * 130) / 100; // 1.3 compound eyes boost
        break;
    case ABILITY_VICTORY_STAR:
        calc = (calc * 110) / 100; // 1.1 victory star boost
        break;
    case ABILITY_HUSTLE:
        if (IsBattleMovePhysical(cv->move))
            calc = (calc * 80) / 100; // 1.2 hustle loss
        break;
    default:
        break;
    }

    // Target's ability
    switch (cv->abilities[battlerDef])
    {
    case ABILITY_SAND_VEIL:
        if (attackerWeather & B_WEATHER_SANDSTORM)
            calc = (calc * 80) / 100; // 1.2 sand veil loss
        break;
    case ABILITY_SNOW_CLOAK:
        if (attackerWeather & B_WEATHER_ICY_ANY)
            calc = (calc * 80) / 100; // 1.2 snow cloak loss
        break;
    case ABILITY_TANGLED_FEET:
        if (gBattleMons[battlerDef].volatiles.confusionTimer)
            calc = (calc * 50) / 100; // 1.5 tangled feet loss
        break;
    default:
        break;
    }

    // Attacker's ally's ability
    enum BattlerId atkAlly = GetPartnerBattler(battlerAtk);
    switch (cv->abilities[atkAlly])
    {
    case ABILITY_VICTORY_STAR:
        if (IsBattlerAlive(atkAlly))
            calc = (calc * 110) / 100; // 1.1 ally's victory star boost
        break;
    default:
        break;
    }

    if (MoveDecreasesAccIfUserNotSameType(cv->move) && !IS_BATTLER_OF_TYPE(battlerAtk, GetBattleMoveType(cv->move)))
        calc = (calc * 90) / 100;

    // Attacker's hold effect
    switch (cv->holdEffects[battlerAtk])
    {
    case HOLD_EFFECT_WIDE_LENS:
        calc = (calc * (100 + atkParam)) / 100;
        break;
    case HOLD_EFFECT_ZOOM_LENS:
        if (HasBattlerActedThisTurn(battlerDef) && !BattlerJustSwitchedIn(battlerDef))
            calc = (calc * (100 + atkParam)) / 100;
        break;
    default:
        break;
    }

    // Target's hold effect
    switch (cv->holdEffects[battlerDef])
    {
    case HOLD_EFFECT_EVASION_UP:
        calc = (calc * (100 - defParam)) / 100;
        break;
    default:
        break;
    }

    if (gBattleStruct->battlerState[battlerAtk].usedMicleBerry)
        calc = (calc * 120) / 100;  // 20% acc boost

    if (gFieldStatuses & STATUS_FIELD_GRAVITY)
        calc = (calc * 5) / 3; // 1.66 Gravity acc boost

    if (B_AFFECTION_MECHANICS == TRUE && GetBattlerAffectionHearts(battlerDef) == AFFECTION_FIVE_HEARTS)
        calc = (calc * 90) / 100;

    if (weather & B_WEATHER_FOG)
        calc = (calc * 60) / 100; // modified by 3/5

    return calc;
}

bool32 DoesOHKOMoveMissTarget(struct BattleCalcValues *cv)
{
    enum OHKOResult {
        NO_HIT,
        CALC_ACC,
        SURE_HIT,
    };

    // Dynamaxed Pokemon cannot be hit by OHKO moves.
    if (GetActiveGimmick(cv->battlerDef) == GIMMICK_DYNAMAX)
    {
        gBattleStruct->moveResultFlags[cv->battlerDef] |= MOVE_RESULT_ONE_HIT_KO_NO_AFFECT;
        return TRUE;
    }

    if (gBattleMons[cv->battlerDef].level > gBattleMons[cv->battlerAtk].level)
    {
        gBattleStruct->moveResultFlags[cv->battlerDef] |= MOVE_RESULT_ONE_HIT_KO_NO_AFFECT;
        return TRUE;
    }

    if (cv->abilities[cv->battlerDef] == ABILITY_STURDY)
    {
        gBattleStruct->moveResultFlags[cv->battlerDef] |= MOVE_RESULT_ONE_HIT_KO_STURDY;
        return TRUE;
    }

    enum OHKOResult lands = NO_HIT;

    if (gBattleMons[cv->battlerDef].volatiles.glaiveRush
     || gBattleMons[cv->battlerAtk].volatiles.battlerWithSureHit == cv->battlerDef + 1
     || IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_NO_GUARD)
     || IsAbilityAndRecord(cv->battlerDef, cv->abilities[cv->battlerDef], ABILITY_NO_GUARD))
    {
        lands = SURE_HIT;
    }
    else
    {
        lands = CALC_ACC;
    }

    if (lands == CALC_ACC)
    {
        u32 odds = GetMoveAccuracy(cv->move) + (gBattleMons[cv->battlerAtk].level - gBattleMons[cv->battlerDef].level);
        if (MoveDecreasesAccIfUserNotSameType(cv->move) && !IS_BATTLER_OF_TYPE(cv->battlerAtk, GetBattleMoveType(cv->move)))
            odds -= 10;
        if (RandomPercentage(RNG_ACCURACY, odds) && gBattleMons[cv->battlerAtk].level >= gBattleMons[cv->battlerDef].level)
            lands = SURE_HIT;
    }

    if (lands == SURE_HIT)
    {
        gBattleStruct->moveResultFlags[cv->battlerDef] |= MOVE_RESULT_ONE_HIT_KO_NO_AFFECT;
        return FALSE;
    }

    return TRUE;
}

bool32 DoesMoveMissTarget(struct BattleCalcValues *cv)
{
    if (GetMoveEffect(cv->move) == EFFECT_OHKO)
        return DoesOHKOMoveMissTarget(cv);

    u32 weather = GetWeather();

    if (CanMoveSkipAccuracyCalc(cv, weather, RUN_SCRIPT))
        return FALSE;

    u32 accuracy = GetTotalAccuracy(cv, weather);

    return !RandomPercentage(RNG_ACCURACY, accuracy);
}

bool32 IsSemiInvulnerable(enum BattlerId battler, enum SemiInvulnerableExclusion excludeCommander)
{
    if (gBattleMons[battler].volatiles.semiInvulnerable == STATE_COMMANDER)
        return excludeCommander != EXCLUDE_COMMANDER;
    return gBattleMons[battler].volatiles.semiInvulnerable != STATE_NONE;
}

static bool32 CanBreakThroughSemiInvulnerablityInternal(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Ability abilityAtk, enum Ability abilityDef, enum Move move, enum SemiInvulnerableState state)
{
    if (state != STATE_COMMANDER)
    {
        if (CanMoveSkipAccuracyCheck(battlerAtk, move))
            return TRUE;
        if (abilityAtk == ABILITY_NO_GUARD || abilityDef == ABILITY_NO_GUARD)
            return TRUE;
        if (gBattleMons[battlerAtk].volatiles.battlerWithSureHit == battlerDef + 1)
            return TRUE;
    }

    switch (state)
    {
    case STATE_UNDERGROUND:
        return MoveDamagesUnderground(move);
    case STATE_UNDERWATER:
        return MoveDamagesUnderWater(move);
    case STATE_ON_AIR:
    case STATE_SKY_DROP_ATTACKER:
    case STATE_SKY_DROP_TARGET:
        return MoveDamagesAirborne(move) || MoveDamagesAirborneDoubleDamage(move);
    case STATE_PHANTOM_FORCE:
        return FALSE;
    case STATE_COMMANDER:
        return GetMoveEffect(move) == EFFECT_TRANSFORM;
    case STATE_NONE:
    case SEMI_INVULNERABLE_COUNT:
        return TRUE;
    }

    return FALSE;
}

bool32 CanBreakThroughSemiInvulnerablity(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Ability abilityAtk, enum Ability abilityDef, enum Move move)
{
    return CanBreakThroughSemiInvulnerablityInternal(battlerAtk, battlerDef, abilityAtk, abilityDef, move, gBattleMons[battlerDef].volatiles.semiInvulnerable);
}

bool32 BreaksThroughSemiInvulnerableState(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Ability abilityAtk, enum Ability abilityDef, enum Move move, enum SemiInvulnerableState state)
{
    return CanBreakThroughSemiInvulnerablityInternal(battlerAtk, battlerDef, abilityAtk, abilityDef, move, state);
}

bool32 IsBattlerOnAir(enum BattlerId battler)
{
    switch (gBattleMons[battler].volatiles.semiInvulnerable)
    {
    case STATE_ON_AIR:
    case STATE_SKY_DROP_ATTACKER:
    case STATE_SKY_DROP_TARGET:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 HasPartnerTrainer(enum BattlerId battler)
{
    if ((GetBattlerSide(battler) == B_SIDE_PLAYER && gBattleTypeFlags & BATTLE_TYPE_PLAYER_HAS_PARTNER)
     || (GetBattlerSide(battler) == B_SIDE_OPPONENT && gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS))
        return TRUE;
    else
        return FALSE;
}

static bool32 IsOpposingSideEmpty(enum BattlerId battler)
{
    enum BattlerId oppositeBattler = GetOppositeBattler(battler);

    if (IsBattlerAlive(oppositeBattler))
        return FALSE;

    if (!IsDoubleBattle())
        return TRUE;

    if (IsBattlerAlive(GetPartnerBattler(oppositeBattler)))
        return FALSE;
    return TRUE;
}

bool32 IsAffectedByPowderMove(enum BattlerId battler, enum Ability ability, enum HoldEffect holdEffect)
{
    if (GetConfig(B_POWDER_OVERCOAT) >= GEN_6 && ability == ABILITY_OVERCOAT)
        return FALSE;
    if (GetConfig(B_POWDER_GRASS) >= GEN_6 && IS_BATTLER_OF_TYPE(battler, TYPE_GRASS))
        return FALSE;
    if (holdEffect == HOLD_EFFECT_SAFETY_GOGGLES)
        return FALSE;
    return TRUE;
}

void RemoveAbilityFlags(enum BattlerId battler)
{
    gBattleMons[battler].volatiles.unburdenActive = FALSE;
    gBattleMons[battler].volatiles.traceActivated = FALSE;

    switch (GetBattlerAbility(battler))
    {
    case ABILITY_NEUTRALIZING_GAS:
        gSpecialStatuses[battler].neutralizingGasRemoved = TRUE;
        break;
    case ABILITY_FLASH_FIRE:
        gBattleMons[battler].volatiles.flashFireBoosted = FALSE;
        break;
    case ABILITY_VESSEL_OF_RUIN:
        gBattleMons[battler].volatiles.vesselOfRuin = FALSE;
        break;
    case ABILITY_TABLETS_OF_RUIN:
        gBattleMons[battler].volatiles.tabletsOfRuin = FALSE;
        break;
    case ABILITY_SWORD_OF_RUIN:
        gBattleMons[battler].volatiles.swordOfRuin = FALSE;
        break;
    case ABILITY_BEADS_OF_RUIN:
        gBattleMons[battler].volatiles.beadsOfRuin = FALSE;
        break;
    default:
       break;
    }
}

void RemoveRuinAbilityFlags(enum BattlerId battler)
{
    switch (GetBattlerAbility(battler))
    {
    case ABILITY_VESSEL_OF_RUIN:
        gBattleMons[battler].volatiles.vesselOfRuin = FALSE;
        break;
    case ABILITY_TABLETS_OF_RUIN:
        gBattleMons[battler].volatiles.tabletsOfRuin = FALSE;
        break;
    case ABILITY_SWORD_OF_RUIN:
        gBattleMons[battler].volatiles.swordOfRuin = FALSE;
        break;
    case ABILITY_BEADS_OF_RUIN:
        gBattleMons[battler].volatiles.beadsOfRuin = FALSE;
        break;
    default:
       break;
    }
}

void CheckSetUnburden(enum BattlerId battler)
{
    if (!(gFieldStatuses & STATUS_FIELD_MAGIC_ROOM)
        && !gBattleMons[battler].volatiles.embargoTimer
        && IsAbilityAndRecord(battler, GetBattlerAbility(battler), ABILITY_UNBURDEN))
    {
        gBattleMons[battler].volatiles.unburdenActive = TRUE;
    }
}

bool32 IsAnyTargetTurnDamaged(enum BattlerId battlerAtk, enum SubCheck subCheck)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (battlerDef == battlerAtk)
            continue;
        if (IsBattlerTurnDamaged(battlerDef, subCheck))
            return TRUE;
    }
    return FALSE;
}

bool32 IsAnyTargetAffected(void)
{
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove);
    bool32 isSpreadMove = IsSpreadMove(moveTarget);

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_NOT_PRESENT)
            continue;

        switch (moveTarget)
        {
        case TARGET_ALL_BATTLERS: // check all battlers
            break;
        case TARGET_USER_AND_ALLY: // only check allied battlers
            if (!IsBattlerAlly(gBattlerAttacker, battler))
                continue;
            break;
        default:
            if (isSpreadMove) // check all battlers except attacker (flags are set for non-targeted battlers)
            {
                if (battler == gBattlerAttacker)
                    continue;
            }
            else // check a single target
            {
                if (battler != gBattlerTarget)
                    continue;
            }
            break;
        }

        if (!IsBattlerUnaffectedByMove(battler))
            return TRUE;
    }

    return FALSE;
}

bool32 IsDoubleSpreadMove(void)
{
    return gBattleStruct->numSpreadTargets > 1
        && !gBattleStruct->unableToUseMove
        && IsSpreadMove(GetBattlerMoveTargetType(gBattlerAttacker, gCurrentMove));
}

bool32 IsBattlerInvalidForSpreadMove(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    return battlerDef == battlerAtk
        || !IsBattlerAlive(battlerDef)
        || IsBattlerUnaffectedByMove(battlerDef);
}

bool32 IsAllowedToUseBag(void)
{
    switch (VarGet(B_VAR_NO_BAG_USE))
    {
    case NO_BAG_RESTRICTION:
        return TRUE;
    case NO_BAG_AGAINST_TRAINER: //True in wild battle; False in trainer battle
        return (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER));
    case NO_BAG_IN_BATTLE:
        return FALSE;
    default:
        return TRUE; // Undefined Behavior
    }
}

bool32 IsMimikyuDisguised(enum BattlerId battler)
{
    return gBattleMons[battler].species == SPECIES_MIMIKYU_DISGUISED
        || gBattleMons[battler].species == SPECIES_MIMIKYU_TOTEM_DISGUISED;
}

#define UNPACK_STARTING_STATUS_TO_EWRAM(_enum, _fieldName, ...) case _enum: gStartingStatuses._fieldName = TRUE; break;

void SetStartingStatus(enum StartingStatus status)
{
    switch (status)
    {
    STARTING_STATUS_DEFINITIONS(UNPACK_STARTING_STATUS_TO_EWRAM);
    }
}

#define UNPACK_STARTING_STATUS_RESET(_enum, _fieldName, ...) gStartingStatuses._fieldName = FALSE;

void ResetStartingStatuses(void)
{
    STARTING_STATUS_DEFINITIONS(UNPACK_STARTING_STATUS_RESET);
}

bool32 IsUsableWhileAsleepEffect(enum BattleMoveEffects effect)
{
    // All moves usable while asleep like Snore, Sleep Talk, etc.
    switch (effect)
    {
    case EFFECT_SNORE:
    case EFFECT_SLEEP_TALK:
        return TRUE;
    default:
        return FALSE;
    }
}

void SetWrapTurns(enum BattlerId battler, enum HoldEffect holdEffect)
{
    u32 normalWrapTurns = B_WRAP_TURNS - 2; // 5 turns
    if (holdEffect == HOLD_EFFECT_GRIP_CLAW)
    {
        gBattleMons[battler].volatiles.wrapTurns = GetConfig(B_BINDING_TURNS) >= GEN_5 ? B_WRAP_TURNS : normalWrapTurns;
        gBattleMons[battler].volatiles.wrappedBindingBand = FALSE;
    }
    else
    {
        gBattleMons[battler].volatiles.wrapTurns = GetConfig(B_BINDING_TURNS) >= GEN_5 ? RandomUniform(RNG_WRAP, 4, normalWrapTurns) : RandomUniform(RNG_WRAP, 2, normalWrapTurns);
        gBattleMons[battler].volatiles.wrappedBindingBand = holdEffect == HOLD_EFFECT_BINDING_BAND;
    }
}

// Return True if the order was changed, and false if the order was not changed(for example because the target would move after the attacker anyway).
bool32 ChangeOrderTargetAfterAttacker(enum BattlerId battlerDef)
{
    u8 data[MAX_BATTLERS_COUNT];
    u8 actionsData[MAX_BATTLERS_COUNT];
    u32 attackerTurnOrderNum = GetBattlerTurnOrderNum(gBattlerAttacker);
    u32 targetTurnOrderNum = GetBattlerTurnOrderNum(battlerDef);

    if (attackerTurnOrderNum > targetTurnOrderNum)
        return FALSE;
    if (attackerTurnOrderNum + 1 == targetTurnOrderNum)
        return GetConfig(B_AFTER_YOU_TURN_ORDER) >= GEN_8;

    for (enum BattlerId i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        data[i] = gBattlerByTurnOrder[i];
        actionsData[i] = gActionsByTurnOrder[i];
    }
    if (attackerTurnOrderNum == 0 && targetTurnOrderNum == 2)
    {
        gBattlerByTurnOrder[1] = battlerDef;
        gActionsByTurnOrder[1] = actionsData[2];
        gBattlerByTurnOrder[2] = data[1];
        gActionsByTurnOrder[2] = actionsData[1];
    }
    else if (attackerTurnOrderNum == 0 && targetTurnOrderNum == 3)
    {
        gBattlerByTurnOrder[1] = battlerDef;
        gActionsByTurnOrder[1] = actionsData[3];
        gBattlerByTurnOrder[2] = data[1];
        gActionsByTurnOrder[2] = actionsData[1];
        gBattlerByTurnOrder[3] = data[2];
        gActionsByTurnOrder[3] = actionsData[2];
    }
    else // attackerTurnOrderNum == 1, targetTurnOrderNum == 3
    {
        gBattlerByTurnOrder[2] = battlerDef;
        gActionsByTurnOrder[2] = actionsData[3];
        gBattlerByTurnOrder[3] = data[2];
        gActionsByTurnOrder[3] = actionsData[2];
    }
    return TRUE;
}

void TryUpdateEvolutionTracker(enum EvolutionConditions evolutionCondition, u32 upAmount, enum Move usedMove)
{
    u32 i, j;

    if (IsOnPlayerSide(gBattlerAttacker)
     && ((TESTING && IsDoubleBattle()) // To be removed when Wild Double Battles are added to tests
     || !(gBattleTypeFlags & (BATTLE_TYPE_LINK
                             | BATTLE_TYPE_EREADER_TRAINER
                             | BATTLE_TYPE_RECORDED_LINK
                             | BATTLE_TYPE_TRAINER_HILL
                             | BATTLE_TYPE_FRONTIER))))
    {
        const struct Evolution *evolutions = GetSpeciesEvolutions(gBattleMons[gBattlerAttacker].species);
        if (evolutions == NULL)
            return;

        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;
            if (evolutions[i].params == NULL)
                continue;

            for (j = 0; evolutions[i].params[j].condition != CONDITIONS_END; j++)
            {
                if (evolutions[i].params[j].condition != evolutionCondition)
                    continue;

                struct Pokemon *monAtk = GetBattlerMon(gBattlerAttacker);
                struct Pokemon *monDef = GetBattlerMon(gBattlerTarget);
                // We only have 10 bits to use
                u16 val = min(1023, GetMonData(monAtk, MON_DATA_EVOLUTION_TRACKER) + upAmount);
                // Reset progress if you faint for the recoil method.
                switch (evolutionCondition)
                {
                case IF_USED_MOVE_X_TIMES:
                    if (evolutions[i].params[j].arg1 == usedMove)
                        SetMonData(monAtk, MON_DATA_EVOLUTION_TRACKER, &val);
                    break;
                case IF_RECOIL_DAMAGE_GE:
                    if (gBattleMons[gBattlerAttacker].hp == 0)
                        val = 0;
                    SetMonData(monAtk, MON_DATA_EVOLUTION_TRACKER, &val);
                    break;
                case IF_DEFEAT_X_WITH_ITEMS:
                    if (GetMonData(monDef, MON_DATA_SPECIES) == evolutions[i].params[j].arg1
                     && GetMonData(monDef, MON_DATA_HELD_ITEM) == evolutions[i].params[j].arg2)
                        SetMonData(monAtk, MON_DATA_EVOLUTION_TRACKER, &val);
                    break;
                default:
                    assertf(FALSE, "evolution condition %d is not handled within TryUpdateEvolutionTracker", evolutionCondition) {}
                    break;
                }
                return;
            }
        }
    }
}

static const u16 sProtectFailChances[] =
{
    1,
    2,
    4,
    8
};

static const u16 sGen5ProtectFailChances[] =
{
    1,
    3,
    9,
    27
};

bool32 CanUseMoveConsecutively(enum BattlerId battler)
{
    u32 moveUses = gBattleMons[battler].volatiles.consecutiveMoveUses;
    if (moveUses >= ARRAY_COUNT(sProtectFailChances))
        moveUses = ARRAY_COUNT(sProtectFailChances) - 1;

    u32 failChances;

    if (B_PROTECT_FAILURE_RATE < GEN_5)
        failChances = sProtectFailChances[moveUses];
    else
        failChances = sGen5ProtectFailChances[moveUses];

    if (failChances == 1)
        return TRUE;

    return RandomUniform(RNG_PROTECT_FAIL, 1, failChances) == 1;
}

// Used for Protect, Endure and Ally switch
void TryResetConsecutiveUseCounter(enum BattlerId battler)
{
    enum Move lastMove = gLastResultingMoves[battler];
    if (lastMove == MOVE_UNAVAILABLE)
    {
        gBattleMons[battler].volatiles.consecutiveMoveUses = 0;
        return;
    }

    enum BattleMoveEffects lastEffect = GetMoveEffect(lastMove);
    if (!gBattleMoveEffects[lastEffect].usesProtectCounter)
    {
        if (GetConfig(B_ALLY_SWITCH_FAIL_CHANCE) < GEN_9 || lastEffect != EFFECT_ALLY_SWITCH)
            gBattleMons[battler].volatiles.consecutiveMoveUses = 0;
    }
}

void SetOrClearRageVolatile(void)
{
    if (GetConfig(B_RAGE_BUILDS) <= GEN_3 && MoveHasAdditionalEffectSelf(gCurrentMove, MOVE_EFFECT_RAGE))
        gBattleMons[gBattlerAttacker].volatiles.rage = TRUE;
    else
        gBattleMons[gBattlerAttacker].volatiles.rage = FALSE;
}

enum BattlerId GetTargetBySlot(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (IsDoubleBattle())
        return GetTargetFromSlotId(battlerAtk, battlerDef);
    return battlerDef;
}

enum BattlerId GetTargetFromSlotId(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    switch (battlerDef)
    {
    case B_BATTLER_0:
        return battlerAtk;
    case B_BATTLER_1:
        return GetPartnerBattler(battlerAtk);
    case B_BATTLER_2:
        return GetBattlerLeftFoe(battlerAtk);
    case B_BATTLER_3:
        return GetBattlerRightFoe(battlerAtk);
    default:
        errorf("Illegal battler");
        return B_BATTLER_0;
    }
}

bool32 IsNaturalEnemy(enum Species speciesAttacker, enum Species speciesTarget)
{
    if (WE_WILD_NATURAL_ENEMIES != TRUE)
        return FALSE;

    switch (speciesAttacker)
    {
    case SPECIES_ZANGOOSE:
        return (speciesTarget == SPECIES_SEVIPER);
    case SPECIES_SEVIPER:
        return (speciesTarget == SPECIES_ZANGOOSE);
    case SPECIES_HEATMOR:
        return (speciesTarget == SPECIES_DURANT);
    case SPECIES_DURANT:
        return (speciesTarget == SPECIES_HEATMOR);
    case SPECIES_SABLEYE:
        return (speciesTarget == SPECIES_CARBINK);
    case SPECIES_MAREANIE:
        return (speciesTarget == SPECIES_CORSOLA);
    default:
        return FALSE;
    }
    return FALSE;
}

enum Stat GetDownloadStat(enum BattlerId battler)
{
    enum BattlerId opposingBattler;
    u32 opposingDef = 0, opposingSpDef = 0;

    opposingBattler = GetOppositeBattler(battler);
    for (u32 i = 0; i < 2; opposingBattler ^= BIT_FLANK, i++)
    {
        if (IsBattlerAlive(opposingBattler))
        {
            opposingDef += gBattleMons[opposingBattler].defense
                        * gStatStageRatios[gBattleMons[opposingBattler].statStages[STAT_DEF]][0]
                        / gStatStageRatios[gBattleMons[opposingBattler].statStages[STAT_DEF]][1];
            opposingSpDef += gBattleMons[opposingBattler].spDefense
                            * gStatStageRatios[gBattleMons[opposingBattler].statStages[STAT_SPDEF]][0]
                            / gStatStageRatios[gBattleMons[opposingBattler].statStages[STAT_SPDEF]][1];
        }
    }

    if (opposingDef < opposingSpDef)
        return STAT_ATK;
    else
        return STAT_SPATK;
}

bool32 BattlerJustSwitchedIn(enum BattlerId battler)
{
    return gBattleStruct->battlerState[battler].isFirstTurn == 2;
}

bool32 IsBattlersFirstTurn(enum BattlerId battler)
{
    return gBattleStruct->battlerState[battler].isFirstTurn == 1
        || gBattleStruct->battlerState[battler].isFirstTurn == 2;
}

struct PartyState *GetBattlerPartyState(enum BattlerId battler)
{
    return &gBattleStruct->partyState[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]];
}

void SetValuesOnFaint(enum BattlerId battler)
{
    gHitMarker |= HITMARKER_FAINTED(battler);
    gBattleStruct->eventState.faintedAction = 0;
    gBattlerFainted = battler;
    TryDeactivateSleepClause(battler, gBattlerPartyIndexes[battler]);

    if (gBattleStruct->faintCounter[GetBattlerTrainer(battler)] < 255)
        gBattleStruct->faintCounter[GetBattlerTrainer(battler)]++;

    if (IsOnPlayerSide(battler))
    {
        gHitMarker |= HITMARKER_PLAYER_FAINTED;
        if (gBattleResults.playerFaintCounter < 255)
            gBattleResults.playerFaintCounter++;
        AdjustFriendshipOnBattleFaint(battler);
        gSideTimers[B_SIDE_PLAYER].retaliateTimer = 2;
    }
    else
    {
        if (gBattleResults.opponentFaintCounter < 255)
            gBattleResults.opponentFaintCounter++;
        gBattleResults.lastOpponentSpecies = GetMonData(GetBattlerMon(battler), MON_DATA_SPECIES);
        gSideTimers[B_SIDE_OPPONENT].retaliateTimer = 2;
    }
}

bool32 IsVictoryCatch(void)
{
    return gBattleTypeFlags & BATTLE_TYPE_RAID
        || FlagGet(B_FLAG_VICTORY_CATCH_RANDOM)
        || FlagGet(B_FLAG_VICTORY_CATCH_GUARANTEED);
}

bool32 IsVictoryCatchGuaranteed(void)
{
    return gBattleTypeFlags & BATTLE_TYPE_RAID
        || FlagGet(B_FLAG_VICTORY_CATCH_GUARANTEED);
}

bool32 IsBattlerInvolvedInSkyDrop(enum BattlerId battler)
{
    return gBattleMons[battler].volatiles.semiInvulnerable == STATE_SKY_DROP_ATTACKER
        || gBattleMons[battler].volatiles.semiInvulnerable == STATE_SKY_DROP_TARGET;
}

bool32 IsAsleepOrComatose(enum BattlerId battler, enum Ability ability)
{
    return (gBattleMons[battler].status1 & STATUS1_SLEEP) || ability == ABILITY_COMATOSE;
}
