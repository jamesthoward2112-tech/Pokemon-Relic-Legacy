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
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

WILD_BATTLE_TEST("Skulberus Shadow Tag innate prevents escape")
{
    GIVEN {
        PLAYER(SPECIES_SKULBERUS) { Ability(ABILITY_MOXIE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(CanBattlerEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), FALSE);
    }
}
