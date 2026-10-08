#!/usr/bin/env python3
"""Export unchanged donor intro source/art into credited, provenance-tracked archives.

Research/asset bank ONLY; nothing is installed into PRL's active ROM build.
Public donor repositories are pinned to exact commit IDs for reproducibility.
"""
from __future__ import annotations
import hashlib
import json
import os
from pathlib import Path
import time
import urllib.error
import urllib.parse
import urllib.request
import zipfile

DEST = Path("prl_intro_donor_exports")
DEST.mkdir(exist_ok=True)
TOKEN = os.environ.get("GITHUB_TOKEN", "")
SOURCES = [
    {
        "repo": "monhacks/vanadium",
        "commit": "7b481b8e223125fed2bf674f6832366e65c5919f",
        "title": "Vanadium_Kecleon_Capture_Donor",
        "prefixes": ["graphics/intro/scene_kecleon/"],
        "additional": ["src/intro.c", "include/intro.h", "README.md"],
        "note": "Kecleon captured and escapes; the original scene uses Pinball Ruby & Sapphire-inspired Poké Ball graphics and its own C animation.",
    },
    {
        "repo": "rafaelsanna/HOENN-S-LAST-WISH-project",
        "commit": "8f9c28e5437d9e37ed3f6873bb8dd54dac78ccf3",
        "title": "Hoenns_Last_Wish_Opening_Scenes_0_to_3",
        "prefixes": [
            "graphics/intro/scene_0/",
            "graphics/intro/scene_1/",
            "graphics/intro/scene_2/",
            "graphics/intro/scene_3/",
        ],
        "additional": ["src/intro.c", "src/intro_credits_graphics.c", "src/expansion_intro.c", "include/intro.h", "CREDITS.md"],
        "note": "Original pre-intro Scene 0 with Celebi/Jirachi/comet/shrine and subsequent altered Emerald opening scenes. Native assets and C source.",
    },
]


def get(url):
    for attempt in range(3):
        headers = {"User-Agent": "PRL-donor-archiver", "Accept": "application/vnd.github+json"}
        if TOKEN and url.startswith("https://api.github.com/"):
            headers["Authorization"] = f"Bearer {TOKEN}"
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, timeout=55) as response:
                return response.read()
        except (urllib.error.URLError, TimeoutError):
            if attempt == 2:
                raise
            time.sleep(2 * (attempt + 1))


result = []
for source in SOURCES:
    owner_repo = source["repo"]
    commit = source["commit"]
    tree_url = f"https://api.github.com/repos/{owner_repo}/git/trees/{commit}?recursive=1"
    tree = json.loads(get(tree_url))
    if tree.get("truncated"):
        raise RuntimeError(f"GitHub tree truncated for {owner_repo}; refusing partial export")
    candidates = {
        item["path"]: item
        for item in tree["tree"]
        if item.get("type") == "blob" and (
            any(item["path"].startswith(prefix) for prefix in source["prefixes"])
            or item["path"] in source["additional"]
        )
        and item["path"].lower().endswith((".c", ".h", ".md", ".png", ".pal", ".bin"))
    }
    for prefix in source["prefixes"]:
        assert any(path.startswith(prefix) for path in candidates), f"Missing prefix {prefix}"
    assert "src/intro.c" in candidates
    assert all(item["size"] < 3_000_000 for item in candidates.values())
    source_manifest = {
        "repo": owner_repo,
        "commit": commit,
        "source_url": f"https://github.com/{owner_repo}/tree/{commit}",
        "note": source["note"],
        "files": []
    }
    dst = DEST / (source["title"] + ".zip")
    with zipfile.ZipFile(dst, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path, item in sorted(candidates.items()):
            quoted = urllib.parse.quote(path, safe="/")
            url = f"https://raw.githubusercontent.com/{owner_repo}/{commit}/{quoted}"
            payload = get(url)
            git_blob = hashlib.sha1(b"blob " + str(len(payload)).encode() + b"\0" + payload).hexdigest()
            if git_blob != item["sha"]:
                raise RuntimeError(f"Source hash mismatch for {owner_repo}:{path}: {git_blob} != {item['sha']}")
            zf.writestr(source["title"] + "/" + path, payload)
            source_manifest["files"].append({
                "path": path,
                "bytes": len(payload),
                "github_git_blob": git_blob,
                "sha256": hashlib.sha256(payload).hexdigest()
            })
        zf.writestr(source["title"] + "/SOURCE_PROVENANCE.json", json.dumps(source_manifest, indent=2))
    print(f"{source['title']}: {len(candidates)} source files, {dst.stat().st_size} ZIP bytes")
    result.append({"zip": dst.name, "count": len(candidates), "commit": commit, "repo": owner_repo})

(DEST / "EXPORT_SUMMARY.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
print("EXPORT_COMPLETE")
