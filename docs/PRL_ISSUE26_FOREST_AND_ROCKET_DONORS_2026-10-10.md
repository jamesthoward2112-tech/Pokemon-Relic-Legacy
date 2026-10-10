# PRL Issue 26 — Restored pre-title forest and Rocket portrait donor notes

This source repair remains on stacked draft PR #25 for GitHub issue #26.

## Intro sequence
The complete original donor Scene 0 implementation is sourced from the earlier October 8 forest trial (`src/prl_intro_trial.c`), restoring the full shrine, forest, clouds, moon, comet, animated Celebi and animated Jirachi. The flow is **Porygon splash → donor forest animation → approved PRL Mode 3 title**. The small Jirachi/Celebi title-screen icon overlay was removed; the approved title image and its sparkle remain.

Donor repository: `rafaelsanna/HOENN-S-LAST-WISH-project`, pinned at commit `8f9c28e5437d9e37ed3f6873bb8dd54dac78ccf3`. The script `tools/prl/prepare_hwl_scene0.py` downloads 21 source PNG/BIN files before the **FireRed** CI compilation, verifying each Git blob SHA-1 before using it. This is reproducible only while the pinned public files are retrievable. The downloaded donor assets are *not* yet vendored in this repo; an offline FireRed build must run the same asset preparation with network access first. Do not replace with unrelated homebrew artwork.

## Rocket trainer graphics
Source bank: `PRL_Team_Rocket_COMPLETE_Donor_Bank_2026-10-09.zip`, `01_Trio_Approved_Donor_Sprites/Jessie` and `James/battle_white_uniform/front_64x64.png`. The encounter trainer data alternates between the `Prl Jessie` and `Prl James` pic IDs, so both graphic slots are now mapped to the same pixel-sharp 64x64 16-colour composite showing both white-uniform characters side by side. This fixes the original one-person portrait inconsistency without modifying trainer parties or creating new trainer IDs. This is a derived GBA-compatible composite of the approved donors, not the original unscaled full individual pictures.

Emulator checks remain mandatory. This update does not certify the larger Noxichu sprite overhaul, completed PKR-story fidelity or flawless full-game battle progression.

All CI compilation targets (FireRed, Emerald, LeafGreen and release / test) must prefetch the pinned forest assets because the engine's INCGFX dependency scanner reads the source even for a non-FireRed conditional compilation target. This does not mean the intro runs in other versions. All four Rocket pair encounters now use the actual local Jessie actor for safe double battle setup and continue after victory; team data is unchanged.
