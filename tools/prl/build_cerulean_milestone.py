from pathlib import Path
import base64, json, re, struct, zlib, binascii

ROOT = Path(__file__).resolve().parents[2]

def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8")

def write(rel, text):
    p = ROOT / rel
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(text, encoding="utf-8", newline="\n")

def replace_once(rel, old, new):
    s = read(rel)
    n = s.count(old)
    if n != 1:
        raise SystemExit(f"{rel}: expected 1 match, got {n}: {old[:80]!r}")
    write(rel, s.replace(old, new, 1))

def replace_regex_once(rel, pattern, repl, flags=re.S):
    s = read(rel)
    out, n = re.subn(pattern, repl, s, count=1, flags=flags)
    if n != 1:
        raise SystemExit(f"{rel}: regex expected 1 match, got {n}: {pattern[:100]!r}")
    write(rel, out)

def insert_once(rel, marker, text, before=False):
    s = read(rel)
    if text.strip() in s:
        return
    if s.count(marker) != 1:
        raise SystemExit(f"{rel}: marker count {s.count(marker)} for {marker!r}")
    if before:
        s = s.replace(marker, text + marker, 1)
    else:
        s = s.replace(marker, marker + text, 1)
    write(rel, s)

# ---------------------------------------------------------------------------
# Noxichu exact approved raster assets. These are reconstructed from the
# approved indexed pixels/palette rather than from donor ROM offsets.
# ---------------------------------------------------------------------------
PAL_B64 = "AAAAc1ogMSAYEBAQ9sUgnHMgizEQUkEgzZwgvUoYOTE5ICApKSkxUkpK3s2kGBgY"
ASSETS = {
    "front.png": (64, 64, "eNrNV+1y6yoMjMAcUQTh/d/27AqT2DHJ7dz8Oe5Mp029+lhJK/V2+9eeUr6CN+1fwSX2r+A92jdw+wZfpAK/jL/VX5iVUpf+i2j9BS0qtvJfBPAe/7usqdbaL/jWtJr9wn2C+4v/KsgcZmM9JZrzNfpcrv4bQsdzdp/ytl3xKbv7E76l4vAjqQAn2a4cb7la0RO+Za0eVOzlgA59EX7bsumG7n3i+dGAT/eONr53wect65bx6iPUtm1k1CktO1qMSS7d80nxiecn9G579hNdbZH9LTleia8PeEao0t19ynmg8Wzpyl4mPOPdPX2PRyxtZNRoX+KAI/urAbye8UqfVI10JCOjndCEH/cANF0s4P2atjCrVwHUPCOy2zTQhwXD3/KpBLKpCV6ezrIyTBJyqCcj6L0+LBzpS2byKH5LRpZzTud2HBGsaECbm5L9PmTEi6w54IN6jFIm3ipm6IR/zo7/UlJEQPGsJrJXAcHmEwGiiJju1d1714YLPLN72ZB6Rg88mA4iknSMbNeAbM9w7W445dcGEOGYBIGBCQ/hrCSonzd/l5wW2sfwwUoxL5A3or10OMnrYTE8xJe9slQrphKv72QUuIdtjdde6v4Y4b2uDcQ3BgLSLpCbMoIH8+WyiYUd/iYC6GwliyMCakavL/ic1HlJiwEWgeckxxRe+IO+M64gaUG/CbPXBwUuuVbmXlLBtzDQsqaPtE94H6JnTiH8+YN26JLCm9X39Fy4bMBh8Y2LhsIPCAWOIzpP13jrZbcgJfoiK75xVYpNSjlOYRl+KxbCwMNMuScmwBawUJ9PF31noFhr3j8Gr3pv+yabgjeY4USBxWX41hIHm/0df+7sA2/C2CdcXL/lDX2gKP3pfj38+flpWkYLU7ofYoXh73p7i6/KXWfkekiJw/uO5/BRn+Xd8cCxc20xG+PXx9p3tYIYbqgDxEvDh7sV2XfcQO48Dsrx4Bv9qxnkOIQP11uNcr8XR8+8mQXLoGO7989XUKxtOKeWlIEfwRj3Kw6MlSocunh/vTNtNqSPcfdOguoph+rDGafqY+tJs5EHHj0XbDDr18VbA6ZjbEfdCo9JczRIw4fmHRnfl9A7zNmK3ueFlrwLfcD21KK+McCrjtueKzL68qXc3uZF4daYXVjjsYGwlYLrLkY40VzoNwbzKBpaAcks8a3Vgt2QRoF8g/JkgfrIQQfVyxjW6otqS/J3FcZAJYQSYwebhxz5Jdf7vo0rM+x66bWPosCno9yimVHK8vpPC5YP1+8M1OHIFb/2cGp5CAgX/RmeMdkIctJk0FJvHVscibh6sMXry+XISyuM9yvWv/JIWcBvfpHhqDpTR6s8qucZzEtQnxf3aUmBvPyyA3pM+tgLhba8kdaDRj4+zXBIie6xqP/v/4BBFDvn16//BVHZPjk="),
    "back.png": (64, 64, "eNrNV9G24yAIrDEcKeD1//92AU2iSWryuD611YEBYbCfz3+3cv69RfkZL/LzUJYXBoh/HiJ+YWBySFiEHvkX+WEgM/Mkut2/HpK7LOZSWB4jIDt0eyxH33oy4F4kh3xjmfktngHz2T07/smA4v3UipfwdVlsMMVzxQuscMFXAgHnF8geQVhHA9nhtpPW/AIvuA4psPQ3aivkKb66wXU41/zXnQk+qxfLE2sAa+jx8bAMU3wpC2IwfO+IDF/SNbJzl/oxA68BYUiAMgPwyGYBWKDRvZOkfLrAtCLhOg2gBoqOp3z6vSh53ZkEUAPlRY+Fvloz++9ObIrP0Q9qmGEQE3H/ZvcBz7VUEoL0ilM7QAlgmiZAS8gJlIQ0tFtxYsuKlsXPnIAzIBkIaGKMAKC2N8x74Ci2PoOxXiFqD02b2K4qNiEQotMNsKaF4EEFnUEVvENzW2LkEa9FbAaqlhHljH9/CEvh2EToSQWti2KM1UAJiBBCSNaZ1ejjHFID0ZZdmDajhaE97QT4zRyqDCyGBQOTLstlKa02X0xiFwJlAEk2uEiMNYUv8JpEZRtTEEdXA1xDos+rReoseQ1IW2wxveNfh0GRYTF7CUa9wH3G5t9vFqlSfqCtrYyBZWQ7ZHKY3+PtYu3LDvm6IOAn374ZTmDHswwVpEIFZuKmJ+2u+YT3+ulvEFYUCoDrH8LNm+WCH5vKCWhLsjiLv+4pmNt57p3bd9XF3oJKol8zU9CP+N3fQm3mXZaMCVACuGUpGQnYdYh/GRhyZROFdwtO4hCccmNF4pgoG3b7VTElrAakisi24vbxlD8ngH2h1NeLypD2C3c3X7aUnPA2lToDtK4tg9FffdaBJujKpj7DTvoFXoLUuf/ukktQJxYEJ2D+T3X29YkaDvbdeNHPCrWlKUkmHxfpPKHFZvd3lwAAarWXAEMs52fl94QWe3wc04166UjKgC7Ufcz2CtG5zySb9ln+l3QiYNwTDx0+uE9EHb72bj4TCEOPq/uDfR7bX2wGXbOPjYLDw+E+d8pdDUCIV+0E69+txpbjcZZ1bqWRPoV4p/0WRKtt9d+6L2sRH/5ty3vgViq/1oBeWyU2AxnRkt/En1PyyUO/RgcYhWoB7N1LGFxl2tJtvfzZ5NA8wlLbFNZUQurQUhtkibPJZ/8YWrtr3qTPPVmZaFwzAtBSYEsjYOrGXjB50a7UCKSD/APcZE2H"),
    "icon.png": (32, 64, "eNrtkzkOQzEIRJ0KPMVw/9sGvHzFm1InMg2SHwybnNJPGPmFW3P7OFQuduA05y8tbpcuATQ/8lAdeBbzt4yarkrFyL1/EiXdqVHH8k68dlWPYlBO7T24BjBj4IE7FzEKBl7Ue4C6PubxSwPsyxIs+0FwgKHFFbcBvLI7zAus/YeJx2F3387lcOrGDacTe65+TLlcmMnKiFv9svB6XmwD/A1Aunb///3/9///jb0BwMoYrw=="),
}

