# PRR / PRL-derived FireRed foundation — build verification (15 November 2026)

## Repository and lineage
- GitHub: jamesthoward2112-tech/Pokemon-Relic-Legacy
- PRR branch: prr/from-prl-kanto-20261115
- Exact donor: PRL Kanto branch pkr2/kanto-only-from-pr108-20261016, commit e4bf56346cce6334ebbd0a7a40c78d292727e503 (PRL PR #109).
- First PRR migration commit: 43e9b86ea42f478dc97fa1c7138703b2c64f69be.
- Independent build worktree: /home/james/kanto-relics/prr-prl-integration
- Build command: make firered -j2
- Private build directory: /home/james/kanto-relics/prr-private-builds/prl-derived

## Evidence
- The first original-donor compile failed because graphics/intro/prl_hwl/bg00.png was absent. The original PNG/BIN intro assets were recovered exactly from the verified read-only PRU Alpha 5 checkout, with a SHA-256 file manifest. No PRU files were changed.
- The source then compiled successfully. Pre-branding ROM SHA-256: e53a23172be1d1150a8eb4940c2405b9c1b90a9340ed8556f07270d9667d5882.
- PRR-specific Makefile identity: GBA title RADICAL RELC, game code BPRR (the cartridge title limit is twelve characters). This is header branding; the in-game title art has not yet been replaced.
- PRR ROM: PRR_PRL_Foundation_20261115.gba; 33,554,432 bytes; SHA-256 8a9ee67275358ab547230e8764d0b94bd33e5d412e5f58f4a00f5c2a060030a9.
- Header checksum valid; maker 01; version 0.
- Memory use reported: EWRAM 87.40%; IWRAM 87.45%; ROM 28,673,328 bytes (85.45% of 32 MB).
- 14 Python checks passed with PRR_ROM_UNDER_TEST pointing at this ROM. They validate source contracts and ROM header, not full gameplay.
- mGBA via Xvfb rendered a non-blank 480x320 screenshot: PRR_PRL_Foundation_Boot.png (25 colours); smoke command exited 0. This proves the initial boot frames render, **not** a complete new-game playthrough.

## Preserved and pending
- Existing Kanto story, Pokémon Centre upstairs, Team Rocket encounters, Route 23 Great Badge Heist and balloon animations are present in source and guarded by tests. Their full runtime sequences and badge/guard flags still need emulator playthrough.
- Ability Capsule costs ₽1,000 and is stocked in nine Kanto shop lists. The pre-existing Ability Patch is labelled Dream Patch and still uses the engine's original Hidden Ability item handler. Field-use behaviour and save/load need in-game tests.
- 384-entry roster CSV brought over from the prior PRR source: 214 populated approved identities, **170 placeholders** pending source audit. These 384 species are NOT yet fully implemented as a new regional Pokédex.
- Radical Red strategic difficulty, level caps, optional specialist minibosses, and final PRR title-screen artwork remain planned, **not implemented** in this foundation checkpoint.
- PRL/PKR originals, PRU Alpha 5, PRU2 and the previous PRR standalone source remain untouched.

Only the relevant source, tests and non-ROM inputs are committed to GitHub. Compiled commercial ROM derivatives stay in private local build storage.
