Warning: truncated output (original token count: 111761)
Total output lines: 12129

#include "global.h"
#include "malloc.h"
#include "battle_anim.h"
#include "battle_pyramid.h"
#include "battle_util.h"
#include "berry.h"
#include "data.h"
#include "debug.h"
#include "decoration.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "faraway_island.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "follower_npc.h"
#include "follower_helper.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "mauville_old_man.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokeball.h"
#include "random.h"
#include "region_map.h"
#include "rtc.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "trainer_see.h"
#include "trainer_hill.h"
#include "util.h"
#include "wild_encounter.h"
#include "wild_encounter_ow.h"
#include "constants/event_object_movement.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/items.h"
#include "constants/mauville_old_man.h"
#include "constants/metatile_behaviors.h"
#include "constants/rgb.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/metatile_behaviors.h"
#include "constants/trainer_types.h"
#include "constants/union_room.h"
#include "constants/weather.h"

#define SPECIAL_LOCALIDS_START (min(LOCALID_CAMERA, \
                                min(LOCALID_PLAYER, \
                                min(LOCALID_BERRY_BLENDER_PLAYER_END - MAX_RFU_PLAYERS + 1, \
                                    LOCALID_OW_ENCOUNTER_END - OWE_SPAWNS_MAX + 1))))

// The object event templates on a map cannot use the special IDs listed above or they can behave unexpectedly.
// For more details on these special IDs see their definitions in 'include/constants/event_objects.h'.
// OBJECT_EVENT_TEMPLATES_COUNT should always be low enough that it doesn't overlap with these IDs.
#if OBJECT_EVENT_TEMPLATES_COUNT >= SPECIAL_LOCALIDS_START
#error "OBJECT_EVENT_TEMPLATES_COUNT is too large. Object event local IDs may overlap with reserved IDs."
#endif

// this file was known as evobjmv.c in Game Freak's original source

enum {
    MOVE_SPEED_NORMAL, // walking
    MOVE_SPEED_FAST_1, // running / surfing / sliding (ice tile)
    MOVE_SPEED_FAST_2, // water current / acro bike
    MOVE_SPEED_FASTER, // mach bike's max speed
    MOVE_SPEED_FASTEST,
};

enum {
    JUMP_DISTANCE_IN_PLACE,
    JUMP_DISTANCE_NORMAL,
    JUMP_DISTANCE_FAR,
};

// Used for storing conditional emotes
struct SpecialEmote
{
    u16 index;
    u8 emotion;
};

// Sprite data used throughout
#define sObjEventId   data[0]
#define sTypeFuncId   data[1] // Index into corresponding gMovementTypeFuncs_* table
#define sActionFuncId data[2] // Index into corresponding gMovementActionFuncs_* table
#define sDirection    data[3]


#define movement_type_def(setup, table) \
static u8 setup##_callback(struct ObjectEvent *, struct Sprite *);\
void setup(struct Sprite *sprite)\
{\
    UpdateObjectEventCurrentMovement(&gObjectEvents[sprite->sObjEventId], sprite, setup##_callback);\
}\
static u8 setup##_callback(struct ObjectEvent *objectEvent, struct Sprite *sprite)\
{\
    return table[sprite->sTypeFuncId](objectEvent, sprite);\
}

#define movement_type_empty_callback(setup) \
static u8 setup##_callback(struct ObjectEvent *, struct Sprite *);\
void setup(struct Sprite *sprite)\
{\
    UpdateObjectEventCurrentMovement(&gObjectEvents[sprite->sObjEventId], sprite, setup##_callback);\
}\
static u8 setup##_callback(struct ObjectEvent *objectEvent, struct Sprite *sprite)\
{\
    return 0;\
}

static EWRAM_DATA u8 sCurrentReflectionType = 0;
static EWRAM_DATA u16 sCurrentSpecialObjectPaletteTag = 0;
static EWRAM_DATA struct LockedAnimObjectEvents *sLockedAnimObjectEvents = {0};

static void MoveCoordsInDirection(u32, s16 *, s16 *, s16, s16);
static bool8 ObjectEventExecSingleMovementAction(struct ObjectEvent *, struct Sprite *);
static bool32 UpdateMonMoveInPlace(struct ObjectEvent *, struct Sprite *);
static void SetMovementDelay(struct Sprite *, s16);
static bool8 WaitForMovementDelay(struct Sprite *);
static enum Direction GetCopyDirection(u8, enum Direction, enum Direction);
static void TryEnableObjectEventAnim(struct ObjectEvent *, struct Sprite *);
static void ObjectEventExecHeldMovementAction(struct ObjectEvent *, struct Sprite *);
static void UpdateObjectEventSpriteAnimPause(struct ObjectEvent *, struct Sprite *);
static bool8 IsCoordOutsideObjectEventMovementRange(struct ObjectEvent *, s16, s16);
static bool8 DoesObjectCollideWithObjectAt(struct ObjectEvent *, s16, s16);
static void UpdateObjectEventOffscreen(struct ObjectEvent *, struct Sprite *);
static void UpdateObjectEventSpriteVisibility(struct ObjectEvent *, struct Sprite *);
static void ObjectEventUpdateMetatileBehaviors(struct ObjectEvent *);
static void GetGroundEffectFlags_Reflection(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_TallGrassOnSpawn(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_LongGrassOnSpawn(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_SandHeap(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_ShallowFlowingWater(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_ShortGrass(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_HotSprings(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_TallGrassOnBeginStep(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_LongGrassOnBeginStep(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_Tracks(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_Puddle(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_Ripple(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_Seaweed(struct ObjectEvent *, u32 *);
static void GetGroundEffectFlags_JumpLanding(struct ObjectEvent *, u32 *);
static u8 ObjectEventGetNearbyReflectionType(struct ObjectEvent *);
static u8 GetReflectionTypeByMetatileBehavior(u32);
static void InitObjectPriorityByElevation(struct Sprite *, u8);
static void ObjectEventUpdateSubpriority(struct ObjectEvent *, struct Sprite *);
static void DoTracksGroundEffect_None(struct ObjectEvent *, struct Sprite *, u8);
static void DoTracksGroundEffect_Footprints(struct ObjectEvent *, struct Sprite *, u8);
static void DoTracksGroundEffect_FootprintsB(struct ObjectEvent*, struct Sprite*, u8);
static void DoTracksGroundEffect_FootprintsC(struct ObjectEvent*, struct Sprite*, u8);
static void DoTracksGroundEffect_BikeTireTracks(struct ObjectEvent *, struct Sprite *, u8);
static void DoTracksGroundEffect_SlitherTracks(struct ObjectEvent*, struct Sprite*, u8);
static void DoRippleFieldEffect(struct ObjectEvent *, struct Sprite *);
static void DoGroundEffects_OnSpawn(struct ObjectEvent *, struct Sprite *);
static void DoGroundEffects_OnBeginStep(struct ObjectEvent *, struct Sprite *);
static void DoGroundEffects_OnFinishStep(struct ObjectEvent *, struct Sprite *);
static void VirtualObject_UpdateAnim(struct Sprite *);
static void ApplyLevitateMovement(u8);
static bool8 MovementType_Disguise_Callback(struct ObjectEvent *, struct Sprite *);
static bool8 MovementType_Buried_Callback(struct ObjectEvent *, struct Sprite *);
static void CreateReflectionEffectSprites(void);
static u8 GetObjectEventIdByLocalIdAndMapInternal(u8, u8, u8);
static u32 GetAvailableObjectEventId(u16, u8, u8);
static void SetObjectEventDynamicGraphicsId(struct ObjectEvent *);
static void RemoveObjectEventInternal(struct ObjectEvent *);
static u16 GetObjectEventFlagIdByObjectEventId(u8);
static void UpdateObjectEventVisibility(struct ObjectEvent *, struct Sprite *);
static void MakeSpriteTemplateFromObjectEventTemplate(const struct ObjectEventTemplate *, struct SpriteTemplate *, const struct SubspriteTable **);
static void GetObjectEventMovingCameraOffset(s16 *, s16 *);
const struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup);
static void RemoveObjectEventIfOutsideView(struct ObjectEvent *);
static void SpawnObjectEventOnReturnToField(u8, s16, s16);
static void SetPlayerAvatarObjectEventIdAndObjectId(u8, u8);
static u8 UpdateSpritePalette(const struct SpritePalette *spritePalette, struct Sprite *sprite);
static void ResetObjectEventFldEffData(struct ObjectEvent *);
static u8 LoadSpritePaletteIfTagExists(const struct SpritePalette *);
static u8 FindObjectEventPaletteIndexByTag(u16);
static bool8 ObjectEventDoesElevationMatch(struct ObjectEvent *, u8);
static void SpriteCB_CameraObject(struct Sprite *);
static void CameraObject_Init(struct Sprite *);
static void CameraObject_UpdateMove(struct Sprite *);
static void CameraObject_UpdateFrozen(struct Sprite *);
static void ObjectEventSetSingleMovement(struct ObjectEvent *, struct Sprite *, u8);
static void SetSpriteDataForNormalStep(struct Sprite *, enum Direction, u8);
static void InitSpriteForFigure8Anim(struct Sprite *);
static bool8 AnimateSpriteInFigure8(struct Sprite *);
enum Direction GetDirectionToFace(s16 x1, s16 y1, s16 x2, s16 y2);
static u32 LoadDynamicFollowerPalette(enum Species species, bool32 shiny, bool32 female);
static void FollowerSetGraphics(struct ObjectEvent *objEvent, enum Species species, bool32 shiny, bool32 female);
static void ObjectEventSetGraphics(struct ObjectEvent *, const struct ObjectEventGraphicsInfo *);
static void SpriteCB_VirtualObject(struct Sprite *);
static void DoShadowFieldEffect(struct ObjectEvent *);
static void SetJumpSpriteData(struct Sprite *, enum Direction, u8, u8);
static void SetWalkSlowSpriteData(struct Sprite *, enum Direction);
static bool8 UpdateWalkSlowAnim(struct Sprite *);
static bool8 UpdateWalkSlowStairs(struct ObjectEvent *objectEvent, struct Sprite *sprite);
static u8 DoJumpSpriteMovement(struct Sprite *);
static u8 DoJumpSpecialSpriteMovement(struct Sprite *);
static void CreateLevitateMovementTask(struct ObjectEvent *);
static void DestroyLevitateMovementTask(u8);
const struct ObjectEventGraphicsInfo *SpeciesToGraphicsInfo(enum Species species, bool32 shiny, bool32 female);
static bool8 NpcTakeStep(struct Sprite *);
static void CopyObjectGraphicsInfoToSpriteTemplate_WithMovementType(u16 graphicsId, u16 movementType, struct SpriteTemplate *spriteTemplate, const struct SubspriteTable **subspriteTables);

static enum Species GetUnownSpecies(struct Pokemon *mon);

static void StartSlowRunningAnim(struct ObjectEvent *objectEvent, struct Sprite *sprite, enum Direction direction);

const u8 gReflectionEffectPaletteMap[16] = {
        [PALSLOT_PLAYER]                 = PALSLOT_PLAYER_REFLECTION,
        [PALSLOT_PLAYER_REFLECTION]      = PALSLOT_PLAYER_REFLECTION,
        [PALSLOT_NPC_1]                  = PALSLOT_NPC_1_REFLECTION,
        [PALSLOT_NPC_2]                  = PALSLOT_NPC_2_REFLECTION,
        [PALSLOT_NPC_3]                  = PALSLOT_NPC_3_REFLECTION,
        [PALSLOT_NPC_4]                  = PALSLOT_NPC_4_REFLECTION,
        [PALSLOT_NPC_1_REFLECTION]       = PALSLOT_NPC_1_REFLECTION,
        [PALSLOT_NPC_2_REFLECTION]       = PALSLOT_NPC_2_REFLECTION,
        [PALSLOT_NPC_3_REFLECTION]       = PALSLOT_NPC_3_REFLECTION,
        [PALSLOT_NPC_4_REFLECTION]       = PALSLOT_NPC_4_REFLECTION,
        [PALSLOT_NPC_SPECIAL]            = PALSLOT_NPC_SPECIAL_REFLECTION,
        [PALSLOT_NPC_SPECIAL_REFLECTION] = PALSLOT_NPC_SPECIAL_REFLECTION
};

static const struct SpriteTemplate sCameraSpriteTemplate = {
    .tileTag = 0,
    .paletteTag = TAG_NONE,
    .oam = &gDummyOamData,
    .callback = SpriteCB_CameraObject
};

enum {
    CAMERA_STATE_INIT,
    CAMERA_STATE_MOVE,
    CAMERA_STATE_FROZEN,
};

static void (*const sCameraObjectFuncs[])(struct Sprite *) = {
    [CAMERA_STATE_INIT]   = CameraObject_Init,
    [CAMERA_STATE_MOVE]   = CameraObject_UpdateMove,
    [CAMERA_STATE_FROZEN] = CameraObject_UpdateFrozen,
};

#include "data/object_events/object_event_graphics.h"

// movement type callbacks
static void (*const sMovementTypeCallbacks[])(struct Sprite *) =
{
    [MOVEMENT_TYPE_NONE] = MovementType_None,
    [MOVEMENT_TYPE_LOOK_AROUND] = MovementType_LookAround,
    [MOVEMENT_TYPE_WANDER_AROUND] = MovementType_WanderAround,
    [MOVEMENT_TYPE_WANDER_AROUND_SLOWER] = MovementType_WanderAroundSlower,
    [MOVEMENT_TYPE_WANDER_UP_AND_DOWN] = MovementType_WanderUpAndDown,
    [MOVEMENT_TYPE_WANDER_DOWN_AND_UP] = MovementType_WanderUpAndDown,
    [MOVEMENT_TYPE_WANDER_LEFT_AND_RIGHT] = MovementType_WanderLeftAndRight,
    [MOVEMENT_TYPE_WANDER_RIGHT_AND_LEFT] = MovementType_WanderLeftAndRight,
    [MOVEMENT_TYPE_FACE_UP] = MovementType_FaceDirection,
    [MOVEMENT_TYPE_FACE_DOWN] = MovementType_FaceDirection,
    [MOVEMENT_TYPE_FACE_LEFT] = MovementType_FaceDirection,
    [MOVEMENT_TYPE_FACE_RIGHT] = MovementType_FaceDirection,
    [MOVEMENT_TYPE_PLAYER] = MovementType_Player,
    [MOVEMENT_TYPE_BERRY_TREE_GROWTH] = MovementType_BerryTreeGrowth,
    [MOVEMENT_TYPE_FACE_DOWN_AND_UP] = MovementType_FaceDownAndUp,
    [MOVEMENT_TYPE_FACE_LEFT_AND_RIGHT] = MovementType_FaceLeftAndRight,
    [MOVEMENT_TYPE_FACE_UP_AND_LEFT] = MovementType_FaceUpAndLeft,
    [MOVEMENT_TYPE_FACE_UP_AND_RIGHT] = MovementType_FaceUpAndRight,
    [MOVEMENT_TYPE_FACE_DOWN_AND_LEFT] = MovementType_FaceDownAndLeft,
    [MOVEMENT_TYPE_FACE_DOWN_AND_RIGHT] = MovementType_FaceDownAndRight,
    [MOVEMENT_TYPE_FACE_DOWN_UP_AND_LEFT] = MovementType_FaceDownUpAndLeft,
    [MOVEMENT_TYPE_FACE_DOWN_UP_AND_RIGHT] = MovementType_FaceDownUpAndRight,
    [MOVEMENT_TYPE_FACE_UP_LEFT_AND_RIGHT] = MovementType_FaceUpRightAndLeft,
    [MOVEMENT_TYPE_FACE_DOWN_LEFT_AND_RIGHT] = MovementType_FaceDownRightAndLeft,
    [MOVEMENT_TYPE_ROTATE_COUNTERCLOCKWISE] = MovementType_RotateCounterclockwise,
    [MOVEMENT_TYPE_ROTATE_CLOCKWISE] = MovementType_RotateClockwise,
    [MOVEMENT_TYPE_WALK_UP_AND_DOWN] = MovementType_WalkBackAndForth,
    [MOVEMENT_TYPE_WALK_DOWN_AND_UP] = MovementType_WalkBackAndForth,
    [MOVEMENT_TYPE_WALK_LEFT_AND_RIGHT] = MovementType_WalkBackAndForth,
    [MOVEMENT_TYPE_WALK_RIGHT_AND_LEFT] = MovementType_WalkBackAndForth,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_LEFT_DOWN] = MovementType_WalkSequenceUpRightLeftDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_DOWN_UP] = MovementType_WalkSequenceRightLeftDownUp,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_RIGHT_LEFT] = MovementType_WalkSequenceDownUpRightLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_UP_RIGHT] = MovementType_WalkSequenceLeftDownUpRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_RIGHT_DOWN] = MovementType_WalkSequenceUpLeftRightDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_DOWN_UP] = MovementType_WalkSequenceLeftRightDownUp,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_LEFT_RIGHT] = MovementType_WalkSequenceDownUpLeftRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_UP_LEFT] = MovementType_WalkSequenceRightDownUpLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_DOWN_RIGHT] = MovementType_WalkSequenceLeftUpDownRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_RIGHT_LEFT] = MovementType_WalkSequenceUpDownRightLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_UP_DOWN] = MovementType_WalkSequenceRightLeftUpDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_LEFT_UP] = MovementType_WalkSequenceDownRightLeftUp,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_DOWN_LEFT] = MovementType_WalkSequenceRightUpDownLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_LEFT_RIGHT] = MovementType_WalkSequenceUpDownLeftRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_UP_DOWN] = MovementType_WalkSequenceLeftRightUpDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_RIGHT_UP] = MovementType_WalkSequenceDownLeftRightUp,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_DOWN_RIGHT] = MovementType_WalkSequenceUpLeftDownRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_UP_LEFT] = MovementType_WalkSequenceDownRightUpLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_RIGHT_UP] = MovementType_WalkSequenceLeftDownRightUp,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_LEFT_DOWN] = MovementType_WalkSequenceRightUpLeftDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_DOWN_LEFT] = MovementType_WalkSequenceUpRightDownLeft,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_UP_RIGHT] = MovementType_WalkSequenceDownLeftUpRight,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_RIGHT_DOWN] = MovementType_WalkSequenceLeftUpRightDown,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_LEFT_UP] = MovementType_WalkSequenceRightDownLeftUp,
    [MOVEMENT_TYPE_COPY_PLAYER] = MovementType_CopyPlayer,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE] = MovementType_CopyPlayer,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE] = MovementType_CopyPlayer,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE] = MovementType_CopyPlayer,
    [MOVEMENT_TYPE_TREE_DISGUISE] = MovementType_TreeDisguise,
    [MOVEMENT_TYPE_MOUNTAIN_DISGUISE] = MovementType_MountainDisguise,
    [MOVEMENT_TYPE_COPY_PLAYER_IN_GRASS] = MovementType_CopyPlayerInGrass,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE_IN_GRASS] = MovementType_CopyPlayerInGrass,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE_IN_GRASS] = MovementType_CopyPlayerInGrass,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE_IN_GRASS] = MovementType_CopyPlayerInGrass,
    [MOVEMENT_TYPE_BURIED] = MovementType_Buried,
    [MOVEMENT_TYPE_WALK_IN_PLACE_DOWN] = MovementType_WalkInPlace,
    [MOVEMENT_TYPE_WALK_IN_PLACE_UP] = MovementType_WalkInPlace,
    [MOVEMENT_TYPE_WALK_IN_PLACE_LEFT] = MovementType_WalkInPlace,
    [MOVEMENT_TYPE_WALK_IN_PLACE_RIGHT] = MovementType_WalkInPlace,
    [MOVEMENT_TYPE_JOG_IN_PLACE_DOWN] = MovementType_JogInPlace,
    [MOVEMENT_TYPE_JOG_IN_PLACE_UP] = MovementType_JogInPlace,
    [MOVEMENT_TYPE_JOG_IN_PLACE_LEFT] = MovementType_JogInPlace,
    [MOVEMENT_TYPE_JOG_IN_PLACE_RIGHT] = MovementType_JogInPlace,
    [MOVEMENT_TYPE_RUN_IN_PLACE_DOWN] = MovementType_RunInPlace,
    [MOVEMENT_TYPE_RUN_IN_PLACE_UP] = MovementType_RunInPlace,
    [MOVEMENT_TYPE_RUN_IN_PLACE_LEFT] = MovementType_RunInPlace,
    [MOVEMENT_TYPE_RUN_IN_PLACE_RIGHT] = MovementType_RunInPlace,
    [MOVEMENT_TYPE_INVISIBLE] = MovementType_Invisible,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_DOWN] = MovementType_WalkSlowlyInPlace,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_UP] = MovementType_WalkSlowlyInPlace,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_LEFT] = MovementType_WalkSlowlyInPlace,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_RIGHT] = MovementType_WalkSlowlyInPlace,
    [MOVEMENT_TYPE_FOLLOW_PLAYER] = MovementType_FollowPlayer,
    [MOVEMENT_TYPE_WANDER_AROUND_OWE] = MovementType_OverworldWildEncounter_WanderAround,
    [MOVEMENT_TYPE_CHASE_PLAYER_OWE] = MovementType_OverworldWildEncounter_ChasePlayer,
    [MOVEMENT_TYPE_FLEE_PLAYER_OWE] = MovementType_OverworldWildEncounter_FleePlayer,
    [MOVEMENT_TYPE_WATCH_PLAYER_OWE] = MovementType_OverworldWildEncounter_WatchPlayer,
    [MOVEMENT_TYPE_APPROACH_PLAYER_OWE] = MovementType_OverworldWildEncounter_ApproachPlayer,
    [MOVEMENT_TYPE_DESPAWN_OWE] = MovementType_OverworldWildEncounter_Despawn,
};

