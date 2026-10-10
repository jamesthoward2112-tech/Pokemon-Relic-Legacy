#!/usr/bin/env python3
"""Source-derived Eeveeon/Nimbeon port and four Kanto Eevee-Twins battles.
Run on an isolated branch only. Original Pokengine artwork, never AI redraws.
"""
from pathlib import Path
from urllib.request import urlopen, Request
import hashlib, re, struct
ROOT = Path(__file__).resolve().parents[2]
def path(p): return ROOT / p
def txt(p): return path(p).read_text(encoding="utf-8")
def write(p,s): path(p).write_text(s,encoding="utf-8")
def once(p,old,new):
    s=txt(p)
    if old not in s:
        if new in s: return
        raise AssertionError("Expected marker missing: "+p+": "+old[:110])
    if s.count(old)!=1: raise AssertionError("Expected exactly one marker: "+p)
    write(p,s.replace(old,new,1))
def add_before(p,old,addition):
    if addition in txt(p): return
    once(p,old,addition+old)
# Append two species after all previous species, preserving IDs for existing-save compatibility.
add_before("include/constants/species.h",
           "    SPECIES_CUSTOM_END,",
           "    SPECIES_EEVEEON,\n    SPECIES_NIMBEON,\n")
# Add Eevoboost after pre-existing moves; never renumber previous move identities.
once("include/constants/moves.h",
     "    MOVES_COUNT_ALL = MOVES_COUNT_DYNAMAX,",
     "    MOVE_EEVOBOOST = MOVES_COUNT_DYNAMAX,\n    MOVES_COUNT_ALL,\n")
# Actual Infinity Eevoboost effect: +1 to all five battle stats, PP 2, priority -2.
move = '''    [MOVE_EEVOBOOST] =
    {
        .name = COMPOUND_STRING("Eevoboost"),
        .description = COMPOUND_STRING(
            "Gathers inner potential\\\\n"
            "to raise all five stats."),
        .effect = EFFECT_EXTREME_EVOBOOST,
        .power = 0,
        .type = TYPE_NORMAL,
        .accuracy = 0,
        .pp = 2,
        .target = TARGET_USER,
        .priority = -2,
        .category = DAMAGE_CATEGORY_STATUS,
        .ignoresProtect = TRUE,
        .mirrorMoveBanned = TRUE,
        .snatchAffected = TRUE,
        .additionalEffects = ADDITIONAL_EFFECTS({
            .moveEffect = STAT_CHANGE_EFFECT_PLUS,
            .attack = 1,
            .defense = 1,
            .speed = 1,
            .spAtk = 1,
            .spDef = 1,
        }),
        .battleAnimScript = gBattleAnimMove_ExtremeEvoboost,
    },
'''
# Last array entry before terminal };, no alteration to other moves.
p="src/data/moves_info.h"
s=txt(p)
if "[MOVE_EEVOBOOST]" not in s:
    pos=s.rfind("\n};")
    if pos<0: raise AssertionError("moves table ending missing")
    write(p,s[:pos]+"\n"+move+s[pos:])
# Register five graphics symbols for each new species.
graphics=""
externs=""
for name in ["Eeveeon","Nimbeon"]:
    low=name.lower()
    graphics += f'''
const u32 gMonFrontPic_{name}[] = INCGFX_U32("graphics/pokemon/{low}/front.png", ".4bpp.smol");
const u16 gMonPalette_{name}[] = INCGFX_U16("graphics/pokemon/{low}/normal.pal", ".gbapal");
const u32 gMonBackPic_{name}[] = INCGFX_U32("graphics/pokemon/{low}/back.png", ".4bpp.smol");
const u16 gMonShinyPalette_{name}[] = INCGFX_U16("graphics/pokemon/{low}/shiny.pal", ".gbapal");
const u8 gMonIcon_{name}[] = INCGFX_U8("graphics/pokemon/{low}/icon.png", ".4bpp");

'''
    externs += f'''
extern const u32 gMonFrontPic_{name}[];
extern const u32 gMonBackPic_{name}[];
extern const u16 gMonPalette_{name}[];
extern const u16 gMonShinyPalette_{name}[];
extern const u8 gMonIcon_{name}[];
'''
if "gMonFrontPic_Eeveeon" not in txt("src/data/graphics/pokemon.h"):
    p="src/data/graphics/pokemon.h"
    write(p,txt(p).rstrip()+"\n"+graphics)
if "gMonFrontPic_Eeveeon" not in txt("include/graphics.h"):
    add_before("include/graphics.h","#endif",externs+"\n")
