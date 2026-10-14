"""Regression for blue screens at Lorelei/Champion and HM party eligibility.

The expansion used trainer IDs 309-311 for Frontier specials, overlapping
Kanto's Cooltrainer Julie, Elite Four Lorelei and Bruno. The Champion's
frame-triggered battle also invoked a native NPC-facing trainer command.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class LoreleiChampionPartyHmRegression(unittest.TestCase):
    def test_frlg_frontier_special_ids_do_not_shadow_kanto_trainers(self):
        opponents = read("include/constants/opponents_frlg.h")
        specials = read("include/constants/trainers.h")
        self.assertRegex(opponents, r"#define TRAINER_COOLTRAINER_JULIE\s+309")
        self.assertRegex(opponents, r"#define TRAINER_ELITE_FOUR_LORELEI\s+310")
        self.assertRegex(opponents, r"#define TRAINER_ELITE_FOUR_BRUNO\s+311")
        special_block = specials.split("#if IS_FRLG", 1)[1].split("#else", 1)[0]
        self.assertIn("TRAINER_EREADER                     0xFF03", special_block)
        self.assertIn("TRAINER_FRONTIER_BRAIN              0xFF04", special_block)
        self.assertIn("TRAINER_PLAYER                      0xFF05", special_block)
        self.assertIn("(TRAINER_EREADER + 1)", specials.split("#else", 1)[1])

    def test_lorelei_unchanged_first_clear_team_and_no_intro(self):
        scripts = read("data/maps/PokemonLeague_LoreleisRoom_Frlg/scripts.inc")
        trainer = read("src/data/trainers_frlg.party")
        self.assertIn("trainerbattle_no_intro TRAINER_ELITE_FOUR_LORELEI,", scripts)
        section = trainer.split("=== TRAINER_ELITE_FOUR_LORELEI ===", 1)[1].split("\n=== TRAINER_", 1)[0]
        levels = [int(x) for x in re.findall(r"^Level: (\d+)$", section, re.M)]
        self.assertEqual(levels, [65, 65, 66, 66, 67, 69])
        self.assertIn("Dewgong @ Leftovers", section)

    def test_champion_cutscene_uses_safe_non_object_trainer_command(self):
        script = read("data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc")
        parties = read("src/data/trainers_frlg.party")
        self.assertIn("map_script MAP_SCRIPT_ON_FRAME_TABLE", script)
        for starter in ("SQUIRTLE", "BULBASAUR", "CHARMANDER"):
            self.assertIn(f"trainerbattle_no_intro TRAINER_CHAMPION_FIRST_{starter},", script)
            self.assertNotIn(f"trainerbattle_double TRAINER_CHAMPION_FIRST_{starter},", script)
            block = parties.split(f"=== TRAINER_CHAMPION_FIRST_{starter} ===", 1)[1].split("\n=== TRAINER_", 1)[0]
            self.assertIn("Double Battle: Yes", block)
            self.assertEqual(len(re.findall(r"^Level: \d+$", block, re.M)), 6)
        self.assertIn("msgbox PokemonLeague_ChampionsRoom_Text_FirstBattleIntro", script)
        self.assertIn("PokemonLeague_ChampionsRoom_EventScript_BattleComplete::", script)

    def test_party_virtual_fly_surf_requires_species_compatibility(self):
        party = read("src/party_menu.c")
        start = party.split("// HM-in-bag and badge still unlock map-based field moves", 1)[1].split("\n    if (!InBattlePike())", 1)[0]
        for move in ("FIELD_MOVE_FLASH", "FIELD_MOVE_FLY", "FIELD_MOVE_SURF"):
            self.assertIn(move, start)
        self.assertIn("GetSpeciesTeachableLearnset(species)", start)
        self.assertIn("teachableMoves[j] == move", start)
        self.assertIn("if (compatible)", start)
        self.assertIn("CheckBagHasItem(hmItem, 1)", start)
        self.assertIn("IsFieldMoveUnlocked(fieldMove)", start)
        self.assertIn("MonKnowsMove(&mons[slotId], move)", start)
        self.assertNotIn("Do not gate either action on the selected Pokémon", start)
        field = read("src/field_move.c")
        self.assertIn("[FIELD_MOVE_SURF]", field)
        self.assertIn("FLAG_BADGE05_GET", field)


if __name__ == "__main__":
    unittest.main()