static const bool8 sMovementTypeHasRange[NUM_MOVEMENT_TYPES] = {
    [MOVEMENT_TYPE_WANDER_AROUND] = TRUE,
    [MOVEMENT_TYPE_WANDER_UP_AND_DOWN] = TRUE,
    [MOVEMENT_TYPE_WANDER_DOWN_AND_UP] = TRUE,
    [MOVEMENT_TYPE_WANDER_LEFT_AND_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WANDER_RIGHT_AND_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_UP_AND_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_DOWN_AND_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_LEFT_AND_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_RIGHT_AND_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_LEFT_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_DOWN_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_RIGHT_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_UP_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_RIGHT_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_DOWN_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_LEFT_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_UP_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_DOWN_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_RIGHT_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_UP_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_LEFT_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_DOWN_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_LEFT_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_UP_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_RIGHT_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_DOWN_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_UP_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_RIGHT_UP] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_LEFT_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_DOWN_LEFT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_UP_RIGHT] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_RIGHT_DOWN] = TRUE,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_LEFT_UP] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_IN_GRASS] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE_IN_GRASS] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE_IN_GRASS] = TRUE,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE_IN_GRASS] = TRUE,
};

const u8 gInitialMovementTypeFacingDirections[NUM_MOVEMENT_TYPES] = {
    [MOVEMENT_TYPE_NONE] = DIR_SOUTH,
    [MOVEMENT_TYPE_LOOK_AROUND] = DIR_SOUTH,
    [MOVEMENT_TYPE_WANDER_AROUND] = DIR_SOUTH,
    [MOVEMENT_TYPE_WANDER_UP_AND_DOWN] = DIR_NORTH,
    [MOVEMENT_TYPE_WANDER_DOWN_AND_UP] = DIR_SOUTH,
    [MOVEMENT_TYPE_WANDER_LEFT_AND_RIGHT] = DIR_WEST,
    [MOVEMENT_TYPE_WANDER_RIGHT_AND_LEFT] = DIR_EAST,
    [MOVEMENT_TYPE_FACE_UP] = DIR_NORTH,
    [MOVEMENT_TYPE_FACE_DOWN] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_LEFT] = DIR_WEST,
    [MOVEMENT_TYPE_FACE_RIGHT] = DIR_EAST,
    [MOVEMENT_TYPE_PLAYER] = DIR_SOUTH,
    [MOVEMENT_TYPE_BERRY_TREE_GROWTH] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_DOWN_AND_UP] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_LEFT_AND_RIGHT] = DIR_WEST,
    [MOVEMENT_TYPE_FACE_UP_AND_LEFT] = DIR_NORTH,
    [MOVEMENT_TYPE_FACE_UP_AND_RIGHT] = DIR_NORTH,
    [MOVEMENT_TYPE_FACE_DOWN_AND_LEFT] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_DOWN_AND_RIGHT] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_DOWN_UP_AND_LEFT] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_DOWN_UP_AND_RIGHT] = DIR_SOUTH,
    [MOVEMENT_TYPE_FACE_UP_LEFT_AND_RIGHT] = DIR_NORTH,
    [MOVEMENT_TYPE_FACE_DOWN_LEFT_AND_RIGHT] = DIR_SOUTH,
    [MOVEMENT_TYPE_ROTATE_COUNTERCLOCKWISE] = DIR_SOUTH,
    [MOVEMENT_TYPE_ROTATE_CLOCKWISE] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_UP_AND_DOWN] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_DOWN_AND_UP] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_LEFT_AND_RIGHT] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_RIGHT_AND_LEFT] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_LEFT_DOWN] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_DOWN_UP] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_RIGHT_LEFT] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_UP_RIGHT] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_RIGHT_DOWN] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_DOWN_UP] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_UP_LEFT_RIGHT] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_UP_LEFT] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_DOWN_RIGHT] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_RIGHT_LEFT] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_LEFT_UP_DOWN] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_LEFT_UP] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_DOWN_LEFT] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_DOWN_LEFT_RIGHT] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_RIGHT_UP_DOWN] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_RIGHT_UP] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_LEFT_DOWN_RIGHT] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_RIGHT_UP_LEFT] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_DOWN_RIGHT_UP] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_UP_LEFT_DOWN] = DIR_EAST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_UP_RIGHT_DOWN_LEFT] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_DOWN_LEFT_UP_RIGHT] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SEQUENCE_LEFT_UP_RIGHT_DOWN] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_LEFT_UP] = DIR_EAST,
    [MOVEMENT_TYPE_COPY_PLAYER] = DIR_NORTH,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE] = DIR_SOUTH,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE] = DIR_WEST,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE] = DIR_EAST,
    [MOVEMENT_TYPE_TREE_DISGUISE] = DIR_SOUTH,
    [MOVEMENT_TYPE_MOUNTAIN_DISGUISE] = DIR_SOUTH,
    [MOVEMENT_TYPE_COPY_PLAYER_IN_GRASS] = DIR_NORTH,
    [MOVEMENT_TYPE_COPY_PLAYER_OPPOSITE_IN_GRASS] = DIR_SOUTH,
    [MOVEMENT_TYPE_COPY_PLAYER_COUNTERCLOCKWISE_IN_GRASS] = DIR_WEST,
    [MOVEMENT_TYPE_COPY_PLAYER_CLOCKWISE_IN_GRASS] = DIR_EAST,
    [MOVEMENT_TYPE_BURIED] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_IN_PLACE_DOWN] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_IN_PLACE_UP] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_IN_PLACE_LEFT] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_IN_PLACE_RIGHT] = DIR_EAST,
    [MOVEMENT_TYPE_JOG_IN_PLACE_DOWN] = DIR_SOUTH,
    [MOVEMENT_TYPE_JOG_IN_PLACE_UP] = DIR_NORTH,
    [MOVEMENT_TYPE_JOG_IN_PLACE_LEFT] = DIR_WEST,
    [MOVEMENT_TYPE_JOG_IN_PLACE_RIGHT] = DIR_EAST,
    [MOVEMENT_TYPE_RUN_IN_PLACE_DOWN] = DIR_SOUTH,
    [MOVEMENT_TYPE_RUN_IN_PLACE_UP] = DIR_NORTH,
    [MOVEMENT_TYPE_RUN_IN_PLACE_LEFT] = DIR_WEST,
    [MOVEMENT_TYPE_RUN_IN_PLACE_RIGHT] = DIR_EAST,
    [MOVEMENT_TYPE_INVISIBLE] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_DOWN] = DIR_SOUTH,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_UP] = DIR_NORTH,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_LEFT] = DIR_WEST,
    [MOVEMENT_TYPE_WALK_SLOWLY_IN_PLACE_RIGHT] = DIR_EAST,
    [MOVEMENT_TYPE_FOLLOW_PLAYER] = DIR_SOUTH,
    [MOVEMENT_TYPE_WANDER_AROUND_OWE] = DIR_SOUTH,
    [MOVEMENT_TYPE_CHASE_PLAYER_OWE] = DIR_SOUTH,
    [MOVEMENT_TYPE_FLEE_PLAYER_OWE] = DIR_SOUTH,
    [MOVEMENT_TYPE_WATCH_PLAYER_OWE] = DIR_SOUTH,
    [MOVEMENT_TYPE_APPROACH_PLAYER_OWE] = DIR_SOUTH,
    [MOVEMENT_TYPE_DESPAWN_OWE] = DIR_SOUTH,
};

#include "data/object_events/object_event_graphics_info_pointers.h"
#include "data/field_effects/field_effect_object_template_pointers.h"
#include "data/object_events/object_event_pic_tables.h"
#include "data/object_events/object_event_anims.h"
#include "data/object_events/base_oam.h"
#include "data/object_events/object_event_subsprites.h"
#include "data/object_events/object_event_graphics_info.h"
#include "data/object_events/object_event_graphics_info_followers.h"

