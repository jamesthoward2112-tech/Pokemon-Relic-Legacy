#!/usr/bin/env python3
"""Restore authentic original approved trainer-only Jessie & James sprite art."""
import base64
import hashlib
import json
import zlib
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]


def unpack():
    bank = json.loads((ROOT / "assets/prl_rocket_dark_20261011.json").read_text())
    assert bank["schema"] == "prl-jj-dark-sprites-v1"
    blob = zlib.decompress(base64.b64decode(bank["zlib_base64"]))
    assert hashlib.sha256(blob).hexdigest() == bank["payload_sha256"]
    files = json.loads(blob)
    expected = {f"graphics/battle_transitions/prl/dark_{p}.{ext}"
                for p in ("arbok", "victreebel", "raticate", "gyarados")
                for ext in ("4bpp.lz", "gbapal")}
    assert set(files) == expected, "Unexpected Rocket asset path"
    for name, encoded in files.items():
        raw = base64.b64decode(encoded, validate=True)
        if name.endswith('.gbapal'):
            assert len(raw) == 32
        else:
            assert len(raw) % 4 == 0 and raw[0] == 0x10 and raw[1:4] == b"\x00\x08\x00"
        dest = ROOT / name
        dest.parent.mkdir(parents=True, exist_ok=True)
        if not dest.exists() or dest.read_bytes() != raw:
            dest.write_bytes(raw)
    return expected


if __name__ == "__main__":
    unpack()