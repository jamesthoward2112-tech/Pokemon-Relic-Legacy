"""Regression coverage: PKR Scope Lens prize is buyable for 1,500 coins.
These are source assertions; manual emulator purchase/held-item QA remains required.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "data/maps/CeladonCity_GameCorner_PrizeRoom_Frlg/scripts.inc"
MENU = ROOT / "src/data/script_menu.h"

class GameCornerScopeLensTests(unittest.TestCase):
    def test_scope_lens_is_displayed_as_a_prize_at_the_agreed_price(self):
        src = MENU.read_text(encoding="utf-8")
        menu = re.search(
            r"static const struct MenuAction sMultichoiceList_GameCornerBattleItemPrizes\[\] = \{(.*?)\};",
            src, re.DOTALL,
        )
        self.assertIsNotNone(menu)
        prizes = menu.group(1)
        self.assertIn('SCOPE LENS{CLEAR_TO 80}{FONT_SMALL}1,500 COINS', prizes)
        self.assertEqual(prizes.count("{sText_NoThanks}"), 1)
        self.assertLess(prizes.index("SCOPE LENS"), prizes.index("{sText_NoThanks}"))
        self.assertEqual(prizes.count("COMPOUND_STRING("), 6)

    def test_prize_menu_selection_routes_to_live_item(self):
        s = SCRIPT.read_text(encoding="utf-8")
        self.assertIn("case 5, CeladonCity_GameCorner_PrizeRoom_EventScript_ScopeLens", s)
        self.assertIn("case 6, CeladonCity_GameCorner_PrizeRoom_EventScript_EndPrizeExchange", s)
        scope = re.search(
            r"CeladonCity_GameCorner_PrizeRoom_EventScript_ScopeLens::\n(.*?)(?:\n\n|\Z)",
            s, re.DOTALL,
        )
        self.assertIsNotNone(scope)
        self.assertIn("setvar VAR_TEMP_1, ITEM_SCOPE_LENS", scope.group(1))
        self.assertIn("setvar VAR_TEMP_2, 1500", scope.group(1))
        self.assertIn("goto CeladonCity_GameCorner_PrizeRoom_EventScript_ConfirmPrizeItem", scope.group(1))

    def test_existing_shared_purchase_guards_remain_in_place(self):
        s = SCRIPT.read_text(encoding="utf-8")
        self.assertIn("goto_if_unset FLAG_GOT_COIN_CASE", s)
        start = s.index("CeladonCity_GameCorner_PrizeRoom_EventScript_TryGivePrize::")
        end = s.index("CeladonCity_GameCorner_PrizeRoom_EventScript_BagFull::", start)
        purchase = s[start:end]
        for token in ("checkcoins VAR_RESULT", "goto_if_lt VAR_RESULT, VAR_TEMP_2",
                      "checkitemspace VAR_TEMP_1", "goto_if_eq VAR_RESULT, FALSE",
                      "removecoins VAR_TEMP_2", "giveitem VAR_TEMP_1"):
            self.assertIn(token, purchase)
        self.assertLess(purchase.index("checkcoins"), purchase.index("checkitemspace"))
        self.assertLess(purchase.index("checkitemspace"), purchase.index("removecoins"))
        self.assertLess(purchase.index("removecoins"), purchase.index("giveitem"))

if __name__ == "__main__":
    unittest.main()
