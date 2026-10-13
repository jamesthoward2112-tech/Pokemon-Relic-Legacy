"""Approved 13 October story teams; exact sets and preserved first-clear data."""
import hashlib
import json
import re
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
PARTIES = ROOT / 'src/data/trainers_frlg.party'
# species|level|ability|item|four moves, in the approved battle order.
RAW = {
'ELITE_FOUR_LORELEI': '''Dewgong|65|Thick Fat|Leftovers|Ice Beam/Surf/Encore/Icy Wind
Cloyster|65|Skill Link|White Herb|Shell Smash/Icicle Spear/Rock Blast/Ice Shard
Slowbro|66|Regenerator|Rocky Helmet|Scald/Psychic/Slack Off/Thunder Wave
Jynx|66|Dry Skin|Focus Sash|Ice Beam/Psychic/Lovely Kiss/Focus Blast
Starmie|67|Natural Cure|Expert Belt|Surf/Thunderbolt/Ice Beam/Recover
Lapras|69|Water Absorb|Assault Vest|Freeze-Dry/Hydro Pump/Thunderbolt/Ice Shard''',
'ELITE_FOUR_BRUNO': '''Onix|66|Sturdy|Eviolite|Stealth Rock/Earthquake/Rock Slide/Dragon Tail
Primeape|66|Defiant|Choice Scarf|Close Combat/U-turn/Stone Edge/Ice Punch
Hitmonlee|67|Reckless|Life Orb|High Jump Kick/Blaze Kick/Stone Edge/Knock Off
Hitmonchan|68|Iron Fist|Expert Belt|Drain Punch/Mach Punch/Ice Punch/Thunder Punch
Poliwrath|69|Water Absorb|Leftovers|Liquidation/Drain Punch/Bulk Up/Ice Punch
Machamp|70|No Guard|Assault Vest|Dynamic Punch/Stone Edge/Knock Off/Bullet Punch''',
'ELITE_FOUR_AGATHA': '''Arbok|67|Intimidate|Black Sludge|Coil/Gunk Shot/Crunch/Earthquake
Golbat|67|Infiltrator|Eviolite|Brave Bird/Roost/U-turn/Toxic
Weezing|68|Levitate|Rocky Helmet|Sludge Bomb/Will-O-Wisp/Flamethrower/Pain Split
Muk|68|Poison Touch|Assault Vest|Gunk Shot/Knock Off/Shadow Sneak/Ice Punch
Maroghost|70|Cursed Body|Spell Tag|Shadow Bone/Bonemerang/Will-O-Wisp/Stone Edge
Gengar|71|Cursed Body|Life Orb|Shadow Ball/Sludge Bomb/Thunderbolt/Focus Blast''',
'ELITE_FOUR_LANCE': '''Gyarados|68|Intimidate|Lum Berry|Dragon Dance/Waterfall/Ice Fang/Earthquake
Seadra|68|Sniper|Scope Lens|Focus Energy/Hydro Pump/Dragon Pulse/Ice Beam
Dragonair|69|Shed Skin|Eviolite|Dragon Dance/Dragon Claw/Thunder Wave/Aqua Tail
Aerodactyl|70|Rock Head|Choice Band|Stone Edge/Earthquake/Aerial Ace/Crunch
Charizard|71|Blaze|Expert Belt|Flamethrower/Air Slash/Dragon Pulse/Focus Blast
Dragonite|72|Multiscale|Weakness Policy|Dragon Dance/Dragon Claw/Thunder Punch/Extreme Speed''',
'PRL_BADGE_RECOVERY_BROCK': '''Onix|56|Sturdy|Eviolite|Stealth Rock/Earthquake/Rock Slide/Dragon Tail
Omastar|57|Shell Armor|White Herb|Shell Smash/Surf/Ice Beam/Earth Power
Kabutops|58|Battle Armor|Life Orb|Swords Dance/Liquidation/Stone Edge/Aqua Jet
Golem|59|Sturdy|Leftovers|Earthquake/Stone Edge/Fire Punch/Sucker Punch
Aerodactyl|60|Rock Head|Choice Band|Stone Edge/Earthquake/Aerial Ace/Crunch
Rhydon|61|Rock Head|Eviolite|Earthquake/Rock Slide/Megahorn/Ice Punch''',
'PRL_BADGE_RECOVERY_MISTY': '''Golduck|57|Cloud Nine|Expert Belt|Surf/Ice Beam/Psychic/Encore
Cloyster|58|Skill Link|White Herb|Shell Smash/Icicle Spear/Rock Blast/Ice Shard
Vaporeon|59|Water Absorb|Leftovers|Scald/Ice Beam/Wish/Protect
Lapras|60|Water Absorb|Assault Vest|Freeze-Dry/Surf/Thunderbolt/Ice Shard
Starmie|61|Natural Cure|Expert Belt|Surf/Psychic/Ice Beam/Recover
Starmie|62|Natural Cure|Life Orb|Hydro Pump/Psychic/Thunderbolt/Ice Beam''',
'PRL_BADGE_RECOVERY_LT_SURGE': '''Electrode|58|Static|Light Clay|Thunderbolt/Volt Switch/Reflect/Light Screen
Magneton|59|Sturdy|Eviolite|Thunderbolt/Flash Cannon/Volt Switch/Thunder Wave
Electabuzz|60|Static|Eviolite|Thunderbolt/Focus Blast/Ice Punch/Volt Switch
Jolteon|61|Volt Absorb|Expert Belt|Thunderbolt/Shadow Ball/Signal Beam/Volt Switch
Noxichu|62|Infiltrator|Life Orb|Thunderbolt/Dark Pulse/Nasty Plot/Extreme Speed
Raichu|63|Lightning Rod|Air Balloon|Thunderbolt/Focus Blast/Grass Knot/Nasty Plot''',
'PRL_BADGE_RECOVERY_ERIKA': '''Tangela|59|Regenerator|Eviolite|Giga Drain/Knock Off/Sleep Powder/Leech Seed
Parasect|60|Dry Skin|Leftovers|Spore/Seed Bomb/X-Scissor/Knock Off
Exeggutor|61|Harvest|Sitrus Berry|Giga Drain/Psychic/Sleep Powder/Reflect
Vileplume|62|Effect Spore|Black Sludge|Giga Drain/Sludge Bomb/Moonblast/Moonlight
Victreebel|63|Chlorophyll|Life Orb|Giga Drain/Sludge Bomb/Sleep Powder/Sunny Day
Victreebel|64|Chlorophyll|Heat Rock|Sunny Day/Solar Beam/Weather Ball/Sludge Bomb''',
'PRL_BADGE_RECOVERY_KOGA': '''Muk|60|Poison Touch|Assault Vest|Gunk Shot/Knock Off/Shadow Sneak/Ice Punch
Venomoth|61|Tinted Lens|Black Sludge|Quiver Dance/Bug Buzz/Sludge Bomb/Sleep Powder
Arbok|62|Intimidate|Black Sludge|Coil/Gunk Shot/Crunch/Earthquake
Nidoking|63|Sheer Force|Life Orb|Earth Power/Sludge Bomb/Ice Beam/Thunderbolt
Weezing|64|Levitate|Rocky Helmet|Sludge Bomb/Will-O-Wisp/Flamethrower/Pain Split
Nidoqueen|65|Sheer Force|Life Orb|Earth Power/Sludge Bomb/Ice Beam/Thunderbolt''',
'PRL_BADGE_RECOVERY_SABRINA': '''Mr Mime|61|Filter|Light Clay|Psychic/Dazzling Gleam/Reflect/Light Screen
Hypno|62|Insomnia|Leftovers|Psychic/Shadow Ball/Thunder Wave/Protect
Slowbro|63|Regenerator|Rocky Helmet|Scald/Psychic/Slack Off/Thunder Wave
Jynx|64|Dry Skin|Focus Sash|Ice Beam/Psychic/Lovely Kiss/Focus Blast
Exeggutor|65|Harvest|Sitrus Berry|Giga Drain/Psychic/Sleep Powder/Reflect
Alakazam|66|Magic Guard|Life Orb|Psychic/Dazzling Gleam/Shadow Ball/Focus Blast''',
'PRL_BADGE_RECOVERY_BLAINE': '''Ninetales|62|Drought|Heat Rock|Flamethrower/Solar Beam/Will-O-Wisp/Nasty Plot
Rapidash|63|Flash Fire|Life Orb|Flare Blitz/High Horsepower/Megahorn/Morning Sun
Flareon|64|Guts|Flame Orb|Flare Blitz/Facade/Superpower/Quick Attack
Magmar|65|Flame Body|Eviolite|Flamethrower/Thunderbolt/Focus Blast/Will-O-Wisp
Arcanine|66|Intimidate|Leftovers|Flare Blitz/Extreme Speed/Crunch/Morning Sun
Arcanine|67|Intimidate|Life Orb|Flare Blitz/Extreme Speed/Wild Charge/Close Combat''',
'PRL_BADGE_RECOVERY_GIOVANNI': '''Tauros|63|Intimidate|Life Orb|Body Slam/Earthquake/Zen Headbutt/Stone Edge
Pinsir|64|Mold Breaker|Life Orb|Swords Dance/X-Scissor/Close Combat/Stone Edge
Kabutops|65|Battle Armor|Life Orb|Swords Dance/Liquidation/Stone Edge/Aqua Jet
Aerodactyl|66|Rock Head|Choice Band|Stone Edge/Earthquake/Aerial Ace/Crunch
Nidoqueen|67|Sheer Force|Life Orb|Earth Power/Sludge Bomb/Ice Beam/Thunderbolt
Nidoking|68|Sheer Force|Life Orb|Earth Power/Sludge Bomb/Ice Beam/Thunderbolt''',
}
PIDGEOT = 'Pidgeot|71|Big Pecks|Focus Sash|Tailwind/Air Slash/Heat Wave/Protect'
ALAKAZAM = 'Alakazam|72|Magic Guard|Wise Glasses|Psychic/Dazzling Gleam/Shadow Ball/Protect'
NIDOKING = 'Nidoking|73|Sheer Force|Life Orb|Earth Power/Sludge Bomb/Ice Beam/Thunderbolt'
ARCANINE = 'Arcanine|74|Intimidate|Leftovers|Flare Blitz/Extreme Speed/Rock Slide/Protect'
EXEGGUTOR = 'Exeggutor|72|Harvest|Sitrus Berry|Giga Drain/Psychic/Sleep Powder/Reflect'
RHYDON = 'Rhydon|72|Lightning Rod|Eviolite|High Horsepower/Rock Slide/Megahorn/Protect'
RAICHU = 'Raichu|72|Lightning Rod|Air Balloon|Thunderbolt/Fake Out/Nuzzle/Protect'
GYARADOS = 'Gyarados|72|Intimidate|Lum Berry|Waterfall/Ice Fang/Dragon Dance/Protect'
for suffix, pair in [('SQUIRTLE', (EXEGGUTOR, RHYDON)), ('BULBASAUR', (RAICHU, GYARADOS)), ('CHARMANDER', (GYARADOS, RHYDON))]:
    RAW['CHAMPION_FIRST_' + suffix] = '\n'.join([PIDGEOT, *pair, ALAKAZAM, NIDOKING, ARCANINE])


