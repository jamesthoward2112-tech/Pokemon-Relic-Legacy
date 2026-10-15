# PR95 — S.S. Legacy ship and original Emerald truck animation

Continuation of PR94, not main. One-way Kanto Champion voyage: depart Vermilion, ship docks at Slateport in the narrative, Birch arranges inland transport in a lorry, then the player steps out at Littleroot. Do not open Slateport's map until that area's map graph has been audited.

Reuse FireRed's Seagallop water/ferry/wake scene, with one new private destination (11) routed to the original Emerald `InsideOfTruck` map. The ordinary ferry destination IDs 0–10 and their mechanics remain untouched. For the special route only, the Seagallop exit invokes `ExecuteTruckSequence` (camera bob, bouncing boxes, truck stop, door opening).

During boarding, `VAR_LITTLEROOT_INTRO_STATE=8` avoids the vanilla new-game truck trigger. A gender-specific dynamic warp leads from the truck door to the matching exterior parking position. At Littleroot stage 1 with sentinel 8, reuse the vanilla step-off animation and hide the truck after arrival, then change to stage 3 and direct the Champion to Birch. For older stage-1 saves without this marker, preserve the PR94 safe outdoor fallback. Do not run Mum/house/clock, Route101 Birch rescue or a second starter. Keep the Kanto party intact until the established atomic starter/PC transfer in Birch's lab.

Compile only the necessary Truck map/layout in FireRed without changing numeric map IDs. Preserve all approved title/battle art, Remote PC, badge caps, existing Kanto save and PR94 Route101 bridge.

Release gates: FireRed build, Python tests, compiled map pointer verifier and real emulator save-copy tests (boat departure, animation, moving truck, right-hand door exit for both genders, arrival dialogue, Birch starter, save/load, Sevii ferry parity). Keep draft until tested in mGBA/Pizza Boy.
