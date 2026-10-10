# PRL Bleeding and Fear move expansion — 10 October 2026
Implemented directly on the Kanto development line (draft PR #25) without altering save structures or level-up learnsets.

## Bleeding
3%: SUPER_FANG, FURY_SWIPES, AIR_CUTTER, AIR_SLASH, RAZOR_WIND, BITTER_BLADE, FURY_CUTTER, PECK, LEECH_LIFE, FURY_ATTACK, PIN_MISSILE, SPIKE_CANNON
5%: BITE, FIRE_FANG, ICE_FANG, THUNDER_FANG, JAW_LOCK, FISHIOUS_REND, CUT, METAL_CLAW, RAZOR_SHELL, AERIAL_ACE, RAZOR_LEAF, STONE_AXE, CEASELESS_EDGE, HORN_ATTACK, VISE_GRIP
10%: CRUNCH, HYPER_FANG, PSYCHIC_FANGS, IRON_FANGS, LOVELY_BITE, SHADOW_FANGS, SHADOW_CLAW, DRAGON_CLAW, X_SCISSOR, LEAF_BLADE, SACRED_SWORD, SOLAR_BLADE, KOWTOW_CLEAVE, SLASH, NIGHT_SLASH, PSYCHO_CUT, AQUA_CUTTER, MIGHTY_CLEAVE, BEHEMOTH_BLADE, DRILL_PECK, DRILL_RUN, CRABHAMMER, SMART_STRIKE
15%: MEGAHORN, STONE_EDGE

Excluded: Poison Fang, Poison Tail, Poison Jab, Cross Poison, Barb Barrage and all other poison-related moves; Scratch remains effect-free.
Rip and Tear keeps its existing 50% donor chance. Redux donor-exclusive special moves not yet imported retain their approved donor values and must be applied when their moves are imported.

## Fear
3%: ASTONISH
5%: SNARL, DARK_PULSE, SHADOW_BALL, HEX, OMINOUS_WIND, THROAT_CHOP
Nightmare: 10% on a successful Nightmare, delivered through its special battle script.
Noble Roar: 10% after successful stat reductions via the existing stat-change command, without doubling the stat drop.
Scary Face and Worry Seed keep their existing guaranteed Fear effects when successful. Terror Charge remains 50% Fear when imported from Elite Redux.

## Preserve
Existing accuracy, damage, power, contact flags, stat drops, flinch, curse, infatuation and other status effects are preserved. Non-volatile Bleed still obeys Rock/Ghost immunity and existing status checks. Fear remains a two-turn volatile with +50% damage and normal switching exceptions.

Changes are source commits with focused battle tests; CI and emulator validation are distinct from committing the source.
