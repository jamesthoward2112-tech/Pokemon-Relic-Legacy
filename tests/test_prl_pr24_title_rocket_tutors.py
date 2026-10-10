"""PR24: verified Kanto title and Mt. Moon scene wiring.

Artwork checks are structural; final acceptance requires emulator tests.
"""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]

def source(path):
    return (ROOT / path).read_text(encoding="utf-8")

def map_file(name):
    return json.loads(source(f"data/maps/{name}_Frlg/map.json"))

def test_title_direct_artwork_and_animation():
    title = source("src/title_screen_frlg.c")
    forest = source("src/prl_intro_trial.c")
    expansion = source("src/expansion_intro.c")
    assert "PRLDrawPrompt(data[1])" in title
    assert "PRLUpdateTitleSparkles" in title
    assert "PRLUpdateCelebiJirachiTitleAnimation" not in title
    assert "CB2_InitPRLHwlScene0" in expansion
    assert "sScene0Celebi_Gfx" in forest and "sScene0Jirachi_Gfx" in forest
    assert "sAnims_Scene0Celebi" in forest and "sAnims_Scene0Jirachi" in forest


def test_fossils_do_not_launch_rocket():
    s=source("data/maps/MtMoon_B2F_Frlg/scripts.inc")
    assert "call MtMoon_B2F_EventScript_PRL_JESSIE_JAMES_MT_MOON" not in s
    assert s.count("setflag FLAG_GOT_FOSSIL_FROM_MT_MOON") == 2
    assert "MtMoon_B2F_Text_PRL_RocketIntro::" in s

def test_rocket_visible_and_obstructs_exit_before_battle():
    j=map_file("MtMoon_B1F")
    exit_warp=next(w for w in j["warp_events"] if w["dest_map"]=="MAP_ROUTE4")
    assert (exit_warp["x"],exit_warp["y"])==(45,4)
    pcs={x.get("local_id"):x for x in j["object_events"]}
    for name in ("JESSIE","JAMES","MEOWTH"):
        e=pcs[f"LOCALID_PRL_{name}_MT_MOON"]
        assert e["flag"]=="FLAG_HIDE_PRL_ROCKET_MT_MOON"
        assert e["script"]=="MtMoon_B1F_EventScript_PRL_JessieJamesEncounter"
        assert e["x"] < exit_warp["x"] && e["y"] == 2
    assert pcs["LOCALID_PRL_JESSIE_MT_MOON"]["y"] == pcs["LOCALID_PRL_JAMES_MT_MOON"]["y"]
    assert all(any(e["x"]==x and e["y"]==5 for e in j["coord_events"]) for x in range(38,47))
    sc=source("data/maps/MtMoon_B1F_Frlg/scripts.inc")
    assert "goto_if_unset FLAG_GOT_FOSSIL_FROM_MT_MOON" in sc
    assert "trainerbattle_double TRAINER_JESSIE_JAMES_MT_MOON" in sc
    assert "removeobject LOCALID_PRL_JESSIE_MT_MOON" in sc
    assert "removeobject LOCALID_PRL_JAMES_MT_MOON" in sc
    assert "removeobject LOCALID_PRL_MEOWTH_MT_MOON" in sc
    assert "goto_if_set FLAG_PRL_RECURRING_JESSIE_JAMES_MT_MOON" in sc

def test_rocket_has_joint_name_and_approved_front_gfx():
    s=source("src/data/trainers_frlg.party")
    for encounter in ("MT_MOON","HIDEOUT","SILPH","VICTORY_ROAD"):
        pos=s.index("=== TRAINER_JESSIE_JAMES_"+encounter+" ===")
        assert "Name: JESS&JAMES" in s[pos:pos+70]
    png=(ROOT/"graphics/trainers/front_pics/prl_jessie.png").read_bytes()
    assert png.startswith(bytes.fromhex("89504e470d0a1a0a"))
    assert int.from_bytes(png[16:20],"big")==64
    assert int.from_bytes(png[20:24],"big")==64
    assert png[24]==4  # palette-indexed 4bpp PNG
    # Both trainer IDs must render the approved *two-person* white-uniform donor.
    import hashlib
    james=(ROOT/"graphics/trainers/front_pics/prl_james.png").read_bytes()
    assert james == png
    assert hashlib.sha256(png).hexdigest()=="37321dc53d66139cb466971b75b9d5764f6148ea45f589617fa7bb4f316006a2"
    assert png[25]==3  # indexed colour PNG
    assert b"tRNS" in png and png[png.index(b"tRNS")+4] == 0  # GBA OBJ colour zero is transparent

def test_route4_blackbelts_teach_correct_moves():
    j=map_file("Route4")
    tutors=[e for e in j["object_events"] if e["graphics_id"]=="OBJ_EVENT_GFX_BLACK_BELT_FRLG"]
    assert len(tutors)==2
    assert {e["script"] for e in tutors}=={
        "Route4_EventScript_PRL_DrainPunchTutor",
        "Route4_EventScript_PRL_EarthPowerTutor",
    }
    sc=source("data/maps/Route4_Frlg/scripts.inc")
    assert "goto EventScript_PRL_TutorDrainPunch" in sc
    assert "goto EventScript_PRL_TutorEarthPower" in sc
    text=source("data/scripts/move_tutors_frlg.inc")
    assert "MOVE_DRAIN_PUNCH" in text and "MOVE_EARTH_POWER" in text
    assert "MtMoon_1F_EventScript_DrainPunchTutor" not in source("data/maps/MtMoon_1F_Frlg/scripts.inc")
    assert not any(e["script"]=="MtMoon_1F_EventScript_DrainPunchTutor" for e in map_file("MtMoon_1F")["object_events"])

# Run these checks under the project's unittest discovery, not just pytest.
import unittest

class PR24SpriteAndMtMoonChecks(unittest.TestCase):
    def test_title(self): test_title_direct_artwork_and_animation()
    def test_fossil(self): test_fossils_do_not_launch_rocket()
    def test_rocket_scene(self): test_rocket_visible_and_obstructs_exit_before_battle()
    def test_rocket_gfx(self): test_rocket_has_joint_name_and_approved_front_gfx()
    def test_route4_tutors(self): test_route4_blackbelts_teach_correct_moves()