def png_chunk(kind, payload):
    return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", binascii.crc32(kind + payload) & 0xFFFFFFFF)

def write_indexed_png(path, w, h, compressed_indices):
    indices = zlib.decompress(base64.b64decode(compressed_indices))
    assert len(indices) == w * h
    palette = base64.b64decode(PAL_B64)
    assert len(palette) == 48
    # PNG indexed image with exact source pixels, 16-entry palette and index 0 transparent.
    raw = b"".join(b"\x00" + indices[y*w:(y+1)*w] for y in range(h))
    data = b"\x89PNG\r\n\x1a\n"
    data += png_chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 4, 3, 0, 0, 0))
    data += png_chunk(b"PLTE", palette)
    data += png_chunk(b"tRNS", bytes([0] + [255] * 15))
    data += png_chunk(b"IDAT", zlib.compress(raw, 9))
    data += png_chunk(b"IEND", b"")
    path.write_bytes(data)

gfx_dir = ROOT / "graphics/pokemon/noxichu"
gfx_dir.mkdir(parents=True, exist_ok=True)
for name, (w, h, comp) in ASSETS.items():
    write_indexed_png(gfx_dir / name, w, h, comp)

pal_lines = [
    "JASC-PAL", "0100", "16",
    "0 0 0", "115 90 32", "49 32 24", "16 16 16",
    "246 197 32", "156 115 32", "139 49 16", "82 65 32",
    "205 156 32", "189 74 24", "57 49 57", "32 32 41",
    "41 41 49", "82 74 74", "222 205 164", "24 24 24",
]
(gfx_dir / "normal.pal").write_text("\n".join(pal_lines) + "\n", encoding="ascii")
(gfx_dir / "shiny.pal").write_text("\n".join(pal_lines) + "\n", encoding="ascii")
icon_pal = ROOT / "graphics/pokemon/icon_palettes/pal6.pal"
icon_pal.write_text("\n".join(pal_lines) + "\n", encoding="ascii")

# ---------------------------------------------------------------------------
# Fresh canonical PRL species ID. Never reuse PKR donor numeric IDs.
# ---------------------------------------------------------------------------
replace_once(
    "include/constants/species.h",
    "    SPECIES_CUSTOM_START = SPECIES_GLIMMORA_MEGA,\n    // Add any custom species between here and SPECIES_CUSTOM_END\n    SPECIES_CUSTOM_END,",
    "    SPECIES_CUSTOM_START = SPECIES_GLIMMORA_MEGA,\n    // PRL canonical custom species. Donor numeric IDs are never reused here.\n    SPECIES_NOXICHU,\n    SPECIES_CUSTOM_END,"
)

