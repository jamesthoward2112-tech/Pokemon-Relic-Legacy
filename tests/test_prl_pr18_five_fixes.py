"""Regression checks for the five approved PR17 playtest fixes / PR18 ROM."""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8")


def label_block(scripts, label):
    start = scripts.index(label + "::\n")
    tail = scripts[start:]
    match = re.search(r"^[A-Za-z0-9_]+::", tail[1:], re.M)
    return tail[:match.start() + 1] if match else tail


def test_immediate_r_to_run_no_intro_a_press():
    battle = source("src/battle_message.c")
    for key in (
        "sText_WildPkmnAppeared",
        "sText_TwoWildPkmnAppeared",
        "sText_LegendaryPkmnAppeared",
    ):
        assert f'static const u8 {key}[] = _("{{PAUSE 1}}")' in battle
    # The catch tutorial and trainer messages remain unaffected.
    assert "sText_WildPkmnAppearedPause" in battle
    controller = source("src/battle_controller_player.c")
    assert "JOY_NEW(R_BUTTON)" in controller
    assert "!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)" in controller
    assert "B_ACTION_RUN" in controller


def test_pallet_trainer_tips_girl_completely_removed_but_sign_remains():
    map_data = json.loads(source("data/maps/PalletTown_Frlg/map.json"))
    assert not any(e.get("local_id") == "LOCALID_PALLET_SIGN_LADY"
                   for e in map_data["object_events"])
    assert not any("SignLady" in e.get("script", "") for e in map_data["coord_events"])
    scripts = source("data/maps/PalletTown_Frlg/scripts.inc")
    assert "PalletTown_EventScript_SignLady" not in scripts
    assert "LOCALID_PALLET_SIGN_LADY" not in scripts
    assert "PalletTown_EventScript_TrainerTips::" in scripts
    assert "PalletTown_Text_PressStartToOpenMenu::" in scripts


def test_oak_gives_and_says_twenty_balls():
    scripts = source("data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc")
    assert "ITEM_POKE_BALL, 20" in scripts
    assert 'received 20 POKé BALLS.$' in scripts
    assert 'received five POKé BALLS.$' not in scripts


def test_all_three_oak_aides_separate_pokemon_and_item_rewards():
    cases = (
        ("ViridianForest", "ViridianForest_EventScript_PRLBulbasaurAide",
         "BULBASAUR", "MIRACLE_SEED"),
        ("Route3", "Route3_EventScript_PRLCharmanderAide",
         "CHARMANDER", "SHARP_BEAK"),
        ("Route4", "Route4_EventScript_PRLSquirtleAide",
         "SQUIRTLE", "METAL_COAT"),
    )
    for map_name, label, species, item in cases:
        scripts = source(f"data/maps/{map_name}_Frlg/scripts.inc")
        gift = label_block(scripts, label)
        assert f"givemon SPECIES_{species}, 10" in gift
        assert f"msgbox {label}_Text_Received" in gift
        assert f"giveitem ITEM_{item}" in gift
        assert gift.index("givemon ") < gift.index(f"msgbox {label}_Text_Received")
        assert gift.index(f"msgbox {label}_Text_Received") < gift.index("giveitem ")
        assert gift.index(f"msgbox {label}_Text_After") < gift.index("giveitem ")
        received = label_block(scripts, f"{label}_Text_Received")
        assert f"received {species}" in received
        assert "PROF. OAK's AIDE" in received
    forest = json.loads(source("data/maps/ViridianForest_Frlg/map.json"))
    aide = next(e for e in forest["object_events"]
                if e.get("script") == "ViridianForest_EventScript_PRLBulbasaurAide")
    assert aide["movement_type"] == "MOVEMENT_TYPE_FACE_DOWN"


def test_annihilape_kanto_inherits_approved_pr19_mechanics():
    families = source("src/data/pokemon/species_info/gen_1_families.h")
    learnsets = source("src/data/pokemon/level_up_learnsets/gen_9.h")
    assert "EVOLUTION({EVO_LEVEL, 20, SPECIES_PRIMEAPE})" in families
    assert ("{EVO_LEVEL, 30, SPECIES_ANNIHILAPE, "
            "CONDITIONS({IF_USED_MOVE_X_TIMES, MOVE_RAGE_FIST, 10})}") in families
    assert learnsets.count("LEVEL_UP_MOVE(30, MOVE_RAGE_FIST)") >= 2
    encounters = source("src/data/wild_encounters.json")
    assert '"map": "MAP_ROUTE22"' in encounters
    assert '"species": "SPECIES_MANKEY"' in encounters
