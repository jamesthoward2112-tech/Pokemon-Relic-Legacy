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
