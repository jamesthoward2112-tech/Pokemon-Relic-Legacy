import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8-sig")

class Step12StaticIntegrationTests(unittest.TestCase):
    def test_manifest_has_no_pending_hooks_and_all_nine_are_completed(self):
        manifest = json.loads(read("docs/prl_step10_integration_manifest.json"))
        expected = {
            "Ancient Grove", "Solar Discipline", "Tidal Bastion", "Ancient Core",
            "Wishmaker", "Eden's Canopy", "Thermal Wings", "Citadel Shell",
            "Relic Wish post-hit healing",
        }
        self.assertEqual(manifest["runtime_hooks_pending"], [])
        self.assertEqual(set(manifest["runtime_hooks_completed"]), expected)

    def test_evolution_condition_engine_and_canonical_requirements(self):
        constants = read("include/constants/pokemon.h")
        pokemon = read("src/pokemon.c")
        self.assertIn("IF_MIN_LEVEL", constants)
        self.assertIn("IF_MAP_TYPE", constants)
        self.assertRegex(pokemon, r"case IF_MIN_LEVEL:\s*if \(level >= params\[i\]\.arg1\)")
        self.assertRegex(pokemon, r"case IF_MAP_TYPE:\s*if \(gMapHeader\.mapType == params\[i\]\.arg1\)")
        families = read("src/data/pokemon/species_info/gen_1_families.h") + read("src/data/pokemon/species_info/gen_2_families.h")
        stone_rules = re.findall(r"EVO_ITEM, ITEM_ANCIENT_STONE, SPECIES_([A-Z0-9_]+), CONDITIONS\(\{IF_MIN_LEVEL, (\d+)\}\)", families)
        self.assertEqual(len(stone_rules), 12)
        expected = {"EDENSAUR": "45", "CHARAXIS": "45", "FORTOTOISE": "45", "MYSTYNX": "40",
                    "FAERANIUM": "45", "PYROCLAST": "45", "FERALODON": "45", "SHUCKOLOSSE": "40",
                    "HERACURION": "40", "SKARMADON": "40", "DONPHALANX": "40", "MILTITAN": "40"}
        self.assertEqual(dict(stone_rules), expected)
        self.assertRegex(families, r"EVO_LEVEL, 50, SPECIES_GHOULBAT, CONDITIONS\(\{IF_TIME, TIME_NIGHT\}, \{IF_MAP_TYPE, MAP_TYPE_UNDERGROUND\}\)")

    def test_donphalanx_uses_great_tusk_visual_and_learnset_tables(self):
        data = read("src/data/pokemon/species_info/prl_custom.h")
        start = data.index("[SPECIES_DONPHALANX]")
        end = data.index("[SPECIES_MYSTYNX]", start)
        block = data[start:end]
        for symbol in ("speciesName = _(\"Donphalanx\")", "gMonFrontPic_GreatTusk",
                       "gMonBackPic_GreatTusk", "gMonPalette_GreatTusk",
                       "gMonShinyPalette_GreatTusk", "gMonIcon_GreatTusk",
                       "sDonphalanxLevelUpLearnset", "sGreatTuskTeachableLearnset"):
            self.assertIn(symbol, block)
        self.assertNotIn("IronTreads", block)
        # Step 4B requires a unique Donphalanx table, but all Great Tusk
        # donor level-up entries must survive in the exact same order.
        pattern = r"static const struct LevelUpMove s{}LevelUpLearnset\[\]\s*=\s*\{([\s\S]*?)\n\};"
        def extract(contents, species):
            match = re.search(pattern.replace("{}", species), contents)
            self.assertIsNotNone(match, "Missing donor/Relic learnset: " + species)
            return re.findall(
                r"LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)", match.group(1)
            )
        originals = extract(read("src/data/pokemon/level_up_learnsets/gen_9.h"), "GreatTusk")
        relic = extract(read("src/data/pokemon/level_up_learnsets/prl_custom.h"), "Donphalanx")
        self.assertEqual(
            [entry for entry in relic if entry != ("73", "MOVE_SEISMIC_SLAM")],
            originals,
        )
        self.assertEqual(relic.count(("73", "MOVE_SEISMIC_SLAM")), 1)

    def test_runtime_hook_dispatch_and_effect_paths_remain_wired(self):
        manifest = json.loads(read("docs/prl_step10_integration_manifest.json"))
        battle = read("src/battle_util.c")
        stat = read("src/battle_stat_change.c")
        moves = read("src/battle_move_resolution.c")
        all_battle = battle + stat + moves
        for ability in ("ANCIENT_GROVE", "SOLAR_DISCIPLINE", "TIDAL_BASTION",
                        "ANCIENT_CORE", "WISHMAKER", "EDENS_CANOPY",
                        "THERMAL_WINGS", "CITADEL_SHELL"):
            self.assertIn("ABILITY_" + ability, all_battle)
        self.assertIn("prlAbilityTimer[battler] = 5", battle)
        self.assertIn("prlAbilityTimer[battler]--", battle)
        self.assertIn("HAZARDS_STEALTH_ROCK", battle)
        self.assertIn("MOVE_RELIC_WISH", moves)
        self.assertIn("GetNonDynamaxMaxHP(cv->battlerAtk) / 8", moves)
        self.assertIn("BattleScript_PRLRelicWishHeal", moves)
        self.assertIn("ABILITY_THERMAL_WINGS", moves)
        self.assertEqual(len(manifest["runtime_hooks_completed"]), 9)

if __name__ == "__main__":
    unittest.main()
