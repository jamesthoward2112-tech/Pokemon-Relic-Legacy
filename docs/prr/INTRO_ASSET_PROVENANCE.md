# Restored PRL title-intro assets for the PRR source branch

The PRL Kanto source references graphics/intro/prl_hwl in src/prl_intro_trial.c.
The Kanto branch did not track the referenced source PNGs and map BINs, causing the first FireRed build attempt to stop at missing bg00.png.

Restoration source (READ ONLY):
/home/james/pru-patchbuild/verified-alpha5-relearner/graphics/intro/prl_hwl

PRR destination:
graphics/intro/prl_hwl/

Only the exact referenced authored PNGs and matching .bin tilemaps were copied.
Generated .4bpp, .gbapal, and compression intermediates are deliberately excluded.
This is a **build dependency recovery**, not a change to the approved PRR title screen.
No source bytes in PRU Alpha 5 were modified.

Cross-check: bg00.png SHA-256 36545d5261eedb92c39f909b88d1de4385361eafec5003b9088f8f1d2642a2f0 matches /home/james/pru-alpha5-source-20261112/graphics/intro/prl_hwl/bg00.png.

See INTRO_ASSET_SHA256.txt for every included input file.
