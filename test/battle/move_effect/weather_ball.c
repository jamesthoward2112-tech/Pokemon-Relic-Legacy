#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_WEATHER_BALL) == EFFECT_WEATHER_BALL);
    ASSUME(GetMoveEffect(MOVE_SUNNY_DAY) == EFFECT_WEATHER);
    ASSUME(GetMoveWeatherType(MOVE_SUNNY_DAY) == BATTLE_WEATHER_SUN);
    ASSUME(GetMoveEffect(MOVE_RAIN_DANCE) == EFFECT_WEATHER);
    ASSUME(GetMoveWeatherType(MOVE_RAIN_DANCE) == BATTLE_WEATHER_RAIN);
    ASSUME(GetMoveEffect(MOVE_SANDSTORM) == EFFECT_WEATHER);
    ASSUME(GetMoveWeatherType(MOVE_SANDSTORM) == BATTLE_WEATHER_SANDSTORM);
    ASSUME(GetMoveEffect(MOVE_HAIL) == EFFECT_WEATHER);
    ASSUME(GetMoveWeatherType(MOVE_HAIL) == BATTLE_WEATHER_HAIL);
    ASSUME(GetMoveEffect(MOVE_SNOWSCAPE) == EFFECT_WEATHER);
    ASSUME(GetMoveWeatherType(MOVE_SNOWSCAPE) == BATTLE_WEATHER_SNOW);
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Fire-type move in Sunlight", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SUNNY_DAY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MEGANIUM);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + type effectiveness + sun 50% boost
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Water-type move in Rain", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCANINE);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + type effectiveness + rain 50% boost
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to a Rock-type move in a Sandstorm", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGMAR) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage); // double base power + type effectiveness.
    }
}

SINGLE_BATTLE_TEST("Weather Ball doubles its power and turns to an Ice-type move in Hail and Snow", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_HAIL; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DRAGONAIR) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage); // double base power + type effectiveness.
    }
}

SINGLE_BATTLE_TEST("Weather Ball becomes Ghost-type and doubles its power in Eerie Fog", s16 damage)
{
    enum Move setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_EERIE_FOG; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(setupMove, MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_MACHOP);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetDynamicMoveType(NULL, MOVE_WEATHER_BALL, player, GetBattlerAbility(player), GetBattlerHoldEffect(player), MON_IN_BATTLE),
                  setupMove == MOVE_EERIE_FOG ? TYPE_GHOST : TYPE_NORMAL);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Ball remains Normal-type when Cloud Nine suppresses Eerie Fog")
{
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Ability(ABILITY_CLOUD_NINE); Moves(MOVE_EERIE_FOG, MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_GENGAR);
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_FOG); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_FOG, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_FOG);
        EXPECT_EQ(GetDynamicMoveType(NULL, MOVE_WEATHER_BALL, player, GetBattlerAbility(player), GetBattlerHoldEffect(player), MON_IN_BATTLE), TYPE_NORMAL);
    }
}

DOUBLE_BATTLE_TEST("Weather Ball doesn't double its power and stays a Normal-type move in strong winds", s16 damage)
{
    bool32 strongWinds;
    enum Species species;
    PARAMETRIZE { strongWinds = FALSE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { strongWinds = TRUE;  species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { strongWinds = TRUE;  species = SPECIES_GASTLY; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_WEATHER_BALL) == TYPE_NORMAL);
        ASSUME(GetSpeciesType(SPECIES_GASTLY, 0) == TYPE_GHOST);
        if (strongWinds)
            PLAYER(SPECIES_RAYQUAZA) { Moves(MOVE_DRAGON_ASCENT, MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_RAYQUAZA) { Ability(ABILITY_AIR_LOCK); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (strongWinds)
            TURN { MOVE(playerLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { MOVE(playerRight, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        if (strongWinds)
            ABILITY_POPUP(playerLeft, ABILITY_DELTA_STREAM);
        if (species == SPECIES_GASTLY) {
            NONE_OF {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, playerRight);
                HP_BAR(opponentLeft);
            }
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, playerRight);
            HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Ball doesn't double its power in Sunlight or Rain if Cloud Nine/Air Lock is on the field", s16 damage)
{
    enum Move setupMove;
    enum Species species;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; setupMove = MOVE_RAIN_DANCE; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK;   setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK;   setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK;   setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        PLAYER(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[3].damage, results[4].damage);
        EXPECT_EQ(results[3].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Ball doesn't change type in Sunlight or Rain if Cloud Nine/Air Lock is on the field")
{
    enum Move setupMove;
    enum Species species;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; setupMove = MOVE_RAIN_DANCE; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK;   setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK;   setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_WEATHER_BALL) == TYPE_NORMAL);
        ASSUME(GetSpeciesType(SPECIES_GASTLY, 0) == TYPE_GHOST);
        PLAYER(species) { Ability(ability); }
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Weather Ball does not double its power in Sunlight or Rain if user holds Utility Umbrella", s16 damage)
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_UTILITY_UMBRELLA); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Weather Ball doesn't change type in Sunlight or Rain if user holds Utility Umbrella")
{
    enum Move setupMove;

    PARAMETRIZE { setupMove = MOVE_SUNNY_DAY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
        ASSUME(GetMoveType(MOVE_WEATHER_BALL) == TYPE_NORMAL);
        ASSUME(GetSpeciesType(SPECIES_GASTLY, 0) == TYPE_GHOST);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_UTILITY_UMBRELLA); }
        OPPONENT(SPECIES_GASTLY);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
            HP_BAR(opponent);
        }
    }
}
