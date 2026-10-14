# PRL Kanto → Hoenn: region-change checkpoint (14 October 2026)

## Status and donor repositories
- **Stage 1 in progress**: short Hall of Fame teaser, safe end-of-credits exit, disable automatic FRLG Oak Pokédex-count ambush, Champion Oak ticket, and Mum's pre-departure briefing.
- Kanto PR70 source is parent of this branch. Keep all existing save data, prior character encounters, visual locks, and PRL QoL.
- Donors: `jamesthoward2112-tech/Pokemon-Hoenn-Relics` (PHR Emerald source, assets and design) and existing Hoenn scripts/maps in PRL's `pokeemerald-expansion` layout.
- **NOT IMPLEMENTED YET**: item donation, cash banking, box migration, ship transition, or Hoenn campaign state. Do not advertise this build as a working regional transfer.

## Player-approved handover (at boarding only)
1. Hall of Fame → "The Journey Continues... In Hoenn!" credits. Champion Hall of Fame and existing save unaffected. No automatic Hoenn warp.
2. Kanto remains explorable indefinitely. Oak in his lab invites the existing Kanto Champion and stamps an S.S. Ticket; his old forced Pokédex-count sequence does not ambush the player outside home.
3. On talking to Mum *after* Oak, she describes how the handover works. **Do not remove a single item or coin at this stage.**
4. On S.S. Anne pier, choose "Sail to Hoenn" or "Not yet". On confirmation, a preflight capacity validation runs. If any check fails, leave every slot/flag/account unchanged and allow the player to cancel.
5. On a successful confirmed departure, move the entire current party into the sorted PC and clear the active party; store all current Kanto money safely under an independently saved region-bank record; remove *all Bag contents in every pocket* (including all TMs/HMs, Balls, berries, held items in Bag and key items) as a permanent donation to the Training School **except** EXP SHARE and REMOTE PC. Never remove existing Pokémon held items, including evolution stones. Any mandatory travel permissions/tickets must be backed by persistent event flags, not carried key items.
6. Begin Hoenn with the approved PRL starter choice: Poochyena / Sableye / Ralts (not the old Emerald starters). Starting funds/items for Hoenn must be separate from Kanto banked money. The player is a Kanto Champion, not a novice; retain Trainer ID/name, National Dex/capture progress and complete Kanto history.
7. Hoenn progression/badges/Hall of Fame/quests have their own status, independent of Kanto's `FLAG_SYS_GAME_CLEAR`. Existing Hoenn vanilla scripts use this flag in several places; audit before allowing first arrival so Hoenn does *not* begin in postgame.
8. Hoenn's one-way access is provisional until post-three-region free travel; never strand a character in Kanto or Hoenn on bad/failing save.

## PC storage migration: high risk
Current `include/pokemon_storage_system.h`: `TOTAL_BOXES_COUNT = 14`, `IN_BOX_COUNT = 30` (420 slots). Increasing `TOTAL_BOXES_COUNT` to 21 **without safe migration** will change the storage struct, risk save corruption and invalidate existing-save assumptions.
Desired presentation: level-tier boxes 1–10 (1–10, 11–20, ... 91–100), matching level-tier overflow boxes, and a separate LEGENDS box. Sort *ascending by actual level* on travel day only, preserving all Pokémon data, personality, OT, moves, ribbons, ability, friendship, shiny flags, EV/IVs and held items. Overflow should never overwrite any Pokémon. The number of boxes and exact sector allocation require a measured save-layout review before implementation. If capacity is insufficient, deny boarding with a specific message and no mutation.
On entry to Hoenn, box withdrawal of Kanto-owned Pokémon is limited by badges (provisional mapping: 0=Lv10, 1=20, 2=25, 3=30, 4=35, 5=40, 6=45, 7=50, 8=uncapped), without interfering with native Hoenn Pokémon progression. Previously withdrawn legal Pokémon may level normally. Apply the same generic region ownership tags for eventual Johto and return travel.

## Reliability checks before any actual boarding
- Test old PR70 Hall of Fame saves, older Kanto saves, and starts from a brand new save.
- Complete the first Elite Four, verify actual saved location and replay credits; do not trust static checks as proof of a safe GPU or map transition.
- Enter/leave Pallet house, talk to Mum before/after Oak, re-enter/reload, speak to Oak repeatedly with/without ticket, full Bag, and skip original National Dex cutscene.
- Travel preparation dialog must not modify money, Bag, PC, party, badges or flag tables outside its own state.
- For full transfer later: exercise 0/6/fully occupied party, 0/419/420+ boxes; held evolution stone; Eggs; fusions; unique legends; bag/key item and TM edge cases; money max; save before/after crossing; reset during transition. Ensure full transaction commit or rollback on capacity failures.
- The ferry must not spawn in a map with unavailable exits, make Kanto Fly reachable prematurely, or accidentally unlock Emerald events.
- Preserve existing PRL menu/QoL sources over older PHR equivalents, and migrate compatible PHR creatures, assets, trainer teams, story and Relic Journal in separable commits.

