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

    def test_all_seven_pokemon_tower_floors_use_ground_floor_grey_background(self):
        import json
        groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
        dungeon_group = "gMapGroup_Dungeons_Frlg"
        group_id = groups["group_order"].index(dungeon_group)
        names = [f"PokemonTower_{n}F_Frlg" for n in range(1, 8)]
        floors = [groups[dungeon_group].index(name) for name in names]

        header = (ROOT / "src/data/prl_crystal_battle_visuals.h").read_text()
        defaults = re.search(
            r"static const u8 sCrystalDefaultVisuals\[BATTLE_ENVIRONMENT_COUNT\]\s*=\s*\{([^}]*)\}",
            header, re.S)
        self.assertIsNotNone(defaults)
        default_donors = [int(value) for value in re.findall(r"\d+", defaults.group(1))]
        self.assertEqual(default_donors[8], 8, "Tower 1F uses the normal indoor building backdrop")

        start = header.index("static const struct CrystalMapVisual sCrystalMapVisuals[]")
        table = header[start:header.index("\n};", start)]
        entries = re.findall(r"^\s*\{\s*(\d+),\s*(\d+),\s*(\d+)\s*\},", table, re.M)
        self.assertEqual(len(entries), len(set((g, n) for g, n, _ in entries)),
                         "Battle-map donor overrides cannot have duplicate locations")
        mappings = {(int(g), int(n)): int(d) for g, n, d in entries}
        for name, number in zip(names, floors):
            with self.subTest(floor=name):
                room = json.loads((ROOT / f"data/maps/{name}/map.json").read_text())
                self.assertEqual(room["map_type"], "MAP_TYPE_INDOOR")
                self.assertEqual(mappings.get((group_id, number), default_donors[8]), default_donors[8],
                                 "All Tower floors must show the same grey Crystal building donor")
                self.assertEqual(mappings[(group_id, number)], 8,
                                 "Record an explicit shared Tower donor to prevent future regressions")

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
