#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SEISMIC_SLAM) == EFFECT_RECOIL);
    ASSUME(GetMoveRecoil(MOVE_SEISMIC_SLAM) == 33);
    ASSUME(GetMoveEffect(MOVE_BRAVADO) == EFFECT_FACADE);
    ASSUME(GetMoveEffect(MOVE_TAKE_FLIGHT) == EFFECT_HIT_ESCAPE);
    ASSUME(IsMultiHitMove(MOVE_BEATDOWN));
}

SINGLE_BATTLE_TEST("Redux Seismic Slam damages target and deals 33 percent recoil")
{
    s16 damage, recoil;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SEISMIC_SLAM); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_SLAM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &recoil);
    } THEN {
        EXPECT_EQ(recoil, damage * 33 / 100);
    }
}

SINGLE_BATTLE_TEST("Redux Soil Drain heals its user after dealing Ground damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SOIL_DRAIN); HP(50); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOIL_DRAIN); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_GT(player->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Redux Bravado is Special and gains the status power boost", s16 damage)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BRAVADO); Status1(status); SpAttack(130); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(120); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAVADO, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(GetMoveCategory(MOVE_BRAVADO), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Redux Raging Souls drops its user's Special Attack by two")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAGING_SOULS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_SOULS); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Redux Iron Fangs breaks an opposing Reflect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_IRON_FANGS, MOVE_CELEBRATE); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_REFLECT, MOVE_CELEBRATE); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_REFLECT); }
        TURN { MOVE(player, MOVE_IRON_FANGS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
    }
}

SINGLE_BATTLE_TEST("Redux Beatdown strikes five times with Skill Link")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BEATDOWN); Ability(ABILITY_SKILL_LINK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(600); MaxHP(600); Defense(180); }
    } WHEN {
        TURN { MOVE(player, MOVE_BEATDOWN); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        HP_BAR(opponent);
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Redux Take Flight deals damage and switches the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TAKE_FLIGHT); }
        PLAYER(SPECIES_PIDGEY);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_FLIGHT); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        SEND_IN_MESSAGE("Pidgey");
    }
}

SINGLE_BATTLE_TEST("Redux Insect Impact can lower target Defense")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_INSECT_IMPACT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_INSECT_IMPACT, WITH_RNG(RNG_SECONDARY_EFFECT, 0)); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Redux Seismic Slam recoil is blocked by Rock Head")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SEISMIC_SLAM); Ability(ABILITY_ROCK_HEAD); HP(300); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_SLAM); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
    }
}
