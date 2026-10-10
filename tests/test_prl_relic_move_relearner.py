"""Regression: every PRL Relic evolution inherits its full pre-evolution Move Relearner pool."""

from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
RELEARNER = ROOT / "src/move_relearner.c"
SPECIES = ROOT / "src/data/pokemon/species_info.h"
CUSTOM = ROOT / "src/data/pokemon/species_info/prl_custom.h"
KANTO_MOVES = ROOT / "src/data/pokemon/level_up_learnsets/gen_1.h"


class RelicMoveRelearnerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.code = RELEARNER.read_text()
        cls.custom = CUSTOM.read_text()

    def test_relic_evolutions_inherit_prior_stages_without_changing_global_setting(self):
        self.assertIn("ShouldInheritRelicPreEvolutionMoves(enum Species species)", self.code)
        self.assertIn('static const u8 sPRLRelicCategoryName[] = _("Relic");', self.code)
        self.assertIn("categoryName, sPRLRelicCategoryName", self.code)
        self.assertIn("species == SPECIES_NOXICHU", self.code)
        self.assertIn("P_PRE_EVO_MOVES || ShouldInheritRelicPreEvolutionMoves(species)", self.code)
        self.assertIn("species = includePreEvolutions ? GetRelearnerPreEvolution(species) : SPECIES_NONE;", self.code)
        self.assertEqual(self.code.count("species = includePreEvolutions ? GetRelearnerPreEvolution(species) : SPECIES_NONE;"), 3)

    def test_hoenn_relic_starter_links_without_changing_evolution_mechanics(self):
        for evolution, previous in [
            ("SCEPTITAN", "SCEPTILE"),
            ("SOLAZIKEN", "BLAZIKEN"),
            ("SWAMPLITH", "SWAMPERT"),
        ]:
            self.assertRegex(self.code, rf"case SPECIES_{evolution}:\s*return SPECIES_{previous};")

    def test_relearner_menu_inherits_eligible_tm_and_tutor_moves(self):
        self.assertIn("CanRelearnRelicTeachableMove(enum Species species, enum Move move)", self.code)
        self.assertEqual(self.code.count("if (!CanRelearnRelicTeachableMove(species, move))"), 4)
        self.assertIn("if (CanLearnTeachableMove(species, move))", self.code)

    def test_egg_moves_look_up_the_earliest_evolution_stage(self):
        self.assertEqual(self.code.count("while (GetRelearnerPreEvolution(species) != SPECIES_NONE)"), 2)

    def test_noxichu_can_relearn_pikachu_early_moves(self):
        source = SPECIES.read_text()
        self.assertIn("[SPECIES_NOXICHU]", source)
        self.assertIn(".levelUpLearnset = sNoxichuLevelUpLearnset", source)
        self.assertIn("species == SPECIES_NOXICHU", self.code)
        pikachu = KANTO_MOVES.read_text().split("sPikachuLevelUpLearnset[] =", 1)[1].split("LEVEL_UP_END", 1)[0]
        for move in ("THUNDER_SHOCK", "GROWL", "TAIL_WHIP", "THUNDER_WAVE", "QUICK_ATTACK"):
            self.assertIn("MOVE_" + move, pikachu)

    def test_relearner_buffers_do_not_overflow_after_inheritance(self):
        self.assertIn("numMoves < MAX_RELEARNER_MOVES", self.code)
        self.assertGreaterEqual(self.code.count("numMoves < MAX_RELEARNER_MOVES"), 4)

    def test_all_custom_relic_species_share_the_relic_marker(self):
        species = re.findall(r"\[SPECIES_([A-Z0-9_]+)\]\s*=\s*\{", self.custom)
        self.assertGreaterEqual(len(species), 40)
        # This marker is what opt-in Relic evolution families to inherited moves.
        self.assertGreaterEqual(self.custom.count('.categoryName = _("Relic")'), 40)


if __name__ == "__main__":
    unittest.main()
