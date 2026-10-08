#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Bleed deals one sixteenth of max HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BLEED); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP - max(1, player->maxHP / 16));
    }
}

SINGLE_BATTLE_TEST("A healing move cures Bleed without restoring HP that turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BLEED); MaxHP(200); HP(100); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Bleed blocks positive stat changes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BLEED); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
