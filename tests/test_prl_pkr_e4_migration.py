"""Regression checks for the first-run PKR Elite Four migration."""
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
EXPECTED = {
    "LORELEI": [("Dewgong", 65), ("Cloyster", 65), ("Slowbro", 66),
                ("Jynx", 66), ("Starmie", 67), ("Lapras", 69)],
    "BRUNO": [("Onix", 66), ("Primeape", 66), ("Hitmonlee", 67),
              ("Hitmonchan", 68), ("Poliwrath", 69), ("Machamp", 70)],
    "AGATHA": [("Arbok", 67), ("Golbat", 67), ("Weezing", 68),
               ("Muk", 68), ("Maroghost", 70), ("Gengar", 71)],
    "LANCE": [("Gyarados", 68), ("Seadra", 68), ("Dragonair", 69),
              ("Aerodactyl", 70), ("Charizard", 71), ("Dragonite", 72)],
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
        self.assertRegex(bruno, r"(?m)^Machamp @ Assault Vest\nLevel: 70\nIVs: [^\n]+\nAbility: No Guard$")
        self.assertRegex(agatha, r"(?m)^Gengar @ Life Orb\nLevel: 71\nIVs: [^\n]+\nAbility: Cursed Body\nShiny: Yes$")


if __name__ == "__main__":
    unittest.main()
