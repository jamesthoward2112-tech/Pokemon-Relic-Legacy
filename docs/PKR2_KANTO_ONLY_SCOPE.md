# Pokémon Kanto Relics 2.0 (PKR 2.0)

**Product direction:** Kanto-only successor to Pokémon Kanto Relics. The latest Pokémon Relic Legacy (PRL) Kanto source and assets remain authoritative. This branch deliberately forks the most recent reviewed source at `prl/pr108-regional-trainer-namespaces-save-architecture-20261015`, commit `6ee9b984521ac6f2beaf995f771a726379e55c88`. Do **not** restart from old PKR or stale PRL `main`.

## Gameplay scope
- Preserve all Kanto main adventure, custom Relics, original and approved sprite work, gym / Elite Four / rival improvements, recurring pairs, Great Badge Heist, Rocket and twins events, QoL, Celebi/Jirachi intro, Cerulean Cave, S.S. Anne and Sevii content.
- **No playable Hoenn chapter.** Vermilion ferry after Hall of Fame must never call the PRL Kanto-to-Hoenn transfer/boxing/reset/region-loading flow.
- Kanto postgame stays accessible after Hall of Fame. Oak celebrates Champion, enables National Dex; Mum congratulates and offers rest, without Hoenn banking or starting a transfer.
- The codebase inherits Emerald engine internals and Hoenn data. Keeping unused engine/asset data in the repository is not the same as offering a playable Hoenn region. **Do not delete native Emerald dependencies just to shrink the repository.**
- **Not approved as game-ready:** Kanto ROM title graphics still say 'Relic Legacy' until PKR 2.0 title artwork is prepared and tested. No claim of a complete renamed playable ROM until graphics, in-game dialogue, packaging, build and emulator tests pass.

## Source-of-truth policy
- For every Kanto creature, event, sprite, trainer, Kanto map, script, dialogue and QoL fix, choose the **latest approved PRL branch** as primary donor. Older PKR may be consulted for design but does not overwrite improved PRL Kanto assets.
- For Pokémon Relics Ultimate, the donor priority is PJR map/story mechanics, then this latest PRL-derived Kanto content, then PHR for Hoenn species mechanics and PJR for Johto species. PKR 2.0 itself never gains a playable Hoenn map.

## Stage 1 implementation in this branch
- Vermilion ferry's Hoenn boarding offer disabled; S.S. Anne + Sevii interactions preserved.
- Oak gives post-Kanto research guidance and enables National Pokédex without a regional ticket.
- Mum offers repeatable rest and Kanto homecoming dialogue; no Hoenn departure instructions.

## Test gates
1. Compile FireRed target, verify fresh Kanto save, Oak's starter branches and sprite compatibility.
2. Eight gyms, champion, Hall of Fame, return to Pallet, Mum and Oak, repeat healing.
3. Ferry with no pass, Tri-Pass and Rainbow Pass; S.S. Anne inaccessible once departed; verify Hoenn never starts.
4. Kanto Relics, trainer rematches, Jessie & James, Steevee & Neevee, Mew truck, Mewtwo, credits / title animation.
5. Only after game testing update cartridge/title branding and produce a PKR 2.0 standalone artifact.

Original PRL branches remain untouched.
