// PRL optional intro animation trial, deliberately separate from the production intro.
// Copyright -> Porygon/Dizzy Egg -> this trial -> approved PRL title.
// Intro-specific sprites only: nothing here alters battle sprites, maps or saves.
#include "global.h"
#include "main.h"
#include "sprite.h"
#include "task.h"
#include "palette.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "title_screen.h"
#include "scanline_effect.h"
#include "trig.h"
#include "constants/rgb.h"

#if defined(FIRERED)
#define TAG_PRL_INTRO_NOX   0x5501
#define TAG_PRL_INTRO_AST   0x5502
#define TAG_PRL_INTRO_HOOH  0x5503
#define PRL_INTRO_FRAMES 440

static const u32 sNoxGfx[] = INCGFX_U32("graphics/intro/prl_trial/noxichu_run.png", ".4bpp.smol");
static const u32 sAstGfx[] = INCGFX_U32("graphics/intro/prl_trial/astrachi_fly.png", ".4bpp.smol");
static const u32 sHoohGfx[] = INCGFX_U32("graphics/intro/prl_trial/ho_oh_relic_fly.png", ".4bpp.smol");
static const u16 sNoxPal[] = INCGFX_U16("graphics/intro/prl_trial/noxichu_run.png", ".gbapal");
static const u16 sAstPal[] = INCGFX_U16("graphics/intro/prl_trial/astrachi_fly.png", ".gbapal");
static const u16 sHoohPal[] = INCGFX_U16("graphics/intro/prl_trial/ho_oh_relic_fly.png", ".gbapal");

static const struct CompressedSpriteSheet sSheets[] =
{
    {sNoxGfx, 0x2000, TAG_PRL_INTRO_NOX},
    {sAstGfx, 0x2000, TAG_PRL_INTRO_AST},
    {sHoohGfx, 0x2000, TAG_PRL_INTRO_HOOH},
    {},
};
static const struct SpritePalette sPals[] =
{
    {sNoxPal, TAG_PRL_INTRO_NOX},
    {sAstPal, TAG_PRL_INTRO_AST},
    {sHoohPal, TAG_PRL_INTRO_HOOH},
    {},
};
static const struct OamData sOam =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 0,
};
static const union AnimCmd sCycle[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims[] = {sCycle};
static const struct SpriteTemplate sNox =
{
    .tileTag = TAG_PRL_INTRO_NOX,
    .paletteTag = TAG_PRL_INTRO_NOX,
    .oam = &sOam,
    .anims = sAnims,
    .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sAst =
{
    .tileTag = TAG_PRL_INTRO_AST,
    .paletteTag = TAG_PRL_INTRO_AST,
    .oam = &sOam,
    .anims = sAnims,
    .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sHooh =
{
    .tileTag = TAG_PRL_INTRO_HOOH,
    .paletteTag = TAG_PRL_INTRO_HOOH,
    .oam = &sOam,
    .anims = sAnims,
    .callback = SpriteCallbackDummy,
};

static void VBlankCB_PRLIntro(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_PRLIntroTrial(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

// Simple native GBA night sky and crescent-like golden moon for the graphics trial.
// No large donor backgrounds or extra graphics buffers are needed.
static void DrawPRLIntroSky(void)
{
    volatile u16 *const gfx = (volatile u16 *)VRAM;
    volatile u16 *const map = (volatile u16 *)BG_SCREEN_ADDR(31);
    const u16 pal[16] = {
        0x1C82, 0x7FFF, 0x6FFF, 0x3FDF, 0x2529, 0x18C6, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    u16 x, y, tx, ty, px, py;
    LoadPalette(pal, BG_PLTT_ID(0), sizeof(pal));
    // Tile 1: lone star. A 4bpp tile holds 16 halfwords.
    for (x = 0; x < 16; x++)
        gfx[16 + x] = 0;
    gfx[16 + 3 * 2] = 0x0100;
    gfx[16 + 4 * 2 + 1] = 0x0001;

    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++)
            map[y * 32 + x] = ((x * 31 + y * 17 + x * y * 7) % 37 == 0) ? 1 : 0;

    // Moon uses 64 unique tiles in the same BG charblock; tile 0 stays transparent.
    for (ty = 0; ty < 8; ty++)
        for (tx = 0; tx < 8; tx++)
        {
            u16 tile = 2 + ty * 8 + tx;
            map[(ty + 1) * 32 + (tx + 13)] = tile;
            for (py = 0; py < 8; py++)
                for (px = 0; px < 8; px++)
                {
                    s16 dx = tx * 8 + px - 31;
                    s16 dy = ty * 8 + py - 31;
                    u16 c = (dx * dx + dy * dy <= 27 * 27) ? (dx + dy < -12 ? 3 : 2) : 0;
                    u16 idx = tile * 16 + py * 2 + (px / 4);
                    u16 shift = (px % 4) * 4;
                    u16 word = gfx[idx];
                    gfx[idx] = (word & ~(0xF << shift)) | (c << shift);
                }
        }
}

static void Task_PRLIntroTrial(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u16 t = ++task->data[1];
    if (task->data[0] == 0)
    {
        // Expose every animation for several seconds, and permit skipping.
        u8 n = task->data[2], a = task->data[3], h = task->data[4];
        if (n < MAX_SPRITES)
        {
            gSprites[n].x = 300 - (t * 2) % 400;
            gSprites[n].y = 125;
        }
        if (a < MAX_SPRITES)
        {
            gSprites[a].x = -40 + (t > 50 ? (t - 50) : 0);
            gSprites[a].y = 79 + Sin((u8)(t * 2), 6);
        }
        if (h < MAX_SPRITES)
        {
            gSprites[h].x = 316 - (t > 90 ? (t - 90) : 0);
            gSprites[h].y = 65 + Sin((u8)(t * 2 + 64), 7);
        }
        if (t >= PRL_INTRO_FRAMES || (t > 50 && gMain.newKeys != 0))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 4, 0, 16, RGB_BLACK);
            task->data[0] = 1;
        }
    }
    else if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetVBlankCallback(NULL);
        SetMainCallback2(CB2_InitTitleScreen);
    }
}

void CB2_InitPRLIntroTrial(void)
{
    u8 id;
    const u16 bgcnt = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) |
                      BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFade();
    DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
    DmaFill16(3, 0, (void *)PLTT, PLTT_SIZE);
    DrawPRLIntroSky();
    SetGpuReg(REG_OFFSET_BG0CNT, bgcnt);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    // Runtime exposes the singular loader; use the three allocated 0x2000 sheets.
    LoadCompressedSpriteSheet(&sSheets[0]);
    LoadCompressedSpriteSheet(&sSheets[1]);
    LoadCompressedSpriteSheet(&sSheets[2]);
    LoadSpritePalettes(sPals);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_BG0_ON | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    id = CreateTask(Task_PRLIntroTrial, 0);
    gTasks[id].data[2] = CreateSprite(&sNox, 300, 125, 0);
    gTasks[id].data[3] = CreateSprite(&sAst, -40, 79, 0);
    gTasks[id].data[4] = CreateSprite(&sHooh, 316, 65, 0);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(VBlankCB_PRLIntro);
    SetMainCallback2(CB2_PRLIntroTrial);
}
#endif // FIRERED
