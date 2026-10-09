#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Scary Face inflicts Fear and lowers Speed by 2 stages")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCARY_FACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CURSED, opponent);
        MESSAGE("The opposing Wobbuffet was gripped by fear!");
    } THEN {
        EXPECT(opponent->volatiles.fearTimer == 1); // The first of Fear's two turns just ended.
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Worry Seed inflicts Fear and gives the target Insomnia")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER) { Ability(ABILITY_BLAZE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WORRY_SEED); }
    } THEN {
        EXPECT_EQ(opponent->ability, ABILITY_INSOMNIA);
        EXPECT(opponent->volatiles.fearTimer == 1);
    }
}

SINGLE_BATTLE_TEST("Simple Beam does not inflict Fear")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SIMPLE_BEAM); }
        OPPONENT(SPECIES_CHARMANDER) { Ability(ABILITY_BLAZE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SIMPLE_BEAM); }
    } THEN {
        EXPECT_EQ(opponent->ability, ABILITY_SIMPLE);
        EXPECT(opponent->volatiles.fearTimer == 0);
    }
}

SINGLE_BATTLE_TEST("Fear increases damage to the opposing Pokémon by 50 percent", s16 damage)
{
    bool32 fear;
    PARAMETRIZE { fear = FALSE; }
    PARAMETRIZE { fear = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT, MOVE_CELEBRATE, MOVE_SCARY_FACE); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        if (fear)
            TURN { MOVE(player, MOVE_SCARY_FACE); MOVE(opponent, MOVE_CELEBRATE); }
        else
            TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Fear increases damage to the player by 50 percent")
{
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT, MOVE_WORRY_SEED); Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WORRY_SEED); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SWIFT); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SWIFT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage[0]);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], UQ_4_12(1.5), damage[0]);
    }
}

SINGLE_BATTLE_TEST("Fear timer expires after its second counted turn")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Moves(MOVE_SCARY_FACE, MOVE_TACKLE); Speed(2); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_TACKLE); Speed(1); }
    } WHEN {
        gBattleStruct->battlerState[1].commanderSpecies = SPECIES_NONE;
        TURN { MOVE(player, MOVE_SCARY_FACE); MOVE(opponent, MOVE_TACKLE); }
        gBattleStruct->battlerState[1].commanderSpecies = SPECIES_NONE;
        EXPECT(!CanBattlerEscape(1));
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
        EXPECT(gBattleMons[1].volatiles.fearTimer == 0);
    }
}

SINGLE_BATTLE_TEST("Fear can be removed by a forced switch")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCARY_FACE, MOVE_ROAR); Speed(2); }
        PLAYER(SPECIES_WYNAUT) { Speed(2); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_CELEBRATE); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        gBattleMons[1].volatiles.fearTimer = 2;
        TURN { MOVE(player, MOVE_ROAR); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT(opponent->volatiles.fearTimer == 0);
    }
}

SINGLE_BATTLE_TEST("Fear does not stop a Ghost-type Pokémon from switching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCARY_FACE, MOVE_CELEBRATE); Speed(2); }
        OPPONENT(SPECIES_GASTLY) { Moves(MOVE_CELEBRATE); Speed(1); }
        OPPONENT(SPECIES_HAUNTER) { Speed(1); }
    } WHEN {
        TURN { gBattleMons[1].volatiles.fearTimer = 2; MOVE(opponent, MOVE_CELEBRATE); }
        EXPECT(opponent->volatiles.fearTimer > 0);
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_HAUNTER);
    }
}
