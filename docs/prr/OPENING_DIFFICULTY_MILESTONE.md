# PRR difficulty milestone 1 — Viridian Forest and Pewter (15 November 2026)

## Scope
This is a **non-roster** change to the PRL-derived PRR FireRed build. It does not add species, change Pokédex order, or choose any of the 170 reserved PRR slots. It ports the earlier PRR opening-chapter idea without overwriting the established PRL Kanto features.

## Optional Viridian Forest challenge
- Existing Bug Catcher Charlie is an **interaction-only optional miniboss**. Walking past him does not trigger battle.
- Strategically upgraded 3-member Bug team: Metapod Lv8, Paras Lv9 (Oran Berry), Butterfree Lv10 (Oran Berry).
- After victory, Charlie awards exactly one Ability Capsule; player can return after freeing Bag space.
- Prize flag `FLAG_PRR_FOREST_MINIBOSS_REWARD` uses unused FireRed slot `0x4AD`. The older PRR implementation incorrectly used `0x4A7`, which overlaps PRL's Norman Gym reward in `flags.h`.
- Charlie hints about Relic Energy and the Pewter Museum. PRL's Oak aide/Bulbasaur gift and all other forest NPCs remain unchanged.

## Brock, Round 1
- Preserve PRL's existing **Geodude Lv13 / Kabuto Lv14 / Onix Lv16**; do not replace Kabuto with Rhyhorn or lower Onix from level 16.
- Smart Trainer AI, sensible Natures and IVs, Oran Berries, coverage and opening-field hazards.
- No changes to Brock's badges, item/TM rewards, Badge Heist recovery rematches, or progression flags.

## Verification gates
1. Source tests: `python3 -m unittest discover -s tests/prr -p test_*.py`.
2. Rebuild FireRed; validate ROM header/code `BPRR`; capture a non-blank mGBA boot frame.
3. Play-test Charley's opt-in interaction, loss/retry, Bag-full reward then return, save/reload, and no second capsule.
4. Play-test Brock's AI and held-item behaviour with all 3 Pokémon and gym progression (including first Badge and PRL reward).
5. Run route/heist/balloon script regression tests. Higher-level boss rebalancing, level caps and the Route 25 Bug specialist remain future work.

Passing a source test or boot test is **not** equivalent to battle-playthrough verification.