# Graphics symbols.
gfx_header = "src/data/graphics/pokemon.h"
gfx_text = read(gfx_header)
if "gMonFrontPic_Noxichu" not in gfx_text:
    gfx_text += """
// PRL â€” Noxichu (approved PKR direct sprite set)
const u32 gMonFrontPic_Noxichu[] = INCGFX_U32("graphics/pokemon/noxichu/front.png", ".4bpp.smol");
const u16 gMonPalette_Noxichu[] = INCGFX_U16("graphics/pokemon/noxichu/normal.pal", ".gbapal");
const u32 gMonBackPic_Noxichu[] = INCGFX_U32("graphics/pokemon/noxichu/back.png", ".4bpp.smol");
const u16 gMonShinyPalette_Noxichu[] = INCGFX_U16("graphics/pokemon/noxichu/shiny.pal", ".gbapal");
const u8 gMonIcon_Noxichu[] = INCGFX_U8("graphics/pokemon/noxichu/icon.png", ".4bpp");
"""
    write(gfx_header, gfx_text)

# Make the new graphic symbols visible to species data.
graphics_h = "include/graphics.h"
gh = read(graphics_h)
if "gMonFrontPic_Noxichu" not in gh:
    marker = "// PokÃ©mon gfx\n"
    decl = """// PRL custom PokÃ©mon gfx
extern const u32 gMonFrontPic_Noxichu[];
extern const u32 gMonBackPic_Noxichu[];
extern const u16 gMonPalette_Noxichu[];
extern const u16 gMonShinyPalette_Noxichu[];
extern const u8 gMonIcon_Noxichu[];

"""
    if marker not in gh:
        raise SystemExit("include/graphics.h PokÃ©mon gfx marker not found")
    write(graphics_h, gh.replace(marker, marker + decl, 1))

# Add an exact seventh icon palette instead of altering any shared species palette.
replace_once(
    "src/graphics.c",
    '    INCGFX_U16("graphics/pokemon/icon_palettes/pal5.pal", ".gbapal"),\n};',
    '    INCGFX_U16("graphics/pokemon/icon_palettes/pal5.pal", ".gbapal"),\n'
    '    INCGFX_U16("graphics/pokemon/icon_palettes/pal6.pal", ".gbapal"),\n};'
)
replace_once(
    "src/pokemon_icon.c",
    "    { gMonIconPalettes[5], POKE_ICON_BASE_PAL_TAG + 5 },\n};",
    "    { gMonIconPalettes[5], POKE_ICON_BASE_PAL_TAG + 5 },\n"
    "    { gMonIconPalettes[6], POKE_ICON_BASE_PAL_TAG + 6 },\n};"
)

# Noxichu level-up learnset from the latest locked PKR master row.
custom_learn = ROOT / "src/data/pokemon/level_up_learnsets/prl_custom.h"
custom_learn.write_text("""// PRL custom species level-up learnsets.
static const struct LevelUpMove sNoxichuLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 0, MOVE_BITE),
    LEVEL_UP_MOVE(24, MOVE_SNARL),
    LEVEL_UP_MOVE(28, MOVE_VOLT_SWITCH),
    LEVEL_UP_MOVE(30, MOVE_ELECTROWEB),
    LEVEL_UP_MOVE(32, MOVE_NIGHT_SLASH),
    LEVEL_UP_MOVE(36, MOVE_CRUNCH),
    LEVEL_UP_MOVE(40, MOVE_AGILITY),
    LEVEL_UP_MOVE(44, MOVE_THUNDERBOLT),
    LEVEL_UP_MOVE(48, MOVE_NASTY_PLOT),
    LEVEL_UP_MOVE(52, MOVE_SUCKER_PUNCH),
    LEVEL_UP_MOVE(56, MOVE_DISCHARGE),
    LEVEL_UP_MOVE(60, MOVE_DARK_PULSE),
    LEVEL_UP_END
};
""", encoding="utf-8", newline="\n")

pokemon_c = "src/pokemon.c"
pc = read(pokemon_c)
if 'data/pokemon/level_up_learnsets/prl_custom.h' not in pc:
    marker = '#include "data/pokemon/level_up_learnsets/gen_9.h"'
    if marker not in pc:
        raise SystemExit("gen9 learnset include not found")
    write(pokemon_c, pc.replace(marker, marker + '\n#include "data/pokemon/level_up_learnsets/prl_custom.h"', 1))

# Teachable union = Pikachu âˆª Umbreon, as locked for Noxichu.
teach_rel = "src/data/pokemon/teachable_learnsets.h"
teach = read(teach_rel)
def teach_moves(name):
    m = re.search(rf"static const u16 {name}\[\] = \{{(.*?)\n\}};", teach, re.S)
    if not m:
        raise SystemExit(f"Could not find {name}")
    return re.findall(r"\bMOVE_[A-Z0-9_]+\b", m.group(1))
moves = []
for mv in teach_moves("sPikachuTeachableLearnset") + teach_moves("sUmbreonTeachableLearnset"):
    if mv != "MOVE_UNAVAILABLE" and mv not in moves:
        moves.append(mv)
nox_teach = "static const u16 sNoxichuTeachableLearnset[] = {\n" + "".join(f"    {m},\n" for m in moves) + "    MOVE_UNAVAILABLE,\n};\n"
if "sNoxichuTeachableLearnset" not in teach:
    write(teach_rel, teach.rstrip() + "\n\n" + nox_teach)

