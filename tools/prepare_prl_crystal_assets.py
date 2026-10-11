#!/usr/bin/env python3
"""Materialize the verified donor assets stored in the source archive."""
import base64
import io
import zipfile
from pathlib import Path

root = Path(__file__).resolve().parents[1] / 'graphics/prl_crystal_redux'
with zipfile.ZipFile(io.BytesIO(base64.b64decode((root / 'assets.zip.b64').read_bytes()))) as archive:
    assert len(archive.namelist()) == 38 * 5 + 1
    for name in archive.namelist():
        assert '..' not in Path(name).parts and not name.startswith('/')
        path = root / name
        data = archive.read(name)
        if not path.exists() or path.read_bytes() != data:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
