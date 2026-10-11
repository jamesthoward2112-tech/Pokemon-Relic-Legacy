"""Dojo double-Hitmon reward, Sabrina badge gate and existing-save safety."""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(p): return (ROOT / p).read_text(encoding="utf-8")
script = read("data/maps/SaffronCity_Dojo_Frlg/scripts.inc")
flags = read("include/constants/flags_frlg.h")
map_data = json.loads(read("data/maps/SaffronCity_Dojo_Frlg/map.json"))

def block(label):
    needle = f"{label}::\n"
    assert needle in script
    tail = script.split(needle, 1)[1]
    end = re.search(r"^[A-Za-z0-9_]+::", tail, re.M)
    return tail[:end.start()] if end else tail

def test_sabrina_required_only_for_second():
    for first, other in (("lee", "chan"), ("chan", "lee")):
        part = block(f"SaffronCity_Dojo_EventScript_CheckHitmon{first}")
        assert f"goto_if_set FLAG_PRL_DOJO_HITMON{first.upper()}_CLAIMED" in part
        assert f"goto_if_unset FLAG_PRL_DOJO_HITMON{other.upper()}_CLAIMED" in part
        assert "goto_if_set FLAG_PRL_BADGE_KANTO_SABRINA" in part
        assert "SaffronCity_Dojo_EventScript_SecondHitmonLocked" in part
    gym = read("data/maps/SaffronCity_Gym_Frlg/scripts.inc")
    assert "setflag FLAG_PRL_BADGE_KANTO_SABRINA" in gym

def test_each_gift_is_remembered_and_ball_disappears():
    for mon in ("LEE", "CHAN"):
        name = f"FLAG_PRL_DOJO_HITMON{mon}_CLAIMED"
        assert re.search(rf"#define {name}\s+FLAG_0x4D[56]", flags)
        thing = next(x for x in map_data["object_events"] if
                     x["script"] == f"SaffronCity_Dojo_EventScript_Hitmon{mon.lower()}Ball")
        assert thing["flag"] == name
        assert f"setflag {name}" in script
    assert script.count("call SaffronCity_Dojo_EventScript_RecordHitmonGift") == 2
    assert "givemon VAR_TEMP_1, 25" in script

def test_koichi_announces_second_when_badge_earned():
    master = block("SaffronCity_Dojo_EventScript_MasterKoichi")
    assert master.lstrip().startswith("trainerbattle_single TRAINER_BLACK_BELT_KOICHI")
    assert "call SaffronCity_Dojo_EventScript_EnsureGiftFlags" in master
    assert "SaffronCity_Dojo_Text_SecondHitmonRequiresSabrina" in script
    assert "SaffronCity_Dojo_Text_SecondHitmonUnlocked" in script
    assert "Beat SABRINA in the GYM next door" in script

def test_legacy_save_migration_uses_caught_dex_flags():
    migrant = block("SaffronCity_Dojo_EventScript_MigrateGiftFlags")
    assert "getcaughtmon SPECIES_HITMONLEE" in migrant
    assert "getcaughtmon SPECIES_HITMONCHAN" in script
    assert "setflag FLAG_PRL_DOJO_GIFT_MIGRATED" in script
    assert "goto_if_set FLAG_GOT_HITMON_FROM_DOJO" in block(
        "SaffronCity_Dojo_EventScript_EnsureGiftFlags")
    assert "setflag FLAG_GOT_HITMON_FROM_DOJO" in script
    assert "FLAG_PRL_DOJO_GIFT_MIGRATED" in flags

def test_gifts_require_dojo_master_and_can_be_declined():
    for mon in ("lee","chan"):
        gift=block(f"SaffronCity_Dojo_EventScript_Hitmon{mon}Ball")
        offer=block(f"SaffronCity_Dojo_EventScript_OfferHitmon{mon}")
        assert "goto_if_defeated TRAINER_BLACK_BELT_KOICHI" in gift
        assert "MSGBOX_YESNO" in offer
        assert "hidemonpic" in offer
        assert "release" in offer
