#!/usr/bin/env python3
"""Focused mGBA runtime QA for PRL's Great Badge Heist build."""

from __future__ import annotations

import argparse
import ctypes as C
import json
import subprocess
from pathlib import Path

from PIL import Image, ImageDraw

from mgba_qa import Emulator


BADGE_FLAGS = range(0x820, 0x828)
RECOVERY_FLAGS = range(0x4DA, 0x4E2)
FLAG_HEIST_STARTED = 0x4D9
FLAG_ALL_RECOVERED = 0x4E2
FLAG_FINAL_DEFEATED = 0x4E3
FLAG_HIDE_TRIO = 0x4E4
FLAG_HIDE_BALLOON = 0x4E5

MAP_ROUTE23 = (37, 42)
MAP_VICTORY_ROAD_1F = (35, 39)
MAP_VIRIDIAN_GYM = (39, 1)

SPECIES_EDENSAUR = 1574
SPECIES_CHARAXIS = 1575
SPECIES_FORTOTOISE = 1576
SPECIES_KABUTOPS = 141
SPECIES_AERODACTYL = 142


def symbols(elf: Path, nm: str) -> dict[str, int]:
    output = subprocess.check_output([nm, "-n", str(elf)], text=True)
    result: dict[str, int] = {}
    for line in output.splitlines():
        fields = line.split()
        if len(fields) == 3:
            result[fields[2]] = int(fields[0], 16)
    return result


def contact_sheet(images: list[tuple[str, Image.Image]], path: Path) -> None:
    columns = 4
    rows = (len(images) + columns - 1) // columns
    sheet = Image.new("RGB", (240 * columns, 180 * rows), "#202020")
    draw = ImageDraw.Draw(sheet)
    for index, (label, frame) in enumerate(images):
        x = index % columns * 240
        y = index // columns * 180
        sheet.paste(frame, (x, y))
        draw.text((x + 4, y + 162), label, fill="white")
    sheet.save(path)


