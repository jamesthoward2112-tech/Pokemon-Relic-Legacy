#!/usr/bin/env python3
"""Verify GBA ROM identity and header integrity for PRR builds."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
from typing import Any

_MIN_HEADER_SIZE = 0xBE


def _decode_ascii(field: bytes) -> str:
    return field.decode("ascii", errors="replace").rstrip("\x00\xff ")


def verify_rom(path: Path) -> dict[str, Any]:
    data = path.read_bytes()
    if len(data) < _MIN_HEADER_SIZE:
        raise ValueError(
            f"{path} is too small to contain a complete GBA header "
            f"({len(data)} bytes)"
        )

    calculated = (-sum(data[0xA0:0xBD]) - 0x19) & 0xFF
    stored = data[0xBD]
    return {
        "path": str(path),
        "size": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "title": _decode_ascii(data[0xA0:0xAC]),
        "game_code": _decode_ascii(data[0xAC:0xB0]),
        "maker_code": _decode_ascii(data[0xB0:0xB2]),
        "version": data[0xBC],
        "header_checksum_stored": stored,
        "header_checksum_calculated": calculated,
        "header_checksum_valid": stored == calculated,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    args = parser.parse_args()

    try:
        result = verify_rom(args.rom)
    except (OSError, ValueError) as error:
        print(json.dumps({"error": str(error)}, indent=2))
        return 2

    print(json.dumps(result, indent=2, sort_keys=True))
    return 0 if result["header_checksum_valid"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