static const struct SpritePalette sObjectEventSpritePalettes[] = {
    {gObjectEventPal_PrlJessie,               OBJ_EVENT_PAL_TAG_PRL_JESSIE},
    {gObjectEventPal_PrlJames,                OBJ_EVENT_PAL_TAG_PRL_JAMES},
    {gObjectEventPal_Npc1,                  OBJ_EVENT_PAL_TAG_NPC_1},
    {gObjectEventPal_Npc2,                  OBJ_EVENT_PAL_TAG_NPC_2},
    {gObjectEventPal_Npc3,                  OBJ_EVENT_PAL_TAG_NPC_3},
    {gObjectEventPal_Npc4,                  OBJ_EVENT_PAL_TAG_NPC_4},
    {gObjectEventPal_Npc1Reflection,        OBJ_EVENT_PAL_TAG_NPC_1_REFLECTION},
    {gObjectEventPal_Npc2Reflection,        OBJ_EVENT_PAL_TAG_NPC_2_REFLECTION},
    {gObjectEventPal_Npc3Reflection,        OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION},
    {gObjectEventPal_Npc4Reflection,        OBJ_EVENT_PAL_TAG_NPC_4_REFLECTION},
    {gObjectEventPal_Brendan,               OBJ_EVENT_PAL_TAG_BRENDAN},
    {gObjectEventPal_BrendanReflection,     OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION},
    {gObjectEventPal_BridgeReflection,      OBJ_EVENT_PAL_TAG_BRIDGE_REFLECTION},
    {gObjectEventPal_PlayerUnderwater,      OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER},
    {gObjectEventPal_QuintyPlump,           OBJ_EVENT_PAL_TAG_QUINTY_PLUMP},
    {gObjectEventPal_QuintyPlumpReflection, OBJ_EVENT_PAL_TAG_QUINTY_PLUMP_REFLECTION},
    {gObjectEventPal_Truck,                 OBJ_EVENT_PAL_TAG_TRUCK},
    {gObjectEventPal_Vigoroth,              OBJ_EVENT_PAL_TAG_VIGOROTH},
    {gObjectEventPal_EnemyZigzagoon,        OBJ_EVENT_PAL_TAG_ZIGZAGOON},
    {gObjectEventPal_May,                   OBJ_EVENT_PAL_TAG_MAY},
    {gObjectEventPal_MayReflection,         OBJ_EVENT_PAL_TAG_MAY_REFLECTION},
    {gObjectEventPal_MovingBox,             OBJ_EVENT_PAL_TAG_MOVING_BOX},
    {gObjectEventPal_CableCar,              OBJ_EVENT_PAL_TAG_CABLE_CAR},
    {gObjectEventPal_SSTidal,               OBJ_EVENT_PAL_TAG_SSTIDAL},
    {gObjectEventPal_Kyogre,                OBJ_EVENT_PAL_TAG_KYOGRE},
    {gObjectEventPal_KyogreReflection,      OBJ_EVENT_PAL_TAG_KYOGRE_REFLECTION},
    {gObjectEventPal_Groudon,               OBJ_EVENT_PAL_TAG_GROUDON},
    {gObjectEventPal_GroudonReflection,     OBJ_EVENT_PAL_TAG_GROUDON_REFLECTION},
    {gObjectEventPal_SubmarineShadow,       OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW},
    {gObjectEventPal_Poochyena,             OBJ_EVENT_PAL_TAG_POOCHYENA},
    {gObjectEventPal_RedLeaf,               OBJ_EVENT_PAL_TAG_RED_LEAF},
    {gObjectEventPal_Deoxys,                OBJ_EVENT_PAL_TAG_DEOXYS},
    {gObjectEventPal_BirthIslandStone,      OBJ_EVENT_PAL_TAG_BIRTH_ISLAND_STONE},
    {gObjectEventPal_HoOh,                  OBJ_EVENT_PAL_TAG_HO_OH},
    {gObjectEventPal_Lugia,                 OBJ_EVENT_PAL_TAG_LUGIA},
    {gObjectEventPal_RubySapphireBrendan,   OBJ_EVENT_PAL_TAG_RS_BRENDAN},
    {gObjectEventPal_RubySapphireMay,       OBJ_EVENT_PAL_TAG_RS_MAY},
#if IS_FRLG
    {gObjectEventPal_PlayerFrlg,            OBJ_EVENT_PAL_TAG_PLAYER_RED},
    {gObjectEventPal_PlayerReflectionFrlg,  OBJ_EVENT_PAL_TAG_PLAYER_RED_REFLECTION},
    {gObjectEventPal_PlayerFrlg,            OBJ_EVENT_PAL_TAG_PLAYER_GREEN},
    {gObjectEventPal_PlayerReflectionFrlg,  OBJ_EVENT_PAL_TAG_PLAYER_GREEN_REFLECTION},
    {gObjectEventPal_NpcBlue,               OBJ_EVENT_PAL_TAG_NPC_BLUE},
    {gObjectEventPal_NpcPink,               OBJ_EVENT_PAL_TAG_NPC_PINK},
    {gObjectEventPal_NpcGreen,              OBJ_EVENT_PAL_TAG_NPC_GREEN},
    {gObjectEventPal_NpcWhite,              OBJ_EVENT_PAL_TAG_NPC_WHITE},
    {gObjectEventPal_NpcBlueReflection,     OBJ_EVENT_PAL_TAG_NPC_BLUE_REFLECTION},
    {gObjectEventPal_NpcPinkReflection,     OBJ_EVENT_PAL_TAG_NPC_PINK_REFLECTION},
    {gObjectEventPal_NpcGreenReflection,    OBJ_EVENT_PAL_TAG_NPC_GREEN_REFLECTION},
    {gObjectEventPal_NpcWhiteReflection,    OBJ_EVENT_PAL_TAG_NPC_WHITE_REFLECTION},
    {gObjectEventPal_Meteorite,             OBJ_EVENT_PAL_TAG_METEORITE},
    {gObjectEventPal_SSAnne,                OBJ_EVENT_PAL_TAG_SS_ANNE},
    {gObjectEventPal_Seagallop,             OBJ_EVENT_PAL_TAG_SEAGALLOP},
#endif // IS_FRLG
#if OW_FOLLOWERS_POKEBALLS
    {gObjectEventPal_MasterBall,            OBJ_EVENT_PAL_TAG_BALL_MASTER},
    {gObjectEventPal_UltraBall,             OBJ_EVENT_PAL_TAG_BALL_ULTRA},
    {gObjectEventPal_GreatBall,             OBJ_EVENT_PAL_TAG_BALL_GREAT},
    {gObjectEventPal_SafariBall,            OBJ_EVENT_PAL_TAG_BALL_SAFARI},
    {gObjectEventPal_NetBall,               OBJ_EVENT_PAL_TAG_BALL_NET},
    {gObjectEventPal_DiveBall,              OBJ_EVENT_PAL_TAG_BALL_DIVE},
    {gObjectEventPal_NestBall,              OBJ_EVENT_PAL_TAG_BALL_NEST},
    {gObjectEventPal_RepeatBall,            OBJ_EVENT_PAL_TAG_BALL_REPEAT},
    {gObjectEventPal_TimerBall,             OBJ_EVENT_PAL_TAG_BALL_TIMER},
    {gObjectEventPal_LuxuryBall,            OBJ_EVENT_PAL_TAG_BALL_LUXURY},
    {gObjectEventPal_PremierBall,           OBJ_EVENT_PAL_TAG_BALL_PREMIER},
    {gObjectEventPal_DuskBall,              OBJ_EVENT_PAL_TAG_BALL_DUSK},
    {gObjectEventPal_HealBall,              OBJ_EVENT_PAL_TAG_BALL_HEAL},
    {gObjectEventPal_QuickBall,             OBJ_EVENT_PAL_TAG_BALL_QUICK},
    {gObjectEventPal_CherishBall,           OBJ_EVENT_PAL_TAG_BALL_CHERISH},
    {gObjectEventPal_ParkBall,              OBJ_EVENT_PAL_TAG_BALL_PARK},
    {gObjectEventPal_FastBall,              OBJ_EVENT_PAL_TAG_BALL_FAST},
    {gObjectEventPal_LevelBall,             OBJ_EVENT_PAL_TAG_BALL_LEVEL},
    {gObjectEventPal_LureBall,              OBJ_EVENT_PAL_TAG_BALL_LURE},
    {gObjectEventPal_HeavyBall,             OBJ_EVENT_PAL_TAG_BALL_HEAVY},
    {gObjectEventPal_LoveBall,              OBJ_EVENT_PAL_TAG_BALL_LOVE},
    {gObjectEventPal_FriendBall,            OBJ_EVENT_PAL_TAG_BALL_FRIEND},
    {gObjectEventPal_MoonBall,              OBJ_EVENT_PAL_TAG_BALL_MOON},
    {gObjectEventPal_SportBall,             OBJ_EVENT_PAL_TAG_BALL_SPORT},
    {gObjectEventPal_DreamBall,             OBJ_EVENT_PAL_TAG_BALL_DREAM},
    {gObjectEventPal_BeastBall,             OBJ_EVENT_PAL_TAG_BALL_BEAST},
    // Gen VIII
    {gObjectEventPal_StrangeBall,           OBJ_EVENT_PAL_TAG_BALL_STRANGE},
#endif //OW_FOLLOWERS_POKEBALLS
    {gObjectEventPal_Substitute,            OBJ_EVENT_PAL_TAG_SUBSTITUTE},
    {gObjectEventPaletteLight,              OBJ_EVENT_PAL_TAG_LIGHT},
    {gObjectEventPaletteLight2,             OBJ_EVENT_PAL_TAG_LIGHT_2},
    {gObjectEventPaletteEmotes,             OBJ_EVENT_PAL_TAG_EMOTES},
    {gObjectEventPaletteNeonLight,          OBJ_EVENT_PAL_TAG_NEON_LIGHT},
#ifdef BUGFIX
    {NULL,                                  OBJ_EVENT_PAL_TAG_NONE},
#else
    {}, // BUG: FindObjectEventPaletteIndexByTag looks for OBJ_EVENT_PAL_TAG_NONE and not 0x0.
        // If it's looking for a tag that isn't in this table, the game locks in an infinite loop.
#endif
};

static const u16 sReflectionPaletteTags_Brendan[] = {
    OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
};

static const u16 sReflectionPaletteTags_May[] = {
    OBJ_EVENT_PAL_TAG_MAY_REFLECTION,
    OBJ_EVENT_PAL_TAG_MAY_REFLECTION,
    OBJ_EVENT_PAL_TAG_MAY_REFLECTION,
    OBJ_EVENT_PAL_TAG_MAY_REFLECTION,
};

static const u16 sReflectionPaletteTags_PlayerUnderwater[] = {
    OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER,
    OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER,
    OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER,
    OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER,
};

static const struct PairedPalettes sPlayerReflectionPaletteSets[] = {
    {OBJ_EVENT_PAL_TAG_BRENDAN,           sReflectionPaletteTags_Brendan},
    {OBJ_EVENT_PAL_TAG_MAY,               sReflectionPaletteTags_May},
    {OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER, sReflectionPaletteTags_PlayerUnderwater},
    {OBJ_EVENT_PAL_TAG_NONE,              NULL},
};

static const u16 sReflectionPaletteTags_QuintyPlump[] = {
    OBJ_EVENT_PAL_TAG_QUINTY_PLUMP_REFLECTION,
    OBJ_EVENT_PAL_TAG_QUINTY_PLUMP_REFLECTION,
    OBJ_EVENT_PAL_TAG_QUINTY_PLUMP_REFLECTION,
    OBJ_EVENT_PAL_TAG_QUINTY_PLUMP_REFLECTION,
};

static const u16 sReflectionPaletteTags_Truck[] = {
    OBJ_EVENT_PAL_TAG_TRUCK,
    OBJ_EVENT_PAL_TAG_TRUCK,
    OBJ_EVENT_PAL_TAG_TRUCK,
    OBJ_EVENT_PAL_TAG_TRUCK,
};

static const u16 sReflectionPaletteTags_VigorothMover[] = {
    OBJ_EVENT_PAL_TAG_VIGOROTH,
    OBJ_EVENT_PAL_TAG_VIGOROTH,
    OBJ_EVENT_PAL_TAG_VIGOROTH,
    OBJ_EVENT_PAL_TAG_VIGOROTH,
};

static const u16 sReflectionPaletteTags_MovingBox[] = {
    OBJ_EVENT_PAL_TAG_MOVING_BOX,
    OBJ_EVENT_PAL_TAG_MOVING_BOX,
    OBJ_EVENT_PAL_TAG_MOVING_BOX,
    OBJ_EVENT_PAL_TAG_MOVING_BOX,
};

static const u16 sReflectionPaletteTags_CableCar[] = {
    OBJ_EVENT_PAL_TAG_CABLE_CAR,
    OBJ_EVENT_PAL_TAG_CABLE_CAR,
    OBJ_EVENT_PAL_TAG_CABLE_CAR,
    OBJ_EVENT_PAL_TAG_CABLE_CAR,
};

static const u16 sReflectionPaletteTags_SSTidal[] = {
    OBJ_EVENT_PAL_TAG_SSTIDAL,
    OBJ_EVENT_PAL_TAG_SSTIDAL,
    OBJ_EVENT_PAL_TAG_SSTIDAL,
    OBJ_EVENT_PAL_TAG_SSTIDAL,
};

static const u16 sReflectionPaletteTags_SubmarineShadow[] = {
    OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW,
    OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW,
    OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW,
    OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW,
};

static const u16 sReflectionPaletteTags_Kyogre[] = {
    OBJ_EVENT_PAL_TAG_KYOGRE_REFLECTION,
    OBJ_EVENT_PAL_TAG_KYOGRE_REFLECTION,
    OBJ_EVENT_PAL_TAG_KYOGRE_REFLECTION,
    OBJ_EVENT_PAL_TAG_KYOGRE_REFLECTION,
};

static const u16 sReflectionPaletteTags_Groudon[] = {
    OBJ_EVENT_PAL_TAG_GROUDON_REFLECTION,
    OBJ_EVENT_PAL_TAG_GROUDON_REFLECTION,
    OBJ_EVENT_PAL_TAG_GROUDON_REFLECTION,
    OBJ_EVENT_PAL_TAG_GROUDON_REFLECTION,
};

static const u16 sReflectionPaletteTags_Npc3[] = { // Only used by the Route 120 bridge Kecleon
    OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
};

static const u16 sReflectionPaletteTags_RedLeaf[] = {
    OBJ_EVENT_PAL_TAG_RED_LEAF,
    OBJ_EVENT_PAL_TAG_RED_LEAF,
    OBJ_EVENT_PAL_TAG_RED_LEAF,
    OBJ_EVENT_PAL_TAG_RED_LEAF,
};

static const struct PairedPalettes sSpecialObjectReflectionPaletteSets[] = {
    {OBJ_EVENT_PAL_TAG_BRENDAN,          sReflectionPaletteTags_Brendan},
    {OBJ_EVENT_PAL_TAG_MAY,              sReflectionPaletteTags_May},
    {OBJ_EVENT_PAL_TAG_QUINTY_PLUMP,     sReflectionPaletteTags_QuintyPlump},
    {OBJ_EVENT_PAL_TAG_TRUCK,            sReflectionPaletteTags_Truck},
    {OBJ_EVENT_PAL_TAG_VIGOROTH,         sReflectionPaletteTags_VigorothMover},
    {OBJ_EVENT_PAL_TAG_MOVING_BOX,       sReflectionPaletteTags_MovingBox},
    {OBJ_EVENT_PAL_TAG_CABLE_CAR,        sReflectionPaletteTags_CableCar},
    {OBJ_EVENT_PAL_TAG_SSTIDAL,          sReflectionPaletteTags_SSTidal},
    {OBJ_EVENT_PAL_TAG_KYOGRE,           sReflectionPaletteTags_Kyogre},
    {OBJ_EVENT_PAL_TAG_GROUDON,          sReflectionPaletteTags_Groudon},
    {OBJ_EVENT_PAL_TAG_NPC_3,            sReflectionPaletteTags_Npc3},
    {OBJ_EVENT_PAL_TAG_SUBMARINE_SHADOW, sReflectionPaletteTags_SubmarineShadow},
    {OBJ_EVENT_PAL_TAG_RED_LEAF,         sReflectionPaletteTags_RedLeaf},
    {OBJ_EVENT_PAL_TAG_NONE,             NULL},
};

