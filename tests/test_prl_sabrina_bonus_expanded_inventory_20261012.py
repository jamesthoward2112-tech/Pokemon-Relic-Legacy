"""Regression coverage for Sabrina's pending bonus and non-destructive bag expansion."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def script(label, data):
    start = data.index(label + "::") + len(label) + 2
    rest = data[start:]
    next_label = re.search(r"(?m)^[A-Za-z0-9_]+::", rest)
    return rest[:next_label.start()] if next_label else rest


class SabrinaBonusTests(unittest.TestCase):
    def test_unclaimed_bonus_precedes_rematch_and_does_not_start_another_battle(self):
        gym = read("data/maps/SaffronCity_Gym_Frlg/scripts.inc")
        main = script("SaffronCity_Gym_EventScript_Sabrina", gym)
        self.assertLess(
            main.index("goto_if_unset FLAG_PRL_GYM_KANTO_SABRINA_EXTRA, SaffronCity_Gym_EventScript_ClaimPendingBonus"),
            main.index("specialvar VAR_RESULT, ShouldTryRematchBattle"),
        )
        pending = script("SaffronCity_Gym_EventScript_ClaimPendingBonus", gym)
        self.assertIn("call_if_unset FLAG_PRL_GYM_KANTO_SABRINA_EXTRA, PRL_EventScript_GymExtra_KANTO_SABRINA", pending)
        self.assertNotIn("trainerbattle_rematch", pending)
        self.assertIn("release", pending)
        self.assertIn("end", pending)
        self.assertIn("goto_if_unset FLAG_GOT_TM04_FROM_SABRINA, SaffronCity_Gym_EventScript_GiveTM04", main)
        self.assertIn("setflag FLAG_GOT_TM04_FROM_SABRINA", gym)

    def test_bonus_only_marks_claimed_after_item_and_cash(self):
        rewards = read("data/scripts/prl_gym_rewards.inc")
        gift = script("PRL_EventScript_GymExtra_KANTO_SABRINA", rewards)
        tokens = [
            "goto_if_set FLAG_PRL_GYM_KANTO_SABRINA_EXTRA",
            "checkitemspace ITEM_TWISTED_SPOON",
            "giveitem ITEM_TWISTED_SPOON",
            "goto_if_eq VAR_RESULT, FALSE, PRL_EventScript_GymExtraNoRoom",
            "addmoney 10000",
            "setflag FLAG_PRL_GYM_KANTO_SABRINA_EXTRA",
        ]
        positions = [gift.index(token) for token in tokens]
        self.assertEqual(positions, sorted(positions))
        self.assertIn("Please free a", rewards)


class SaveSafeExpandedInventoryTests(unittest.TestCase):
    def test_capacities_larger_in_every_pocket(self):
        src = read("include/constants/global.h")
        for k, v in {
            "BAG_ITEMS_COUNT": 60, "BAG_KEYITEMS_COUNT": 50,
            "BAG_POKEBALLS_COUNT": 32, "BAG_TMHM_COUNT": 100,
            "BAG_BERRIES_COUNT": 56, "PC_ITEMS_COUNT": 100,
        }.items():
            self.assertRegex(src, rf"(?m)^#define {k} {v}$")

    def test_legacy_saveblock1_layout_remains_fixed(self):
        h = read("include/global.h")
        old = h.split("struct LegacyBag", 1)[1].split("struct Bag", 1)[0]
        for f, n in (("items", 30), ("keyItems", 30), ("pokeBalls", 16),
                     ("TMsHMs", 64), ("berries", 46)):
            self.assertIn(f"struct ItemSlot {f}[{n}]", old)
        sav = h.split("struct SaveBlock1", 1)[1]
        self.assertIn("struct ItemSlot pcItems[50]", sav)
        self.assertIn("struct LegacyBag bag", sav)
        self.assertIn("u32 prlInventoryStorage[399]", h)
        self.assertIn("struct ItemSlot items[BAG_ITEMS_COUNT]", h)

    def test_old_saves_migrate_and_new_saves_initialize(self):
        item = read("src/item.c")
        save = read("src/save.c")
        new = read("src/new_game.c")
        self.assertIn("STATIC_ASSERT(sizeof(struct LegacyBag) == 0x2E8", item)
        self.assertIn("STATIC_ASSERT(sizeof(struct PrlInventory) <=", item)
        self.assertIn("memcpy(inventory->bag.items, gSaveBlock1Ptr->bag.items", item)
        self.assertIn("memcpy(inventory->pcItems, gSaveBlock1Ptr->pcItems", item)
        self.assertIn("if (status == SAVE_STATUS_OK)\n            EnsurePrlInventoryInitialized();", save)
        self.assertIn("SyncPrlInventoryLegacyBackup();", save)
        self.assertLess(new.index("ClearSav3();"), new.index("ClearBag();"))
        self.assertLess(new.index("ClearBag();"), new.index("NewGameInitPCItems();"))
        self.assertIn("GetPrlInventory()->magic = PRL_INVENTORY_MAGIC", item)

    def test_all_bag_storage_redirects_follow_expanded_save(self):
        item = read("src/item.c")
        for f in ("items", "keyItems", "pokeBalls", "TMsHMs", "berries"):
            self.assertIn("itemSlots = GetPrlBag()->" + f, item)
        self.assertIn(".itemSlots = GetPrlPCItems()", item)
        self.assertIn("memcpy(&gLoadedSaveData.bag, &(*GetPrlBag()), sizeof(struct Bag));", read("src/load_save.c"))
        pc = read("src/player_pc.c")
        self.assertNotIn("gSaveBlock1Ptr->pcItems", pc)
        self.assertIn("sizeof(struct ItemSlot) * PC_ITEMS_COUNT", pc)
        self.assertNotIn("sizeof(GetPrlPCItems())", pc)
        bag = read("src/item_menu.c")
        self.assertNotIn("gSaveBlock1Ptr->bag.", bag)
        self.assertIn("GetPrlBag()->items", bag)
        self.assertIn("GetPrlBag()->pokeBalls", bag)


if __name__ == "__main__":
    unittest.main()
