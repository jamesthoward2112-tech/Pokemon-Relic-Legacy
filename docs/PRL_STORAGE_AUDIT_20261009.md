# PRL — Storage expansion implementation and safety audit (9 October 2026)

## Implemented on isolated branch `prl/safe-inventory-20261009`

Expanded distinct inventory slots:
- Bag Items: 30 → 60
- Bag Key Items: 30 → 50
- Bag Poké Balls: 16 → 32
- Bag TMs/HMs: 64 → 100
- Bag Berries: 46 → 56
- PC item storage: 50 → 100

The original `SaveBlock1` retains exact legacy bag and PC item field sizes and positions, so existing save sectors do not shift. The expanded live `Bag` and PC item array are stored in the spare SaveBlock3 region, using a magic marker (`PRLI`) and migrated from SaveBlock1 on the first successful load of an old save. The old 30/30/16/64/46 bag and 50 PC slots are kept as a compatibility backup before saving. The full inventory is persisted in SaveBlock3; downgrade to an older build is NOT supported once slots beyond original capacity are occupied.

SaveBlock3 extended region is 399 aligned words (1596 bytes). The `STATIC_ASSERT` in item.c guarantees that the new inventory fits. SaveBlock3's existing DexNav chain remains intact; enabling major additional SaveBlock3 features would require re-auditing flash space.

## Pokémon PC: NOT yet expanded

**14 boxes of 30 = 420 total capacity remains unchanged.** The original FireRed `PokemonStorage` is 34,144 bytes of 35,712 available (1,568 spare). An extra full box needs more than the spare space. Arbitrarily raising `TOTAL_BOXES_COUNT` overflows fixed save sectors and risks save corruption. PRL needs a separate save serializer/extension design, explicit migration, and save/reload tests to support meaningful regional box expansion. Do not mark this aspect complete.

## Outstanding test work

Before release: FireRed full compilation; emulator save and reload; fresh save and old-save migration with exact item quantities; filling and sorting each pocket past its old slot limit; Wally battle temporarily swapped bag restore; PC withdraw/deposit/toss/sort after 50 different items; save/reload and checksum recovery. Test any new box implementation separately.
