#!/usr/bin/env bash
set -euo pipefail

if [[ "$#" -ne 2 ]]; then
    echo "usage: $0 ROM_PATH SCREENSHOT_PATH" >&2
    exit 2
fi

rom_path=$1
screenshot_path=$2
evidence_dir=$(dirname "$screenshot_path")
log_prefix="${screenshot_path%.png}"
display_file=$(mktemp)

mkdir -p "$evidence_dir"

Xvfb -displayfd 1 -screen 0 480x320x24 -nolisten unix -listen tcp -ac >"$display_file" 2>"${log_prefix}.xvfb.log" &
xvfb_pid=$!
mgba_pid=

cleanup() {
    if [[ -n "$mgba_pid" ]]; then
        kill "$mgba_pid" 2>/dev/null || true
        wait "$mgba_pid" 2>/dev/null || true
    fi
    kill "$xvfb_pid" 2>/dev/null || true
    wait "$xvfb_pid" 2>/dev/null || true
    rm -f "$display_file"
}
trap cleanup EXIT

for _ in {1..50}; do
    if [[ -s "$display_file" ]]; then
        break
    fi
    sleep 0.1
done
if [[ ! -s "$display_file" ]]; then
    echo "Xvfb did not allocate a display" >&2
    exit 1
fi
display_number=127.0.0.1:$(cat "$display_file")
env -u WAYLAND_DISPLAY QT_QPA_PLATFORM=xcb QT_OPENGL=software LIBGL_ALWAYS_SOFTWARE=1 DISPLAY="$display_number" mgba -2 "$rom_path" >"${log_prefix}.mgba.log" 2>&1 &
mgba_pid=$!

sleep 1
if ! kill -0 "$mgba_pid" 2>/dev/null; then
    echo "mGBA exited before the screenshot was captured" >&2
    sed -n '1,120p' "${log_prefix}.mgba.log" >&2
    exit 1
fi

env -u WAYLAND_DISPLAY DISPLAY="$display_number" xwininfo -root -tree >"${log_prefix}.windows.log"
window_id=$(awk '/"mGBA"/ { print $1; exit }' "${log_prefix}.windows.log")
if [[ -n "$window_id" ]]; then
    capture_target=$window_id
else
    capture_target=root
fi

rm -f "$screenshot_path"
color_count=0
for _ in {1..10}; do
    sleep 2
    if ! kill -0 "$mgba_pid" 2>/dev/null; then
        echo "mGBA exited before a rendered frame was captured" >&2
        sed -n '1,120p' "${log_prefix}.mgba.log" >&2
        exit 1
    fi
    env -u WAYLAND_DISPLAY DISPLAY="$display_number" import -window "$capture_target" "$screenshot_path"
    color_count=$(identify -format "%k" "$screenshot_path")
    if (( color_count > 2 )); then
        break
    fi
done

if [[ ! -s "$screenshot_path" ]]; then
    echo "boot screenshot was not created" >&2
    exit 1
fi
if (( color_count <= 2 )); then
    echo "mGBA remained on a blank frame during the smoke-check window" >&2
    exit 1
fi

identify "$screenshot_path"
