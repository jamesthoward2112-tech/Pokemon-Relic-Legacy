# PRR — PRU QoL and Kanto Chansey Twins build checkpoint

Date: **15 November 2026**

## Repository and baselines
- Protected PRL donor: `jamesthoward2112-tech/Pokemon-Relic-Legacy`, Kanto-only branch `pkr2/kanto-only-from-pr108-20261016`.
- Separate PRR branch: `prr/from-prl-kanto-20261115` (source parent `4dfaa41e2a1f8da23e90ce117155c2b246e1975c`).
- PRU source donor read-only: `/home/james/pru-patchbuild/verified-alpha5-relearner`, `2c6f87c4` at review.
- PRR worktree `/home/james/kanto-relics/prr-prl-integration`.
- Earlier private working PRR checkpoint remains preserved.

## New source features
1. Exact IV values and EV/IV summary tabs: `P_SUMMARY_SCREEN_IV_EV_INFO=TRUE`, `P_SUMMARY_SCREEN_IV_EV_VALUES=TRUE`.
2. Bag-use eligible held evolution items: `I_USE_EVO_HELD_ITEMS_FROM_BAG=TRUE`.
3. DexNav search timer **30 seconds**, matching PRU.
4. Easier fishing: `I_FISHING_MINIGAME=GEN_2`, one untimed button after bite rather than repeated Gen 3 minigame. Rod bite odds retained; follower boost remains disabled.
5. Fast ordinary Nurse Joy interaction: immediate HealPlayerParty special; standard restoration after whiteout left unchanged.
6. The 12 existing **Kanto** PRL upstairs Chansey trainers are now repeatable **two-Chansey Double Battles**, with first local tier maintained and no new map NPCs. Two usable Pokémon are required by the `trainerbattle_double` engine. Original PRU trainer front artwork copied *byte-for-byte* (`graphics/trainers/front_pics/pru_chansey_twins.png`; SHA-256 `aabcca878b1e775d67886aa869e386fc9af99176945af1c377924654393123b1`) and registered as `TRAINER_PIC_PRR_CHANSEY_TWINS`. The NPC upstairs still uses its existing PRL reception-desk overworld graphics; the **custom portrait** appears on the trainer battle intro.
7. Three approved QoL features already existed: Celadon rooftop Eeveelutions Expert, Repel reuse prompt, and gated R-button Town Map Fly (HM Fly in Bag + field unlock). Do not duplicate them.

**Explicit exclusions:** No following Pokémon, no additional cheat passwords, no changes to unfinished 384-slot PRR Pokémon roster, no new roaming encounter species, no Sevii campaign access.

## Build and checks
- `make firered -j2` **succeeded**.
- ROM `/home/james/kanto-relics/prr-private-builds/prl-derived/PRR_PRU_QoL_ChanseyTwins_20261115.gba` — **33,554,432 bytes**.
- Game code **BPRR**, title `RADICAL RELC`, valid GBA checksum.
- SHA-256 `1dce9e873ec8b80a7db649641e86d5604b5bf6d4c78ca4364eb7ab1e3ff6996c`.
- Full initial suite: **36 tests passed** prior to adding explicit Fly-preservation check; all tests should be rerun against this same ROM before publishing.
- mGBA via Xvfb boot screenshot `PRR_PRU_QoL_ChanseyTwins_Boot.png` (480 × 320, nonblank), smoke exit 0.
- Source build already contains an unrelated existing LittlerootTown script comment warning; build exit 0.

## Outstanding QA / later feature batches
- Manual first-floor Nurse heal, after-whiteout restoration, Rod bite->one-button fishing, DexNav 30-sec timeout, IV/EV tabs, Bag evolution items.
- Upstairs Kanto NPC approach, two usable Pokémon requirement, new Chansey Twins trainer portrait, training repeatability and post-battle return; test save/reload.
- PRU battle backgrounds (modern selector vs old PRR terrain format) — **not imported yet**.
- Kanto-only ambient Pokémon decorative NPCs, PRU Rock Shop relocated into a valid Kanto setting, unique surfing sprites, expanded PRU Eevee Evolution page — **not implemented yet**. User has approved all.
- PRR roster continues separately; any evolution-method text must be validated against final approved data.

Passing a compile/header/boot test is not equivalent to complete in-game verification.
