"""Regression tests for the first PKR Kanto migration build."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

GYM_TEAMS = {
    "BROCK": [("Geodude", 13), ("Kabuto", 14), ("Onix", 16)],
    "MISTY": [("Goldeen", 20), ("Staryu", 21), ("Omanyte", 22), ("Starmie", 24)],
    "LT_SURGE": [("Voltorb", 27), ("Pikachu", 27), ("Electabuzz", 28), ("Magneton", 29), ("Raichu", 31)],
    "ERIKA": [("Tangela", 33), ("Parasect", 33), ("Victreebel", 34), ("Exeggutor", 35), ("Vileplume", 36)],
    "KOGA": [("Venomoth", 39), ("Golbat", 40), ("Muk", 41), ("Arbok", 41), ("Weezing", 43)],
    "SABRINA": [("Mr Mime", 44), ("Jynx", 44), ("Hypno", 45), ("Slowbro", 46), ("Alakazam", 48)],
    "BLAINE": [("Ninetales", 50), ("Rapidash", 50), ("Flareon", 51), ("Magmar", 52), ("Charizard", 54)],
    "GIOVANNI": [("Onix", 53), ("Marowak", 54), ("Omastar", 54), ("Kabutops", 55), ("Aerodactyl", 56), ("Rhydon", 58)],
}
ROUTE23_TEAM = [
    ("Vaporeon", 52), ("Jolteon", 52), ("Flareon", 55),
    ("Toxeon", 55), ("Eevee", 51), ("Eevee", 53),
]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8", errors="surrogateescape")


def party_record(party, trainer):
    return party.split(f"=== TRAINER_{trainer} ===", 1)[1].split("\n=== ", 1)[0]


def species_levels(record):
    return [(species.strip(), int(level)) for species, level in re.findall(
        r"^([^\n@]+?)(?: @ [^\n]+)?\nLevel: (\d+)", record, re.M
    )]


class PRLPKRMigration(unittest.TestCase):
    def test_route5_fire_red_has_exactly_ten_percent_wild_eevee(self):
        data = json.loads(read("src/data/wild_encounters.json"))
        group = next(g for g in data["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
        rates = group["fields"][0]["encounter_rates"]
        route5 = [e for e in group["encounters"] if e["map"] == "MAP_ROUTE5" and e["base_label"] == "sRoute5_FireRed"]
        self.assertEqual(len(route5), 1)
        slots = route5[0]["land_mons"]["mons"]
        self.assertEqual(sum(rate for rate, slot in zip(rates, slots) if slot["species"] == "SPECIES_EEVEE"), 10)
        self.assertEqual(sum(rates), 100)
        self.assertTrue({"SPECIES_MEOWTH", "SPECIES_PIDGEY", "SPECIES_ODDISH"}.issubset({s["species"] for s in slots}))
        self.assertTrue(all(set(s) == {"min_level", "max_level", "species"} for s in slots))

    def test_first_clear_kanto_gym_parties_match_approved_pkr_rosters(self):
        party = read("src/data/trainers_frlg.party")
        for leader, expected in GYM_TEAMS.items():
            with self.subTest(leader=leader):
                record = party_record(party, f"LEADER_{leader}")
                self.assertEqual(species_levels(record), expected)
                self.assertIn("Double Battle: No", record)

    def test_final_route23_twins_party_has_toxeon_and_keeps_approved_eveelutions(self):
        party = read("src/data/trainers_frlg.party")
        record = party_record(party, "STEEVE_NEEVEE_ROUTE23")
        self.assertEqual(species_levels(record), ROUTE23_TEAM)
        self.assertIn("Double Battle: Yes", record)


if __name__ == "__main__":
    unittest.main()
