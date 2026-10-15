#include "global.h"
#include "save.h"
#include "pokemon_storage_system.h"
#include "event_data.h"
#include "constants/opponents.h"
#include "prl_region_save.h"
#include "string.h"

EWRAM_DATA struct PrlRegionSave gPrlRegionSave = {0};

STATIC_ASSERT(sizeof(struct PokemonStorage) <=
              (SECTOR_ID_PKMN_STORAGE_END - SECTOR_ID_PKMN_STORAGE_START + 1) * SECTOR_DATA_SIZE
              - sizeof(struct PrlRegionSave), PrlRegionSaveFitsPokemonStorageTail);

// This checksum is deliberately independent of vanilla's flash-sector checksum.
// Old saves have a zero-filled tail. A torn or invalid extension must never
// invalidate the player's Pokemon, Kanto progress, or existing inventory.
static u32 GetPrlRegionSaveChecksum(const struct PrlRegionSave *state)
{
    const u8 *data = (const u8 *)state;
    u32 hash = 2166136261u;
    u32 i;
    for (i = 0; i < offsetof(struct PrlRegionSave, checksum); ++i)
    {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

void PrlClearRegionSave(void)
{
    memset(&gPrlRegionSave, 0, sizeof(gPrlRegionSave));
    gPrlRegionSave.magic = PRL_REGION_SAVE_MAGIC;
    gPrlRegionSave.version = PRL_REGION_SAVE_VERSION;
    gPrlRegionSave.size = sizeof(gPrlRegionSave);
}

u8 *PrlGetRegionFlagPointer(u16 id)
{
    if (id >= PRL_HOENN_EVENT_FLAGS_START
        && id < PRL_HOENN_EVENT_FLAGS_START + PRL_REGION_EVENT_FLAG_COUNT)
        return &gPrlRegionSave.hoennQuestFlags[(id - PRL_HOENN_EVENT_FLAGS_START) / 8];
    if (id >= PRL_JOHTO_EVENT_FLAGS_START
        && id < PRL_JOHTO_EVENT_FLAGS_START + PRL_REGION_EVENT_FLAG_COUNT)
        return &gPrlRegionSave.johtoQuestFlags[(id - PRL_JOHTO_EVENT_FLAGS_START) / 8];
    if (id >= PRL_HOENN_TRAINER_FLAGS_START
        && id < PRL_HOENN_TRAINER_FLAGS_START + PRL_REGION_TRAINER_FLAG_COUNT)
        return &gPrlRegionSave.hoennTrainerDefeated[(id - PRL_HOENN_TRAINER_FLAGS_START) / 8];
    if (id >= PRL_JOHTO_TRAINER_FLAGS_START
        && id < PRL_JOHTO_TRAINER_FLAGS_START + PRL_REGION_TRAINER_FLAG_COUNT)
        return &gPrlRegionSave.johtoTrainerDefeated[(id - PRL_JOHTO_TRAINER_FLAGS_START) / 8];
    return NULL;
}

void PrlReadRegionSaveSector(const u8 *data)
{
    struct PrlRegionSave diskState;
    memcpy(&diskState, data + PRL_REGION_SAVE_OFFSET, sizeof(diskState));
    if (diskState.magic == PRL_REGION_SAVE_MAGIC
        && diskState.version == PRL_REGION_SAVE_VERSION
        && diskState.size == sizeof(diskState)
        && diskState.checksum == GetPrlRegionSaveChecksum(&diskState))
        gPrlRegionSave = diskState;
    else
        PrlClearRegionSave();
}

void PrlWriteRegionSaveSector(u8 *data)
{
    gPrlRegionSave.magic = PRL_REGION_SAVE_MAGIC;
    gPrlRegionSave.version = PRL_REGION_SAVE_VERSION;
    gPrlRegionSave.size = sizeof(gPrlRegionSave);
    gPrlRegionSave.checksum = GetPrlRegionSaveChecksum(&gPrlRegionSave);
    memcpy(data + PRL_REGION_SAVE_OFFSET, &gPrlRegionSave, sizeof(gPrlRegionSave));
}

// Only the dedicated Route102 IDs 659..662 from PR107 can be migrated
// unambiguously. The older Emerald 615 collision with Kanto Melissa cannot
// be auto-migrated because both games used exactly the same defeated bit.
void PrlMigrateLegacyHoennRoute102Flags(void)
{
#if IS_FRLG
    static const u16 sOldTrainerIds[] = {659, 660, 661, 662};
    static const u16 sNewLocalIds[] = {TRAINER_CALVIN_1, TRAINER_RICK, TRAINER_TIANA, TRAINER_ALLEN};
    u32 i;
    for (i = 0; i < ARRAY_COUNT(sOldTrainerIds); ++i)
    {
        if (FlagGet(TRAINER_FLAGS_START + sOldTrainerIds[i]))
            FlagSet(PRL_HOENN_TRAINER_FLAGS_START + sNewLocalIds[i]);
    }
#endif
}