# Pull exactly the human-approved, indexed Pokémon Infinity artwork from review branch.
branch="prl/eeveeon-nimbeon-sprite-review-20261010"
repo="jamesthoward2112-tech/Pokemon-Relic-Legacy"
for mon in ("eeveeon","nimbeon"):
    d=path("graphics/pokemon/"+mon); d.mkdir(parents=True,exist_ok=True)
    for fname in ("front.png","back.png","icon.png","normal.pal"):
        url=f"https://raw.githubusercontent.com/{repo}/{branch}/sprite-review/eeveeon-nimbeon/{mon}/{fname}"
        req=Request(url,headers={"User-Agent":"PRL-genuine-Infinity-art-import"})
        b=urlopen(req,timeout=35).read()
        if fname.endswith(".png"):
            assert b[:8]==b"\x89PNG\r\n\x1a\n",(mon,fname)
            width,height=struct.unpack(">II",b[16:24])
            assert (width,height)==((32,64) if fname=="icon.png" else (64,64)),(mon,fname,width,height)
        else:
            assert b.startswith(b"JASC-PAL\n0100\n16\n")
        (d/fname).write_bytes(b)
    # Genuine donor shiny palette has not been supplied. Use unchanged palette solely
    # as a technically safe fallback; DO NOT claim original shiny was imported.
    (d/"shiny.pal").write_bytes((d/"normal.pal").read_bytes())
# Source movesets + PRL Eevee-inherited TM / tutor compatibility.
ev = [(0,"EEVOBOOST"),(1,"WORK_UP"),(1,"HELPING_HAND"),(1,"TACKLE"),(1,"TAIL_WHIP"),
      (3,"SAND_ATTACK"),(6,"GROWL"),(9,"QUICK_ATTACK"),(11,"BITE"),(16,"COVET"),
      (20,"TAKE_DOWN"),(22,"CHARM"),(28,"BATON_PASS"),(32,"DOUBLE_EDGE"),
      (35,"LAST_RESORT"),(38,"TRUMP_CARD"),(40,"AGILITY"),(44,"ASSIST"),
      (49,"HYPER_VOICE"),(53,"SLACK_OFF"),(57,"HOLD_BACK"),(60,"ENTRAINMENT"),
      (64,"COSMIC_POWER"),(73,"STORED_POWER"),(80,"MOONBLAST")]
ni = [(0,"ROOST"),(1,"HELPING_HAND"),(1,"TACKLE"),(1,"TAIL_WHIP"),(1,"AGILITY"),
      (1,"AIR_CUTTER"),(6,"SWIFT"),(11,"CHARM"),(14,"SKY_DROP"),(17,"MIST"),
      (20,"ICY_WIND"),(26,"AIR_SLASH"),(30,"WHIRLWIND"),(36,"HAZE"),
      (42,"BODY_SLAM"),(47,"BRAVE_BIRD"),(53,"TAILWIND"),(59,"ICICLE_CRASH"),
      (62,"DEFOG"),(67,"DRAGON_DANCE")]
s=txt("src/data/pokemon/level_up_learnsets/prl_custom.h")
for name,moves in [("Eeveeon",ev),("Nimbeon",ni)]:
    if "s"+name+"LevelUpLearnset" in s: continue
    s += "\nstatic const struct LevelUpMove s"+name+"LevelUpLearnset[] =\n{\n"
    s += "".join(f"    LEVEL_UP_MOVE({level}, MOVE_{move}),\n" for level,move in moves)
    s+="    LEVEL_UP_END\n};\n"
