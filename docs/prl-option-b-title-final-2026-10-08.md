# Final PRL Option B title-screen integration — 8 Oct 2026

- Approval: sharp Option B from the user-supplied 1536x1024 original, resized to the GBA's native 240x160 with no in-game blur or colour-palette reduction.
- Format: direct RGB555 / Mode 3. The title frame includes centered PRESS START at x=88..150, y=145..151, with a dark outline within the gold-edged box. The exact OFF strip is stored separately.
- Source asset hashes (SHA-256): full title 61d225c91f0c9139a9c02720b2df20d1e7eb37f20ee924ae7e69c0061d25b744; prompt-off strip 3df3735b0a4e97424225045028bb2b455bd176fbb0465c65263ef8199d4fcf4c.
- Title blinks on/off every 40 frames using bottom 20 scanlines only. A or START continues to the existing main menu.
- Vanilla Professor Oak and Omanyte intro remain intact; no gameplay mechanics touched.
- Presentation visual acceptance is pending actual emulator/player test after build.
