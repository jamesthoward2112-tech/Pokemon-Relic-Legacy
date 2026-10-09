# PRL Gym Rewards & Expanded Storage — 9 October 2026

## Gym rule approved
Across 24 gyms, first-clear badge + original TM + a type-appropriate held item and 10,000 bonus in addition to battle winnings. After the first victory, only regular rematch money. Each extra item+money claim uses one region-unique flag. A full Bag prevents the extra item and bonus and is recoverable by returning to the leader. Existing badge and original TM flags are kept untouched. 24 reward flags are reserved at 0x493–0x4AA.

**Source implementation:** All eight Kanto Gym reward scripts and eight Hoenn Gym reward scripts now have first-clear and follow-up extra-claim hooks. Johto's eight reward flags are reserved for the future Johto map import; the checkout currently contains no Johto Gym map scripts. This does NOT itself implement 24 distinct badge flags; the old two donor regions still share vanilla badge symbols.

| Kanto | Extra held item |
|---|---|
| BROCK | HARD_STONE |
| MISTY | MYSTIC_WATER |
| SURGE | MAGNET |
| ERIKA | MIRACLE_SEED |
| KOGA | POISON_BARB |
| SABRINA | TWISTED_SPOON |
| BLAINE | CHARCOAL |
| GIOVANNI | SOFT_SAND |

| Hoenn | Extra held item |
|---|---|
| ROXANNE | HARD_STONE |
| BRAWLY | BLACK_BELT |
| WATTSON | MAGNET |
| FLANNERY | CHARCOAL |
| NORMAN | SILK_SCARF |
| WINONA | SHARP_BEAK |
| TATE_LIZA | TWISTED_SPOON |
| JUAN | MYSTIC_WATER |

| Johto (pending map port) | Extra held item | TM |
|---|---|---|
| FALKNER | SHARP_BEAK | ROOST |
| BUGSY | SILVER_POWDER | U_TURN |
| WHITNEY | EVIOLITE | RETURN |
| MORTY | SPELL_TAG | SHADOW_BALL |
| CHUCK | BLACK_BELT | FOCUS_PUNCH |
| JASMINE | METAL_COAT | IRON_TAIL |
| PRYCE | NEVER_MELT_ICE | HAIL |
| CLAIR | DRAGON_FANG | DRAGON_PULSE_AFTER_DEN |

**Johto caveat:** Whitney's updated donor Eviolite/Return selection replaces the older Silk Scarf; Clair's Dragon Pulse is awarded only after Dragon's Den shrine test, unlike the immediate held item and bonus.

## Storage request — NOT IMPLEMENTED YET

User explicitly demands expanded Bag slots, PC item storage, and Pokémon PC storage. The current compiled defaults are BAG_ITEMS_COUNT=30, BAG_KEYITEMS_COUNT=30, BAG_POKEBALLS_COUNT=16, BAG_TMHM_COUNT=64, BAG_BERRIES_COUNT=46 and PC_ITEMS_COUNT=50; stack limits already allow 999 items. The current Pokémon PC is 14 boxes ×30 = 420 Pokémon. The locked three-region canon requires 20 ordinary level-tier boxes (600) plus LEGENDS (30): **21 boxes / 630 Pokémon**.

**Save safety blocker:** `src/save.c` allots nine 3968-byte flash sectors per save slot to `struct PokemonStorage`. Changing `TOTAL_BOXES_COUNT` from 14 to 21 overflows that allocation and its STATIC_ASSERT. Increasing Bag/PC item slots shifts SaveBlock1 offsets unless implemented with a deliberate migration. Plan as a separate save-format engineering task including versioning, back-compat, sector layout/capacity, full Bag and full 21-box stress saves, UI cycling, save/reload and cross-region transfer. Neither Pokémon boxes nor Bag/PC item capacities are expanded in this Gym commit. Do not call them done.

## Validation
Static gym-script checks for unique 24 flag IDs, held item before cash flag, exact one-time hooks, and unchanged badges and TMs; FireRed compilation and focused battle regressions required. In-game full Bag, existing save migration, rematches, money cap and Johto integration remain pending.
