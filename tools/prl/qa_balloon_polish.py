#!/usr/bin/env python3
"""Real-ROM, per-frame assertions and video for both balloon escapes."""
import argparse
import json
import subprocess
from qa_great_badge_heist import QA, contact_sheet, BADGE_FLAGS, RECOVERY_FLAGS
from qa_great_badge_heist import FLAG_HEIST_STARTED, FLAG_ALL_RECOVERED
from qa_great_badge_heist import FLAG_FINAL_DEFEATED, FLAG_HIDE_TRIO, FLAG_HIDE_BALLOON, MAP_ROUTE23


def signed(value):
    return value if value < 0x8000 else value - 0x10000


def check_scene(qa, final):
    emu = qa.open()
    name = 'final' if final else 'heist'
    video = None
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
        if not final:
            # Match the actual south coordinate trigger camera, rather than the warp tile.
            qa.call(emu, 'TryMoveObjectEventToMapCoords', 255, 42, 37, 8, 152)
            emu.frame(2)
        label = 'Route23_EventScript_PRLFinalRocketWon' if final else 'Route23_EventScript_PRLGreatBadgeHeist'
        qa.call(emu, 'ScriptContext_SetupScript', qa.sym[label])
        trace = []
        pictures = []
        ready = False
        appeared = False
        overlap = 0
        previous_y = None
        previous_bottom = None
        upward_pixels = downward_pixels = 0
        jump_seen = set()
        last_coords = {}
        last_finished = {}
        collected = set()
        collected_frame = exit_frame = None
        last_balloon_bottom = None
        entered_at = None
        video = subprocess.Popen(['ffmpeg', '-v', 'error', '-y', '-f', 'rawvideo', '-pixel_format', 'rgba', '-video_size', '240x160', '-framerate', '59.7275', '-i', '-', '-an', '-vf', 'scale=720:480:flags=neighbor', '-c:v', 'libx264', '-crf', '18', '-pix_fmt', 'yuv420p', str(qa.out / (name + '-escape.mp4'))], stdin=subprocess.PIPE)
        for frame in range(2200):
            emu.frame(1, 'A' if frame % 120 == 80 else 0)
            video.stdin.write(emu.pixels())
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
                    sprite = qa.sym['gSprites'] + emu.read(address + 0x23, 1) * 0x44
                    if appeared and signed(emu.read(sprite + 0x26, 2)) < 0:
                        jump_seen.add(key)
                elif appeared and key in last_coords and key not in collected:
                    assert last_coords[key] == (15, 37 if final else 157), (name, key, 'removed before basket', last_coords[key])
                    assert last_finished[key], (name, key, 'boarding movement unfinished')
                    collected.add(key)
                    if len(collected) == 3:
                        collected_frame = frame
            actors = all(key in visible for key in (10, 11, 12))
            ready |= actors
            bottom = y = None
            balloon = False
            if 13 in objects:
                sprite = qa.sym['gSprites'] + emu.read(objects[13] + 0x23, 1) * 0x44
                y = signed(emu.read(sprite + 0x22, 2)) + signed(emu.read(sprite + 0x26, 2)) + signed(emu.read(qa.sym['gSpriteCoordOffsetY'], 2))
                bottom = y + 32
                balloon = 13 in visible and bottom > 0 and y - 32 < 160
                if not emu.read(objects[13] + 1, 1) & 0x20:
                    if previous_y is not None:
                        delta = y - previous_y
                        assert abs(delta) <= 2, (name, frame, 'position jump', previous_y, y)
                        if delta > 0:
                            assert exit_frame is None, (name, 'descent after takeoff')
                            downward_pixels += delta
                        elif delta < 0:
                            assert collected_frame is not None, (name, 'lift before boarding completed')
                            exit_frame = exit_frame if exit_frame is not None else frame
                            upward_pixels -= delta
                    previous_y = y
                    previous_bottom = bottom
                last_balloon_bottom = bottom
            elif previous_y is not None:
                assert last_balloon_bottom <= 0, (name, 'removed before clearing top', last_balloon_bottom)
            if ready and not appeared:
                assert actors or balloon, (name, frame, 'trio disappeared before arrival')
            if balloon:
                if not appeared:
                    assert actors, (name, 'trio absent at arrival')
                    assert bottom <= 2, (name, 'balloon popped in', bottom)
                    entered_at = frame
                    pictures.append(('first pixels at top', emu.image()))
                appeared = True
                overlap += actors
            if entered_at is not None and frame % 8 == 0 and (13 in objects or frame == collected_frame):
                pictures.append((f'frame {frame}', emu.image()))
            trace.append({'frame': frame, 'visible': sorted(visible), 'balloon_y': y, 'balloon_bottom': bottom})
        assert appeared and overlap >= 24, (name, 'missing boarding overlap', overlap)
        assert collected == {10, 11, 12} and jump_seen == {10, 11, 12}, (name, 'missing visible jump or boarding', collected, jump_seen)
        assert downward_pixels >= 24 and upward_pixels >= 24
        assert exit_frame - collected_frame <= 3, (name, 'pause before takeoff', collected_frame, exit_frame)
        assert last_balloon_bottom <= 0
        assert qa.flag(emu, FLAG_HEIST_STARTED) and all(qa.flag(emu, flag) for flag in BADGE_FLAGS)
        assert qa.flag(emu, FLAG_HIDE_TRIO) and qa.flag(emu, FLAG_HIDE_BALLOON)
        if final:
            assert qa.flag(emu, FLAG_FINAL_DEFEATED)
        contact_sheet(pictures, qa.out / f'{name}-boarding-contact.png')
        (qa.out / f'{name}-frames.json').write_text(json.dumps(trace))
        return {'smooth_top_entry': True, 'all_three_jump_and_finish_boarding': True, 'top_fully_cleared_before_removal': True, 'overlap_frames': overlap, 'takeoff_delay_frames': exit_frame - collected_frame, 'upward_pixels': upward_pixels, 'downward_pixels': downward_pixels, 'progression_preserved': True}
    finally:
        if video:
            video.stdin.close()
            assert video.wait() == 0
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
