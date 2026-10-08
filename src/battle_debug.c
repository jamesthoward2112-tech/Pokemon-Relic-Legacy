#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "scanline_effect.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_icon.h"
#include "sprite.h"
#include "item.h"
#include "task.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "text.h"
#include "text_window.h"
#include "international_string_util.h"
#include "strings.h"
#include "battle_ai_util.h"
#include "list_menu.h"
#include "decompress.h"
#include "trainer_pokemon_sprites.h"
#include "malloc.h"
#include "string_util.h"
#include "util.h"
#include "data.h"
#include "reset_rtc_screen.h"
#include "reshow_battle_screen.h"
#include "constants/abilities.h"
#include "constants/party_menu.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/rgb.h"

#define MAX_MODIFY_DIGITS 4

struct BattleDebugModifyArrows
{
    u8 arrowSpriteId[2];
    u16 minValue;
    u16 maxValue;
    int currValue;
    u8 currentDigit:4;
    u8 maxDigits:4;
    u8 charDigits[MAX_MODIFY_DIGITS];
    void *modifiedValPtr;
    u8 typeOfVal;
};

struct BattleDebugMenu
{
    enum BattlerId battlerId:3;
    enum BattlerId aiBattlerId:3;

    u8 battlerWindowId;

    u8 mainListWindowId;
    u8 mainListTaskId;
    u8 currentMainListItemId;

    u8 secondaryListWindowId;
    u8 secondaryListTaskId;
    u8 currentSecondaryListItemId;
    u8 secondaryListItemCount;

    u8 modifyWindowId;

    u8 activeWindow;

    struct BattleDebugModifyArrows modifyArrows;
    const struct BitfieldInfo *bitfield;
    bool8 battlerWasChanged[MAX_BATTLERS_COUNT];

    u8 aiViewState;

    u8 aiMonSpriteId;
    u8 aiMovesWindowId;

    union
    {
        u8 aiIconSpriteIds[MAX_BATTLERS_COUNT];
        u8 aiPartyIcons[PARTY_SIZE];
    } spriteIds;
};

struct __attribute__((__packed__)) BitfieldInfo
{
    u8 bitsCount;
    u8 currBit;
};

enum
{
    LIST_ITEM_MOVES,
    LIST_ITEM_ABILITY,
    LIST_ITEM_HELD_ITEM,
    LIST_ITEM_PP,
    LIST_ITEM_TYPES,
    LIST_ITEM_STATS,
    LIST_ITEM_STAT_STAGES,
    LIST_ITEM_STATUS1,
    LIST_ITEM_VOLATILE,
    LIST_ITEM_HAZARDS,
    LIST_ITEM_SIDE_STATUS,
    LIST_ITEM_AI,
    LIST_ITEM_AI_MOVES_PTS,
    LIST_ITEM_AI_INFO,
    LIST_ITEM_AI_PARTY,
    LIST_ITEM_VARIOUS,
    LIST_ITEM_INSTANT_WIN,
    LIST_ITEM_COUNT
};

enum
{
    LIST_STAT_HP_CURRENT,
    LIST_STAT_HP_MAX,
    LIST_STAT_ATTACK,
    LIST_STAT_DEFENSE,
    LIST_STAT_SPEED,
    LIST_STAT_SP_ATK,
    LIST_STAT_SP_DEF,
};

enum
{
    LIST_STATUS1_SLEEP,
    LIST_STATUS1_POISON,
    LIST_STATUS1_BURN,
    LIST_STATUS1_FREEZE,
    LIST_STATUS1_PARALYSIS,
    LIST_STATUS1_TOXIC_POISON,
    LIST_STATUS1_TOXIC_COUNTER,
    LIST_STATUS1_FROSTBITE,
};

enum
{
    LIST_SIDE_STICKY_WEB,
    LIST_SIDE_SPIKES,
    LIST_SIDE_TOXIC_SPIKES,
    LIST_SIDE_STEALTH_ROCK,
    LIST_SIDE_STEELSURGE,
};

enum
{
    LIST_SIDE_REFLECT,
    LIST_SIDE_LIGHTSCREEN,
    LIST_SIDE_SAFEGUARD,
    LIST_SIDE_MIST,
    LIST_SIDE_TAILWIND,
    LIST_SIDE_AURORA_VEIL,
    LIST_SIDE_LUCKY_CHANT,
    LIST_SIDE_DAMAGE_NON_TYPES,
    LIST_SIDE_RAINBOW,
    LIST_SIDE_SEA_OF_FIRE,
    LIST_SIDE_SWAMP,
};

enum
{
    LIST_AI_CHECK_BAD_MOVE,
    LIST_AI_TRY_TO_FAINT,
    LIST_AI_CHECK_VIABILITY,
    LIST_AI_FORCE_SETUP_FIRST_TURN,
    LIST_AI_RISKY,
    LIST_AI_TRY_TO_2HKO,
    LIST_AI_PREFER_BATON_PASS,
    LIST_AI_DOUBLE_BATTLE,
    LIST_AI_HP_AWARE,
    LIST_AI_POWERFUL_STATUS,
    LIST_AI_NEGATE_UNAWARE,
    LIST_AI_WILL_SUICIDE,
    LIST_AI_PREFER_STATUS_MOVES,
    LIST_AI_STALL,
    LIST_AI_SMART_SWITCHING,
    LIST_AI_ACE_POKEMON,
    LIST_AI_OMNISCIENT,
    LIST_AI_SMART_MON_CHOICES,
    LIST_AI_CONSERVATIVE,
    LIST_AI_SEQUENCE_SWITCHING,
    LIST_AI_DOUBLE_ACE_POKEMON,
    LIST_AI_WEIGH_ABILITY_PREDICTION,
    LIST_AI_PREFER_HIGHEST_DAMAGE_MOVE,
    LIST_AI_PREDICT_SWITCH,
    LIST_AI_PREDICT_INCOMING_MON,
    LIST_AI_DYNAMIC_FUNC,
    LIST_AI_ROAMING,
    LIST_AI_SAFARI,
    LIST_AI_FIRST_BATTLE,
};

