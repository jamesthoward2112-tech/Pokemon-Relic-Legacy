# PRL — Complete Team Rocket Jessie & James donor intake (9 October 2026)

**State:** owner-approved donor bank, extracted and archived. **NOT** playable in PRL yet. Separate technical integration tasks are required.

## Source and provenance

- Donor game: *Pokémon Team Rocket Jessie & James* by MrNightology, FireRed/BPRE-based.
- Original uploaded source: `TR-J&J-FINAL.zip` containing `TR-J&J-FINAL.gba`.
- SHA-256 of the extracted ROM: `03b53a3594d7f67afe3c4b6de7127027d8ae4ac64961b59a51fe1cbd0578fe53`.
- Complete donor pack: `/Pokémon Relic Legacy/Sprite Resources/Team Rocket Jessie James/PRL_Team_Rocket_COMPLETE_Donor_Bank_2026-10-09.zip` (ChatGPT Library; id `libfile_cc820416608c8191a833fd1c505e3ae4`).
- Original source ROM ZIP: `/Pokémon Relic Legacy/Sprite Resources/Team Rocket Jessie James/Original ROM/TR-J&J-FINAL.zip` (ChatGPT Library; id `libfile_01739c013b508191998c7e427246002d`).
- Existing smaller curated donor pack and technical report are in the same Library folder.
- Canonical design / owner approval: [Notion PRL sprite-port manifest](https://app.notion.com/p/3f14939d43168107a9accfa462bf0122).

No donor ROM binary is checked into public GitHub. Individual donor art credits / reuse terms require verification before public distribution.

## Inventory extracted from user-provided ROM

| Group | Exact scope | Status |
| --- | --- | --- |
| Jessie | Normal four-direction overworld walk; bicycle set; classic white battle portrait #156 and dark variant #148 | Captured; runtime hook-up pending |
| James | Four-direction overworld walk; bike/action/casual sets; white battle portrait #155, dark #154, other looks #169/#0 | Captured; runtime hook-up pending |
| Meowth | Giant Meowth balloon frame, initial sprite header `0x3A4ABC`; cat candidate `0x74D990` | Balloon captured; Meowth walking sprite **unverified** |
| Other overworld | 252 detected overworld headers (99 expanded, 153 native), 2,260 decoded frames and raw 4bpp files | All audited entries retained as donors; animation and OAM mapping not validated |
| Trainer portraits | 180 rendered IDs, 167 unique images | All retained as visual donors; may include duplicates and non-Rocket trainers |
| Reproduction | Contact sheets, header offsets, 4bpp buffers, palettes where known, extraction code and JSON/CSV | Retained for source verification |

**Counts are detection results, not claims of every graphic in the ROM.** Only up to nine inspected frames per header were exported. For native sprites whose GBA palette was not located, coloured PNGs may use approximated palettes; raw indexed frames remain for accurate reconstruction. Some graphic dimensions and palette relationships remain uncertain.

## Scope of newly approved *candidate* imports (user: "Take it all")

1. **Rocket characters/art:** Jessie, James, Meowth, all alternative Rocket uniforms, grunts/executives, trainer portraits, special-character sprites, balloon and scriptable scene graphics.
2. **Story/event assets:** Team Rocket missions, hideout/HQ scene ideas, Rocket dialogue/event choreography and recurring encounters; carry character history across Kanto, Johto, Hoenn.
3. **Graphics:** expanded Pokémon overworld sets, animated NPCs, objects, transport/train/subway visuals, any useful scene assets, palettes and battle VS bars.
4. **Possible engine features:** Battle Subway, transit and teleport systems, random-island scripts, extended overworld/music tables and dynamic overworld palettes. The compiled hack does not supply transplantable engine source code; each desired feature must be reimplemented and validated against PRL.
5. **Later-regional content:** Ruins of Alph content may remain in the bank as a Johto-era candidate. Do not import Johto systems into the current Kanto-first production build.

## No-regression / canon guardrails

- Preserve authentic FireRed Kanto geography and **Kanto-only** regional species/Eeveelutions during Kanto.
- Do not replace PRL's fixed species IDs, registered sprite locks, starters, Gym rewards/rematches or region ordering.
- Jessie, James and Meowth belong to the continuous regional Rocket plot. Meowth **must follow Jessie and James during walking scenes**, with actual walk/collision/animation logic rather than a non-animated substitute; do not mark complete until proven in emulator.
- Use PRL symbolic/object/trainer IDs, not donor ROM numeric IDs/offsets as game canonical IDs.
- Do not automatically insert unrelated donor trainer art, transport geography, Rocket script content, overworld encounters or gameplay features. Each selected asset must fit PRL and pass visual/functional QA.
- Keep this intake separate from draft Gym PR #9, storage PR #7 and other active implementation branches.

## Recommended work sequence

### Priority 1 — current Kanto
- Audit PNG/RGB555 transparency, palette widths, spritesheet animation order and normal/dark outfit variants.
- Register Jessie and James as NPC/trainer assets in PRL with proper sprite IDs; add scripted movements and dialogue continuity.
- Identify a verifiable Meowth overworld sprite with all necessary directions/animation frames. Implement a companion/follower event so Meowth walks behind Jessie and James where scripted.
- Stage the Meowth balloon for a Rocket set piece in Kanto when story placement is chosen.
- Test walking/interactions/visibility across map warps and save-load; test trainer portraits, palette persistence, and collision events.

### Priority 2 — optional Kanto enhancements
- Individually review other Rocket NPC/trainer art and animated Pokémon overworld graphics and use only where they improve already-agreed events.
- Evaluate dynamic palettes and VS-bar mechanics with source-level testing; do not import raw binary hooks.
- Evaluate Rocket mission and teleport architecture only if complementary to PRL's progression.

### Priority 3 — future regional integration
- Preserve subway/train assets and Battle Subway, map transport and Ruins of Alph donors for later. Adopt only after Kanto story advances and after separate design/compatibility checks.

## Validation gate before merge of actual game code

- `git diff --check`, existing Python suite and an **actual** `make firered` ROM build.
- Emulator playtest for all four directions, full animation loops, NPC collisions, walking/teleporting followers, scene continuity and Battle Trainer visuals.
- Full game regression: trainers and scripted events, saves, region locks, all Gym flags/rewards.
- Donor art attribution and rights review.

This branch is a **documentation/provenance checkpoint only**. This file makes the owner decision reproducible but does not itself import the donor art or functionality.
