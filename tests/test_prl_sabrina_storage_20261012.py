"""Storage migration must preserve all old saves and new pocket slots."""
import pathlib
import re
import unittest

ROOT=pathlib.Path(__file__).resolve().parents[1]
def read(p): return (ROOT/p).read_text()

class PRLExpandedBagTests(unittest.TestCase):
    def test_new_pocket_sizes(self):
        s=read("include/constants/global.h")
        for name,n in {"BAG_ITEMS_COUNT":60,"BAG_KEYITEMS_COUNT":50,
                       "BAG_POKEBALLS_COUNT":32,"BAG_TMHM_COUNT":100,
                       "BAG_BERRIES_COUNT":56,"PC_ITEMS_COUNT":100}.items():
            self.assertRegex(s,rf"#define\s+{name}\s+{n}\b")
    def test_legacy_saveblock_positions_unchanged(self):
        s=read("include/global.h")
        block=s.split("struct LegacyBag",1)[1].split("// Expanded bag",1)[0]
        for name,n in [("items",30),("keyItems",30),("pokeBalls",16),("TMsHMs",64),("berries",46)]:
            self.assertIn(f"{name}[{n}]",block)
        save=s.split("struct SaveBlock1\n",1)[1]
        self.assertIn("struct ItemSlot pcItems[50];",save)
        self.assertIn("struct LegacyBag bag;",save)
        self.assertIn("u32 prlInventoryStorage[399];",s)
    def test_all_existing_items_migrated_encrypted_and_saved(self):
        s=read("src/item.c")
        m=s.split("void EnsurePrlInventoryInitialized(void)",1)[1].split("void SyncPrlInventoryLegacyBackup(void)",1)[0]
        b=s.split("void SyncPrlInventoryLegacyBackup(void)",1)[1].split('#include "data/pokemon/item_effects.h"',1)[0]
        for pocket in ("items","keyItems","pokeBalls","TMsHMs","berries"):
            self.assertIn(f"memcpy(inventory->bag.{pocket}, gSaveBlock1Ptr->bag.{pocket}, sizeof(gSaveBlock1Ptr->bag.{pocket}))",m)
            self.assertIn(f"memcpy(gSaveBlock1Ptr->bag.{pocket}, inventory->bag.{pocket}, sizeof(gSaveBlock1Ptr->bag.{pocket}))",b)
        self.assertIn("memcpy(inventory->pcItems, gSaveBlock1Ptr->pcItems, sizeof(gSaveBlock1Ptr->pcItems))",m)
        self.assertIn("memcpy(gSaveBlock1Ptr->pcItems, inventory->pcItems, sizeof(gSaveBlock1Ptr->pcItems))",b)
        self.assertIn("STATIC_ASSERT(sizeof(struct PrlInventory) <= sizeof(gSaveBlock3Ptr->prlInventoryStorage)",s)
        self.assertIn("STATIC_ASSERT(sizeof(struct LegacyBag) == 0x2E8",s)
    def test_all_item_pockets_use_expanded_live_storage(self):
        s=read("src/item.c")
        for p in ("items","keyItems","pokeBalls","TMsHMs","berries"):
            self.assertIn(f"itemSlots = GetPrlBag()->{p}",s)
        self.assertIn(".itemSlots = GetPrlPCItems()",s)
        pc=read("src/player_pc.c")
        self.assertNotIn("gSaveBlock1Ptr->pcItems",pc)
        self.assertIn("PC_ITEMS_COUNT * sizeof(struct ItemSlot)",pc)
        bag=read("src/item_menu.c")
        self.assertNotIn("gSaveBlock1Ptr->bag",bag)
        load=read("src/load_save.c")
        self.assertIn("memcpy(&gLoadedSaveData.bag, GetPrlBag(), sizeof(struct Bag))",load)
        self.assertIn("memcpy(GetPrlBag(), &gLoadedSaveData.bag, sizeof(struct Bag))",load)
    def test_load_migrates_before_party_and_save_mirrors_backup(self):
        s=read("src/save.c")
        assert s.index("EnsurePrlInventoryInitialized();") < s.index("CopyPartyAndObjectsFromSave();")
        assert s.index("SyncPrlInventoryLegacyBackup();") < s.index("UpdateSaveAddresses();",s.index("u8 HandleSavingData(u8 saveType)"))
