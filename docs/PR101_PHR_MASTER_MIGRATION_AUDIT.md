# PRL — PHR master migration audit (15 October 2026)

Source of truth: `Pokemon_Hoenn_Relics_Master_Sheet_v0.16.xlsx` (all **31 worksheets** inspected), PHR `develop` donor repository `jamesthoward2112-tech/Pokemon-Hoenn-Relics`, `docs/H010_OPENING_WATTSON.md`, `docs/H021_ELITE_FOUR_CHAMPION.md`, and latest PRL canonical Golden Feather Saga/Hoenn donor migration bank specifications in Notion.

**Important:** Audit of the master design is NOT evidence that features have all been migrated or gameplay-tested. Every future donor change needs source-diff, build, converted-save emulator test and existing Kanto regression check. Original donor assets/source stay unchanged.

## Superseding PRL rules

- Kanto→Hoenn→Johto order, region-gated Pokémon withdrawal, Kanto Bag transfer/donation to Trainer School, Mum in Kanto retains savings.
- Hoenn starter choices are **Ralts, Poochyena, Gemlin** offered by Birch's sprite previews, not PHR's Pichu/Azurill/Wynaut or traditional Treecko/Torchic/Mudkip opening. Traditional Hoenn starters are later obtainable gifts.
- Ralts→**Valkyvoir** (Psychic/Fairy, Pixilate, Boomburst, Shiny Stone + Lv50+); Poochyena→Mightyena→**Skulberus** via Hard Stone; **Gemlin** becomes Sableye at night. Do not revert to Hyenadon as the Mightyena evolution.
- Relic Torkoal and Tropius outcomes follow the *later* approved **Tortemple** and **Brontonana** choices, not obsolete Torkaldera/Tropisaur. Confirm art, species entries, evolution compatibility before inserting.
- **Uncle Frank** replaces Norman-as-father; **Aunt Rosie** replaces older draft Aunt Hazel. Both need appropriate regional dialogue and home scripts.
- **Coelossus**: direct Sealed Chamber guardian encounter after all ten guardians, *not* ordinary Relicanth evolving or requiring the one-of-a-kind Kanto shiny Wonder Guard Relicanth.
- **Astrachi** concludes Hoenn. **Golden Relic Ho-Oh**, final Golden Feather payoff, belongs to Johto. Do not spoil that in Hoenn.
- Keep PRL's completed Kanto implementation (intros, Eevee Twins, J&J, Celebi/title, Trainer Card, box guards) and existing save layout untouched.

## Sheet-by-sheet intake and migration queue

