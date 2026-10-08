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
        specs = {
            'LOVELY_BITE': ('power = 85', 'TYPE_FAIRY', 'accuracy = 100', 'pp = 15', 'MOVE_EFFECT_INFATUATION', 'chance = 10'),
            'SHADOW_FANGS': ('power = 80', 'TYPE_GHOST', 'accuracy = 100', 'pp = 15', 'MOVE_EFFECT_CURSE', 'chance = 10'),
            'RIP_AND_TEAR': ('power = 110', 'TYPE_DARK', 'accuracy = 90', 'pp = 5', 'MOVE_EFFECT_STAT_MINUS', 'speed = 1', 'MOVE_EFFECT_BLEED', 'chance = 50', 'gBattleAnimMove_Crunch'),
        }
        for name, requirements in specs.items():
            start = moves.index(f'[MOVE_{name}]')
            end = moves.find('\n[MOVE_', start + 1)
            entry = moves[start:end if end != -1 else None]
            for requirement in requirements:
                self.assertTrue(requirement in entry)
            self.assertTrue('.makesContact = TRUE' in entry and '.bitingMove = TRUE' in entry)

    def test_early_catalyst_and_donor_progression_are_preserved(self):
        species = self.source('src/data/pokemon/species_info/gen_3_families.h')
        poochyena = species.split('[SPECIES_POOCHYENA]', 1)[1].split('[SPECIES_MIGHTYENA]', 1)[0]
        self.assertTrue('.itemCommon = ITEM_HARD_STONE' in poochyena)
        evolutions = self.source('src/data/pokemon/species_info/gen_3_families.h')
        self.assertTrue('SPECIES_SKULBERUS, CONDITIONS({IF_HOLD_ITEM, ITEM_HARD_STONE})' in evolutions)
        learnset = self.source('src/data/pokemon/level_up_learnsets/prl_custom.h')
        skulberus = learnset.split('sSkulberusLevelUpLearnset[]', 1)[1].split('sGemigoyleLevelUpLearnset[]', 1)[0]
        for move in ('MOVE_LOVELY_BITE', 'MOVE_SHADOW_FANGS', 'MOVE_RIP_AND_TEAR'):
            self.assertTrue(move in skulberus)

    def test_species_slot_stats_type_and_donor_graphics_are_integrated(self):
        species = self.source('src/data/pokemon/species_info/prl_custom.h')
        skulberus = species.split('[SPECIES_SKULBERUS]', 1)[1].split('[SPECIES_GEMIGOYLE]', 1)[0]
        for field in (
            '.baseHP = 100', '.baseAttack = 135', '.baseDefense = 90',
            '.baseSpAttack = 60', '.baseSpDefense = 80', '.baseSpeed = 115',
            '.types = MON_TYPES(TYPE_DARK)',
            '.frontPic = gMonFrontPic_Skulberus',
            '.backPic = gMonBackPic_Skulberus',
            '.palette = gMonPalette_Skulberus',
            '.shinyPalette = gMonShinyPalette_Skulberus',
            '.iconSprite = gMonIcon_Skulberus',
        ):
            self.assertIn(field, skulberus)

        graphics = self.source('src/data/graphics/pokemon.h')
        for asset in ('front.png', 'back.png', 'normal.pal', 'shiny.pal', 'icon.png'):
            self.assertIn(f'graphics/pokemon/skulberus/{asset}', graphics)
            self.assertTrue((ROOT / f'graphics/pokemon/skulberus/{asset}').is_file())

        canonical = self.source('docs/prl-canonical/PRL_Pokemon_Data.json')
        ids = self.source('include/constants/prl_ids.h')
        self.assertIn('"PRLNumericID": "PRL013"', canonical)
        self.assertIn('#define PRL_SPECIES_SKULBERUS SPECIES_SKULBERUS // PRL013', ids)
        self.assertNotIn('[SPECIES_HYENADON]', self.source('include/constants/species.h'))

    def test_runtime_battle_regressions_cover_all_three_innates_and_priority(self):
        cases = self.source('test/battle/ability/skulberus.c')
        for behavior in ('Stygian Rush gives Skulberus Dark moves priority', 'Guard Dog innate blocks Attack drops', 'Shadow Tag innate prevents escape'):
            self.assertIn(behavior, cases)

    def test_runtime_battle_regressions_cover_bleed_turns_healing_and_boosts(self):
        cases = self.source('test/battle/status1/bleed.c')
        for behavior in ('Bleed deals one sixteenth', 'healing move cures Bleed', 'Bleed blocks positive stat changes'):
            self.assertIn(behavior, cases)

    def test_bleed_engine_hooks_cover_damage_and_status_immunities(self):
        self.assertIn('STATUS1_BLEED', self.source('src/battle_end_turn.c'))
        self.assertIn('STATUS1_BLEED', self.source('src/battle_util.c'))
        self.assertIn('MOVE_EFFECT_BLEED', self.source('src/battle_script_commands.c'))
        self.assertIn('STATUS1_BLEED', self.source('src/battle_stat_change.c'))
        self.assertIn('ABILITY_GUARD_DOG', self.source('src/battle_stat_change.c'))
        self.assertIn('gBattleMons[cv->battlerDef].species == SPECIES_SKULBERUS', self.source('src/battle_stat_change.c'))
        self.assertIn('CureBleedWithHealingMove', self.source('include/battle.h'))
        self.assertIn('MOVE_RIP_AND_TEAR && gLastResultingMoves', self.source('src/battle_move_resolution.c'))

if __name__ == '__main__':
    unittest.main()
