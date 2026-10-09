import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def block(source, label):
    match = re.search(rf"^{re.escape(label)}::\\n", source, re.MULTILINE)
    assert match, f"missing script label: {label}"
    next_label = re.search(r"^[A-Za-z0-9_]+::\\n", source[match.end():], re.MULTILINE)
    end = match.end() + next_label.start() if next_label else len(source)
    return source[match.start():end]


MOVE_TUTORS = {
    "DrainPunch": "MOVE_DRAIN_PUNCH",
    "EarthPower": "MOVE_EARTH_POWER",
    "PowerUpPunch": "MOVE_POWER_UP_PUNCH",
    "IcePunch": "MOVE_ICE_PUNCH",
    "BlazeKick": "MOVE_BLAZE_KICK",
    "HighJumpKick": "MOVE_HIGH_JUMP_KICK",
    "HeatWave": "MOVE_HEAT_WAVE",
}


def test_all_seven_moves_use_repeatable_eligible_party_selection():
    tutors = read("data/scripts/move_tutors_frlg.inc")
    for tutor, move in MOVE_TUTORS.items():
        script = block(tutors, f"EventScript_PRL_Tutor{tutor}")
        assert "MSGBOX_YESNO" in script
        assert "EventScript_CanOnlyBeLearnedOnce" not in script
        assert "FLAG_" not in script
    choose = block(tutors, "EventScript_PRL_TutorChooseMon")
    assert "call EventScript_ChooseMoveTutorMon" in choose
    assert "setvar VAR_0x8005, MOVE_DRAIN_PUNCH" in tutors
    for move in MOVE_TUTORS.values():
        assert f"setvar VAR_0x8005, {move}" in tutors


def test_moon_and_rock_tunnel_objects_are_wired_to_their_tutors():
    locations = [
        ("data/maps/MtMoon_1F_Frlg/map.json",
         "data/maps/MtMoon_1F_Frlg/scripts.inc",
         "MtMoon_1F_EventScript_DrainPunchTutor",
         "EventScript_PRL_TutorDrainPunch"),
        ("data/maps/RockTunnel_B1F_Frlg/map.json",
         "data/maps/RockTunnel_B1F_Frlg/scripts.inc",
         "RockTunnel_B1F_EventScript_EarthPowerTutor",
         "EventScript_PRL_TutorEarthPower"),
    ]
    for map_path, scripts_path, object_script, tutor_script in locations:
        map_data = json.loads(read(map_path))
        assert any(obj["script"] == object_script
                   for obj in map_data["object_events"])
        assert object_script in read(scripts_path)
        assert tutor_script in read(scripts_path)


def test_dojo_tutors_require_their_own_black_belt_defeat():
    scripts = read("data/maps/SaffronCity_Dojo_Frlg/scripts.inc")
    assignments = {
        "Hitoshi": ("TRAINER_BLACK_BELT_HITOSHI", "PowerUpPunch"),
        "Hideki": ("TRAINER_BLACK_BELT_HIDEKI", "IcePunch"),
        "Aaron": ("TRAINER_BLACK_BELT_AARON", "BlazeKick"),
        "Mike": ("TRAINER_BLACK_BELT_MIKE", "HighJumpKick"),
    }
    for trainer_name, (trainer, tutor) in assignments.items():
        script = block(scripts, f"SaffronCity_Dojo_EventScript_{trainer_name}")
        check = f"goto_if_defeated {trainer}, SaffronCity_Dojo_EventScript_{tutor}"
        assert check in script
        assert script.index(check) < script.index(f"trainerbattle_single {trainer}")
        unlock = block(scripts, f"SaffronCity_Dojo_EventScript_{tutor}")
        assert f"goto EventScript_PRL_Tutor{tutor}" in unlock

    koichi = block(scripts, "SaffronCity_Dojo_EventScript_MasterKoichi")
    assert "FLAG_GOT_HITMON_FROM_DOJO" in koichi


def test_cinnabar_scientist_keeps_research_dialogue_and_offers_heat_wave():
    script = block(
        read("data/maps/CinnabarIsland_PokemonLab_ResearchRoom_Frlg/scripts.inc"),
        "CinnabarIsland_PokemonLab_ResearchRoom_EventScript_Scientist",
    )
    assert "CinnabarIsland_PokemonLab_ResearchRoom_Text_EeveeCanEvolveIntroThreeMons" in script
    assert "goto EventScript_PRL_TutorHeatWave" in script


def test_khangs_internal_kangaskhan_learnset_has_approved_punches():
    learnables = json.loads(read("src/data/pokemon/all_learnables.json"))
    kangaskhan = learnables["KANGASKHAN"]
    assert "MOVE_DRAIN_PUNCH" in kangaskhan
    assert "MOVE_POWER_UP_PUNCH" in kangaskhan


def test_fuchsia_substitute_remains_repeatable():
    script = block(read("data/scripts/move_tutors_frlg.inc"),
                   "FuchsiaCity_EventScript_SubstituteTutor")
    assert "call EventScript_ChooseMoveTutorMon" in script
    assert "FLAG_TUTOR_SUBSTITUTE" not in script
    assert "EventScript_CanOnlyBeLearnedOnce" not in script