# Noxichu SpeciesInfo. Unspecified breeding/capture internals inherit sensible
# Pikachu-family defaults; locked PRL battle mechanics are exact.
species_info_rel = "src/data/pokemon/species_info.h"
si = read(species_info_rel)
if "[SPECIES_NOXICHU]" not in si:
    block = r'''    [SPECIES_NOXICHU] =
    {
        .baseHP        = 70,
        .baseAttack    = 110,
        .baseDefense   = 70,
        .baseSpeed     = 125,
        .baseSpAttack  = 110,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_ELECTRIC, TYPE_DARK),
        .catchRate = 190,
        .expYield = 112,
        .evYield_Speed = 2,
        .itemRare = ITEM_SCOPE_LENS,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 10,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_FAIRY),
        .abilities = { ABILITY_INFILTRATOR, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Noxichu"),
        .cryId = CRY_PIKACHU,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("Night Mouse"),
        .height = 7,
        .weight = 210,
        .description = COMPOUND_STRING(
            "It prowls after sunset, storing electricity\n"
            "in the yellow rings across its body.\n"
            "From deep shadow it strikes so quickly\n"
            "that its target sees only a flash."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Noxichu,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Noxichu,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL,
        .palette = gMonPalette_Noxichu,
        .shinyPalette = gMonShinyPalette_Noxichu,
        .iconSprite = gMonIcon_Noxichu,
        .iconPalIndex = 6,
        .pokemonJumpType = PKMN_JUMP_TYPE_NORMAL,
        SHADOW(-1, 5, SHADOW_SIZE_M)
        FOOTPRINT(Pikachu)
        .levelUpLearnset = sNoxichuLevelUpLearnset,
        .teachableLearnset = sNoxichuTeachableLearnset,
    },

'''
    marker = "    [SPECIES_EGG] =\n"
    if marker not in si:
        raise SystemExit("SPECIES_EGG SpeciesInfo marker not found")
    write(species_info_rel, si.replace(marker, block + marker, 1))

# Pikachu + Moon Stone -> Noxichu, preserving Raichu and Alolan Raichu paths.
pik_rel = "src/data/pokemon/species_info/gen_1_families.h"
pik = read(pik_rel)
needle = "        .evolutions = EVOLUTION({EVO_ITEM, ITEM_THUNDER_STONE, SPECIES_RAICHU, CONDITIONS({IF_NOT_REGION, REGION_ALOLA})}"
if "SPECIES_NOXICHU" not in pik:
    if needle not in pik:
        raise SystemExit("Pikachu evolution marker not found")
    repl = "        .evolutions = EVOLUTION({EVO_ITEM, ITEM_MOON_STONE, SPECIES_NOXICHU},\n" \
           "                                {EVO_ITEM, ITEM_THUNDER_STONE, SPECIES_RAICHU, CONDITIONS({IF_NOT_REGION, REGION_ALOLA})}"
    write(pik_rel, pik.replace(needle, repl, 1))

# ---------------------------------------------------------------------------
# QoL locks for the Cerulean milestone.
# ---------------------------------------------------------------------------
replace_once("include/config/item.h", "#define I_REUSABLE_TMS          FALSE", "#define I_REUSABLE_TMS          TRUE")
replace_once("include/config/item.h", "#define I_EXP_SHARE_FLAG        0", "#define I_EXP_SHARE_FLAG        FLAG_UNUSED_0x026")
replace_once("include/config/item.h", "#define I_EXP_SHARE_ITEM        GEN_5", "#define I_EXP_SHARE_ITEM        GEN_6")
replace_once("include/config/battle.h", "#define B_LAST_USED_BALL_BUTTON     R_BUTTON", "#define B_LAST_USED_BALL_BUTTON     L_BUTTON")

# Direct R -> Run in ordinary wild battles. Trainer battles ignore R.
battle_rel = "src/battle_controller_player.c"
bc = read(battle_rel)
if "PRL: R-to-run" not in bc:
    marker = "static void HandleInputChooseAction(void)\n{\n"
    if marker not in bc:
        raise SystemExit("HandleInputChooseAction marker not found")
    hook = """static void HandleInputChooseAction(void)
{
    // PRL: R-to-run. One press immediately chooses RUN in ordinary wild battles.
    // Trainer battles deliberately ignore this shortcut.
    if (JOY_NEW(R_BUTTON) && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
    {
        BtlController_EmitTwoReturnValues(BUFFER_B, B_ACTION_RUN, 0);
        HideLastUsedBall();
        PlayerBufferExecCompleted();
        return;
    }
"""
    write(battle_rel, bc.replace(marker, hook, 1))

# ---------------------------------------------------------------------------
# Relic Reports replaces visible Oak's Parcel presentation while retaining the
# internal story item ID/flow.
# ---------------------------------------------------------------------------
items_rel = "src/data/items.h"
items = read(items_rel)
parcel_pat = r'(\[ITEM_PARCEL\]\s*=\s*\{\s*\.name = ITEM_NAME\(")Parcel("\),\s*\.price = 0,\s*\.description = COMPOUND_STRING\(\s*)"A parcel for Prof\.\\n"\s*"Oak from a PokÃ©mon\\n"\s*"Mart clerk\."\),'
parcel_repl = r'\1Relic Reports\2\n        .price = 0,\n        .description = COMPOUND_STRING(\n            "Archaeology reports\\n"\n            "requested by Prof.\\n"\n            "Oak."),'
items2, n = re.subn(parcel_pat, parcel_repl, items, count=1, flags=re.S)
if n != 1:
    raise SystemExit(f"Parcel item data patch count {n}")
write(items_rel, items2)

mart_rel = "data/maps/ViridianCity_Mart_Frlg/scripts.inc"
mart = read(mart_rel)
mart = mart.replace(
    '\t.string "His order came in.\\n"\n\t.string "Can I get you to take it to him?$"',
    '\t.string "His RELIC REPORTS came in.\\n"\n\t.string "Can I get you to take them to him?$"'
)
mart = mart.replace(
    '\t.string "{PLAYER} received OAK\'S PARCEL\\n"\n\t.string "from the POKÃ©MON MART clerk.$"',
    '\t.string "{PLAYER} received RELIC REPORTS\\n"\n\t.string "from the POKÃ©MON MART clerk.$"'
)
write(mart_rel, mart)

