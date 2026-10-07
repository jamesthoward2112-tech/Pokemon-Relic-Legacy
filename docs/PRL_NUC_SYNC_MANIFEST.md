# PRL NUC to GitHub sync

Canonical live source synced from:
- RelicDev/PRL-STEP10-INTEGRATION
- RelicDev/Pokemon-Relic-Legacy-Canonical/Canonical-Data

Intentionally not copied:
- build/ and compiler caches
- generated .gba, .elf, and .map outputs
- emulator saves/states
- donor/archive libraries not required by the source build
- screenshots/videos/debug captures
- temporary transfer/staging folders

GitHub Actions should be able to rebuild PRL from this branch without access to the NUC.