class QA:
    def __init__(self, args: argparse.Namespace):
        self.args = args
        self.sym = symbols(Path(args.elf), args.nm)
        self.out = Path(args.out)
        self.out.mkdir(parents=True, exist_ok=True)
        self.results: dict[str, object] = {}

    def open(self) -> Emulator:
        emu = Emulator(self.args.rom, self.args.save, self.args.probe)
        emu.lib.prl_call.argtypes = [C.c_uint, C.POINTER(C.c_uint), C.c_uint]
        emu.lib.prl_call.restype = C.c_uint
        return emu

    def call(self, emu: Emulator, name: str, *args: int) -> int:
        values = (C.c_uint * len(args))(*args) if args else None
        return emu.lib.prl_call(self.sym[name], values, len(args))

    def flag(self, emu: Emulator, flag: int) -> bool:
        return bool(self.call(emu, "FlagGet", flag))

    def set_flag(self, emu: Emulator, flag: int, value: bool = True) -> None:
        self.call(emu, "FlagSet" if value else "FlagClear", flag)

    def load_existing_save(self, emu: Emulator) -> None:
        emu.frame(1400)
        emu.press("START", after=180)
        emu.press("A", after=300)
        emu.press("A", after=800)

    def warp(self, emu: Emulator, group: int, number: int, warp_id: int) -> None:
        self.call(emu, "SetWarpDestination", group, number, warp_id & 0xFF, 0xFF, 0xFF)
        self.call(emu, "WarpIntoMap")
        self.call(emu, "SetMainCallback2", self.sym["CB2_LoadMap"] | 1)
        emu.frame(600)

    def setup_party(self, emu: Emulator, species: tuple[int, ...]) -> None:
        party = emu.read(self.sym["gPlayerPartyPtr"], 4)
        count = emu.read(self.sym["gPlayerPartyCountPtr"], 4)
        for index, mon in enumerate(species):
            self.call(emu, "CreateRandomMon", party + index * 100, mon, 70)
        emu.write(count, len(species), 1)

    def run_script_frames(self, emu: Emulator, label: str, presses: int,
                          prefix: str) -> list[tuple[str, Image.Image]]:
        self.call(emu, "ScriptContext_SetupScript", self.sym[label])
        frames: list[tuple[str, Image.Image]] = []
        for index in range(presses):
            emu.frame(80)
            frames.append((f"{prefix} {index + 1}", emu.image()))
            emu.frame(1, "A")
            emu.frame(8)
            frames.append((f"{prefix} {index + 1} +8", emu.image()))
            emu.frame(32)
        emu.frame(240)
        frames.append((f"{prefix} end", emu.image()))
        return frames

    def existing_save_and_heist(self) -> None:
        emu = self.open()
        try:
            self.load_existing_save(emu)
            emu.shot(self.out / "existing-save-loaded.png")
            loaded = emu.read(self.sym["gSaveBlock1Ptr"], 4)
            assert 0x02000000 <= loaded < 0x02040000
            self.results["existing_save_loaded"] = True

            for flag in BADGE_FLAGS:
                self.set_flag(emu, flag)
            self.set_flag(emu, FLAG_HEIST_STARTED, False)
            self.set_flag(emu, FLAG_HIDE_TRIO, False)
            self.set_flag(emu, FLAG_HIDE_BALLOON, True)
            self.warp(emu, *MAP_ROUTE23, 2)
            frames = self.run_script_frames(
                emu, "Route23_EventScript_PRLGreatBadgeHeist", 14, "heist"
            )
            contact_sheet(frames, self.out / "heist-balloon-contact.png")
            assert self.flag(emu, FLAG_HEIST_STARTED)
            assert all(self.flag(emu, flag) for flag in BADGE_FLAGS)
            self.results["heist_script_completed"] = True
            self.results["original_badges_preserved"] = True

            self.call(emu, "SetWarpDestination", *MAP_VICTORY_ROAD_1F, 0xFF, 10, 10)
            destination = self.sym["sWarpDestination"]
            redirected = (emu.read(destination, 1), emu.read(destination + 1, 1),
                          emu.read(destination + 2, 1))
            assert redirected == (*MAP_ROUTE23, 2), redirected
            self.results["victory_road_warp_redirected"] = True

            save = emu.read(self.sym["gSaveBlock1Ptr"], 4)
            emu.write(save + 4, MAP_VICTORY_ROAD_1F[0], 1)
            emu.write(save + 5, MAP_VICTORY_ROAD_1F[1], 1)
            emu.write(save + 6, 0xFF, 1)
            emu.write(save + 8, 10, 2)
            emu.write(save + 10, 10, 2)
            self.call(emu, "SetMainCallback2", self.sym["CB2_ContinueSavedGame"] | 1)
            emu.frame(900)
            save = emu.read(self.sym["gSaveBlock1Ptr"], 4)
            resumed = (emu.read(save + 4, 1), emu.read(save + 5, 1),
                       emu.read(save + 6, 1))
            assert resumed == (*MAP_ROUTE23, 2), resumed
            emu.shot(self.out / "existing-save-victory-road-redirect.png")
            self.results["existing_save_victory_road_redirected"] = True
        finally:
            emu.close()

    def final_battle_and_escape(self) -> None:
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.setup_party(emu, (SPECIES_EDENSAUR, SPECIES_CHARAXIS))
            self.set_flag(emu, FLAG_HEIST_STARTED)
            for flag in RECOVERY_FLAGS:
                self.set_flag(emu, flag)
            self.set_flag(emu, FLAG_ALL_RECOVERED)
            self.set_flag(emu, FLAG_FINAL_DEFEATED, False)
            self.set_flag(emu, FLAG_HIDE_TRIO, False)
            self.set_flag(emu, FLAG_HIDE_BALLOON, True)
            self.warp(emu, *MAP_ROUTE23, 0)
            battle = self.run_script_frames(
                emu, "Route23_EventScript_PRLFinalRocketBattle", 12, "final battle"
            )
            contact_sheet(battle, self.out / "final-battle-contact.png")
            self.results["final_battle_transition_rendered"] = True

            emu.close()
            emu = self.open()
            self.load_existing_save(emu)
            self.set_flag(emu, FLAG_HEIST_STARTED)
            self.set_flag(emu, FLAG_ALL_RECOVERED)
            self.set_flag(emu, FLAG_FINAL_DEFEATED, False)
            self.set_flag(emu, FLAG_HIDE_TRIO, False)
            self.set_flag(emu, FLAG_HIDE_BALLOON, True)
            self.warp(emu, *MAP_ROUTE23, 0)
            escape = self.run_script_frames(
                emu, "Route23_EventScript_PRLFinalRocketWon", 14, "final escape"
            )
            contact_sheet(escape, self.out / "final-balloon-contact.png")
            assert self.flag(emu, FLAG_FINAL_DEFEATED)
            assert self.flag(emu, FLAG_HIDE_TRIO)
            assert self.flag(emu, FLAG_HIDE_BALLOON)
            self.results["final_balloon_script_completed"] = True
            self.results["victory_road_unlocked"] = True
        finally:
            emu.close()

    def giovanni_and_fortotoise(self) -> None:
        emu = self.open()
        try:
            self.load_existing_save(emu)
            self.setup_party(emu, (SPECIES_FORTOTOISE,))
            self.set_flag(emu, FLAG_HEIST_STARTED)
            for flag in range(0x4DA, 0x4E1):
                self.set_flag(emu, flag)
            self.set_flag(emu, 0x4E1, False)
            self.set_flag(emu, FLAG_ALL_RECOVERED, False)
            self.set_flag(emu, 0x055, False)  # FLAG_HIDE_VIRIDIAN_GIOVANNI
            self.warp(emu, *MAP_VIRIDIAN_GYM, 0)
            # The entrance camera does not instantiate Giovanni yet. Point the
            # trainer command at an inactive non-player object slot; this only
            # supplies the facing-direction bookkeeping the command requires.
            emu.write(self.sym["gSelectedObjectEvent"], 1, 1)
            emu.write(self.sym["gSpecialVar_LastTalked"], 8, 2)
            battle = self.run_script_frames(
                emu, "ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecovery", 12,
                "giovanni"
            )
            contact_sheet(battle, self.out / "giovanni-fortotoise-contact.png")
            self.results["giovanni_recovery_battle_rendered"] = True
            self.results["fortotoise_back_rendered"] = True
        finally:
            emu.close()

        skeleton_frames: list[tuple[str, Image.Image]] = []
        for species, name in ((SPECIES_KABUTOPS, "Kabutops"),
                              (SPECIES_AERODACTYL, "Aerodactyl")):
            emu = self.open()
            try:
                self.load_existing_save(emu)
                self.setup_party(emu, (SPECIES_FORTOTOISE,))
                self.set_flag(emu, FLAG_HEIST_STARTED)
                for flag in range(0x4DA, 0x4E1):
                    self.set_flag(emu, flag)
                self.set_flag(emu, 0x4E1, False)
                self.set_flag(emu, FLAG_ALL_RECOVERED, False)
                self.set_flag(emu, 0x055, False)
                self.warp(emu, *MAP_VIRIDIAN_GYM, 0)
                emu.write(self.sym["gSelectedObjectEvent"], 1, 1)
                emu.write(self.sym["gSpecialVar_LastTalked"], 8, 2)
                self.call(
                    emu, "ScriptContext_SetupScript",
                    self.sym["ViridianCity_Gym_Frlg_EventScript_PRLBadgeRecovery"]
                )
                for _ in range(5):
                    emu.frame(80)
                    emu.press("A", after=40)

                enemy_party = emu.read(self.sym["gEnemyPartyPtr"], 4)
                self.call(emu, "CreateRandomMon", enemy_party, species, 62)
                for index in range(7):
                    emu.frame(80)
                    skeleton_frames.append((f"{name} {index + 1}", emu.image()))
                    emu.press("A", after=40)
                self.results[f"giovanni_skeletal_{name.lower()}_rendered"] = True
            finally:
                emu.close()
        contact_sheet(skeleton_frames, self.out / "giovanni-skeletons-contact.png")

    def run(self) -> None:
        self.existing_save_and_heist()
        self.final_battle_and_escape()
        self.giovanni_and_fortotoise()
        (self.out / "results.json").write_text(
            json.dumps(self.results, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
        print(json.dumps(self.results, sort_keys=True))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom")
    parser.add_argument("--elf", required=True)
    parser.add_argument("--save", required=True)
    parser.add_argument("--probe", required=True)
    parser.add_argument("--nm", default="arm-none-eabi-nm")
    parser.add_argument("--out", required=True)
    QA(parser.parse_args()).run()


if __name__ == "__main__":
    main()
