# Noxichu approved GBA graphics — 10 Oct 2026

Integration of user-approved Noxichu front, back and party sprites into the existing PRL draft (Issue #26).

- `graphics/pokemon/noxichu/front.png`: approved indexed 64×64 normal front battle sprite.
- `graphics/pokemon/noxichu/back.png`: approved indexed 64×64 back; **pixels in rows 48–63 removed; sprite NOT moved**.
- `graphics/pokemon/noxichu/icon.png`: approved indexed 32×64 party icon (two stacked 32×32 frames).
- `graphics/pokemon/noxichu/normal.pal` and `graphics/pokemon/icon_palettes/pal6.pal`: matching normal/icon RGB palette, 14 distinct entries including transparent index 0, plus two unused 4bpp palette slots.
- Shiny palette and Noxichu abilities, typing, evolution, stats, moves, trainers, flags and save layout remain unchanged.

Verified original assets: indexed PNG 4bpp, transparent index 0, shared 14-colour active palette, 8×8-aligned dimensions and a literal lower-quarter back-sprite crop. The branch requires ROM compilation, emulator QA (including shiny appearance) and player approval before merging. This does not claim to close Issue #26's other gameplay tasks.
