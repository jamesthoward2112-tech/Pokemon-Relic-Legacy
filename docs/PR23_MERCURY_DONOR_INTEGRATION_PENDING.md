# PR23 — Approved Mercury visual donor integration

**Status: imported to PR23 source; in-emulator visual review still required.**

The user uploaded `Mercury_GMax_Blastoise_Charizard_Eevee_Full_Donor_Pack.zip`
on 10 October 2026. The original ZIP is archived in the PRL file Library
at `/Pokémon Relic Legacy/Sprite Resources/Mercury/`.

## Reproducible sprite sources

- `assets/prl_mercury/mercury_sprite_payload.b64` contains compressed,
  verified GBA tile data and normal/shiny RGB555 palettes from Mercury 1.3.3.
- `tools/prl_mercury_assets.py` rebuilds native 64×64 `front.png`,
  `back.png`, `normal.pal` and `shiny.pal` using Python standard library.
  Its output matches the uploaded normal sprite PNGs pixel-for-pixel.
- The Makefile runs this decoder **before graphics are scanned/compiled**,
  making clean GitHub Actions builds reproducible.

### Final graphics targets

- **Fortotoise** ← Mercury Gigantamax Blastoise:
  `graphics/pokemon/fortotoise/{front.png,back.png,normal.pal,shiny.pal}`.
  Keep Water/Steel, Citadel Shell, Bastion Cannon, existing stats, and
  Ancient Stone evolution unchanged.
- **Charaxis** ← Mercury Gigantamax Charizard:
  `graphics/pokemon/charaxis/{front.png,back.png,normal.pal,shiny.pal}`.
  Keep Fire/Flying, Thermal Wings, Scorchwing, existing stats and evolution.
- **Steeve/Neevee special Eevee** ← Mercury Gigantamax Eevee:
  `graphics/pokemon/prl_eevee_twins/` provides trainer-only front
  sprite and normal/shiny palettes. The `PRLIsSteeveNeeveeSignatureEevee`
  battle gfx hook applies that visual only to opponent-side Eevee in the
  Route 3, S.S. Anne, Pokémon Tower, and Route 23 encounters.
  All other Eevee remain normal, including captured/player Eevee.

### Nonnegotiable exclusions

**No Dynamax/Gigantamax mechanics.** No extra Pokémon species, form change,
Max Moves, stats, abilities, size manipulation, or Pokédex slot. Species
remains `SPECIES_EEVEE` in twins battles. `Edensaur` remains untouched.

### Validation / pending user gameplay checks

- Build FireRed ROM and run all Python tests.
- Inspect Fortotoise and Charaxis front/back, normal/shiny, and evolution.
- Verify regular Eevee unaffected and Twins' Eevee sprite only in 4 battles.
- Check title screen animated Jirachi/Celebi, Route 3 stationary Charmander
  Aide, and Squirtle with its stationary Aide at the eastern Mt. Moon exit.
- Keep PR23 a draft pending in-emulator confirmation.

This is the **five-group PR23 limit:** Fortotoise, Charaxis,
Steeve/Neevee Eevee, title animation, combined Aide correction.
