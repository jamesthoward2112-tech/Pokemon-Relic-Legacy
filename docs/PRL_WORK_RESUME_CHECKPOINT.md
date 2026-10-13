# PRL Work Resume Checkpoint

Updated: 2026-10-13 UTC

## Repository state

- Branch: `prl/great-badge-heist-20261012`
- Approved base: `1f0127d81e3b29346544c8e7bc8ed14e8dd79a84` (`origin/prl/giovanni-skeletal-fossils-20261012`)
- Current source commit: `a9bf3e29a40c2d3e07176417eca87263b2eb7e16`
- Published source snapshot: `7d8391f5b2c77a0b6f96985cee0bcaa0177b389a` on `prl/great-badge-heist-20261012`. Recovered through connected GitHub access after the old CLI authentication failure. Original local history remains at `fd34a2b3`.
- Build worktree: `/workspace/scratch/b89c0838e65a/great-badge-heist-build`
- Source worktree: `/workspace/scratch/b89c0838e65a/Pokemon-Relic-Legacy/.worktrees/great-badge-heist`

## Last successful ROM

- File: `PRL_Great_Badge_Heist_Giovanni_Skeletons_2026-10-13.gba`
- Size: 33,554,432 bytes
- SHA-256: `5b796c632eaf78a40940072de204ddfefc7d261da0a7c24bf4437b129a4d389a`
- Product source tree: `a9bf3e29` (the detached build commit `37de98f5` has an empty tree diff against `a9bf3e29`)
- Build result: successful, ROM usage 27,655,356 bytes / 32 MiB (82.42%)

## Implemented and verified

- Route 23 one-time Great Badge Heist with Jessie, James and Meowth.
- Persistent recovery flags separate from the original eight Kanto badge flags.
- Save-compatible Surf-edge trigger using elevation 0 and a temporary scene latch.
- Both Meowth balloon scenes use the approved integrated balloon asset. The balloon template/spawn was moved into the actual Route 23 scene so the complete sprite is visible before its five-step takeoff.
- Recovery scripts and stronger late-game teams for Brock, Misty, Lt. Surge, Erika, Koga, Sabrina, Blaine and Giovanni.
- Giovanni remains gated until the first seven recovery flags are set; his recovery battle does not replay the original disbandment sequence.
- Giovanni-only skeletal Kabutops and Aerodactyl graphics remain active and were rendered in mGBA.
- Final Jessie & James double battle uses the approved Team Rocket transition and custom encounter artwork.
- Active-quest Victory Road protection covers normal, dynamic, heal, teleport, escape, dive, fixed-hole, continue-game and direct existing-save entry paths.
- Edensaur, Charaxis and Fortotoise back sprites were rendered in actual mGBA battles; Edensaur and Charaxis use their requested corrected offsets and Fortotoise retains its approved alignment.
- All eight recovery completion scripts set their individual quest flags; Giovanni also sets the all-recovered flag. The original eight badge flags remained set.
- Normal in-game save and cold reload preserved partial quest progress (started + Boulder recovered, Cascade/all-recovered unset).
- A no-save boot reached Professor Oak's new-game introduction.
- Python source regression suite: 193 tests passed.

## mGBA runtime results

Focused harness: `tools/prl/qa_great_badge_heist.py`

- Existing 128 KiB PRL save loaded successfully.
- Heist script completed and original badge flags were preserved.
- Normal Victory Road warp bypass was redirected to Route 23.
- A save whose stored location was directly inside Victory Road was redirected to Route 23 on continue.
- Final Rocket transition and double battle rendered with Edensaur and Charaxis backs.
- Initial and final balloon escape scripts completed; visual contact sheets show the giant Meowth balloon appearing and moving upward.
- Giovanni recovery battle and Fortotoise back rendered.
- Giovanni-only skeletal Kabutops and Aerodactyl rendered.
- Final-defeated/hide flags were set and Victory Road was unlocked after the final callback.

Evidence is under `/workspace/scratch/b89c0838e65a/qa-evidence/final-rom-verification` and `/workspace/scratch/b89c0838e65a/qa-evidence/save-flow` in the current Work environment.