static const u16 sObjectPaletteTags0[] = {
    [PALSLOT_PLAYER]            = OBJ_EVENT_PAL_TAG_BRENDAN,
    [PALSLOT_PLAYER_REFLECTION] = OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    [PALSLOT_NPC_1]             = OBJ_EVENT_PAL_TAG_NPC_1,
    [PALSLOT_NPC_2]             = OBJ_EVENT_PAL_TAG_NPC_2,
    [PALSLOT_NPC_3]             = OBJ_EVENT_PAL_TAG_NPC_3,
    [PALSLOT_NPC_4]             = OBJ_EVENT_PAL_TAG_NPC_4,
    [PALSLOT_NPC_1_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_1_REFLECTION,
    [PALSLOT_NPC_2_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_2_REFLECTION,
    [PALSLOT_NPC_3_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    [PALSLOT_NPC_4_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_4_REFLECTION,
};

static const u16 sObjectPaletteTags1[] = {
    [PALSLOT_PLAYER]            = OBJ_EVENT_PAL_TAG_BRENDAN,
    [PALSLOT_PLAYER_REFLECTION] = OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    [PALSLOT_NPC_1]             = OBJ_EVENT_PAL_TAG_NPC_1,
    [PALSLOT_NPC_2]             = OBJ_EVENT_PAL_TAG_NPC_2,
    [PALSLOT_NPC_3]             = OBJ_EVENT_PAL_TAG_NPC_3,
    [PALSLOT_NPC_4]             = OBJ_EVENT_PAL_TAG_NPC_4,
    [PALSLOT_NPC_1_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_1_REFLECTION,
    [PALSLOT_NPC_2_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_2_REFLECTION,
    [PALSLOT_NPC_3_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    [PALSLOT_NPC_4_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_4_REFLECTION,
};

static const u16 sObjectPaletteTags2[] = {
    [PALSLOT_PLAYER]            = OBJ_EVENT_PAL_TAG_BRENDAN,
    [PALSLOT_PLAYER_REFLECTION] = OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    [PALSLOT_NPC_1]             = OBJ_EVENT_PAL_TAG_NPC_1,
    [PALSLOT_NPC_2]             = OBJ_EVENT_PAL_TAG_NPC_2,
    [PALSLOT_NPC_3]             = OBJ_EVENT_PAL_TAG_NPC_3,
    [PALSLOT_NPC_4]             = OBJ_EVENT_PAL_TAG_NPC_4,
    [PALSLOT_NPC_1_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_1_REFLECTION,
    [PALSLOT_NPC_2_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_2_REFLECTION,
    [PALSLOT_NPC_3_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    [PALSLOT_NPC_4_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_4_REFLECTION,
};

static const u16 sObjectPaletteTags3[] = {
    [PALSLOT_PLAYER]            = OBJ_EVENT_PAL_TAG_BRENDAN,
    [PALSLOT_PLAYER_REFLECTION] = OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION,
    [PALSLOT_NPC_1]             = OBJ_EVENT_PAL_TAG_NPC_1,
    [PALSLOT_NPC_2]             = OBJ_EVENT_PAL_TAG_NPC_2,
    [PALSLOT_NPC_3]             = OBJ_EVENT_PAL_TAG_NPC_3,
    [PALSLOT_NPC_4]             = OBJ_EVENT_PAL_TAG_NPC_4,
    [PALSLOT_NPC_1_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_1_REFLECTION,
    [PALSLOT_NPC_2_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_2_REFLECTION,
    [PALSLOT_NPC_3_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_3_REFLECTION,
    [PALSLOT_NPC_4_REFLECTION]  = OBJ_EVENT_PAL_TAG_NPC_4_REFLECTION,
};

static const u16 *const sObjectPaletteTagSets[] = {
    sObjectPaletteTags0,
    sObjectPaletteTags1,
    sObjectPaletteTags2,
    sObjectPaletteTags3,
};

#include "data/object_events/berry_tree_graphics_tables.h"
#include "data/field_effects/field_effect_objects.h"

static const s16 sMovementDelaysMedium[] = {32, 64,  96, 128};
static const s16 sMovementDelaysLong[] =   {32, 64, 128, 192}; // Unused
static const s16 sMovementDelaysShort[] =  {32, 48,  64,  80};
static const s16 sMovementDelaysOWE[] =    {64, 80,  96, 128};

#include "data/object_events/movement_type_func_tables.h"

static const u8 sFaceDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_STD_FACE_SOUTH,
    [DIR_SOUTH] = ANIM_STD_FACE_SOUTH,
    [DIR_NORTH] = ANIM_STD_FACE_NORTH,
    [DIR_WEST] = ANIM_STD_FACE_WEST,
    [DIR_EAST] = ANIM_STD_FACE_EAST,
    [DIR_SOUTHWEST] = ANIM_STD_FACE_WEST,
    [DIR_SOUTHEAST] = ANIM_STD_FACE_EAST,
    [DIR_NORTHWEST] = ANIM_STD_FACE_WEST,
    [DIR_NORTHEAST] = ANIM_STD_FACE_EAST,
};
static const u8 sMoveDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_STD_GO_SOUTH,
    [DIR_SOUTH] = ANIM_STD_GO_SOUTH,
    [DIR_NORTH] = ANIM_STD_GO_NORTH,
    [DIR_WEST] = ANIM_STD_GO_WEST,
    [DIR_EAST] = ANIM_STD_GO_EAST,
    [DIR_SOUTHWEST] = ANIM_STD_GO_WEST,
    [DIR_SOUTHEAST] = ANIM_STD_GO_EAST,
    [DIR_NORTHWEST] = ANIM_STD_GO_WEST,
    [DIR_NORTHEAST] = ANIM_STD_GO_EAST,
};
static const u8 sMoveDirectionFastAnimNums[] = {
    [DIR_NONE] = ANIM_STD_GO_FAST_SOUTH,
    [DIR_SOUTH] = ANIM_STD_GO_FAST_SOUTH,
    [DIR_NORTH] = ANIM_STD_GO_FAST_NORTH,
    [DIR_WEST] = ANIM_STD_GO_FAST_WEST,
    [DIR_EAST] = ANIM_STD_GO_FAST_EAST,
    [DIR_SOUTHWEST] = ANIM_STD_GO_FAST_WEST,
    [DIR_SOUTHEAST] = ANIM_STD_GO_FAST_EAST,
    [DIR_NORTHWEST] = ANIM_STD_GO_FAST_WEST,
    [DIR_NORTHEAST] = ANIM_STD_GO_FAST_EAST,
};
static const u8 sMoveDirectionFasterAnimNums[] = {
    [DIR_NONE] = ANIM_STD_GO_FASTER_SOUTH,
    [DIR_SOUTH] = ANIM_STD_GO_FASTER_SOUTH,
    [DIR_NORTH] = ANIM_STD_GO_FASTER_NORTH,
    [DIR_WEST] = ANIM_STD_GO_FASTER_WEST,
    [DIR_EAST] = ANIM_STD_GO_FASTER_EAST,
    [DIR_SOUTHWEST] = ANIM_STD_GO_FASTER_WEST,
    [DIR_SOUTHEAST] = ANIM_STD_GO_FASTER_EAST,
    [DIR_NORTHWEST] = ANIM_STD_GO_FASTER_WEST,
    [DIR_NORTHEAST] = ANIM_STD_GO_FASTER_EAST,
};
static const u8 sMoveDirectionFastestAnimNums[] = {
    [DIR_NONE] = ANIM_STD_GO_FASTEST_SOUTH,
    [DIR_SOUTH] = ANIM_STD_GO_FASTEST_SOUTH,
    [DIR_NORTH] = ANIM_STD_GO_FASTEST_NORTH,
    [DIR_WEST] = ANIM_STD_GO_FASTEST_WEST,
    [DIR_EAST] = ANIM_STD_GO_FASTEST_EAST,
    [DIR_SOUTHWEST] = ANIM_STD_GO_FASTEST_WEST,
    [DIR_SOUTHEAST] = ANIM_STD_GO_FASTEST_EAST,
    [DIR_NORTHWEST] = ANIM_STD_GO_FASTEST_WEST,
    [DIR_NORTHEAST] = ANIM_STD_GO_FASTEST_EAST,
};
static const u8 sJumpSpecialDirectionAnimNums[] = { // used for jumping onto surf mon
    [DIR_NONE] = ANIM_GET_ON_OFF_POKEMON_SOUTH,
    [DIR_SOUTH] = ANIM_GET_ON_OFF_POKEMON_SOUTH,
    [DIR_NORTH] = ANIM_GET_ON_OFF_POKEMON_NORTH,
    [DIR_WEST] = ANIM_GET_ON_OFF_POKEMON_WEST,
    [DIR_EAST] = ANIM_GET_ON_OFF_POKEMON_EAST,
    [DIR_SOUTHWEST] = ANIM_GET_ON_OFF_POKEMON_WEST,
    [DIR_SOUTHEAST] = ANIM_GET_ON_OFF_POKEMON_EAST,
    [DIR_NORTHWEST] = ANIM_GET_ON_OFF_POKEMON_WEST,
    [DIR_NORTHEAST] = ANIM_GET_ON_OFF_POKEMON_EAST,
};
static const u8 sAcroWheelieDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_BUNNY_HOP_BACK_WHEEL_SOUTH,
    [DIR_SOUTH] = ANIM_BUNNY_HOP_BACK_WHEEL_SOUTH,
    [DIR_NORTH] = ANIM_BUNNY_HOP_BACK_WHEEL_NORTH,
    [DIR_WEST] = ANIM_BUNNY_HOP_BACK_WHEEL_WEST,
    [DIR_EAST] = ANIM_BUNNY_HOP_BACK_WHEEL_EAST,
    [DIR_SOUTHWEST] = ANIM_BUNNY_HOP_BACK_WHEEL_WEST,
    [DIR_SOUTHEAST] = ANIM_BUNNY_HOP_BACK_WHEEL_EAST,
    [DIR_NORTHWEST] = ANIM_BUNNY_HOP_BACK_WHEEL_WEST,
    [DIR_NORTHEAST] = ANIM_BUNNY_HOP_BACK_WHEEL_EAST,
};
static const u8 sAcroUnusedDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_BUNNY_HOP_FRONT_WHEEL_SOUTH,
    [DIR_SOUTH] = ANIM_BUNNY_HOP_FRONT_WHEEL_SOUTH,
    [DIR_NORTH] = ANIM_BUNNY_HOP_FRONT_WHEEL_NORTH,
    [DIR_WEST] = ANIM_BUNNY_HOP_FRONT_WHEEL_WEST,
    [DIR_EAST] = ANIM_BUNNY_HOP_FRONT_WHEEL_EAST,
    [DIR_SOUTHWEST] = ANIM_BUNNY_HOP_FRONT_WHEEL_SOUTH,
    [DIR_SOUTHEAST] = ANIM_BUNNY_HOP_FRONT_WHEEL_SOUTH,
    [DIR_NORTHWEST] = ANIM_BUNNY_HOP_FRONT_WHEEL_NORTH,
    [DIR_NORTHEAST] = ANIM_BUNNY_HOP_FRONT_WHEEL_NORTH,
};
static const u8 sAcroEndWheelieDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_STANDING_WHEELIE_BACK_WHEEL_SOUTH,
    [DIR_SOUTH] = ANIM_STANDING_WHEELIE_BACK_WHEEL_SOUTH,
    [DIR_NORTH] = ANIM_STANDING_WHEELIE_BACK_WHEEL_NORTH,
    [DIR_WEST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_WEST,
    [DIR_EAST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_EAST,
    [DIR_SOUTHWEST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_WEST,
    [DIR_SOUTHEAST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_EAST,
    [DIR_NORTHWEST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_WEST,
    [DIR_NORTHEAST] = ANIM_STANDING_WHEELIE_BACK_WHEEL_EAST,
};
static const u8 sAcroUnusedActionDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_SOUTH,
    [DIR_SOUTH] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_SOUTH,
    [DIR_NORTH] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_NORTH,
    [DIR_WEST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_WEST,
    [DIR_EAST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_EAST,
    [DIR_SOUTHWEST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_SOUTH,
    [DIR_SOUTHEAST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_SOUTH,
    [DIR_NORTHWEST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_NORTH,
    [DIR_NORTHEAST] = ANIM_STANDING_WHEELIE_FRONT_WHEEL_NORTH,
};
static const u8 sAcroWheeliePedalDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_MOVING_WHEELIE_SOUTH,
    [DIR_SOUTH] = ANIM_MOVING_WHEELIE_SOUTH,
    [DIR_NORTH] = ANIM_MOVING_WHEELIE_NORTH,
    [DIR_WEST] = ANIM_MOVING_WHEELIE_WEST,
    [DIR_EAST] = ANIM_MOVING_WHEELIE_EAST,
    [DIR_SOUTHWEST] = ANIM_MOVING_WHEELIE_WEST,
    [DIR_SOUTHEAST] = ANIM_MOVING_WHEELIE_EAST,
    [DIR_NORTHWEST] = ANIM_MOVING_WHEELIE_WEST,
    [DIR_NORTHEAST] = ANIM_MOVING_WHEELIE_EAST,
};
static const u8 sFishingDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_TAKE_OUT_ROD_SOUTH,
    [DIR_SOUTH] = ANIM_TAKE_OUT_ROD_SOUTH,
    [DIR_NORTH] = ANIM_TAKE_OUT_ROD_NORTH,
    [DIR_WEST] = ANIM_TAKE_OUT_ROD_WEST,
    [DIR_EAST] = ANIM_TAKE_OUT_ROD_EAST,
    [DIR_SOUTHWEST] = ANIM_TAKE_OUT_ROD_SOUTH,
    [DIR_SOUTHEAST] = ANIM_TAKE_OUT_ROD_SOUTH,
    [DIR_NORTHWEST] = ANIM_TAKE_OUT_ROD_NORTH,
    [DIR_NORTHEAST] = ANIM_TAKE_OUT_ROD_NORTH,
};
static const u8 sFishingNoCatchDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_PUT_AWAY_ROD_SOUTH,
    [DIR_SOUTH] = ANIM_PUT_AWAY_ROD_SOUTH,
    [DIR_NORTH] = ANIM_PUT_AWAY_ROD_NORTH,
    [DIR_WEST] = ANIM_PUT_AWAY_ROD_WEST,
    [DIR_EAST] = ANIM_PUT_AWAY_ROD_EAST,
    [DIR_SOUTHWEST] = ANIM_PUT_AWAY_ROD_SOUTH,
    [DIR_SOUTHEAST] = ANIM_PUT_AWAY_ROD_SOUTH,
    [DIR_NORTHWEST] = ANIM_PUT_AWAY_ROD_NORTH,
    [DIR_NORTHEAST] = ANIM_PUT_AWAY_ROD_NORTH,
};
static const u8 sFishingBiteDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_HOOKED_POKEMON_SOUTH,
    [DIR_SOUTH] = ANIM_HOOKED_POKEMON_SOUTH,
    [DIR_NORTH] = ANIM_HOOKED_POKEMON_NORTH,
    [DIR_WEST] = ANIM_HOOKED_POKEMON_WEST,
    [DIR_EAST] = ANIM_HOOKED_POKEMON_EAST,
    [DIR_SOUTHWEST] = ANIM_HOOKED_POKEMON_SOUTH,
    [DIR_SOUTHEAST] = ANIM_HOOKED_POKEMON_SOUTH,
    [DIR_NORTHWEST] = ANIM_HOOKED_POKEMON_NORTH,
    [DIR_NORTHEAST] = ANIM_HOOKED_POKEMON_NORTH,
};
static const u8 sRunningDirectionAnimNums[] = {
    [DIR_NONE] = ANIM_RUN_SOUTH,
    [DIR_SOUTH] = ANIM_RUN_SOUTH,
    [DIR_NORTH] = ANIM_RUN_NORTH,
    [DIR_WEST] = ANIM_RUN_WEST,
    [DIR_EAST] = ANIM_RUN_EAST,
    [DIR_SOUTHWEST] = ANIM_RUN_WEST,
    [DIR_SOUTHEAST] = ANIM_RUN_EAST,
    [DIR_NORTHWEST] = ANIM_RUN_WEST,
    [DIR_NORTHEAST] = ANIM_RUN_EAST,
};

const u8 gTrainerFacingDirectionMovementTypes[] = {
    [DIR_NONE] = MOVEMENT_TYPE_FACE_DOWN,
    [DIR_SOUTH] = MOVEMENT_TYPE_FACE_DOWN,
    [DIR_NORTH] = MOVEMENT_TYPE_FACE_UP,
    [DIR_WEST] = MOVEMENT_TYPE_FACE_LEFT,
    [DIR_EAST] = MOVEMENT_TYPE_FACE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_TYPE_FACE_DOWN,
    [DIR_SOUTHEAST] = MOVEMENT_TYPE_FACE_DOWN,
    [DIR_NORTHWEST] = MOVEMENT_TYPE_FACE_UP,
    [DIR_NORTHEAST] = MOVEMENT_TYPE_FACE_UP,
};

static const u8 sSpinDirectionAnimNums[] = {
    [DIR_NONE]      = ANIM_SPIN_SOUTH,
    [DIR_SOUTH]     = ANIM_SPIN_SOUTH,
    [DIR_NORTH]     = ANIM_SPIN_NORTH,
    [DIR_WEST]      = ANIM_SPIN_WEST,
    [DIR_EAST]      = ANIM_SPIN_EAST,
    [DIR_SOUTHWEST] = ANIM_SPIN_SOUTH,
    [DIR_SOUTHEAST] = ANIM_SPIN_NORTH,
    [DIR_NORTHWEST] = ANIM_SPIN_WEST,
    [DIR_NORTHEAST] = ANIM_SPIN_EAST,
};

bool8 (*const gOppositeDirectionBlockedMetatileFuncs[])(u8) = {
    MetatileBehavior_IsSouthBlocked,
    MetatileBehavior_IsNorthBlocked,
    MetatileBehavior_IsWestBlocked,
    MetatileBehavior_IsEastBlocked
};

bool8 (*const gDirectionBlockedMetatileFuncs[])(u8) = {
    MetatileBehavior_IsNorthBlocked,
    MetatileBehavior_IsSouthBlocked,
    MetatileBehavior_IsEastBlocked,
    MetatileBehavior_IsWestBlocked
};

static const struct Coords16 sDirectionToVectors[] = {
    { 0,  0},
    { 0,  1},
    { 0, -1},
    {-1,  0},
    { 1,  0},
    {-1,  1},
    { 1,  1},
    {-1, -1},
    { 1, -1},
    {-2,  1},
    { 2,  1},
    {-2, -1},
    { 2, -1}
};

const u8 gFaceDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_FACE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_FACE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_FACE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_FACE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_FACE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_FACE_LEFT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_FACE_RIGHT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_FACE_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_FACE_RIGHT
};
static const u8 gWalkSlowStairsMovementActions[] = {
    [DIR_NONE]  = MOVEMENT_ACTION_WALK_SLOW_STAIRS_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_SLOW_STAIRS_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_SLOW_STAIRS_UP,
    [DIR_WEST]  = MOVEMENT_ACTION_WALK_SLOW_STAIRS_LEFT,
    [DIR_EAST]  = MOVEMENT_ACTION_WALK_SLOW_STAIRS_RIGHT,
};
const u8 gWalkSlowMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_SLOW_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_SLOW_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_SLOW_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_SLOW_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_SLOW_RIGHT,
};
const u8 gWalkNormalMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_NORMAL_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_NORMAL_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_NORMAL_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_NORMAL_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_NORMAL_RIGHT,
};
const u8 gWalkFastMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_FAST_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_FAST_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_FAST_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_FAST_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_FAST_RIGHT,
};
const u8 gRideWaterCurrentMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_RIDE_WATER_CURRENT_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_RIDE_WATER_CURRENT_UP,
    [DIR_WEST] = MOVEMENT_ACTION_RIDE_WATER_CURRENT_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_RIDE_WATER_CURRENT_RIGHT,
};
const u8 gWalkFasterMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_FASTER_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_FASTER_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_FASTER_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_FASTER_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_FASTER_RIGHT,
};
const u8 gSlideMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_SLIDE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_SLIDE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_SLIDE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_SLIDE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_SLIDE_RIGHT,
};
const u8 gPlayerRunMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_PLAYER_RUN_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_PLAYER_RUN_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_PLAYER_RUN_UP,
    [DIR_WEST] = MOVEMENT_ACTION_PLAYER_RUN_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_PLAYER_RUN_RIGHT,
};
const u8 gJump2MovementActions[] = {
    MOVEMENT_ACTION_JUMP_2_DOWN,
    MOVEMENT_ACTION_JUMP_2_DOWN,
    MOVEMENT_ACTION_JUMP_2_UP,
    MOVEMENT_ACTION_JUMP_2_LEFT,
    MOVEMENT_ACTION_JUMP_2_RIGHT,
};
const u8 gJumpInPlaceMovementActions[] = {
    MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN,
    MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN,
    MOVEMENT_ACTION_JUMP_IN_PLACE_UP,
    MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT,
    MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT,
};
const u8 gJumpInPlaceTurnAroundMovementActions[] = {
    MOVEMENT_ACTION_JUMP_IN_PLACE_UP_DOWN,
    MOVEMENT_ACTION_JUMP_IN_PLACE_UP_DOWN,
    MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN_UP,
    MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT_LEFT,
    MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT_RIGHT,
};
const u8 gJumpMovementActions[] = {
    MOVEMENT_ACTION_JUMP_DOWN,
    MOVEMENT_ACTION_JUMP_DOWN,
    MOVEMENT_ACTION_JUMP_UP,
    MOVEMENT_ACTION_JUMP_LEFT,
    MOVEMENT_ACTION_JUMP_RIGHT,
};
const u8 gJumpSpecialMovementActions[] = {
    MOVEMENT_ACTION_JUMP_SPECIAL_DOWN,
    MOVEMENT_ACTION_JUMP_SPECIAL_DOWN,
    MOVEMENT_ACTION_JUMP_SPECIAL_UP,
    MOVEMENT_ACTION_JUMP_SPECIAL_LEFT,
    MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT,
    MOVEMENT_ACTION_JUMP_SPECIAL_LEFT,
    MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT,
    MOVEMENT_ACTION_JUMP_SPECIAL_LEFT,
    MOVEMENT_ACTION_JUMP_SPECIAL_RIGHT,
};
const u8 gWalkInPlaceSlowMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_RIGHT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_SLOW_RIGHT
};
const u8 gWalkInPlaceNormalMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_RIGHT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_NORMAL_RIGHT
};
const u8 gWalkInPlaceFastMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_RIGHT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FAST_RIGHT
};
const u8 gWalkInPlaceFasterMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_UP,
    [DIR_WEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_RIGHT
};
const u8 gAcroWheelieFaceDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_LEFT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_RIGHT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_FACE_RIGHT
};
const u8 gAcroPopWheelieFaceDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_LEFT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_RIGHT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_ACRO_POP_WHEELIE_RIGHT,
};
const u8 gAcroEndWheelieFaceDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_LEFT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_RIGHT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_ACRO_END_WHEELIE_FACE_RIGHT,
};
const u8 gAcroWheelieHopFaceDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_UP,
    [DIR_WEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_LEFT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_RIGHT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_FACE_RIGHT,
};
const u8 gAcroWheelieHopDirectionMovementActions[] = {
    [DIR_NONE] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_DOWN,
    [DIR_SOUTH] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_DOWN,
    [DIR_NORTH] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_UP,
    [DIR_WEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_LEFT,
    [DIR_EAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_RIGHT,
    [DIR_SOUTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_LEFT,
    [DIR_NORTHWEST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_LEFT,
    [DIR_SOUTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_RIGHT,
    [DIR_NORTHEAST] = MOVEMENT_ACTION_ACRO_WHEELIE_HOP_RIG…81761 tokens truncated…avior;
        FieldEffectStart(FLDEFF_TRACKS_SLITHER);
    }
}

void GroundEffect_Ripple(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    DoRippleFieldEffect(objEvent, sprite);
}

void GroundEffect_StepOnPuddle(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    StartFieldEffectForObjectEvent(FLDEFF_SPLASH, objEvent);
}

void GroundEffect_SandHeap(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    StartFieldEffectForObjectEvent(FLDEFF_SAND_PILE, objEvent);
}

void GroundEffect_JumpOnTallGrass(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    u8 spriteId;

    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = objEvent->previousElevation;
    gFieldEffectArguments[3] = 2;
    FieldEffectStart(FLDEFF_JUMP_TALL_GRASS);

    spriteId = FindTallGrassFieldEffectSpriteId(
        objEvent->localId,
        objEvent->mapNum,
        objEvent->mapGroup,
        objEvent->currentCoords.x,
        objEvent->currentCoords.y);

    if (spriteId == MAX_SPRITES)
        GroundEffect_SpawnOnTallGrass(objEvent, sprite);
}

void GroundEffect_JumpOnLongGrass(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = objEvent->previousElevation;
    gFieldEffectArguments[3] = 2;
    FieldEffectStart(FLDEFF_JUMP_LONG_GRASS);
}

void GroundEffect_JumpOnShallowWater(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = objEvent->previousElevation;
    gFieldEffectArguments[3] = sprite->oam.priority;
    FieldEffectStart(FLDEFF_JUMP_SMALL_SPLASH);
}

void GroundEffect_JumpOnWater(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = objEvent->previousElevation;
    gFieldEffectArguments[3] = sprite->oam.priority;
    FieldEffectStart(FLDEFF_JUMP_BIG_SPLASH);
}

void GroundEffect_JumpLandingDust(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = objEvent->previousElevation;
    gFieldEffectArguments[3] = sprite->oam.priority;
    FieldEffectStart(FLDEFF_DUST);
}

void GroundEffect_ShortGrass(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    StartFieldEffectForObjectEvent(FLDEFF_SHORT_GRASS, objEvent);
}

void GroundEffect_HotSprings(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    StartFieldEffectForObjectEvent(FLDEFF_HOT_SPRINGS_WATER, objEvent);
}

void GroundEffect_Seaweed(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    FieldEffectStart(FLDEFF_BUBBLES);
}

static void (*const sGroundEffectFuncs[])(struct ObjectEvent *objEvent, struct Sprite *sprite) = {
    GroundEffect_SpawnOnTallGrass,      // GROUND_EFFECT_FLAG_TALL_GRASS_ON_SPAWN
    GroundEffect_StepOnTallGrass,       // GROUND_EFFECT_FLAG_TALL_GRASS_ON_MOVE
    GroundEffect_SpawnOnLongGrass,      // GROUND_EFFECT_FLAG_LONG_GRASS_ON_SPAWN
    GroundEffect_StepOnLongGrass,       // GROUND_EFFECT_FLAG_LONG_GRASS_ON_MOVE
    GroundEffect_WaterReflection,       // GROUND_EFFECT_FLAG_WATER_REFLECTION
    GroundEffect_IceReflection,         // GROUND_EFFECT_FLAG_ICE_REFLECTION
    GroundEffect_FlowingWater,          // GROUND_EFFECT_FLAG_SHALLOW_FLOWING_WATER
    GroundEffect_SandTracks,            // GROUND_EFFECT_FLAG_SAND
    GroundEffect_DeepSandTracks,        // GROUND_EFFECT_FLAG_DEEP_SAND
    GroundEffect_Ripple,                // GROUND_EFFECT_FLAG_RIPPLES
    GroundEffect_StepOnPuddle,          // GROUND_EFFECT_FLAG_PUDDLE
    GroundEffect_SandHeap,              // GROUND_EFFECT_FLAG_SAND_PILE
    GroundEffect_JumpOnTallGrass,       // GROUND_EFFECT_FLAG_LAND_IN_TALL_GRASS
    GroundEffect_JumpOnLongGrass,       // GROUND_EFFECT_FLAG_LAND_IN_LONG_GRASS
    GroundEffect_JumpOnShallowWater,    // GROUND_EFFECT_FLAG_LAND_IN_SHALLOW_WATER
    GroundEffect_JumpOnWater,           // GROUND_EFFECT_FLAG_LAND_IN_DEEP_WATER
    GroundEffect_JumpLandingDust,       // GROUND_EFFECT_FLAG_LAND_ON_NORMAL_GROUND
    GroundEffect_ShortGrass,            // GROUND_EFFECT_FLAG_SHORT_GRASS
    GroundEffect_HotSprings,            // GROUND_EFFECT_FLAG_HOT_SPRINGS
    GroundEffect_Seaweed                // GROUND_EFFECT_FLAG_SEAWEED
};

static void DoFlaggedGroundEffects(struct ObjectEvent *objEvent, struct Sprite *sprite, u32 flags)
{
    u32 i;
    if (ObjectEventIsFarawayIslandMew(objEvent) == TRUE && !ShouldMewShakeGrass(objEvent))
        return;

    for (i = 0; i < ARRAY_COUNT(sGroundEffectFuncs); i++, flags >>= 1)
        if (flags & 1)
            sGroundEffectFuncs[i](objEvent, sprite);
    if (!OW_OBJECT_VANILLA_SHADOWS && CurrentMapHasShadows() && !(gWeatherPtr->noShadows || objEvent->inHotSprings || objEvent->inSandPile || MetatileBehavior_IsPuddle(objEvent->currentMetatileBehavior)))
    {
        SetUpShadow(objEvent);
    }
}

void filters_out_some_ground_effects(struct ObjectEvent *objEvent, u32 *flags)
{
    if (objEvent->disableCoveringGroundEffects)
    {
        objEvent->inShortGrass = 0;
        objEvent->inSandPile = 0;
        objEvent->inShallowFlowingWater = 0;
        objEvent->inHotSprings = 0;
        *flags &= ~(GROUND_EFFECT_FLAG_HOT_SPRINGS
                  | GROUND_EFFECT_FLAG_SHORT_GRASS
                  | GROUND_EFFECT_FLAG_SAND_PILE
                  | GROUND_EFFECT_FLAG_SHALLOW_FLOWING_WATER
                  | GROUND_EFFECT_FLAG_TALL_GRASS_ON_MOVE);
    }
}

void FilterOutStepOnPuddleGroundEffectIfJumping(struct ObjectEvent *objEvent, u32 *flags)
{
    if (objEvent->landingJump)
        *flags &= ~GROUND_EFFECT_FLAG_PUDDLE;
}

static void DoGroundEffects_OnSpawn(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    u32 flags;

#ifdef BUGFIX
    if (objEvent->triggerGroundEffectsOnMove && objEvent->localId != OBJ_EVENT_ID_CAMERA)
#else
    if (objEvent->triggerGroundEffectsOnMove)
#endif
    {
        flags = 0;
        if (OW_LARGE_OW_SUPPORT && !sprite->oam.affineMode)
            sprite->subspriteMode = SUBSPRITES_ON;
        UpdateObjectEventElevationAndPriority(objEvent, sprite);
        GetAllGroundEffectFlags_OnSpawn(objEvent, &flags);
        SetObjectEventSpriteOamTableForLongGrass(objEvent, sprite);
        DoFlaggedGroundEffects(objEvent, sprite, flags);
        objEvent->triggerGroundEffectsOnMove = FALSE;
        objEvent->disableCoveringGroundEffects = 0;
    }
}

static void DoGroundEffects_OnBeginStep(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    u32 flags;

#ifdef BUGFIX
    if (objEvent->triggerGroundEffectsOnMove && objEvent->localId != OBJ_EVENT_ID_CAMERA)
#else
    if (objEvent->triggerGroundEffectsOnMove)
#endif
    {
        flags = 0;
        if (OW_LARGE_OW_SUPPORT && !sprite->oam.affineMode)
            sprite->subspriteMode = SUBSPRITES_ON;
        UpdateObjectEventElevationAndPriority(objEvent, sprite);
        GetAllGroundEffectFlags_OnBeginStep(objEvent, &flags);
        SetObjectEventSpriteOamTableForLongGrass(objEvent, sprite);
        filters_out_some_ground_effects(objEvent, &flags);
        DoFlaggedGroundEffects(objEvent, sprite, flags);
        objEvent->triggerGroundEffectsOnMove = FALSE;
        objEvent->disableCoveringGroundEffects = 0;
    }
}

static void DoGroundEffects_OnFinishStep(struct ObjectEvent *objEvent, struct Sprite *sprite)
{
    u32 flags;

#ifdef BUGFIX
    if (objEvent->triggerGroundEffectsOnStop && objEvent->localId != OBJ_EVENT_ID_CAMERA)
#else
    if (objEvent->triggerGroundEffectsOnStop)
#endif
    {
        flags = 0;
        UpdateObjectEventElevationAndPriority(objEvent, sprite);
        GetAllGroundEffectFlags_OnFinishStep(objEvent, &flags);
        SetObjectEventSpriteOamTableForLongGrass(objEvent, sprite);
        FilterOutStepOnPuddleGroundEffectIfJumping(objEvent, &flags);
        DoFlaggedGroundEffects(objEvent, sprite, flags);
        objEvent->triggerGroundEffectsOnStop = 0;
        objEvent->landingJump = 0;
    }
}

bool8 FreezeObjectEvent(struct ObjectEvent *objectEvent)
{
    if (objectEvent->heldMovementActive || objectEvent->frozen)
    {
        return TRUE;
    }
    else
    {
        objectEvent->frozen = TRUE;
        objectEvent->spriteAnimPausedBackup = gSprites[objectEvent->spriteId].animPaused;
        objectEvent->spriteAffineAnimPausedBackup = gSprites[objectEvent->spriteId].affineAnimPaused;
        gSprites[objectEvent->spriteId].animPaused = TRUE;
        gSprites[objectEvent->spriteId].affineAnimPaused = TRUE;
        return FALSE;
    }
}

void FreezeObjectEvents(void)
{
    u8 i;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        if (gObjectEvents[i].active && i != gPlayerAvatar.objectEventId)
            FreezeObjectEvent(&gObjectEvents[i]);
}

void FreezeObjectEventsExceptOne(u8 objectEventId)
{
    u8 i;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        if (i != objectEventId && gObjectEvents[i].active && i != gPlayerAvatar.objectEventId)
            FreezeObjectEvent(&gObjectEvents[i]);
}

void UnfreezeObjectEvent(struct ObjectEvent *objectEvent)
{
    if (objectEvent->active && objectEvent->frozen)
    {
        objectEvent->frozen = 0;
        gSprites[objectEvent->spriteId].animPaused = objectEvent->spriteAnimPausedBackup;
        gSprites[objectEvent->spriteId].affineAnimPaused = objectEvent->spriteAffineAnimPausedBackup;
    }
}

void UnfreezeObjectEvents(void)
{
    u8 i;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        if (gObjectEvents[i].active)
            UnfreezeObjectEvent(&gObjectEvents[i]);
}

static void Step1(struct Sprite *sprite, enum Direction dir)
{
    sprite->x += sDirectionToVectors[dir].x;
    sprite->y += sDirectionToVectors[dir].y;
}

static void Step2(struct Sprite *sprite, enum Direction dir)
{
    sprite->x += 2 * (u16) sDirectionToVectors[dir].x;
    sprite->y += 2 * (u16) sDirectionToVectors[dir].y;
}

static void Step3(struct Sprite *sprite, enum Direction dir)
{
    sprite->x += 2 * (u16) sDirectionToVectors[dir].x + (u16) sDirectionToVectors[dir].x;
    sprite->y += 2 * (u16) sDirectionToVectors[dir].y + (u16) sDirectionToVectors[dir].y;
}

static void Step4(struct Sprite *sprite, enum Direction dir)
{
    sprite->x += 4 * (u16) sDirectionToVectors[dir].x;
    sprite->y += 4 * (u16) sDirectionToVectors[dir].y;
}

static void Step8(struct Sprite *sprite, enum Direction dir)
{
    sprite->x += 8 * (u16) sDirectionToVectors[dir].x;
    sprite->y += 8 * (u16) sDirectionToVectors[dir].y;
}

#define sSpeed data[4]
#define sTimer data[5]

static void SetSpriteDataForNormalStep(struct Sprite *sprite, enum Direction direction, u8 speed)
{
    sprite->sDirection = direction;
    sprite->sSpeed = speed;
    sprite->sTimer = 0;
}

typedef void (*SpriteStepFunc)(struct Sprite *sprite, enum Direction direction);

static const SpriteStepFunc sStep1Funcs[] = {
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
    Step1,
};

static const SpriteStepFunc sStep2Funcs[] = {
    Step2,
    Step2,
    Step2,
    Step2,
    Step2,
    Step2,
    Step2,
    Step2,
};

static const SpriteStepFunc sStep3Funcs[] = {
    Step2,
    Step3,
    Step3,
    Step2,
    Step3,
    Step3,
};

static const SpriteStepFunc sStep4Funcs[] = {
    Step4,
    Step4,
    Step4,
    Step4,
};

static const SpriteStepFunc sStep8Funcs[] = {
    Step8,
    Step8,
};

static const SpriteStepFunc *const sNpcStepFuncTables[] = {
    [MOVE_SPEED_NORMAL] = sStep1Funcs,
    [MOVE_SPEED_FAST_1] = sStep2Funcs,
    [MOVE_SPEED_FAST_2] = sStep3Funcs,
    [MOVE_SPEED_FASTER] = sStep4Funcs,
    [MOVE_SPEED_FASTEST] = sStep8Funcs,
};

static const s16 sStepTimes[] = {
    [MOVE_SPEED_NORMAL] = ARRAY_COUNT(sStep1Funcs),
    [MOVE_SPEED_FAST_1] = ARRAY_COUNT(sStep2Funcs),
    [MOVE_SPEED_FAST_2] = ARRAY_COUNT(sStep3Funcs),
    [MOVE_SPEED_FASTER] = ARRAY_COUNT(sStep4Funcs),
    [MOVE_SPEED_FASTEST] = ARRAY_COUNT(sStep8Funcs),
};

static bool8 NpcTakeStep(struct Sprite *sprite)
{
    if (sprite->sTimer >= sStepTimes[sprite->sSpeed])
        return FALSE;

    sNpcStepFuncTables[sprite->sSpeed][sprite->sTimer](sprite, sprite->sDirection);

    sprite->sTimer++;

    if (sprite->sTimer < sStepTimes[sprite->sSpeed])
        return FALSE;

    return TRUE;
}

#undef sSpeed
#undef sTimer

#define sTimer     data[4]
#define sNumSteps  data[5]

static void SetWalkSlowSpriteData(struct Sprite *sprite, enum Direction direction)
{
    sprite->sDirection = direction;
    sprite->sTimer = 0;
    sprite->sNumSteps = 0;
}

static bool8 UpdateWalkSlowAnim(struct Sprite *sprite)
{
    if (!(sprite->sTimer & 1))
    {
        Step1(sprite, sprite->sDirection);
        sprite->sNumSteps++;
    }

    sprite->sTimer++;

    if (sprite->sNumSteps > 15)
        return TRUE;
    else
        return FALSE;
}

bool8 UpdateWalkSlowStairsAnim(struct Sprite *sprite)
{
    if (++sprite->sTimer < 3)
    {
        Step1(sprite, sprite->sDirection);
        sprite->sNumSteps++;
    }
    else
        sprite->sTimer = 0;

    if (sprite->sNumSteps > 15)
        return TRUE;
    else
        return FALSE;
}

#undef sTimer
#undef sNumSteps

static const s8 sFigure8XOffsets[FIGURE_8_LENGTH] = {
    1, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 1, 2, 2, 1, 2,
    2, 1, 2, 2, 1, 2, 1, 1,
    2, 1, 1, 2, 1, 1, 2, 1,
    1, 2, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 0,
    1, 0, 1, 0, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
};

static const s8 sFigure8YOffsets[FIGURE_8_LENGTH] = {
     0,  0,  1,  0,  0,  1,  0,  0,
     1,  0,  1,  1,  0,  1,  1,  0,
     1,  1,  0,  1,  1,  0,  1,  1,
     0,  0,  1,  0,  0,  1,  0,  0,
     1,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0, -1,  0,  0, -1,  0,  0,
    -1,  0, -1, -1,  0, -1, -1,  0,
    -1, -1, -1, -1, -1, -1, -1, -2,
};

s16 GetFigure8YOffset(s16 idx)
{
    return sFigure8YOffsets[idx];
}

s16 GetFigure8XOffset(s16 idx)
{
    return sFigure8XOffsets[idx];
}

static void InitSpriteForFigure8Anim(struct Sprite *sprite)
{
    sprite->data[6] = 0;
    sprite->data[7] = 0;
}

static bool8 AnimateSpriteInFigure8(struct Sprite *sprite)
{
    bool8 finished = FALSE;

    switch (sprite->data[7])
    {
    case 0:
        sprite->x2 += GetFigure8XOffset(sprite->data[6]);
        sprite->y2 += GetFigure8YOffset(sprite->data[6]);
        break;
    case 1:
        sprite->x2 -= GetFigure8XOffset((FIGURE_8_LENGTH - 1) - sprite->data[6]);
        sprite->y2 += GetFigure8YOffset((FIGURE_8_LENGTH - 1) - sprite->data[6]);
        break;
    case 2:
        sprite->x2 -= GetFigure8XOffset(sprite->data[6]);
        sprite->y2 += GetFigure8YOffset(sprite->data[6]);
        break;
    case 3:
        sprite->x2 += GetFigure8XOffset((FIGURE_8_LENGTH - 1) - sprite->data[6]);
        sprite->y2 += GetFigure8YOffset((FIGURE_8_LENGTH - 1) - sprite->data[6]);
        break;
    }
    if (++sprite->data[6] == FIGURE_8_LENGTH)
    {
        sprite->data[6] = 0;
        sprite->data[7]++;
    }
    if (sprite->data[7] == 4)
    {
        sprite->y2 = 0;
        sprite->x2 = 0;
        finished = TRUE;
    }
    return finished;
}

static const s8 sJumpY_High[] = {
     -4,  -6,  -8, -10, -11, -12, -12, -12,
    -11, -10,  -9,  -8,  -6,  -4,   0,   0
};

static const s8 sJumpY_Low[] = {
    0,   -2,  -3,  -4,  -5,  -6,  -6,  -6,
    -5,  -5,  -4,  -3,  -2,   0,   0,   0
};

static const s8 sJumpY_Normal[] = {
    -2,  -4,  -6,  -8,  -9, -10, -10, -10,
    -9,  -8,  -6,  -5,  -3,  -2,   0,   0
};

static const s8 *const sJumpYTable[] = {
    [JUMP_TYPE_HIGH]   = sJumpY_High,
    [JUMP_TYPE_LOW]    = sJumpY_Low,
    [JUMP_TYPE_NORMAL] = sJumpY_Normal
};

static s16 GetJumpY(s16 i, u8 type)
{
    return sJumpYTable[type][i];
}

#define sDistance  data[4]
#define sJumpType  data[5]
#define sTimer     data[6]

static void SetJumpSpriteData(struct Sprite *sprite, enum Direction direction, u8 distance, u8 type)
{
    sprite->sDirection = direction;
    sprite->sDistance = distance;
    sprite->sJumpType = type;
    sprite->sTimer = 0;
}

static u8 DoJumpSpriteMovement(struct Sprite *sprite)
{
    s16 distanceToTime[] =
    {
        [JUMP_DISTANCE_IN_PLACE] = 16,
        [JUMP_DISTANCE_NORMAL] = 16,
        [JUMP_DISTANCE_FAR] = 32,
    };
    u8 distanceToShift[] =
    {
        [JUMP_DISTANCE_IN_PLACE] = 0,
        [JUMP_DISTANCE_NORMAL] = 0,
        [JUMP_DISTANCE_FAR] = 1,
    };
    u8 result = 0;

    if (sprite->sDistance != JUMP_DISTANCE_IN_PLACE)
        Step1(sprite, sprite->sDirection);

    if (sprite->sJumpType == JUMP_TYPE_FASTER)
    {
        Step3(sprite, sprite->sDirection);
        sprite->y2 = GetJumpY(sprite->sTimer >> distanceToShift[sprite->sDistance], JUMP_TYPE_NORMAL);
        sprite->sTimer += 3;
    }
    else if (sprite->sJumpType == JUMP_TYPE_FAST)
    {
        Step1(sprite, sprite->sDirection);
        sprite->y2 = GetJumpY(sprite->sTimer >> distanceToShift[sprite->sDistance], JUMP_TYPE_NORMAL);
        sprite->sTimer++;
    }
    else
    {
        sprite->y2 = GetJumpY(sprite->sTimer >> distanceToShift[sprite->sDistance], sprite->sJumpType);
    }

    sprite->sTimer++;

    if (sprite->sTimer == distanceToTime[sprite->sDistance] >> 1)
        result = JUMP_HALFWAY;

    if (sprite->sTimer >= distanceToTime[sprite->sDistance])
    {
        sprite->y2 = 0;
        result = JUMP_FINISHED;
    }

    return result;
}

static u8 DoJumpSpecialSpriteMovement(struct Sprite *sprite)
{
    s16 distanceToTime[] = {
        [JUMP_DISTANCE_IN_PLACE] = 32,
        [JUMP_DISTANCE_NORMAL] = 32,
        [JUMP_DISTANCE_FAR] = 64,
    };
    u8 distanceToShift[] = {
        [JUMP_DISTANCE_IN_PLACE] = 1,
        [JUMP_DISTANCE_NORMAL] = 1,
        [JUMP_DISTANCE_FAR] = 2,
    };
    u8 result = 0;

    if (sprite->sDistance != JUMP_DISTANCE_IN_PLACE && !(sprite->sTimer & 1))
        Step1(sprite, sprite->sDirection);

    sprite->y2 = GetJumpY(sprite->sTimer >> distanceToShift[sprite->sDistance], sprite->sJumpType);

    sprite->sTimer++;

    if (sprite->sTimer == distanceToTime[sprite->sDistance] >> 1)
        result = JUMP_HALFWAY;

    if (sprite->sTimer >= distanceToTime[sprite->sDistance])
    {
        sprite->y2 = 0;
        result = JUMP_FINISHED;
    }

    return result;
}

#undef sDistance
#undef sJumpType
#undef sTimer

static void SetMovementDelay(struct Sprite *sprite, s16 timer)
{
    sprite->data[3] = timer; // kept for legacy reasons
    sprite->data[7] = timer; // actual timer
}

static bool8 WaitForMovementDelay(struct Sprite *sprite)
{
    if (--sprite->data[7] == 0)
    {
        sprite->data[3] = 0; // reset animation timer
        return TRUE;
    }
    return FALSE;
}

void SetAndStartSpriteAnim(struct Sprite *sprite, u8 animNum, u8 animCmdIndex)
{
    sprite->animNum = animNum;
    sprite->animPaused = FALSE;
    SeekSpriteAnim(sprite, animCmdIndex);
}

bool8 SpriteAnimEnded(struct Sprite *sprite)
{
    if (sprite->animEnded)
        return TRUE;
    else
        return FALSE;
}

void UpdateObjectEventSpriteInvisibility(struct Sprite *sprite, bool8 invisible)
{
    u16 x, y;
    s16 x2, y2;

    sprite->invisible = invisible;

    if (sprite->coordOffsetEnabled)
    {
        x = sprite->x + sprite->x2 + sprite->centerToCornerVecX + gSpriteCoordOffsetX;
        y = sprite->y + sprite->y2 + sprite->centerToCornerVecY + gSpriteCoordOffsetY;
    }
    else
    {
        x = sprite->x + sprite->x2 + sprite->centerToCornerVecX;
        y = sprite->y + sprite->y2 + sprite->centerToCornerVecY;
    }

    x2 = x - (sprite->centerToCornerVecX >> 1);
    y2 = y - (sprite->centerToCornerVecY >> 1);

    if ((s16)x >= DISPLAY_WIDTH + 16 || x2 < -16)
        sprite->invisible = TRUE;
    if ((s16)y >= DISPLAY_HEIGHT + 16 || y2 < -16)
        sprite->invisible = TRUE;
}

#define sInvisible     data[2]
#define sAnimNum       data[3]
#define sAnimState     data[4]

static void SpriteCB_VirtualObject(struct Sprite *sprite)
{
    VirtualObject_UpdateAnim(sprite);
    SetObjectSubpriorityByElevation(sprite->sVirtualObjElev, sprite, 1);
    UpdateObjectEventSpriteInvisibility(sprite, sprite->sInvisible);
}

static void UNUSED DestroyVirtualObjects(void)
{
    int i;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        struct Sprite *sprite = &gSprites[i];
        if (sprite->inUse && sprite->callback == SpriteCB_VirtualObject)
            DestroySprite(sprite);
    }
}

static int GetVirtualObjectSpriteId(u8 virtualObjId)
{
    int i;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        struct Sprite *sprite = &gSprites[i];
        if (sprite->inUse && sprite->callback == SpriteCB_VirtualObject && (u8)sprite->sVirtualObjId == virtualObjId)
            return i;
    }
    return MAX_SPRITES;
}

void TurnVirtualObject(u8 virtualObjId, enum Direction direction)
{
    u8 spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId != MAX_SPRITES)
        StartSpriteAnim(&gSprites[spriteId], GetFaceDirectionAnimNum(direction));
}

void SetVirtualObjectGraphics(u8 virtualObjId, u16 graphicsId)
{
    int spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId != MAX_SPRITES)
    {
        struct Sprite *sprite = &gSprites[spriteId];
        const struct ObjectEventGraphicsInfo *graphicsInfo = GetObjectEventGraphicsInfo(graphicsId);
        u16 tileNum = sprite->oam.tileNum;
        u8 i = FindObjectEventPaletteIndexByTag(graphicsInfo->paletteTag);
        if (i != 0xFF)
            UpdateSpritePalette(&sObjectEventSpritePalettes[i], sprite);

        sprite->oam = *graphicsInfo->oam;
        sprite->oam.tileNum = tileNum;
        sprite->images = graphicsInfo->images;

        if (graphicsInfo->subspriteTables == NULL)
        {
            sprite->subspriteTables = NULL;
            sprite->subspriteTableNum = 0;
            sprite->subspriteMode = SUBSPRITES_OFF;
        }
        else
        {
            SetSubspriteTables(sprite, graphicsInfo->subspriteTables);
            sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
        }
        StartSpriteAnim(sprite, 0);
    }
}

void SetVirtualObjectInvisibility(u8 virtualObjId, bool32 invisible)
{
    u8 spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId == MAX_SPRITES)
        return;

    if (invisible)
        gSprites[spriteId].sInvisible = TRUE;
    else
        gSprites[spriteId].sInvisible = FALSE;
}

