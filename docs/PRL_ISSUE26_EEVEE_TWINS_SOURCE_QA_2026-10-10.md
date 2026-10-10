# Issue #26 — Eevee Twins and Mt Moon battle-context repair (source QA)

Work remains in existing **draft PR #25**, no separate PR or ROM variant. Build and in-game approval required.

| Encounter | Visible actors | Trigger lane | Story prerequisite | Previous victory |
|---|---|---|---|---|
| Route 3 | Steeve (59,1), Neevee (61,1) | (60,1–3) | Boulder Badge | existing trainer defeated flag syncs hide / trigger variable |
| S.S. Anne 2F | Steeve (29,3), Neevee (31,3) | x30–32, y4–5 after rival | S.S. Ticket / rival scene 1 | hide + scene 2 |
| Pokémon Tower 1F | Steeve (17,8), Neevee (19,8) | x17–19, y10 | Silph Scope | hide + trigger variable |
| Route 23 | Steeve (3,28), Neevee (4,28) | x1–6, y30 | Earth Badge / scene 8 | hide + scene 9 |
| Mt Moon B1F | approved Jessie/James actors unchanged | upper y2 escape plus x42, y3–5 / x45, y3 | fossil | no repeat |

The double-battle macro now has optional NPC `localId` and `continueScript` parameters. It deliberately defaults to the original behaviour for all unrelated trainer battles. In these five scripted scenes, the battle system selects a **real object event** before revealing/facing the trainer. Postbattle script execution continues, allowing the existing Relic dialogue and departure sequence to run. A two-usable-Pokémon check occurs before the Twins step forwards. Original trainer records, battle progression, Mercury opponent Eevee art, flag numbers and save layout are untouched.

## Real emulator acceptance still outstanding
- Fresh-save and existing-save walkthrough of each of the four Twins scenes
- All approach coordinates, no bypass via alternate paths, no blue assertion, double-battle portraits and special Eevee rendered
- Both actors exit and revisits do not replay; one-usable-Pokémon path retries cleanly
- Mt Moon both fossil choices, all upper and lower approaches, full post-battle dialogue and saved flags
- S.S. Anne old saves *after the ship has departed* cannot revisit the ship; this requires explicit compatibility decision
- Cold-boot Celebi/Jirachi donor animation, corrected Jessie/James **trainer portraits**, updated exact Noxichu donor and full PKR-style dialogue remain separate acceptance points in the **same** issue #26, not declared complete by this source patch

Static tests: `python -m unittest discover -s tests -p "test_prl_issue26_visible_duos.py"`; they check source wiring, not collision, video, save replay or actual ROM behaviour.

## Issue 26 subsequent source integration
- Original Hoenn's Last Wish shrine/forest/cloud/moon/comet **Celebi and Jirachi** scene restored from pinned Oct 8 working trial source, called after Porygon before Mode 3 title
- Vendor sprite backgrounds restored at FireRed CI build time with SHA-pinned donor script (21 original PNG/BIN files); deliberately no title-only mini icons
- Jessie/James trainer fronts use a 64x64, 16-colour, two-character composite derived from the approved white uniform Jessie and James donor pack; both existing trainer pic slots use the identical duo image so neither appears alone
- Battle team records, approved overworld actors, Mercury Eevee special battle art and all save flags are unchanged
- Not a gameplay acceptance claim: verify exact pre-title timings, individual versus composite art, encounter approach and save compatibility in mGBA before release

Additional anti-bypass gates added around Mt Moon's eastern Route 4 exit warp (45,4) and its lower/right approach tiles (44,5),(45,5),(46,5),(46,4). Coordinate and warp precedence remains an emulator acceptance point.