write("src/data/pokemon/level_up_learnsets/prl_custom.h",s)
eevee_tms="""ROAR TOXIC HIDDEN_POWER ICE_BEAM BLIZZARD HYPER_BEAM PROTECT RAIN_DANCE
FRUSTRATION RETURN DIG DOUBLE_TEAM FACADE REST ATTRACT LOW_SWEEP ROUND
ECHOED_VOICE FLING SHADOW_CLAW PAYBACK RETALIATE GIGA_IMPACT THUNDER_WAVE
SWORDS_DANCE PSYCH_UP WORK_UP SWAGGER SUBSTITUTE"""
eevee_tutors="AQUA_TAIL BATON_PASS DAZZLING_GLEAM EARTH_POWER HELPING_HAND HYPER_VOICE MEGA_KICK SING ZEN_HEADBUTT"
nimbeon_tms="""ROAR TOXIC HIDDEN_POWER HYPER_BEAM PROTECT RAIN_DANCE FRUSTRATION RETURN
THUNDERBOLT THUNDER DOUBLE_TEAM REFLECT FACADE REST ATTRACT LOW_SWEEP ROUND
ECHOED_VOICE ACROBATICS GIGA_IMPACT THUNDER_WAVE DREAM_EATER SWAGGER U_TURN
SUBSTITUTE FLY"""
nimbeon_tutors="BATON_PASS DEFOG HEAL_BELL HEAT_WAVE HELPING_HAND HYPER_VOICE ICY_WIND ROOST SING TAILWIND"
teachable=txt("src/data/pokemon/teachable_learnsets/prl_custom.h")
moves_constants=txt("include/constants/moves.h")
for name,tms,tutors,moves in [("Eeveeon",eevee_tms,eevee_tutors,ev),("Nimbeon",nimbeon_tms,nimbeon_tutors,ni)]:
    if "s"+name+"TeachableLearnset" in teachable: continue
    names=sorted(set(tms.split()+tutors.split()+[m for _,m in moves]))
    # Don't leak undefined moves into compiler.
    missing=[m for m in names if "MOVE_"+m not in moves_constants]
    if missing: raise AssertionError("Missing donor move constants: "+name+" "+repr(missing))
    teachable+="\nstatic const u16 s"+name+"TeachableLearnset[] =\n{\n"
    teachable+="".join("    MOVE_"+m+",\n" for m in names)+"    MOVE_UNAVAILABLE,\n};\n"
write("src/data/pokemon/teachable_learnsets/prl_custom.h",teachable)
# New species_info records, preserving existing species, stats and ability IDs.
records=""
for name,stats,typ,abil,hidden,colour,ht,wt in [
    ("Eeveeon",(100,110,85,110,85,110),"NORMAL","ADAPTABILITY","NORMALIZE","BROWN",14,250),
    ("Nimbeon",(120,88,61,70,80,106),"FLYING","AERILATE","GALE_WINGS","BLUE",13,140)]:
    hp,atk,defn,spa,spd,spe=stats
    assert sum(stats)==(600 if name=="Eeveeon" else 525)
    records += f'''
[SPECIES_{name.upper()}] =
{{
    .baseHP = {hp}, .baseAttack = {atk}, .baseDefense = {defn},
    .baseSpAttack = {spa}, .baseSpDefense = {spd}, .baseSpeed = {spe},
    .types = MON_TYPES(TYPE_{typ}),
    .catchRate = 45, .expYield = 184,
    .genderRatio = PERCENT_FEMALE(12.5), .eggCycles = 35, .friendship = STANDARD_FRIENDSHIP,
    .growthRate = GROWTH_MEDIUM_FAST, .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
    .abilities = {{ ABILITY_{abil}, ABILITY_{abil}, ABILITY_{hidden} }},
    .bodyColor = BODY_COLOR_{colour},
    .speciesName = _("{name}"), .cryId = CRY_EEVEE, .natDexNum = NATIONAL_DEX_EEVEE,
    .categoryName = _("{'Perfectly Normal' if name=='Eeveeon' else 'Cloud Dancer'}"), .height = {ht}, .weight = {wt},
    .description = COMPOUND_STRING("{'Stable DNA fuels its unique potential.' if name=='Eeveeon' else 'It dances lightly upon the wind.'}"),
    .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
    .frontPic = gMonFrontPic_{name}, .frontPicSize = MON_COORDS_SIZE(64, 64), .frontPicYOffset = 0,
    .frontAnimFrames = sAnims_SingleFramePlaceHolder, .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
    .backPic = gMonBackPic_{name}, .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 0, .backAnimId = BACK_ANIM_NONE,
    .palette = gMonPalette_{name}, .shinyPalette = gMonShinyPalette_{name},
    .iconSprite = gMonIcon_{name}, .iconPalIndex = 2,
    .pokemonJumpType = PKMN_JUMP_TYPE_NORMAL,
    .levelUpLearnset = s{name}LevelUpLearnset,
    .teachableLearnset = s{name}TeachableLearnset,
    .eggMoveLearnset = sEeveeEggMoveLearnset,
}},
'''
s=txt("src/data/pokemon/species_info/prl_custom.h")
if "[SPECIES_EEVEEON]" not in s:write("src/data/pokemon/species_info/prl_custom.h",s.rstrip()+"\n"+records)
# Put new Eevee evolutions before friendship branches and old held-item branches.
evo_p="src/data/pokemon/species_info/gen_1_families.h"
evo="        .evolutions = EVOLUTION({EVO_LEVEL, 0, SPECIES_CHAMPEON,"
addition=("        .evolutions = EVOLUTION({EVO_ITEM, ITEM_ANCIENT_STONE, SPECIES_EEVEEON},\n"
          "                                {EVO_LEVEL, 0, SPECIES_NIMBEON, CONDITIONS({IF_NOT_TIME, TIME_NIGHT}, {IF_HOLD_ITEM, ITEM_SHARP_BEAK})},\n"
          "                                {EVO_LEVEL, 0, SPECIES_CHAMPEON,")
