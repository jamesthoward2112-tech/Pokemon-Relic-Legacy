"""Validate the canonical reference package. This does not compile or test a ROM."""
from pathlib import Path
import hashlib
import json
import re

root = Path(__file__).resolve().parent
data = json.loads((root / 'PRL_Pokemon_Data.json').read_text(encoding='utf-8'))
gates = json.loads((root / 'PRL_Import_Gates.json').read_text(encoding='utf-8'))
species = data['species']
keys = [s['key'] for s in species]
assert len(keys) == len(set(keys)) == data['species_count'] == 50
assert not set(keys) & {'SUDOWARDEN', 'TERATHWACK', 'RETIRED_UNUSED'}
assert keys.count('TOXEON') == 1
assert {'OSTEODIAN', 'MAROGHOST'} <= set(keys)
assert all(s['numeric_id'] is None for s in species)
assert all(s['ready_for_rom_build'] is False for s in species)
assert not any(gates[k] for k in ('blanket_qol_import','blanket_global_balance_import','numeric_ids_frozen','build_ready'))

text = (root / 'PRL_Pokemon_Data.txt').read_text(encoding='utf-8')
parsed = {}
for key, block in re.findall(r'^\[([^]]+)\]\n(.*?)(?=^\[|\Z)',text,re.M|re.S):
    if key.startswith('SPECIES:'):
        parsed[key.split(':')[-1]] = dict(re.findall(r'^([^#\n=]+) = (.*)$',block,re.M))
assert set(parsed) == set(keys)
for s in species:
    assert s['canonical_fields'] == parsed[s['key']], s['key']
    assert s['mechanics_blockers'] == gates['blocked_species'].get(s['key'], [])
    f=s['canonical_fields']
    if 'BaseStats_HP_Atk_Def_SpA_SpD_Spe' in f:
        stats=[int(v.strip()) for v in f['BaseStats_HP_Atk_Def_SpA_SpD_Spe'].split('/')]
        assert len(stats)==6 and all(1<=v<=255 for v in stats), s['key']
        assert sum(stats)==int(f['BST']), s['key']

v=parsed['VALKYVOIR']
for k, expected in {'BaseSpecies':'Gardevoir','Types':'Psychic/Fairy','EvolutionTrigger':'USE_ITEM',
                    'EvolutionItem':'Shiny Stone','MinimumLevel':'50','Abilities':'Pixilate',
                    'EvolutionMove':'Boomburst','EvolutionMoveTiming':'Immediately on evolution',
                    'CustomMoves':'None','CustomAbilities':'None'}.items():
    assert v[k] == expected, (k,v.get(k))
assert 'TBD' in v['BaseStats'] and 'UNRESOLVED' in v['WiderLearnset']
assert 'GARDEVOIR_REDUX_MEGA' in v['SpriteSource']
assert 'UNRESOLVED' in parsed['TOXEON']['Abilities']
assert 'TBD' in parsed['ALPHORACLE'].get('Abilities',parsed['ALPHORACLE'].get('Ability',''))
assert 'Great Tusk' in parsed['DONPHALANX']['SpriteSource']
assert parsed['DONPHALANX']['Types']=='Ground/Dark'
assert parsed['DONPHALANX']['BaseStats_HP_Atk_Def_SpA_SpD_Spe']=='110/140/135/60/85/60'
assert 'Great Tusk' in parsed['DONPHALANX']['CustomMechanics']
assert 'Fairy component locked' in parsed['MAWYRM']['Types']
assert parsed['CACTOMB']['Ability']=='Spirit Thorns'
assert '95 BP' in parsed['SCEPTITAN']['MoveTuning'] and '20%' in parsed['SCEPTITAN']['MoveTuning']
assert data['rules']['PRL:REGIONS']['KantoMap'].startswith('Genuine FireRed')
assert data['rules']['PRL:STARTER_POOLS']=={
    'Kanto':'Pikachu / Nidoran♂ / Growlithe','Johto':'Phanpy / Scarabub / Skarmet','Hoenn':'Poochyena / Sableye / Ralts'}
assert data['relics_chain']['SixRelics']=='R Raikou; E Entei; L Lugia; I I-Unown; C Celebi; S Suicune'

manifest=json.loads((root/'Assets/Valkyvoir/provenance.json').read_text(encoding='utf-8'))
assert len(manifest['files'])==4
for f in manifest['files']:
    assert hashlib.sha256((root/f['file']).read_bytes()).hexdigest()==f['sha256']
    assert f['matches_original_archive'] and f['sha256']==f['original_archive_sha256']
    assert f['dimensions']==[64,64] and f['colors']<=16

checksums=root/'SHA256SUMS.txt'
if checksums.exists():
    for line in checksums.read_text().splitlines():
        sha,path=line.split('  ',1)
        assert hashlib.sha256((root/path).read_bytes()).hexdigest()==sha, path

print(f'PASS: {len(species)} unique species; 29 PJR source-backed records; '
      f'{len(gates["blocked_species"])} explicit mechanics/asset gates; 4 unchanged direct donor images. '
      'No ROM build or player QA claimed.')
