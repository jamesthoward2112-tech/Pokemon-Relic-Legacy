# PRR Rocket event reliability — Mt. Moon floor IDs

## PRL donor defect verified
PRL's Mt. Moon B1F map defined Jessie/James/Meowth as local IDs 1, 2 and 3 but B2F reused the **same macro names** for local IDs 12, 13 and 14. The generated `include/constants/map_event_ids.h` redefined all three. In a compiled script, B1F object movement could consequently target the wrong objects.

## PRR fix
- B1F names now end `_MT_MOON_B1F`; B2F names now end `_MT_MOON_B2F`.
- Changed B1F JSON, B2F JSON and B1F script references consistently. No changes to battles, dialogue, map geography, Rocket teams, flags, or route triggers.
- Regenerated map-event header now defines six distinct IDs; original conflicts are absent.
- Test: `tests/prr/test_mtmoon_rocket_ids.py` verifies both map IDs and B1F battle/movement references.
- This does **not** modify Route 23's Great Badge Heist or the original balloon/boarding sequences.

## Required in-game QA
Enter Mt. Moon B1F via its normal progression and confirm all three Rocket characters appear, face and move correctly, the Double Battle triggers, departure animation completes, and the game never softlocks. Repeat after save/reload. Then test the original Route 23 Badge Heist independently.

Static ID correctness and a successful ROM compile are prerequisites, not substitutes for complete gameplay QA.
