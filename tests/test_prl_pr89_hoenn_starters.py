"""PR89 regression: one Hoenn choice through visible balls, not Emerald Johto gifts."""
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class PR89HoennStarterSelection(unittest.TestCase):
    def test_birch_offers_three_individual_sprite_previews(self):
        script = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        for species in ("RALTS", "POOCHYENA", "GEMLIN"):
            self.assertIn("showmonpic SPECIES_" + species, script)
            self.assertIn("PRLOffer" + species.title().capitalize(), script)
        self.assertIn("LittlerootTown_ProfessorBirchsLab_Text_PRLOfferPartner::", script)
        self.assertNotIn("EventScript_PRLPlaceStarterBalls::", script)
        self.assertNotIn("PRLWantSableye", script)

    def test_one_time_transfer_and_old_gifts_blocked(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        c = read("src/field_specials.c")
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 4", lab)
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 4", c)
        self.assertIn("starterSpecies = SPECIES_GEMLIN;", c)
        self.assertIn("VarSet(VAR_PRL_REGION_STAGE, 2);", c)
        for old, new in (("Cyndaquil","Ralts"),("Totodile","Poochyena"),("Chikorita","Gemlin")):
            event = lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_" + old + "::", 1)[1].split("\n\n",1)[0]
            self.assertIn("VAR_PRL_REGION_STAGE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_PRL" + new + "Ball", event)
            self.assertIn("VAR_PRL_REGION_STAGE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_PRLAlreadyChoseStarter", event)
        for flag in ("CYNDAQUIL", "TOTODILE", "CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_" + flag, lab)
        self.assertIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer", lab)

    def test_ship_arrival_hides_trucks_without_changing_map_or_save(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn("LittlerootTown_EventScript_PRLHideTrucks::", town)
        self.assertIn("FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK", town)
        self.assertIn("FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_TRUCK", town)
        self.assertIn("removeobject 5", town)
        self.assertIn("removeobject 6", town)

if __name__ == "__main__":
    unittest.main()
