# PRL — Kanto Relic narrative restoration (10 October 2026)

## User-approved narrative canon
- Restore the substantial PKR Kanto archaeological / fossil / Team Rocket dialogue progression, adapting scripts to PRL's currently approved gameplay.
- **Absolutely no player-facing dialogue in PRL should identify Relicanth as a Rocket target or ultimate Relic.** Retire the old PKR Relicanth-target story entirely; ordinary species data is a separate concern.
- Kanto legends describe an **unknown golden bird** as a possible *ultimate Relic*. It is hearsay, incomplete records, silhouettes and speculation — Kanto characters do **not** know its name and must never call it Ho-Oh.
- The story unfolds: Oak's archaeological reports → Pewter fossils → Rocket's Mt. Moon collection → Bill's strange readings → Celadon excavation ledgers → Silph's fossil revival + Master Ball theft motive → Cinnabar RELIC-01 / dormant traits → Giovanni fossil-team finale → Victory Road / League aftermath, with mystery unresolved.
- Team Rocket's final Kanto leader confrontation resolves the current criminal organisation; Victory Road Jessie & James must not speak as though Giovanni is still actively assigning orders.
- Hoenn and Johto Relic exposition is **out of scope** for this Kanto-only story pass; no spoilers of the bird's true identity.

## Implementation safety
- Keep game-specific mechanics, sprites, rivals, regional Pokémon data, party arrays and world transitions unchanged.
- Preserve existing event labels, flag conditions, Gym reward scripts, NPC/object IDs, trainer commands, duos' four mandatory encounters, and save compatibility.
- Prefer replacing text attached to existing reachable script labels; track text-only vs new-event changes distinctly.
- Use readable two-line text pages and proper message terminators. No Relicanth anywhere in newly authored dialogue.
- Source donor: PKR master (not its outdated binary pointer offsets). The post-Giovanni Sevii expansion is retired.
- Do not merge the draft automatically. Validate CI, then playtest Oak, Mt. Moon, Celadon, Silph, Cinnabar, Giovanni, Victory Road and Hall of Fame.

## Progress
First stage is a source dialogue pass based on PR #24. Reachability and old-save behaviour need emulator verification. More Kanto story breadth can be layered without touching the later-region engine plan.