bool32 IsVirtualObjectInvisible(u8 virtualObjId)
{
    u8 spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId == MAX_SPRITES)
        return FALSE;

    return (gSprites[spriteId].sInvisible == TRUE);
}

void SetVirtualObjectSpriteAnim(u8 virtualObjId, u8 animNum)
{
    u8 spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].sAnimNum = animNum;
        gSprites[spriteId].sAnimState = 0;
    }
}

static void MoveUnionRoomObjectUp(struct Sprite *sprite)
{
    switch (sprite->sAnimState)
    {
    case 0:
        sprite->y2 = 0;
        sprite->sAnimState++;
    case 1:
        sprite->y2 -= 8;
        if (sprite->y2 == -DISPLAY_HEIGHT)
        {
            sprite->y2 = 0;
            sprite->sInvisible = TRUE;
            sprite->sAnimNum = 0;
            sprite->sAnimState = 0;
        }
    }
}

static void MoveUnionRoomObjectDown(struct Sprite *sprite)
{
    switch (sprite->sAnimState)
    {
    case 0:
        sprite->y2 = -DISPLAY_HEIGHT;
        sprite->sAnimState++;
    case 1:
        sprite->y2 += 8;
        if (sprite->y2 == 0)
        {
            sprite->sAnimNum = 0;
            sprite->sAnimState = 0;
        }
    }
}

