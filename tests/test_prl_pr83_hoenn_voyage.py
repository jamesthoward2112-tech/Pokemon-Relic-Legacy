"""PR83: enforce real Kanto → Hoenn transfer, preserve existing 128K save."""
import re
import unittest
from pathlib import Path

R = Path(__file__).resolve().parents[1]
def read(path): return (R / path).read_text(encoding="utf-8")

class PR83Journey(unittest.TestCase):
    def test_transaction_allocates_then_packs_every_mon_before_mutation(self):
        c=read("src/field_specials.c")
        fn=c.split("u16 PRLHoennCommitKantoTransfer(void)",1)[1]
        expected=[
            "PRLHoennCheckPCSpaceForParty()", "AllocZeroed(sizeof(*planned) * TOTAL_BOXES_COUNT)",
            "if (planned == NULL)", "for (box = 0; box < TOTAL_BOXES_COUNT; box++)",
            "for (slot = 0; slot < IN_BOX_COUNT; slot++)", "GetBoxedMonPtr(box, slot)",
            "for (partySlot = 0; partySlot < PARTY_SIZE; partySlot++)",
            "gParties[B_TRAINER_PLAYER][partySlot]", "planned[destination][used[destination] - 1] = *mon",
            "planned[destination][used[destination] - 1] = mon->box",
            "PRLHoennSortEachBoxAscending(planned, used)"
        ]
        for phrase in expected: self.assertIn(phrase,fn)
        self.assertLess(fn.index("PRLHoennSortEachBoxAscending(planned, used)"), fn.index("SetBoxMonAt(box, slot, &planned[box][slot])"))
        self.assertLess(fn.index("planned = AllocZeroed"),fn.index("SetBoxMonAt(box, slot, &planned[box][slot])"))
        self.assertEqual(fn.count("Free(planned);"),3)
    def test_cash_bag_and_badges_are_save_compatible(self):
        c=read("src/field_specials.c")
        for phrase in ["GetMoney(&gSaveBlock1Ptr->money)", "VarSet(VAR_PRL_BANK_KANTO_MONEY_LO",
                       "VarSet(VAR_PRL_BANK_KANTO_MONEY_HI", "SetMoney(&gSaveBlock1Ptr->money, 3000)",
                       "ClearBag()", "AddBagItem(ITEM_EXP_SHARE, expShareQuantity)",
                       "SyncPrlInventoryLegacyBackup()", "FlagSet(FLAG_PRL_KANTO_CHAMPION)",
                       "FlagClear(FLAG_BADGE01_GET + box)", "FlagClear(FLAG_SYS_GAME_CLEAR)"]:
            self.assertIn(phrase,c)
        v=read("include/constants/vars.h")
        for target in ("VAR_PRL_REGION_STAGE", "VAR_PRL_BANK_KANTO_MONEY_LO",
                       "VAR_PRL_BANK_KANTO_MONEY_HI", "VAR_PRL_HOENN_STARTER"):
            self.assertIn(target,v)
        self.assertIn("#define TOTAL_BOXES_COUNT       14",read("include/pokemon_storage_system.h"))
    def test_arrival_starter_once_and_uncle_frank(self):
        sc=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("goto_if_eq VAR_PRL_REGION_STAGE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_PRLHoennArrival", sc)
        for species in ("RALTS", "POOCHYENA", "GEMLIN"):
            self.assertIn("starterSpecies = SPECIES_"+species+";", read("src/field_specials.c"))
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 4", sc)
        for phrase in ("setvar VAR_PRL_REGION_STAGE, 2", "setflag FLAG_RESCUED_BIRCH",
                       "uncle FRANK", "A strange RESONANCE"):
            self.assertIn(phrase,sc)
    def test_sailor_full_confirmation_and_failure_guard(self):
        sc=read("data/maps/VermilionCity_Frlg/scripts.inc")
        point=sc.index("VermilionCity_EventScript_PRLHoennBoardingPreflight::")
        part=sc[point:sc.index("VermilionCity_EventScript_PRLHoennNoRoom::",point)]
        self.assertIn("MSGBOX_YESNO",part)
        self.assertIn("setvar VAR_PRL_REGION_STAGE, 1",part)
        self.assertNotIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer",part)
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("specialvar VAR_RESULT, PRLHoennCommitKantoTransfer",lab)
        self.assertIn("warp MAP_VERMILION_CITY, 24, 32",lab)
        self.assertIn("S.S. LEGACY",sc)
        self.assertIn("CreateRandomMon(&gParties[B_TRAINER_PLAYER][0], starterSpecies, 5);", read("src/field_specials.c"))
    def test_no_save_struct_relayout(self):
        storage=read("include/pokemon_storage_system.h")
        self.assertIn("struct BoxPokemon boxes[TOTAL_BOXES_COUNT][IN_BOX_COUNT]",storage)
        self.assertIn("#define TOTAL_BOXES_COUNT       14",storage)
if __name__=="__main__": unittest.main()
