# PRL move order and Crystal Redux presentation — 2026-10-11

Base: `prl/pjr-qol-reliable-selector-20261011`, `b3cd91c09783bcda5f6eef08fa9e337fbac6bbe7` (PR #34).
ROM: 33,554,432 bytes, SHA-256 `a3efee04c35a037211be1f3cb82af87cb36566af7d06c7b0852f232371dd3c6f`.

## Implementation

Reused all 38 existing approved donor descriptors (indices 00–37), their tiles, entry tiles, tilemaps and palettes. Pinned donor SHA-256: `242e40954879f61ca95897f6880009bf65d1c30d07e8086658f7019f46024720`. The prior `prl/crystal-redux-restart-20261011` checkpoint contains extraction tooling, not a completed source renderer integration; the previously supplied asset ZIP and location assignments are reused here. Every native background load path now selects the visual descriptor, with 35 default environment mappings and 55 location overrides. Native environment mechanics remain unchanged; water, underwater, special and link priorities remain separate from outdoor location art overrides.

Ordinary Kanto trainers use the native large Poké Ball transition. Rocket/Boss classes use the donor's authentic Rocket logo (gfx offset `0x45cc18`, palette `0x410840`). The four Jessie/James encounters and four Steevee/Neevee encounters have exclusive emblem transitions using their existing PRL trainer/Pokémon sprites. No completed approved exclusive-transition implementation was present in the supplied checkpoint, so these use the existing native shimmer/hold/circular-wipe engine with existing artwork.

Move reordering was **already functional in this exact approved source base** and in the supplied candidate when the complete A/select-position/confirm sequence was used. Ordinary party and boxed modes are unlocked; in-battle and choose-half summaries are locked. These paths were retained rather than making an unsupported mode change. Added regression checks and tested the newly compiled ROM with actual Summary inputs.

The compact, checksummed asset bank is unpacked before Make scans graphics. It recreates exact GBA bytes; no newly generated substitute art is used. No save layout, SELECT registration code, Relearner code, aide scripts, time code, species data or story scripts changed.

## Validation

- `make -j8 firered`: exit 0. ROM usage 27,604,932 / 33,554,432 bytes (82.27%).
- `python3 -m unittest discover -s tests`: 111 tests, all passing. Obsolete four-item selector, replaced encounter-trigger and superseded Eevee-team assertions were updated to approved behavior; no tests disabled.
- Actual mGBA core, supplied 128 KiB `test` save, disposable copies only. Original save untouched.
- Party and actual PC storage Summary: real GBA keys exchange moves 1/2, current PP and four distinct two-bit PP Up fields. Reopen Summary, normal in-game save and cold restart preserve both party and box swaps.
- All 38 visual assignments verified in a running mGBA battle: exact decompressed VRAM bytes for both tilesets and both tilemaps, exact 96-byte palettes, and rendered screenshots. Entry overlay layers are disabled after byte verification for the main-background contact sheet, as they are after the normal entry animation.
- Native trainer battle-start routines tested for trainer IDs 1, 251, 628 and 624. Observed Poké Ball, Rocket logo, Jessie/James and Eevee Twins transitions and completed battle introductions with native teams. Test contexts are set up through native calls; the ROM is not binary patched.
- SELECT tap activates Bicycle; 61-frame hold activates a separately registered Itemfinder (item 712), with no selector menu.
- Upstairs time script selects evening and native local time reads 18:00.
- Move Relearner script opens its chooser and level-up move list without crashing. Egg/TM/Tutor paths were source-checked and covered by existing automated tests; full teaching of every category was not emulator-tested.
- Route 10 aide gives Lucky Egg and sets its one-time flag in a disposable context.

Limits: scripted/native setup accelerates contexts. This is not a full story playthrough or a visit to every assigned map; other aide gifts and every duo rematch were not individually played through. Rendering/mapping and all eight exclusive trainer routes are covered by automated assertions.

## Reproduce

Build with the repository's existing ARM/newlib prerequisites. Compile `tools/prl/mgba_probe.c` against libmGBA (`cc -shared -fPIC ... -lmgba`). Run `tools/prl/qa_restore_presentation.py` then `tools/prl/qa_preserved_features.py` with `--rom`, `--elf`, `--save`, `--probe` and `--out`; ARM `nm` must be on PATH. Outputs include disposable saves, screenshots and JSON results. Native QA calls restore the CPU pipeline and registers around each setup call.
