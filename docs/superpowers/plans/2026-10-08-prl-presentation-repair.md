# PRL presentation integration repair

**Goal:** Restore the approved PRL title, ArchOak and Omanyte on Step 12.
**Architecture:** Reuse the existing FireRed Mode 4 title implementation and
Oak presentation files from PRL-FOUNDATION-1171. Publish only the listed files
on parent 9f629939fc18570e6981371bc4b60daa3a4f1957. No mechanics changes.
**Tech stack:** GBA C, existing graphics pipeline, Python unittest, GitHub Actions.
**Spec:** Player QA repair request, 8 October 2026.

## Constraints and provenance

- Preserve all Step 10-12 mechanics, evolutions, nine hooks, species and maps.
- Title binaries match `prl_title_mode4_assets_transfer.txt` exactly.
- ArchOak reuses approved `archoak_gba.png` (Library libfile_f882551d89948191a3b357665563a494), saved unchanged as `archoak_approved.png`.
  The old staged intro PNG was still standard Oak and is rejected. Preserve every
  approved 64x64 pixel at native RGB555 precision; pad 16 rows above/below and
  index the 113 opaque GBA colours at BG palette positions 97-209. Load the
  128-entry palette at 96, below the text palette at 240. No redraw or resizing.
- Existing approved dialogue is identical in donor and Step 12; leave it intact.
- User authorizes implementation without another design/approval cycle.

## Repair and verification

- [x] Add `tests/test_prl_step12_presentation.py`; prove it rejects stock title,
  stock Oak and Nidoran intro in the unmodified checkpoint.
- [x] Restore `src/title_screen_frlg.c`, the Omanyte presentation-only change in
  `src/oak_speech.c`, `graphics/oak_speech/oak/pic.png` and its palette, and the four assets in
  `graphics/title_screen_prl/` from the prior implementation; use the actual approved ArchOak fixture.
- [x] Run focused checks; independently revert each omission and prove failure.
- [ ] Review the diff against the exact parent; use the existing Step 12 test
  discovery so the new checks are mandatory on each hosted build.
- [ ] Publish one grouped commit. Run canonical validation, the unchanged
  Step 12 suite and one clean build via the existing hosted workflow.
- [ ] Verify linked presentation assets in the resulting ROM, hash and deliver.

## Review focus

Title entrypoint must actually choose PRL; preserve the blank ornate prompt
area and blink overlay; use ArchOak's own palette; keep Omanyte's picture and
cry consistent; retain the existing main-menu transition and approved dialogue.
Player gameplay QA remains deferred until the player confirms it.
