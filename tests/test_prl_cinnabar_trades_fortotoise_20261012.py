"""PRL Cinnabar Lounge trade separation and Fortotoise battle-sprite alignment."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

class TestCinnabarTradeFixAndFortotoise(unittest.TestCase):
    def test_cinnabar_trades_are_distinct_and_use_safe_common_helpers(self):
        script = read("data/maps/CinnabarIsland_PokemonLab_Lounge_Frlg/scripts.inc")
        clifton = script.split("CinnabarIsland_PokemonLab_Lounge_EventScript_Clifton::", 1)[1].split(
            "CinnabarIsland_PokemonLab_Lounge_EventScript_CliftonDeclineTrade::", 1)[0]
        norma = script.split("CinnabarIsland_PokemonLab_Lounge_EventScript_Norma::", 1)[1].split(
            "CinnabarIsland_PokemonLab_Lounge_EventScript_NormaDeclineTrade::", 1)[0]
        self.assertIn("INGAME_TRADE_ELECTRODE", clifton)
        self.assertIn("FLAG_DID_ESPHERE_TRADE", clifton)
        self.assertIn("INGAME_TRADE_TANGELA", norma)
        self.assertIn("FLAG_DID_TANGENY_TRADE", norma)
        for trade in (clifton, norma):
            self.assertIn("call EventScript_GetInGameTradeSpeciesInfo", trade)
            self.assertIn("call EventScript_ChooseMonForInGameTrade", trade)
            self.assertIn("call EventScript_GetInGameTradeSpecies", trade)
            self.assertIn("call EventScript_DoInGameTrade", trade)
        self.assertNotIn("specialvar VAR_RESULT, GetInGameTradeSpeciesInfo", norma)
        self.assertNotIn("special CreateInGameTradePokemon", norma)
        table = read("src/data/trade.h")
        electrode = table.split("[INGAME_TRADE_ELECTRODE]", 1)[1].split("[INGAME_TRADE_TANGELA]", 1)[0]
        tangela = table.split("[INGAME_TRADE_TANGELA]", 1)[1].split("[INGAME_TRADE_SEEL]", 1)[0]
        self.assertIn(".species = SPECIES_ELECTRODE", electrode)
        self.assertIn(".requestedSpecies = SPECIES_RAICHU", electrode)
        self.assertIn(".species = SPECIES_TANGELA", tangela)
        self.assertIn(".requestedSpecies = SPECIES_VENONAT", tangela)

    def test_fortotoise_back_is_lowered_without_changing_other_sprites(self):
        src = read("src/data/pokemon/species_info/prl_custom.h")
        block = src.split("[SPECIES_FORTOTOISE]", 1)[1].split("[SPECIES_KHANG]", 1)[0]
        self.assertIn(".backPic = gMonBackPic_Fortotoise", block)
        self.assertIn(".backPicYOffset = 5", block)
        self.assertIn(".frontPicYOffset = 0", block)

if __name__ == "__main__":
    unittest.main()
