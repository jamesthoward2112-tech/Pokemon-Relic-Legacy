# PRR PC code and HGSS/PRU evolution screen — build verification

- Date: 15 November 2026
- PRR GitHub branch: `prr/from-prl-kanto-20261115`
- Previous checkpoint: `231b96072442633c4227fbc8782ddd41ee9e2cf9`
- Isolated local worktree: `/home/james/kanto-relics/prr-prl-integration`
- Build command: `make firered -j2` — **exit 0**
- Private ROM: `/home/james/kanto-relics/prr-private-builds/prl-derived/PRR_QoL_PC_Codes_PRU_Dex_20261115.gba`
- ROM size: **33,554,432 bytes**
- ROM header title: `RADICAL RELC`; game code `BPRR`; stored and calculated header checksum both 132 (valid)
- SHA-256: `0aacea1014f1a2850ef556e6ae69c83e3fbc0786d49c866d11337cf5872a074f`
- Build map ROM usage: 28,724,720 bytes, **85.61%** of 32 MB
- Source/header regression suite: **25/25 tests passed** against this ROM
- mGBA/Xvfb smoke test rendered a nonblank 480 × 320 screenshot at `PRR_QoL_PC_Codes_PRU_Dex_Boot.png` and exited 0
- No PRR 384-slot roster manifest, wild tables or species data was edited in this milestone.
- Original PRL branch, older standalone PRR, PKR, PRU Alpha 5, and PRU2 left unchanged.

**Outstanding emulator gameplay QA**: actually interact with Pokémon Centre PC before/after Hall of Fame; enter the four passwords and confirm toggled save persistence; view normal DexNav vs DexAll icons; press START in trainer battle to cycle enemy team pages and verify prompt restoration; open HGSS Evolution screen and confirm actual methods for multi-stage evolution families. A successful GBA compilation and first-frames smoke test are not a full playthrough.
