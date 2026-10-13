# Final Kanto validation — 13 October 2026

Playable ROM: PRL_Final_Kanto_20261013.gba,33554432bytes. SHA256:94f97b4c85d98a868474754a2ab36789ecdd10f936cfb52d4f82d6c8fef4ac41. Based on approved PR56 d62ba920; gameplay/build source published at797cc610544baf290dbbd606972e1833f4e2af07. Later commits only preserve QA/checkpoint/test corrections unless explicitly noted.

| Check | Result / boundary |
| --- | --- |
| Build |32MiB FireRed release; ROM82.73% used |
| Source checks |206 Python unittests and56 standalone checks passed |
| Exact parties |mGBA engine-created15battles/90mons: species, levels, all moves, items, abilities, shiny state, IVs; singles/doubles and actual rival starter dispatch matched |
| Trainer cosmetics |23 enemy renderer selections displayed and visually reviewed; ace-only duplicates, player exclusion and original skeletal Giovanni checks passed;44 binary/palette ROM parity checks and12 preview RGB5 checks passed |
| Rewards |All8 recovery callbacks register victory with full Bag; failed gift leaves item/cash unclaimed; later item+10000 paid once; repeat cannot duplicate. Brock pending and claimed state saved through actual game menu then cold-reloaded |
| Healers |All3 victory callbacks and already-defeated talk fully heal6mons HP/PP/status; party levels/EVs/items/moves preserved; actual save/cold reload passed |
| Indigo Mart |Actual purchases: PPMax50000→45000; MaxRevive50000→47000; each quantity0→1. Expanded stock preserved |
| Khang |No Bell rejects at friendship0/255; Bell accepts at friendship0 on next level; actual scene produces species1577, learns Return216 and retains Bell463. Return actual battle power102; approved ability/art/dex preserved |
| Safari |Compiled map retains four original item objects, no unwanted West SilkScarf; reserved0x8FC kept. Other Safari parameters unchanged |
| Champion/Hall of Fame |All3 branches won using real battle button input against unmodified enemy teams, engine outcome1. FRLG HOF map/callback, SYS_GAME_CLEAR and six Champion ribbons confirmed. AI used Protect/FakeOut/DragonDance |
| Approved balloons |Both framewise mGBA runs passed smooth entry, all3 visible jumps/finished boarding, continuous ascent and full top-edge clearance;1frame boarding→takeoff gap. Route23 scripts/event-object movement unchanged from PR56 |

Runtime fixtures use the existing128KiB save in a copy. Party/render checks stage trainer scripts; Champion win fixtures power up the player party only and use normal battle input. After battle returns to overworld the unchanged EnterRoom script is resumed from the engine-set defeated trainer flag to exercise Oak/HOF. No battle outcomes, victory flags or ribbons are injected. These are emulator tests, not a claim of a complete manual adventure or turn-by-turn defeat of every Gym/E4. The user's latest separately repaired save was not available; compatibility checks use the supplied older existing save.

Evidence directories in Work: qa-final-parties-release, qa-final-rewards-release, qa-final-economy-release, qa-final-cosmetics, qa-final-approved-balloons. Harnesses are committed under tools/prl/qa_final_*.py and qa_balloon_polish.py. Full upstream battle-suite and physical VictoryRoad route status are recorded in the final checkpoint.
