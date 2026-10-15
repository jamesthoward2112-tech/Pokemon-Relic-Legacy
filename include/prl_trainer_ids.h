#ifndef GUARD_PRL_TRAINER_IDS_H
#define GUARD_PRL_TRAINER_IDS_H

// Region-tagged 16-bit trainer handles. The original FireRed 0..767 IDs and
// partner IDs are not renumbered; Emerald trainers use their original Emerald
// local IDs within the new 0x1000 range. Johto is reserved for future imports.
// All ranges sit below the special 0xFF00 link/secret-base sentinels.
#define PRL_TRAINER_HOENN_BASE 0x1000
#define PRL_TRAINER_JOHTO_BASE 0x2000
#define PRL_TRAINER_REGION_CAPACITY 1024

#define PRL_HOENN_TRAINER(id) (PRL_TRAINER_HOENN_BASE + (id))
#define PRL_JOHTO_TRAINER(id) (PRL_TRAINER_JOHTO_BASE + (id))
#define PRL_IS_HOENN_TRAINER(id) ((id) >= PRL_TRAINER_HOENN_BASE && (id) < PRL_TRAINER_HOENN_BASE + TRAINERS_COUNT_EMERALD)
#define PRL_IS_JOHTO_TRAINER(id) ((id) >= PRL_TRAINER_JOHTO_BASE && (id) < PRL_TRAINER_JOHTO_BASE + PRL_TRAINER_REGION_CAPACITY)

#endif