static void VirtualObject_UpdateAnim(struct Sprite *sprite)
{
    switch (sprite->sAnimNum)
    {
    case UNION_ROOM_SPAWN_IN:
        MoveUnionRoomObjectDown(sprite);
        break;
    case UNION_ROOM_SPAWN_OUT:
        MoveUnionRoomObjectUp(sprite);
        break;
    case 0:
        break;
    default:
        sprite->sAnimNum = 0;
        break;
    }
}

bool32 IsVirtualObjectAnimating(u8 virtualObjId)
{
    u8 spriteId = GetVirtualObjectSpriteId(virtualObjId);

    if (spriteId == MAX_SPRITES)
        return FALSE;

    if (gSprites[spriteId].sAnimNum != 0)
        return TRUE;

    return FALSE;
}

u32 StartFieldEffectForObjectEvent(u8 fieldEffectId, struct ObjectEvent *objectEvent)
{
    ObjectEventGetLocalIdAndMap(objectEvent, &gFieldEffectArguments[0], &gFieldEffectArguments[1], &gFieldEffectArguments[2]);
    return FieldEffectStart(fieldEffectId);
}

static void DoShadowFieldEffect(struct ObjectEvent *objectEvent)
{
    if (objectEvent->noShadow)
    {
        objectEvent->noShadow = FALSE;
        StartFieldEffectForObjectEvent(FLDEFF_SHADOW, objectEvent);
    }
}

