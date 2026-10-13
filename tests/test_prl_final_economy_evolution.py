"""Final integration contracts for the two narrowly approved source patches."""
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASE = 'origin/prl/giovanni-skeletal-fossils-20261012'
def read(path): return (ROOT / path).read_text()
def baseline(path): return subprocess.check_output(['git', 'show', f'{BASE}:{path}'], cwd=ROOT, text=True)

def test_only_approved_prices_changed():
    def prices(source):
        return {m[0]:m[1] for m in re.findall(r'\[(ITEM_\w+)\]\s*=\s*\{.*?\.price\s*=\s*([^,]+),', source, re.S)}
    old = prices(baseline('src/data/items.h'))
    expected = dict(old, ITEM_PP_MAX='5000', ITEM_MAX_REVIVE='3000')
    assert prices(read('src/data/items.h')) == expected

def test_kangaskhan_requires_bell_without_friendship_gate():
    path = 'src/data/pokemon/species_info/gen_1_families.h'
    original = baseline(path)
    expected = original.replace('EVO_LEVEL, 0, SPECIES_KHANG, CONDITIONS({IF_MIN_FRIENDSHIP, FRIENDSHIP_EVO_THRESHOLD})', 'EVO_LEVEL, 0, SPECIES_KHANG, CONDITIONS({IF_HOLD_ITEM, ITEM_SOOTHE_BELL})')
    assert expected != original
    assert read(path) == expected

def test_khang_only_description_changed():
    path = 'src/data/pokemon/species_info/prl_custom.h'
    expected = baseline(path).replace('A Pokemon awakened by Relic Resonance.\\nIts ancient power has taken a new form.', 'After its mother passed on,\\nit carries her soothing bell.\\nNow it stands on its own.')
    extract = lambda source: source.split('[SPECIES_KHANG]')[1].split('[SPECIES_')[0]
    assert extract(read(path)) == extract(expected)

def test_fuchsia_and_indigo_preserve_expanded_stock():
    path = 'data/maps/FuchsiaCity_Mart_Frlg/scripts.inc'
    assert read(path) == baseline(path).replace('\t.2byte ITEM_NONE', '\t.2byte ITEM_SOOTHE_BELL\n\t.2byte ITEM_NONE')
    path = 'data/maps/IndigoPlateau_PokemonCenter_1F_Frlg/scripts.inc'
    assert read(path) == baseline(path)

def test_safari_only_extra_scarf_removed_and_old_flag_reserved():
    path = 'data/maps/SafariZone_West_Frlg/map.json'
    expected = json.loads(baseline(path))
    expected['object_events'] = [o for o in expected['object_events'] if o['script'] != 'SafariZone_West_EventScript_ItemSilkScarf']
    assert json.loads(read(path)) == expected
    assert 'SafariZone_West_EventScript_ItemSilkScarf::' not in read('data/scripts/item_ball_scripts_frlg.inc')
    flags = read('include/constants/flags_frlg.h')
    assert re.search(r'#define FLAG_HIDE_PRL_SAFARI_WEST_SILK_SCARF\s+FLAG_0x8FC', flags)
    assert 'Reserved legacy Safari West Silk Scarf pickup flag; do not reuse for save compatibility.' in flags

def test_safari_qol_and_khang_return_graphics_unchanged():
    for path in ('src/safari_zone.c', 'src/data/pokemon/level_up_learnsets/prl_custom.h'):
        assert read(path) == baseline(path)
    assert 'LEVEL_UP_MOVE(0, MOVE_RETURN)' in read('src/data/pokemon/level_up_learnsets/prl_custom.h')
    assert '(IS_FRLG && (gBattleTypeFlags & BATTLE_TYPE_SAFARI)) ? 20' in read('src/battle_main.c')
    for path in sorted((ROOT / 'graphics/pokemon/khang').iterdir()):
        if path.suffix in ('.png', '.pal'):
            old = subprocess.check_output(['git', 'show', f'{BASE}:{path.relative_to(ROOT)}'], cwd=ROOT)
            actual = path.read_bytes()
            if path.suffix == '.pal':
                actual, old = actual.replace(b'\r\n', b'\n'), old.replace(b'\r\n', b'\n')
            assert actual == old