lab_rel = "data/maps/PalletTown_ProfessorOaksLab_Frlg/scripts.inc"
lab = read(lab_rel)
lab = lab.replace(
    '\t.string "What\'s that?\\n"\n\t.string "You have something for me?$"',
    '\t.string "What\'s that?\\n"\n\t.string "You have the RELIC REPORTS?$"'
)
lab = lab.replace(
    '\t.string "Ah! \\n"\n\t.string "It\'s the custom POKÃ© BALL!\\p"\n\t.string "I had it on order.\\n"\n\t.string "Thank you!$"',
    '\t.string "Ah! The archaeology reports!\\p"\n\t.string "Perfect timing. These findings may\\n"\n\t.string "explain KANTO\'s ancient RELICS.\\p"\n\t.string "Thank you, {PLAYER}!$"'
)
lab = lab.replace(
    '\t.string "{PLAYER} delivered OAK\'S PARCEL.$"',
    '\t.string "{PLAYER} delivered the RELIC REPORTS.$"'
)
# Twenty balls like mature PKR opening, and skip forced Old Man catching lesson.
lab = lab.replace(
    '\tgiveitem_msg PalletTown_ProfessorOaksLab_Text_ReceivedFivePokeBalls, ITEM_POKE_BALL, 5',
    '\tgiveitem_msg PalletTown_ProfessorOaksLab_Text_ReceivedFivePokeBalls, ITEM_POKE_BALL, 20'
)
lab = lab.replace(
    '\tsetvar VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 1',
    '\tsetvar VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 2'
)
lab = lab.replace(
    '\t.string "{PLAYER} received five POKÃ© BALLS\\n"',
    '\t.string "{PLAYER} received twenty POKÃ© BALLS\\n"'
)
# No Oak battle tutorial commentary in first rival fight.
lab = lab.replace(", RIVAL_BATTLE_TUTORIAL, PalletTown_ProfessorOaksLab_Text_RivalDefeat", ", 0, PalletTown_ProfessorOaksLab_Text_RivalDefeat")

# PRL starter mapping: left=NidoranM, middle=Growlithe, right=Pikachu.
lab = lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_BULBASAUR\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_CHARMANDER\n\tsetvar RIVAL_STARTER_ID, LOCALID_CHARMANDER_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_GROWLITHE\n\tsetvar RIVAL_STARTER_ID, LOCALID_SQUIRTLE_BALL"
)
lab = lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_SQUIRTLE\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_BULBASAUR\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_GROWLITHE\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL"
)
lab = lab.replace(
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_CHARMANDER\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_SQUIRTLE\n\tsetvar RIVAL_STARTER_ID, LOCALID_SQUIRTLE_BALL",
    "\tsetvar PLAYER_STARTER_SPECIES, SPECIES_PIKACHU\n\tsetvar RIVAL_STARTER_SPECIES, SPECIES_NIDORAN_M\n\tsetvar RIVAL_STARTER_ID, LOCALID_BULBASAUR_BALL"
)

# Generic confirmation uses the buffered actual starter name.
confirm_pat = r"PalletTown_ProfessorOaksLab_EventScript_ConfirmStarterChoice::.*?\n\tend\n\nPalletTown_ProfessorOaksLab_EventScript_ConfirmBulbasaur::"
confirm_repl = """PalletTown_ProfessorOaksLab_EventScript_ConfirmStarterChoice::
\tapplymovement LOCALID_OAKS_LAB_PROF_OAK, Common_Movement_FaceRight
\twaitmovement 0
\tshowmonpic PLAYER_STARTER_SPECIES, 10, 3
\tbufferspeciesname STR_VAR_1, PLAYER_STARTER_SPECIES
\ttextcolor NPC_TEXT_COLOR_MALE
\tmsgbox PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter, MSGBOX_YESNO
\tgoto_if_eq VAR_RESULT, YES, PalletTown_ProfessorOaksLab_EventScript_ChoseStarter
\tgoto_if_eq VAR_RESULT, NO, PalletTown_ProfessorOaksLab_EventScript_DeclinedStarter
\tend

PalletTown_ProfessorOaksLab_EventScript_ConfirmBulbasaur::"""
lab, n = re.subn(confirm_pat, confirm_repl, lab, count=1, flags=re.S)
if n != 1:
    raise SystemExit(f"starter confirmation block patch count {n}")

# Rival walks to the actual initial ball under PRL mapping.
old = """\tgoto_if_eq PLAYER_STARTER_NUM, 0, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToCharmander
\tgoto_if_eq PLAYER_STARTER_NUM, 1, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur
\tgoto_if_eq PLAYER_STARTER_NUM, 2, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToSquirtle"""
new = """\tgoto_if_eq PLAYER_STARTER_NUM, 0, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToSquirtle
\tgoto_if_eq PLAYER_STARTER_NUM, 1, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur
\tgoto_if_eq PLAYER_STARTER_NUM, 2, PalletTown_ProfessorOaksLab_EventScript_RivalWalksToBulbasaur"""
if old not in lab:
    raise SystemExit("Rival starter walk dispatch not found")
lab = lab.replace(old, new, 1)

# On Relic Reports return the rival takes the remaining unchosen starter line.
parcel_marker = "\tmsgbox PalletTown_ProfessorOaksLab_Text_RivalWhatDidYouCallMeFor\n\tclosemessage\n"
parcel_insert = parcel_marker + """\tmsgbox PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter
\tremoveobject LOCALID_BULBASAUR_BALL
\tremoveobject LOCALID_SQUIRTLE_BALL
\tremoveobject LOCALID_CHARMANDER_BALL
"""
if "PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter" not in lab:
    if parcel_marker not in lab:
        raise SystemExit("Parcel rival marker missing")
    lab = lab.replace(parcel_marker, parcel_insert, 1)

