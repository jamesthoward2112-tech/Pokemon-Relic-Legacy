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

## 2026-10-09 gameplay trial and focused repair assessment — STOPPED

- User's Android gameplay video confirms map entrance/traversal and Camper Liam battle function, but the overworld is visibly assembled from mismatched tiles with wrong colours and missing/background regions.
- Checked original donor graphics. The donor ROM's primary and secondary 4bpp decompression produces recognisable rock artwork; it was not a case of missing source artwork.
- An offline rendering of the imported donor map/metatile indices was already badly scrambled *before* compiling the PRL ROM, so this is not simply a linker/build failure. Tile graphics/metatile composition and/or game-specific map rendering differ from the importer's assumptions. Exact root cause has NOT been proved.
- Tried an isolated palette-only visual correction, replacing bright-green background palette entries 0[0] and 1[0] with black in the preview. It removes part of the green but DOES NOT restore coherent rock formations. No point compiling that as another ROM.
- Compared with official screenshot: https://pokemonwaterblue.com/media/brock-gym-interior.webp ; reference shows a cohesive rocky gym, unlike preview/gameplay.
- No revised ROM was built or claimed fixed. Trial source and previous ROM remain available for reference on this isolated branch; do NOT merge to PRL mainline. Restoring correctly would require a more substantial tile/metatile reconstruction or source project assets, beyond the approved single quick repair attempt.
