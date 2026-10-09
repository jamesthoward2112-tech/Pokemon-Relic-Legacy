import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def trainer_block(source, trainer):
    match = re.search(rf"^=== {re.escape(trainer)} ===\n(.*?)(?=^=== TRAINER_|\Z)", source, re.M | re.S)
    assert match, f"missing trainer record {trainer}"
    return match.group(1)


def script_block(source, label):
    match = re.search(rf"^{re.escape(label)}::\n", source, re.M)
    assert match, f"missing script label {label}"
    next_label = re.search(r"^[A-Za-z0-9_]+::\n", source[match.end():], re.M)
    end = match.end() + next_label.start() if next_label else len(source)
    return source[match.start():end]


def species_levels(block):
    return [(name, int(level)) for name, level in re.findall(r"^([^\n]+)\nLevel: (\d+)", block, re.M)]


def test_prl_starter_selection_and_rival_dispatch_cover_each_starter():
    starter = read("src/starter_choose.c")
    selection = re.search(r"static const u16 sStarterMon\[STARTER_MON_COUNT\] =\s*\{([^}]+)\}", starter, re.S)
    assert selection, "starter species array must remain explicit"
    assert re.findall(r"SPECIES_[A-Z0-9_]+", selection.group(1)) == [
        "SPECIES_PIKACHU", "SPECIES_NIDORAN_M", "SPECIES_GROWLITHE"
    ]

    encounters = [
        "PalletTown_ProfessorOaksLab_Frlg",
        "Route22_Frlg",
        "CeruleanCity_Frlg",
        "SSAnne_2F_Corridor_Frlg",
        "PokemonTower_2F_Frlg",
        "SilphCo_7F_Frlg",
        "PokemonLeague_ChampionsRoom_Frlg",
    ]
    for encounter in encounters:
        scripts = read(f"data/maps/{encounter}/scripts.inc")
        for starter_value, rival_branch in ((0, "Squirtle"), (1, "Bulbasaur"), (2, "Charmander")):
            assert re.search(
                rf"(?:call|goto)_if_eq VAR_STARTER_MON, {starter_value}, [A-Za-z0-9_]*{rival_branch}",
                scripts,
            ), f"{encounter} misroutes starter value {starter_value}"



def test_oaks_lab_physical_balls_save_correct_starter_and_rival_choice():
    """The ball's visible Pokémon, saved selection and early rival must agree."""
    lab = read("data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc")
    cases = {
        # Actual ball script: (saved player index, player, rival, rival ball).
        "BulbasaurBall": (1, "NIDORAN_M", "GROWLITHE", "SQUIRTLE"),
        "SquirtleBall": (2, "GROWLITHE", "PIKACHU", "CHARMANDER"),
        "CharmanderBall": (0, "PIKACHU", "NIDORAN_M", "BULBASAUR"),
    }
    for ball, (index, player, rival, rival_ball) in cases.items():
        block = script_block(lab, f"PalletTown_ProfessorOaksLab_EventScript_{ball}")
        assert f"setvar PLAYER_STARTER_NUM, {index}" in block
        assert f"setvar PLAYER_STARTER_SPECIES, SPECIES_{player}" in block
        assert f"setvar RIVAL_STARTER_SPECIES, SPECIES_{rival}" in block
        assert f"setvar RIVAL_STARTER_ID, LOCALID_{rival_ball}_BALL" in block

    rival_picks = script_block(lab, "PalletTown_ProfessorOaksLab_EventScript_RivalPicksStarter")
    for index, rival_ball in ((0, "Bulbasaur"), (1, "Squirtle"), (2, "Charmander")):
        assert (
            f"goto_if_eq PLAYER_STARTER_NUM, {index}, "
            f"PalletTown_ProfessorOaksLab_EventScript_RivalWalksTo{rival_ball}"
        ) in rival_picks

    early_party = read("src/data/trainers_frlg.party")
    for suffix, expected_species in (
        ("SQUIRTLE", "Nidoran M"),
        ("BULBASAUR", "Growlithe"),
        ("CHARMANDER", "Pikachu"),
    ):
        block = trainer_block(early_party, f"TRAINER_RIVAL_OAKS_LAB_{suffix}")
        assert [species for species, _ in species_levels(block)] == [expected_species]

    # The starter selection must be persisted before any rival battle begins.
    chose_starter = script_block(lab, "PalletTown_ProfessorOaksLab_EventScript_ChoseStarter")
    assert "copyvar VAR_STARTER_MON, PLAYER_STARTER_NUM" in chose_starter


