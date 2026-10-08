#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Stygian Rush gives Skulberus Dark moves priority at full HP")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_STYGIAN_RUSH); Speed(1); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

SINGLE_BATTLE_TEST("Skulberus Guard Dog innate blocks Attack drops")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Skulberus Shadow Tag innate prevents the opposing Pokemon from escaping")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Speed(100); }
        OPPONENT(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        enum BattlerId trapper = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        EXPECT_EQ(IsAbilityPreventingEscape(battler), trapper + 1);
    }
}

SINGLE_BATTLE_TEST("Skulberus Nocturnal boosts Dark damage independently of its selected ability", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DARK_PULSE; }
    PARAMETRIZE { move = MOVE_SHADOW_BALL; }
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Dark Pulse also gets Skulberus's Dark-type STAB, while Shadow Ball does not.
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.875), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Skulberus Strong Jaw boosts its custom biting moves", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MOXIE; }
    PARAMETRIZE { ability = ABILITY_STRONG_JAW; }
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ability); Moves(MOVE_LOVELY_BITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LOVELY_BITE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Skulberus Moxie raises Attack after a knockout")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_ABRA);
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Lovely Bite can infatuate an opposing Pokemon")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_STRONG_JAW); Gender(MON_MALE); Moves(MOVE_LOVELY_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_FEMALE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LOVELY_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}
