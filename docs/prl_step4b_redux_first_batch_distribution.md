# Step 4B — First Eight Redux Moves: Approved Relic Distribution

Source: [locked 9,537 compatibility decisions](https://app.notion.com/p/3f34939d431681058088dd7b53b895dc). The following are all **40 approved pairings for 29 custom species** in this batch; other custom species are forbidden from learning these eight moves, including Valkyvoir. The eight move battle effects passed the [93/93 focused tests](https://github.com/jamesthoward2112-tech/Pokemon-Relic-Legacy/actions/runs/37922583392).

| Region | Relic species | Approved move and exact method |
| --- | --- | --- |
| Kanto | Edensaur | Soil Drain (TM) |
| Kanto | Charaxis | Take Flight (TM) |
| Kanto | Khang | Beatdown (LEVELUP 58); Bravado (TUTOR); Seismic Slam (TUTOR) |
| Kanto | Nolax | Beatdown (LEVELUP 54); Bravado (TUTOR); Seismic Slam (LEVELUP 64) |
| Kanto | Champeon | Beatdown (TUTOR); Insect Impact (LEVELUP 48) |
| Kanto | Lepideon | Take Flight (TM); Insect Impact (LEVELUP 46) |
| Kanto | Sphynxeon | Soil Drain (TM); Seismic Slam (LEVELUP 60) |
| Kanto | Guardeon | Iron Fangs (TM) |
| Kanto | Omeon | Raging Souls (EXCLUSIVE) |
| Kanto | Osteodian | Seismic Slam (LEVELUP 60) |
| Johto | Scarabub | Insect Impact (LEVELUP, deferred for Lv18 baby progression) |
| Johto | Skarmet | Take Flight (TM) |
| Johto | Mootiny | Bravado (LEVELUP, deferred for Lv18 baby progression) |
| Johto | Heracurion | Insect Impact (LEVELUP 50); Seismic Slam (TUTOR) |
| Johto | Miltitan | Bravado (TUTOR); Seismic Slam (LEVELUP 62) |
| Johto | Donphalanx | Seismic Slam (LEVELUP 73) |
| Johto | Pinsirex | Insect Impact (LEVELUP 46) |
| Johto | Faeranium | Soil Drain (TM) |
| Johto | Pyroclast | Raging Souls (EXCLUSIVE) |
| Johto | Feralodon | Seismic Slam (TUTOR) |
| Johto | Grimfowl | Take Flight (TM) |
| Johto | Ghoulbat | Raging Souls (EXCLUSIVE); Take Flight (TM) |
| Hoenn | Solaziken | Insect Impact (TUTOR) |
| Hoenn | Swamplith | Seismic Slam (TUTOR) |
| Hoenn | Skulberus | Beatdown (TUTOR); Bravado (TUTOR) |
| Hoenn | Mawyrm | Iron Fangs (TM) |
| Hoenn | Cactomb | Soil Drain (TM) |
| Hoenn | Tropisaur | Take Flight (TM) |
| Hoenn | Coelossus | Seismic Slam (TUTOR) |

## Implemented in this commit

**11 LEVELUP pairs:** at the indicated late levels above, introduced into the normal evolution line's PRL custom level-up table without replacing any prior move. Donphalanx copies its complete Great Tusk level-up donor sequence, Osteodian copies its complete Marowak sequence, each adding only Seismic Slam, and retains the original teachable pool.

**29 pairs not yet executable:** two baby LEVELUP cases (Scarabub/Insect Impact and Mootiny/Bravado) stay deferred because they evolve at Lv18 and early high-power access would hurt balance. The other 27 are **12 TM, 12 TUTOR, 3 EXCLUSIVE** and must wait for separately programmed region/badge/quest gates and curated reusable TMs. Matrix legality is approved, but the gameplay method is **not** implemented here.

No changes to the nine classic starter abilities and signature moves; no egg moves, no instant evolution grants, no broad normal-species grants, no change to Skulberus donor powers/innates, no Mega feature. Level choices are engineering defaults awaiting pacing QA. Step 4C will implement validated TM/Tutor/exclusive distribution with these exact whitelists; do not create 187 individual TM items.