# Useful Oak aides: Exp Share / Quick Claw / Soothe Bell, one each.
for label, body in {
"PalletTown_ProfessorOaksLab_EventScript_Aide1": """\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_EXP_SHARE, PalletTown_ProfessorOaksLab_EventScript_Aide1Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideExpShare
\tgiveitem ITEM_EXP_SHARE
\tsetflag FLAG_PRL_GOT_LAB_EXP_SHARE
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide1Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend""",
"PalletTown_ProfessorOaksLab_EventScript_Aide2": """\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_QUICK_CLAW, PalletTown_ProfessorOaksLab_EventScript_Aide2Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideQuickClaw
\tgiveitem ITEM_QUICK_CLAW
\tsetflag FLAG_PRL_GOT_LAB_QUICK_CLAW
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide2Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend""",
"PalletTown_ProfessorOaksLab_EventScript_Aide3": """\tlock
\tfaceplayer
\tgoto_if_set FLAG_PRL_GOT_LAB_SOOTHE_BELL, PalletTown_ProfessorOaksLab_EventScript_Aide3Repeat
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideSootheBell
\tgiveitem ITEM_SOOTHE_BELL
\tsetflag FLAG_PRL_GOT_LAB_SOOTHE_BELL
\trelease
\tend

PalletTown_ProfessorOaksLab_EventScript_Aide3Repeat::
\tmsgbox PalletTown_ProfessorOaksLab_Text_PRLAideResearch
\trelease
\tend"""
}.items():
    pat = rf"{label}::.*?(?=\nPalletTown_ProfessorOaksLab_EventScript_[A-Za-z0-9_]+::)"
    lab, n = re.subn(pat, label + "::\n" + body, lab, count=1, flags=re.S)
    if n != 1:
        raise SystemExit(f"Aide patch failed for {label}: {n}")

# Append new PRL lab strings.
if "PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter::" not in lab:
    lab += r'''
PalletTown_ProfessorOaksLab_Text_OakChoosingPRLStarter::
	.string "So, {PLAYER}, you want {STR_VAR_1}?$"

PalletTown_ProfessorOaksLab_Text_RivalTakesLastPRLStarter::
	.string "{RIVAL}: Gramps, I'm taking the last\n"
	.string "one too.\p"
	.string "I won't leave a good POKÃ©MON sitting\n"
	.string "around while {PLAYER} gets ahead!$"

PalletTown_ProfessorOaksLab_Text_PRLAideExpShare::
	.string "OAK wants every field researcher\n"
	.string "properly equipped.\p"
	.string "Take this EXP. SHARE. It'll keep your\n"
	.string "whole team moving forward.$"

PalletTown_ProfessorOaksLab_Text_PRLAideQuickClaw::
	.string "Fieldwork rewards preparation.\p"
	.string "Take this QUICK CLAW. A slower\n"
	.string "POKÃ©MON can still surprise you.$"

PalletTown_ProfessorOaksLab_Text_PRLAideSootheBell::
	.string "Some ancient traits only surface\n"
	.string "when a POKÃ©MON truly trusts you.\p"
	.string "This SOOTHE BELL should help.$"

PalletTown_ProfessorOaksLab_Text_PRLAideResearch::
	.string "Keep an eye out for unusual changes.\n"
	.string "OAK's relic research is just starting.$"
'''
write(lab_rel, lab)

# Flags use previously-unused IDs; existing numbering is untouched.
flags_rel = "include/constants/flags.h"
flags = read(flags_rel)
if "FLAG_PRL_GOT_LAB_EXP_SHARE" not in flags:
    marker = "#define FLAG_UNUSED_0x02B    0x2B // Unused Flag\n"
    aliases = """\n// PokÃ©mon Relic Legacy early-Kanto flags.
#define FLAG_PRL_GOT_LAB_EXP_SHARE FLAG_UNUSED_0x020
#define FLAG_PRL_GOT_LAB_QUICK_CLAW FLAG_UNUSED_0x021
#define FLAG_PRL_GOT_LAB_SOOTHE_BELL FLAG_UNUSED_0x022
#define FLAG_PRL_GOT_BULBASAUR_GIFT FLAG_UNUSED_0x023
#define FLAG_PRL_GOT_CHARMANDER_GIFT FLAG_UNUSED_0x024
#define FLAG_PRL_GOT_SQUIRTLE_GIFT FLAG_UNUSED_0x025
"""
    if marker not in flags:
        raise SystemExit("Unused-flag marker not found")
    write(flags_rel, flags.replace(marker, marker + aliases, 1))

# ---------------------------------------------------------------------------
# Early NPC/tutorial cleanup and Noxichu hint.
# ---------------------------------------------------------------------------
vir_map_rel = "data/maps/ViridianCity_Frlg/map.json"
vm = json.loads(read(vir_map_rel))
vm["object_events"] = [
    o for o in vm["object_events"]
    if o.get("script") not in ("ViridianCity_EventScript_Woman", "ViridianCity_EventScript_Youngster")
]
write(vir_map_rel, json.dumps(vm, indent=2) + "\n")

gate_map_rel = "data/maps/Route2_ViridianForest_SouthEntrance_Frlg/map.json"
gm = json.loads(read(gate_map_rel))
gm["object_events"] = [
    o for o in gm["object_events"]
    if o.get("script") != "Route2_ViridianForest_SouthEntrance_EventScript_Woman2"
]
write(gate_map_rel, json.dumps(gm, indent=2) + "\n")

vir_script_rel = "data/maps/ViridianCity_Frlg/scripts.inc"
vs = read(vir_script_rel)
old_boy = '''ViridianCity_Text_CanCarryMonsAnywhere::
\t.string "Those POKÃ© BALLS at your waist!\\n"
\t.string "You have POKÃ©MON, don't you?\\p"
\t.string "It's great that you can carry and\\n"
\t.string "use POKÃ©MON anytime, anywhere.$"'''
new_boy = '''ViridianCity_Text_CanCarryMonsAnywhere::
\t.string "VIRIDIAN FOREST is just ahead.\\p"
\t.string "OAK says PIKACHU may hide an\\n"
\t.string "ancient trait of its own.\\p"
\t.string "He mentioned a MOON STONEâ€¦$"'''
if old_boy not in vs:
    raise SystemExit("Viridian boy text block not found")
write(vir_script_rel, vs.replace(old_boy, new_boy, 1))

