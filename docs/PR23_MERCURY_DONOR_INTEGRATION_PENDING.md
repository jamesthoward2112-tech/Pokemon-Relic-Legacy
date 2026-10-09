# PRL PR23 — Pokémon Mercury graphics locks (pending source files)

The following three approved visual changes remain part of PR23's *five* agreed updates.
The extracted donor ZIP archives were generated in a **different conversation** and
are not mounted in the current working container, the project's Library, or its linked
Drive transfer folder. Do not claim these graphics are integrated, or replace them
with unrelated Mega graphics. Preserve all previous gameplay mechanics.

## Source archives to attach or make accessible
- `Mercury_Gigantamax_Blastoise_Donor.zip`: normal front/back, shiny front/back and palettes; **Fortotoise** target, separate playable species. Keep Water/Steel, Citadel Shell, Bastion Cannon, stats and Ancient Stone evolution unchanged.
- `Mercury_Gigantamax_Charizard_Donor.zip`: normal front/back, shiny front/back and palettes; **Charaxis** target, separate playable species. Keep Fire/Flying, Thermal Wings, Scorchwing, stats and Ancient Stone evolution unchanged.
- `Mercury_Gigantamax_Eevee_Donor.zip`: **Steeve and Neevee/Eevee Twins signature Eevee only**; preserve the ordinary species ID `SPECIES_EEVEE`, its normal graphics for all other encounters and trainers, and its original stats/moveset. Set appearance only during approved Twins battles. Do not globally replace Eevee or create a new species.

## Absolutely no Gigantamax mechanics
- No Dynamax, Gigantamax, battle size changes, Max Moves, temporary stat boosts, added evolution/form in Pokédex, or altered species ID.
- **Edensaur** keeps its current artwork, untouched.
- Import source-verified 64×64 indexed 4bpp sprite graphics; inspect preview, palettes, build wiring and normal/shiny sprites for both playable evolutions.
- Eevee Twins' battle artwork must remain isolated by trainer-specific drawing logic and restore original graphics after every battle.
- Keep non-sprite PR23 fixes: Jirachi/Celebi title animation and fixed Route 3/4 aides.

## Validation before final five-change PR23 ROM
1. Reconcile archive names and contents against donor manifest and original Mercury source.
2. Compile FireRed, validate front/back/shiny pairs and palette purity, verify existing evolutions.
3. Verify ordinary wild Eevee keeps original artwork, only Twins' special Eevee looks Gigantamax-inspired.
4. Use user's 128KB PR18/PR22-compatible save to check Mt. Moon east Route 4 exit, stationary aides, title animation, and continued save access.

**Current build status:** Only title and Aide corrections coded pending donor pack access.
