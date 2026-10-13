# PRL Work Resume Checkpoint

Updated: 2026-10-13 UTC

## Repository state

- Branch: `prl/great-badge-heist-20261012`
- Approved base: `1f0127d81e3b29346544c8e7bc8ed14e8dd79a84` (`origin/prl/giovanni-skeletal-fossils-20261012`)
- Current source commit: `a9bf3e29a40c2d3e07176417eca87263b2eb7e16`
- Latest pushed commit: none on the feature branch. The remote currently only exposes the approved base branch at `1f0127d8`. A push attempt on 2026-10-13 failed with `fatal: could not read Username for 'https://github.com': No such device or address`; no GitHub credential/helper is available to the command-line environment.
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
- The feature branch, checkpoint and ROM release asset still need GitHub authentication before they can be pushed/uploaded. Do not discard the local branch or ROM.
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

## Exact next action

Authenticate GitHub for command-line push, then push `prl/great-badge-heist-20261012`, open a PR to the repository's intended integration branch, and attach the checksum-matched ROM to a durable GitHub Release. No gameplay implementation restart is needed.

## Recovery session 2026-10-13

Recovered local source HEAD fd34a2b3 and detached release build 37de98f5. Connected GitHub access used to publish this checkpoint first. Source publication follows; this remote branch initially contains the skeletal-fossils base plus this checkpoint, not the full heist source yet. Eight modified Charaxis/Fortotoise asset files and untracked graphics/pokemon/prl_eevee_twins remain in the source worktree; preserve separately and do not assume they are included in the tested release. No completed implementation is being restarted.
