/*
 * PRL FULL Hoenn's Last Wish Scene 0 trial (isolated).
 * Source adapted from rafaelsanna/HOENN-S-LAST-WISH-project
 * pinned commit 8f9c28e5437d9e37ed3f6873bb8dd54dac78ccf3.
 * Uses real donor shrine/forest/cloud/moon/comet assets and replaces the
 * the original Celebi/Jirachi sprite sheets and palettes without substitutions.
 * Keeps the original forest visuals and timing unchanged.
 * Porygon splash and canonical title remain unchanged outside this branch.
 */
#include "global.h"
#include "bg.h"
#include "gpu_regs.h"
#include "main.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "decompress.h"
#include "scanline_effect.h"
#include "title_screen.h"
#include "intro_credits_graphics.h"
#include "graphics.h"
#include "sound.h"
#include "m4a.h"
#include "random.h"
#include "trig.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#if defined(FIRERED)
#define TAG_SCENE0_SHRINE 1600
#define TAG_SCENE0_CELEBI 1601
#define TAG_SCENE0_JIRACHI 1602
#define TAG_SCENE0_MOON 1603
#define TAG_SCENE0_COMET 1604
#define TAG_SCENE0_SPARKLE 1605
#define TAG_SCENE0_CELEBI2 1606
#define TAG_SCENE0_JIRACHI2 1607
// Original forest, shrine, cloud, moon and comet imagery from pinned HWL Scene 0.
// PRL's INCGFX pipeline converts each source PNG into GBA 4bpp compressed tiles.
static const u16 sScene0BgSky00_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bgsky00.png", ".gbapal");
static const u32 sScene0BgSky00_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bgsky00.png", ".4bpp.smol");
static const u16 sScene0BgSky00_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bgsky00.bin");
static const u16 sScene0Bg00_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bg00.png", ".gbapal");
static const u32 sScene0Bg00_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bg00.png", ".4bpp.smol");
static const u16 sScene0Bg00_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bg00.bin");
static const u16 sScene0BgSky01_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bgsky01.png", ".gbapal");
static const u32 sScene0BgSky01_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bgsky01.png", ".4bpp.smol");
static const u16 sScene0BgSky01_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bgsky01.bin");
static const u16 sScene0Bg01_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bg01.png", ".gbapal");
static const u32 sScene0Bg01_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bg01.png", ".4bpp.smol");
static const u16 sScene0Bg01_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bg01.bin");
static const u16 sScene0BgSky02_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bgsky02.png", ".gbapal");
static const u32 sScene0BgSky02_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bgsky02.png", ".4bpp.smol");
static const u16 sScene0BgSky02_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bgsky02.bin");
static const u16 sScene0Bg02_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bg02.png", ".gbapal");
static const u32 sScene0Bg02_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bg02.png", ".4bpp.smol");
static const u16 sScene0Bg02_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bg02.bin");
static const u16 sScene0Bg03_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/bg03.png", ".gbapal");
static const u32 sScene0Bg03_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/bg03.png", ".4bpp.smol");
static const u16 sScene0Bg03_Map[] = INCBIN_U16("graphics/intro/prl_hwl/bg03.bin");
static const u16 sScene0Clouds_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/clouds.png", ".gbapal");
static const u32 sScene0Clouds_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/clouds.png", ".4bpp.smol");
static const u16 sScene0Clouds_Map[] = INCBIN_U16("graphics/intro/prl_hwl/clouds.bin");
static const u16 sScene0Shrine_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/shrine.png", ".gbapal");
static const u32 sScene0Shrine_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/shrine.png", ".4bpp.smol");
static const u16 sScene0Moon_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/moon.png", ".gbapal");
static const u32 sScene0Moon_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/moon.png", ".4bpp.smol");
static const u16 sScene0Comet_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/comet.png", ".gbapal");
static const u32 sScene0Comet_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/comet.png", ".4bpp.smol");
static const u16 sScene0Celebi_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/celebi.png", ".gbapal");
static const u32 sScene0Celebi_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/celebi.png", ".4bpp.smol");
static const u16 sScene0Jirachi_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/jirachi.png", ".gbapal");
static const u32 sScene0Jirachi_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/jirachi.png", ".4bpp.smol");
static const u32 sScene0Celebi2_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/celebi2.png", ".4bpp.smol");
static const u16 sScene0Celebi2_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/celebi2.png", ".gbapal");
static const u32 sScene0Jirachi2_Gfx[] = INCGFX_U32("graphics/intro/prl_hwl/jirachi2.png", ".4bpp.smol");
static const u16 sScene0Jirachi2_Pal[] = INCGFX_U16("graphics/intro/prl_hwl/jirachi2.png", ".gbapal");

static void CB2_PRLHwlScene0(void);
static void PRL_Scene0ResetGpuRegs(void);
static void Task_Scene0_Main(u8 taskId);
static void Task_Scene0_Load(u8 taskId);
static void Scene0_LoadBgSet(u8 setIndex);
static void Scene0_LoadSet03(void);
static void SpriteCB_Scene0Shrine(struct Sprite *sprite);
static void SpriteCB_Scene0Moon(struct Sprite *sprite);
static void SpriteCB_Scene0Celebi(struct Sprite *sprite);
static void SpriteCB_Scene0Jirachi(struct Sprite *sprite);
static void SpriteCB_Scene0Comet(struct Sprite *sprite);
static void SpriteCB_Scene0Sparkle(struct Sprite *sprite);
static void SpriteCB_Scene0Celebi2(struct Sprite *sprite);
static void SpriteCB_Scene0Jirachi2(struct Sprite *sprite);

static void PRL_Scene0ResetGpuRegs(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    ScanlineEffect_Stop();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFade();
    DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
    DmaFill16(3, 0, (void *)PLTT, PLTT_SIZE);
}
static void VBlankCB_PRLHwlScene0(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}
static void CB2_PRLHwlScene0(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}
// =========================================================================
// Scene 0 — sprite data tables
// =========================================================================