def blocks():
    return dict(re.findall(r'^=== (TRAINER_\w+) ===\n(.*?)(?=^=== |\Z)', PARTIES.read_text(), re.M | re.S))


def parse_party(block):
    rows = []
    for match in re.finditer(r'^([^\n]+) @ ([^\n]+)\nLevel: (\d+)\n(.*?)(?=\n\n|\Z)', block, re.M | re.S):
        species, item, level, attrs = match.groups()
        assert re.search(r'^Ability: (.+)$', attrs, re.M), f'{species} needs an explicit ability'
        rows.append(dict(species=species, level=int(level), ability=re.search(r'^Ability: (.+)$', attrs, re.M).group(1), item=item, moves=re.findall(r'^- (.+)$', attrs, re.M), shiny='Shiny: Yes' in attrs, ivs=[int(x) for x in re.findall(r'(\d+) (?:HP|Atk|Def|SpA|SpD|Spe)', attrs)]))
    return rows


def test_approved_exact_sets(suffix):
    block = blocks()['TRAINER_' + suffix]
    actual = parse_party(block)
    assert len(actual) == 6
    for mon, raw in zip(actual, RAW[suffix].splitlines()):
        species, level, ability, item, moves = raw.split('|')
        assert (mon['species'], mon['level'], mon['ability'], mon['item'], mon['moves']) == (species, int(level), ability, item, moves.split('/'))
        assert mon['ivs'] == [30 if suffix.startswith('ELITE_FOUR') else 31] * 6
        assert mon['shiny'] == ((suffix == 'ELITE_FOUR_AGATHA' and species == 'Gengar') or (suffix.startswith('CHAMPION_FIRST') and species == 'Alakazam'))
    assert ('Double Battle: Yes' in block) == suffix.startswith('CHAMPION_FIRST')
    assert 'AI: Check Bad Move / Try To Faint / Check Viability' in block


