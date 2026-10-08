#ifndef GUARD_CONSTANTS_BATTLE_H
#define GUARD_CONSTANTS_BATTLE_H

#include "constants/moves.h"

/*
 * A battler may be in one of four positions on the field. The first bit determines
 * what side the battler is on, either the player's side or the opponent's side.
 * The second bit determines what flank the battler is on, either the left or right.
 * Note that the opponent's flanks are drawn corresponding to their perspective, so
 * their right mon appears on the left, and their left mon appears on the right.
 * The battler ID is usually the same as the position, except in the case of link battles.
 *
 *   + ------------------------- +
 *   |           Opponent's side |
 *   |            Right    Left  |
 *   |              3       1    |
 *   |                           |
 *   | Player's side             |
 *   |  Left   Right             |
 *   |   0       2               |
 *   +---------------------------+
 *   |                           |
 *   |                           |
 *   +---------------------------+
 */

/*
 * BattleTrainer is the identifier used to reference one of the four 6-mon battle parties
 * in gParties[MAX_BATTLE_TRAINERS]. gParties[B_TRAINER_PLAYER] is always the player's party.
 * gParties[B_TRAINER_OPPONENT_A] is always the first opponent trainer's party, or holds the first
 * wild mon during an encounter. gParties[B_TRAINER_PARTNER] is only used in multibattles where
 * the player's side has a second trainer such as Mossdeep Space Center tag battle with
 * trainer Steven. gParties[B_TRAINER_OPPONENT_B] is only used in battles with two opponent trainers.
 * In a double battle where the battle side only has a single trainer, both battlers on that battle
 * side will reside in the same party (gParties[B_TRAINER_PLAYER] for player side and
 * gParties[B_TRAINER_OPPONENT_A] for opponent side).
 * Note in link multi battles, parties are set locally on each player's device, meaning
 * even if a player is in the right position, on their device they will still occupy
 * gParties[B_TRAINER_PLAYER], with their link partner using gParties[B_TRAINER_PARTNER].
 *
 *          Regular battles              Link multi (player on left)         Link multi (player on right)
 *   + ------------------------- +      + ------------------------- +       + ------------------------- +
 *   |           Opponent's side |      |           Opponent's side |       |           Opponent's side |
 *   |            Right    Left  |      |            Right    Left  |       |            Right    Left  |
 *   | (1 trainer) opA     opA   |      |             opB     opA   |       |             opB     opA   |
 *   | (2 trainers)opB     opA   |      |                           |       |                           |
 *   |                           |      |                           |       |                           |
 *   | Player's side             |      |                           |       |                           |
 *   |  Left   Right             |      | Player's side             |       | Player's side             |
 *   |  pla     pla(double)      |      |  Left   Right             |       |  Left   Right             |
 *   |  pla     par(multi)       |      |  pla     par              |       |  par     pla              |
 *   +---------------------------+      +---------------------------+       +---------------------------+
 *   |                           |      |                           |       |                           |
 *   |                           |      |                           |       |                           |
 *   +---------------------------+      +---------------------------+       +---------------------------+
 */

enum BattlerPosition
{
    B_POSITION_PLAYER_LEFT,
    B_POSITION_OPPONENT_LEFT,
    B_POSITION_PLAYER_RIGHT,
    B_POSITION_OPPONENT_RIGHT,
    MAX_POSITION_COUNT,
    B_POSITION_ABSENT = 0xFF,
};

enum __attribute__((packed)) BattlerId
{
    B_BATTLER_0,
    B_BATTLER_1,
    B_BATTLER_2,
    B_BATTLER_3,
    MAX_BATTLERS_COUNT,
};

enum __attribute__((packed)) BattleTrainer
{
    B_TRAINER_PLAYER,
    B_TRAINER_OPPONENT_A,
    B_TRAINER_PARTNER,
    B_TRAINER_OPPONENT_B,
    MAX_BATTLE_TRAINERS,
};