// ---------------------------------------------------------------------------
// Shrine — static 64×64, sits in front of the forest BG layer
//   OAM priority 0 so it renders above BG1 (forest, BG priority 0)
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Shrine =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,   // in front of all BGs
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_Scene0Shrine[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_Scene0Shrine[] =
{
    sAnim_Scene0Shrine,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Shrine[] =
{
    // 64×64 px, 4bpp = 2048 bytes (0x800)
    {sScene0Shrine_Gfx, 0x800, TAG_SCENE0_SHRINE},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Shrine[] =
{
    {sScene0Shrine_Pal, TAG_SCENE0_SHRINE},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Shrine =
{
    .tileTag = TAG_SCENE0_SHRINE,
    .paletteTag = TAG_SCENE0_SHRINE,
    .oam = &sOamData_Scene0Shrine,
    .anims = sAnims_Scene0Shrine,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Scene0Shrine,
};

// ---------------------------------------------------------------------------
// Celebi — 4 frames × 64×64, fly animation (same layout as HWL logo sprites)
//   Sheet: 64 px wide × 256 px tall (4 frames stacked vertically)
//   Each frame tile offset: 0, 64, 128, 192 (in 8×8 tile units)
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Celebi =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_Scene0Celebi_Fly[] =
{
    ANIMCMD_FRAME(0,   20),
    ANIMCMD_FRAME(64,  20),
    ANIMCMD_FRAME(128, 20),
    ANIMCMD_FRAME(192, 20),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Celebi[] =
{
    sAnim_Scene0Celebi_Fly,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Celebi[] =
{
    // 4 frames × 64×64 px, 4bpp = 4 × 2048 = 8192 bytes (0x2000)
    {sScene0Celebi_Gfx, 0x2000, TAG_SCENE0_CELEBI},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Celebi[] =
{
    {sScene0Celebi_Pal, TAG_SCENE0_CELEBI},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Celebi =
{
    .tileTag = TAG_SCENE0_CELEBI,
    .paletteTag = TAG_SCENE0_CELEBI,
    .oam = &sOamData_Scene0Celebi,
    .anims = sAnims_Scene0Celebi,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Scene0Celebi,
};

// ---------------------------------------------------------------------------
// Jirachi — same structure as Celebi
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Jirachi =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_Scene0Jirachi_Fly[] =
{
    ANIMCMD_FRAME(0,   20),
    ANIMCMD_FRAME(64,  20),
    ANIMCMD_FRAME(128, 20),
    ANIMCMD_FRAME(192, 20),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Jirachi[] =
{
    sAnim_Scene0Jirachi_Fly,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Jirachi[] =
{
    {sScene0Jirachi_Gfx, 0x2000, TAG_SCENE0_JIRACHI},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Jirachi[] =
{
    {sScene0Jirachi_Pal, TAG_SCENE0_JIRACHI},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Jirachi =
{
    .tileTag = TAG_SCENE0_JIRACHI,
    .paletteTag = TAG_SCENE0_JIRACHI,
    .oam = &sOamData_Scene0Jirachi,
    .anims = sAnims_Scene0Jirachi,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Scene0Jirachi,
};

// ---------------------------------------------------------------------------
// Celebi2 — segundo voo: de baixo pra cima, vindo da esquerda da lua
// priority=1: frente do bg03, atras das nuvens (BG1 prio=0)
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Celebi2 =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_NORMAL,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(64x64),
    .size       = SPRITE_SIZE(64x64),
    .priority   = 1,   // atras das nuvens, frente do ceu
};
static const union AnimCmd sAnim_Scene0Celebi2_Fly[] =
{
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Celebi2[] =
{
    sAnim_Scene0Celebi2_Fly,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Celebi2[] =
{
    {sScene0Celebi2_Gfx, 0x2000, TAG_SCENE0_CELEBI2},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Celebi2[] =
{
    {sScene0Celebi2_Pal, TAG_SCENE0_CELEBI2},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Celebi2 =
{
    .tileTag     = TAG_SCENE0_CELEBI2,
    .paletteTag  = TAG_SCENE0_CELEBI2,
    .oam         = &sOamData_Scene0Celebi2,
    .anims       = sAnims_Scene0Celebi2,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCB_Scene0Celebi2,
};

// ---------------------------------------------------------------------------
// Jirachi2 — segundo voo: de baixo pra cima, vindo da direita da lua
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Jirachi2 =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_NORMAL,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(64x64),
    .size       = SPRITE_SIZE(64x64),
    .priority   = 0,
};
static const union AnimCmd sAnim_Scene0Jirachi2_Fly[] =
{
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Jirachi2[] =
{
    sAnim_Scene0Jirachi2_Fly,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Jirachi2[] =
{
    {sScene0Jirachi2_Gfx, 0x2000, TAG_SCENE0_JIRACHI2},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Jirachi2[] =
{
    {sScene0Jirachi2_Pal, TAG_SCENE0_JIRACHI2},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Jirachi2 =
{
    .tileTag     = TAG_SCENE0_JIRACHI2,
    .paletteTag  = TAG_SCENE0_JIRACHI2,
    .oam         = &sOamData_Scene0Jirachi2,
    .anims       = sAnims_Scene0Jirachi2,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCB_Scene0Jirachi2,
};

// ---------------------------------------------------------------------------
// Moon — sprite 64x64 centralizado, glow estatico
// ---------------------------------------------------------------------------
// Moon — OAM com affine normal para o grow manual via SetOamMatrix
// O grow é controlado diretamente no SpriteCB_Scene0Moon (data[1] = escala PA).
// SetOamMatrix(matrixNum, PA, PB, PC, PD):
//   PA=PD=256 → 1:1 (tamanho real). PA=PD=768 → 1/3 do tamanho.
//   Decrementamos PA/PD de 768→256 a 4px/frame = ~128 frames (~2.1s) de crescimento.
static const struct OamData sOamData_Scene0Moon =
{
    .y          = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_NORMAL, // affinematrix controlada manualmente
    .objMode    = ST_OAM_OBJ_BLEND,    // hardware lighten glow (BLDY)
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(64x64),
    .size       = SPRITE_SIZE(64x64),
    .matrixNum  = 0,
    .priority   = 1,
};
static const union AnimCmd sAnim_Scene0Moon[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Moon[] =
{
    sAnim_Scene0Moon,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Moon[] =
{
    {sScene0Moon_Gfx, 0x800, TAG_SCENE0_MOON},   // 1 frame 64x64 = 2048 bytes
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Moon[] =
{
    {sScene0Moon_Pal, TAG_SCENE0_MOON},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Moon =
{
    .tileTag     = TAG_SCENE0_MOON,
    .paletteTag  = TAG_SCENE0_MOON,
    .oam         = &sOamData_Scene0Moon,
    .anims       = sAnims_Scene0Moon,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable, // grow é manual, não via affine anim
    .callback    = SpriteCB_Scene0Moon,
};

// ---------------------------------------------------------------------------
// Cometa — 4 frames 64x64, animacao rapida, cruza diagonal direita->esquerda
//   Sheet: 4 × 64×64 × 4bpp = 4 × 2048 = 8192 = 0x2000 bytes
//   Tile offsets: 0, 64, 128, 192
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Comet =
{
    .y          = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_NORMAL,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(64x64),
    .size       = SPRITE_SIZE(64x64),
    .priority   = 0,
};
static const union AnimCmd sAnim_Scene0Comet[] =
{
    ANIMCMD_FRAME(  0, 3),   // rapido: 3 frames cada = ~20fps
    ANIMCMD_FRAME( 64, 3),
    ANIMCMD_FRAME(128, 3),
    ANIMCMD_FRAME(192, 3),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_Scene0Comet[] =
{
    sAnim_Scene0Comet,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Comet[] =
{
    {sScene0Comet_Gfx, 0x2000, TAG_SCENE0_COMET},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Comet[] =
{
    {sScene0Comet_Pal, TAG_SCENE0_COMET},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Comet =
{
    .tileTag     = TAG_SCENE0_COMET,
    .paletteTag  = TAG_SCENE0_COMET,
    .oam         = &sOamData_Scene0Comet,
    .anims       = sAnims_Scene0Comet,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCB_Scene0Comet,
};

// ---------------------------------------------------------------------------
// Sparkle — reutiliza gIntroSparkle_Gfx (ja compilado no jogo)
//   Aparece em posicoes aleatorias durante a passagem do cometa
// ---------------------------------------------------------------------------
static const struct OamData sOamData_Scene0Sparkle =
{
    .y          = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode    = ST_OAM_OBJ_NORMAL,
    .bpp        = ST_OAM_4BPP,
    .shape      = SPRITE_SHAPE(16x32),
    .size       = SPRITE_SIZE(16x32),
    .priority   = 0,
};
static const union AnimCmd sAnim_Scene0Sparkle[] =
{
    ANIMCMD_FRAME( 0, 4),
    ANIMCMD_FRAME( 8, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(24, 4),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_Scene0Sparkle[] =
{
    sAnim_Scene0Sparkle,
};
static const struct CompressedSpriteSheet sSpriteSheet_Scene0Sparkle[] =
{
    {gIntroSparkle_Gfx, 0x400, TAG_SCENE0_SPARKLE},
    {},
};
static const struct SpritePalette sSpritePalette_Scene0Sparkle[] =
{
    {gIntroLightning_Pal, TAG_SCENE0_SPARKLE},
    {},
};
static const struct SpriteTemplate sSpriteTemplate_Scene0Sparkle =
{
    .tileTag     = TAG_SCENE0_SPARKLE,
    .paletteTag  = TAG_SCENE0_SPARKLE,
    .oam         = &sOamData_Scene0Sparkle,
    .anims       = sAnims_Scene0Sparkle,
    .images      = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback    = SpriteCB_Scene0Sparkle,
};

// Task data layout for Task_Scene0_Main:
//   data[0] = tState
//   data[1] = tTimer        — contador generico de frames
//   data[2] = tBgSet        — BG set atual (0/1/2/3)
//   data[3] = tShrineId     — sprite do shrine (set 0/1/2)
//   data[4] = tMoonId       — sprite da lua (set 3)
//   data[5] = tCelebiId
//   data[6] = tJirachiId
//   data[7] = tCometId
//   data[8] = tCloudScroll  — sub-pixel scroll das nuvens (1/2 pixel por frame)
//   data[9] = tSparkleTimer — frame counter para spawn de sparkles
// =========================================================================
#define tState        data[0]
#define tTimer        data[1]
#define tBgSet        data[2]
#define tShrineId     data[3]
#define tMoonId       data[4]
#define tCelebiId     data[5]
#define tJirachiId    data[6]
#define tCometId      data[7]
#define tCloudScroll  data[8]
#define tSparkleTimer data[9]

enum {
    S0_FADE_IN_SET00 = 0,
    S0_HOLD_SET00,
    S0_FADE_TO_SET01,
    S0_HOLD_SET01,
    S0_FADE_TO_SET02,
    S0_HOLD_SET02,
    S0_LAUNCH_SPRITES,     // Jirachi + Celebi voam (com cry)
    S0_WAIT_SPRITES,
    S0_FADE_TO_SET03,      // tela preta -> carrega bg03 + clouds + moon
    S0_HOLD_SET03,         // fade in do set03, nuvens comecam a mover
    S0_LAUNCH_COMET,       // cometa cruza + sparkles espalhados
    S0_WAIT_COMET,
    S0_LAUNCH_SPRITES2,    // celebi2 + jirachi2 voam pelo ceu noturno
    S0_WAIT_SPRITES2,
    S0_FADE_OUT,
    S0_SKIP_FADE,
    S0_DONE,
};

#define S0_SKY_CHARBASE    0
#define S0_FOREST_CHARBASE 1
#define S0_SKY_SCREEN      24
#define S0_FOREST_SCREEN   28
#define S0_SKY_PAL_SLOT    0
#define S0_FOREST_PAL_SLOT 1

// =========================================================================
// Scene0_LoadBgSet — sets 0, 1, 2 (floresta + ceu)
// =========================================================================
static void Scene0_LoadBgSet(u8 setIndex)
{
    const u16 *skyPal, *fgPal;
    const u32 *skyGfx, *fgGfx;
    const void *skyMap, *fgMap;

    switch (setIndex)
    {
    default:
    case 0:
        skyPal = sScene0BgSky00_Pal; fgPal = sScene0Bg00_Pal;
        skyGfx = sScene0BgSky00_Gfx; fgGfx = sScene0Bg00_Gfx;
        skyMap = sScene0BgSky00_Map; fgMap = sScene0Bg00_Map;
        break;
    case 1:
        skyPal = sScene0BgSky01_Pal; fgPal = sScene0Bg01_Pal;
        skyGfx = sScene0BgSky01_Gfx; fgGfx = sScene0Bg01_Gfx;
        skyMap = sScene0BgSky01_Map; fgMap = sScene0Bg01_Map;
        break;
    case 2:
        skyPal = sScene0BgSky02_Pal; fgPal = sScene0Bg02_Pal;
        skyGfx = sScene0BgSky02_Gfx; fgGfx = sScene0Bg02_Gfx;
        skyMap = sScene0BgSky02_Map; fgMap = sScene0Bg02_Map;
        break;
    }

    DecompressDataWithHeaderVram(skyGfx, (void *)(BG_CHAR_ADDR(S0_SKY_CHARBASE)));
    DecompressDataWithHeaderVram(fgGfx,  (void *)(BG_CHAR_ADDR(S0_FOREST_CHARBASE)));
    CpuFill16(0, (void *)(BG_SCREEN_ADDR(S0_SKY_SCREEN)),    BG_SCREEN_SIZE);
    CpuFill16(0, (void *)(BG_SCREEN_ADDR(S0_FOREST_SCREEN)), BG_SCREEN_SIZE);
    CpuCopy16(skyMap, (void *)(BG_SCREEN_ADDR(S0_SKY_SCREEN)),    32 * 20 * 2);
    CpuCopy16(fgMap,  (void *)(BG_SCREEN_ADDR(S0_FOREST_SCREEN)), 32 * 20 * 2);
    {
        u16 *screenSky = (u16 *)BG_SCREEN_ADDR(S0_SKY_SCREEN);
        u16 *screenFg  = (u16 *)BG_SCREEN_ADDR(S0_FOREST_SCREEN);
        u16 i;
        for (i = 0; i < 32 * 32; i++)
            screenSky[i] = (screenSky[i] & 0x0FFF) | (0 << 12);
        for (i = 0; i < 32 * 32; i++)
            screenFg[i]  = (screenFg[i]  & 0x0FFF) | (1 << 12);
    }
    LoadPalette(skyPal, BG_PLTT_ID(S0_SKY_PAL_SLOT),    PLTT_SIZE_4BPP);
    LoadPalette(fgPal,  BG_PLTT_ID(S0_FOREST_PAL_SLOT), PLTT_SIZE_4BPP);
}

// =========================================================================
// Scene0_LoadSet03 — bg03 (ceu noturno) + clouds (overlay com scroll)
// Chamado enquanto a tela esta preta (dentro de S0_FADE_TO_SET03)
// BG0 = bg03, BG1 = clouds
// =========================================================================
static void Scene0_LoadSet03(void)
{
    // bg03 — ceu noturno com lua: charbase 0, screenbase 24
    DecompressDataWithHeaderVram(sScene0Bg03_Gfx, (void *)(BG_CHAR_ADDR(S0_SKY_CHARBASE)));
    CpuFill16(0, (void *)(BG_SCREEN_ADDR(S0_SKY_SCREEN)), BG_SCREEN_SIZE);
    CpuCopy16(sScene0Bg03_Map, (void *)(BG_SCREEN_ADDR(S0_SKY_SCREEN)), 32 * 20 * 2);
    LoadPalette(sScene0Bg03_Pal, BG_PLTT_ID(S0_SKY_PAL_SLOT), PLTT_SIZE_4BPP);

    // clouds — overlay semitransparente: charbase 1, screenbase 28
    DecompressDataWithHeaderVram(sScene0Clouds_Gfx, (void *)(BG_CHAR_ADDR(S0_FOREST_CHARBASE)));
    CpuFill16(0, (void *)(BG_SCREEN_ADDR(S0_FOREST_SCREEN)), BG_SCREEN_SIZE);
    CpuCopy16(sScene0Clouds_Map, (void *)(BG_SCREEN_ADDR(S0_FOREST_SCREEN)), 32 * 20 * 2);
    LoadPalette(sScene0Clouds_Pal, BG_PLTT_ID(S0_FOREST_PAL_SLOT), PLTT_SIZE_4BPP);

    SetGpuReg(REG_OFFSET_BG0CNT,
        BGCNT_PRIORITY(1)
        | BGCNT_CHARBASE(S0_SKY_CHARBASE)
        | BGCNT_SCREENBASE(S0_SKY_SCREEN)
        | BGCNT_16COLOR
        | BGCNT_TXT256x256);
    SetGpuReg(REG_OFFSET_BG1CNT,
        BGCNT_PRIORITY(0)
        | BGCNT_CHARBASE(S0_FOREST_CHARBASE)
        | BGCNT_SCREENBASE(S0_FOREST_SCREEN)
        | BGCNT_16COLOR
        | BGCNT_TXT256x256);

    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
}

// =========================================================================
// Falling stars para bg03 — sistema idêntico ao Birch, tags separadas
// Prioridade: lua=1, estrelas=2, nuvens=0 (nuvem na frente de tudo)
// =========================================================================
#define NUM_S0_STARS      25
#define S0_STAR_TAG       1610

static u8 sS0StarSpriteIds[NUM_S0_STARS];

// Tiles idênticos ao Birch (medium e large, 8x8)
static const u32 sS0StarTiles[][8] = {
    { 0x00000000, 0x00010000, 0x00111000, 0x00010000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00010000, 0x00010000, 0x00111000, 0x01111100,
      0x00111000, 0x00010000, 0x00010000, 0x00000000 },
};
static const u16 sS0StarPal0[4] = { RGB(0,0,0), RGB( 7, 9,14), RGB(0,0,0), RGB(0,0,0) };
static const u16 sS0StarPal1[4] = { RGB(0,0,0), RGB(14,16,22), RGB(0,0,0), RGB(0,0,0) };
static const u16 sS0StarPal2[4] = { RGB(0,0,0), RGB(22,24,29), RGB(0,0,0), RGB(0,0,0) };
static const u16 sS0StarPal3[4] = { RGB(0,0,0), RGB(31,31,31), RGB(0,0,0), RGB(0,0,0) };
static const u16 sS0StarPalDummy[4] = { RGB(0,0,0), RGB(31,31,31), RGB(0,0,0), RGB(0,0,0) };

static const struct SpriteSheet sS0StarSheet = {
    .data = sS0StarTiles,
    .size = sizeof(sS0StarTiles),
    .tag  = S0_STAR_TAG,
};
static const struct SpritePalette sS0StarPalette = {
    .data = sS0StarPalDummy,
    .tag  = S0_STAR_TAG,
};
static const struct OamData sOamData_S0Star = {
    .shape    = SPRITE_SHAPE(8x8),
    .size     = SPRITE_SIZE(8x8),
    .priority = 1,  
};
static const union AnimCmd sAnim_S0StarMedium[] = { ANIMCMD_FRAME(0, 0), ANIMCMD_END };
static const union AnimCmd sAnim_S0StarLarge[]  = { ANIMCMD_FRAME(1, 0), ANIMCMD_END };
static const union AnimCmd *const sAnims_S0Star[] = { sAnim_S0StarMedium, sAnim_S0StarLarge };
static const struct SpriteTemplate sSpriteTemplate_S0Star = {
    .tileTag    = S0_STAR_TAG,
    .paletteTag = S0_STAR_TAG,
    .oam        = &sOamData_S0Star,
    .anims      = sAnims_S0Star,
    .callback   = SpriteCallbackDummy,
};

static void Scene0_CreateStars(void)
{
    LoadSpriteSheet(&sS0StarSheet);
    LoadSpritePalette(&sS0StarPalette);
    // Paletas de brilho nos slots OBJ 4–7 (longe dos slots do cometa/lua/sparkle)
    LoadPalette(sS0StarPal0, OBJ_PLTT_ID(4), sizeof(sS0StarPal0));
    LoadPalette(sS0StarPal1, OBJ_PLTT_ID(5), sizeof(sS0StarPal1));
    LoadPalette(sS0StarPal2, OBJ_PLTT_ID(6), sizeof(sS0StarPal2));
    LoadPalette(sS0StarPal3, OBJ_PLTT_ID(7), sizeof(sS0StarPal3));

    for (int i = 0; i < NUM_S0_STARS; i++)
    {
        u8 id = CreateSprite(&sSpriteTemplate_S0Star,
                             Random2() % DISPLAY_WIDTH,
                             Random2() % DISPLAY_HEIGHT, 2);
        sS0StarSpriteIds[i] = (id == MAX_SPRITES) ? SPRITE_NONE : id;
        if (id == MAX_SPRITES) continue;

        u8 sizeType = (Random2() % 10 < 6) ? 0 : 1;
        StartSpriteAnim(&gSprites[id], sizeType);
        gSprites[id].data[0] = (s16)(Random2() & 0xFF);   // fase inicial
        gSprites[id].data[1] = 0;                          // contador de pulso
        gSprites[id].data[2] = (sizeType == 0) ? 1 : 2;   // velocidade de queda
        gSprites[id].data[3] = 0;                          // sub-counter de queda
        gSprites[id].data[4] = (s16)sizeType;              // tamanho (para paleta)
    }
}

static void Scene0_UpdateStars(void)
{
    for (int i = 0; i < NUM_S0_STARS; i++)
    {
        if (sS0StarSpriteIds[i] == SPRITE_NONE) continue;
        struct Sprite *spr = &gSprites[sS0StarSpriteIds[i]];
        if (!spr->inUse) continue;

        // Queda lenta (idêntica ao Birch)
        spr->data[3]++;
        if (spr->data[3] >= spr->data[2] * 4)
        {
            spr->y += 1;
            spr->data[3] = 0;
        }
        if (spr->y > DISPLAY_HEIGHT + 16)
        {
            spr->y = -8;
            spr->x = Random2() % DISPLAY_WIDTH;
        }

        // Pulso de brilho (idêntico ao Birch)
        spr->data[1] = (spr->data[1] + 1) & 0xFF;
        u8 phase   = (u8)((spr->data[1] + spr->data[0]) & 0xFF);
        u8 tv      = (phase < 128) ? phase : (u8)(255 - phase);
        u8 hiPhase = (tv >= 64) ? 1 : 0;
        u8 depth   = (u8)spr->data[4];
        spr->oam.paletteNum = 4 + depth + hiPhase;  // slots 4-7 (não colide com Birch 9-11)
    }
}

static void Scene0_DestroyStars(void)
{
    FreeSpriteTilesByTag(S0_STAR_TAG);
    FreeSpritePaletteByTag(S0_STAR_TAG);
    for (int i = 0; i < NUM_S0_STARS; i++)
    {
        if (sS0StarSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sS0StarSpriteIds[i]]);
            sS0StarSpriteIds[i] = SPRITE_NONE;
        }
    }
}

// =========================================================================
// Task_Scene0_Load
// =========================================================================
void Task_Scene0_Load(u8 taskId)
{
    SetVBlankCallback(NULL);
    m4aSongNumStart(MUS_INTRO);
    PRL_Scene0ResetGpuRegs();

    // Blend hardware — TGT1=OBJ, EFFECT=LIGHTEN (BLDY).
    // Ativo durante toda a Scene0; BLDY é controlado individualmente
    // pela lua (pulso lento) e pelo cometa (flash rápido).
    // Zerado em S0_DONE antes de passar para a Scene1.
    SetGpuReg(REG_OFFSET_BLDCNT,
        BLDCNT_TGT1_OBJ
        | BLDCNT_EFFECT_LIGHTEN);
    SetGpuReg(REG_OFFSET_BLDY, 0);

    SetGpuReg(REG_OFFSET_BG0CNT,
        BGCNT_PRIORITY(1)
        | BGCNT_CHARBASE(S0_SKY_CHARBASE)
        | BGCNT_SCREENBASE(S0_SKY_SCREEN)
        | BGCNT_16COLOR
        | BGCNT_TXT256x256);
    SetGpuReg(REG_OFFSET_BG1CNT,
        BGCNT_PRIORITY(0)
        | BGCNT_CHARBASE(S0_FOREST_CHARBASE)
        | BGCNT_SCREENBASE(S0_FOREST_SCREEN)
        | BGCNT_16COLOR
        | BGCNT_TXT256x256);

    Scene0_LoadBgSet(0);

    // Sprites do set 0/1/2
    LoadCompressedSpriteSheet(sSpriteSheet_Scene0Shrine);
    LoadSpritePalettes(sSpritePalette_Scene0Shrine);
    LoadCompressedSpriteSheet(sSpriteSheet_Scene0Celebi);
    LoadSpritePalettes(sSpritePalette_Scene0Celebi);
    LoadCompressedSpriteSheet(sSpriteSheet_Scene0Jirachi);
    LoadSpritePalettes(sSpritePalette_Scene0Jirachi);

    gTasks[taskId].tShrineId    = CreateSprite(&sSpriteTemplate_Scene0Shrine,
                                               DISPLAY_WIDTH / 2,
                                               DISPLAY_HEIGHT - 40, 0);
    for (u8 i = 0; i < NUM_S0_STARS; ++i) sS0StarSpriteIds[i] = SPRITE_NONE;
    gTasks[taskId].tMoonId      = SPRITE_NONE;
    gTasks[taskId].tCelebiId    = SPRITE_NONE;
    gTasks[taskId].tJirachiId   = SPRITE_NONE;
    gTasks[taskId].tCometId     = SPRITE_NONE;
    gTasks[taskId].tState       = S0_FADE_IN_SET00;
    gTasks[taskId].tTimer       = 0;
    gTasks[taskId].tBgSet       = 0;
    gTasks[taskId].tCloudScroll = 0;
    gTasks[taskId].tSparkleTimer = 0;

    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(VBlankCB_PRLHwlScene0);
    SetGpuReg(REG_OFFSET_DISPCNT,
        DISPCNT_MODE_0
        | DISPCNT_OBJ_1D_MAP
        | DISPCNT_BG0_ON
        | DISPCNT_BG1_ON
        | DISPCNT_OBJ_ON);

    gTasks[taskId].func = Task_Scene0_Main;
}

// =========================================================================
// Task_Scene0_Main
// =========================================================================
static void Task_Scene0_Main(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    // B/START/A skip: fade safely back to the approved PRL title.
    if (gMain.newKeys != 0 && task->tState != S0_SKIP_FADE && task->tState != S0_DONE)
    {
        BeginNormalPaletteFade(PALETTES_ALL, 3, 0, 16, RGB_BLACK);
        task->tState = S0_SKIP_FADE;
    }
    switch (task->tState)
    {
    // ------------------------------------------------------------------
    case S0_FADE_IN_SET00:
        if (!gPaletteFade.active)
        {
            task->tTimer = 0;
            task->tState = S0_HOLD_SET00;
        }
        break;

    // ------------------------------------------------------------------
    case S0_HOLD_SET00:
        if (++task->tTimer >= 70)   // ~1.2 s: faster forest opening
        {
            BeginNormalPaletteFade(PALETTES_BG, 8, 0, 16, RGB_BLACK);
            task->tTimer = 0;
            task->tState = S0_FADE_TO_SET01;
        }
        break;

    // ------------------------------------------------------------------
    case S0_FADE_TO_SET01:
        if (!gPaletteFade.active)
        {
            Scene0_LoadBgSet(1);
            task->tBgSet = 1;
            BeginNormalPaletteFade(PALETTES_BG, 8, 16, 0, RGB_BLACK);
            task->tTimer = 0;
            task->tState = S0_HOLD_SET01;
        }
        break;

    // ------------------------------------------------------------------
    case S0_HOLD_SET01:
        if (gPaletteFade.active)
            break;
        if (++task->tTimer >= 65)   // ~1.1 s: faster transition
        {
            BeginNormalPaletteFade(PALETTES_BG, 8, 0, 16, RGB_BLACK);
            task->tTimer = 0;
            task->tState = S0_FADE_TO_SET02;
        }
        break;

    // ------------------------------------------------------------------
    case S0_FADE_TO_SET02:
        if (!gPaletteFade.active)
        {
            Scene0_LoadBgSet(2);

            task->tBgSet = 2;
            BeginNormalPaletteFade(PALETTES_BG, 8, 16, 0, RGB_BLACK);
            task->tTimer = 0;
            task->tState = S0_HOLD_SET02;
        }
        break;

    // ------------------------------------------------------------------
    case S0_HOLD_SET02:
        if (gPaletteFade.active)
            break;
        if (++task->tTimer >= 70)   // ~1.2 s: preserve original fly-past
        {
            task->tTimer = 0;
            task->tState = S0_LAUNCH_SPRITES;
        }
        break;

    // ------------------------------------------------------------------
    // Jirachi e Celebi voam pela tela com seus cries
    case S0_LAUNCH_SPRITES:
    {
        // Celebi: esquerda -> direita
        u8 celebiId = CreateSprite(&sSpriteTemplate_Scene0Celebi,
                                   -64, DISPLAY_HEIGHT / 2, 0);
        gSprites[celebiId].invisible = FALSE;
        task->tCelebiId = celebiId;

        // Jirachi: direita -> esquerda (espelhado)
        u8 jirachiId = CreateSprite(&sSpriteTemplate_Scene0Jirachi,
                                    DISPLAY_WIDTH + 32, DISPLAY_HEIGHT / 2 - 20, 0);
        gSprites[jirachiId].invisible = FALSE;
        gSprites[jirachiId].hFlip    = TRUE;
        task->tJirachiId = jirachiId;

        // Cries
        PlayCryInternal(SPECIES_CELEBI,  0, 120, 10, 0);
        PlayCryInternal(SPECIES_JIRACHI, 0, 120, 10, 0);

        task->tTimer = 0;
        task->tState = S0_WAIT_SPRITES;
        break;
    }

    // ------------------------------------------------------------------
    case S0_WAIT_SPRITES:
    {
        bool8 cDone = (task->tCelebiId  == SPRITE_NONE
                       || gSprites[task->tCelebiId].invisible);
        bool8 jDone = (task->tJirachiId == SPRITE_NONE
                       || gSprites[task->tJirachiId].invisible);

        if (cDone && jDone)
        {
            if (task->tCelebiId  != SPRITE_NONE)
                DestroySprite(&gSprites[task->tCelebiId]);
            if (task->tJirachiId != SPRITE_NONE)
                DestroySprite(&gSprites[task->tJirachiId]);
            task->tCelebiId  = SPRITE_NONE;
            task->tJirachiId = SPRITE_NONE;
            task->tTimer     = 0;
            task->tState     = S0_FADE_TO_SET03;
        }
        break;
    }

    // ------------------------------------------------------------------
    // Fade to black, libera sprites antigos, carrega set03 + moon + comet
    // Speed 8 = fade suave (~2.1s para escurecer completamente), RGB_BLACK
    case S0_FADE_TO_SET03:
        if (task->tTimer == 0)
            BeginNormalPaletteFade(PALETTES_ALL, 8, 0, 16, RGB_BLACK);
        task->tTimer++;
        if (!gPaletteFade.active && task->tTimer > 1)
        {
            // Desliga display e blend ANTES de mexer na VRAM
            SetGpuReg(REG_OFFSET_DISPCNT, 0);
            SetGpuReg(REG_OFFSET_BLDCNT, 0);
            SetGpuReg(REG_OFFSET_BLDY, 0);

            // Libera recursos antigos
            if (task->tShrineId != SPRITE_NONE)
                DestroySprite(&gSprites[task->tShrineId]);
            ResetSpriteData();

            // Temporariamente desativa transferências automáticas de paleta
            gPaletteFade.bufferTransferDisabled = TRUE;

            FreeAllSpritePalettes();

            // Carrega nova cena (display desligado)
            Scene0_LoadSet03();

            // Recria blend para o glow da lua (será restaurado ao religar)
            SetGpuReg(REG_OFFSET_BLDCNT,
                BLDCNT_TGT1_OBJ | BLDCNT_EFFECT_LIGHTEN);
            SetGpuReg(REG_OFFSET_BLDY, 0);

            // Reserve os primeiros 8 slots OBJ para elementos fixos (estrelas, etc.)
            gReservedSpritePaletteCount = 8;

            // Carrega sprites (lua, cometa, sparkles e segundos voos)
            LoadCompressedSpriteSheet(sSpriteSheet_Scene0Moon);
            LoadSpritePalettes(sSpritePalette_Scene0Moon);
            LoadCompressedSpriteSheet(sSpriteSheet_Scene0Comet);
            LoadSpritePalettes(sSpritePalette_Scene0Comet);
            LoadCompressedSpriteSheet(sSpriteSheet_Scene0Sparkle);
            LoadSpritePalettes(sSpritePalette_Scene0Sparkle);
            LoadCompressedSpriteSheet(sSpriteSheet_Scene0Celebi2);
            LoadSpritePalettes(sSpritePalette_Scene0Celebi2);
            LoadCompressedSpriteSheet(sSpriteSheet_Scene0Jirachi2);
            LoadSpritePalettes(sSpritePalette_Scene0Jirachi2);

            Scene0_CreateStars();

            // Após carregar elementos fixos, permitir alocação normal de paletas
            gReservedSpritePaletteCount = 0;

            // As paletas foram atualizadas enquanto o display estava desligado; força uma
            // transferência imediata para a PAL durante o VBlank
            gPaletteFade.bufferTransferDisabled = FALSE;
            TransferPlttBuffer();

            // Cria a lua
            task->tMoonId = CreateSprite(&sSpriteTemplate_Scene0Moon,
                                         DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 1);
            gSprites[task->tMoonId].invisible = FALSE;
            gSprites[task->tMoonId].data[1] = 768;
            gSprites[task->tMoonId].data[2] = 0;

            // RELIGA O DISPLAY (agora sem flash)
            SetGpuReg(REG_OFFSET_DISPCNT,
                DISPCNT_MODE_0
                | DISPCNT_OBJ_1D_MAP
                | DISPCNT_BG0_ON
                | DISPCNT_BG1_ON
                | DISPCNT_OBJ_ON);

            task->tTimer = 0;
            task->tCloudScroll = 0;
            task->tSparkleTimer = 0;
            BeginNormalPaletteFade(PALETTES_ALL, 8, 16, 0, RGB_BLACK);
            PlaySE(SE_M_MOONLIGHT);
            task->tState = S0_HOLD_SET03;
        }
        break;

    // ------------------------------------------------------------------
    case S0_HOLD_SET03:
        task->tCloudScroll++;
        SetGpuReg(REG_OFFSET_BG1HOFS, (u16)(task->tCloudScroll >> 1));
    Scene0_UpdateStars();   // mesma frequência do Birch: a cada frame
        task->tTimer++;

        // Igual S0_HOLD_SET01/02: pausa até o fade-in terminar, depois conta
        if (gPaletteFade.active)
            break;
        if (task->tTimer >= 115)   // ~1.9 s for moon and clouds
        {
            task->tTimer = 0;
            task->tState = S0_LAUNCH_COMET;
        }
        break;

    // ------------------------------------------------------------------
    case S0_LAUNCH_COMET:
    {
        // Trajetoria EXATA: diagonal 45 graus cruzando o quadrado central 160x160
        // GBA: 240x160. Quadrado central 160x160 ocupa x=[40,200], y=[0,160].
        // Entra pelo canto superior-direito (x=200, y=0) e sai pelo canto
        // inferior-esquerdo (x=40, y=160). Sprite 64x64, começa alem da borda.
        // vx=-3, vy=+3 → slope perfeitamente 1:1 (45 graus).
        u8 id = CreateSprite(&sSpriteTemplate_Scene0Comet,
                             232, -32, 0);   // fora da tela: x=200+32, y=-32
        gSprites[id].invisible = FALSE;
        PlaySE(SE_M_HYPER_BEAM2);            // som do cometa
        task->tCometId      = id;
        task->tTimer        = 0;
        task->tSparkleTimer = 0;
        task->tState        = S0_WAIT_COMET;
        break;
    }

        // ------------------------------------------------------------------
    case S0_WAIT_COMET:
    {
        // Scroll das nuvens continua
        task->tCloudScroll++;
        SetGpuReg(REG_OFFSET_BG1HOFS, (u16)(task->tCloudScroll >> 1));
        Scene0_UpdateStars();   // mesma frequência do Birch: a cada frame

        // Sparkles ao longo da trajetória do cometa (menos frequentes — mais bonito)
        if (task->tCometId != SPRITE_NONE && ++task->tSparkleTimer % 8 == 0)
        {
            // Spawna perto da posição atual do cometa para simular rastro
            s16 cx = gSprites[task->tCometId].x;
            s16 cy = gSprites[task->tCometId].y;
            s16 rx = cx + (s16)((Random() % 40) - 20);
            s16 ry = cy + (s16)((Random() % 40) - 20);
            u8 spk = CreateSprite(&sSpriteTemplate_Scene0Sparkle,
                                  rx, ry, 0);
            StartSpriteAnim(&gSprites[spk], 0);
        }

        // Verifica se o cometa saiu da tela
        if (task->tCometId != SPRITE_NONE
            && gSprites[task->tCometId].invisible)
        {
            DestroySprite(&gSprites[task->tCometId]);
            task->tCometId = SPRITE_NONE;
            task->tTimer   = 0;
            task->tState   = S0_LAUNCH_SPRITES2;  // próxima etapa: segundo par de sprites
        }
        break;
    }

    // ------------------------------------------------------------------
    case S0_LAUNCH_SPRITES2:
        // Delay de 1.5s (90 frames) apos o cometa, ceu fica respirando
        task->tCloudScroll++;
        SetGpuReg(REG_OFFSET_BG1HOFS, (u16)(task->tCloudScroll >> 1));
        Scene0_UpdateStars();
        if (++task->tTimer < 50)
            break;
        // Após o delay, cria os sprites
        {
            u8 celebiId = CreateSprite(&sSpriteTemplate_Scene0Celebi2,
                                       48, DISPLAY_HEIGHT + 32, 2);
            gSprites[celebiId].invisible = FALSE;
            task->tCelebiId = celebiId;

            u8 jirachiId = CreateSprite(&sSpriteTemplate_Scene0Jirachi2,
                                        192, DISPLAY_HEIGHT + 32, 2);
            gSprites[jirachiId].invisible = FALSE;
            task->tJirachiId = jirachiId;

            task->tTimer = 0;
            task->tState = S0_WAIT_SPRITES2;
        }
        break;
    case S0_WAIT_SPRITES2:
    {
        task->tCloudScroll++;
        SetGpuReg(REG_OFFSET_BG1HOFS, (u16)(task->tCloudScroll >> 1));
        Scene0_UpdateStars();

        bool8 cDone = (task->tCelebiId  == SPRITE_NONE
                       || gSprites[task->tCelebiId].invisible);
        bool8 jDone = (task->tJirachiId == SPRITE_NONE
                       || gSprites[task->tJirachiId].invisible);

        if (cDone && jDone)
        {
            if (task->tCelebiId  != SPRITE_NONE)
                DestroySprite(&gSprites[task->tCelebiId]);
            if (task->tJirachiId != SPRITE_NONE)
                DestroySprite(&gSprites[task->tJirachiId]);
            task->tCelebiId  = SPRITE_NONE;
            task->tJirachiId = SPRITE_NONE;
            task->tTimer     = 0;
            task->tState     = S0_FADE_OUT;
        }
        break;
    }

    // ------------------------------------------------------------------
    case S0_FADE_OUT:
        // Delay generoso após os sprites: glow das estrelas + nuvens + lua por ~2.5s
        // Scroll das nuvens e lua continuam durante todo o delay
        task->tCloudScroll++;
        SetGpuReg(REG_OFFSET_BG1HOFS, (u16)(task->tCloudScroll >> 1));
        Scene0_UpdateStars();

        // Sparkles aleatórios continuam por mais um tempo
        if (++task->tSparkleTimer % 10 == 0 && task->tTimer < 95)
        {
            u16 rx = (u16)((Random() % 220) + 10);
            u16 ry = (u16)((Random() % 120) + 10);
            CreateSprite(&sSpriteTemplate_Scene0Sparkle, (s16)rx, (s16)ry, 0);
        }

        // Fade into the approved PRL title after ~1.7 seconds (100 frames).
        if (++task->tTimer == 100)
            BeginNormalPaletteFade(PALETTES_ALL, 8, 0, 16, RGB_BLACK);

        if (!gPaletteFade.active && task->tTimer > 100)
            task->tState = S0_DONE;
        break;

    // ------------------------------------------------------------------
    case S0_SKIP_FADE:
        if (!gPaletteFade.active)
            task->tState = S0_DONE;
        break;

    case S0_DONE:
    default:
        SetGpuReg(REG_OFFSET_DISPCNT,  0);
        SetGpuReg(REG_OFFSET_MOSAIC,   0);
        SetGpuReg(REG_OFFSET_BLDCNT,   0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY,     0);
        DmaClear16(3, (void *)BG_SCREEN_ADDR(S0_SKY_SCREEN),    2048);
        DmaClear16(3, (void *)BG_SCREEN_ADDR(S0_FOREST_SCREEN), 2048);
        Scene0_DestroyStars();
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        DestroyTask(taskId);
        SetVBlankCallback(NULL);
        m4aSongNumStop(MUS_INTRO);
        SetMainCallback2(CB2_InitTitleScreen);
        break;
    }
}

#undef tState
#undef tTimer
#undef tBgSet
#undef tShrineId
#undef tMoonId
#undef tCelebiId
#undef tJirachiId
#undef tCometId
#undef tCloudScroll
#undef tSparkleTimer

// =========================================================================
// CB2_Scene0Intro
// =========================================================================
void CB2_InitPRLHwlScene0(void)
{
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFade();
    SetVBlankCallback(NULL);
    CreateTask(Task_Scene0_Load, 0);
    SetMainCallback2(CB2_PRLHwlScene0);
}

// =========================================================================
// SpriteCB_Scene0Shrine — estatico
// =========================================================================
static void SpriteCB_Scene0Shrine(struct Sprite *sprite)
{
    (void)sprite;
}

// =========================================================================
// SpriteCB_Scene0Celebi — esquerda->direita, deriva levemente para cima
// =========================================================================
static void SpriteCB_Scene0Celebi(struct Sprite *sprite)
{
    sprite->x += 3;
    if (++sprite->data[0] % 4 == 0)
        sprite->y -= 1;
    if (sprite->x > DISPLAY_WIDTH + 64)
        sprite->invisible = TRUE;
}

// =========================================================================
// SpriteCB_Scene0Jirachi — direita->esquerda, deriva levemente para cima
// =========================================================================
static void SpriteCB_Scene0Jirachi(struct Sprite *sprite)
{
    sprite->x -= 3;
    if (++sprite->data[0] % 4 == 0)
        sprite->y -= 1;
    if (sprite->x < -64)
        sprite->invisible = TRUE;
}

// =========================================================================
// SpriteCB_Scene0Celebi2 — voa de baixo pra cima em linha reta (x fixo)
// Entra pela base da tela, passa pela frente da lua, some no topo
// =========================================================================
static void SpriteCB_Scene0Celebi2(struct Sprite *sprite)
{
    // 4px a cada 3 frames = 4/3 px/frame = 3x mais lento que 4px/frame
    // data[0] = acumulador: +4 por frame; quando >=3 subtrai 3 e move 1px
    sprite->data[0] += 4;
    while (sprite->data[0] >= 3)
    {
        sprite->data[0] -= 3;
        sprite->y--;
    }
    if (sprite->y < -64)
        sprite->invisible = TRUE;
}

static void SpriteCB_Scene0Jirachi2(struct Sprite *sprite)
{
    sprite->data[0] += 4;
    while (sprite->data[0] >= 3)
    {
        sprite->data[0] -= 3;
        sprite->y--;
    }
    if (sprite->y < -64)
        sprite->invisible = TRUE;
}

// =========================================================================
// =========================================================================
// SpriteCB_Scene0Comet — diagonal 45 graus, mais rápido com glow
// Velocidade: 5px/frame (era 3px) → cruzamento em ~33 frames (~0.55s)
// Glow: oscila BLDY entre 4 e 8 a cada 3 frames (flash rápido e brilhante)
// =========================================================================
static void SpriteCB_Scene0Comet(struct Sprite *sprite)
{
    // Movimento: 5px diagonal, mais rápido que antes
    sprite->x -= 5;
    sprite->y += 5;

    // Glow pulsante rápido: BLDY oscila 4→8→4 a cada 3 frames
    // data[1] = contador de frames para o glow
    {
        u8 phase = (u8)(++sprite->data[1] % 6);
        // 0,1,2 → sobe 4→6→8; 3,4,5 → desce 8→6→4
        static const u8 bldy_table[6] = {4, 5, 6, 7, 8, 7};
        SetGpuReg(REG_OFFSET_BLDY, bldy_table[phase]);
    }

    if (sprite->x < -64 || sprite->y > DISPLAY_HEIGHT + 64)
    {
        // Restaura BLDY para o pulso da lua ao sair da tela
        SetGpuReg(REG_OFFSET_BLDY, 0);
        sprite->invisible = TRUE;
    }
}

// =========================================================================
// SpriteCB_Scene0Sparkle — toca animacao uma vez e se destroi
// =========================================================================
static void SpriteCB_Scene0Sparkle(struct Sprite *sprite)
{
    // A animacao tem 4 frames x 4 game-frames = 16 frames total
    // ANIMCMD_END vai travar o sprite — destruimos apos 20 frames
    if (++sprite->data[0] >= 20)
        DestroySprite(sprite);
}

// =========================================================================
// SpriteCB_Scene0Moon
//
// GROW: data[1] guarda o valor atual de PA/PD (escala inversa do GBA).
//   PA=256 = tamanho 100%. PA=768 = ~33% (começa pequena).
//   Decrementamos 3 por frame até chegar em 256 → ~171 frames ≈ 2.85s de crescimento.
//   Depois que chegou em 256, o valor para de mudar.
//
// BLDY PULSE: data[2] = contador de fase (0..119).
//   Triângulo suave: 0→3→0 em 120 frames (2s por ciclo).
//   Só escreve BLDY se o cometa não está ativo (detectado por BLDY >= 4).
// =========================================================================
static void SpriteCB_Scene0Moon(struct Sprite *sprite)
{
    s16 scale;
    u8  phase;
    u8  bldy;

    if (sprite->invisible)
        return;

    // --- Grow suave: escala PA/PD de 768 → 256 a 3 por frame ---
    scale = sprite->data[1];
    if (scale > 256)
    {
        // data[3] = sub-contador: so decrementa nos frames pares
        // media: -1.5/frame → ~341 frames ≈ 5.7s (metade da velocidade anterior)
        if (++sprite->data[3] % 2 == 0)
        {
            scale -= 3;
            if (scale < 256)
                scale = 256;
            sprite->data[1] = scale;
        }
    }
    // SetOamMatrix(matrixNum, PA, PB, PC, PD)
    // PB=PC=0 (sem rotação/skew), PA=PD=scale (escala uniforme)
    SetOamMatrix((u8)sprite->oam.matrixNum, (u16)scale, 0, 0, (u16)scale);

    // --- Pulso BLDY suave: triângulo 0→3→0 em 120 frames ---
    phase = (u8)(++sprite->data[2]);
    if (sprite->data[2] >= 120)
        sprite->data[2] = 0;

    if (phase < 60)
        bldy = (u8)((phase * 3 + 29) / 59);
    else
        bldy = (u8)(((119 - phase) * 3 + 29) / 59);

    // Respeita o flash do cometa (cometa usa BLDY >= 4)
    if (GetGpuReg(REG_OFFSET_BLDY) < 4)
        SetGpuReg(REG_OFFSET_BLDY, bldy);
}

#endif
