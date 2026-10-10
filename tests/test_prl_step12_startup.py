"""Regression tests for the optional FRLG opening screens skipped by PRL.

Static source gates prevent unrelated Step 10-12 mechanics, save initialization,
intro Pokémon and name entry from changing silently. Actual emulator QA remains
separate from these checks.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def source(rel):
    return (ROOT / rel).read_text(encoding="utf-8-sig")


def function(source_text, name):
    match = re.search(r"\b" + re.escape(name) + r"\([^;{}]*\)\s*\{", source_text)
    if match is None:
        raise AssertionError("Function not found: " + name)
    start = match.end()
    depth = 1
    for offset in range(start, len(source_text)):
        depth += (source_text[offset] == "{") - (source_text[offset] == "}")
        if depth == 0:
            return source_text[start:offset]
    raise AssertionError("Function body not closed: " + name)


class DirectToOakStartupTests(unittest.TestCase):
    def test_firered_boot_restores_porygon_but_initializes_save(self):
        body = function(source("src/intro.c"), "CB2_InitCopyrightScreenAfterBootup")
        self.assertRegex(body, r"#if defined\(FIRERED\)\s*bool8 introFinished = TRUE;")
        self.assertRegex(body, r"#else\s*bool8 introFinished = !SetUpCopyrightScreen\(\);")
        for call in (
            "SetSaveBlocksPointers(GetSaveBlocksPointersBaseOffset())",
            "ResetMenuAndMonGlobals()",
            "Save_ResetSaveCounters()",
            "LoadGameSave(SAVE_NORMAL)",
            "Sav2_ClearSetDefault()",
            "SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound)",
            "InitHeap(gHeap, HEAP_SIZE)",
            "ResetTasks()",
            "ResetSpriteData()",
            "ResetPaletteFade()",
            "SetMainCallback2(CB2_ExpansionIntro)",
            "CreateTask(Task_HandleExpansionIntro, 0)",
        ):
            with self.subTest(call=call):
                self.assertIn(call, body)
        self.assertIn("#if EXPANSION_INTRO == TRUE", body)
        self.assertIn("SetMainCallback2(CB2_InitTitleScreen)", body)  # fallback only
        self.assertLess(body.index("LoadGameSave(SAVE_NORMAL)"), body.index("CreateTask(Task_HandleExpansionIntro, 0)"))

    def test_porygon_finishes_at_prl_title_not_stock_game_freak_movie(self):
        splash = source("src/expansion_intro.c")
        self.assertIn('graphics/expansion_intro/sprites/porygon.png', splash)
        self.assertIn('graphics/expansion_intro/sprites/dizzy_egg.png', splash)
        task = function(splash, "Task_HandleExpansionIntro")
        self.assertIn("tFrameCounter == 208", task)
        self.assertIn("gMain.newKeys != 0", task)  # player can still skip splash
        # Issue #26 restored the full pre-title Celebi/Jirachi forest, not title icons.
        self.assertRegex(task, r"#if defined\(FIRERED\)[\s\S]*?SetMainCallback2\(CB2_InitPRLHwlScene0\);")
        self.assertIn("CB2_SetUpIntroFrlg", task)  # other versions retain stock entry
        self.assertLess(task.index("SetMainCallback2(CB2_InitPRLHwlScene0)"),
                        task.index("SetMainCallback2(CB2_SetUpIntroFrlg)"))
        forest = source("src/prl_intro_trial.c")
        self.assertIn("SetMainCallback2(CB2_InitTitleScreen)", forest)

    def test_title_remains_approved_prl_entry_point(self):
        src = source("src/title_screen_frlg.c")
        self.assertIn("InitPRLTitleScreen();", function(src, "CB2_InitTitleScreenFrlg"))
        self.assertIn("SetMainCallback2(CB2_InitMainMenu)", function(src, "Task_PRLTitle"))
        menu = function(source("src/main_menu.c"), "Task_HandleMainMenuAPressed")
        self.assertIn("StartNewGameSceneFrlg()", menu)
        self.assertIn("CB2_ContinueSavedGame", menu)

    def test_new_game_initializes_resources_then_goes_direct_to_oak(self):
        body = function(source("src/oak_speech.c"), "Task_NewGameScene")
        self.assertIn("sOakSpeechResources = AllocZeroed", body)
        self.assertIn("CreateMonSpritesGfxManager", body)
        self.assertIn("InitBgsFromTemplates", body)
        self.assertIn("InitTextBoxGfxAndPrinters", body)
        setup = body.split("case 7:", 1)[1].split("return;", 1)[0]
        self.assertIn("AddWindow(&sIntro_WindowTemplates[WIN_INTRO_TEXTBOX])", setup)
        self.assertIn("SetVBlankCallback(VBlankCB_NewGameScene)", setup)
        self.assertIn("gTasks[taskId].tTimer = 0;", setup)
        self.assertIn("gTasks[taskId].func = Task_OakSpeech_Init;", setup)
        self.assertNotIn("Task_ControlsGuide_HandleInput", setup)
        self.assertNotIn("Task_PikachuIntro_LoadPage1", setup)
        self.assertNotIn("MUS_RG_NEW_GAME_INSTRUCT", setup)

    def test_vanilla_oak_and_omanyte_are_preserved(self):
        src = source("src/oak_speech.c")
        self.assertIn("#define INTRO_SPECIES SPECIES_OMANYTE", src)
        oak = function(src, "Task_OakSpeech_Init")
        self.assertIn("LoadTrainerPic(OAK_PIC, 0)", oak)
        self.assertIn("CreateNidoranFSprite(taskId)", oak)
        self.assertIn("Task_OakSpeech_WelcomeToTheWorld", oak)
        self.assertIn("DoNamingScreen", src)
        self.assertFalse((ROOT / "graphics/oak_speech/oak/archoak_approved.png").exists())


if __name__ == "__main__":
    unittest.main()