## Stage breakdown
1. **This PR:** Kanto homecoming + non-destructive preparation, regression checks, GBA build, emulator QA.
2. **Next:** Save-layout audit, safe PC expansion/level sorting, region flags and bank data with rollback checks.
3. **Then:** S.S. Anne pier confirmation + atomic handover + first Hoenn landing/approved starter.
4. **Next:** PHR Hoenn-specific mechanics/content migration, region-specific gyms and League; thorough emulator regression.
5. **Later:** Hoenn → Johto move and, eventually, 24-badge cross-region free travel and level-100 regional Elite Four rematches.

No automatic migration, no deletion of a user's old saves and no source-changing content copy without compatibility checks.

## PR74 stage A — non-destructive boarding preflight
The post-PR73 Vermilion ferry sailor recognises Oak's Hoenn ticket. Talking to the sailor offers a **yes/no storage readiness check** for the S.S. Anne, uses `PRLHoennCheckPCSpaceForParty` to count free slots against every occupied party slot (including Eggs), and says if there is not enough room. The preview performs no transfer or warp. It must not take money, donate Bag contents, reorganise boxes, or move/remove Pokémon. This is an explicit temporary safeguard **until real migration is validated**, not a claim that Hoenn is playable.
The real departure must also preserve Sevii Island ferry access; existing Seagallop routes must not be regressed by this new sailor branch (declining the Hoenn check routes to the pre-existing Seagallop menu after the original ship departs, preserving travel; manually playtest route selection).
Do not convert this non-destructive checkpoint into an actual sail merely by changing its last message. It requires independent Kanto/Hoenn story flags, PC save migration, atomic cash/Bag transfer, Hoenn arrival/starter, and end-to-end emulation first.

## Exact GBA save constraints from source audit
- `include/save.h` divides the 128-KB Flash into **two 14-sector save slots** (28 sectors), Hall of Fame sectors (28–29), Trainer Hill (30), and recorded battle (31).
- Each 4-KB sector holds **3968 bytes** of SaveBlock data + 116 bytes SaveBlock3 + 12 bytes footer. Storage uses **9 sectors**, so total fixed storage capacity is `9 * 3968 = 35,712` bytes.
- `include/pokemon_storage_system.h` currently allocates 14 boxes × 30 BoxPokemon (80 bytes each) = **33,600 bytes** just for Pokémon, with box names/wallpapers/fusions using more storage.
- The proposed **21 boxes × 30 × 80 = 50,400 bytes** for Pokémon alone cannot fit the fixed 35,712-byte storage partition. Simply changing `TOTAL_BOXES_COUNT` is structurally unsafe and breaks existing .sav files.
- **Safe near-term implementation option:** preserve the 14 physical boxes/420 slots. Label 10 as level tiers, 3 as overflow, 1 as LEGENDS; sort all occupied Pokémon by level at departure using in-place scratch/storage and preserve everything, especially held items. This is a possible compromise requiring user agreement for fewer overflow slots; it is **not yet implemented**.
- The 21-box architecture needs verified data compression, separately versioned extended-save layout, or an equivalent migration; standard 128 KB Pizza Boy A save compatibility must be empirically checked.

## PR75 — carry over PKR's verified Mew-at-the-truck Easter egg
- **Location:** Kanto, S.S. Anne Exterior off the Vermilion harbour (NOT the Hoenn arrival moving truck). Compatible with PR74's working sailor; do not change any ferry menu or destination scripts.
- **Donor authority:** PKR V18 verification notes, user-confirmed 14 Sep 2026: broad 9 × 8 normal A-button trigger region **map-local x55–63, y0–7**, because old guessed truck face coordinates failed. Leave existing hidden Lava Cookie at x58/y28 unchanged.
- **Encounter:** Kanto Hall of Fame completed, truck starts shaking, Mew cries and starts **Lv50 Mew** legendary battle. KO, run, Teleport or flee **do not close** event; interact again to retry. Only **B_OUTCOME_CAUGHT** sets dedicated permanent `FLAG_PRL_SSANNE_TRUCK_MEW_CAUGHT`, alias reserved `FLAG_UNUSED_0x4DD`. The Faraway Island original Mew flags and scripts remain untouched.
- **Save protection:** Existing saves do not require migration; new caught flag defaults false. Do not remove/warp any existing truck events or alter the sailor/Sevii code.
- **Testing required:** On an accepted PR73/74 Champion save, navigate to truck and interact from the accessible area; verify x55–63/y0–7 are truck tiles and not walkable free ground, full party/bag capture behavior, KO/run return to field with retry, capture persists after save/reload, pre-Champion text, hidden Lava Cookie still collectible, and ability to walk back through the pier. These must be in-game checked before user confirms.
