# PRL Kanto recurring double battles — placement, parties and sprites
**Design checkpoint:** 2026-10-09. **Status:** placement/team/asset-reference specification only; NOT installed in playable event scripts. Do not merge this spec as a claim of gameplay completion.

## Preservation rules
- Kanto stays the canonical FireRed geography. Do not replace maps or override PR #9 gym reward/rematch logic.
- Every encounter is a true **double battle**, never a single. Make it unskippable on the required story route by properly gated step trigger / full-width blocker; no reliance on ordinary trainer sight cones.
- Eight separate persistent first-victory flags (4 Twins + 4 Jessie/James); a defeated pair must not force repeat battles after re-entry or save/reload. A blackout must not permanently skip an uncompleted battle.
- Do **not** globally rename the existing FireRed Twins class or ordinary Twins NPCs. Create named recurring trainer records using the same existing battle/front/overworld graphics; preserve other Twins encounters.
- Within this combined game, trainers recognise past Kanto encounters and remember them in Johto and Hoenn. Johto/Hoenn battle levels reset to the new region's curve.
- Route placements are design targets; exact map tile coordinates need Porymap collision/warp/event inspection before implementation.

## Eevee Twins: Steeve & Neevee
**Confirmed, zero-import in-ROM graphics**:
- Front: `graphics/trainers/front_pics/twins_frlg.png` (paired young girls).
- Overworld: `graphics/object_events/pics/people/twin.png` (paired young girls).
- The previously selected reference appears at Library `/Pokemon Sprite Libraries/Trainer Sprite Library/Steeve & Neevee/Steeve_Neevee_SELECTED_FRLG_Twins.png`. Prefer identical **existing PRL** art; no sprite searching/redrawing/recolouring/imports.
- Use a **single Twins-styled double-battle trainer/party** labelled **STEEVE & NEEVEE**, with 2/4/6 Pokémon; avoid claiming separate character-front sprites are necessary.
- Only Eevee and Eeveelutions. All **16** Eeveelutions stay obtainable in Kanto by their previously locked evolution methods; not every evolution needs to appear on the Twins' team. Reserve custom-Eeveelution trainer use for when PRL species/graphics are actually runtime-ready; do not silently invent missing species or overwrite established custom sprites.

| ID | Exact Kanto story placement | Battle entry / gate | Combined party in deployment order |
|---|---|---|---|
| EVTW_K1 | `Route3_Frlg`, east end, mandatory approach to Mt. Moon after Brock | Narrowest safe corridor on Mt Moon approach, not on Charmander gift aide; story check first badge | Eevee Lv16 + Eevee Lv16; held items none |
| EVTW_K2 | `SSAnne_2F_Corridor_Frlg`, route to captain | Before vanilla rival/Captain sequence, avoid rival triggers at x30/31 y6 and captain warp x30 y2; require S.S. Anne access | Vaporeon Lv26 + Jolteon Lv26 lead; Eevee Lv25 + Eevee Lv25 reserve |
| EVTW_K3 | `PokemonTower_1F_Frlg`, approach to upstairs | Appear only after Silph Scope obtained; before the stairway to the rescue route, no conflict with Marowak ghost trigger on 6F | Flareon Lv37 + Glaceon Lv37 lead; Leafeon Lv37 + Umbreon Lv37, Espeon Lv39 + Sylveon Lv39 reserve |
| EVTW_K4 | `Route23_Frlg`, final badge-gate approach to Victory Road | After 8-badge checkpoint, before Victory Road entrance; do not overlap Rocket's later tunnel battle | Vaporeon Lv52 + Jolteon Lv52 lead; Leafeon Lv53 + Umbreon Lv53, Espeon Lv55 + Sylveon Lv55 reserve |

**Character voice:** two enthusiastic, slightly competitive young Eevee collectors; line 1 introduction, line 2 surprise at evolved starters, line 3 familiarity and trust, line 4 last friendly rivalry ahead of Kanto League. Recognise player's earlier battles.

