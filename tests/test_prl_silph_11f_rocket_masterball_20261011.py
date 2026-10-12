"""Silph 11F Rocket guard placement, Giovanni battle and Master Ball safety."""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MAP = json.loads((ROOT / "data/maps/SilphCo_11F_Frlg/map.json").read_text())
SCRIPT = (ROOT / "data/maps/SilphCo_11F_Frlg/scripts.inc").read_text()

def block(label):
    needle = f"{label}::" + chr(10)
    assert needle in SCRIPT, label
    remainder = SCRIPT.split(needle, 1)[1]
    next_label = re.search(r"^[A-Za-z0-9_]+::", remainder, re.M)
    return remainder[:next_label.start()] if next_label else remainder

class SilphCo11FProgressionTests(unittest.TestCase):
    def test_duo_lines_the_left_wall_facing_right(self):
        actors = {obj.get("local_id"): obj for obj in MAP["object_events"]}
        for name, position in (("JESSIE", (0, 14)), ("JAMES", (0, 15)),
                               ("MEOWTH", (0, 13))):
            actor = actors["LOCALID_PRL_" + name + "_SILPH"]
            assert (actor["x"], actor["y"]) == position
            assert actor["movement_type"] == "MOVEMENT_TYPE_FACE_RIGHT"
            assert actor["elevation"] == 3
            assert actor["flag"] == "FLAG_HIDE_PRL_ROCKET_SILPH"
            if name != "MEOWTH":
                assert actor["trainer_type"] == "TRAINER_TYPE_NORMAL"
                assert actor["trainer_sight_or_berry_tree_id"] == "4"
    
    def test_giovanni_requires_real_victory_without_premature_trigger(self):
        assert not any((e["x"], e["y"]) == (6, 12) for e in MAP["coord_events"])
        assert len(MAP["coord_events"]) == 2
        boss = block("SilphCo_11F_EventScript_BattleGiovanni")
        assert "goto_if_defeated TRAINER_JESSIE_JAMES_SILPH, SilphCo_11F_EventScript_GiovanniBattleUnlocked" in boss
        assert "SilphCo_11F_EventScript_GiovanniWaitForJessieJames" in boss
        unlocked = block("SilphCo_11F_EventScript_GiovanniBattleUnlocked")
        assert unlocked.index("setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH") < unlocked.index(
            "trainerbattle_no_intro TRAINER_BOSS_GIOVANNI_2")
        assert "call_if_eq VAR_TEMP_1, 0" in unlocked
        assert "call_if_eq VAR_TEMP_1, 1" in unlocked
        assert "setvar VAR_MAP_SCENE_SILPH_CO_11F, 1" in unlocked
        for label in ("SilphCo_11F_EventScript_GiovanniTriggerLeft", "SilphCo_11F_EventScript_GiovanniTriggerRight"):
            assert any(e["script"] == label for e in MAP["coord_events"])

    def test_master_ball_cannot_be_claimed_before_giovanni(self):
        president = block("SilphCo_11F_EventScript_President")
        assert president.index("goto_if_set FLAG_GOT_MASTER_BALL_FROM_SILPH") < president.index(
            "goto_if_eq VAR_MAP_SCENE_SILPH_CO_11F, 0, SilphCo_11F_EventScript_PresidentRocketOccupied"
        ) < president.index("giveitem_msg SilphCo_11F_Text_ObtainedMasterBallFromPresident")
        assert "setflag FLAG_GOT_MASTER_BALL_FROM_SILPH" in president
        denied = block("SilphCo_11F_EventScript_PresidentRocketOccupied")
        assert "release" in denied and "giveitem" not in denied
        secretary = block("SilphCo_11F_EventScript_Secretary")
        assert "goto_if_eq VAR_MAP_SCENE_SILPH_CO_11F, 0" in secretary
        assert "setvar VAR_MAP_SCENE_SILPH_CO_11F, 1" in block("SilphCo_11F_EventScript_GiovanniBattleUnlocked")
    
    def test_existing_duo_uses_native_trainer_detection_and_one_shared_fight(self):
        for name in ("Jessie", "James"):
            x = block(f"SilphCo_11F_EventScript_PRL_{name}")
            assert x.lstrip().startswith("trainerbattle_double TRAINER_JESSIE_JAMES_SILPH")
            assert f"localId=LOCALID_PRL_{name.upper()}_SILPH" in x
        assert "setflag FLAG_PRL_RECURRING_JESSIE_JAMES_SILPH" in block(
            "SilphCo_11F_EventScript_PRL_RocketWon")
    