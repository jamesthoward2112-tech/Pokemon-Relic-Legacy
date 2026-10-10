"""PR23: exact raw Mercury donor assets and Eevee visual-only isolation."""
from pathlib import Path
import importlib.util
import subprocess
import sys
import struct
import zlib

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "assets/prl_mercury/mercury_sprite_payload.b64"


def generated_sources():
    subprocess.run([sys.executable, str(ROOT / "tools/prl_mercury_assets.py")],
                   cwd=ROOT, check=True)


def test_complete_source_archive_and_palette_generation():
    generated_sources()
    assert SOURCE.is_file() and SOURCE.stat().st_size >= 5000
    for species in ("fortotoise", "charaxis", "prl_eevee_twins"):
        directory = ROOT / "graphics/pokemon" / species
        for pose in ("front", "back"):
            png = (directory / (pose + ".png")).read_bytes()
            assert png[:8] == b"\x89PNG\r\n\x1a\n"
            assert struct.unpack_from(">II", png, 16) == (64, 64)
            assert png.endswith(b"IEND\xaeB\x60\x82")
        for palette in ("normal", "shiny"):
            pal = (directory / (palette + ".pal")).read_text(encoding="ascii")
            assert pal.startswith("JASC-PAL\n0100\n16\n")
            assert len(pal.splitlines()) == 19


def test_relic_species_are_not_replaced_with_new_forms():
    info = (ROOT / "src/data/pokemon/species_info/prl_custom.h").read_text()
    for species in ("Charaxis", "Fortotoise"):
        assert "gMonFrontPic_" + species in info
        assert "gMonBackPic_" + species in info
        assert "gMonPalette_" + species in info
        assert "gMonShinyPalette_" + species in info
    assert "gMonFrontPic_Edensaur" in info
    assert "Gigantamax" not in info


def test_eevee_twins_specific_four_battles_only():
    code = (ROOT / "src/battle_gfx_sfx_util.c").read_text()
    graphics = (ROOT / "src/graphics.c").read_text()
    assert "PRLIsSteeveNeeveeSignatureEevee" in code
    assert "species != SPECIES_EEVEE" in code
    assert "IsOnPlayerSide(battler)" in code
    assert "BATTLE_TYPE_TRAINER" in code
    for tag in ("ROUTE3", "SS_ANNE", "TOWER", "ROUTE23"):
        assert "TRAINER_STEEVE_NEEVEE_" + tag in code
    assert "DecompressDataWithHeaderWram(gPRLEeveeTwinsFrontPic" in code
    assert "gPRLEeveeTwinsShinyPalette" in code
    assert "GetMonFrontSpritePal(mon)" in code
    assert "gPRLEeveeTwinsFrontPic" in graphics
    assert "graphics/pokemon/prl_eevee_twins/front.png" in graphics
    assert "GetGMaxTargetSpecies" not in code.split("PRLIsSteeveNeeveeSignatureEevee")[1].split("void BattleLoadMonSpriteGfx")[0]
