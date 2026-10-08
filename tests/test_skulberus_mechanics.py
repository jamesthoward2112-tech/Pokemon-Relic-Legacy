from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class SkulberusMechanicsTests(unittest.TestCase):
    def source(self, path):
        return (ROOT / path).read_text()

    def test_species_selectable_abilities_and_three_innates_are_live(self):
        species = self.source('src/data/pokemon/species_info/prl_custom.h')
        self.assertIn('[SPECIES_SKULBERUS]', species)
        self.assertIn('ABILITY_STYGIAN_RUSH', species)
        battle = self.source('src/battle_main.c')
        self.assertIn('ABILITY_STYGIAN_RUSH', battle)
        util = self.source('src/battle_util.c')
        self.assertIn('SPECIES_SKULBERUS', util)
        self.assertIn('STATUS1_BLEED', self.source('include/constants/battle.h'))

    def test_original_custom_moves_are_defined_and_biting(self):
        moves = self.source('src/data/moves_info.h')
        for name in ('LOVELY_BITE', 'SHADOW_FANGS', 'RIP_AND_TEAR'):
            self.assertIn(f'[MOVE_{name}]', moves)
        self.assertIn('MOVE_EFFECT_BLEED', moves)

    def test_bleed_engine_hooks_cover_damage_and_status_immunities(self):
        self.assertIn('STATUS1_BLEED', self.source('src/battle_end_turn.c'))
        self.assertIn('STATUS1_BLEED', self.source('src/battle_util.c'))
        self.assertIn('MOVE_EFFECT_BLEED', self.source('src/battle_script_commands.c'))
        self.assertIn('STATUS1_BLEED', self.source('src/battle_stat_change.c'))

if __name__ == '__main__':
    unittest.main()
