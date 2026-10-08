#!/usr/bin/env python3
"""Fetch original Vanadium capture effect art from pinned GitHub commit.
The Kecleon character sheet is deliberately excluded; Noxichu is generated
from the approved PRL artwork instead.
"""
import hashlib
import io
import urllib.request
from pathlib import Path
from PIL import Image

REPO = "monhacks/vanadium"
COMMIT = "7b481b8e223125fed2bf674f6832366e65c5919f"
ASSETS = {
    "exclamation_mark.png": "e4f70c95e2483efd79e383104ce4063a2d819b59",
    "sand.png": "1377658d6c2499e899e0b37dc484ac41d8dc0e01",
    "poke_ball.png": "9a609f580295d730b084eac1512cf6298c6764d2",
    "flash.png": "5f857a334a9ec9342b36649ad49398552e63f785",
}
DEST = Path("graphics/intro/prl_vanadium")
DEST.mkdir(parents=True, exist_ok=True)
for filename, sha in ASSETS.items():
    url = f"https://raw.githubusercontent.com/{REPO}/{COMMIT}/graphics/intro/scene_kecleon/{filename}"
    with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent":"PRL-vanadium-pinned-donor"}), timeout=50) as response:
        raw = response.read()
    blob_sha = hashlib.sha1(b"blob " + str(len(raw)).encode() + b"\0" + raw).hexdigest()
    if blob_sha != sha:
        raise ValueError(f"Original Vanadium art hash mismatch {filename}: {blob_sha}")
    image = Image.open(io.BytesIO(raw))
    image.load()
    if image.mode != "P" or len(set(image.getdata())) > 16:
        raise ValueError(f"Vanadium source cannot be used for GBA 4bpp: {filename}")
    (DEST / filename).write_bytes(raw)
    print("VANADIUM_DONOR_PASS", filename, image.size)
print("VANADIUM_DONOR_READY")
