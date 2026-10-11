"""PRL reusable held items: post-battle restoration, not extra in-battle uses.

These source checks protect save behavior and all battle exit paths. In-game
activation, switching, battle losses and wild battles still need emulator QA.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class ReusableHeldItems(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.util = (ROOT / "src/battle_util.c").read_text()
        cls.main = (ROOT / "src/battle_main.c").read_text()
        cls.command = (ROOT / "src/battle_script_commands.c").read_text()
        cls.config = (ROOT / "include/config/battle.h").read_text()

    def test_original_held_item_recorded_for_each_party_member_at_battle_start(self):
        self.assertIn("gBattleStruct->itemLost[trainer][i].originalItem = GetMonData(", self.main)
        self.assertIn("MON_DATA_HELD_ITEM", self.main)

    def test_recovery_runs_on_common_end_battle_path(self):
        self.assertIn("TryRestoreHeldItems();", self.main)
        self.assertLess(self.main.index("TryRestoreHeldItems();"),
                        self.main.index("RecordedBattle_SetPlaybackFinished();"))
        self.assertIn("#define B_RESTORE_HELD_BATTLE_ITEMS GEN_LATEST",self.config)
        self.assertIn("if (!B_TRAINERS_KNOCK_OFF_ITEMS && B_RESTORE_HELD_BATTLE_ITEMS < GEN_9)",self.util)

    def test_consumed_berry_and_focus_sash_are_restored_when_slot_is_empty(self):
        func=self.util.split("void TryRestoreHeldItems(void)\n{",1)[1].split("\n}\n",1)[0]
        self.assertIn("enum Item originalItem =",func)
        self.assertIn("enum Item currentItem = GetMonData(",func)
        self.assertIn("if (originalItem != ITEM_NONE && currentItem == ITEM_NONE)",func)
        self.assertIn("SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM, &originalItem)",func)
        self.assertLess(func.index("if (originalItem != ITEM_NONE && currentItem == ITEM_NONE)"),
                        func.index("if (gBattleStruct->itemLost[B_TRAINER_PLAYER][i].stolen || returnNPCItems)"))
        self.assertNotIn("GetItemPocket(originalItem) != POCKET_BERRIES",func)

    def test_existing_battle_consumption_is_not_disabled(self):
        self.assertIn("gBattleMons[battler].item = ITEM_NONE;",self.command)
        self.assertIn("BtlController_EmitSetMonData(battler, B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE",self.command)
        self.assertIn("gBattleStruct->battlerState[battler].canPickupItem = TRUE;",self.command)

    def test_no_trick_overwrite_and_no_item_duplication_for_empty_start(self):
        func=self.util.split("void TryRestoreHeldItems(void)\n{",1)[1].split("\n}\n",1)[0]
        # Restore consumed originals only into empty held slots, never replace
        # a new item acquired during battle or grant an initially empty slot.
        self.assertIn("originalItem != ITEM_NONE && currentItem == ITEM_NONE",func)
        self.assertIn("continue;",func)
        self.assertIn("if (gBattleStruct->itemLost[B_TRAINER_PLAYER][i].stolen || returnNPCItems)",func)
        self.assertNotIn("AddBagItem",func)

    def test_caught_wild_mon_existing_item_behavior_is_not_modified(self):
        self.assertIn("if (lostItem != ITEM_NONE && GetItemPocket(lostItem) != POCKET_BERRIES)",self.command)
        self.assertIn("SetMonData(caughtMon, MON_DATA_HELD_ITEM, &lostItem)",self.command)

if __name__ == "__main__":
    unittest.main()
