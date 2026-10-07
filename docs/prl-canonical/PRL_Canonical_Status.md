# PRL canonical data continuation â€” 6 October 2026

Status: reconciled reference package, with explicit unresolved gates. No PRL ROM built.

## Completed
- Continued the four existing snapshots; retained regional donor balance references without importing them globally.
- Backfilled source-backed base stats and table references for 29 current PJR custom species.
- Reconciled Osteodian/Maroghost against Run292 and current source; excluded historical Terathwack and Sudowarden from PRL.
- Backfilled five PKR PokÃ©dex rows and two Marowak-branch rows in Notion.
- Produced one PRL record per identity: 50 custom-species records, including the incomplete locked records.
- Applied the existing PHR Notion overrides and separated legacy donor values.
- Staged exact Valkyvoir front/back/normal/shiny donor bytes and hashes. No conversion, scaling or redraw.
- Added machine-readable reference data, import gates, obtainability matrix and validation.
- Locked Kanto availability for Eevee and all 16 Eeveelutions; custom Eeveelution rows now carry Kanto availability while exact placement remains pending.
- Locked recurring Steeve & Neevee and Jessie & James encounter structure across all three regions: regional difficulty resets, within-region escalation, static overworld sprites allowed, and mandatory/unskippable placement.

## Genuine unresolved gates
- Bellantem/Sabellite: no complete final mechanics in connected Notion, current masters or inspected PHR GitHub data.
- Mawyrm: complete typing and exact shrine evolution; Cactomb: Spirit Thorns/Grave Spines effects/timing; Coelossus: exact entry-hazard specification.
- Toxeon: PKR/PJR mechanics conflict; only shared identity, Poison typing and current PJR visual are locked for the merged record.
- Valkyvoir: stats/wider learnset remain unapproved; neither inspected direct sprite archive supplies its party icon or overworld.
- Alphoracle: replacement ability remains unapproved; pinned donor still declares rejected Full Metal Body.
- Run292 player QA remains pending. Source reading is not visual/evolution playtesting.
- PRL has no selected production source foundation or GitHub remote in the inspected NUC workspace.

## Source pins
- PJR current donor: 78f3d308f71be9990262a5985b2e0c1e92d7e3cb; Run292 source 64c68bfa8b2db8ae31af21837cabe1953729379d.
- PHR develop: 1b54f17e3650a58c08f2eb9f3aa1de4b39014851. Its data and the inspected fix/alpha2-gemigoyle-runtime scripts retain the old roster.
- PKR: existing canonical snapshot and its master-sheet hash; no claimed new PKR runtime audit.
- Notion authority: https://app.notion.com/p/3f14939d43168198af98dcce4834d06c

## Next concrete PRL build step
Create an isolated source-engine baseline and a minimal FireRed-Kanto map/start test. Validate compile, boot and save before adding the gated species through an engine adapter. Preserve real FireRed Kanto geography. Keep this package engine-neutral until that foundation is selected; do not patch the Quetzal reference ROM to pretend these data files are a build.

## Storage / integration
Canonical snapshot: docs/prl-canonical
Branch: data/canonical-20261006. Live PJR and PRL main checkouts are unchanged by this task.
