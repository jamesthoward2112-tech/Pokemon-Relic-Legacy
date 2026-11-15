# PRR official title screen and Oak introductory Kan — 15 November 2026

## Provenance, non-regression and intent
- Base: verified PRL-derived PRR source branch `prr/from-prl-kanto-20261115`, parent `8c45c81a7c2b93ef125cc32dbdb9a96117f52f37`.
- **Official PRR title source**, previously approved and converted to indexed 240 × 160 GBA 256 colours: `PRR_TitleScreen_GBA_240x160_256col.png` (SHA-256 `55ce9b1e6aacb6bdda1a222458061a59310510a8fae6b592061c58c3ec0f193b`).
- Converted **losslessly at the indexed bitmap level** into a GBA Mode 4 8bpp bitmap (38,400 bytes, SHA-256 `020f6c3abaa57ee327b1fbad61ec333001543a3bef8e5a299925956e2e2f9a1d`) and 512-byte 256-colour GBA BGR555 palette (SHA-256 `307f8453bf14d797beaacb1c3454c9bd85efabad5c0c7e78320cc9c2eb53219b`).
- Files: `graphics/title_screen/prr/official_title.8bpp`, `graphics/title_screen/prr/official_title.gbapal`, referenced from `src/title_screen.c`.
- Borrowed PRU Alpha 5's verified Mode 4 title implementation pattern. Existing `CreatePressStartBanner` is loaded as its own sprite layer and blink callback shows/hides it every 16 frames. Position X=128, Y=138. Title input remains A/START to main menu and the existing special key combinations remain available. No title text or artwork pixels were redrawn or baked.
- Professor Oak now **displays the approved PRU Alpha 5 Kan front sprite** (byte-identical 64x64 PNG `graphics/oak_speech/kan_intro.png`, SHA-256 `497cc11aab9bb6af5bf9767b68c958c01f6d8ffe033ad4be4684de575f41a72f`). `CreateNidoranFSprite` uses a dedicated tagged sprite sheet/template and palette. `INTRO_SPECIES=SPECIES_KANGASKHAN` provides a temporary compatible Poké Ball release effect and cry, until a final Kan species index is approved in PRR. **No new species index, Pokédex ordering, roster entries, level-ups, player gifts, or wild encounters were changed.**

## Verification
- `make firered -j2`: **successful**, output 33,554,432 bytes; game code `BPRR`, header valid.
- Private build: `/home/james/kanto-relics/prr-private-builds/prl-derived/PRR_OfficialTitle_OakKan_20261115.gba`; SHA-256 `78113253cde9aa5ce50c8b0775b2adfe1a3dd4790ec296a59a5e1c668f66b646`.
- 40/40 automated PRR tests passed including exact artwork/asset hash validation and no Kan species table insertion.
- mGBA boot test passed. Screenshot at 30 seconds confirms the official PRR logo and landscape, with separately overlaid Press Start text.
- mGBA frame screenshots `prr_title_start_kan-blink-0.png` through `-5.png` verify blinking: Press Start visible in certain frames and absent in others. `FLASH_COMPARISON [False, True, True, True, False]`.
- **Focused X11 emulator input QA passed:** Press START moved from official PRR title to the NEW GAME / OPTION menu; pressing mapped A (X key) entered Professor Oak's opening scene. Screenshots `prr_input_kan_focus-afterreturn.png`, `-afterx.png`, and `-afterz.png` verify the transitions.
- **Oak Kan scene QA passed:** screenshots `prr_oak_kan_visual-10.png` through `-17.png` show the exact custom Kan appearing beside Oak and remaining during the dialogue; screenshot `-20.png` confirms it disappears when the scene advances. This validates the sprite and release/return visuals for this introductory sequence.
- Full new-game playthrough, save compatibility, and sound/cry quality remain to be tested separately.

## Deliberate exclusions
- No following Pokémon.
- No new cheat codes or hidden abilities system.
- No 384-roster mutations (roster being independently developed).
- No PRU 450-species dataset or obsolete 444-slot dataset.
- PRL and PRU donor branches untouched.

The ROM is a test checkpoint, not a finished-game release.
