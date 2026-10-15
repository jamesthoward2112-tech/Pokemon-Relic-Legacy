"""PR93 regression: Hoenn Bag/party isolation, Remote PC and baked-in ball tiles."""
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
def read(path): return (ROOT/path).read_text(encoding="utf-8")
def script_block(source,label):
    return source.split(label+"::",1)[1].split("\n\n",1)[0]

class TestHoennEssentialsAndStarterRoom(unittest.TestCase):
    def test_birch_offers_sprite_previews_without_table_or_balls(self):
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        for label in ("PRLWaitForBirch", "PRLSetupStarterRoom",
                      "PRLHideStarterRoom", "PRLStarterComplete"):
            block=script_block(lab,"LittlerootTown_ProfessorBirchsLab_EventScript_"+label)
            self.assertIn("setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB",block)
            self.assertNotIn("WITH_TABLE",block)
        self.assertIn("map_script_2 VAR_PRL_REGION_STAGE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_PRLRemoveStarterBalls",lab)
        for name in ("RALTS", "POOCHYENA", "GEMLIN"):
            self.assertIn("showmonpic SPECIES_"+name,lab)
        complete=script_block(lab,"LittlerootTown_ProfessorBirchsLab_EventScript_PRLStarterComplete")
        for name in ("CYNDAQUIL","TOTODILE","CHIKORITA"):
            self.assertIn("setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_"+name,complete)

    def test_kanto_party_and_bag_inaccessible_until_starter_chosen(self):
        src=read("src/start_menu.c")
        menu=src.split("static void BuildNormalStartMenu(void)\n{",1)[1].split("static void BuildDebugStartMenu",1)[0]
        for stage in (1,3,4):
            self.assertIn("VarGet(VAR_PRL_REGION_STAGE) == "+str(stage),menu)
        self.assertLess(menu.index("return;"), menu.index("AddStartMenuAction(MENU_ACTION_POKEMON)"))
        self.assertLess(menu.index("return;"), menu.index("AddStartMenuAction(MENU_ACTION_BAG)"))
        self.assertIn("AddStartMenuAction(MENU_ACTION_SAVE)",menu)

    def test_remote_pc_is_actual_kept_key_item(self):
        data=read("src/data/items.h")
        item=data.split("[ITEM_POKEMON_BOX_LINK] =",1)[1].split("},",1)[0]
        self.assertIn("Remote PC",item)
        self.assertIn("ItemUseOutOfBattle_PokemonBoxLink",item)
        code=read("src/field_specials.c")
        transfer=code.split("u16 PRLHoennCommitKantoTransfer(void)",1)[1]
        for line in ("ClearBag();","AddBagItem(ITEM_EXP_SHARE, expShareQuantity);",
                     "AddBagItem(ITEM_POKEMON_BOX_LINK, 1);",
                     "FlagSet(FLAG_PRL_HOENN_ESSENTIALS_READY);",
                     "LoadPlayerBag();","SyncPrlInventoryLegacyBackup();"):
            self.assertIn(line,transfer)
        self.assertLess(transfer.index("ClearBag();"),transfer.index("AddBagItem(ITEM_POKEMON_BOX_LINK, 1);"))
        self.assertIn("FlagSet(FLAG_SYS_B_DASH);",code)

    def test_existing_hoenn_saves_get_one_time_normalization(self):
        source=read("src/field_specials.c")
        normalizer=source.split("u16 PRLHoennEnsureEssentials(void)",1)[1]
        self.assertIn("VarGet(VAR_PRL_REGION_STAGE) != 2",normalizer)
        self.assertIn("!FlagGet(FLAG_PRL_HOENN_ESSENTIALS_READY)",normalizer)
        self.assertIn("CountTotalItemQuantityInBag(ITEM_EXP_SHARE)",normalizer)
        self.assertIn("ClearBag();",normalizer)
        self.assertIn("AddBagItem(ITEM_POKEMON_BOX_LINK, 1)",normalizer)
        self.assertIn("LoadPlayerBag();",normalizer)
        town=read("data/maps/LittlerootTown/scripts.inc")
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("special PRLHoennEnsureEssentials",town)
        self.assertIn("special PRLHoennEnsureEssentials",lab)
        self.assertIn("def_special PRLHoennEnsureEssentials",read("data/specials.inc"))
        self.assertIn("u16 PRLHoennEnsureEssentials(void);",read("include/field_specials.h"))

    def test_hoenn_pc_hard_level_caps_and_move_bypass_protection(self):
        code=read("src/pokemon_storage_system.c")
        self.assertIn("static u8 PRLHoennPcLevelLimit(void)",code)
        self.assertIn("FlagGet(FLAG_SYS_GAME_CLEAR)",code)
        self.assertIn("FlagGet(FLAG_BADGE01_GET + i)",code)
        self.assertIn("return 10 + badges * 10;",code)
        self.assertIn("PRLHoennStorageLevelLocked(GetLevelFromBoxMonExp(GetBoxedMonPtr(StorageGetCurrentBox(), sCursorPosition)))",code)
        self.assertIn("PRLHoennStorageLevelLocked(GetMonData(&sStorage->movingMon, MON_DATA_LEVEL))",code)
        self.assertIn("MSG_PRL_HOENN_LEVEL_LOCK",code)

    def test_birch_explains_lock_and_aunt_is_not_mum(self):
        lab=read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        town=read("data/maps/LittlerootTown/scripts.inc")
        for phrase in ("KANTO team","safely into the PC","Lv1-10","Each BADGE adds ten levels","HOENN LEAGUE","potential","mystery"):
            self.assertIn(phrase,lab)
        self.assertIn("AUNT ROSIE: {PLAYER}",town)
        self.assertNotIn("VALKYVOIR",lab)
        self.assertNotIn("SKULBERUS",lab)
        self.assertIn("MUM sent me to greet you",town)
        self.assertNotIn("MOM: HOENN looks lovely",town)

    def test_hoenn_house_guardian_is_aunt_not_kanto_mom(self):
        import json
        for name in ("Brendans", "Mays"):
            prefix = "data/maps/LittlerootTown_" + name + "House_1F/"
            setting = json.loads(read(prefix + "map.json"))
            guardian = next(obj for obj in setting["object_events"] if obj.get("local_id") == "LOCALID_PLAYERS_HOUSE_1F_MOM")
            self.assertEqual(guardian["script"], "LittlerootTown_" + name + "House_1F_EventScript_PRLAunt")
            events = read(prefix + "scripts.inc")
            self.assertIn('AUNT ROSIE: Welcome to', events)
            self.assertIn('Your MUM is keeping your', events)
            self.assertIn("goto PlayersHouse_1F_EventScript_Mom", events)
        lab = read("data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc")
        self.assertIn("Each BADGE adds ten levels.", lab)
        self.assertIn("Hoenn BADGES unlock your", lab)

if __name__=="__main__": unittest.main()
