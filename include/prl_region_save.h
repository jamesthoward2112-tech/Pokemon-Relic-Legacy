#ifndef GUARD_PRL_REGION_SAVE_H
#define GUARD_PRL_REGION_SAVE_H

// Region-persistent flags live outside the legacy FRLG flag array.
// Do not assign anything in these ranges to an existing Kanto quest.
#define PRL_HOENN_EVENT_FLAGS_START    0xA000
#define PRL_JOHTO_EVENT_FLAGS_START    0xA100
#define PRL_REGION_EVENT_FLAG_COUNT   128

#define PRL_HOENN_TRAINER_FLAGS_START  0xB000
#define PRL_JOHTO_TRAINER_FLAGS_START  0xB400
#define PRL_REGION_TRAINER_FLAG_COUNT 1024

#define PRL_REGION_SAVE_MAGIC   0x504C5254 // "TRLP" on little endian
#define PRL_REGION_SAVE_VERSION 1

struct PrlRegionSave
{
    u32 magic;
    u16 version;
    u16 size;
    u8 hoennTrainerDefeated[PRL_REGION_TRAINER_FLAG_COUNT / 8];
    u8 johtoTrainerDefeated[PRL_REGION_TRAINER_FLAG_COUNT / 8];
    u8 hoennQuestFlags[PRL_REGION_EVENT_FLAG_COUNT / 8];
    u8 johtoQuestFlags[PRL_REGION_EVENT_FLAG_COUNT / 8];
    // Rematch groups are separate from first-defeat bits. Only allocate
    // stable group IDs in later, audited region-specific rematch tables.
    u8 rematchTiers[64];
    u32 checksum;
};

// The region block is in the unallocated *data tail* of PokemonStorage's
// final flash sector. SaveBlock1, the existing flags and SaveBlock3 (bag)
// retain their exact offsets, sizes and normal checksums.
#define PRL_REGION_SAVE_OFFSET (SECTOR_DATA_SIZE - sizeof(struct PrlRegionSave))

extern struct PrlRegionSave gPrlRegionSave;
u8 *PrlGetRegionFlagPointer(u16 id);
void PrlClearRegionSave(void);
void PrlReadRegionSaveSector(const u8 *data);
void PrlWriteRegionSaveSector(u8 *data);

#endif // GUARD_PRL_REGION_SAVE_H