enum BattleSide
{
    B_SIDE_PLAYER = 0,
    B_SIDE_OPPONENT = 1,
    NUM_BATTLE_SIDES = 2,
};

#define B_FLANK_LEFT  0
#define B_FLANK_RIGHT 1

#define BIT_SIDE        1
#define BIT_FLANK       2

// Battle Type Flags
#define BATTLE_TYPE_DOUBLE             (1 << 0)
#define BATTLE_TYPE_LINK               (1 << 1)
#define BATTLE_TYPE_IS_MASTER          (1 << 2) // In not-link battles, it's always set.
#define BATTLE_TYPE_TRAINER            (1 << 3)
#define BATTLE_TYPE_FIRST_BATTLE       (1 << 4)
#define BATTLE_TYPE_LINK_IN_BATTLE     (1 << 5) // Set on battle entry, cleared on exit. Checked rarely
#define BATTLE_TYPE_MULTI              (1 << 6)
#define BATTLE_TYPE_SAFARI             (1 << 7)
#define BATTLE_TYPE_BATTLE_TOWER       (1 << 8)
#define BATTLE_TYPE_CATCH_TUTORIAL     (1 << 9)
#define BATTLE_TYPE_ROAMER             (1 << 10)
#define BATTLE_TYPE_EREADER_TRAINER    (1 << 11)
#define BATTLE_TYPE_RAID               (1 << 12)
#define BATTLE_TYPE_LEGENDARY          (1 << 13)
#define BATTLE_TYPE_14                 (1 << 14)
#define BATTLE_TYPE_TWO_OPPONENTS      (1 << 15)
#define BATTLE_TYPE_DOME               (1 << 16)
#define BATTLE_TYPE_PALACE             (1 << 17)
#define BATTLE_TYPE_ARENA              (1 << 18)
#define BATTLE_TYPE_FACTORY            (1 << 19)
#define BATTLE_TYPE_PIKE               (1 << 20)
#define BATTLE_TYPE_PYRAMID            (1 << 21)
#define BATTLE_TYPE_INGAME_PARTNER     (1 << 22)
#define BATTLE_TYPE_TOWER_LINK_MULTI   (1 << 23)
#define BATTLE_TYPE_RECORDED           (1 << 24)
#define BATTLE_TYPE_RECORDED_LINK      (1 << 25)
#define BATTLE_TYPE_TRAINER_HILL       (1 << 26)
#define BATTLE_TYPE_TRAINER_TOWER      BATTLE_TYPE_TRAINER_HILL
#define BATTLE_TYPE_SECRET_BASE        (1 << 27)
#define BATTLE_TYPE_GHOST              (1 << 28)
#define BATTLE_TYPE_POKEDUDE           (1 << 29)
#define BATTLE_TYPE_30                 (1 << 30)
#define BATTLE_TYPE_RECORDED_IS_MASTER (1 << 31)
#define BATTLE_TYPE_FRONTIER                (BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_DOME | BATTLE_TYPE_PALACE | BATTLE_TYPE_ARENA | BATTLE_TYPE_FACTORY | BATTLE_TYPE_PIKE | BATTLE_TYPE_PYRAMID)
#define BATTLE_TYPE_FRONTIER_NO_PYRAMID     (BATTLE_TYPE_BATTLE_TOWER | BATTLE_TYPE_DOME | BATTLE_TYPE_PALACE | BATTLE_TYPE_ARENA | BATTLE_TYPE_FACTORY | BATTLE_TYPE_PIKE)
#define BATTLE_TYPE_RECORDED_INVALID        ((BATTLE_TYPE_LINK | BATTLE_TYPE_SAFARI | BATTLE_TYPE_FIRST_BATTLE                  \
                                             | BATTLE_TYPE_CATCH_TUTORIAL | BATTLE_TYPE_ROAMER | BATTLE_TYPE_EREADER_TRAINER    \
                                             | BATTLE_TYPE_LEGENDARY                                                            \
                                             | BATTLE_TYPE_RECORDED | BATTLE_TYPE_TRAINER_HI