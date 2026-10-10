# PRL — PKR-based full Kanto narrative pass (10 October 2026)

## Source and status
- Narrative reference: verified Project Kanto Relics master `MasterRomPKR.gba`, SHA256 `fb8d62b8410f8de2e5a12e48819d10be946a9c65729392bab0dd083d53c16307`, plus the 10 Oct `PRL_PR24_Relic_Dialogue_Audit`.
- Kanto story canon: ancient fossils and dormant Relic traits lead from Oak and Mt. Moon through Cerulean/Rock Tunnel/Celadon/Silph/Cinnabar to Giovanni, Victory Road and the League.
- PRL canon overrides PKR: **Retire PKR's legendary Relicanth target.** An **unnamed golden bird** is only rumoured and not identified as Ho-Oh in Kanto dialogue.
- This is a dialogue source pass, not a statement of full emulator verification. **Runtime approval pending.**
- No encounter, party, map trigger, item reward, save structure, sprite, opening, battle-engine or Pokémon data change is intended in this pass. Only map dialogue, tests and documentation change.

## Dialogue migration matrix
Each row is an existing in-game text label updated without changing its event command. Dialogues attached to NPC trainer battles become visible at the appropriate encounter or after defeat; signs and books remain optional.

| Location | Blocks rewritten | Player trigger and progression | Source test | Emulator QA |
|---|---:|---|---|---|
| `RocketHideout_B1F_Frlg` | 5 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `RocketHideout_B2F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `RocketHideout_B3F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `RocketHideout_B4F_Frlg` | 4 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_2F_Frlg` | 4 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_3F_Frlg` | 3 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_4F_Frlg` | 3 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_5F_Frlg` | 4 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_6F_Frlg` | 4 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_7F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_8F_Frlg` | 3 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_9F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_10F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SilphCo_11F_Frlg` | 5 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PalletTown_ProfessorOaksLab_Frlg` | 2 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PewterCity_Museum_2F_Frlg` | 2 | Pewter museum visitor | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `Route25_SeaCottage_Frlg` | 3 | Bill rescue/ticket reward, then PC research | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `Route10_Frlg` | 2 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `RockTunnel_1F_Frlg` | 2 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `RockTunnel_B1F_Frlg` | 2 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `LavenderTown_Frlg` | 1 | Lavender and Mr Fuji NPCs | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PokemonTower_7F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `LavenderTown_VolunteerPokemonHouse_Frlg` | 2 | Lavender and Mr Fuji NPCs | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `FuchsiaCity_Frlg` | 2 | Safari staff, Gym and fossil exhibits | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `FuchsiaCity_Gym_Frlg` | 1 | Safari staff, Gym and fossil exhibits | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `FuchsiaCity_SafariZone_Office_Frlg` | 1 | Safari staff, Gym and fossil exhibits | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `SafariZone_SecretHouse_Frlg` | 1 | Safari staff, Gym and fossil exhibits | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CinnabarIsland_Frlg` | 1 | Laboratory/nearby island researcher or fossil restoration | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CinnabarIsland_PokemonLab_Entrance_Frlg` | 2 | Laboratory/nearby island researcher or fossil restoration | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CinnabarIsland_PokemonLab_Lounge_Frlg` | 1 | Laboratory/nearby island researcher or fossil restoration | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CinnabarIsland_PokemonLab_ResearchRoom_Frlg` | 1 | Laboratory/nearby island researcher or fossil restoration | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CinnabarIsland_PokemonLab_ExperimentRoom_Frlg` | 4 | Laboratory/nearby island researcher or fossil restoration | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PokemonMansion_2F_Frlg` | 2 | Readable mansion diaries and nearby researcher | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PokemonMansion_3F_Frlg` | 1 | Readable mansion diaries and nearby researcher | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PokemonMansion_B1F_Frlg` | 2 | Readable mansion diaries and nearby researcher | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `Route20_Frlg` | 3 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `Route19_Frlg` | 1 | Overworld NPC/trainer conversation | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `ViridianCity_Gym_Frlg` | 3 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `VictoryRoad_1F_Frlg` | 1 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `VictoryRoad_2F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `VictoryRoad_3F_Frlg` | 2 | Rocket/trainer battle dialogue during normal Kanto progression | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `PokemonLeague_ChampionsRoom_Frlg` | 1 | Rival Champion defeat | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CeruleanCity_Frlg` | 5 | Rocket Dig robbery and house conversations | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
| `CeruleanCity_House2_Frlg` | 2 | Rocket Dig robbery and house conversations | Tracked in `test_prl_pkr_full_kanto_dialogue.py` | Pending |
 
**Continued approved material preserved**: Oak's original Relic Reports handoff; Mt. Moon fossil dig, Miguel and Jessie/James; Silph Lapras and gold-bird files; Cinnabar RELIC-01 research and Blaine; Viridian Giovanni's disbanding scene; all four distinct Jessie/James encounters; Hall of Fame and first Champion double battle.

## Remaining verification before claiming full PKR dialogue parity
1. In an emulator, start from the working Build #106 save and verify changed dialogue already encountered on an older save is not retroactively replayed.
2. Fresh-run mandatory tests: Oak handoff, Mt. Moon, Cerulean TM28, Bill's ticket, Celadon Giovanni, Silph floor scenes/President, Cinnabar fossil/diaries, Viridian final Gym, Victory Road and the Champion. Check all three starter/rival paths.
3. Check that event flags, held items, TM awards and trainer rematch state are unchanged; verify typography/line pacing in Pizza Boy.
4. Additional post-Hall-of-Fame/Seafoam special encounters should be designed only when those actual events are implemented in PRL. Do not reinstate discarded PKR Sevii expedition.
5. ROM source checks and GitHub CI do not constitute gameplay validation. Keep `pkr_story_kanto` marked `in_progress` until end-to-end QA.
