"""PR98: reported live Pizza Boy issues on 15 Oct 2026.
Investigative HoennTest save is 128KiB; it must not be modified by source tests.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def read(name): return (ROOT / name).read_text(encoding="utf-8")

class HoennCriticalFindings(unittest.TestCase):
    def test_direct_withdraw_does_not_depend_on_cursor_area(self):
        code = read("src/pokemon_storage_system.c")
        fn = code.split("static void Task_WithdrawMon(u8 taskId)\n{", 1)[1].split(
            "static void Task_DepositMenu", 1)[0]
        self.assertIn("if (PRLHoennStorageLevelLocked(GetLevelFromBoxMonExp(GetBoxedMonPtr", fn)
        self.assertNotIn("if (sCursorArea == CURSOR_AREA_IN_BOX", fn)
        self.assertIn("PRLHoennStorageLevelLocked(GetMonData(&sStorage->movingMon, MON_DATA_LEVEL))", fn)
        self.assertLess(fn.index("PRLHoennStorageLevelLocked(GetLevelFromBoxMonExp(GetBoxedMonPtr"),
                        fn.index("InitMonPlaceChange(CHANGE_GRAB)"))
        self.assertLess(fn.index("PRLHoennStorageLevelLocked(GetMonData(&sStorage->movingMon"),
                        fn.index("InitMonPlaceChange(CHANGE_PLACE)"))
        self.assertIn("PRLHoennPrintLevelLock(", fn)
        self.assertIn("static void Task_PlaceMon(u8 taskId)",code)
        self.assertIn("static void Task_ShiftMon(u8 taskId)",code)

    def test_save_compatible_pre_starter_arrival_money_bank_and_rollback(self):
        src = read("src/field_specials.c")
        h = read("include/constants/flags.h")
        self.assertIn("FLAG_PRL_HOENN_FUNDS_BANKED",h)
        bank = src.split("u16 PRLHoennBankArrivalFunds(void)",1)[1].split(
            "u16 PRLHoennRestoreKantoFunds(void)",1)[0]
        self.assertIn("VAR_PRL_BANK_KANTO_MONEY_LO",bank)
        self.assertIn("VAR_PRL_BANK_KANTO_MONEY_HI",bank)
        self.assertIn("SetMoney(&gSaveBlock1Ptr->money, 0)",bank)
        self.assertIn("FlagSet(FLAG_PRL_HOENN_FUNDS_BANKED)",bank)
        restore = src.split("u16 PRLHoennRestoreKantoFunds(void)",1)[1].split(
            "// Return FALSE without mutating",1)[0]
        self.assertIn("FlagClear(FLAG_PRL_HOENN_FUNDS_BANKED)",restore)
        self.assertIn("SetMoney(&gSaveBlock1Ptr->money, money)",restore)
        commit = src.split("u16 PRLHoennCommitKantoTransfer(void)",1)[1].split(
            "u16 PRLHoennResortExistingPC(void)",1)[0]
        self.assertIn("if (!FlagGet(FLAG_PRL_HOENN_FUNDS_BANKED))",commit)
        self.assertIn("SetMoney(&gSaveBlock1Ptr->money, 3000)",commit)
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("special PRLHoennRestoreKantoFunds",lab)
        self.assertIn("special PRLHoennBankArrivalFunds",lab)
        self.assertIn("special PRLHoennBankArrivalFunds", read("data/maps/LittlerootTown/scripts.inc"))

    def test_truck_stays_and_collides_until_area_reload(self):
        mapgen = read("tools/mapjson/mapjson.cpp")
        self.assertIn('bool prlArrivalTruck = version == "firered"',mapgen)
        self.assertIn('(prlArrivalTruck ? "3" : json_to_string(obj_event, "elevation"))',mapgen)
        town = read("data/maps/LittlerootTown/scripts.inc")
        exit = town.split("LittlerootTown_EventScript_PRLFinishShuttleArrival::",1)[1].split(
            "LittlerootTown_Text_PRLShuttleArrival::",1)[0]
        self.assertNotIn("removeobject 5",exit)
        self.assertNotIn("removeobject 6",exit)
        self.assertNotIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK",exit)
        self.assertIn("special ShowMapNamePopup",exit)
        self.assertIn("LittlerootTown_EventScript_PRLHideTrucks",town)

    def test_aunt_rosie_and_inert_rival_props(self):
        town = read("data/maps/LittlerootTown/scripts.inc")
        self.assertIn('AUNT ROSIE: {PLAYER}!',town)
        self.assertIn("PRLPlaceAuntNearBrendansHouse",town)
        self.assertIn("PRLPlaceAuntNearMaysHouse",town)
        self.assertIn("MOVEMENT_TYPE_FACE_DOWN",town)
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_RIVAL",town)
        self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCH",town)
        mom = read("data/maps/PalletTown_PlayersHouse_1F_Frlg/scripts.inc")
        self.assertIn("AUNT ROSIE",mom)
        self.assertIn("bank all your KANTO money",mom)

    def test_no_post_starter_ball_resurrection(self):
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("hideobjectat LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL",lab)
        self.assertIn("hideobjectat LOCALID_BIRCHS_LAB_TOTODILE_BALL",lab)
        self.assertIn("hideobjectat LOCALID_BIRCHS_LAB_CHIKORITA_BALL",lab)
        block = lab.split("LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForJohtoStarters::",1)[1].split(
            "LittlerootTown_ProfessorBirchsLab_OnFrame:",1)[0]
        self.assertLess(block.index("goto_if_eq VAR_PRL_REGION_STAGE, 2"),
                        block.index("addobject LOCALID_BIRCHS_LAB_CYNDAQUIL_BALL"))

    def test_distinct_region_card_with_select_to_inspect_kanto(self):
        card = read("src/trainer_card.c")
        self.assertIn("sPRLKantoCardLabel",card)
        self.assertIn("sPRLHoennCardLabel",card)
        self.assertIn("JOY_NEW(SELECT_BUTTON)",card)
        self.assertIn("sData->trainerCard.version = sData->prlShowKantoCard ? VERSION_FIRE_RED : VERSION_EMERALD",card)
        self.assertIn("FLAG_PRL_BADGE_KANTO_BROCK + i",card)
        self.assertIn("SetMainCallback2(CB2_InitTrainerCard)",card)
        self.assertIn("trainerCard->version = (IS_FRLG && VarGet(VAR_PRL_REGION_STAGE) == 2)",card)

if __name__ == "__main__":
    unittest.main()
