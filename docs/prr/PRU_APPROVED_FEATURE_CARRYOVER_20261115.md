# PRR — approved PRU feature carryover (15 November 2026)

## User decisions (authoritative for this migration)
**Explicitly rejected:** Following Pokémon. Keep `OW_FOLLOWERS_ENABLED=FALSE`; many PRR species have no validated walking/overworld sprite. Do not switch it back on, even to solve fishing-related mechanics. **No new cheat passwords** beyond DexAll and TeamPreview on the PC.

**Approved:**
1. Ambient overworld Pokémon across Kanto. **Kanto-native species only** (not Johto/Hoenn imports, relic form replacements, or roaming wild encounter changes). Use PRU placement/design as a visual inspiration. Decorative NPCs must not change the unfinished 384-Pokémon roster or wild encounters. Only use verified overworld art.
2. Show numerical IV values and an EV information viewer on the Pokémon summary screen.
3. Use held evolution items from the Bag where the evolution data permits (e.g. Electirizer); don't override permanent Relic methods.
4. Increase the DexNav timeout to 30 seconds (PRU setting).
5. Unique surfing Pokémon visuals using approved Kanto sprites; do not require followers.
6. PRU's larger Eevee-family Evolution page and method descriptions; complete after the final PRR roster identities are verified, preserving already active HGSS Dex screen.
7. Eeveelutions Expert: retain existing Celadon Condominium rooftop NPC; audit its methods/names with the final approved PRR roster. Do not create a duplicate NPC.
8. PRU Rock Shop: a dedicated Kanto service selling evolution stones (including Ancient Stone) with badge-dependent advanced stock. PRU's donor is `data/maps/EcruteakCity_RockShop_hns/scripts.inc`; **never import the Johto map to PRR**. Kanto placement/design pending, considering Pewter City.
9. Repel / Lure re-use prompt: already enabled in `include/config/item.h` (`I_REPEL_LURE_MENU=TRUE`); preserve, do not duplicate.
10. Fly directly from the map: **already implemented in PRL**. The Town Map supports **R to Fly**, gated by field-move unlock, HM Fly in the Bag and valid destinations; preserve and play-test, do not duplicate.
11. PRU-approved battle backgrounds: port validated authored battle art/palettes/animation/layout, with mapping by Kanto environment. Original PRU has modern environment selectors and 34 standard/modern folders. PRR has 12 canonical folders and a different source-asset pipeline; **no blind file copying**. Retain previous PRR backgrounds until a compiling, visually QA'd replacement.
12. Repeatable **two-Chansey Double Battles upstairs** in Kanto Pokémon Centres, using PRU's exact custom `graphics/trainers/front_pics/pru_chansey_twins.png` portrait. PRL already has 12 Kanto upstairs training NPCs, so modify their trainer teams/scripts rather than adding new NPCs. Require two usable Pokémon; retain per-center level tier and repeatability. Keep inaccessible Sevii content unchanged.
13. Easier fishing: remove repeated timed Gen 3 reeling prompts; single-button Gen 2 minigame, retaining normal rod encounter and bite odds. Follower fishing boost must stay off.
14. Faster Nurse Joy healing: PRL already skips dialogue; eliminate the extended normal healing animation, preserving white-out restoration.

## Stage 1 source changes — committed only after successful tests
- `include/config/summary_screen.h`: `P_SUMMARY_SCREEN_IV_EV_INFO TRUE`, `P_SUMMARY_SCREEN_IV_EV_VALUES TRUE`.
- `include/config/item.h`: `I_USE_EVO_HELD_ITEMS_FROM_BAG TRUE`; existing Repel menu untouched.
- `include/config/dexnav.h`: `DEXNAV_TIMEOUT 30`.
- `include/config/fishing.h`: `I_FISHING_MINIGAME GEN_2`, follower boost remains FALSE.
- `data/scripts/pkmn_center_nurse_frlg.inc`: instant direct `HealPlayerParty` during regular nurse interaction; whiteout flow unchanged.
- `tests/prr/test_pru_qol_carryover.py`: regression checks for these features and no followers.

## Stage 2 Chansey source migration
- PRU original custom Chansey Twins 64×64 indexed PNG is copied byte-identically; SHA-256 `aabcca878b1e775d67886aa869e386fc9af99176945af1c377924654393123b1`.
- Append non-shifting trainer portrait enum and graphics mapping.
- Convert PRL's existing 12 Kanto upstairs trainer scripts from six-Chansey single battles to repeatable two-Chansey Double Battles using original PRU portrait; retain the first local level tier at each Center. Other NPCs and scenery remain.
- `tests/prr/test_pru_chansey_twins.py`: guards all 12 trainer teams/scripts, art hash, and original upstairs NPCs.
- The source is not considered gameplay-tested until actual two-live-Pokémon eligibility, battle portrait, save/retry and later-level fights have been tried.

## Remaining independent work
- Approved Rock Shop in Kanto (not Ecruteak).
- Kanto ambient Pokémon placements based on validated PRU assets, with route-specific map collision review and non-encounter behaviour.
- PRU's selected battle backgrounds, animation/palette integration and full Kanto emulator visual QA.
- Species-specific surf mounts and large Eevee Evolution display. The existing map Fly command still requires runtime QA.
- The existing 384-slot roster is a guarded donor manifest and must not be edited in these tasks.
- Do not add easy-catch, free items, EXP cheat passwords, follower sprites, or a four-ability system.

## Verification rules
For each batch, run all `tests/prr/test_*.py` and `make firered`, check GBA header/code BPRR, run mGBA smoke test and retain private ROM hash. Source tests and emulator boot tests are not substitutes for end-to-end battle/event testing. Never overwrite the verified previous build or original PRL/PRU.
