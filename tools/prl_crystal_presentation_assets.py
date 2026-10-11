#!/usr/bin/env python3
"""Materialize the exact approved GBA art before Make scans graphics."""
import base64, hashlib, json, zlib
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
def unpack():
    bank = json.loads((ROOT / 'assets/prl_crystal_presentation.json').read_text())
    raw = zlib.decompress(base64.b64decode(bank['zlib_base64']))
    if hashlib.sha256(raw).hexdigest() != bank['payload_sha256']:
        raise ValueError('Crystal presentation asset bank checksum mismatch')
    for name, encoded in json.loads(raw).items():
        path = Path(name)
        if '..' in path.parts or path.is_absolute() or not name.startswith(('graphics/battle_environment/crystal_redux/', 'graphics/battle_transitions/prl/')):
            raise ValueError('Invalid art path')
        data = base64.b64decode(encoded)
        target = ROOT / path
        if not target.exists() or target.read_bytes() != data:
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
if __name__ == '__main__':
    unpack()