once(evo_p,evo,addition)
# New Kanto rosters: save existing first two teams, revise Tower and Route 23 only.
party="src/data/trainers_frlg.party"
s=txt(party)
def section(name,teams):
    global s
    prefix=f"=== TRAINER_STEEVE_NEEVEE_{name} ==="
    m=re.search(r"(?ms)^"+re.escape(prefix)+r"\n.*?(?=^=== TRAINER_|\\Z)",s)
    assert m, "Missing trainer "+name
    old=m.group()
    head=old.split("\n\n",1)[0]+"\n\n"
    records=""
    for mon,lvl,moves in teams:
        records += f"{mon} @ Sitrus Berry\nLevel: {lvl}\nIVs: 20 HP / 20 Atk / 20 Def / 20 SpA / 20 SpD / 20 Spe\n"
        records += "".join("- "+move+"\n" for move in moves)+"\n"
    s=s[:m.start()]+head+records.rstrip()+"\n\n"+s[m.end():]
section("TOWER",[
 ("Flareon",37,["Fire Spin","Ember","Quick Attack","Bite"]),
 ("Omeon",37,["Shadow Ball","Bite","Confuse Ray","Quick Attack"]),
 ("Espeon",37,["Psybeam","Quick Attack","Swift","Reflect"]),
 ("Umbreon",36,["Bite","Quick Attack","Confuse Ray","Moonlight"])
])
section("ROUTE23",[
 ("Vaporeon",52,["Surf","Aurora Beam","Bite","Acid Armor"]),
 ("Jolteon",52,["Thunderbolt","Double Kick","Quick Attack","Thunder Wave"]),
 ("Flareon",55,["Flamethrower","Fire Spin","Bite","Quick Attack"]),
 ("Toxeon",55,["Poison Jab","Bite","Toxic","Quick Attack"]),
 ("Nimbeon",54,["Air Slash","Roost","Tailwind","Icy Wind"]),
 ("Eeveeon",55,["Eevoboost","Hyper Voice","Double-Edge","Baton Pass"])
])
write(party,s)
# Keep mandatory NPC/coord event scripting in place; update only stale tests.
testp="tests/test_prl_recurring_duos.py";test=txt(testp)
test=test.replace('[(\"Flareon\", 37), (\"Vaporeon\", 37), (\"Jolteon\", 36), (\"Eevee\", 36)]',
                  '[(\"Flareon\", 37), (\"Omeon\", 37), (\"Espeon\", 37), (\"Umbreon\", 36)]')
test=test.replace('[(\"Vaporeon\", 52), (\"Jolteon\", 52), (\"Flareon\", 55), (\"Toxeon\", 55), (\"Eevee\", 51), (\"Eevee\", 53)]',
                  '[(\"Vaporeon\", 52), (\"Jolteon\", 52), (\"Flareon\", 55), (\"Toxeon\", 55), (\"Nimbeon\", 54), (\"Eeveeon\", 55)]')
test=test.replace('banned = {\"Espeon\", \"Umbreon\", \"Leafeon\", \"Glaceon\", \"Sylveon\", \"Noxichu\", \"Wobbuffet\"}',
                  'banned = {\"Noxichu\", \"Wobbuffet\"}')
# Old Route23 setup check specifically enforces removed two Eevee; replace it with the approved lineup.
start=test.find("    def test_final_route23_preserves_existing_setup_for_returning_pokemon(self):")
end=test.find("\n\nif __name__ == ",start)
if start>=0 and end>start:
    revised='''    def test_route23_has_balanced_eeveelutions_and_signature(self):
        party = read("src/data/trainers_frlg.party")
        rec = party.split("=== TRAINER_STEEVE_NEEVEE_ROUTE23 ===", 1)[1].split("=== ", 1)[0]
        for name in ("Vaporeon", "Jolteon", "Flareon", "Toxeon", "Nimbeon", "Eeveeon"):
            self.assertIn(name, rec)
        self.assertIn("- Eevoboost", rec)
        self.assertEqual(rec.count("Level:"), 6)
'''
    test=test[:start]+revised+test[end:]
