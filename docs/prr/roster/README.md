# PRR 384-Slot Roster Seed

`PRR_384_ROSTER_SEED.csv` is a count-preserving implementation gate, not a claim that all 384 species are approved or imported.

## Locked identities

The first 214 rows are concrete:

- 151 original Kanto Pokémon, parsed directly from the selected engine's species constants.
- 37 official later members of Kanto evolutionary families.
- 26 established PRU Alpha 5 Kanto custom Pokémon, including all ten approved Eeveelutions.

The Alpha 5 custom source constants are retained even where the player-facing Relic name differs. The Mewduo row deliberately records the current `SPECIES_MEWSION` source-name conflict for resolution before import.

## Reserved capacity

Rows 215–384 preserve the agreed category totals:

| Category | Reserved rows |
| --- | ---: |
| Mega/Redux-derived permanent Relic candidates | 70 |
| Regional forms | 38 |
| Redux alternate forms | 41 |
| Other custom evolutionary species/forms | 14 |
| Other special forms | 7 |

A reserved row is not approved art, a final species name, or a licence decision. Donor names and asset IDs must be added only after the NextDex 2.65.5 duplicate audit confirms that an existing PRU species does not already occupy the concept.

The named Nidoking, Nidoqueen, Butterfree, Beedrill, and Machamp rows are temporary candidate identifiers from the development brief. They do not grant final names or automatically approve their donor assets.

## Rebuild and verify

Run:

```sh
python3 tools/prr/build_roster_seed.py
python3 -m unittest tests.prr.test_roster_seed -v
```

The automated gate requires exactly 384 contiguous slots, the agreed category counts, 214 complete approved identities, 170 explicit reserved rows, and no obsolete Pokédex target.
