# Pokémon Radical Relics — PRL-derived foundation (15 November 2026)

## Decision and exact source
PRR is a **separate Kanto-only game**, developed from the full editable Pokémon Relic Legacy (PRL) source, **not** from the older compiled PKR binary. This branch is cut from `pkr2/kanto-only-from-pr108-20261016`, source commit `e4bf56346cce6334ebbd0a7a40c78d292727e503` (PRL PR #109). Original PRL, PKR, Pokémon Relics Ultimate Alpha 5 and PRU2 must not be changed. Existing independently boot-tested PRR Expansion source (local `prr/foundation`, commit `62926925`) remains a reference and recovery checkpoint, not the selected build base.

**Stage:** source-branch and requirements checkpoint. Source-build, emulator playthrough, fully implemented 384 species, and title artwork are **not** established by creating this branch. PR #109 is an unmerged PRL-derived Kanto branch.

## Preserve and verify from PRL
- Every approved Kanto map, overworld/graphics, permanent Relic, correct custom sprite and battle data, upgrade, dialogue, trainer, Gym/League reward, secret and QoL already working in the Kanto donor.
- Pokémon Centre upstairs, recurring Jessie and James/Meowth, Rocket's fossil/relic conspiracy, Oak's aide rewards, specialist tutors, fossil recovery, Champion Circuit and Kanto postgame.
- **Great Badge Heist:** the Route 23 scripts already hold `Route23_EventScript_PRLGreatBadgeHeist`, `Route23_Movement_PRLBalloonEnter` and `Route23_Movement_PRLBalloonTakeoff`, with explicit Jessie/James/Meowth boarding. The associated Indigo Plateau gate must respect badge-recovery progress. Keep real badge flags intact and never replace this with a generic disappearance or flash. Preserve the special balloon exit only for this story sequence.
- Kanto only. Do not enter Hoenn or Johto; do not activate Sevii Islands as a playable campaign. Retain required unused Emerald/Sevii engine data when deletion would destabilise compilation.

## PRR features to integrate
- **Exactly 384 Pokédex slots**, using the previously developed PRR roster seed as a guarded inventory, not a completed 384-species implementation. Current seed has 214 populated approved identities and **170 reserved candidates** that require actual donor and duplication audits. Never silently fill reserved slots or resurrect the obsolete 444-Pokédex dataset.
- **PRU Alpha 5 (450 species)** is authoritative for established Kanto-related custom Pokémon and sprites, learnsets, evolutions, types, abilities and lore; it is a donor, not the PRR roster size.
- Permanent original Relic Evolutions (not battle-time Mega transformation), accurate named Pokémon (including Charaxis's approved black G-Max-derived appearance), original Kanto families and relevant later evolutions/forms. No separate Dex entries for cosmetic costumes.
- Radical Red-style strategic difficulty: intelligent AI/switching, synergistic competitive teams, level caps, held items, EV/IV/nature balance, challenging Gym/Rocket/rival/Elite Four teams, optional minibosses including the Route 25 Bug specialist, and fair training/retry tools.
- Modern move-relearner in the Pokémon move menu; DexNav/encounter search; reusable TMs, optional HM field use, accessible evolution information, remote PC/modern item QoL as supported. Do not regress prior PRL Kanto features.
- Official PRR logo/title-screen assets integrated **only after source graphics validation**.
- Approved PRR Pokémon renaming decisions are the naming register; unapproved names remain pending. Never silently apply provisional names.

## Ability contract — the ONLY removed PRR requirement
Remove the earlier **four selectable abilities** feature entirely. Each Pokémon has **only one active ability**, from up to **two normal ability slots and an eligible Hidden Ability**.
- Ability Capsule: toggle between legal normal ability slots; sell for **₽1,000** at every Kanto Poké Mart.
- Dream Patch (PRR-facing label; verify item's engine identity and behaviour): change to/from legal Hidden Ability, without overwriting intended signature/locked Relic abilities.
- Validate capture, species/ability inheritance, battle effect, evolution, storage and save/load. No simultaneous four-innate abilities.

## Migration safety gates
1. Compile current PRL-derived FireRed target and confirm title, map and save boot in mGBA; record source commit, ROM hash and screenshot.
2. Import guarded roster manifest and run integrity tests; audit every donor species, source ID, sprite/palette, abilities, evolutions and move tables.
3. Add PRR branding, Ability Capsule/Dream Patch behaviour, and user-approved roster incrementally **with compiled ROM and regression tests per batch**.
4. Audit and play-test the entire Kanto story in order: Oak/starter, first chapter, gyms, Rocket scenes, badge heist/balloon, badge recovery, League gate, Hall of Fame and postgame.
5. Validate no Kanto-to-Hoenn/Sevii/Johto playable route. Check saves after every significant script flag transition.
6. Test level-cap tables, AI, miniboss triggers/rewards, ability choice and competition teams; do not claim Radical Red-level difficulty merely because levels were raised.

**No protected source is to be overwritten, and no ROM is release-ready until tests pass.**
