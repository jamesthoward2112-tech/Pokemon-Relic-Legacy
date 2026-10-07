#!/usr/bin/env python3
"""PRL Foundation 001 guardrail for the native FRLG opening slice."""
from __future__ import annotations
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = Path(__file__).with_name("frlg_kanto_slice.json")
MAP_GROUPS = ROOT / "data/maps/map_groups.json"
LAYOUTS = ROOT / "data/layouts/layouts.json"

def fail(msg: str) -> None:
    print(f"[PRL foundation] FAIL: {msg}", file=sys.stderr)
    raise SystemExit(1)

def load(path: Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"cannot parse {path.relative_to(ROOT)}: {exc}")

def main() -> None:
    spec = load(MANIFEST)
    groups = load(MAP_GROUPS)
    layouts_doc = load(LAYOUTS)
    names_in_groups = {
        name
        for key, value in groups.items()
        if key != "group_order" and isinstance(value, list)
        for name in value
    }
    layouts = {x["id"]: x for x in layouts_doc["layouts"]}
    maps = {}
    for entry in spec["maps"]:
        name = entry["name"]
        if name not in names_in_groups:
            fail(f"{name} is missing from map_groups.json")
        map_json = ROOT / "data/maps" / name / "map.json"
        if not map_json.is_file():
            fail(f"{name} is missing map.json")
        m = load(map_json)
        if m.get("name") != name:
            fail(f"{name}: map.json name is {m.get('name')!r}")
        if m.get("id") != entry["id"]:
            fail(f"{name}: expected id {entry['id']}, got {m.get('id')}")
        if m.get("region") != entry["region"]:
            fail(f"{name}: expected {entry['region']}, got {m.get('region')}")
        layout_id = m.get("layout")
        if layout_id not in layouts:
            fail(f"{name}: layout {layout_id!r} is missing")
        layout = layouts[layout_id]
        if layout.get("layout_version") != "frlg":
            fail(f"{name}: layout {layout_id} is not frlg")
        for field in ("border_filepath", "blockdata_filepath"):
            p = ROOT / layout[field]
            if not p.is_file():
                fail(f"{name}: missing {field} target {layout[field]}")
        maps[name] = m
    ids_to_name = {entry["id"]: entry["name"] for entry in spec["maps"]}
    def neighbors(name: str):
        m = maps[name]
        out = set()
        for w in (m.get("warp_events") or []):
            target = ids_to_name.get(w.get("dest_map"))
            if target:
                out.add(target)
        for c in (m.get("connections") or []):
            target = ids_to_name.get(c.get("map"))
            if target:
                out.add(target)
        return out
    for a, b in spec["required_edges"]:
        if b not in neighbors(a) and a not in neighbors(b):
            fail(f"required route/warp edge missing: {a} <-> {b}")
    print(f"[PRL foundation] PASS: {len(spec['maps'])} native FRLG maps, layouts, binaries and route edges validated.")

if __name__ == "__main__":
    main()