static void DoRippleFieldEffect(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    const struct ObjectEventGraphicsInfo *graphicsInfo = GetObjectEventGraphicsInfo(objectEvent->graphicsId);
    gFieldEffectArguments[0] = sprite->x;
    gFieldEffectArguments[1] = sprite->y + (graphicsInfo->height >> 1) - 2;
    gFieldEffectArguments[2] = 151;
    gFieldEffectArguments[3] = 3;
    FieldEffectStart(FLDEFF_RIPPLE);
}

u8 (*const gMovementActionFuncs_LockAnim[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_LockAnim_Step0,
    MovementAction_Finish,
};

u8 (*const gMovementActionFuncs_UnlockAnim[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_UnlockAnim_Step0,
    MovementAction_Finish,
};

u8 (*const gMovementActionFuncs_FlyUp[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FlyUp_Step0,
    MovementAction_FlyUp_Step1,
    MovementAction_Fly_Finish,
};

u8 (*const gMovementActionFuncs_FlyDown[])(struct ObjectEvent *, struct Sprite *) = {
    MovementAction_FlyDown_Step0,
    MovementAction_FlyDown_Step1,
    MovementAction_Fly_Finish,
};

u8 MovementAction_LockAnim_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    bool32 ableToStore = FALSE;
    if (sLockedAnimObjectEvents == NULL)
    {
        sLockedAnimObjectEvents = AllocZeroed(sizeof(struct LockedAnimObjectEvents));
        sLockedAnimObjectEvents->localIds[0] = objectEvent->localId;
        sLockedAnimObjectEvents->count = 1;
        ableToStore = TRUE;
    }
    else
    {
        u8 i;
        u8 firstFreeSlot = OBJECT_EVENTS_COUNT;
        bool32 found = FALSE;
        for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            if (firstFreeSlot == OBJECT_EVENTS_COUNT && sLockedAnimObjectEvents->localIds[i] == LOCALID_NONE)
                firstFreeSlot = i;

            if (sLockedAnimObjectEvents->localIds[i] == objectEvent->localId)
            {
                found = TRUE;
                break;
            }
        }

        if (!found && firstFreeSlot != OBJECT_EVENTS_COUNT)
        {
            sLockedAnimObjectEvents->localIds[firstFreeSlot] = objectEvent->localId;
            sLockedAnimObjectEvents->count++;
            ableToStore = TRUE;
        }
    }

    if (ableToStore == TRUE)
    {
        objectEvent->inanimate = TRUE;
        objectEvent->facingDirectionLocked = TRUE;
    }

    sprite->sActionFuncId = 1;
    return TRUE;
}

u8 MovementAction_UnlockAnim_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    bool32 ableToStore;
    u8 index;

    sprite->sActionFuncId = 1;
    if (sLockedAnimObjectEvents != NULL)
    {
        ableToStore = FALSE;
        index = FindLockedObjectEventIndex(objectEvent);
        if (index != OBJECT_EVENTS_COUNT)
        {
            sLockedAnimObjectEvents->localIds[index] = LOCALID_NONE;
            sLockedAnimObjectEvents->count--;
            ableToStore = TRUE;
        }
        if (sLockedAnimObjectEvents->count == 0)
            FREE_AND_SET_NULL(sLockedAnimObjectEvents);
        if (ableToStore == TRUE)
        {
            objectEvent->inanimate = GetObjectEventGraphicsInfo(objectEvent->graphicsId)->inanimate;
            objectEvent->facingDirectionLocked = FALSE;
            sprite->animPaused = 0;
        }
    }

    return TRUE;
}

u8 FindLockedObjectEventIndex(struct ObjectEvent *objectEvent)
{
    u8 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (sLockedAnimObjectEvents->localIds[i] == objectEvent->localId)
            return i;
    }
    return OBJECT_EVENTS_COUNT;
}

static void CreateLevitateMovementTask(struct ObjectEvent *objectEvent)
{
    u8 taskId = CreateTask(ApplyLevitateMovement, 0xFF);
    struct Task *task = &gTasks[taskId];

    StoreWordInTwoHalfwords((u16*) &task->data[0], (u32)objectEvent);
    objectEvent->warpArrowSpriteId = taskId;
    task->data[3] = 0xFFFF;
}

static void ApplyLevitateMovement(u8 taskId)
{
    struct ObjectEvent *objectEvent;
    struct Sprite *sprite;
    struct Task *task = &gTasks[taskId];

    LoadWordFromTwoHalfwords((u16*) &task->data[0], (u32 *)&objectEvent); // load the map object pointer.
    sprite = &gSprites[objectEvent->spriteId];

    if (!(task->data[2] & 3))
        sprite->y2 += task->data[3];

    if (!(task->data[2] & 15))
        task->data[3] = -task->data[3];

    task->data[2]++;
}

static void DestroyLevitateMovementTask(u8 taskId)
{
    struct ObjectEvent *objectEvent;
    struct Task *task = &gTasks[taskId];

    LoadWordFromTwoHalfwords((u16*) &task->data[0], (u32 *)&objectEvent); // unused objectEvent
    DestroyTask(taskId);
}

// Used to freeze other objects except two trainers approaching for battle
void FreezeObjectEventsExceptTwo(u8 objectEventId1, u8 objectEventId2)
{
    u8 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (i != objectEventId1 && i != objectEventId2 &&
            gObjectEvents[i].active && i != gPlayerAvatar.objectEventId)
                FreezeObjectEvent(&gObjectEvents[i]);
    }
}

u8 MovementAction_FlyUp_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    sprite->y2 = 0;
    sprite->sActionFuncId++;
    return FALSE;
}

u8 MovementAction_FlyUp_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    sprite->y2 -= 8;

    if (sprite->y2 == -DISPLAY_HEIGHT)
        sprite->sActionFuncId++;
    return FALSE;
}

u8 MovementAction_FlyDown_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    sprite->y2 = -DISPLAY_HEIGHT;
    sprite->sActionFuncId++;
    return FALSE;
}

u8 MovementAction_FlyDown_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    sprite->y2 += 8;

    if (!sprite->y2)
        sprite->sActionFuncId++;
    return FALSE;
}

// though this function returns TRUE without doing anything, this header is required due to being in an array of functions which needs it.
u8 MovementAction_Fly_Finish(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    return TRUE;
}

bool8 MovementAction_EmoteX_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ObjectEventGetLocalIdAndMap(objectEvent, &gFieldEffectArguments[0], &gFieldEffectArguments[1], &gFieldEffectArguments[2]);
    FieldEffectStart(FLDEFF_X_ICON);
    sprite->sActionFuncId = 1;
    return TRUE;
}

bool8 MovementAction_EmoteDoubleExclamationMark_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ObjectEventGetLocalIdAndMap(objectEvent, &gFieldEffectArguments[0], &gFieldEffectArguments[1], &gFieldEffectArguments[2]);
    FieldEffectStart(FLDEFF_DOUBLE_EXCL_MARK_ICON);
    sprite->sActionFuncId = 1;
    return TRUE;
}

// Get gfx data from daycare Pokémon and store it in vars
void GetDaycareGraphics(struct ScriptContext *ctx)
{
    u16 varGfx[] = {ScriptReadHalfword(ctx), ScriptReadHalfword(ctx)};
    u16 varForm[] = {ScriptReadHalfword(ctx), ScriptReadHalfword(ctx)};
    enum Species specGfx;
    bool32 shiny;
    bool32 female;
    s32 i;

    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(varGfx[0]);
    Script_RequestWriteVar(varGfx[1]);
    Script_RequestWriteVar(varForm[0]);
    Script_RequestWriteVar(varForm[1]);

    for (i = 0; i < 2; i++)
    {
        GetMonInfo((struct Pokemon *) &gSaveBlock1Ptr->daycare.mons[i].mon, &specGfx, &shiny, &female);
        if (specGfx == SPECIES_NONE)
            break;
        // Assemble gfx ID like FollowerSetGraphics
        specGfx = GetGraphicsIdForMon(specGfx, shiny, female);
        VarSet(varGfx[i], (u16)specGfx);
        VarSet(varForm[i], 0);  //  This shouldn't be needed anymore, track down
    }
    gSpecialVar_Result = i;
}

// running slow
static void StartSlowRunningAnim(struct ObjectEvent *objectEvent, struct Sprite *sprite, enum Direction direction)
{
    InitNpcForWalkSlow(objectEvent, sprite, direction);
    SetStepAnimHandleAlternation(objectEvent, sprite, GetRunningDirectionAnimNum(objectEvent->facingDirection));
}

bool8 MovementActionFunc_RunSlowDown_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    StartSlowRunningAnim(objectEvent, sprite, DIR_SOUTH);
    return MovementActionFunc_RunSlow_Step1(objectEvent, sprite);
}

bool8 MovementActionFunc_RunSlowUp_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    StartSlowRunningAnim(objectEvent, sprite, DIR_NORTH);
    return MovementActionFunc_RunSlow_Step1(objectEvent, sprite);
}

bool8 MovementActionFunc_RunSlowLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        StartSlowRunningAnim(objectEvent, sprite, objectEvent->directionOverwrite);
    else
        StartSlowRunningAnim(objectEvent, sprite, DIR_WEST);
    return MovementActionFunc_RunSlow_Step1(objectEvent, sprite);
}

bool8 MovementActionFunc_RunSlowRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        StartSlowRunningAnim(objectEvent, sprite, objectEvent->directionOverwrite);
    else
        StartSlowRunningAnim(objectEvent, sprite, DIR_EAST);
    return MovementActionFunc_RunSlow_Step1(objectEvent, sprite);
}

bool8 MovementActionFunc_RunSlow_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->sActionFuncId = 2;
        return TRUE;
    }
    return FALSE;
}

static bool8 UpdateWalkSlowStairs(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateWalkSlowStairsAnim(sprite))
    {
        ShiftStillObjectEventCoords(objectEvent);
        objectEvent->triggerGroundEffectsOnStop = TRUE;
        sprite->animPaused = TRUE;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_WalkSlowStairsUp_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitWalkSlow(objectEvent, sprite, DIR_NORTH);
    return MovementAction_WalkSlowStairsUp_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkSlowStairsUp_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateWalkSlowStairs(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

// fast diagonal
bool8 MovementAction_WalkFastDiagonalUpLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementNormal(objectEvent, sprite, DIR_NORTHWEST, 1);
    return MovementAction_WalkFastDiagonal_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkFastDiagonalUpRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementNormal(objectEvent, sprite, DIR_NORTHEAST, 1);
    return MovementAction_WalkFastDiagonal_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkFastDiagonalDownLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementNormal(objectEvent, sprite, DIR_SOUTHWEST, 1);
    return MovementAction_WalkFastDiagonal_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkFastDiagonalDownRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementNormal(objectEvent, sprite, DIR_SOUTHEAST, 1);
    return MovementAction_WalkFastDiagonal_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkFastDiagonal_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_WalkSlowStairsDown_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitWalkSlow(objectEvent, sprite, DIR_SOUTH);
    return MovementAction_WalkSlowStairsDown_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkSlowStairsDown_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateWalkSlowStairs(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_WalkSlowStairsLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        InitWalkSlow(objectEvent, sprite, objectEvent->directionOverwrite);
    else
        InitWalkSlow(objectEvent, sprite, DIR_WEST);
    return MovementAction_WalkSlowStairsLeft_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkSlowStairsLeft_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateWalkSlowStairs(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_WalkSlowStairsRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        InitWalkSlow(objectEvent, sprite, objectEvent->directionOverwrite);
    else
        InitWalkSlow(objectEvent, sprite, DIR_EAST);
    return MovementAction_WalkSlowStairsRight_Step1(objectEvent, sprite);
}

bool8 MovementAction_WalkSlowStairsRight_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateWalkSlowStairs(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

u16 GetGraphicsIdForMon(enum Species species, bool32 shiny, bool32 female)
{
    u16 graphicsId = species + OBJ_EVENT_MON;
    if (shiny)
        graphicsId += OBJ_EVENT_MON_SHINY;
    if (female)
        graphicsId += OBJ_EVENT_MON_FEMALE;
    return graphicsId;
}

static enum Species GetUnownSpecies(struct Pokemon *mon)
{
    u32 form = GET_UNOWN_LETTER(mon->box.personality);
    if (form == 0)
        return SPECIES_UNOWN;
    return SPECIES_UNOWN_B + form - 1;
}

static void InitMovementSurfStill(struct ObjectEvent *objectEvent, struct Sprite *sprite, enum Direction direction, u8 speed)
{
    u8 (*functions[ARRAY_COUNT(sDirectionAnimFuncsBySpeed)])(u8);

    memcpy(functions, sDirectionAnimFuncsBySpeed, sizeof sDirectionAnimFuncsBySpeed);
    InitNpcForMovement(objectEvent, sprite, direction, speed);
    ObjectEventTurn(objectEvent, direction);
}

bool8 MovementAction_SurfStillDown_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementSurfStill(objectEvent, sprite, DIR_SOUTH, MOVE_SPEED_FAST_1);
    sprite->animPaused = TRUE;
    return MovementAction_SurfStillDown_Step1(objectEvent, sprite);
}

bool8 MovementAction_SurfStillDown_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->sActionFuncId = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SurfStillUp_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitMovementSurfStill(objectEvent, sprite, DIR_NORTH, MOVE_SPEED_FAST_1);
    sprite->animPaused = TRUE;
    return MovementAction_SurfStillUp_Step1(objectEvent, sprite);
}

bool8 MovementAction_SurfStillUp_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->sActionFuncId = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SurfStillLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        InitMovementSurfStill(objectEvent, sprite, objectEvent->directionOverwrite, MOVE_SPEED_FAST_1);
    else
        InitMovementSurfStill(objectEvent, sprite, DIR_WEST, MOVE_SPEED_FAST_1);
    sprite->animPaused = TRUE;
    return MovementAction_SurfStillLeft_Step1(objectEvent, sprite);
}

bool8 MovementAction_SurfStillLeft_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->sActionFuncId = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SurfStillRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (objectEvent->directionOverwrite)
        InitMovementSurfStill(objectEvent, sprite, objectEvent->directionOverwrite, MOVE_SPEED_FAST_1);
    else
        InitMovementSurfStill(objectEvent, sprite, DIR_EAST, MOVE_SPEED_FAST_1);
    sprite->animPaused = TRUE;
    return MovementAction_SurfStillRight_Step1(objectEvent, sprite);
}

