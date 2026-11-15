# PRR non-roster milestone — build verification (15 November 2026)

## Result
**Built and mGBA boot-tested** from the PRL-derived Kanto PRR branch after integrating the Viridian Forest optional challenge, tactical Brock battle, and the Mt. Moon Team Rocket object-ID fix.

- Base PRR GitHub branch: `prr/from-prl-kanto-20261115` (starting HEAD `310097048e50660357ae4ac68654138103b7c94c`).
- Local build source: `/home/james/kanto-relics/prr-prl-integration`.
- Local private ROM: `/home/james/kanto-relics/prr-private-builds/prl-derived/PRR_PRL_Opening_Difficulty_Rocket_Fix_20261115.gba`.
- `make firered -j2` completed with exit code **0**.
- ROM: 33,554,432 bytes, header title `RADICAL RELC`, game code `BPRR`, valid header checksum.
- SHA-256: `6e03696a7858b8feb6fe53140f8908e0bf9034a0864234b7b4fdc07df4031820`.
- 19 automated source/header/roster regression tests **passed** against the new ROM.
- mGBA/Xvfb smoke check exited **0** with a non-blank 480×320 screenshot at `PRR_Opening_Rocket_Boot.png`.
- Generated `map_event_ids.h` now has distinct Jessie/James/Meowth IDs on Mt. Moon B1F and B2F. Previous collision/redefinition warnings disappeared.

## What this does not prove yet
- No complete playthrough of Charlie's reward edge cases, Brock's AI/hold effects, Rocket double-battle departure, Badge Heist balloon, or later save/reload sequence has been done.
- The **384-slot roster seed was not changed**, and no donor species were imported; 170 slots remain reserved for the separately ongoing roster work.
- Radical Red-wide difficulty, dynamic level caps, boss roster overhaul and Route 25 Bug specialist still require implementation.
- PRL master and PRU Alpha 5 were not changed. Existing worktree had previously dirty sprite/generated graphic files and they were deliberately **excluded** from this commit.
