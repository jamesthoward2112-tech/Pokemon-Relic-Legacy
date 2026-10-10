# Skarmadon: chrome-only sprite lock (9 Oct 2026)

The playable Skarmadon has **one approved colourway**: chrome/steel-silver,
gunmetal, and deep burgundy. Its authoritative GBA graphics are under
`graphics/pokemon/skarmadon/` (front/back/icon/normal + shiny palettes).

**Do not import or restore the legacy orange/yellow Skarmory Mega artwork.**
It was a mistaken duplicate donor that repeatedly resurfaced when
selecting Skarmadon artwork.

The engine still defines `SPECIES_SKARMORY_MEGA`, so deleting its required
graphics paths without refactoring that optional form would risk a build
failure. To eliminate the unwanted colourway safely, the historical
`graphics/pokemon/skarmory/mega/` five asset paths now point to the
corresponding already-approved *chrome Skarmadon* Git blobs. No orange
front/back/icon or orange normal/shiny palette is present at those
active source paths.

This is an **art-only correction**. It does not modify game mechanics,
trainer teams, species identities, battle scripts or the existing
Kanto/Hoenn/Johto story. The source bank may still retain archival copies
for provenance, but those are rejected for PRL use.
