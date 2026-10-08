#include "global.h"
#include "battle.h"
#include "battle_hold_effects.h"
#include "battle_setup.h"
#include "battle_util.h"
#include "battle_controllers.h"
#include "battle_ai_record.h"
#include "battle_stat_change.h"
#include "battle_gimmick.h"
#include "battle_scripts.h"
#include "constants/battle.h"
#include "constants/battle_string_ids.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"

static enum BattlerId GetBattlerSideForMessage(enum BattleSide side)
{
    enum BattlerId battler;

    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battler) == side)
            break;
    }

    return battler;
}

static bool32 HandleEndTurnOrder(enum BattlerId battler)
{
    bool32 effect = FALSE;

    gBattleTurnCounter++;
    gBattleStruct->eventState.endTurn++;

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
        gBattlerByTurnOrder[i] = i;
    SortBattlersBySpeed(gBattlerByTurnOrder, FALSE);

    return effect;
}

static bool32 HandleEndTurnVarious(enum BattlerId battler)
{
    bool32 effect = FALSE;

    gBattleStruct->eventState.endTurn++;

    if (gFieldTimers.fairyLockTimer > 0 && --gFieldTimers.fairyLockTimer == 0)
        gFieldStatuses &= ~STATUS_FIELD_FAIRY_LOCK;

    for (enum BattleSide i = 0; i < NUM_BATTLE_SIDES; i++)
    {
        if (gSideTimers[i].damageNonTypesTimer > 0 && --gSideTimers[i].damageNonTypesTimer == 0)
            gSideStatuses[i] &= ~SIDE_STATUS_DAMAGE_NON_TYPES;
    }

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].volatiles.fearTimer > 0)
            gBattleMons[i].volatiles.fearTimer--;

        if (gBattleMons[i].volatiles.throatChopTimer > 0)
        {
            gBattleMons[i].volatiles.throatChopTimer--;
            if (gBattleMons[i].volatiles.uproarTurns)
                gBattleMons[i].volatiles.uproarTurns = 1; // end the move this turn
        }

        if (gBattleMons[i].volatiles.lockOn > 0 && --gBattleMons[i].volatiles.lockOn == 0)
            gBattleMons[i].volatiles.battlerWithSureHit = 0;

        if (B_CHARGE < GEN_9 && gBattleMons[i].volatiles.chargeTimer > 0)
            gBattleMons[i].volatiles.chargeTimer--;

        if (gBattleMons[i].volatiles.laserFocusTimer > 0)
            gBattleMons[i].volatiles.laserFocusTimer--;

        gBattleStruct->battlerState[i].wasAboveHalfHp = gBattleMons[i].hp > gBattleMons[i].maxHP / 2;
    }

    if (gBattleStruct->incrementEchoedVoice)
    {
        if (gBattleStruct->echoedVoiceCounter < 4)
            gBattleStruct->echoedVoiceCounter++;
        gBattleStruct->incrementEchoedVoice = FALSE;
    }
    else
    {
        gBattleStruct->echoedVoiceCounter = 0;
    }

    return effect;
}

static bool32 HandleEndTurnWeather(enum BattlerId battler)
{
    gBattleStruct->eventState.endTurn++;
    return EndOrContinueWeather();
}