**Moveset reference (all existing vanilla move names, validate legality in PRL's current learnsets before final build):**
- Eevee K1: Quick Attack, Bite, Sand Attack, Tail Whip; second Eevee may use Helping Hand if actually available at level 16.
- K2: Vaporeon — Water Pulse/Bite/Quick Attack/Sand Attack; Jolteon — Thunder Shock/Quick Attack/Double Kick/Thunder Wave; Eevee — Quick Attack/Bite/Swift/Helping Hand.
- K3: Flareon — Flamethrower/Quick Attack/Bite/Sand Attack; Glaceon — Ice Beam/Quick Attack/Bite/Protect; Leafeon — Razor Leaf/Quick Attack/Synthesis/Protect; Umbreon — Feint Attack/Moonlight/Confuse Ray/Protect; Espeon — Psybeam/Quick Attack/Reflect/Calm Mind; Sylveon — Draining Kiss/Swift/Light Screen/Protect.
- K4: upgrade their same Eeveelutions with legal higher-grade STAB, utility and competitive but fair held items (not Mega/Z gimmicks). Verify each chosen move against final available PRL data, do not silently add new moves.

## Team Rocket: Jessie & James
**Battle format:** *one continuous mandatory double-battle encounter for the pair* at each location, even if the two overworld NPCs are rendered as distinct individuals. Never give Jessie or James an unpaired solo battle. Interleave the two characters' leads within a single combined double battle party if that is simplest for this engine.

**Battle-visual donor:** the PJR master sheet pins a **GBA-ready 64×64 / 16-colour Jessie-and-James paired trainer front** from Ody-chan / Pokémon Zero (credited), with source mirror `LucianoNeo/gen1recomp-mods` commit `768f60d9d0f097b9cc65e78099574c296a3f63d2`. Reuse/verify the final converted PJR asset rather than re-ripping a ROM or approximating from screenshot. The **standalone PNG is not currently confirmed copied to PRL**: resolve before marking battle art imported.
**Overworld ready fallback:** `graphics/object_events/pics/people/rocket_f.png` and `rocket_m.png` are confirmed **already GBA-ready in PRL**. A distinctive Jessie/James overworld from an external donor is permitted **only if confirmed GBA-ready with acceptable provenance/palette/frames**; unconverted 128px game-mod sheets are *not* GBA-ready. Until confirmed, use the existing Rocket female/male overworld art for the pair and preserve the original GBA palette limits. Two characters in overworld, one triggered double battle.

| ID | Exact Kanto story placement | Trigger story phase | Jessie & James combined party (lead first) |
|---|---|---|---|
| ROCKET_K1 | `MtMoon_B2F_Frlg`, downstream of Miguel/fossil event | After PKR's already-tested fossil choice/relic dialogue; do not overwrite it | Ekans Lv16 + Koffing Lv16 lead, Bellsprout Lv16 + Meowth Lv17 |
| ROCKET_K2 | `RocketHideout_B4F_Frlg`, approach to Giovanni | Before Giovanni; preserve lift-key and Silph Scope collection, and existing Rocket grunt scripts | Arbok Lv29 + Weezing Lv29 lead; Wobbuffet Lv30 + Meowth Lv30 |
| ROCKET_K3 | `SilphCo_11F_Frlg`, approach to Giovanni | After normal Silph access, before Giovanni; do not interfere with rival or story teleport tile | Arbok Lv39 + Weezing Lv39 lead; Wobbuffet Lv40 + Victreebel Lv40, Pikachu Lv41 + Meowth Lv41 |
| ROCKET_K4 | `VictoryRoad_3F_Frlg`, League-bound final exit passage | Final pre-League Rocket reappearance; do not obstruct boulder puzzles or Route23 Twins | Arbok Lv54 + Weezing Lv54 lead; Wobbuffet Lv55 + Victreebel Lv55, Pikachu Lv56 + Meowth Lv56 |

**Longitudinal canon:** Meowth is a signature recurring member, preferably with its speaking/display identity retained if technically practical; Ekans→Arbok and Koffing→Weezing are returning signatures; Jessie gains Pikachu in Kanto's Silph arc and keeps ordinary Pikachu into Johto. **Never give them Noxichu** (reserved for Red among major NPCs). Late-Kanto Pikachu carries **Light Ball** if available; earlier Silph Pikachu has **Magnet**. Do not award first-time gym rewards for any story trainers. All storyline encounters recognise previous losses and carry into Johto.

## Current asset audit (verified 9 Oct 2026)
| Item | Status |
|---|---|
| Twins front `twins_frlg.png` | **Exists in PRL main** |
| Twins overworld `twin.png` | **Exists in PRL main** |
| Twin combat party records for these 4 battles | **Not yet added/validated** |
| Rocket female/male overworld fallback | **Exists in PRL main** |
| PJR Ody-chan ready 64×64 trainer pair | **Documented GBA-ready donor; direct standalone import into PRL unverified** |
| Dedicated Jessie and James distinctive overworld | **Not proven PRL GBA-ready**; use existing Rocket silhouettes unless ready conversion is confirmed |
| Event flags, map scripting, trainer battles and placement | **Not yet implemented/tested** |

## Implementation checklist
1. Work on a fresh implementation branch created from the most recent validated PRL checkpoint with PR #9 handled explicitly; do not overwrite draft PR #9 or pending bag/PC save work.
2. Review collision, warps, event scripts and all bypass paths of the eight maps in Porymap. Select explicit tile positions and blocking triggers; first-visit and return paths must remain traversable.
3. Register four **new** Eevee Twins double-battle parties with the shared vanilla Twins artwork. Register four Jessie & James combined double-battle trainer parties.
4. Script eight gated first-victory story encounters with dedicated flags. Both visible Rocket NPCs must start the **same** shared duo battle, not separate fights.
5. Preserve existing Mt Moon Miguel/fossil sequence, SS Anne rival/Cut acquisition, Rocket Hideout Giovanni/Silph Scope, Pokemon Tower Marowak/Fuji, Silph Co Giovanni, and Victory Road puzzle/League entry.
6. Run source tests and FireRed build. Emulator-test early arrival, forced triggers, one vs two able Pokémon handling, no duplicate challenge, saving/reloading before and after victories, respawn after wipe, Rocket continuity, and all eight battles' actual double-battle metadata. Mark implementation/build/test state only upon corresponding evidence.

**No runtime implementation was made in this design checkpoint.** The available remote Windows development environment reached its Desktop Commander monthly tool-call limit on 9 Oct; do not describe this branch as a playable Kanto encounter build.