bool8 MovementAction_SurfStillRight_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->sActionFuncId = 2;
        return TRUE;
    }
    return FALSE;
}

u8 GetObjectEventApricornTreeId(u8 objectEventId)
{
    return gObjectEvents[objectEventId].trainerRange_berryTreeId;
}

void InitSpin(struct ObjectEvent *objectEvent, struct Sprite *sprite, u8 direction, u8 speed)
{
    InitNpcForMovement(objectEvent, sprite, direction, speed);
    SetStepAnimHandleAlternation(objectEvent, sprite, GetSpinDirectionAnimNum(objectEvent->facingDirection));
    SeekSpriteAnim(sprite, 0);
}

bool8 MovementAction_SpinDown_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitSpin(objectEvent, sprite, DIR_SOUTH, MOVE_SPEED_FAST_1);
    return MovementAction_SpinDown_Step1(objectEvent, sprite);
}

bool8 MovementAction_SpinDown_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SpinUp_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitSpin(objectEvent, sprite, DIR_NORTH, MOVE_SPEED_FAST_1);
    return MovementAction_SpinUp_Step1(objectEvent, sprite);
}

bool8 MovementAction_SpinUp_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SpinLeft_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitSpin(objectEvent, sprite, DIR_WEST, MOVE_SPEED_FAST_1);
    return MovementAction_SpinLeft_Step1(objectEvent, sprite);
}

bool8 MovementAction_SpinLeft_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_SpinRight_Step0(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    InitSpin(objectEvent, sprite, DIR_EAST, MOVE_SPEED_FAST_1);
    return MovementAction_SpinRight_Step1(objectEvent, sprite);
}

bool8 MovementAction_SpinRight_Step1(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (UpdateMovementNormal(objectEvent, sprite))
    {
        sprite->data[2] = 2;
        return TRUE;
    }
    return FALSE;
}

bool8 MovementAction_OverworldEncounterSpawn(enum SpawnDespawnTypeOWE spawnAnimType, struct ObjectEvent *objEvent)
{
    gFieldEffectArguments[0] = objEvent->currentCoords.x;
    gFieldEffectArguments[1] = objEvent->currentCoords.y;
    gFieldEffectArguments[2] = spawnAnimType;
    FieldEffectStart(FLDEFF_OW_ENCOUNTER_SPAWN_ANIM);
    return TRUE;
}

movement_type_def(MovementType_OverworldWildEncounter_WanderAround, gMovementTypeFuncs_WanderAround_OverworldWildEncounter)

bool8 MovementType_OverworldWildEncounter_WanderAround_Step2(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (!ObjectEventExecSingleMovementAction(objectEvent, sprite))
        return FALSE;
    
    SetMovementDelay(sprite, sMovementDelaysOWE[Random() % ARRAY_COUNT(sMovementDelaysOWE)]);
    sprite->sTypeFuncId = 3;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_WanderAround_Step3(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (WaitForMovementDelay(sprite))
    {
        // resets a mid-movement sprite
        ClearObjectEventMovement(objectEvent, sprite);
        sprite->sTypeFuncId = 4;
        return TRUE;
    }
    
    if (OW_MON_WANDER_WALK == TRUE && IS_OW_MON_OBJ(objectEvent))
        UpdateMonMoveInPlace(objectEvent, sprite);

    if (CanAwareOWESeePlayer(objectEvent))
        sprite->sTypeFuncId = 7;
    
    return FALSE;
}

bool8 MovementType_OverworldWildEncounter_WanderAround_Step4(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction chosenDirection = objectEvent->movementDirection;
    if ((Random() & 3) != 0)
        chosenDirection = GetNinetyDegreeDirection(chosenDirection, Random() % 2);

    SetObjectEventDirection(objectEvent, chosenDirection);
    sprite->sTypeFuncId = 5;
    if (CheckRestrictedOWEMovement(objectEvent, chosenDirection))
        sprite->sTypeFuncId = 1;

    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_WanderAround_Step5(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ObjectEventSetSingleMovement(objectEvent, sprite, GetOWEWalkMovementActionInDirectionWithSpeed(objectEvent->movementDirection, OWE_GetIdleSpeedFromSpecies(OW_SPECIES(objectEvent))));
    objectEvent->singleMovementActive = TRUE;
    sprite->sTypeFuncId = 6;
    return TRUE;
}

movement_type_def(MovementType_OverworldWildEncounter_ChasePlayer, gMovementTypeFuncs_ChasePlayer_OverworldWildEncounter)

bool8 MovementType_OverworldWildEncounter_Common_Step7(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ClearObjectEventMovement(objectEvent, sprite);
    SetSavedOWEMovementState(objectEvent);
    sprite->sTypeFuncId = 8;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_ChasePlayer_Step8(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    if (IsOWENextToPlayer(objectEvent))
    {
        sprite->sTypeFuncId = 10;
        return TRUE;
    }

    ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_EMOTE_EXCLAMATION_MARK);
    PlaySE(SE_PIN);
    sprite->sTypeFuncId = 9;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_Common_Step9(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (ObjectEventExecSingleMovementAction(objectEvent, sprite))
        sprite->sTypeFuncId = 10;
    
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_ChasePlayer_Step10(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 11;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_ChasePlayer_Step11(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Species speciesId = OW_SPECIES(objectEvent);
    u32 movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(objectEvent->movementDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
    sprite->sTypeFuncId = 12;

    if (CheckRestrictedOWEMovement(objectEvent, objectEvent->movementDirection))
    {
        s16 x = objectEvent->currentCoords.x;
        s16 y = objectEvent->currentCoords.y;
        MoveCoords(objectEvent->movementDirection, &x, &y);
        // If colliding with the player object, don't try to walk around it.
        if (GetObjectObjectCollidesWith(objectEvent, x, y, FALSE) == gPlayerAvatar.objectEventId)
        {
            ObjectEventSetSingleMovement(objectEvent, sprite, GetFaceDirectionMovementAction(objectEvent->facingDirection));
            objectEvent->singleMovementActive = TRUE;
            return FALSE;
        }

        enum Direction newDirection = DirectionOfOWEToPlayerFromCollision(objectEvent);
        movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(newDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
        if (CheckRestrictedOWEMovement(objectEvent, newDirection))
            movementActionId = GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection);
    }

    ObjectEventSetSingleMovement(objectEvent, sprite, movementActionId);
    objectEvent->singleMovementActive = TRUE;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_Common_Step12(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (ObjectEventExecSingleMovementAction(objectEvent, sprite))
    {
        objectEvent->singleMovementActive = FALSE;
        sprite->sTypeFuncId = 10;
        bool32 returnToIdle;
        switch(OWE_GetReturnToIdleFromSpecies(OW_SPECIES(objectEvent)))
        {
        case NEVER_RETURN:
            returnToIdle = FALSE;
            break;
        case PLAYER_CANT_BE_SEEN:
            returnToIdle = !CanAwareOWESeePlayer(objectEvent);
            break;
        case PLAYER_OUTSIDE_ACTIVE_RANGE:
        default:
            returnToIdle = !IsPlayerInsideOWEActiveDistance(objectEvent);
            break;
        }
        if (returnToIdle)
        {
            ClearSavedOWEMovementState(objectEvent);
            sprite->sTypeFuncId = 0;
        }
    }
    return FALSE;
}

movement_type_def(MovementType_OverworldWildEncounter_FleePlayer, gMovementTypeFuncs_FleePlayer_OverworldWildEncounter)

bool8 MovementType_OverworldWildEncounter_FleePlayer_Step8(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = GetOppositeDirection(DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent));
    SetObjectEventDirection(objectEvent, direction);
    ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_EMOTE_EXCLAMATION_MARK);
    PlaySE(SE_PIN);
    sprite->sTypeFuncId = 9;
    return TRUE;
}

#define sCollisionTimer     sprite->data[6]

bool8 MovementType_OverworldWildEncounter_FleePlayer_Step10(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (WE_OWE_FLEE_DESPAWN && sCollisionTimer >= OWE_FLEE_COLLISION_TIME && CanRemoveObjectForOWEMovement(objectEvent))
    {
        RemoveObjectEvent(objectEvent);
        return FALSE;
    }

    enum Direction direction = GetOppositeDirection(DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent));
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 11;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_FleePlayer_Step11(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Species speciesId = OW_SPECIES(objectEvent);
    u32 movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(objectEvent->movementDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
    if (CheckRestrictedOWEMovement(objectEvent, objectEvent->movementDirection))
    {
        enum Direction newDirection = DirectionOfOWEToPlayerFromCollision(objectEvent);
        if (newDirection != objectEvent->movementDirection)
            newDirection = GetOppositeDirection(newDirection);
        
        movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(newDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
        if (CheckRestrictedOWEMovement(objectEvent, newDirection))
        {
            sCollisionTimer++;
            movementActionId = GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection);
        }
        else
        {
            sCollisionTimer = 0;
        }
    }
    else
    {
        sCollisionTimer = 0;
    }

    ObjectEventSetSingleMovement(objectEvent, sprite, movementActionId);
    objectEvent->singleMovementActive = TRUE;
    sprite->sTypeFuncId = 12;
    return TRUE;
}

#undef sCollisionTimer

movement_type_def(MovementType_OverworldWildEncounter_WatchPlayer, gMovementTypeFuncs_WatchPlayer_OverworldWildEncounter)

bool8 MovementType_OverworldWildEncounter_WatchPlayer_Step8(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 10;
    if (!IsOWENextToPlayer(objectEvent))
    {
        ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_EMOTE_QUESTION_MARK);
        sprite->sTypeFuncId = 9;
    }
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_WatchPlayer_Step10(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 11;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_WatchPlayer_Step11(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    ObjectEventSetSingleMovement(objectEvent, sprite, GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection));
    objectEvent->singleMovementActive = TRUE;
    sprite->sTypeFuncId = 12;
    return TRUE;
}

movement_type_def(MovementType_OverworldWildEncounter_ApproachPlayer, gMovementTypeFuncs_ApproachPlayer_OverworldWildEncounter)

#define sJumpTimer     sprite->data[7]

bool8 MovementType_OverworldWildEncounter_ApproachPlayer_Step8(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sJumpTimer = RandomUniform(RNG_NONE, OWE_APPROACH_JUMP_TIMER_MIN, OWE_APPROACH_JUMP_TIMER_MAX);
    sprite->sTypeFuncId = 10;
    if (!IsOWENextToPlayer(objectEvent))
    {
        ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_EMOTE_QUESTION_MARK);
        sprite->sTypeFuncId = 9;
    }
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_ApproachPlayer_Step10(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 11;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_ApproachPlayer_Step11(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    bool32 equalDistances = FALSE;
    u32 distance = GetApproachingOWEDistanceToPlayer(objectEvent, &equalDistances);
    enum Species speciesId = OW_SPECIES(objectEvent);
    u32 movementActionId;
    if (distance <= 1)
    {
        SetObjectEventDirection(objectEvent, GetOppositeDirection(objectEvent->movementDirection));
        movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(objectEvent->movementDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
        if (CheckRestrictedOWEMovement(objectEvent, objectEvent->movementDirection))
        {
            struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
            enum Direction newDirection = DirectionOfOWEToPlayerFromCollision(objectEvent);
            if (objectEvent->currentCoords.x != player->currentCoords.x && objectEvent->currentCoords.y != player->currentCoords.y)
                newDirection = GetOppositeDirection(newDirection);

            movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(newDirection, OWE_GetActiveSpeedFromSpecies(speciesId));
            if (CheckRestrictedOWEMovement(objectEvent, newDirection))
                movementActionId = GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection);
        }
    }
    else if (distance == OWE_APPROACH_DISTANCE && !equalDistances)
    {
        if (sJumpTimer <= 0)
        {
            sJumpTimer = RandomUniform(RNG_NONE, OWE_APPROACH_JUMP_TIMER_MIN, OWE_APPROACH_JUMP_TIMER_MAX);
            movementActionId = GetJumpInPlaceMovementAction(objectEvent->facingDirection);
            PlaySE(SE_LEDGE);
        }
        else
        {
            sJumpTimer--;
            movementActionId = GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection);
        }
    }
    else
    {
        movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(objectEvent->movementDirection, OWE_GetActiveSpeedFromSpecies(speciesId));

        if (CheckRestrictedOWEMovement(objectEvent, objectEvent->movementDirection))
        {
            s16 x = objectEvent->currentCoords.x;
            s16 y = objectEvent->currentCoords.y;
            MoveCoords(objectEvent->movementDirection, &x, &y);
            // If colliding with the player object, don't try to walk around it.
            if (GetObjectObjectCollidesWith(objectEvent, x, y, FALSE) == gPlayerAvatar.objectEventId)
            {
                ObjectEventSetSingleMovement(objectEvent, sprite, GetFaceDirectionMovementAction(objectEvent->facingDirection));
                objectEvent->singleMovementActive = TRUE;
                return FALSE;
            }
            enum Direction newDirection = DirectionOfOWEToPlayerFromCollision(objectEvent);
            movementActionId = GetOWEWalkMovementActionInDirectionWithSpeed(newDirection, OWE_GetActiveSpeedFromSpecies(speciesId));

            if (CheckRestrictedOWEMovement(objectEvent, newDirection))
                movementActionId = GetWalkInPlaceNormalMovementAction(objectEvent->facingDirection);
        }

        sJumpTimer = RandomUniform(RNG_NONE, OWE_APPROACH_JUMP_TIMER_MIN, OWE_APPROACH_JUMP_TIMER_MAX);
    }

    ObjectEventSetSingleMovement(objectEvent, sprite, movementActionId);
    objectEvent->singleMovementActive = TRUE;
    sprite->sTypeFuncId = 12;
    return TRUE;
}

movement_type_def(MovementType_OverworldWildEncounter_Despawn, gMovementTypeFuncs_Despawn_OverworldWildEncounter)

#define sDespawnTimer     sprite->data[6]

bool8 MovementType_OverworldWildEncounter_Despawn_Step8(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    ObjectEventSetSingleMovement(objectEvent, sprite, MOVEMENT_ACTION_EMOTE_EXCLAMATION_MARK);
    PlaySE(SE_PIN);
    sDespawnTimer = 0;
    sprite->sTypeFuncId = 9;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_Despawn_Step10(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    enum Direction direction = DetermineObjectEventDirectionFromObject(&gObjectEvents[gPlayerAvatar.objectEventId], objectEvent);
    SetObjectEventDirection(objectEvent, direction);
    sprite->sTypeFuncId = 11;
    return TRUE;
}

bool8 MovementType_OverworldWildEncounter_Despawn_Step11(struct ObjectEvent *objectEvent, struct Sprite *sprite)
{
    if (sDespawnTimer == OWE_DESPAWN_FRAMES && CanRemoveObjectForOWEMovement(objectEvent))
    {
        RemoveObjectEvent(objectEvent);
        return FALSE;
    }

    ObjectEventSetSingleMovement(objectEvent, sprite, GetFaceDirectionMovementAction(objectEvent->facingDirection));
    objectEvent->singleMovementActive = TRUE;
    sDespawnTimer++;
    sprite->sTypeFuncId = 12;
    return TRUE;
}

#undef sDespawnTimer
