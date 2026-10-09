# PRL Kanto recurring double battles — placement, parties and sprites
**Design checkpoint:** 2026-10-09. **Status:** placement/team/asset-reference specification only; NOT installed in playable event scripts. Do not merge this spec as a claim of gameplay completion.

## Preservation rules
- Kanto stays the canonical FireRed geography. Do not replace maps or override PR #9 gym reward/rematch logic.
- Every encounter is a true **double battle**, never a single. Make it unskippable on the required story route by properly gated step trigger / full-width blocker; no reliance on ordinary trainer sight cones.
- Eight separate persistent first-victory flags (4 Twins + 4 Jessie/James); a defeated pair must not force repeat battles after re-entry or save/reload. A blackout must not permanently skip an uncompleted battle.
- Do **not** globally rename the existing FireRed Twins class or ordinary Twins NPCs. Create named recurring trainer records using the same existing battle/front/overworld graphics; preserve other Twins encounters.
- Within this combined game, trainers recognise past Kanto encounters and remember them in Johto and Hoenn. Johto/Hoenn battle levels reset to the new region's curve.
- **Kanto regional isolation, user-corrected 9 Oct:** Kanto's regional content draws from the original 151 Kanto Pokémon and the already-approved PKR/PRL Kantomon/Relic evolutions of Kanto lines only. Do not introduce Johto/Hoenn Pokémon, their new evolutions or later Eeveelutions into Kanto trainers, wild tables, story, gifts or these double-battle teams. Eevee has only Vaporeon, Jolteon and Flareon here. Regional Pokémon access/transition to Johto is DEFERRED until the entire Kanto-to-E4 player run is accepted. Kanto Pokémon may use engine-supported newer moves but not new out-of-region species.
- Route placements are design targets; exact map tile coordinates need Porymap collision/warp/event inspection before implementation.

## Eevee Twins: Steeve & Neevee
**Confirmed, zero-import in-ROM graphics**:
- Front: `graphics/trainers/front_pics/twins_frlg.png` (paired young girls).
- Overworld: `graphics/object_events/pics/people/twin.png` (paired young girls).
- The previously selected reference appears at Library `/Pokemon Sprite Libraries/Trainer Sprite Library/Steeve & Neevee/Steeve_Neevee_SELECTED_FRLG_Twins.png`. Prefer identical **existing PRL** art; no sprite searching/redrawing/recolouring/imports.
- Use a **single Twins-styled double-battle trainer/party** labelled **STEEVE & NEEVEE**, with 2/4/6 Pokémon; avoid claiming separate character-front sprites are necessary.
- Only Eevee, Vaporeon, Jolteon and Flareon in Kanto. Further Eeveelutions remain reserved for later regions as permitted by their own canon; do NOT port later-region evolution methods or sprite variants into Kanto.

| ID | Exact Kanto story placement | Battle entry / gate | Combined party in deployment order |
|---|---|---|---|
| EVTW_K1 | `Route3_Frlg`, east end, mandatory approach to Mt. Moon after Brock | Narrowest safe corridor on Mt Moon approach, not on Charmander gift aide; story check first badge | Eevee Lv16 + Eevee Lv16; held items none |
| EVTW_K2 | `SSAnne_2F_Corridor_Frlg`, route to captain | Before vanilla rival/Captain sequence, avoid rival triggers at x30/31 y6 and captain warp x30 y2; require S.S. Anne access | Vaporeon Lv26 + Jolteon Lv26 lead; Eevee Lv25 + Eevee Lv25 reserve |
| EVTW_K3 | `PokemonTower_1F_Frlg`, approach to upstairs | Appear only after Silph Scope obtained; before the stairway to the rescue route, no conflict with Marowak ghost trigger on 6F | Flareon Lv37 + Vaporeon Lv37 lead; Jolteon Lv36 + Eevee Lv36 reserve |
| EVTW_K4 | `Route23_Frlg`, final badge-gate approach to Victory Road | After 8-badge checkpoint, before Victory Road entrance; do not overlap Rocket's later tunnel battle | Vaporeon Lv52 + Jolteon Lv52 lead; Flareon Lv55 + Eevee Lv51, Eevee Lv52 + Eevee Lv53 reserve |

