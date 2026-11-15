import csv
from collections import Counter
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "docs/prr/roster/PRR_384_ROSTER_SEED.csv"

EXPECTED_COUNTS = {
    "original_kanto": 151,
    "official_kanto_family_extension": 37,
    "pru_alpha5_kanto_custom": 26,
    "relic_donor_candidate_reserved": 70,
    "regional_form_reserved": 38,
    "redux_alternate_reserved": 41,
    "other_custom_evolution_reserved": 14,
    "other_special_form_reserved": 7,
}


class RosterSeedTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        with MANIFEST.open(newline="", encoding="utf-8") as handle:
            cls.rows = list(csv.DictReader(handle))

    def test_manifest_has_exactly_384_contiguous_slots(self):
        self.assertEqual(384, len(self.rows))
        self.assertEqual(list(range(1, 385)), [int(row["slot"]) for row in self.rows])

    def test_category_counts_match_approved_breakdown(self):
        self.assertEqual(EXPECTED_COUNTS, Counter(row["category"] for row in self.rows))

    def test_approved_rows_are_complete_and_unique(self):
        approved = [row for row in self.rows if row["status"].startswith("approved")]
        self.assertEqual(214, len(approved))
        self.assertTrue(all(row["display_name"] and row["source_constant"] for row in approved))
        constants = [row["source_constant"] for row in approved]
        self.assertEqual(len(constants), len(set(constants)))

    def test_reserved_rows_are_explicit_not_silent_species_choices(self):
        reserved = [row for row in self.rows if row["status"] == "reserved_requires_audit"]
        self.assertEqual(170, len(reserved))
        self.assertTrue(all(row["entry_key"] for row in reserved))

    def test_official_constants_exist_in_selected_engine(self):
        header = (ROOT / "include/constants/species.h").read_text(encoding="utf-8")
        engine_constants = set(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", header))
        missing = [
            row["source_constant"]
            for row in self.rows
            if row["status"] == "approved_official" and row["source_constant"] not in engine_constants
        ]
        self.assertEqual([], missing)

    def test_manifest_contains_no_obsolete_444_target(self):
        self.assertNotIn("444", MANIFEST.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
