#!/usr/bin/env python3
"""Frame-by-frame real-ROM boarding and lift-off assertions for both escapes."""
import argparse
import json
from qa_great_badge_heist import QA, contact_sheet, BADGE_FLAGS, RECOVERY_FLAGS
from qa_great_badge_heist import FLAG_HEIST_STARTED, FLAG_ALL_RECOVERED
from qa_great_badge_heist import FLAG_FINAL_DEFEATED, FLAG_HIDE_TRIO, FLAG_HIDE_BALLOON, MAP_ROUTE23


def check_scene(qa, final):
    emu = qa.open()
    name = 'final' if final else 'heist'
    try:
        qa.load_existing_save(emu)
        for flag in BADGE_FLAGS:
            qa.set_flag(emu, flag)
        qa.set_flag(emu, FLAG_HEIST_STARTED, final)
        qa.set_flag(emu, FLAG_ALL_RECOVERED, final)
        for flag in RECOVERY_FLAGS:
            qa.set_flag(emu, flag, final)
        qa.set_flag(emu, FLAG_FINAL_DEFEATED, False)
        qa.set_flag(emu, FLAG_HIDE_TRIO, False)
        qa.set_flag(emu, FLAG_HIDE_BALLOON, True)
        qa.warp(emu, *MAP_ROUTE23, 0 if final else 2)
        label = 'Route23_EventScript_PRLFinalRocketWon' if final else 'Route23_EventScript_PRLGreatBadgeHeist'
        qa.call(emu, 'ScriptContext_SetupScript', qa.sym[label])
        trace = []
        pictures = []
        ready = False
        appeared = False
        overlap = 0
        previous_y = None
        upward_pixels = 0
        last_coords = {}
        last_finished = {}
        collected = set()
        for frame in range(2200):
            emu.frame(1, 'A' if frame % 120 == 80 else 0)
            objects = {}
            for i in range(16):
                address = qa.sym['gObjectEvents'] + 36 * i
                if emu.read(address, 1) & 1:
                    local_id = emu.read(address + 8, 1)
                    if local_id in (10, 11, 12, 13):
                        objects[local_id] = address
            visible = {key for key, address in objects.items() if not emu.read(address + 1, 1) & 0x60}
            for key in (10, 11, 12):
                if key in objects:
                    address = objects[key]
                    last_coords[key] = (emu.read(address + 0x10, 2), emu.read(address + 0x12, 2))
                    last_finished[key] = bool(emu.read(address, 1) & 0x80)
                elif appeared and key in last_coords and key not in collected:
                    assert last_coords[key] == (15, 37 if final else 157), (name, key, 'removed before reaching basket', last_coords[key])
                    assert last_finished[key], (name, key, 'removed while boarding movement still active')
                    collected.add(key)
            actors = all(key in visible for key in (10, 11, 12))
            ready |= actors
            balloon = 13 in visible
            if ready and not appeared:
                assert actors or balloon, (name, frame, 'trio vanished before balloon', visible)
            if balloon:
                if not appeared:
                    assert actors, (name, frame, 'balloon appeared after trio vanished', visible)
                    pictures.append(('balloon appears', emu.image()))
                appeared = True
                overlap += actors
                sprite = qa.sym['gSprites'] + emu.read(objects[13] + 0x23, 1) * 0x44
                y = emu.read(sprite + 0x22, 2)
                y = y if y < 0x8000 else y - 0x10000
                if previous_y is not None:
                    assert -1 <= y - previous_y <= 0, (name, frame, 'discontinuous lift', previous_y, y)
                    upward_pixels += previous_y - y
                previous_y = y
                if frame % 16 == 0:
                    pictures.append((f'frame {frame}', emu.image()))
            trace.append({'frame': frame, 'visible': sorted(visible), 'balloon_y': previous_y if balloon else None})
        assert appeared and overlap >= 24, (name, 'missing boarding overlap', overlap)
        assert collected == {10, 11, 12}, (name, 'trio not collected', collected)
        assert upward_pixels >= 48, (name, 'takeoff did not lift continuously', upward_pixels)
        assert qa.flag(emu, FLAG_HEIST_STARTED)
        assert all(qa.flag(emu, flag) for flag in BADGE_FLAGS)
        assert qa.flag(emu, FLAG_HIDE_TRIO) and qa.flag(emu, FLAG_HIDE_BALLOON)
        if final:
            assert qa.flag(emu, FLAG_FINAL_DEFEATED)
        contact_sheet(pictures, qa.out / f'{name}-boarding-contact.png')
        (qa.out / f'{name}-frames.json').write_text(json.dumps(trace))
        return {'balloon_before_trio_disappears': True, 'overlap_frames': overlap, 'continuous_upward_pixels': upward_pixels, 'progression_preserved': True}
    finally:
        emu.close()


def main():
    p = argparse.ArgumentParser()
    p.add_argument('rom')
    p.add_argument('--elf', required=True)
    p.add_argument('--save', required=True)
    p.add_argument('--probe', required=True)
    p.add_argument('--out', required=True)
    p.add_argument('--nm', default='arm-none-eabi-nm')
    qa = QA(p.parse_args())
    results = {name: check_scene(qa, final) for name, final in [('heist', False), ('final', True)]}
    (qa.out / 'results.json').write_text(json.dumps(results, indent=2))
    print(json.dumps(results))


if __name__ == '__main__':
    main()
