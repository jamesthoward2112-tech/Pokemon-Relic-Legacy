# PRL 8 Oct 2026 — Skulberus complete donor / Phantowl reserved

## Locked decision
Fully retire playable PRL HYENADON. SKULBERUS replaces it in the same species/progression slot. Keep the historical PHR Hyenadon files archival only. **Not sprite-only**: original Elite Redux statistics, typing, artwork, full level-up/tutor moves, custom moves and original 3 simultaneous innate abilities are required.

## PRL evolution override
Mightyena levels up while holding **Hard Stone**, consumed on evolution, identical to the old PRL Hyenadon method. Do NOT use Elite Redux's Lv50 condition. Make Hard Stones much easier to obtain early via the Hoenn region's Rock Shop or another reliably accessible early supplier.

## Original Elite Redux donor
Source: https://github.com/Elite-Redux/er-config/blob/upcoming/SpeciesList.textproto
Graphics: https://github.com/Elite-Redux/eliteredux-source/tree/upcoming/graphics/pokemon/skulberus
- Name: Skulberus; Type Dark; base stats 100/135/90/60/80/115, BST580.
- Three selectable abilities: Moxie / Dark Gale Wings (Stygian Rush) / Strong Jaw. **Stygian Rush:** Dark-type moves gain +1 priority when the user is at full HP.
- Nocturnal innate: user's Dark-type attacks deal x1.25 damage; damage taken from Dark and Fairy attacks is reduced by 25% (multiplicative). Guard Dog and Shadow Tag keep their original Elite Redux semantics.
- Three simultaneous innate abilities: Nocturnal / Guard Dog / Shadow Tag. These are not substitutes for selectable ability slots.
- Learnset: L1 Howl, Hyper Fang, Odor Sleuth, Roar; L17 Snarl, Snatch, Swagger; L24 After You, Crunch, Double-Edge, Follow Me, Instruct, Sucker Punch; L37 Fling, Super Fang, Throat Chop; L46 Lovely Bite, Psychic Fangs, Shadow Fangs; L51 Assurance, Jaw Lock, Scary Face; L56 Beat Up, Double Slap, Rip and Tear.
- Tutors: Agility, Dark Pulse, Feint, Foul Play, Frustration, Giga Impact, Hyper Beam, Knock Off, Nasty Plot, Play Rough, Pursuit, Shadow Ball, Taunt, Thief, Trick, Metronome, Bite.
- Preserve custom moves with FULL ORIGINAL EFFECTS (strong-jaw-biting tags and engine mechanics included):
  - **Lovely Bite**: Fairy, physical, power 85, 100% accuracy, 15 PP, 10% infatuation, contact, Strong Jaw boost.
  - **Shadow Fangs**: Ghost, physical, power 80, 100% accuracy, 15 PP, 10% curse, contact, Strong Jaw boost.
  - **Rip and Tear**: Dark, physical, power 110, 90% accuracy, 5 PP, lowers opponent Speed, 50% chance of Bleed, cannot be used consecutively, contact, Strong Jaw boost; uses Crunch animation in source.
- Retain original category Alpha Bite, original art front/back/icon plus normal/shiny palettes. Keep exact source and original move effects. Bleed requires a full additional battle effect implementation if absent.

## Phantowl
User-approved exact original source sprite donor, staged at graphics/pokemon/phantowl; the Elite Redux evolution is Noctowl Lv50. PRL's existing Noctowl evolution Grimfowl must remain until the user selects whether Phantowl is a playable replacement or trainer-only visual. Do not automatically add a duplicate species.

## Integration to-do — not yet implemented
1. Replace all PRL Hyenadon species declarations/learnsets/tables, references, icon/graphics pointers and canonical registry/validator names with Skulberus, preserving species slot and existing Hard Stone evolution.
2. Reconcile engine support for selectable Dark Gale Wings; simultaneous innate abilities Nocturnal, Guard Dog and Shadow Tag; donor custom moves Lovely Bite, Shadow Fangs and Rip and Tear plus their actual effects. Do not silently drop or approximate. Other existing moves must be mapped without changing identities.
3. Ensure convenient early-game Hard Stone availability and consumption.
4. Build CI and test evolution, battle abilities, move learn/usage, sprite dimensions, icon, shiny and save compatibility. Only then call it finished.

Status: exact original sprites STAGED. ROM CODE / MECHANICS PORT AND BUILD NOT YET DONE; leave existing gameplay source untouched until the full compatibility port.
