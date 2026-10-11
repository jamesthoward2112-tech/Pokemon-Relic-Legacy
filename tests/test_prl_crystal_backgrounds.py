"""Check actual donor bytes and the source render/transition wiring."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]

class CrystalBackgroundTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        import importlib.util
        spec = importlib.util.spec_from_file_location("crystal_assets", ROOT / "tools/prl_crystal_presentation_assets.py")
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        module.unpack()

    def test_all_38_descriptors_are_used_by_every_render_path(self):
        code = (ROOT / 'src/battle_bg.c').read_text()
        self.assertIn('GetCrystalBattleVisual(environment)', code)
        self.assertNotIn('gBattleEnvironmentInfo[GetBattleEnvironmentOverride()].background', code)
        art = ROOT / 'graphics/battle_environment/crystal_redux'
        for i in range(38):
            for name in ('tiles.4bpp.lz', 'tilemap.bin.lz', 'entry_tiles.4bpp.lz', 'entry_tilemap.bin.lz'):
                self.assertEqual((art / f'{i:02}' / name).read_bytes()[0], 0x10)
            self.assertEqual(len((art / f'{i:02}' / 'palette_48colors.gbapal').read_bytes()), 96)
        header = (ROOT / 'src/data/prl_crystal_battle_visuals.h').read_text()
        self.assertEqual(len(re.findall(r'^    CRYSTAL_VISUAL\(\d+\),', header, re.M)), 38)

    def test_specific_trainers_select_exclusive_transitions(self):
        code = (ROOT / 'src/battle_setup.c').read_text()
        for label in ('B_TRANSITION_ROCKET', 'B_TRANSITION_JESSIE_JAMES', 'B_TRANSITION_EEVEE_TWINS'):
            self.assertIn('return ' + label, code)
        for duo in ('JESSIE_JAMES_MT_MOON','JESSIE_JAMES_HIDEOUT','JESSIE_JAMES_SILPH','JESSIE_JAMES_VICTORY_ROAD',
                    'STEEVE_NEEVEE_ROUTE3','STEEVE_NEEVEE_SS_ANNE','STEEVE_NEEVEE_TOWER','STEEVE_NEEVEE_ROUTE23'):
            self.assertIn('case TRAINER_' + duo + ':', code)

class MoveReorderingContextTests(unittest.TestCase):
    def test_ordinary_party_summary_remains_unlocked(self):
        code = (ROOT / 'src/party_menu.c').read_text()
        block = code[code.index('static void CB2_ShowPokemonSummaryScreen(void)\n{'):]
        block = block[:block.index('void CB2_ReturnToPartyMenuFromSummaryScreen')]
        self.assertIn('PARTY_MENU_TYPE_IN_BATTLE', block)
        self.assertIn('PARTY_MENU_TYPE_CHOOSE_HALF', block)
        self.assertIn('ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL', block)
        self.assertEqual(block.count('SUMMARY_MODE_LOCK_MOVES'), 4)

    def test_box_and_party_swaps_store_moves_pp_and_bonus_bits(self):
        code = (ROOT / 'src/pokemon_summary_screen.c').read_text()
        for setter in ('SetMonData', 'SetBoxMonData'):
            self.assertIn(setter + '(mon, MON_DATA_PP_BONUSES, &ppBonuses)', code)
            self.assertIn(setter + '(mon, MON_DATA_MOVE1 + moveIndex1, &move2)', code)
            self.assertIn(setter + '(mon, MON_DATA_PP1 + moveIndex1, &move2pp)', code)

if __name__ == '__main__':
    unittest.main()
