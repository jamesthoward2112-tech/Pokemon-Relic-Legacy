#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rip and Tear lowers the target's Speed after it hits")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(500); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_RIP_AND_TEAR, hit: TRUE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Rip and Tear applies Bleeding when its secondary-effect RNG succeeds")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(500); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_RIP_AND_TEAR, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BLEED);
    }
}

SINGLE_BATTLE_TEST("Rip and Tear's Bleeding chance can fail when its secondary-effect RNG rejects")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(500); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_RIP_AND_TEAR, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, FALSE)); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BLEED));
    }
}

#define RIP_AND_TEAR_BLEED_IMMUNITY_TEST(species) \
SINGLE_BATTLE_TEST("Rip and Tear does not Bleed " #species) \
{ \
    GIVEN { \
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); } \
        OPPONENT(species) { Defense(500); HP(1000); MaxHP(1000); } \
    } WHEN { \
        TURN { MOVE(player, MOVE_RIP_AND_TEAR, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); } \
    } THEN { \
        EXPECT(!(opponent->status1 & STATUS1_BLEED)); \
    } \
}

RIP_AND_TEAR_BLEED_IMMUNITY_TEST(SPECIES_ROGGENROLA)
RIP_AND_TEAR_BLEED_IMMUNITY_TEST(SPECIES_GASTLY)

SINGLE_BATTLE_TEST("Rip and Tear can Bleed a neutral target")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(500); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_RIP_AND_TEAR, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BLEED);
    }
}

SINGLE_BATTLE_TEST("Rip and Tear cannot be used consecutively")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Attack(1); Moves(MOVE_RIP_AND_TEAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(500); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_RIP_AND_TEAR); }
        TURN { FORCED_MOVE(player); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RIP_AND_TEAR, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, player);
    }
}
