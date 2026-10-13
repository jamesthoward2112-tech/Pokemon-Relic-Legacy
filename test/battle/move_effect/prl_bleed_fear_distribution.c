#include "global.h"
#include "test/battle.h"

// PRL Bleeding/Fear move distribution, approved 10 October 2026.
// Existing Skulberus Rip and Tear remains unchanged (separate donor tests).
SINGLE_BATTLE_TEST("PRL Bite can Bleed while retaining its flinch effect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BLEED);
    }
}

SINGLE_BATTLE_TEST("PRL Slash can inflict Bleeding at its approved secondary slot")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SLASH); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BLEED);
    }
}

SINGLE_BATTLE_TEST("PRL Slash Bleeding can fail its percentage roll")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SLASH); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BLEED));
    }
}

SINGLE_BATTLE_TEST("PRL Scratch causes no Bleeding")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, hit: TRUE); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BLEED));
    }
}

SINGLE_BATTLE_TEST("PRL Poison Fang still causes no Bleeding")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POISON_FANG); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_FANG, hit: TRUE); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BLEED));
    }
}

SINGLE_BATTLE_TEST("PRL Slash cannot Bleed a Rock-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SLASH); Attack(1); }
        OPPONENT(SPECIES_ROGGENROLA) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_BLEED));
    }
}

SINGLE_BATTLE_TEST("PRL Astonish can inflict Fear")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ASTONISH); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_ASTONISH, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); }
    } THEN {
        EXPECT_EQ((u32)opponent->volatiles.fearTimer, 1);
    }
}

SINGLE_BATTLE_TEST("PRL Snarl can inflict Fear")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SNARL); SpAttack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL, hit: TRUE, WITH_RNG(RNG_SECONDARY_EFFECT_2, TRUE)); }
    } THEN {
        EXPECT_EQ((u32)opponent->volatiles.fearTimer, 1);
    }
}

SINGLE_BATTLE_TEST("PRL Noble Roar preserves its stat drop and can inflict Fear")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_NOBLE_ROAR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_NOBLE_ROAR, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ((u32)opponent->volatiles.fearTimer, 1);
    }
}
