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

## Save-compatible expanded Bag and PC item storage — 12 October 2026

PRL ports the experimental save-compatible inventory layout from `prl/safe-inventory-20261009` onto the latest Silph 11F branch. Slot limits are **Bag items 60**, **Key Items 50**, **Poké Balls 32**, **TMs/HMs 100**, **Berries 56**, and **PC item storage 100**. Each normal stack can already hold up to 999 units; the change adds *distinct item slots* rather than quantity-per-slot.

The original `SaveBlock1` bag/PC arrays stay at their exact old sizes and offsets so current saves retain badges, flags, Pokémon and other data. Expanded inventory data is persisted in the spare `SaveBlock3` area. When an old save loads, the live inventory is initialized by copying the original encrypted bag slots and unencrypted PC items. Saving retains legacy-size backup arrays; downgrading to older PRL builds **after filling additional slots is not supported**.

Sabrina's first-clear held item is **Twisted Spoon**, separate from TM04 Calm Mind. Her one-time held-item-plus-10,000 bonus can be claimed on a return visit without a rematch, even if the TM was already received. The claim flag is set only after the item is awarded and the money added.

Pokémon PC remains at **14 boxes × 30 Pokémon (420)**; no Pokémon storage changes were made.

## Validation
Static checks verify the 24 reward flags, 24 region-specific badge flags, item-before-cash ordering, first-clear hooks, rematch routes, and approved TMs/items. Save flags use the project's persistent event-flag storage, but emulator save/reload and live full-Bag recovery were not available in this environment. FireRed compilation was attempted but is blocked here because arm-none-eabi-gcc, pkg-config, and libpng headers are missing. Johto integration remains pending until its Gym maps are imported.
