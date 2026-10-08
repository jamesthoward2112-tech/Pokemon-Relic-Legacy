#!/usr/bin/env python3
"""Pull exact unchanged HWL Scene 0 art for an *isolated* PRL test.
License/provenance remains with the donor; no gameplay resources overwritten.
Only GitHub Actions receives these files prior to make.
"""
from pathlib import Path
import hashlib
import urllib.request
from PIL import Image

REPO = "rafaelsanna/HOENN-S-LAST-WISH-project"
COMMIT = "8f9c28e5437d9e37ed3f6873bb8dd54dac78ccf3"
BLOBS = {
    "bgsky00.png": "946533a0a497fd3dd3b0a6d5630d21a25d0b3e56",
    "bgsky00.bin": "8c28782ebe6f3137f7994b7379bd62fc2b7baf1c",
    "bgsky01.png": "d9996cb311cd6cdc18b4e59952de93db1e7e55b3",
    "bgsky01.bin": "a834fae31d5a052d96123a54c6ddf9424f3e88cf",
    "bgsky02.png": "34e26f0464b2879338ebe6fe62809a9978d52e1a",
    "bgsky02.bin": "6d1d740822f8f652029b1edb73189b39d1729037",
    "bg00.png": "a584d86db4547df7d905b10b13ceaf46ef8418d3",
    "bg00.bin": "6ba3e0531dfd571bbba2eb8a91eba38f99a8dbd3",
    "bg01.png": "0854fbbc809260aade6ff680c37b58b3db0eeab5",
    "bg01.bin": "bb053875446a2a9a7521502948f3168025befdd5",
    "bg02.png": "5256ed42ef97be5570b56ba728851a537104e0a3",
    "bg02.bin": "abad780751a2595494f57b1e6d4d638cbeb18f22",
    "bg03.png": "8689e10514369964b63b41a15e3b12623bc7b7c9",
    "bg03.bin": "b2434c47d1b97032bb5e642ad913adf1c0c09cb0",
    "clouds.png": "4537995f38e0769b8d244ba07c0034f174183610",
    "clouds.bin": "9a5fab9b4d4582fd5dd9445db8c3ade5e5a07c04",
    "shrine.png": "8322783b36041ef8982ddd526a829ad940ca2911",
    "moon.png": "ca89951f84986578aca5b43674e0d599e5e53ea9",
    "comet.png": "c2719a0e8c9d85060b1733efdffb6a7ad5593e36",
    "celebi.png": "e0de2459dca6c16c8d0aa01c23b73065869bea8c",
    "jirachi.png": "427bc5565899fe667b67a8be0c4ca027d5e4e9b6",
    "celebi2.png": "4ecdd567a34285738566e29f261eb27f530f1a87",
    "jirachi2.png": "d3c880a460620eb965e4629556703241d5ae7822"
}
DEST = Path("graphics/intro/prl_hwl")
DEST.mkdir(parents=True, exist_ok=True)
for name, expected in BLOBS.items():
    url = f"https://raw.githubusercontent.com/{REPO}/{COMMIT}/graphics/intro/scene_0/{name}"
    with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent":"PRL-intro-donor-trial"}), timeout=55) as res:
        data = res.read()
    digest = hashlib.sha1(b"blob " + str(len(data)).encode() + b"\0" + data).hexdigest()
    if digest != expected:
        raise RuntimeError(f"HWL source mismatch: {name}, {digest} != {expected}")
    if name.endswith(".bin") and len(data) != 1280:
        raise RuntimeError(f"Invalid 32x20 donor map size: {name} {len(data)}")
    if name.endswith(".png"):
        im = Image.open(__import__("io").BytesIO(data))
        if im.mode != "P" or len(set(im.getdata())) > 16:
            raise RuntimeError(f"HWL source not 4bpp paletted: {name}")
    (DEST / name).write_bytes(data)
    print("HWL_SOURCE_PASS", name, len(data), digest)
print("HWL_SCENE0_DONOR_READY",len(BLOBS))