def test_late_route22_has_six_scaled_mon_and_only_the_two_unselected_starter_lines():
    party = read("src/data/trainers_frlg.party")
    expected = {
        "TRAINER_RIVAL_ROUTE22_LATE_SQUIRTLE": {"Nidoking", "Arcanine"},
        "TRAINER_RIVAL_ROUTE22_LATE_BULBASAUR": {"Raichu", "Arcanine"},
        "TRAINER_RIVAL_ROUTE22_LATE_CHARMANDER": {"Raichu", "Nidoking"},
    }
    for trainer, starter_lines in expected.items():
        block = trainer_block(party, trainer)
        entries = species_levels(block)
        assert len(entries) == 6
        assert all(56 <= level <= 60 for _, level in entries)
        species = {name for name, _ in entries}
        assert starter_lines <= species
        if trainer.endswith("_SQUIRTLE"):
            assert not ({"Pikachu", "Raichu"} & species)
        elif trainer.endswith("_BULBASAUR"):
            assert not ({"Nidoran M", "Nidorino", "Nidoking"} & species)
        else:
            assert not ({"Growlithe", "Arcanine"} & species)


def test_first_champion_is_six_vs_six_double_and_protects_one_usable_pokemon():
    scripts = read("data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc")
    for starter_value in range(3):
        assert f"call_if_eq VAR_STARTER_MON, {starter_value}" in scripts
    assert "trainerbattle_double " in scripts
    assert "PokemonLeague_ChampionsRoom_Text_NotEnoughMons" in scripts
    for trainer in ("TRAINER_CHAMPION_FIRST_SQUIRTLE", "TRAINER_CHAMPION_FIRST_BULBASAUR",
                    "TRAINER_CHAMPION_FIRST_CHARMANDER"):
        event = trainer.split("_")[-1].title()
        block = script_block(scripts, f"PokemonLeague_ChampionsRoom_EventScript_Battle{event}")
        assert f"goto_if_defeated {trainer}" in block
        assert "setvar VAR_TEMP_1, 1" in block and "releaseall" in block and "end" in block
    assert "PokemonLeague_ChampionsRoom_OnTransition" in scripts
    assert "PokemonLeague_ChampionsRoom_EventScript_ResetFirstChampionScene" in scripts

    party = read("src/data/trainers_frlg.party")
    for trainer in (
        "TRAINER_CHAMPION_FIRST_SQUIRTLE",
        "TRAINER_CHAMPION_FIRST_BULBASAUR",
        "TRAINER_CHAMPION_FIRST_CHARMANDER",
    ):
        block = trainer_block(party, trainer)
        assert "Double Battle: Yes" in block
        entries = species_levels(block)
        assert len(entries) == 6
        assert all(64 <= level <= 67 for _, level in entries)
        species = {name for name, _ in entries}
        if trainer.endswith("_SQUIRTLE"):
            assert {"Nidoking", "Arcanine"} <= species
            assert not ({"Pikachu", "Raichu"} & species)
        elif trainer.endswith("_BULBASAUR"):
            assert {"Raichu", "Arcanine"} <= species
            assert not ({"Nidoran M", "Nidorino", "Nidoking"} & species)
        else:
            assert {"Raichu", "Nidoking"} <= species
            assert not ({"Growlithe", "Arcanine"} & species)


def test_first_champion_keeps_safe_hall_of_fame_and_elite_four_singles():
    champion = read("data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc")
    assert "PokemonLeague_ChampionsRoom_EventScript_Battle" in champion
    assert "warp MAP_POKEMON_LEAGUE_HALL_OF_FAME" in champion
    hall = read("data/maps/PokemonLeague_HallOfFame_Frlg/scripts.inc")
    for required in ("FLDEFF_HALL_OF_FAME_RECORD_FRLG", "EnterHallOfFame",
                     "EventScript_SetDefeatedEliteFourFlagsVars"):
        assert required in hall

    party = read("src/data/trainers_frlg.party")
    for trainer in ("TRAINER_ELITE_FOUR_LORELEI", "TRAINER_ELITE_FOUR_BRUNO",
                    "TRAINER_ELITE_FOUR_AGATHA", "TRAINER_ELITE_FOUR_LANCE"):
        block = trainer_block(party, trainer)
        assert "Double Battle: No" in block
        assert len(species_levels(block)) == 6


def test_previous_tutors_and_shop_stock_are_preserved():
    tutors = read("data/scripts/move_tutors_frlg.inc")
    for move in ("MOVE_DRAIN_PUNCH", "MOVE_EARTH_POWER", "MOVE_POWER_UP_PUNCH",
                 "MOVE_ICE_PUNCH", "MOVE_BLAZE_KICK", "MOVE_HIGH_JUMP_KICK",
                 "MOVE_HEAT_WAVE"):
        assert move in tutors
    celadon = read("data/maps/CeladonCity_DepartmentStore_2F_Frlg/scripts.inc")
    indigo = read("data/maps/IndigoPlateau_PokemonCenter_1F_Frlg/scripts.inc")
    assert "ITEM_TM50" in celadon and "ITEM_PP_MAX" in celadon
    assert "ITEM_RARE_CANDY" in indigo
