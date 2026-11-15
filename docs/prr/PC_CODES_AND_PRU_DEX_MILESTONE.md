# PRR Quality of Life — PC passwords and PRU-style Pokédex (15 November 2026)

## Protected source and scope
This milestone uses the **PRL-derived PRR** branch `prr/from-prl-kanto-20261115`, and changes **no Pokémon roster data**. The 384-slot PRR roster seed remains separate while its 170 reserved entries are being finalised. PKR, PRL, PRU Alpha 5 and PRU2 originals are unchanged.

## Pokémon Centre PC
The existing PC retains storage, Player's PC and Hall of Fame (after winning). A new **PRR CODES** entry at index 2 accepts text through the existing Expansion naming-screen `EnterCode` special, followed by `GetCodeFeedback`.

Passwords (capitalisation matters):
- `DexAll` — set persistent flag `FLAG_PRR_DEX_ALL` (0x4AE). DexNav reveals the local encounter species icons/names without marking seen, caught, or adding Pokémon.
- `DexOff` — remove that flag.
- `TeamPreview` — set persistent flag `FLAG_PRR_TEAM_PREVIEW` (0x4AF).
- `TeamOff` — remove that flag.

Code length is limited to **11 characters**; `TeamPreview` exactly fits. No new save-block fields or invasive save migrations.

## TeamPreview interaction
When enabled, press **START at the command menu during an ordinary single-opponent trainer battle**. The action prompt changes to show **two opposing team members per page**, including unrevealed species. Press START to cycle pages; A or B closes and restores the normal action prompt.

It reads the preloaded opposing battle party directly, without editing or revealing Pokédex flags. This deliberate first iteration does **not** support link, recorded, multi or two-opponent battles, and does not emulate Radical Red's full icon-view overlay yet. It requires in-game input and redraw testing.

## PRU Pokédex layout
Enabled the existing shared `POKEDEX_PLUS_HGSS` UI in PRR, matching the **base HGSS-style layout** used in PRU. Its `EVO_SCREEN` uses `PrintEvolutionTargetSpeciesAndMethod`, covering level, stone/item, trade and other methods stored in the evolution data. Unseen evolution names and unowned evolution methods are *not hidden*. Evolutions such as Ancient Stone work when entered as `EVO_ITEM` in the actual species table.

**Not ported yet:** PRU's extra large-family Eevee pagination, proprietary 450-species ordering, custom PRU sprite/palette tweaks and PRU-specific evolution methods that aren't applicable to PRR. Audit these when the 384 roster is finished. Do not re-enable mega evolutions as a battle mechanic.

## Tests and limitations
Run:
`python3 -m unittest discover -s tests/prr -p 'test_*.py'`
`make firered -j2`
`python3 tools/prr/verify_gba.py path/to/ROM.gba`
`bash tools/prr/boot_smoke.sh path/to/ROM.gba screenshot.png`

Source tests cover PC indices, flag isolation, password toggles, view-only DexNav logic, TeamPreview controller wiring and Dex evolution method availability. A successful compile/boot test **does not prove** PC menu navigation, battle START preview input or evolution screen runtime, and these should be play-tested before release.

### Candidate next codes, not included
`EZCatch`, `SO2Toxic` care packages, `Woyaopp` training aids (adapted for PRR), plus an in-game code list/review UI. None is implemented in this milestone. Avoid unlimited rewards interfering with the intended Radical Red-level difficulty unless explicitly opted in.