enum
{
    VARIOUS_SHOW_HP,
    VARIOUS_SUBSTITUTE_HP,
    VARIOUS_IN_LOVE,
};

enum
{
    ACTIVE_WIN_MAIN,
    ACTIVE_WIN_SECONDARY,
    ACTIVE_WIN_MODIFY
};

enum
{
    VAL_U8,
    VAL_U16,
    VAL_U32,
    VAL_BITFIELD_8,
    VAL_BITFIELD_16,
    VAL_BITFIELD_32,
    VAL_VOLATILE,
    VAL_HAZARDS,
    VAR_SIDE_STATUS,
    VAR_SHOW_HP,
    VAR_SUBSTITUTE,
    VAR_IN_LOVE,
    VAR_U16_4_ENTRIES,
    VAL_S8,
    VAL_ALL_STAT_STAGES,
};

// Static Declarations
static const u8 *GetHoldEffectName(enum HoldEffect holdEffect);

// const rom data
static const u8 sText_Ability[] = _("Ability");
static const u8 sText_HeldItem[] = _("Held Item");
static const u8 sText_HoldEffect[] = _("Hold Effect");
static const u8 sText_EmptyString[] = _("");

static const struct BitfieldInfo sStatus1Bitfield[] =
{
    {/*Sleep*/ 3, 0},
    {/*Poison*/ 1, 3},
    {/*Burn*/ 1, 4},
    {/*Freeze*/ 1, 5},
    {/*Paralysis*/1, 6},
    {/*Toxic Poison*/ 1, 7},
    {/*Toxic Counter*/ 4, 8},
    {/*Frostbite*/ 1, 12},
};

static const struct BitfieldInfo sStatus3Bitfield[] =
{
    {/*Leech Seed Battler*/ 2, 0},
    {/*Leech Seed*/ 1, 2},
    {/*Always Hits*/ 2, 3},
    {/*Perish Song*/ 1, 5},
    {/*On Air*/ 1, 6},
    {/*Underground*/ 1, 7},
    {/*Minimized*/ 1, 8},
    {/*Charged Up*/ 1, 9},
    {/*Rooted*/ 1, 10},
    {/*Yawn*/ 2, 11},
    {/*Imprisoned Others*/ 1, 13},
    {/*Grudge*/ 1, 14},
    {/*Gastro Acid*/ 1, 16},
    {/*Embargo*/ 1, 17},
    {/*Underwater*/ 1, 18},
    {/*Smacked Down*/ 1, 21},
    {/*Telekinesis*/ 1, 23},
    {/*Miracle Eyed*/ 1, 25},
    {/*Magnet Rise*/ 1, 26},
    {/*Heal Blocked*/ 1, 27},
    {/*Aqua Ring*/ 1, 28},
    {/*Laser Focus*/ 1, 29},
    {/*Power Trick*/ 1, 30},
};

static const struct BitfieldInfo sAIBitfield[] =
{
    {/*Check Bad Move*/ 1, 0},
    {/*Try to Faint*/ 1, 1},
    {/*Check Viability*/ 1, 2},
    {/*Force Setup First Turn*/ 1, 3},
    {/*Risky*/ 1, 4},
    {/*Prefer Strongest Move*/ 1, 5},
    {/*Prefer Baton Pass*/ 1, 6},
    {/*Double Battle*/ 1, 7},
    {/*HP Aware*/ 1, 8},
    {/*Powerful Status*/ 1, 9},
    {/*Negate Unaware*/ 1, 10},
    {/*Will Suicide*/ 1, 11},
    {/*Prefer Status Moves*/ 1, 12},
    {/*Stall*/ 1, 13},
    {/*Smart Switching*/ 1, 14},
    {/*Ace Pokemon*/ 1, 15},
    {/*Omniscient*/ 1, 16},
    {/*Smart Mon Choices*/ 1, 17},
    {/*Conservative*/ 1, 18},
    {/*Sequence Switching*/ 1, 19},
    {/*Double Ace Pokemon*/ 1, 20},
    {/*Weigh Ability Prediction*/ 1, 21},
    {/*Prefer Highest Damage Move*/ 1, 22},
    {/*Predict Switch*/ 1, 23},
    {/*Predict Incoming Mon*/ 1, 24},
    {/*Dynamic Func*/ 1, 28},
    {/*Roaming*/ 1, 29},
    {/*Safari*/ 1, 30},
    {/*First Battle*/ 1, 31},
};

static const struct ListMenuItem sMainListItems[] =
{
    {COMPOUND_STRING("Moves"),        LIST_ITEM_MOVES},
    {sText_Ability,                   LIST_ITEM_ABILITY},
    {sText_HeldItem,                  LIST_ITEM_HELD_ITEM},
    {COMPOUND_STRING("PP"),           LI