| # | PHR sheet | What to migrate / reconcile | Status in PRL as of PR101 |
|---|---|---|---|
| 01 | Overview | Production scope, release milestones, 215 target Hoenn Dex | Source audited; PRL species mapping must supersede raw PHR indexing. |
| 02 | Canon | Resonance, Oak continuation, guardian network, mythology | Story reference, conflicting old Coelossus/Relicanth rule superseded. |
| 03 | Story | Core Magma/Aqua/resonance journey | Unported in full; Golden Feather Saga wins on ties. |
| 04 | Hoennmon | Stats/types/evolutions/abilities and normal/shiny art | Several species already in PRL; reconcile replacements and final forms. |
| 05 | Characters | Birch, rival, Steven, Wallace, Magma/Aqua, aides, Norman | Character roles pending full port; Uncle Frank and Aunt Rosie override. |
| 06 | Locations | Mural rooms, sanctuary, museums, relic altars, chambers | Not yet opened as a coherent in-ROM Hoenn region. |
| 07 | QoL & Systems | Remote PC, DexNav, HM field use, Fly, L ball, R run, relearn | Preserve current PRL working baseline; inspect source parity, do not wholesale replace. |
| 08 | PKR Reuse Map | Original Relic rules/species/HM/gameplay migration | Use as parity checklist; newer PRL overrides Kanto gameplay. |
| 09 | Production | Source/art/task gates, testing deliverables | Convert to PRL build-and-emulator validation. |
| 10 | Engine Baseline | Expansion mechanics configuration | PRL FireRed-compatible current engine; do not rebase engine silently. |
| 11 | Regression Gates | Gym/story/legend and save checks | Add as targeted PRL tests + full converted-save playthrough. |
| 12 | H005 Source Plan | Source patch structure | Donor review; not directly copy-pastable into PRL. |
| 13 | H006 Art Pipeline | Indexed sprite constraints/front/back/icon/palettes | Validate exact user-approved replacements; Eeveeon icon separately broken. |
| 14 | Starter & Gift Plan | Native Treecko/Torchic/Mudkip gifts and starter rite | Original opening replaced; gift network and rite need port. |
| 15 | Species Rebalance | 82 explicit Hoenn species role/stat targets | Comparative audit and selective source migration pending. |
| 16 | Relicanth Legacy | Normal vs unique Relicanth, guardians | Direct-encounter Coelossus override; prevent regular Relicanth overlap. |
| 17 | Moves & Abilities | Move effect identities and ability patches | Evaluate against already-integrated Elite Redux battle mechanics. |
| 18 | Field Effects | Weather and special status field rules | Parity test against current PRL rather than overwrite. |
| 19 | Global Battle Locks | Battler-level core rules and balance constraints | Port compatible locks only; first Kanto implementation takes priority. |
| 20 | Full Dex Balance Audit | Complete Hoenn Dex: 200+ individual entries | Full data diff still required, not implemented merely by audit. |
| 21 | Learnset & TM Pass | 90+ detailed species/move entries | Detailed diff and teaching compatibility port pending. |
| 22 | Full Story v0.13 | 58+ location-by-location story beats | Major story port pending; original Route101 rescue must never re-trigger. |
| 23 | Aides Gifts Items | Mira, Lio, Rowan, Kiri, Neris, Sol and others | Gift/service NPCs to script; Rosie Town Map gift is newer PRL choice. |
| 24 | Rival Teams | Eight primary, ally, postgame/rival rematch encounters | Rework rival ace for PRL starter branches. |
| 25 | Bosses & TRE Art | Boss encounter teams, presentations, sprite donors | Port only approved art/team selections. |
| 26 | Gym Rematch Circuit | Eight story gym rewards (TM + item + ₽10,000), buffed rematches | Planned staged gyms and ultimate cross-region rematches. |
| 27 | Shops & TM Plan | Half-price balls, 2.5× Premier Ball, TM counters, stones | Implement and test economy once marts open. |
| 28 | Feature Decisions | R run, L last ball, HM in bag, Remote PC, DexNav, previews | Compare feature by feature to existing PRL functionality. |
| 29 | Missed Items Check | Fossils, reward alternatives, missable gifts, catalysts | Verify all availability and one-time flags in eventual playable maps. |
| 30 | v0.14 Update Log | 17 Sep consolidated locks and superseding donor data | Historical donor baseline; later PRL canon wins. |
| 31 | v0.15 Sprite Lock | 18 Sep front/back/icon approvals | Audit each donor vs later PRL-approved imagery before replacement. |

## Immediate open-map implementation order

1. **PR101**: character dialogue and PC badge-count warnings, Rosie entering house and Town Map on post-starter talk. Source and compilation check; leave PR100 stable for gameplay.
2. **PR102**: Route101→Oldale boundary, family houses, Oldale Center/Mart/houses, safe compiled connections/warps and binary pointer verification. Route102/Route103 remain CLOSED until verified. Check *actual wild encounters* on Route101; JSON data being present is not sufficient.
3. **Follow-on**: Route102→Petalburg/Wally and Uncle Frank; Route103 rival and reworked relic story; preserve both non-overlapping wild/trainer progression.
4. **Later**: Rustboro/Mr Stone/Devon Lens, Slateport, Mauville, Mt. Chimney and all remaining region/story maps, in connected and playable slices.
5. **Endgame**: ten guardian circuit, direct Coelossus at Sealed Chamber, Astrachi, gym circuit, Hoenn League, return-to-region gating and final Ho-Oh Johto payoff.

## Regression rules

- Never clear or overwrite the customer's original 128-KiB save, `HoennTruck`. Test on a copy.
- Confirm special starter, no duplicate Route101 Birch rescue, Bag/party transfer, all boxed Pokémon retained with correct level tiers, no illegal WITHDRAW or MOVE/SHIFT bypass, no new Kanto regressions.
- Verify every new map has a real, non-NULL compiled header/layout, matching map connections and safe warp destinations; **a source assertion alone is not enough**.
- Verify wild battle encounter frequency and species in *emulator runtime*, NPC dialogue pages, Town Map region, Eeveeon icon, nurse healing/respawn locations, all exit/return routes.
- A passing GitHub Actions ROM build is **not** a guarantee of live gameplay correctness.

## Donor references

- `jamesthoward2112-tech/Pokemon-Hoenn-Relics` develop, `docs/H010_OPENING_WATTSON.md`, `docs/H021_ELITE_FOUR_CHAMPION.md`
- PRL Notion 9 Oct donor migration bank, PRL Notion 13 Oct Golden Feather Saga
- `Pokemon_Hoenn_Relics_Master_Sheet_v0.16.xlsx`, all 31 worksheets.