def test_actual_starter_dispatch_is_preserved():
    source = (ROOT / 'src/starter_choose.c').read_text()
    array = re.search(r'sStarterMon\[STARTER_MON_COUNT\] =\s*\{([^}]+)', source, re.S).group(1)
    assert re.findall(r'SPECIES_\w+', array) == ['SPECIES_PIKACHU', 'SPECIES_NIDORAN_M', 'SPECIES_GROWLITHE']
    script = (ROOT / 'data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc').read_text()
    for index, suffix in enumerate(['Squirtle', 'Bulbasaur', 'Charmander']):
        assert f'call_if_eq VAR_STARTER_MON, {index}, PokemonLeague_ChampionsRoom_EventScript_Battle{suffix}' in script
        assert f'trainerbattle_double TRAINER_CHAMPION_FIRST_{suffix.upper()}' in script
    assert 'PokemonLeague_ChampionsRoom_EventScript_BattleComplete' in script


def test_runtime_manifest_matches_source():
    manifest = json.loads((ROOT / 'tools/prl/final_kanto_parties.json').read_text())
    for suffix in RAW:
        expected = parse_party(blocks()['TRAINER_' + suffix])
        assert manifest['trainers']['TRAINER_' + suffix]['party'] == expected


class FinalKantoParties(unittest.TestCase):
    def test_exact_sets(self):
        for suffix in RAW:
            with self.subTest(trainer=suffix):
                test_approved_exact_sets(suffix)

    def test_dispatch(self):
        test_actual_starter_dispatch_is_preserved()

    def test_preservation(self):
        test_preserved_other_parties_and_headers()

    def test_legal_abilities_and_constants(self):
        test_legal_abilities_and_constants()

    def test_manifest(self):
        test_runtime_manifest_matches_source()


