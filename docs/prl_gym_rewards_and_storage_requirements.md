# PRL Gym Rewards & Expanded Storage — 9 October 2026

## Gym rule approved
Across 24 gyms, first-clear badge + approved TM + a type-appropriate held item and £10,000 bonus in addition to battle winnings. After the first victory, rematches award normal battle winnings only. The 24 extra item+money claims use persistent one-time flags at 0x493–0x4AA. The reward script gives the item before money and sets the claim flag last; a full Bag leaves the item and bonus outstanding for a later visit to the leader. Existing donor badge and TM flags remain in place for story logic.

**Current implementation:** All eight Kanto and eight Hoenn Gyms have first-clear reward hooks, distinct persistent region badge flags, and a post-victory Gym statue shortcut to the leader. Kanto rematch table entries reuse the donor leader teams; Hoenn keeps its existing rematch teams. Each statue retains its original informational behavior before the badge is earned. Johto's eight reward flags are reserved for a future map import; this checkout contains no Johto Gym maps, so Johto implementation is pending.

| Kanto | Approved TM | Extra held item |
|---|---|---|
| BROCK | TM39 | HARD_STONE |
| MISTY | TM03 | MYSTIC_WATER |
| SURGE | TM34 | MAGNET |
| ERIKA | TM19 | MIRACLE_SEED |
| KOGA | TM06 | POISON_BARB |
| SABRINA | TM04 | TWISTED_SPOON |
| BLAINE | TM38 | CHARCOAL |
| GIOVANNI | TM26 | SOFT_SAND |

| Hoenn | Approved TM | Extra held item |
|---|---|---|
| ROXANNE | ROCK_TOMB | HARD_STONE |
| BRAWLY | BULK_UP | BLACK_BELT |
| WATTSON | SHOCK_WAVE | MAGNET |
| FLANNERY | OVERHEAT | CHARCOAL |
| NORMAN | FACADE | SILK_SCARF |
| WINONA | AERIAL_ACE | SHARP_BEAK |
| TATE_LIZA | CALM_MIND | TWISTED_SPOON |
| JUAN | WATER_PULSE | MYSTIC_WATER |

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

## Storage capacities — unchanged

This Gym work does not expand Bag or PC storage. Capacities remain BAG_ITEMS_COUNT=30, BAG_KEYITEMS_COUNT=30, BAG_POKEBALLS_COUNT=16, BAG_TMHM_COUNT=64, BAG_BERRIES_COUNT=46, PC_ITEMS_COUNT=50, and Pokémon storage at 14 boxes ×30 (420 Pokémon). The separate experimental storage expansion draft is not part of this implementation.

## Validation
Static checks verify the 24 reward flags, 24 region-specific badge flags, item-before-cash ordering, first-clear hooks, rematch routes, and approved TMs/items. Save flags use the project's persistent event-flag storage, but emulator save/reload and live full-Bag recovery were not available in this environment. FireRed compilation was attempted but is blocked here because arm-none-eabi-gcc, pkg-config, and libpng headers are missing. Johto integration remains pending until its Gym maps are imported.
