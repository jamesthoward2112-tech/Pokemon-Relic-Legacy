# Azul Agua Brock's Gym — isolated PRL trial

- Donor: user-supplied Pokémon Water Blue / Azul Agua Beta 1.4 ML (creator: gameboy_cl).
- Source gym: FireRed group 6, map 2; map header 0xA0A47C, layout 0x98EAA8.
- Full donor 13x17 layout, 2x2 map border, donor primary/secondary 4bpp tiles,
  palettes and primary metatile attributes, with original collision/elevation map blocks.
- Preserve PRL Brock/Camper Liam/Gym Guide dialogue, battles, flags and badge logic.
- Align the Brock, Gym Guide, statues and exit to the new floor plan.
- Deliberately not a canonical map change; don't merge without manual gameplay review.
- Azul Agua-specific extra events/scripts and graphics-linked NPCs are not copied,
  since their semantics have not been mapped to PRL's script engine.
- Verify sprites, elevations, passability, door exit, dialogue and Brock battle in mGBA.
- Before redistributing artwork, confirm the original artist's reuse terms.
