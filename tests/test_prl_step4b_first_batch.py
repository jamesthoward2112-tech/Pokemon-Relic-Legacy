"""Prevent unapproved first-batch Redux move learnset changes in PRL."""
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
LEVEL = ROOT / "src/data/pokemon/level_up_learnsets/prl_custom.h"
SPECIES = ROOT / "src/data/pokemon/species_info/prl_custom.h"
EXPECTED = {
    "Khang": [(58, "MOVE_BEATDOWN")],
    "Nolax": [(54, "MOVE_BEATDOWN"), (64, "MOVE_SEISMIC_SLAM")],
    "Heracurion": [(50, "MOVE_INSECT_IMPACT")],
    "Miltitan": [(62, "MOVE_SEISMIC_SLAM")],
    "Donphalanx": [(73, "MOVE_SEISMIC_SLAM")],
    "Pinsirex": [(46, "MOVE_INSECT_IMPACT")],
    "Champeon": [(48, "MOVE_INSECT_IMPACT")],
    "Lepideon": [(46, "MOVE_INSECT_IMPACT")],
    "Sphynxeon": [(60, "MOVE_SEISMIC_SLAM")],
    "Osteodian": [(60, "MOVE_SEISMIC_SLAM")],
}
FIRST_EIGHT = {
    "MOVE_SEISMIC_SLAM", "MOVE_SOIL_DRAIN", "MOVE_BRAVADO",
    "MOVE_RAGING_SOULS", "MOVE_IRON_FANGS", "MOVE_BEATDOWN",
    "MOVE_TAKE_FLIGHT", "MOVE_INSECT_IMPACT",
}

def entries(source, name):
    m = re.search(
        r"static const struct LevelUpMove s" + name +
        r"LevelUpLearnset\[\]\s*=\s*\{([\s\S]*?)\n\};", source
    )
    if not m:
        raise AssertionError("Missing table for " + name)
    return [(int(lvl), move) for lvl, move in re.findall(
        r"LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)", m.group(1)
    )]

class Step4BTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.level = LEVEL.read_text(encoding="utf-8")
        cls.species = SPECIES.read_text(encoding="utf-8")

    def test_exact_approved_levelup_assignments(self):
        for name, expected in EXPECTED.items():
            with self.subTest(species=name):
                self.assertCountEqual(
                    [e for e in entries(self.level, name) if e[1] in FIRST_EIGHT],
                    expected
                )
        all_moves = re.findall(r"LEVEL_UP_MOVE\(\s*\d+,\s*(MOVE_[A-Z0-9_]+)\)", self.level)
        self.assertEqual(sum(x in FIRST_EIGHT for x in all_moves), 11)

    def test_baby_species_remain_deferred(self):
        for name in ("Scarabub", "Mootiny"):
            self.assertFalse(any(m in FIRST_EIGHT for _, m in entries(self.level, name)))

    def test_donor_progression_exactly_preserved(self):
        for name, donor, file in (
            ("Donphalanx", "GreatTusk", "gen_9.h"),
            ("Osteodian", "Marowak", "gen_1.h"),
        ):
            original = entries((LEVEL.parent / file).read_text(encoding="utf-8"), donor)
            custom = entries(self.level, name)
            self.assertEqual([e for e in custom if e not in EXPECTED[name]], original)
            self.assertIn(".levelUpLearnset = s" + name + "LevelUpLearnset", self.species)

    def test_classic_starter_evolution_signatures_survive(self):
        for name, move in {
            "Edensaur": "MOVE_EDEN_BLOOM", "Charaxis": "MOVE_SCORCHWING",
            "Fortotoise": "MOVE_BASTION_CANNON", "Faeranium": "MOVE_ETERNAL_BLOOM",
            "Pyroclast": "MOVE_MAGMA_RIFT", "Feralodon": "MOVE_DEATH_ROLL",
            "Sceptitan": "MOVE_AMBER_BLADE", "Solaziken": "MOVE_SUN_RITE",
            "Swamplith": "MOVE_TECTONIC_TIDE",
        }.items():
            with self.subTest(species=name):
                self.assertIn((0, move), entries(self.level, name))
