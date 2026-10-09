"""Regression checks for the first-run PKR Elite Four migration."""
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
EXPECTED = {
    "LORELEI": [("Dewgong", 60), ("Cloyster", 60), ("Slowbro", 61),
                ("Jynx", 61), ("Starmie", 62), ("Lapras", 63)],
    "BRUNO": [("Primeape", 61), ("Hitmonlee", 61), ("Hitmonchan", 62),
              ("Poliwrath", 62), ("Onix", 63), ("Machamp", 64)],
    "AGATHA": [("Arbok", 62), ("Golbat", 62), ("Haunter", 63),
               ("Weezing", 63), ("Muk", 64), ("Gengar", 65)],
    "LANCE": [("Gyarados", 63), ("Seadra", 63), ("Dragonair", 64),
              ("Aerodactyl", 64), ("Charizard", 65), ("Dragonite", 66)],
}


def read_party():
    return (ROOT / "src/data/trainers_frlg.party").read_text(
        encoding="utf-8", errors="surrogateescape"
    )


def trainer_record(party, name):
    start = party.index(f"=== TRAINER_ELITE_FOUR_{name} ===")
    end = party.find("\n=== ", start + 5)
    return party[start:] if end < 0 else party[start:end]


def species_levels(record):
    return [(species.strip(), int(level)) for species, level in re.findall(
        r"^([^\n@]+?)(?: @ [^\n]+)?\nLevel: (\d+)", record, re.M
    )]


class PRLPKRFirstRunEliteFour(unittest.TestCase):
    def test_first_run_teams_have_six_approved_species_and_levels(self):
        party = read_party()
        for leader, expected in EXPECTED.items():
            with self.subTest(leader=leader):
                record = trainer_record(party, leader)
                self.assertEqual(species_levels(record), expected)
                self.assertIn("Double Battle: No", record)

    def test_bruno_ace_uses_no_guard_and_agatha_ace_is_shiny(self):
        party = read_party()
        bruno = trainer_record(party, "BRUNO")
        agatha = trainer_record(party, "AGATHA")
        self.assertRegex(bruno, r"(?m)^Machamp @ Sitrus Berry\nLevel: 64\nIVs: [^\n]+\nAbility: No Guard$")
        self.assertRegex(agatha, r"(?m)^Gengar @ Sitrus Berry\nLevel: 65\nIVs: [^\n]+\nShiny: Yes$")


if __name__ == "__main__":
    unittest.main()
