# PRL PJR registered-items integration

Base: `981cebc5aeb91ed396c89a0deb8d41a77789ec9b`, after aide gifts and Teala Move Relearner fixes.
Donor: Pokémon Johto Relics `pjr-rebuild`, PJR's tap/hold SELECT implementation.

## Implementation

- Replaced the malfunctioning PHR four-slot quick menu with PJR-style registered-item actions: register TAP or HOLD from the Bag; short SELECT press uses the TAP item; holding SELECT for 60 frames uses the HOLD item.
- Directly dispatches item field tasks, as in PJR, instead of opening a custom Start menu window.
- The original registeredItem field remains in SaveBlock1. The HOLD entry is saved via the already allocated legacy variable 0x40FD (previously PHR slot 2). This deliberately avoids modifying SaveBlock structures. 0x40FE and 0x40FF remain unchanged but are no longer presented as extra active registrations.
- Keeps PRL's bike registration replacement behaviour, DexNav Start menu, clocks, HM field rules, R-Run and L-Last-Ball, Professor Oak aide gifts and the repaired Move Relearner.
- Uses PJR's dedicated hold-registration Bag icon.

## Testing status

Static wiring tests are included in `tests/test_prl_pjr_registered_items.py`. **Compilation, mGBA runtime tests, and existing-save emulator tests have not yet been performed**. This branch is a source candidate, not a verified playable ROM. Validate tapping/holding SELECT, registering/deselecting either slot, swapping bikes, Bag return, empty slots, save/reload, and previous saves before promotion.
