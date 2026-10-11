#!/usr/bin/env python3
"""Unpack approved PRL Jessie & James / Steevee & Neevee BG0 intro artwork.

Runs AFTER the unchanged, user-tested Crystal Redux presentation importer.
Native PatternWeave task/IDs/trainer routes already exist in the approved source.
"""
import base64
import hashlib
import json
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PACK = ROOT / "assets/prl_duo_20261011.json"


def unpack():
    bank = json.loads(PACK.read_text(encoding="utf-8"))
    assert bank["schema"] == "prl-duo-asset-pack-v1"
    raw = zlib.decompress(base64.b64decode(bank["zlib_base64"]))
    assert hashlib.sha256(raw).hexdigest() == bank["payload_sha256"]
    data = json.loads(raw)
    expected = {f"graphics/battle_transitions/prl/{name}.{ext}"
                for name in ("jessie_james", "eevee_twins")
                for ext in ("4bpp", "bin", "gbapal")}
    assert set(data) == expected, "Unapproved asset path in pack"
    for path, payload in data.items():
        bytes_ = base64.b64decode(payload, validate=True)
        ext = path.rsplit(".", 1)[-1]
        assert len(bytes_) == {"4bpp": 16384, "bin": 2048, "gbapal": 32}[ext]
        if ext == "bin":
            import struct
            tiles = struct.unpack("<1024H", bytes_)
            assert all(tile >> 12 == 13 and (tile & 0x3ff) < 512 for tile in tiles)
        target = ROOT / path
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.exists() or target.read_bytes() != bytes_:
            target.write_bytes(bytes_)
    return expected


if __name__ == "__main__":
    unpack()