static bool32 HandleEndTurnWeatherDamage(enum BattlerId battler)
{
    bool32 effect = FALSE;

    enum Ability ability = GetBattlerAbility(battler);
    enum BattleWeather currBattleWeather = GetBattleWeather(gBattleWeather);

    if (currBattleWeather == BATTLE_WEATHER_NONE)
    {
        // If there is no weather on the field, no need to check other battlers so go to next state
        gBattleStruct->eventState.endTurnBattler = 0;
        gBattleStruct->eventState.endTurn++;
        return effect;
    }

    gBattleStruct->eventState.endTurnBattler++;

    if (!IsBattlerPresent(battler) || !HasWeatherEffect())
        return effect;


    switch (currBattleWeather)
    {
    case BATTLE_WEATHER_FOG:
    case BATTLE_WEATHER_STRONG_WINDS:
        break;
    case BATTLE_WEATHER_RAIN:
    case BATTLE_WEATHER_RAIN_PRIMAL:
    case BATTLE_WEATHER_RAIN_DOWNPOUR:
        if (ability == ABILITY_DRY_SKIN || ability == ABILITY_RAIN_DISH)
        {
            if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, battler, ability, MOVE_NONE, TRUE))
                effect = TRUE;
        }
        break;
    case BATTLE_WEATHER_SUN:
    case BATTLE_WEATHER_SUN_PRIMAL:
        if (ability == ABILITY_DRY_SKIN || ability == ABILITY_SOLAR_POWER)
        {
            if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, battler, ability, MOVE_NONE, TRUE))
                effect = TRUE;
        }
        break;
    case BATTLE_WEATHER_SANDSTORM:
        if (ability != ABILITY_SAND_VEIL
         && ability != ABILITY_SAND_FORCE
         && ability != ABILITY_SAND_RUSH
         && ability != ABILITY_OVERCOAT
         && !IS_BATTLER_ANY_TYPE(battler, TYPE_ROCK, TYPE_GROUND, TYPE_STEEL)
         && gBattleMons[battler].volatiles.semiInvulnerable != STATE_UNDERGROUND
         && gBattleMons[battler].volatiles.semiInvulnerable != STATE_UNDERWATER
         && GetBattlerHoldEffect(battler) != HOLD_EFFECT_SAFETY_GOGGLES
         && !IsAbilityAndRecord(battler, ability, ABILITY_MAGIC_GUARD))
        {
            SetPassiveDamageAmount(battler, GetNonDynamaxMaxHP(battler) / 16);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SANDSTORM;
            BattleScriptCall(BattleScript_DamagingWeather);
            effect = TRUE;
        }
        break;
    case BATTLE_WEATHER_HAIL:
    case BATTLE_WEATHER_SNOW:
        if (ability == ABILITY_ICE_BODY)
        {
            if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, battler, ability, MOVE_NONE, TRUE))
                effect = TRUE;
        }
        else if (currBattleWeather == BATTLE_WEATHER_HAIL)
        {
            if (ability != ABILITY_SNOW_CLOAK
             && ability != ABILITY_OVERCOAT
             && !IS_BATTLER_OF_TYPE(battler, TYPE_ICE)
             && gBattleMons[battler].volatiles.semiInvulnerable != STATE_UNDERGROUND
             && gBattleMons[battler].volatiles.semiInvulnerable != STATE_UNDERWATER
             && GetBattlerHoldEffect(battler) != HOLD_EFFECT_SAFETY_GOGGLES
             && !IsAbilityAndRecord(battler, ability, ABILITY_MAGIC_GUARD))
            {
                SetPassiveDamageAmount(battler, GetNonDynamaxMaxHP(battler) / 16);
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_HAIL;
                BattleScriptCall(BattleScript_DamagingWeather);
                effect = TRUE;
            }
        }
        break;
    case BATTLE_WEATHER_NONE:
    case BATTLE_WEATHER_COUNT:
        break;
    }

    return effect;
}

static bool32 HandleEndTurnSendOutReplacements(enum BattlerId battler)
{
    gBattleStruct->eventState.endTurnBattler++;

    switch (gSpecialStatuses[battler].queuedSwitch)
    {
    case NO_QUEUED_SWITCH:
        break;
    case QUEUED_SWITCH_SEND_REPLACEMENT:
        gBattleScripting.battler = battler;
        BattleScriptCall(BattleScript_QueuedSwitch);
        return TRUE;
    case QUEUED_SWITCH_OPEN_PARTY_SCREEN:
        gBattleScripting.battler = battler;
        BattleScriptCall(BattleScript_QueuedSwitchOpenPartyScreen);
        return TRUE;
    default:
        errorf("Invalid value - queuedSwitch");
        break;
    }
    return