## Partial or outstanding

- The full upstream `make check BUILD=firered` target was started after the release build. Record its final result here if the process finishes before handoff; the focused PRL test suite and release compilation already pass.
- Feature source and checkpoint are now published. ROM delivery completed; GitHub Release binary upload remains unavailable through the connected tool.
- End-to-end battles were rendered and battle/callback progression was exercised with the mGBA harness; every trainer battle was not manually played turn-by-turn to defeat without harness setup.

## Continuation commands

```sh
cd /workspace/scratch/b89c0838e65a/Pokemon-Relic-Legacy/.worktrees/great-badge-heist
git status --short
git log --oneline -12
python3 -m unittest discover -s tests -p 'test_*.py'

cd /workspace/scratch/b89c0838e65a/great-badge-heist-build
PATH=/workspace/scratch/b89c0838e65a/toolchains/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH make firered -j8 -O
sha256sum pokefirered.gba
```

Focused mGBA command:

```sh
cd /workspace/scratch/b89c0838e65a/great-badge-heist-build
PATH=/workspace/scratch/b89c0838e65a/toolchains/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH \
PYTHONPATH=tools/prl \
python3 tools/prl/qa_great_badge_heist.py pokefirered.gba \
  --elf pokefirered.elf \
  --save '/workspace/scratch/b89c0838e65a/qa-input/test(20261011-152102)' \
  --probe /workspace/scratch/b89c0838e65a/deps/libprl_mgba_probe.so \
  --out /workspace/scratch/b89c0838e65a/qa-evidence/resume-run
```

## Recovery session 2026-10-13

- Checkpoint published first as commit `4a2f070e`; full recovered committed source published as `7d8391f5`.
- Fresh Python suite: 193/193 passed.
- Eight uncommitted Charaxis/Fortotoise asset changes and untracked Twins assets remain untouched in the source worktree. Build worktree assets archived separately before restoring committed release assets to ensure source/ROM match. These pending assets are excluded from the approved release.
- Committed-release rebuild passed, matching SHA-256 `5b796c632eaf78a40940072de204ddfefc7d261da0a7c24bf4437b129a4d389a` exactly (32 MiB, 82.42% ROM used).
- Fresh focused mGBA run passed all 12 result assertions; heist balloon and final battle contact sheets visually reviewed. Evidence: `/workspace/scratch/95236103e27c/qa-release/results.json`.
- Pending 12 asset files separately preserved on `prl/recovered-pending-assets-20261013`, commit `c544cbec31be7e55ded1a6843e0b9c8545a68d0e`. Do not merge into the tested release without approval and verification.
- ROM delivered as `PRL_Great_Badge_Heist_Giovanni_Skeletons_2026-10-13(1).gba`, persistent file ID `libfile_fb3df0d8d6a481918a5fde6d71a04f39`, SHA-256 as above. Original local path remains `/workspace/scratch/b89c0838e65a/PRL_Great_Badge_Heist_Giovanni_Skeletons_2026-10-13.gba`.
- Full upstream make-check completion was not recovered. Do not claim it passed.

## Exact next action

