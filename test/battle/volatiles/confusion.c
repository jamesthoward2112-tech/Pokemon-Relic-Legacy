#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Confused Pokémon execute physical moves and take 33 percent of actual damage as recoil")
{
    s16 damage, recoil;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        gBattleMons[B_BATTLER_0].volatiles.confusionTimer = 2;
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_CONFUSION, 0xFFFF)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, damage * 33 / 100);
    }
}

SINGLE_BATTLE_TEST("Confused opponents execute special moves and take recoil")
{
    s16 damage, recoil;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); Speed(2); }
    } WHEN {
        gBattleMons[B_BATTLER_1].volatiles.confusionTimer = 2;
        TURN { MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_CONFUSION, 0xFFFF)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player, captureDamage: &damage);
        HP_BAR(opponent, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, damage * 33 / 100);
    }
}

SINGLE_BATTLE_TEST("Confusion recoil uses the combined damage from all hits")
{
    s16 damage, recoil;
    GIVEN {
        ASSUME(GetMoveStrikeCount(MOVE_DOUBLE_HIT) == 2);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_HIT); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        gBattleMons[B_BATTLER_0].volatiles.confusionTimer = 2;
        TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_CONFUSION, 0xFFFF)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, damage * 33 / 100);
    }
}

DOUBLE_BATTLE_TEST("Confusion recoil sums the damage dealt by a spread move")
{
    s16 allyDamage, leftDamage, rightDamage, recoil;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SURF); Speed(2); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        gBattleMons[B_BATTLER_0].volatiles.confusionTimer = 2;
        TURN {
            MOVE(playerLeft, MOVE_SURF, WITH_RNG(RNG_CONFUSION, 0xFFFF));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &allyDamage);
        HP_BAR(opponentLeft, captureDamage: &leftDamage);
        HP_BAR(opponentRight, captureDamage: &rightDamage);
        HP_BAR(playerLeft, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, (allyDamage + leftDamage + rightDamage) * 33 / 100);
    }
}

SINGLE_BATTLE_TEST("A confused Pokémon does not take damage-based recoil for a status move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE); HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        gBattleMons[B_BATTLER_0].volatiles.confusionTimer = 2;
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}
