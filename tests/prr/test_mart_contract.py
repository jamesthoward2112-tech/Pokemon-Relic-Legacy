from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[2]


KANTO_MART_SCRIPTS = (
    "data/maps/ViridianCity_Mart_Frlg/scripts.inc",
    "data/maps/PewterCity_Mart_Frlg/scripts.inc",
    "data/maps/CeruleanCity_Mart_Frlg/scripts.inc",
    "data/maps/VermilionCity_Mart_Frlg/scripts.inc",
    "data/maps/LavenderTown_Mart_Frlg/scripts.inc",
    "data/maps/FuchsiaCity_Mart_Frlg/scripts.inc",
    "data/maps/SaffronCity_Mart_Frlg/scripts.inc",
    "data/maps/CinnabarIsland_Mart_Frlg/scripts.inc",
    "data/maps/IndigoPlateau_PokemonCenter_1F_Frlg/scripts.inc",
)


class MartContractTests(unittest.TestCase):
    def test_ability_capsule_costs_exactly_1000(self):
        items = (ROOT / "src/data/items.h").read_text(encoding="utf-8")
        block = re.search(
            r"\[ITEM_ABILITY_CAPSULE\]\s*=\s*\{(?P<body>.*?)\n\s*\},",
            items,
            re.DOTALL,
        )
        self.assertIsNotNone(block)
        self.assertRegex(block.group("body"), r"\.price\s*=\s*1000,")
        self.assertNotIn("#if I_PRICE", block.group("body"))

    def test_every_kanto_poke_mart_sells_ability_capsules(self):
        missing = []
        for relative in KANTO_MART_SCRIPTS:
            script = (ROOT / relative).read_text(encoding="utf-8")
            if "ITEM_ABILITY_CAPSULE" not in script:
                missing.append(relative)

        self.assertEqual([], missing, f"missing Ability Capsule: {missing}")


if __name__ == "__main__":
    unittest.main()