User playtest of delivered ROM. No gameplay implementation restart is needed. Broader upstream make-check remains unverified, and harness runtime coverage does not mean each battle was manually defeated turn-by-turn. Apply later final-PRL changes (including PR #53) in a separate pass after playtest.

## Balloon boarding polish — 2026-10-13

Branch: `prl/balloon-boarding-polish-20261013`, based on published release `3809ee2d`. Both Route 23 escapes now reveal the balloon before moving/removing the trio, render it in front for boarding, and use slow converging boarding movement. South heist spawn raised from (8,151) to (8,150); final escape uses its actual northern trio location (8,30). Existing logic, flags, trainers and sprite assets are unchanged. 194 source tests passed and release rebuild exited 0. Frame-by-frame mGBA and progression checks passed. Pending unrelated source assets remain preserved on their separate recovery branch; generated PNG/palette build rewrites are excluded from this change. Completed: all three boarding movements explicitly awaited; both escapes retained balloon/trio overlap for 110 frames, and every actor reached the basket and finished its movement before removal. Takeoff pixel deltas are monotonic, at most one pixel per frame, with no respawn. All 12 original focused progression checks passed again; contact sheets visually reviewed. ROM: `PRL_Balloon_Boarding_Polish_20261013.gba`, 32 MiB, SHA-256 `f272096530f21664be465a14c57d37a2bfdf5ef9d29cc4f57bec2231177a7f3d`. Evidence: `/workspace/scratch/95236103e27c/balloon-final/results.json` and `/workspace/scratch/95236103e27c/polish-progression-final/results.json`. Source checkpoint initially published as `348e1f6f`; final follow-up records this completed verification. Next: user playtest. Full upstream make-check remains unverified as documented above.

## Final balloon flow — 2026-10-13

Branch: `prl/final-balloon-flow-20261013`, based on PR #55 head `22fa79c933b960c28997688f2e02d6b956ba0994`. Replaced balloon pop-in and five-step removal with existing fly_down/fly_up actions, specialized only for the Meowth balloon to 2px/frame and complete screen-edge clipping. Spawn is initially hidden; entry reveals from above the top edge. All three actors use jump movements, render in front during boarding, then are removed only after finishing. Dialogue moved before animation, with explicit message closure; added waits/delays removed from the flight flow. Quest logic, save data, battles and graphics unchanged. Release build passed, 195 Python tests passed. Final results: both escapes passed frame-by-frame mGBA checks and visual contact-sheet review, including smooth top entry, visible jumps by all three actors, completed boarding before removal, and complete top-edge clearance. Takeoff starts one frame after the trio is collected. South heist rests at (8,151) to keep the full graphic in the actual (8,152) trigger camera; final scene remains (8,30). Both reuse unchanged balloon graphics. All 12 focused progression checks passed, and all 195 Python source tests passed. No full upstream make-check claim.

Delivered ROM: `PRL_Final_Balloon_Escape_20261013.gba`, 33,554,432 bytes; SHA-256 `6e42fd1875f0ba6b54eb6e95a50df74787aef52703269025d4a4fda88250dacd`. Persistent file ID: `libfile_ba12903f51348191a7f2cb2d3f18eea1`. Evidence: `/workspace/scratch/95236103e27c/final-flow-qa2/` (both 60fps videos, contact sheets and traces) and `/workspace/scratch/95236103e27c/final-flow-progression/results.json`. Initial source checkpoint commit: `6cae0f19fb994c3418f185f34e42fce66e686fae`; final follow-up preserves completed verification. Next: user playtest; do not repeat implementation.

## Integration progress snapshot — 13 October 2026

New party tables for all15 specified Gym/E4/Champion encounters implemented; original unrelated headers/parties preserved. Eight independent rewards and three healing trainer scripts implemented. PR52/53 minimal economy/evolution/Safari patches reconciled. Source-focused tests passed, compiled/runtime verification still pending. Boss front/palette integration and two SS2 Weezing forms are in progress. PKR expanded SpeciesInfo table found by donor worker; exact original six Giovanni and shiny boss assets recoverable. Do not modify approved Route23/balloon code. Current baseline build only; no final ROM claimed. Next: import selected assets, compile, run real mGBA feature/battle/save/balloon checks, and deliver.

## Final Kanto integration runtime checkpoint — 2026-10-13

Branch `prl/final-kanto-integration-20261013`; source snapshot published as `cb3d927cd7df6278b145740a3af09fbfc9e23dfa`, parent approved PR56 `d62ba9204c6c4180ff2961fb18fe2900cc2b9f44`. No newer user-approved source was identified.

Implemented all eight exact recovery parties/aces, independent one-time item +10000 cash rewards with Bag-full deferred claims, and three existing Victory Road healers. Reconciled only approved PR52/53 prices, Bell evolution, Fuchsia stock and Safari West extra-item removal, retaining reserved0x8FC. Implemented exact first E4 singles/three Champion doubles, all trainer-only boss cosmetics (six Giovanni), separate original skeleton battle, and both SS2 Weezings: WEEZING_2 blue/white in Silph, WEEZING fiery in final Route23 battle. Underlying species unchanged.

Build32MiB succeeds. Current candidate `pokefirered.gba` SHA256 `94f97b4c85d98a868474754a2ab36789ecdd10f936cfb52d4f82d6c8fef4ac41`.206 Python unittests pass. Current mGBA evidence: all15 parties/90mons exact;23 cosmetic selections plus normal-player/ace exclusions and original skeletons pass; both approved balloon escapes pass framewise entry/jumps/boarding/top-clear and retain1frame takeoff gap. Rewards, actual purchases/evolution, save/reload healing pass on first build and final-build reruns underway. Three real Champion wins/HOF passed first build; final-build rerun underway. Actual physical Strength/puzzle entrance-to-exit Victory Road route in progress. Full upstream make-check started; not yet claimed passed.

Do not change balloons: Route23 script and event_object_movement.c match approved source. Preserve unrelated generated/pending assets on `prl/recovered-pending-assets-20261013` (`c544cbec31be7e55ded1a6843e0b9c8545a68d0e`); exclude from integration commits. Mercury materialization at make time is pre-existing approved behavior.

Exact continuation: finish already-running QA against current candidate; examine `qa-final-*-release/`, cosmetic and approved-balloon evidence, then record final outcomes. Do not restart investigation or reconvert sprites. Raw imported fronts are pinned4096bytes (two64x64frames) and palettes32bytes; explicit graphics_file_rules.mk rule protects them from single-frame PNG preview conversion. Deliver candidate after final checks, save artifact, publish final checkpoint/PR.

## Final candidate preserved — 2026-10-13

PR #60: https://github.com/jamesthoward2112-tech/Pokemon-Relic-Legacy/pull/60, branch `prl/final-kanto-integration-20261013`. Gameplay and pinned sprite source commit: `797cc610544baf290dbbd606972e1833f4e2af07`. Final ROM `PRL_Final_Kanto_20261013.gba` is saved as persistent file `libfile_96e8288a44088191aa8b83294ace2b70`, 33,554,432 bytes; SHA-256 `94f97b4c85d98a868474754a2ab36789ecdd10f936cfb52d4f82d6c8fef4ac41`. ELF SHA-256 `89349962212a146c604a683a357d03b2d7a80e4ba4f65baaddeebb9b32985825`.

Final candidate reruns passed the 15 parties / 90 mons, rewards including full Bag and actual save/reload, all three healers, actual purchases/evolution, trainer artwork and original skeleton checks, and both approved balloons. All three Champion branches won through normal battle input and reached the FRLG Hall of Fame, game-clear flag and six awarded ribbons. Exact runtime boundaries are in `docs/PRL_FINAL_KANTO_VALIDATION.md`; machine evidence is copied into `docs/qa/20261013-final-kanto/`. No critical or important issue found in independent source review. 206 Python unit tests and 56 standalone source checks passed.

Broad upstream battle suite initially failed to compile because EXPECT_EQ applied typeof to a fearTimer bitfield. Test-only explicit u32 casts correct this without changing the ROM. Full suite now compiles and runs; final outcome remains pending. Physical Victory Road proof confirms first-floor entrance, Rolando proximity, actual first-floor switch/Strength and the first second-floor switch. Whole-cave continuous route remains unverified; bounded late-floor proximity checks are underway. Do not claim either pending check passed.

Exact next action if interrupted: retain the saved ROM and existing implementation. Read ongoing `upstream-check-final2.log`, agent route results and `qa-final-route/`; record their final status and commit test-only correction/evidence. No new source conversion or gameplay work is required unless a reproducible regression is found. Do not alter approved balloons.

## Chansey training coach source branch — 2026-10-13 (not yet in playable ROM)

Branch: `prl/chansey-center-training-20261013`, based on PR #60 source (`e5dae865551be875f502b00b809616fdbb61213f`). Every one of the 19 accessible Kanto and Sevii Pokemon Center upstairs right-hand Move Deleter receptionist scripts now points to a unique, location-scaled, repeatable six-Chansey trainer. The original Move Relearner on the left and daypart switch in the center are unchanged; standalone move deletion elsewhere is not removed.

- All 114 Chanseys know only **Memento** and **Heal Pulse**. Teams ascend by local progress from Viridian Lv7–12 and Cerulean Lv16–21, through Indigo Lv47–52 and One Island Lv50–55, to **Three Island Lv65–70** (highest of all centers). Other Sevii sites: Two Island Lv55–60, Four Island Lv59–64, Five Island Lv60–65, Six Island Lv61–66, Seven Island Lv62–67.
- New trainer IDs 640–658, within the existing save-allocated maximum 768. A fresh trainer flag is cleared before each rematch and in the completion callback. No save-block layout changes.
- No trainer prize-money payouts (isolated ID-range exemption in `GetTrainerMoneyToGive`). Shared scripts included in `data/event_scripts.s`, location NPC references verified across all 19 center map JSON files. Focused source structural tests added at `tests/test_prl_chansey_center_training.py`.
- Inspect `data/battle_scripts_1.s` Memento path, `Cmd_getexp`, and `B_MEMENTO_FAIL=GEN_LATEST`; battle EXP should work on self-faint in normal trainer fights. **Still must confirm by a real mGBA battle and measurable EXP change.**
- **No updated GBA ROM has been compiled or delivered from this branch.** Desktop Commander monthly quota prevented using the NUC; this chat's Python/container runtime also unavailable for compile. Do not claim player-ready until source tests, release compile and runtime validation all pass. Keep `PRL_Final_Kanto_Sprite_Polish_20261013.gba` backed up; the direct ROM sprite polish was not committed into GitHub source, so it must be preserved/reconciled when building this new feature. Do not regress Brock's Rhydon and Khang art or the approved balloons.

Next action: build on the committed branch, run `python3 -m unittest tests.test_prl_chansey_center_training` (or discovery), compile release, and verify repeat challenge, six 2-move Chanseys, EXP gain from Memento, level ranges, zero money reward, normal healing, and previous-save loading in mGBA. Then merge only after user playtest and deliver a new `.gba`.


## Eeveeon icon and EevoBoost follow-up — 13 October 2026 (source complete; ROM not yet built)

Starting at Chansey training branch `bf99eb314857f101f9f7e5ceee80800d3326ea08`, the interrupted Eeveeon feature work was preserved in `prl/eeveeon-icon-eevoboost-20261013` at `31c95ebc5882af720dbe11c71c64e4c0521fecec`. The source fixes set the custom move's base PP to **5**, stop the normal-move fallback for EevoBoost in `STRINGID_USEDMOVE`, register a dedicated 16-entry Eeveeon icon palette at index 7, and select that palette for Eeveeon only.

Follow-up branch: `prl/eeveeon-icon-eevoboost-complete-20261013`. Fixed two additional generic battle-message paths, `B_TXT_CURRENT_MOVE` and `B_TXT_LAST_MOVE`, to display EevoBoost by name; each checks the matching current/original move ID in the Z/Max exclusion guards. The save-stable `MOVE_EEVOBOOST = MOVES_COUNT_DYNAMAX` ID and existing game mechanics remain unchanged. Added `tests/test_prl_eeveeon_icon_eevoboost.py` covering all three message paths, existing move allocation, PP 5, and the isolated palette.

**Unverified:** Compiled FireRed ROM, in-battle animation/name and party icon appearance, plus existing-save behavior. The local container has no GBA compiler or network access and Remote Desktop Commander reports zero tool calls left this month. A GitHub PR triggers the existing Python/FireRed CI, but Actions artifact quota may prevent ROM publishing. Do not claim playable completion without successful build and runtime checks. Keep approved balloon/League source, Chansey coaches, and separately ROM-patched Khang and Brock sprites intact. Transfer the latter binary art fixes into source before claiming they are present in a new all-in-one ROM.
