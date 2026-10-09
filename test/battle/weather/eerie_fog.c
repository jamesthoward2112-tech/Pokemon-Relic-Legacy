#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveWeatherType(MOVE_EERIE_FOG) == BATTLE_WEATHER_FOG);
    ASSUME(GetMoveAccuracy(MOVE_THUNDER) == 70);
}

SINGLE_BATTLE_TEST("Eerie Fog starts with the existing fog message and weather animation")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_FOG, player);
        MESSAGE("Fog crept up as thick as soup!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_FOG);
        EXPECT_EQ(gBattleStruct->weatherDuration, 7);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog has an eight-turn duration")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_EERIE_FOG, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_FOG);
        EXPECT_EQ(gBattleStruct->weatherDuration, 1);
    }
}

SINGLE_BATTLE_TEST("Smoke Ball extends Eerie Fog to twelve turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_SMOKE_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_FOG);
        EXPECT_EQ(gBattleStruct->weatherDuration, 11);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog expires after its eighth turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_EERIE_FOG, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The fog lifted.");
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_FOG));
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog replaces existing weather")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM, MOVE_EERIE_FOG); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SANDSTORM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_FOG);
        EXPECT(!(gBattleWeather & B_WEATHER_SANDSTORM));
    }
}

SINGLE_BATTLE_TEST("Eerie Fog lowers each positive stat stage once and preserves neutral and negative stages")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        gBattleMons[B_BATTLER_0].statStages[STAT_ATK] = DEFAULT_STAT_STAGE + 2;
        gBattleMons[B_BATTLER_0].statStages[STAT_DEF] = DEFAULT_STAT_STAGE + 1;
        gBattleMons[B_BATTLER_0].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE - 1;
        gBattleMons[B_BATTLER_1].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE + 1;
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Ghost and Psychic Pokémon are exempt from Eerie Fog stat reduction")
{
    u32 species;
    PARAMETRIZE { species = SPECIES_GENGAR; }
    PARAMETRIZE { species = SPECIES_ABRA; }
    GIVEN {
        PLAYER(species);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        gBattleMons[B_BATTLER_0].statStages[STAT_ATK] = DEFAULT_STAT_STAGE + 1;
        gBattleMons[B_BATTLER_1].statStages[STAT_ATK] = DEFAULT_STAT_STAGE + 1;
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog reduces incoming damage by twenty percent for Ghost and Psychic types")
{
    u32 species;
    enum Move move;
    s16 damage[2];
    PARAMETRIZE { species = SPECIES_GENGAR; move = MOVE_EMBER; }
    PARAMETRIZE { species = SPECIES_GENGAR; move = MOVE_FLAME_WHEEL; }
    PARAMETRIZE { species = SPECIES_ABRA; move = MOVE_EMBER; }
    PARAMETRIZE { species = SPECIES_ABRA; move = MOVE_FLAME_WHEEL; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move, MOVE_EERIE_FOG); Speed(2); }
        OPPONENT(species) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage[0]);
        MESSAGE("The fog is deep…");
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[0], UQ_4_12(0.8), damage[1]);
    }
}

SINGLE_BATTLE_TEST("Cloud Nine suppresses Eerie Fog's stat effect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GOLDUCK) { Ability(ABILITY_CLOUD_NINE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        gBattleMons[B_BATTLER_0].statStages[STAT_ATK] = DEFAULT_STAT_STAGE + 1;
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT(gBattleWeather & B_WEATHER_FOG);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog does not reduce move accuracy")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER, MOVE_EERIE_FOG); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER, WITH_RNG(RNG_ACCURACY, 65)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Eerie Fog halves weather-based recovery for all three recovery moves")
{
    enum Move move;
    enum Ability suppressingAbility;
    s16 expectedHeal;

    PARAMETRIZE { move = MOVE_SYNTHESIS;   suppressingAbility = ABILITY_NONE;        expectedHeal = 100; }
    PARAMETRIZE { move = MOVE_MOONLIGHT;   suppressingAbility = ABILITY_NONE;        expectedHeal = 100; }
    PARAMETRIZE { move = MOVE_MORNING_SUN; suppressingAbility = ABILITY_NONE;        expectedHeal = 100; }
    PARAMETRIZE { move = MOVE_SYNTHESIS;   suppressingAbility = ABILITY_CLOUD_NINE;  expectedHeal = 200; }
    PARAMETRIZE { move = MOVE_MOONLIGHT;   suppressingAbility = ABILITY_CLOUD_NINE;  expectedHeal = 200; }
    PARAMETRIZE { move = MOVE_MORNING_SUN; suppressingAbility = ABILITY_CLOUD_NINE;  expectedHeal = 200; }
    GIVEN {
        WITH_CONFIG(B_TIME_OF_DAY_HEALING_MOVES, GEN_3);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Moves(MOVE_EERIE_FOG, move); }
        if (suppressingAbility == ABILITY_CLOUD_NINE)
            OPPONENT(SPECIES_GOLDUCK) { Ability(suppressingAbility); Moves(MOVE_CELEBRATE); }
        else
            OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_FOG, player);
        HP_BAR(player, damage: -expectedHeal);
    }
}

SINGLE_BATTLE_TEST("Ordinary Curse and Weather Ball behavior returns after Eerie Fog expires")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_EERIE_FOG, MOVE_CURSE, MOVE_WEATHER_BALL, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GENGAR) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); }
        gBattleStruct->weatherDuration = 1;
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CURSE); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
            HP_BAR(opponent);
        }
    } THEN {
        enum BattlerId playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(GetBattlerMoveTargetType(playerBattler, MOVE_CURSE), TARGET_USER);
        EXPECT_EQ(GetDynamicMoveType(NULL, MOVE_WEATHER_BALL, playerBattler, GetBattlerAbility(playerBattler), GetBattlerHoldEffect(playerBattler), MON_IN_BATTLE), TYPE_NORMAL);
        EXPECT(!opponent->volatiles.cursed);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