def test_preserved_other_parties_and_headers():
    source = blocks()
    targets = {'TRAINER_' + suffix for suffix in RAW}
    untouched = ''.join(k + '\n' + v for k, v in source.items() if k not in targets)
    assert hashlib.sha256(untouched.encode()).hexdigest() == '58d13ca3722d6ddb02255659bdf233dbf491964c0a88568b25a9d6b704c1caa3'
    headers = {key: value.split('\n\n', 1)[0] for key, value in source.items() if key in targets}
    assert hashlib.sha256(json.dumps(headers, sort_keys=True).encode()).hexdigest() == 'af116c620d5bc9ba3e0ffcd326e68f7d6748a8ed436ac310b9dd8347fc2afe5a'


def test_legal_abilities_and_constants():
    constants = '\n'.join(p.read_text() for p in (ROOT / 'include/constants').glob('*.h'))
    species_source = '\n'.join(p.read_text() for p in [ROOT / 'src/data/pokemon/species_info.h', *(ROOT / 'src/data/pokemon/species_info').glob('*.h')])
    for suffix in RAW:
        for mon in parse_party(blocks()['TRAINER_' + suffix]):
            def constant(prefix, name):
                return prefix + '_' + name.upper().replace(' ', '_').replace('-', '_')
            species = constant('SPECIES', mon['species'])
            ability = constant('ABILITY', mon['ability'])
            for name in [species, ability, constant('ITEM', mon['item']), *[constant('MOVE', move) for move in mon['moves']]]:
                assert re.search(r'\b' + name + r'\b', constants), name
            block = re.search(r'\[' + species + r'\]\s*=\s*\{(.*?)(?=\n\s*\[SPECIES_|\Z)', species_source, re.S).group(1)
            abilities = re.search(r'\.abilities\s*=\s*([^\n]+)', block).group(1)
            if 'GENGAR_ABILITIES' in abilities:
                assert ability == 'ABILITY_CURSED_BODY'
                assert re.search(r'GENGAR_ABILITIES.*CURSED_BODY', species_source)
            else:
                assert ability in abilities, (species, ability, abilities)


if __name__ == '__main__':
    unittest.main()