# ---------------------------------------------------------------------------
# Visible Lv10 classic-starter gifts beside Oak aides.
# Reuse existing early NPC slots as aides; preserve genuine FRLG geography.
# ---------------------------------------------------------------------------
gift_specs = [
    ("data/maps/ViridianForest_Frlg/map.json", "data/maps/ViridianForest_Frlg/scripts.inc",
     "ViridianForest_EventScript_Youngster", "ViridianForest_EventScript_PRLBulbasaurAide",
     "LOCALID_PRL_BULBASAUR", "BULBASAUR", "FLAG_PRL_GOT_BULBASAUR_GIFT",
     "ITEM_MIRACLE_SEED", 29, 58, 30, 58,
     "BULBASAUR", "MIRACLE SEED", "GRASS-type"),
    ("data/maps/Route3_Frlg/map.json", "data/maps/Route3_Frlg/scripts.inc",
     "Route3_EventScript_Youngster", "Route3_EventScript_PRLCharmanderAide",
     "LOCALID_PRL_CHARMANDER", "CHARMANDER", "FLAG_PRL_GOT_CHARMANDER_GIFT",
     "ITEM_SHARP_BEAK", 70, 13, 71, 13,
     "CHARMANDER", "SHARP BEAK", "FIRE-type"),
    ("data/maps/Route4_Frlg/map.json", "data/maps/Route4_Frlg/scripts.inc",
     "Route4_EventScript_Woman", "Route4_EventScript_PRLSquirtleAide",
     "LOCALID_PRL_SQUIRTLE", "SQUIRTLE", "FLAG_PRL_GOT_SQUIRTLE_GIFT",
     "ITEM_METAL_COAT", 9, 8, 10, 8,
     "SQUIRTLE", "METAL COAT", "WATER-type"),
]

for map_rel, script_rel, old_script, aide_script, localid, species, flag, item, ax, ay, px, py, disp, item_disp, type_disp in gift_specs:
    m = json.loads(read(map_rel))
    target = None
    for o in m["object_events"]:
        if o.get("script") == old_script:
            target = o
            break
    if target is None:
        raise SystemExit(f"{map_rel}: donor NPC {old_script} missing")
    target["graphics_id"] = "OBJ_EVENT_GFX_SCIENTIST"
    target["script"] = aide_script
    # Keep its known safe existing coordinates.
    if not any(o.get("local_id") == localid for o in m["object_events"]):
        m["object_events"].append({
            "local_id": localid,
            "type": "object",
            "graphics_id": f"OBJ_EVENT_GFX_SPECIES({species})",
            "x": px,
            "y": py,
            "elevation": target.get("elevation", 3),
            "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
            "movement_range_x": 0,
            "movement_range_y": 0,
            "trainer_type": "TRAINER_TYPE_NONE",
            "trainer_sight_or_berry_tree_id": "0",
            "script": aide_script + "_Pokemon",
            "flag": flag
        })
    write(map_rel, json.dumps(m, indent=2) + "\n")

    sc = read(script_rel)
    if aide_script + "::" not in sc:
        add = f'''
{aide_script}::
\tlock
\tfaceplayer
\tgoto_if_set {flag}, {aide_script}_Repeat
\tmsgbox {aide_script}_Text_Intro
\tsetvar VAR_TEMP_1, SPECIES_{species}
\tgivemon SPECIES_{species}, 10
\tgoto_if_eq VAR_RESULT, 2, Common_EventScript_NoMoreRoomForPokemon
\tsetflag {flag}
\tremoveobject {localid}
\tgiveitem {item}
\tmsgbox {aide_script}_Text_After
\trelease
\tend

{aide_script}_Repeat::
\tmsgbox {aide_script}_Text_Repeat
\trelease
\tend

{aide_script}_Pokemon::
\tmsgbox {aide_script}_Text_Pokemon, MSGBOX_NPC
\tend

{aide_script}_Text_Intro::
\t.string "PROF. OAK asked me to entrust this\\n"
\t.string "POKÃ©MON to a promising TRAINER.\\p"
\t.string "It may look ordinary nowâ€¦ but OAK\\n"
\t.string "believes something ancient sleeps\\l"
\t.string "within it.$"

{aide_script}_Text_After::
\t.string "Take this {item_disp}, too.\\p"
\t.string "OAK thinks certain {type_disp}\\n"
\t.string "POKÃ©MON may respond to it as they\\l"
\t.string "mature. Take good care of {disp}.$"

{aide_script}_Text_Repeat::
\t.string "Keep training {disp}. OAK's relic\\n"
\t.string "theory may take time to prove.$"

{aide_script}_Text_Pokemon::
\t.string "{disp} looks ready to travel.$"
'''
        write(script_rel, sc.rstrip() + "\n" + add)

# ---------------------------------------------------------------------------
# PRL rival: by Route 22 and Cerulean he owns both unchosen starter lines.
# Existing three FRLG branch IDs are retained as implementation slots only.
# ---------------------------------------------------------------------------
party_rel = "src/data/trainers_frlg.party"
party = read(party_rel)

def trainer_block(label, mons):
    lines = [f"=== {label} ===", "Name: TERRY", "Class: Rival Early Frlg",
             "Pic: Rival Early Frlg", "Gender: Male", "Music: Male",
             "Double Battle: No", "AI: Check Bad Move / Try To Faint / Check Viability", ""]
    for species, level, iv in mons:
        lines += [species, f"Level: {level}", f"IVs: {iv} HP / {iv} Atk / {iv} Def / {iv} SpA / {iv} SpD / {iv} Spe", ""]
    return "\n".join(lines).rstrip() + "\n"

