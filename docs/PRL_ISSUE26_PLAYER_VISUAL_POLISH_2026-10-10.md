# PRL Issue 26: player visual feedback (10 October)

The player confirms the restored full Celebi/Jirachi pre-title scene works. No intro code was changed.

Noxichu: the accepted front and back remain 64x64, without any enlargement or redesign. Only isolated gold-highlight pixels were cleaned. A new 32x64 two-frame dark/gold icon matches the actual front rather than the pale previous icon. All mechanics/save flags unchanged.

Jessie and James: replaced both battle portrait slots with a joint 64x64 GBA 4bpp image derived from the approved white-uniform donor sprites. Teams/flags and the local NPC trainer binding remain intact. In Rocket Hideout, Silph 11F and Victory Road 3F, both trainers stage side by side with Meowth behind. Existing Mt Moon exit triggers and its side-by-side NPCs remain intact. Removed copy/paste triplicate flag commands.

Runtime approval outstanding: player's 128 KiB SAV is preserved but gameplay testing in mGBA, collision approach, intro still running, portrait render, no blue assertion and all four postbattle departures need verification. Keep PR #25 draft and issue #26 open.

## Reproduction-driven repair after user reported no visible change

The supplied 128 KiB FRLG `.sav` has latest save slot index 9: group 35/map 3 = Mt Moon B2F, position (5,11). Flag 0x232 (fossil obtained) is **true**, flag 0x8F4 (Mt Moon Jessie/James completed) is **false**, and flag 0x8F8 (Mt Moon Jessie/James hidden) is **true**. This is a live reproduction of the hidden-NPC state we previously overlooked. Mt Moon B1F now has an `ON_LOAD` visibility re-sync before map actors instantiate, while `ON_TRANSITION` still adds actual NPCs. Meowth is behind James, not visually hiding Jessie.

Noxichu's party icon has updated stronger amber eye/yellow markings, clearer graphite separation, and distinct second animation-frame blinking. Still 32x64 indexed/16-colour and maps to existing palette 6 (which matches Noxichu normal palette). No species or sprite dimension changes. Full emulator QA remains outstanding.

Noxichu's shared front/back and dedicated icon palette slot 6 have been refined together for improved shadow separation and stronger gold/amber contrast, without changing the 64x64 sprite canvases or 32x64 icon canvas. This affects appearance visibly while retaining the approved black-and-gold concept.

## 10 October follow-up: full-width Mt Moon detection and reference artwork
- Player reference explicitly requires **James LEFT, Jessie RIGHT** in one shared white-uniform trainer portrait. The new 64×64 indexed portrait is adapted from the player's supplied screenshot of the actual duo pose, rather than combining two unrelated solo trainer pictures.
- Noxichu front/back/icon artwork has been rebuilt from the archived approved PKR Noxichu reference poster. All size limits remain 64×64/64×64/32×64, and the icon palette slot 6 matches the new normal sprite palette. This is a substantial redraw, not a 1-pixel recolour.
- Mt Moon B1F places Jessie at (42,2), James at (43,2), Meowth at (44,2), along the **top wall** of the exit room. All face down. Automatically triggers the conversation from the **full bottom walking row x38–46 y5**, the approach corridor x40–45 y4, and top approach x41–44 y3. The saved fossil and victory flags, trainer parties, duo battle, and exit warp are unchanged.
- Automated source/build checks do not replace emulator QA. Verify with the submitted Mt Moon B2F .sav before closing issue #26; keep the existing Celebi/Jirachi opening unchanged.
