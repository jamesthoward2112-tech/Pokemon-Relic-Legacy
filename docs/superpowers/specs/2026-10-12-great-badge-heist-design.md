# Great Badge Heist Design

## Goal

Add a save-compatible post-Earth-Badge Kanto quest in which Jessie, James,
and Meowth steal the displayed badge set, the player wins eight dedicated
late-game recovery battles, and the trio makes one final Route 23 stand before
Victory Road. Preserve the real badge flags and every previously approved PRL
system.

## Persistent state

Reserve named flags in the unused `0x4D5+` range for quest start, eight
individual recovered badges, all-badges-recovered, final-duo defeat, and scene
visibility. The original `FLAG_BADGE01_GET` through `FLAG_BADGE08_GET` are
read-only prerequisites; they are never cleared. Existing saves therefore keep
HM permissions, story progression, rewards, and rematch state.

## Route 23 flow

An outdoor Route 23 coordinate trigger runs once when all eight original badge
flags are present. Established Jessie, James, and Meowth overworld sprites
approach, explain the theft, and leave in the approved 64x64 Meowth balloon.
The balloon is integrated as its own inanimate object-event graphic and moves
upward during both departures.

During recovery, each badge checkpoint checks its separate recovery flag before
accepting the original badge. A destination guard also redirects any attempted
warp or saved-game load into a Victory Road map back to Route 23 until the final
duo is defeated. This covers both Route 23 entrances and old saves positioned
beyond a checkpoint.

After all eight recovery flags are set, the final Jessie and James battle moves
from Victory Road 3F to Route 23. It reuses the approved final trainer ID,
exclusive transition, strong team, and trainer-only unusual artwork. The trio
then performs a second balloon escape and permanently unlocks Victory Road.

## Gym recovery battles

Each original Kanto leader script checks the heist before ordinary post-clear
logic. An unrecovered leader starts a dedicated late-game single battle with
new dialogue and sets only that leader's recovery flag after victory. No TM,
money bonus, held item, first-clear callback, or original story sequence runs.
The first seven can be completed in any order.

Viridian Gym reveals Giovanni only after the other seven recovery flags are
set. His dedicated six-Pokémon team retains Kabutops and Aerodactyl; the
trainer-scoped skeletal graphics hook is extended to the new recovery trainer
ID. Defeat sets the Earth recovery flag and the all-recovered flag without
replaying Team Rocket's disbandment.

## Sprite corrections

Set Edensaur's back-sprite Y offset to a visibly lower tested value and
Charaxis's back-sprite Y offset to exactly one pixel below the baseline. Front
sprites, dimensions, palettes, and artwork files remain unchanged.

## Verification

Add source regressions for persistent flags, untouched real badges, all eight
leader routes and parties, reward isolation, Giovanni gating and skeletal art,
both balloon scenes, final-duo placement and transition, Victory Road fallback,
and exact sprite offsets. Run the full Python suite, compile FireRed, then use
mGBA-based smoke checks with a new and an existing save. Capture Edensaur,
Charaxis, and Giovanni battle screenshots if the runtime supports automated
mGBA capture; otherwise report that limitation explicitly rather than claiming
visual proof.
