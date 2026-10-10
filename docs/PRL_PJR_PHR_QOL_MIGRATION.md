# PRL PJR/PHR QoL migration — working branch

Branch: `prl/pjr-phr-qol-migration-20261010` (derived from playable Eeveeon/Nimbeon and standing Twins build).

## Confirmed on the first implementation pass

- DexNav logic exists in PRL already but was disabled in `include/config/dexnav.h`.
- Enabled DexNav, allocated currently unused FRLG 0x8E5–0x8E7 flags and 0x40DB–0x40DC vars, and put it on the **Start menu once the Pokédex has been obtained**, without requiring a new flag on older save files. PRL's existing DexNav UI/task is reused.
- The normal Start-menu list has room for this extra entry, with maximum 8 normal actions (array length 9).
- Preserve R=Run, L=Last Ball, existing map/event logic, current Pokémon data and trainer rosters.

## Remaining work — NOT represented as implemented

1. PJR Pokémon Centre clocks at all locations with persistent time adjustment and Nimbeon day evolution, using existing PRL wallclock callbacks where possible. Issue #28.
2. **PHR** four-slot registered Key Item selector, **not PJR's selector**. The PHR repository currently exposes an asset/patch scaffold rather than this selector's complete source; locate a verified approved source/ROM before claiming a faithful port. Issue #29.
3. Full PJR QoL feature comparison and working implementations: healing, Fly/HM rules, Repel prompts, battle shortcuts, AutoRun/FastSurf, item handling, Start-menu conveniences, save behavior and remaining validated non-story QoL. Issue #30.
4. Emulator test with a progressed existing save and a new save; verify DexNav open, search, chaining, saved state. Compilation alone does not establish this.
5. Build and package test ROM with all completed features; do not replace the previously playable ROM until tested.

No PJR Key Item selector shall be introduced; the selected donor is the PHR four-slot selector.