**Character voice:** two enthusiastic, slightly competitive young Eevee collectors; line 1 introduction, line 2 surprise at evolved starters, line 3 familiarity and trust, line 4 last friendly rivalry ahead of Kanto League. Recognise player's earlier battles.

**Moveset reference (all existing vanilla move names, validate legality in PRL's current learnsets before final build):**
- Eevee K1: Quick Attack, Bite, Sand Attack, Tail Whip; second Eevee may use Helping Hand if actually available at level 16.
- K2: Vaporeon — Water Pulse/Bite/Quick Attack/Sand Attack; Jolteon — Thunder Shock/Quick Attack/Double Kick/Thunder Wave; Eevee — Quick Attack/Bite/Swift/Helping Hand.
- K3: Flareon — Flamethrower/Quick Attack/Bite/Protect; Vaporeon — Surf/Bite/Acid Armor/Protect; Jolteon — Thunderbolt/Quick Attack/Thunder Wave/Protect; Eevee — Swift/Bite/Helping Hand/Quick Attack. Confirm in-engine legality and tune without importing later-region Pokémon.
- K4: upgrade their same Eeveelutions with legal higher-grade STAB, utility and competitive but fair held items (not Mega/Z gimmicks). Verify each chosen move against final available PRL data, do not silently add new moves.

## Team Rocket: Jessie & James
**Battle format:** *one continuous mandatory double-battle encounter for the pair* at each location, even if the two overworld NPCs are rendered as distinct individuals. Never give Jessie or James an unpaired solo battle. Interleave the two characters' leads within a single combined double battle party if that is simplest for this engine.

**Battle-visual donor — SUPERSEDED by verified user-provided donor 9 Oct:** the PJR master sheet pins a **GBA-ready 64×64 / 16-colour Jessie-and-James paired trainer front** from Ody-chan / Pokémon Zero (credited), with source mirror `LucianoNeo/gen1recomp-mods` commit `768f60d9d0f097b9cc65e78099574c296a3f63d2`. Reuse/verify the final converted PJR asset rather than re-ripping a ROM or approximating from screenshot. The **standalone PNG is not currently confirmed copied to PRL**: resolve before marking battle art imported.
**Overworld sprites — SUPERSEDED by verified user-provided donor 9 Oct:** User rejects PRL's generic Rocket female/male sprites for Jessie/James presentation. Source matching distinctive **Jessie and James** overworld art from the original GBA-style Ody-chan/Pokémon Zero donor and the PJR pinned sources, prepare and validate true GBA-compatible indexed palettes, correct directional standing/walking frames, native 16×16/16×32 footprint and offsets, and transparent index. Do NOT use a generic Rocket grunt fallback or 128px source tiles mislabeled ROM-ready. Both humans can have individual overworld sprites, but they must initiate one combined double battle. Battle-front donor is documented as converted but must be located and tested in PRL before the donor gate is complete.
**Meowth NPC follower is mandatory:** already present in PRL at `graphics/pokemon/meowth/overworld.png` and `graphics/object_events/pics/pokemon_old/meowth.png`; use whichever is confirmed supported by the current object-event tables. When Jessie and James walk as part of scripted scenes, Meowth must visibly move one step behind (not teleport, slide, lag or obstruct the player); on scene end, hide all three cleanly, restoring player control. Implement scene-local NPC follower/path coordination rather than accidentally attaching Meowth to the player party-follow system. Regression-test turns, collision, offscreen, reload and warp. If full walking fidelity cannot be proved, mark visual gate incomplete rather than fake it.

| ID | Exact Kanto story placement | Trigger story phase | Jessie & James combined party (lead first) |
|---|---|---|---|
| ROCKET_K1 | `MtMoon_B2F_Frlg`, downstream of Miguel/fossil event | After PKR's already-tested fossil choice/relic dialogue; do not overwrite it | Ekans Lv16 + Koffing Lv16 lead, Bellsprout Lv16 + Meowth Lv17 |
| ROCKET_K2 | `RocketHideout_B4F_Frlg`, approach to Giovanni | Before Giovanni; preserve lift-key and Silph Scope collection, and existing Rocket grunt scripts | Arbok Lv29 + Weezing Lv29 lead; Raticate Lv30 + Meowth Lv30 |
| ROCKET_K3 | `SilphCo_11F_Frlg`, approach to Giovanni | After normal Silph access, before Giovanni; do not interfere with rival or story teleport tile | Arbok Lv39 + Weezing Lv39 lead; Drowzee Lv40 + Victreebel Lv40, Pikachu Lv41 + Meowth Lv41 |
| ROCKET_K4 | `VictoryRoad_3F_Frlg`, League-bound final exit passage | Final pre-League Rocket reappearance; do not obstruct boulder puzzles or Route23 Twins | Arbok Lv54 + Weezing Lv54 lead; Hypno Lv55 + Victreebel Lv55, Pikachu Lv56 + Meowth Lv56 |

**Longitudinal canon:** Meowth is a signature recurring member, preferably with its speaking/display identity retained if technically practical; Ekans→Arbok and Koffing→Weezing are returning signatures; Jessie gains Pikachu in Kanto's Silph arc and keeps ordinary Pikachu into Johto. **Never give them Noxichu** (reserved for Red among major NPCs). Late-Kanto Pikachu carries **Light Ball** if available; earlier Silph Pikachu has **Magnet**. Do not award first-time gym rewards for any story trainers. All storyline encounters recognise previous losses and carry into Johto.

## Current asset audit (verified 9 Oct 2026)
| Item | Status |
|---|---|
| Twins front `twins_frlg.png` | **Exists in PRL main** |
| Twins overworld `twin.png` | **Exists in PRL main** |
| Twin combat party records for these 4 battles | **Not yet added/validated** |
| Existing generic Rocket female/male overworld | **Present but USER-REJECTED for Jessie/James presentation; don't use** |
| Legacy PJR Ody-chan 64×64 trainer pair (no longer preferred) | **Documented GBA-ready donor; direct standalone import into PRL unverified** |
| Dedicated Jessie and James distinctive overworld | **Required** from credited Pokémon Zero/Ody-chan GBA donor; pinned PJR sources documented, conversion/PRL in-game preview still to verify |
| Meowth Pokémon overworld art | **Exists in PRL main**; three-character NPC follow scene unimplemented |
| Event flags, map scripting, trainer battles and placement | **Not yet implemented/tested** |

## Implementation checklist
1. Work on a fresh implementation branch created from the most recent validated PRL checkpoint with PR #9 handled explicitly; do not overwrite draft PR #9 or pending bag/PC save work.
2. Review collision, warps, event scripts and all bypass paths of the eight maps in Porymap. Select explicit tile positions and blocking triggers; first-visit and return paths must remain traversable.
3. Register four **new** Eevee Twins double-battle parties with the shared vanilla Twins artwork. Register four Jessie & James combined double-battle trainer parties.
4. Script eight gated first-victory story encounters with dedicated flags. Both visible Rocket NPCs must start the **same** shared duo battle, not separate fights; Meowth accompanies both when they walk.
5. Preserve existing Mt Moon Miguel/fossil sequence, SS Anne rival/Cut acquisition, Rocket Hideout Giovanni/Silph Scope, Pokemon Tower Marowak/Fuji, Silph Co Giovanni, and Victory Road puzzle/League entry.
6. Run source tests and FireRed build. Emulator-test early arrival, forced triggers, one vs two able Pokémon handling, no duplicate challenge, saving/reloading before and after victories, respawn after wipe, Rocket continuity, and all eight battles' actual double-battle metadata. Mark implementation/build/test state only upon corresponding evidence.

**Deferred, not to be implemented in this Kanto-only chapter:** player suggests post-Elite-Four S.S. Anne as region-travel vessel to Johto. Keep that as a future investigation, not an active transition; inspect the actual PKR 'new encounter method' before attempting to copy or reinterpret it.

**PKR dialogue contract:** Restore all authentic PKR Kanto NPC/story dialogue from source/ROM reference where extractable, keeping existing player's tested Oak introduction and Miguel Mt Moon interactions, and add carefully separated hints that Relic evolution research and unresolved Rocket business continue in Johto/Hoenn; no early transition or spoilers. Inventory/map/story audit should cover Pallet through first Kanto E4 including Giovanni, Rocket, legendaries and relic availability.

**No runtime implementation was made in this design checkpoint.** The available remote Windows development environment reached its Desktop Commander monthly tool-call limit on 9 Oct; do not describe this branch as a playable Kanto encounter build.

## Updated source-of-truth Rocket sprite donor — 9 October 2026
The user supplied a stronger, dedicated Rocket character archive **already stored in the ChatGPT Library**:
- Folder: `/Pokémon Relic Legacy/Sprite Resources/Team Rocket Jessie James/`
- Archive: `PRL_Jessie_James_Meowth_Donor_and_ROM_Audit.zip`; notes: `PRL_Jessie_James_ROM_Audit.md`.
- Donor: FireRed BPRE hack *Pokémon Team Rocket Jessie & James* attributed to **MrNightology** (source-ROM SHA-256 `03b53a3594d7f67afe3c4b6de7127027d8ae4ac64961b59a51fe1cbd0578fe53`). Check artwork credits and redistribution permissions before any release.
- Primary **white-uniform** battle sprites: `01_Approved_Character_Donors/Jessie/battle_white_uniform/front_64x64.png` and `.../James/battle_white_uniform/front_64x64.png`. Each is 64×64 with no more than 16 RGBA colours including transparency. Alternative dark/black attire is archived but **not** selected by default.
- **Walking overworld** sprites: `01_Approved_Character_Donors/Jessie/overworld_walk/` and `.../James/overworld_walk/`. Each has nine 16×32 PNG frames plus matching nine **256-byte original 4bpp frames** and a **32-byte original RGB555 palette**.
- Validation performed 9 October 2026: ZIP CRC check passed. A source-accurate decode of all nine 16×32 walking frames for each person matched the original 4bpp pixels and RGB555 palettes at **4608/4608 pixels per character** (no RGB or transparency mismatches). This establishes correctly preserved indexed donor assets, **not** emulator runtime compatibility.
- The archive contains a Meowth-themed *balloon*, not a usable walking Meowth. Ignore its unverified small-animal candidate. Instead use **PRL's existing Meowth overworld art** from `graphics/pokemon/meowth/overworld.png` or `graphics/object_events/pics/pokemon_old/meowth.png` after confirming frames and registration in the PRL object-graphics tables.
- **Behavior:** Jessie & James always appear as a duo, they fight a single **double battle**, and when walking the Meowth sprite walks with/follows them, synchronizing facing, turns, pauses and collision. It must not attach to the player's own follower system.
- Do not keep existing generic `rocket_f.png`/`rocket_m.png` as Jessie & James. The **newly supplied donor overrides older references to Ody-chan/Pokémon Zero** for these PRL characters. All other art in the archive remains unapproved for import.
- **NOT YET IMPLEMENTED:** The newly supplied images still need proper PRL asset registration, OAM/animation/frame-table mapping, palettes, in-engine comparison screenshots, working Meowth follower and FireRed ROM/emulator test before calling them production-ready.