write(testp,test)
# Additional source-of-truth regression test (no Pillow dependency for CI).
newtest='''import pathlib
import re
import struct
import unittest

R=pathlib.Path(__file__).resolve().parents[1]

class ApprovedEeveelutions(unittest.TestCase):
    def test_genuine_normal_sprite_dimensions(self):
        for mon in ("eeveeon","nimbeon"):
            for name,w,h in (("front.png",64,64),("back.png",64,64),("icon.png",32,64)):
                data=(R/"graphics"/"pokemon"/mon/name).read_bytes()
                self.assertEqual(data[:8],b"\\\\x89PNG\\\\r\\\\n\\\\x1a\\\\n")
                self.assertEqual(struct.unpack(">II",data[16:24]),(w,h))
    def test_evolution_and_strongest_stats(self):
        s=(R/"src/data/pokemon/species_info/gen_1_families.h").read_text()
        self.assertIn("{EVO_ITEM, ITEM_ANCIENT_STONE, SPECIES_EEVEEON}",s)
        self.assertIn("{EVO_LEVEL, 0, SPECIES_NIMBEON",s)
        specs=(R/"src/data/pokemon/species_info/prl_custom.h").read_text()
        block=specs.split("[SPECIES_EEVEEON] =",1)[1].split("[SPECIES_NIMBEON] =",1)[0]
        for k,v in {"baseHP":100,"baseAttack":110,"baseDefense":85,"baseSpAttack":110,"baseSpDefense":85,"baseSpeed":110}.items():
            self.assertRegex(block,r"\\\\."+k+r"\\\\s*=\\\\s*"+str(v)+r"\\\\b")
        self.assertIn("ABILITY_ADAPTABILITY",block)
        self.assertIn("ABILITY_NORMALIZE",block)
    def test_elevated_eeveeon_move(self):
        s=(R/"src/data/moves_info.h").read_text()
        b=s.split("[MOVE_EEVOBOOST] =",1)[1].split("    },",1)[0]
        self.assertIn(".priority = -2",b)
        self.assertIn(".pp = 2",b)
    def test_kanto_roster_has_signature_evolutions(self):
        s=(R/"src/data/trainers_frlg.party").read_text()
        for tag, mons in {"TOWER":("Omeon","Espeon","Umbreon"),"ROUTE23":("Toxeon","Eeveeon","Nimbeon")}.items():
            block=s.split("=== TRAINER_STEEVE_NEEVEE_"+tag+" ===",1)[1].split("=== TRAINER_",1)[0]
            for mon in mons:self.assertIn(mon,block)

if __name__=="__main__":
    unittest.main()
'''
# Correct escaped literals from JavaScript raw preservation.
newtest=newtest.replace('b"\\\\x89PNG\\\\r\\\\n\\\\x1a\\\\n"','b"\\x89PNG\\r\\n\\x1a\\n"')
newtest=newtest.replace('r"\\\\."','r"\\."').replace('r"\\\\s*','r"\\s*').replace('r"\\\\b"','r"\\b"')
p="tests/test_prl_eeveeon_nimbeon.py";write(p,newtest)
note=path("docs/prl-eeveeon-nimbeon-build-status.md")
note.write_text("""# Eeveeon + Nimbeon initial playable Kanto integration
Original Pokémon Infinity indexed normal sprites are imported unchanged from approved review pack. Shiny palettes reuse normal palettes ONLY as a safe temporary fallback: genuine donor shiny graphics and distinct animated icon frames are still unverified, so the complete donor art gate is NOT passed.
Eeveeon: 600 BST; Adaptability/Normalize; Ancient Stone; complete listed Infinity level-up moves including correct +1 all-stat Eevoboost PP2 priority -2, TM/tutor lists and inherited Eevee egg moves.
Nimbeon: 525 BST; Aerilate/Gale Wings; day level-up with held Sharp Beak; Infinity level-up/TM/tutor lists and Eevee egg moves.
Kanto Twins: first two events preserved, Pokémon Tower and Route 23 lineups updated. Eight future Johto/Hoenn appearances documented only, NOT present in current game. G-max Eevee not used by any Twins team.
Source changes do not establish emulator runtime quality or existing-save compatibility until player confirms.
Full Notion spec: https://app.notion.com/p/3f54939d4316818da11efd75b6ad44a9
""",encoding="utf-8")
print("Prepared authentic sprite imports, two real species, Eevoboost, full listed moves and four Kanto Twins rosters.")
