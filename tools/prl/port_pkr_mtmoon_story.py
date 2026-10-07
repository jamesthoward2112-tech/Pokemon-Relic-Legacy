from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[2]
target = ROOT / "data/maps/MtMoon_B2F_Frlg/scripts.inc"
text = target.read_text(encoding="utf-8")

replacements = {
'''MtMoon_B2F_Text_WellEachTakeAFossil::
\t.string "We'll each take a fossil!\\n"
\t.string "No being greedy!$"''':
'''MtMoon_B2F_Text_WellEachTakeAFossil::
\t.string "TEAM ROCKET almost took both!\\p"
\t.string "We'll each take a fossil.\\n"
\t.string "No being greedy!$"''',

'''MtMoon_B2F_Text_Grunt1Intro::
\t.string "We, TEAM ROCKET, shall find the\\n"
\t.string "fossils!\\p"
\t.string "Reviving POKÃ©MON from them will\\n"
\t.string "earn us huge riches!$"

MtMoon_B2F_Text_Grunt1Defeat::
\t.string "Urgh!\\n"
\t.string "Now I'm mad!$"

MtMoon_B2F_Text_Grunt1PostBattle::
\t.string "You made me mad!\\n"
\t.string "TEAM ROCKET will blacklist you!$"''':
'''MtMoon_B2F_Text_Grunt1Intro::
\t.string "We, TEAM ROCKET, are taking\\n"
\t.string "every fossil in MT. MOON!\\p"
\t.string "BOSS wants the whole dig\\n"
\t.string "catalogued. Hand it over!$"

MtMoon_B2F_Text_Grunt1Defeat::
\t.string "The excavationâ€¦!$"

MtMoon_B2F_Text_Grunt1PostBattle::
\t.string "This was supposed to be a simple\\n"
\t.string "recovery job!\\p"
\t.string "Fossils, stones, old relicsâ€¦\\n"
\t.string "BOSS wants all of it.$"''',

'''MtMoon_B2F_Text_Grunt2Intro::
\t.string "We, TEAM ROCKET, are POKÃ©MON\\n"
\t.string "gangsters!\\l"
\t.string "We strike fear with our strength!$"

MtMoon_B2F_Text_Grunt2Defeat::
\t.string "I blew it!$"

MtMoon_B2F_Text_Grunt2PostBattle::
\t.string "Darn it all!\\n"
\t.string "My associates won't stand for this!$"''':
'''MtMoon_B2F_Text_Grunt2Intro::
\t.string "This cave is TEAM ROCKET's\\n"
\t.string "recovery site!\\p"
\t.string "Anything ancient here belongs\\n"
\t.string "to BOSS!$"

MtMoon_B2F_Text_Grunt2Defeat::
\t.string "The relicsâ€¦!$"

MtMoon_B2F_Text_Grunt2PostBattle::
\t.string "MT. MOON is only the beginning.\\n"
\t.string "We're searching all over KANTO.$"''',

'''MtMoon_B2F_Text_Grunt3Intro::
\t.string "We're pulling a big job here!\\n"
\t.string "Get lost, kid!$"

MtMoon_B2F_Text_Grunt3Defeat::
\t.string "So, you are goodâ€¦$"

MtMoon_B2F_Text_Grunt3PostBattle::
\t.string "If you find a fossil, give it to me\\n"
\t.string "and scram!$"''':
'''MtMoon_B2F_Text_Grunt3Intro::
\t.string "We're pulling a big job here!\\p"
\t.string "The fossils go straight to our\\n"
\t.string "research team. Get lost, kid!$"

MtMoon_B2F_Text_Grunt3Defeat::
\t.string "You ruined the dig!$"

MtMoon_B2F_Text_Grunt3PostBattle::
\t.string "You think this is about money?\\n"
\t.string "BOSS is hunting something older.$"''',

'''MtMoon_B2F_Text_Grunt4Intro::
\t.string "Little kids shouldn't be messing\\n"
\t.string "around with grown-ups!\\p"
\t.string "It could be bad news!$"

MtMoon_B2F_Text_Grunt4Defeat::
\t.string "I'm steamed!$"

MtMoon_B2F_Text_Grunt4PostBattle::
\t.string "POKÃ©MON lived here long before\\n"
\t.string "people came.$"''':
'''MtMoon_B2F_Text_Grunt4Intro::
\t.string "Keep away from the specimens!\\p"
\t.string "Some of what we found predates\\n"
\t.string "KANTO's towns by centuries!$"

MtMoon_B2F_Text_Grunt4Defeat::
\t.string "The specimens!$"

MtMoon_B2F_Text_Grunt4PostBattle::
\t.string "If BOSS learns you wrecked our\\n"
\t.string "MT. MOON operation, we're done!$"'''
}

for old, new in replacements.items():
    count = text.count(old)
    if count != 1:
        raise SystemExit(f"Safety check failed: expected one match, got {count} for block starting {old.splitlines()[0]}")
    text = text.replace(old, new)

target.write_text(text, encoding="utf-8", newline="\n")

manifest_path = ROOT / "tools/prl/pkr_port_manifest.json"
manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
layer = next(x for x in manifest["layers"] if x["id"] == "pkr_story_kanto")
ported = layer.setdefault("ported", [])
entry = "Mt. Moon Rocket relic/fossil story pass: four B2F grunts + Miguel fossil handoff reauthored to establish Rocket excavation, cataloguing and BOSS continuity without changing FireRed geography or progression."
if entry not in ported:
    ported.append(entry)
layer["status"] = "in_progress"
manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

print("Applied PKR Mt. Moon story layer safely.")
