# PRL / PHR Hoenn Migration — Source and Dependency Manifest

**Current truth:** PR93 is the tested-compile Littleroot Town + Birch Lab baseline. PR94 is an **unverified draft** to compile Route 101 with its southern Littleroot link; its north border to Oldale remains closed, as do the houses and other Hoenn maps. The PHR campaign is **not** open yet. PR94 requires FireRed CI, compiled pointer checks and in-emulator save-copy tests.

**PR94 Birch rescue continuity:** The player already met Birch and selected Ralts/Poochyena/Gemlin in his lab. FireRed Route 101 OnTransition marks the original Emerald rescue state completed (VAR_ROUTE101_STATE=3, FLAG_RESCUED_BIRCH) and hides the Bag, Zigzagoon and both Birch rescue NPCs; do not run the original Zigzagoon chase, grant another starter or revive the Emerald truck story. The standalone Emerald configuration keeps its original rescue implementation.

## Source inventory

Actual source donor: [Pokemon-Hoenn-Relics](https://github.com/jamesthoward2112-tech/Pokemon-Hoenn-Relics), **branch `feature/h022-postgame-rematches`**. The `main` branch is a scaffold; the H022 branch contains PHR's H005–H022 source overlays, assets, `data/hoennmon_*_v0.15` and design documentation. Its `engine/pokeemerald-expansion` is a pinned expansion **1.17.0 submodule** at `e8bd1cd7b03fc032ea37e3ecd38b379b5d01a1e7`. PRL's build is a different revision/configuration. All PHR `tools/apply_*.py` assume that original engine SHA, so NEVER run them directly against PRL or copy a PHR ROM binary over the PRL ROM. Port intended behavior with separate patches and compatibility tests.

## Sequenced import gates

1. **PR90 entry repair:** new Hoenn relic narration; Birch greets only when approached; only then spawn Ralts/Poochyena/Gemlin balls at original map coordinates on the lab table; prevent repeatedly receiving Running Shoes from Mom even on an existing PR89 Hoenn save. Retain full Kanto party until one Hoenn starter chosen. 
2. **Hoenn map graph:** compile/enable Hoenn in bounded map groups starting with both Littleroot homes, Route 101, Oldale (Center/Mart/interiors), Route 102, Petalburg. Preserve exact numeric map IDs, and validate every connected-map header, layout, tile set, warp, respawn, whiteout, Fly/Teleport/Dig/Escape Rope, object and flag. A newly unlocked road without its destination is a crash risk.
3. **PHR H009/H010 foundation:** port Oak's Kanto resonance research, Birch's Relic Journal, Resonance Lens theft, ten-guardian mural and aide network (Mira, Lio, Rowan, Kiri, Neris, Sol), with PRL's post-Champion provenance. Fix Hoenn Remote PC access as a separate compatibility gate. PHR used `ITEM_POKEMON_BOX_LINK` alias as a Key Item; PRL intended Remote PC access must not be broken by its Bag-clearing transfer.
4. **PHR H005–H008 data/art:** source-audit all Hoennmon species, evolved forms, moves, abilities, back/front/party icons and shinies against PRL's later species, ID and sprite locks. Preserve PRL's approved **Skulberus** (not original PHR Hyenadon), **Tortemple**, **Brontonana**, **Gemlin**, **Valkyvoir**, Noxichu and all other current evolutions. Never silently renumber species or invalidate saved Pokémon.
5. **PHR H010–H021 campaign:** trainer teams, Route 101 onwards, early gym rewards, museum relic theft, volcanic plot, Hoenn weather, Dive, Sootopolis, legendary guardians and Hoenn League. Keep **Ralts / Poochyena / Gemlin** as PRL's Hoenn starter trio, not PHR's old Pichu/Azurill/Wynaut Birch-rescue opening. Replace PHR's Norman-dad premise with **Uncle Frank** in PRL; player arrives as Kanto Champion.
6. **PHR H022/postgame:** source-port the free-order rematch circuit, Mega Ring / Oak-Birch research, strengthened Hoenn E4, Coelossus/Astrachi and later cross-region endgame. Do not use Kanto's game-clear flag to unlock Hoenn postgame prematurely.

## PRL family and PC access canon (do not overwrite with Emerald/PHR defaults)

- The player is the **Kanto Champion**. Their **Mum remains in Kanto** and holds the saved Kanto funds. The Littleroot relative is the player's **Aunt**, never Mum. Norman is **Uncle Frank** in Petalburg, not the player's father.
- Birch explains before selection: after choosing Ralts/Poochyena/Gemlin, Kanto party is boxed safely; equipment is donated to Hoenn's Trainer School; Kanto Mum safeguards the money; Hoenn starts at 3,000; EXP Share, Running Shoes and functional Remote PC Key Item are retained.
- Hoenn PC hard-lock on withdrawals to the party: no Hoenn badges → level 1–10, then +10 per earned Hoenn badge; eighth badge → through 90; Hoenn League clear → 100. Kanto badges never count. This applies to normal Withdraw and Move Pokémon placement/shift, and never deletes boxed Pokémon. Deposits and box reorganisation remain allowed.
- Preserve saved species, OT, shiny, held items, checksums, box layout and out-of-region transfer consistency. Future story content must not restore Emerald's “Mum and Dad” premise.

## Rules

- Preserve PR89's successful Kanto→Hoenn voyage, Celebi/Jirachi title assets, Kanto trainers/scripts, saved-money banking, EXP Share exception, 14×30 boxes and original Trainer ID.
- PRL already has parts of DexNav, HM-without-teaching, move relearning, register/quick item and Relic features; integrate, never duplicate. H014 source should be examined for genuinely missing behaviour.
- Each map group and feature imports as a standalone reviewable PR with compilation, regression and real mGBA/Pizza Boy save-copy playtest. Do not merge or distribute as a full Hoenn adventure before safe map navigation is proven.

Donor source references: `docs/H009_RELIC_JOURNAL.md`, `docs/H010_OPENING_WATTSON.md`, `docs/H014_QOL_INTEGRATION.md`, `docs/H022_POSTGAME_REMATCHES.md`, `tools/apply_h010_story_events.py`, `tools/apply_h014_qol.py`, `tools/apply_h022_postgame.py`.
