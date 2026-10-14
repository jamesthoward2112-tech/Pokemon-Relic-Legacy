"""PR74: boarding readiness must never mutate the player's Kanto save.

This first step is a deliberately non-destructive preflight. It does NOT
claim that cross-region travel or save migration is complete.
"""
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


class PRLHoennPreflight(unittest.TestCase):
    def test_pc_space_check_accounts_for_party_and_every_physical_box(self):
        body = read("src/field_specials.c").split("u16 PRLHoennCheckPCSpaceForParty(void)", 1)[1].split("\n}", 1)[0]
        for statement in ("CountPartyMons()", "gPokemonStoragePtr == NULL",
                          "box < TOTAL_BOXES_COUNT", "CountMonsInBox(box)",
                          "IN_BOX_COUNT", "return freeSlots >= partyCount;"):
            self.assertIn(statement, body)
        for destructive in ("ZeroBoxMonAt(", "ZeroMonData(", "RemoveBagItem(", "SetMoney(", "SetBoxMonAt(", "SetMonData("):
            self.assertNotIn(destructive, body)

    def test_sailor_honours_oak_ticket_and_does_not_warp(self):
        script = read("data/maps/VermilionCity_Frlg/scripts.inc")
        sailor = script.split("VermilionCity_EventScript_FerrySailor::", 1)[1].split("VermilionCity_EventScript_CheckHasMysticTicket::", 1)[0]
        self.assertIn("goto_if_set FLAG_PRL_POSTHOF_OAK_TICKET", sailor)
        self.assertIn("MSGBOX_YESNO", sailor)
        self.assertIn("specialvar VAR_RESULT, PRLHoennCheckPCSpaceForParty", sailor)
        self.assertIn("VermilionCity_EventScript_PRLHoennNoRoom", sailor)
        self.assertIn("VermilionCity_EventScript_PRLHoennBoardingDeclined", sailor)
        self.assertIn("VermilionCity_EventScript_CheckSeagallopPresent", sailor)
        for command in ("\twarp ", "\tremoveitem ", "\tsetmoney ", "\tsetflag FLAG_PRL_", "\tremoveobject "):
            self.assertNotIn(command, sailor)

    def test_save_storage_layout_stays_unchanged(self):
        storage = read("include/pokemon_storage_system.h")
        assert "TOTAL_BOXES_COUNT       14" in storage
        assert "IN_BOX_COUNT            (IN_BOX_ROWS * IN_BOX_COLUMNS)" in storage
        assert "struct BoxPokemon boxes[TOTAL_BOXES_COUNT][IN_BOX_COUNT]" in storage
        save = read("include/save.h")
        assert "SECTOR_ID_PKMN_STORAGE_END   13" in save
        assert "NUM_SECTORS_PER_SLOT         14" in save

    def test_special_registered(self):
        self.assertIn("def_special PRLHoennCheckPCSpaceForParty", read("data/specials.inc"))
        self.assertIn("u16 PRLHoennCheckPCSpaceForParty(void);", read("include/field_specials.h"))


if __name__ == "__main__":
    unittest.main()
