"""PRL Kanto: Remote PC, instant Nurse Joy, and reusable-TM dialogue checks.

The scripted interactions still require a FireRed emulator acceptance pass.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def src(path):
    return (ROOT / path).read_text(encoding="utf-8")


def block(text, label):
    pos = text.index(label + "::")
    rest = text[pos:]
    next_label = re.search(r"(?m)^[A-Za-z_][A-Za-z0-9_]*::", rest[len(label) + 2:])
    return rest[:len(label) + 2 + next_label.start()] if next_label else rest


def test_remote_pc_uses_existing_registered_storage_item():
    items = src("src/data/items.h")
    start = items.index("[ITEM_POKEMON_BOX_LINK] =")
    section = items[start:items.index("\n    [ITEM_", start + 1)]
    assert 'ITEM_NAME("Remote PC")' in section
    assert ".pocket = POCKET_KEY_ITEMS" in section
    assert ".type = ITEM_USE_FIELD" in section
    assert ".fieldUseFunc = ItemUseOutOfBattle_PokemonBoxLink" in section
    scripts = src("data/scripts/pc.inc")
    assert "EventScript_AccessPokemonBoxLink::" in scripts
    assert "special ShowPokemonStorageSystemPC" in scripts


def test_pewter_running_shoes_aide_gives_remote_pc():
    scripts = src("data/maps/PewterCity_Frlg/scripts.inc")
    gift = block(scripts, "PewterCity_EventScript_AideGiveRunningShoes")
    assert "msgbox PewterCity_Text_SwitchedShoesWithRunningShoes" in gift
    assert "giveitem ITEM_POKEMON_BOX_LINK" in gift
    assert gift.index("giveitem ITEM_POKEMON_BOX_LINK") < gift.index("setflag FLAG_SYS_B_DASH")


def test_prior_save_gets_one_remote_pc_when_returning_to_pewter():
    scripts = src("data/maps/PewterCity_Frlg/scripts.inc")
    transition = block(scripts, "PewterCity_OnTransition")
    assert "call_if_set FLAG_SYS_B_DASH, PewterCity_EventScript_PRLGrantMissedRemotePC" in transition
    fallback = block(scripts, "PewterCity_EventScript_PRLGrantMissedRemotePC")
    assert "checkitem ITEM_POKEMON_BOX_LINK, 1" in fallback
    assert "goto_if_eq VAR_RESULT, TRUE" in fallback
    assert "checkitemspace ITEM_POKEMON_BOX_LINK, 1" in fallback
    assert "goto_if_eq VAR_RESULT, FALSE" in fallback
    assert "additem ITEM_POKEMON_BOX_LINK, 1" in fallback


def test_nurse_joy_heals_with_animation_without_questions_or_dialogue():
    nurse = src("data/scripts/pkmn_center_nurse_frlg.inc")
    interaction = block(nurse, "EventScript_PkmnCenterNurse_Frlg")
    assert "incrementgamestat GAME_STAT_USED_POKECENTER" in interaction
    assert "call EventScript_PkmnCenterNurse_TakeAndHealPkmn_Frlg" in interaction
    assert all(not x in interaction for x in ("multichoice", "msgbox", "message ", "waitmessage"))
    healing = block(nurse, "EventScript_PkmnCenterNurse_TakeAndHealPkmn_Frlg")
    assert "dofieldeffect FLDEFF_POKECENTER_HEAL" in healing
    assert "waitfieldeffect FLDEFF_POKECENTER_HEAL" in healing
    assert "special HealPlayerParty" in healing
    scripts = src("data/maps/PewterCity_PokemonCenter_1F_Frlg/scripts.inc")
    assert "call EventScript_PkmnCenterNurse_Frlg" in scripts


def test_brock_does_not_say_tms_are_consumable():
    scripts = src("data/maps/PewterCity_Gym_Frlg/scripts.inc")
    talk = block(scripts, "PewterCity_Gym_Text_ExplainTM39")
    assert "TM39 contains ROCK TOMB" in talk
    assert "only one use" not in talk
    assert "pick the" not in talk
