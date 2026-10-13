# Great Badge Heist Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship one FireRed ROM with the complete Great Badge Heist, Giovanni's return, two approved balloon escapes, and the Edensaur/Charaxis back-sprite fixes.

**Architecture:** Keep the original badge system immutable and layer the quest on new persistent flags. Reuse PRL's existing map scripting, trainer battle, final-duo transition/art, and trainer-scoped Giovanni graphics; add a destination-level Victory Road fallback for bypass protection.

**Tech Stack:** pokeemerald-expansion FRLG C/assembly map scripts, trainerproc party data, Python source regressions, GNU Make/devkitARM, mGBA.

**Spec:** `docs/superpowers/specs/2026-10-12-great-badge-heist-design.md`

## Global Constraints

- Base source is `1f0127d8` plus the worktree-ignore setup commit.
- Never clear or repurpose `FLAG_BADGE01_GET` through `FLAG_BADGE08_GET`.
- Preserve all source and gameplay changes already present on the Giovanni Skeletons branch.
- Use the approved balloon bytes from `PRL_Team_Rocket_COMPLETE_Donor_Bank_2026-10-09.zip`.
- Do not award first-clear Gym rewards from recovery battles.
- Final Jessie/James artwork remains scoped to `TRAINER_JESSIE_JAMES_VICTORY_ROAD`.

## Review Focus

- Existing save already inside Victory Road while recovery is active is redirected safely.
- Seven recovered badges cannot reveal or battle Giovanni before the correct state.
- Losing a recovery battle cannot set its flag or all-recovered state.
- Original badge/HM checks remain unchanged and functional during the quest.
- Final-duo defeat permanently unlocks both Route 23 Victory Road warps.

---

### Task 1: Persistent quest contract and tests

**Files:**
- Create: `tests/test_prl_great_badge_heist_20261012.py`
- Modify: `include/constants/flags.h`

**Interfaces:**
- Produces: named quest and visibility flags consumed by every later task.

- [ ] Write source-level tests for unique flags, original-badge preservation, eight recovery states, and completion gates.
- [ ] Run the focused test and confirm it fails because the quest does not exist.
- [ ] Add only the required persistent flag constants.
- [ ] Run the focused test and confirm the flag contract passes.
- [ ] Commit.

### Task 2: Route 23 scenes, balloon, and Victory Road gate

**Files:**
- Modify: `data/maps/Route23_Frlg/scripts.inc`
- Modify: `data/maps/Route23_Frlg/map.json`
- Modify: `data/scripts/route23.inc`
- Modify: `src/overworld.c`
- Modify: object-event graphics tables/constants under `src/data/object_events/`, `src/event_object_movement.c`, and `include/constants/event_objects.h`
- Create: approved balloon graphics/palette files under `graphics/object_events/`
- Modify: `data/maps/VictoryRoad_3F_Frlg/scripts.inc`
- Modify: `data/maps/VictoryRoad_3F_Frlg/map.json`

**Interfaces:**
- Consumes: quest flags from Task 1.
- Produces: one-time heist, reusable badge checks, final encounter, and hard Victory Road lock.

- [ ] Extend the focused tests for both scenes, approved balloon checksum/dimensions, actor choreography, final trainer ID, and bypass handling; confirm RED.
- [ ] Integrate the balloon object graphic and Route 23 actors/scripts.
- [ ] Make badge guards require recovery flags while the quest is active.
- [ ] Move the final duo encounter from Victory Road 3F to Route 23.
- [ ] Add destination redirection for active-quest Victory Road loads/warps.
- [ ] Run focused and existing duo tests; update superseded assertions only where the approved story moved.
- [ ] Commit.

### Task 3: Eight dedicated recovery battles

**Files:**
- Modify: eight Kanto Gym `scripts.inc` files.
- Modify: `src/data/trainers_frlg.party`
- Modify generated trainer constants/output as required by trainerproc.
- Modify: `src/battle_gfx_sfx_util.c`

**Interfaces:**
- Consumes: quest flags from Task 1.
- Produces: eight recovery trainer IDs and recovery callbacks.

- [ ] Add tests for any-order first-seven fights, no duplicate rewards, seven-badge Giovanni gate, six-member late-game teams, and Giovanni-only skeletal art; confirm RED.
- [ ] Add eight late-game trainer records at the end of the FRLG party table.
- [ ] Wire each leader to its recovery battle and dialogue.
- [ ] Reveal and battle Giovanni only after the first seven recovery flags.
- [ ] Extend the skeletal-art predicate to Giovanni's recovery trainer ID.
- [ ] Run trainerproc/focused tests and commit.

### Task 4: Back-sprite positioning

**Files:**
- Modify: `src/data/pokemon/species_info/prl_custom.h`
- Modify: `tests/test_prl_great_badge_heist_20261012.py`

**Interfaces:**
- Produces: exact Edensaur and Charaxis back-sprite offsets.

- [ ] Add exact-offset and front-art-preservation tests; confirm RED.
- [ ] Lower Edensaur to the verified natural offset and Charaxis by exactly one pixel.
- [ ] Run focused tests and commit.

### Task 5: Build and full verification

**Files:**
- Create QA evidence under `.superpowers/sdd/` only.

**Interfaces:**
- Consumes: completed source branch.
- Produces: compiled `.gba`, hashes, screenshots/logs, and compatibility evidence.

- [ ] Run the full Python suite.
- [ ] Build FireRed and verify the ROM is exactly 32 MiB with a valid header.
- [ ] Run mGBA smoke checks with a new save and an existing PRL save.
- [ ] Exercise heist persistence, all eight recoveries, HM permissions, both Victory Road warps, final duo, Giovanni skeletal art, Edensaur, and Charaxis.
- [ ] Capture battle screenshots for the three requested sprite checks.
- [ ] Commit any test-driven fixes.

### Task 6: Review and delivery

**Files:**
- No product changes unless a reviewed finding receives a RED-to-GREEN fix.

**Interfaces:**
- Produces: pushed branch, GitHub PR, and downloadable Library ROM.

- [ ] Run a fresh whole-branch review.
- [ ] Fix Critical/Important findings test-first and rerun the full suite/build.
- [ ] Push `prl/great-badge-heist-20261012` and open a PR against the source branch.
- [ ] Save the compiled ROM to Library and report exact verification status.
