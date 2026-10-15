"""PRL Chansey center training: one six-Chansey EXP-only coach per center."""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CENTERS = (("VIRIDIAN","ViridianCity",7),("PEWTER","PewterCity",10),("ROUTE4","Route4",14),("CERULEAN","CeruleanCity",16),("VERMILION","VermilionCity",22),("ROUTE10","Route10",26),("LAVENDER","LavenderTown",27),("CELADON","CeladonCity",30),("SAFFRON","SaffronCity",34),("FUCHSIA","FuchsiaCity",38),("CINNABAR","CinnabarIsland",42),("INDIGO","IndigoPlateau",47),("ONE_ISLAND","OneIsland",50),("TWO_ISLAND","TwoIsland",55),("THREE_ISLAND","ThreeIsland",65),("FOUR_ISLAND","FourIsland",59),("FIVE_ISLAND","FiveIsland",60),("SIX_ISLAND","SixIsland",61),("SEVEN_ISLAND","SevenIsland",62))

class ChanseyCenterTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.parties = (ROOT / 'src/data/trainers_frlg.party').read_text()
        cls.trainer_ids = (ROOT / 'include/constants/opponents_frlg.h').read_text()
        cls.scripts = (ROOT / 'data/scripts/prl_center_chansey_training.inc').read_text()

    def test_all_19_location_parties_and_moves(self):
        for index, (key, city, starting_level) in enumerate(CENTERS):
            with self.subTest(center=key):
                label = 'TRAINER_PRL_CHANSEY_' + key
                trainer = re.search(r'^=== ' + label + r' ===\n(.*?)(?=^=== |\Z)', self.parties, re.M | re.S)
                self.assertIsNotNone(trainer, label)
                party = trainer.group(1)
                self.assertEqual(party.count('\nChansey\n'), 6)
                self.assertEqual([int(x) for x in re.findall(r'^Level: (\d+)$', party, re.M)], list(range(starting_level, starting_level + 6)))
                self.assertEqual(re.findall(r'^- (.*?)$', party, re.M), ['Memento', 'Heal Pulse'] * 6)
                self.assertIn(f'#define {label}', self.trainer_ids)
                self.assertRegex(self.trainer_ids, label + r'\s+' + str(640 + index) + r'\b')

    def test_all_19_centers_replace_only_move_deleter(self):
        for key, city, _ in CENTERS:
            with self.subTest(center=city):
                p = ROOT / 'data/maps' / (city + '_PokemonCenter_2F_Frlg') / 'map.json'
                m = json.loads(p.read_text())
                scripts = [x.get('script') for x in m['object_events']]
                self.assertNotIn('EventScript_PRL_CenterMoveDeleter', scripts)
                self.assertEqual(scripts.count('EventScript_PRL_Chansey_' + key), 1)
                self.assertIn('Common_EventScript_MoveRelearner', scripts)
                self.assertIn('EventScript_PRL_CenterDaypart', scripts)

    def test_fights_repeat_without_persistent_defeat(self):
        for key, _, _ in CENTERS:
            start = 'EventScript_PRL_Chansey_' + key + '::'
            after = 'EventScript_PRL_Chansey_' + key + '_After::'
            self.assertIn(start, self.scripts)
            self.assertIn(after, self.scripts)
            segment = self.scripts.split(start, 1)[1].split(after, 1)[0]
            self.assertIn('cleartrainerflag TRAINER_PRL_CHANSEY_' + key, segment)
            self.assertIn('trainerbattle_single TRAINER_PRL_CHANSEY_' + key, segment)
            callback = self.scripts.split(after, 1)[1].split('\n\tend', 1)[0]
            self.assertIn('cleartrainerflag TRAINER_PRL_CHANSEY_' + key, callback)

    def test_source_wiring_and_money(self):
        self.assertIn('.include "data/scripts/prl_center_chansey_training.inc"', (ROOT / 'data/event_scripts.s').read_text())
        source = (ROOT / 'src/battle_script_commands.c').read_text()
        self.assertIn('trainerId >= TRAINER_PRL_CHANSEY_VIRIDIAN && trainerId <= TRAINER_PRL_CHANSEY_SEVEN_ISLAND', source)
        self.assertIn('#define TRAINERS_COUNT_FRLG                      663', self.trainer_ids)
        self.assertIn('#define MAX_TRAINERS_COUNT_FRLG                  768', self.trainer_ids)

if __name__ == '__main__':
    unittest.main()
