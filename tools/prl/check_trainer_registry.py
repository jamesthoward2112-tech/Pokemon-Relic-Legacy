#!/usr/bin/env python3
"""Generate/check the append-only PRL trainer namespace from the ID lock.

Kanto IDs may never move. Emerald local IDs remain unchanged and become
trainer handles 0x1000+ID. Future Johto entries use 0x2000+ID.
"""
import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LOCK = ROOT / "tools/prl/trainer_ids.lock.json"
FRLG = ROOT / "include/constants/opponents_frlg.h"
EMERALD = ROOT / "include/constants/opponents.h"
OUTPUT = ROOT / "include/constants/opponents_prl.h"
ENTRY = re.compile(r"^#define\s+(TRAINER_[A-Z0-9_]+)\s+(\d+)\s*(?://.*)?$", re.M)

def ids(source):
    return {key: int(value) for key, value in ENTRY.findall(source.read_text())}

def run():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="verify source against committed lock and aliases")
    args = parser.parse_args()
    lock = json.loads(LOCK.read_text())
    current_kanto, current_hoenn = ids(FRLG), ids(EMERALD)
    for region, prior, current in (
        ("Kanto", lock["kanto"], current_kanto),
        ("Hoenn", lock["hoenn"], current_hoenn),
    ):
        for name, stable_id in prior.items():
            if current.get(name) != stable_id:
                raise SystemExit(f"{region} trainer ID changed/removed: {name}: {stable_id} -> {current.get(name)}")
        # New entries can be appended but require an explicit reviewed lock update.
        if set(current) != set(prior):
            raise SystemExit(f"{region} IDs added without updating lock: {sorted(set(current)-set(prior))}")
    if max(current_hoenn.values()) >= 1024:
        raise SystemExit("Hoenn exceeded reserved 1024-ID trainer handle/flag namespace.")
    defines = {
        name: "TRAINER_HOENN_" + name.removeprefix("TRAINER_")
        for name in current_hoenn
        if name != "TRAINER_NONE"
    }
    expected_lines = {f"#define {alias} (0x1000 + {name})" for name, alias in defines.items()}
    generated_lines = {l for l in OUTPUT.read_text().splitlines() if l.startswith("#define TRAINER_HOENN_")}
    if generated_lines != expected_lines:
        raise SystemExit("Hoenn aliases don't match ID lock; regenerate opponents_prl.h.")
    values = list(current_kanto.values())
    if len(values) != len(set(values)):
        raise SystemExit("Duplicate Kanto trainer IDs.")
    print(f"PASS: {len(current_kanto)} immutable Kanto IDs; {len(current_hoenn)} stable Hoenn IDs; regional flags independent.")

if __name__ == "__main__":
    run()
