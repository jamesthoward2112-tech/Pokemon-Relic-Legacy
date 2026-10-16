# Pokémon Kanto Relics 2.0 — standalone Kanto-only conversion

**Status:** source preservation and scope lock; Hoenn removal has not yet been implemented or emulator-tested.

## Source of truth
- New working branch: `pkr2/kanto-only-20261016`, based on the **latest** PRL PR108 source `6ee9b984521ac6f2beaf995f771a726379e55c88`, not the stale main branch.
- Approved Kanto gameplay reference: PR56 `prl/final-balloon-flow-20261013` at `d62ba9204c6c4180ff2961fb18fe2900cc2b9f44`, and `docs/PRL_FINAL_KANTO_VALIDATION.md`.
- Product name: **Pokémon Kanto Relics 2.0 (PKR 2.0)**. It is **not** a predecessor or required source save for Pokémon Relics Ultimate (PRU).
- Don't rename the repository or merge older base branches over latest approved Kanto assets until the delta audit and exact test cases are complete.

## Locked target
1. Full Kanto adventure beginning in Pallet Town and ending with Kanto's Elite Four/Champion; use the original PRL Kanto map/gameplay/story assets (including modern Kanto patches), not PJR/HNS Johto maps.
2. Preserve all approved Kanto quality-of-life features, custom species, sprites, starter/rival branches, Eevee Twins (Steevee/Neevee), Jessie/James, skeletal-fossil Giovanni, Safari, Legendary birds, Celadon and Indigo shops, Gym rewards, rematches, title/intro animations, Mew truck, the Great Badge Heist and the approved balloon animation.
3. Retain the complete PKR/PRL Kanto archaeological Relic Reports / fossils / gold-bird arc as its **own Kanto-first story**. PRU will separately *adapt* selected motifs to its **Johto-first** chronology and cannot use the same initial events or save.
4. **Remove playable Hoenn** from the finished PKR 2.0 ROM. No S.S. Legacy one-way voyage; no Hoenn starters/regional PC level locks/14-tier boxes/party banking/bag donation/Aunt Rosie/Birch lab travel/dialogue; no Hoenn rematch or map loading from the Kanto story. Replace post-Hall-of-Fame Hoenn prompts with Kanto-only postgame progression, preserving required Sevii/regular ship behavior.
5. **Do not mass-delete engine species/maps/assets.** Kanto's existing Relic species and dependent Pokémon/graphics may rely on shared expansion data. Remove *reachable Hoenn content and narratives* deliberately, then consider dead-data cleanup only after graph/audit/build verification.
6. Keep PKR 2.0 ROM title, internal identity, credits and artifacts distinct from PRU and donor PJR/PRL. Verify new-game and save compatibility before calling a ROM final.

## Specific high-risk transition files from latest PRL source
- `data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc`: post-Hall-of-Fame Hoenn invitation and ticket scripts.
- `data/maps/VermilionCity_Frlg/scripts.inc`: one-way Hoenn sailor, S.S. Legacy, boxing, Bag donation and money transfer.
- Post-Hall-of-Fame Hall of Fame / Oaks' Pokémon research / mother scripts: replace only the **Hoenn** successor states, not established Kanto story flags.
- `docs/PRL_PHR_HOENN_MIGRATION.md`: donor reference only, not a list of changes to copy over.
- `docs/PRL_PKR_FULL_KANTO_DIALOGUE_MIGRATION_2026-10-10.md` and `docs/PRL_KANTO_GOLDEN_BIRD_STORY_CANON_2026-10-10.md`: approved Kanto dialogue canon.
- `docs/PRL_FINAL_KANTO_VALIDATION.md`: replayable Kanto test acceptance, with its limitations.

## Acceptance gates
- Verify the latest Kanto content in PR108 versus known-approved PR56 with a per-file source diff; protect title art and other post-PR56 Kanto hotfixes.
- List all reachable Hoenn flags, scripts, trainers, warps, map connections, Fly/Teleport/respawn exits and storage mutations. Remove/detach them with distinct patches, not a monolithic blind delete.
- Rebuild Kanto-only ROM and perform fresh-save and Champion-save mGBA tests: 8 Gyms, Route 23, Victory Road, E4, Hall of Fame, Mew, postgame, Seagallop ferries where included, title and custom trainers.
- Release only after **real emulator** gameplay confirms no Hoenn transitions, no unexpected map/credits errors, and no loss of Kanto content.

## Relationship to PRU
- **PKR 2.0**: Kanto-first, self-contained Kanto story, adapted from latest PRL Kanto source.
- **PRU**: separate Johto-first standalone Relic Ultimate, built on PJR maps; imports approved species/content and *adapts* lore without prior-ROM assumptions or Kanto-first Giovanni timeline.
- The projects may share donor sprites, moves, species data or scene concepts; they do **not** share a save progression, chapter continuation or mandatory cross-game history.
