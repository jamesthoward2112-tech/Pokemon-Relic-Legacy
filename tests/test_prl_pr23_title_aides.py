"""Retain PR23 aide positions and replace title icons with Issue 26's forest intro."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def load(path):
    return (ROOT / path).read_text(encoding="utf-8")


def test_title_keeps_artwork_and_animates_both_mythicals():
    # Issue 26 supersedes title-screen icon animation with full forest intro.
    title = load("src/title_screen_frlg.c")
    expansion = load("src/expansion_intro.c")
    forest = load("src/prl_intro_trial.c")
    build = load(".github/workflows/build.yml")
    assert 'graphics/title_screen_prl/prl_option_b_title_mode3.bin' in title
    assert "CpuCopy16(sPRLTitleMode3" in title
    assert "PRLUpdateTitleSparkles" in title
    assert "PRLDrawPrompt(TRUE)" in title
    assert "JOY_NEW(A_BUTTON | START_BUTTON)" in title
    assert "PRLDrawPrompt(data[1])" in title
    assert "PRLUpdateCelebiJirachiTitleAnimation" not in title
    assert "SetMainCallback2(CB2_InitPRLHwlScene0)" in expansion
    assert "void CB2_InitPRLHwlScene0(void)" in forest
    assert 'graphics/intro/prl_hwl/celebi.png' in forest
    assert 'graphics/intro/prl_hwl/jirachi.png' in forest
    assert "TAG_SCENE0_CELEBI" in forest
    assert "TAG_SCENE0_JIRACHI" in forest
    assert "Restore SHA-pinned original Celebi/Jirachi forest artwork" in build
    assert "python3 tools/prl/prepare_hwl_scene0.py" in build


def test_charmander_aide_is_stationary_on_route_3():
    scene = json.loads(load("data/maps/Route3_Frlg/map.json"))
    aide = next(o for o in scene["object_events"]
                if o["script"] == "Route3_EventScript_PRLCharmanderAide")
    assert aide["movement_type"] == "MOVEMENT_TYPE_FACE_DOWN"
    assert (aide["movement_range_x"], aide["movement_range_y"]) == (0, 0)
    mon = next(o for o in scene["object_events"]
               if o.get("local_id") == "LOCALID_PRL_CHARMANDER")
    assert (mon["x"], mon["y"]) == (aide["x"] + 1, aide["y"])


def test_squirtle_aide_is_stationary_at_eastern_mt_moon_exit():
    scene = json.loads(load("data/maps/Route4_Frlg/map.json"))
    # Warp 1 is the eastern exit from Mt Moon B1F. The west-side
    # Pokemon Centre/entrance appears at x=12 and x=19 respectively.
    eastern_exit = next(w for w in scene["warp_events"]
                        if w["dest_map"] == "MAP_MT_MOON_B1F")
    assert (eastern_exit["x"], eastern_exit["y"]) == (32, 5)
    aide = next(o for o in scene["object_events"]
                if o["script"] == "Route4_EventScript_PRLSquirtleAide")
    mon = next(o for o in scene["object_events"]
               if o.get("local_id") == "LOCALID_PRL_SQUIRTLE")
    assert (aide["x"], aide["y"]) == (35, 8)
    assert (mon["x"], mon["y"]) == (36, 8)
    assert aide["movement_type"] == "MOVEMENT_TYPE_FACE_DOWN"
    assert aide["movement_range_x"] == aide["movement_range_y"] == 0
    assert mon["flag"] == "FLAG_PRL_GOT_SQUIRTLE_GIFT"


def test_aides_keep_approved_pokemon_and_item_rewards():
    data = (("Route3", "CHARMANDER", "SHARP_BEAK"),
            ("Route4", "SQUIRTLE", "METAL_COAT"))
    for route, species, item in data:
        s = load(f"data/maps/{route}_Frlg/scripts.inc")
        assert f"givemon SPECIES_{species}, 10" in s
        assert f"giveitem ITEM_{item}" in s
        assert f"LOCALID_PRL_{species}" in s