teams = {
    # First Oak Lab battle. Script mapping: starter2->SQUIRTLE, 1->BULBASAUR, 0->CHARMANDER.
    "TRAINER_RIVAL_OAKS_LAB_SQUIRTLE": [("NidoranM",5,0)],
    "TRAINER_RIVAL_OAKS_LAB_BULBASAUR": [("NidoranM",5,0)],
    "TRAINER_RIVAL_OAKS_LAB_CHARMANDER": [("Growlithe",5,0)],
    # After Relic Reports: both unchosen lines.
    "TRAINER_RIVAL_ROUTE22_EARLY_SQUIRTLE": [("Pidgey",9,6),("NidoranM",9,6),("Growlithe",9,6)],
    "TRAINER_RIVAL_ROUTE22_EARLY_BULBASAUR": [("Pidgey",9,6),("NidoranM",9,6),("Pikachu",9,6)],
    "TRAINER_RIVAL_ROUTE22_EARLY_CHARMANDER": [("Pidgey",9,6),("Growlithe",9,6),("Pikachu",9,6)],
    "TRAINER_RIVAL_CERULEAN_SQUIRTLE": [("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Nidorino",18,12),("Growlithe",18,12)],
    "TRAINER_RIVAL_CERULEAN_BULBASAUR": [("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Nidorino",18,12),("Pikachu",18,12)],
    "TRAINER_RIVAL_CERULEAN_CHARMANDER": [("Pidgeotto",17,6),("Abra",16,6),("Rattata",15,6),("Growlithe",18,12),("Pikachu",18,12)],
}
for label, mons in teams.items():
    pat = rf"=== {re.escape(label)} ===\n.*?(?=\n=== |\Z)"
    party, n = re.subn(pat, trainer_block(label, mons), party, count=1, flags=re.S)
    if n != 1:
        raise SystemExit(f"trainer block {label} patch count {n}")
write(party_rel, party)

# ---------------------------------------------------------------------------
# PKR-style early encounter friction.
# Patch only the FRLG entries matching the expected species sets.
# ---------------------------------------------------------------------------
wild_rel = "src/data/wild_encounters.json"
wild = json.loads(read(wild_rel))
def walk(obj):
    if isinstance(obj, dict):
        yield obj
        for v in obj.values():
            yield from walk(v)
    elif isinstance(obj, list):
        for v in obj:
            yield from walk(v)

route1_hits = route2_hits = 0
for d in walk(wild):
    if d.get("map") == "MAP_ROUTE1" and isinstance(d.get("land_mons"), dict):
        lm = d["land_mons"]
        species = {x.get("species") for x in lm.get("mons", [])}
        if species and species <= {"SPECIES_PIDGEY","SPECIES_RATTATA"}:
            lm["encounter_rate"] = 15
            route1_hits += 1
    if d.get("map") == "MAP_ROUTE2" and isinstance(d.get("land_mons"), dict):
        lm = d["land_mons"]
        species = {x.get("species") for x in lm.get("mons", [])}
        target = {"SPECIES_PIDGEY","SPECIES_RATTATA","SPECIES_CATERPIE","SPECIES_WEEDLE"}
        if species and species <= target and len(lm.get("mons", [])) == 12:
            lm["encounter_rate"] = 15
            rows = [
                (3,3,"SPECIES_PIDGEY"), (3,3,"SPECIES_RATTATA"),
                (4,4,"SPECIES_CATERPIE"), (4,4,"SPECIES_WEEDLE"),
                (2,2,"SPECIES_CATERPIE"), (2,2,"SPECIES_WEEDLE"),
                (5,5,"SPECIES_PIDGEY"), (5,5,"SPECIES_RATTATA"),
                (4,4,"SPECIES_CATERPIE"), (4,4,"SPECIES_WEEDLE"),
                (5,5,"SPECIES_CATERPIE"), (5,5,"SPECIES_WEEDLE"),
            ]
            lm["mons"] = [{"min_level":a,"max_level":b,"species":s} for a,b,s in rows]
            route2_hits += 1
if route1_hits < 1 or route2_hits < 1:
    raise SystemExit(f"FRLG encounter records not found safely: route1={route1_hits}, route2={route2_hits}")
write(wild_rel, json.dumps(wild, indent=2) + "\n")

# Migration manifest: exact implementation scope, explicitly not QA-passed yet.
manifest_rel = "tools/prl/pkr_port_manifest.json"
manifest = json.loads(read(manifest_rel))
layer = next(x for x in manifest["layers"] if x["id"] == "pkr_story_kanto")
ported = layer.setdefault("ported", [])
for entry in [
    "Cerulean milestone: Relic Reports replaces visible Oak Parcel presentation while retaining the safe internal story item.",
    "Cerulean milestone: Viridian forced Weedle tutorial skipped after Relic Reports; low-value early NPC clutter removed/replaced with a Noxichu Moon Stone hint.",
    "Cerulean milestone: Kanto starters changed to NidoranM / Growlithe / Pikachu with rival owning both unchosen lines by Route 22 and Cerulean.",
    "Cerulean milestone: visible Lv10 Bulbasaur/Charmander/Squirtle gifts beside Oak aides with Miracle Seed/Sharp Beak/Metal Coat.",
    "Cerulean milestone: Noxichu added as fresh PRL custom species ID with locked Electric/Dark, Infiltrator, 70/110/70/110/80/125 and Moon Stone evolution.",
    "Cerulean milestone QoL: direct R-to-run in wild battles, L last Ball, reusable TMs, Gen6-style Exp Share key item and useful Oak lab aides."
]:
    if entry not in ported:
        ported.append(entry)
layer["status"] = "in_progress"
manifest.setdefault("milestones", {})["PRL_PKR_PORT_003_CERULEAN"] = {
    "status": "implementation_complete_awaiting_build",
    "boundary": "New game through Cerulean/Bill approach",
    "requires_fresh_save": True
}
write(manifest_rel, json.dumps(manifest, indent=2) + "\n")

print("PRL Cerulean milestone source patch applied.")
print(f"FRLG encounter records patched: Route1={route1_hits}, Route2={route2_hits}")
