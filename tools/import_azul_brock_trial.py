#!/usr/bin/env python3
"""Isolated Azul Agua Beta 1.4 Brock gym interior trial for PRL FRLG.
Source artwork is from the user's supplied donor ROM. Does not copy code/scripts.
"""
from pathlib import Path
from collections import Counter
import struct, json, zlib, binascii, hashlib

ROOT = Path("C:/Users/HowardGMKtec/Documents/RelicDev/PRL-AZUL-BROCK-TRIAL-20261009")
ROM = Path("C:/Users/HowardGMKtec/Documents/RelicDev/Archive/2026-10-05_Housekeeping/Donors/AzulAgua/azul_agua_beta1.4ml.gba")
data = ROM.read_bytes()
assert len(data) == 0x1000000 and data[0xAC:0xB0] == b'BPRE', "Unexpected donor ROM."
u16 = lambda n: struct.unpack_from("<H", data, n)[0]
u32 = lambda n: struct.unpack_from("<I", data, n)[0]
def ptr(x):
    assert 0x08000000 <= x < 0x09000000
    return x - 0x08000000
def lz(off):
    assert data[off] == 0x10, hex(off)
    size = data[off+1] | data[off+2]<<8 | data[off+3]<<16
    out = bytearray()
    i = off+4
    while len(out)<size:
        flags = data[i]; i+=1
        for shift in range(7,-1,-1):
            if flags & (1<<shift):
                b1,b2 = data[i],data[i+1]; i+=2
                count = (b1>>4)+3
                distance = (((b1&15)<<8)|b2)+1
                for _ in range(count): out.append(out[-distance])
            else:
                out.append(data[i]);i+=1
            if len(out)>=size:break
    return bytes(out)
def png_from_4bpp(src,path,expected_tiles,base_palette):
    assert len(src)==expected_tiles*32
    # Store each 8x8 tile in a 16-column sheet, exactly like FRLG's tiles.png.
    width=128
    height=(expected_tiles//16)*8
    pixels=bytearray(width*height)
    for tile in range(expected_tiles):
        xo=(tile%16)*8
        yo=(tile//16)*8
        for y in range(8):
            for bx in range(4):
                a=src[tile*32+y*4+bx]
                pixels[(yo+y)*width+xo+bx*2]=a&15
                pixels[(yo+y)*width+xo+bx*2+1]=a>>4
    palette=b''.join(bytes(((v&31)*8,((v>>5)&31)*8,((v>>10)&31)*8)) for v in base_palette)
    def ch(tag,b):return struct.pack(">I",len(b))+tag+b+struct.pack(">I",binascii.crc32(tag+b)&0xffffffff)
    payload=b"\x89PNG\r\n\x1a\n"
    payload+=ch(b'IHDR',struct.pack(">IIBBBBB",width,height,8,3,0,0,0))
    payload+=ch(b'PLTE',palette)
    scan=b''.join(b'\x00'+pixels[y*width:(y+1)*width] for y in range(height))
    payload+=ch(b'IDAT',zlib.compress(scan,9))+ch(b'IEND',b'')
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_bytes(payload)
    return (width,height,len(payload))
def paltext(base):
    entries=[u16(base+2*i) for i in range(16)]
    return "JASC-PAL\n0100\n16\n"+"\n".join(f"{(v&31)*8} {((v>>5)&31)*8} {((v>>10)&31)*8}" for v in entries)+"\n"
def emit_palette_set(folder,base,start,num):
    pal=folder/"palettes";pal.mkdir(parents=True,exist_ok=True)
    for i in range(num):
        (pal/f"{start+i:02d}.pal").write_text(paltext(base+i*32),encoding="ascii")
def replace_once(file,old,new):
    src=file.read_text(encoding="utf-8")
    if src.count(old)!=1:
        raise ValueError(f"Expected one occurrence of {old[:60]} in {file}: got {src.count(old)}")
    file.write_text(src.replace(old,new),encoding="utf-8",newline="\n")
def insert_before(file,needle,addition):
    replace_once(file,needle,addition+"\n"+needle)

# Verify the exact map and tileset headers, not an arbitrary LZ77 candidate.
gym_header=0xA0A47C
layout=ptr(u32(gym_header))
assert layout==0x98EAA8
w,h=u32(layout),u32(layout+4)
assert (w,h)==(13,17)
border=ptr(u32(layout+8))
blocks=ptr(u32(layout+12))
primary=ptr(u32(layout+16))
secondary=ptr(u32(layout+20))
assert (border,blocks,primary,secondary)==(0x98E8E4,0x98E8EC,0x946984,0x946BA0)
assert data[primary]==1 and data[secondary:secondary+2]==b'\x01\x01'
P=ROOT/"data/tilesets/primary/azul_agua_brock_trial"
S=ROOT/"data/tilesets/secondary/azul_agua_brock_trial"
gfxP=lz(ptr(u32(primary+4)))
gfxS=lz(ptr(u32(secondary+4)))
assert (len(gfxP),len(gfxS))==(640*32,384*32)
emit_palette_set(P,ptr(u32(primary+8)),0,7)
emit_palette_set(S,ptr(u32(secondary+8)),7,6)
palP=[u16(ptr(u32(primary+8))+i*2) for i in range(16)]
palS=[u16(ptr(u32(secondary+8))+i*2) for i in range(16)]
print("Tilesheets",png_from_4bpp(gfxP,P/"tiles.png",640,palP),png_from_4bpp(gfxS,S/"tiles.png",384,palS))
metP=ptr(u32(primary+12))
attrP=ptr(u32(primary+20))
assert metP==0x9070C8 and attrP==0x90ACC8
(P/"metatiles.bin").write_bytes(data[metP:metP+640*16])
(P/"metatile_attributes.bin").write_bytes(data[attrP:attrP+640*4])
layout_dir=ROOT/"data/layouts/PewterCity_Gym_Frlg"
(layout_dir/"map.bin").write_bytes(data[blocks:blocks+2*w*h])
(layout_dir/"border.bin").write_bytes(data[border:border+8])
used=Counter(u16(blocks+2*i)&1023 for i in range(w*h))
assert max(used)<640
# The map uses first 221 primary metatiles; local tiles are referenced by the primary metatiles.
palette_indices=set();tile_indices=set()
for index in used:
    for x in range(8):
        met=u16(metP+index*16+2*x)
        palette_indices.add((met>>12)&15)
        tile_indices.add(met&1023)
assert max(tile_indices)<1024 and max(palette_indices)<=12
assert any(x>=640 for x in tile_indices)
ljson=ROOT/"data/layouts/layouts.json"
j=json.loads(ljson.read_text(encoding="utf-8"))
layout_obj=next(x for x in j["layouts"] if x["id"]=="LAYOUT_PEWTER_CITY_GYM")
assert (layout_obj["width"],layout_obj["height"])==(13,16)
assert layout_obj["primary_tileset"]=="gTileset_BuildingFrlg" and layout_obj["secondary_tileset"]=="gTileset_PewterGym"
layout_obj["height"]=h
layout_obj["primary_tileset"]="gTileset_AzulAguaBrockTrialPrimary"
layout_obj["secondary_tileset"]="gTileset_AzulAguaBrockTrialSecondary"
ljson.write_text(json.dumps(j,indent=2)+"\n",encoding="utf-8",newline="\n")
mjson=ROOT/"data/maps/PewterCity_Gym_Frlg/map.json"
m=json.loads(mjson.read_text(encoding="utf-8"))
assert len(m["object_events"])==3
for obj in m["object_events"]:
    if obj["script"]=="PewterCity_Gym_EventScript_Brock":
        assert (obj["x"],obj["y"])==(6,5)
        obj["y"]=3
    if obj["script"]=="PewterCity_Gym_EventScript_GymGuy":
        assert (obj["x"],obj["y"])==(7,12)
        obj["y"]=13
# Preserve PRL's existing trainer scripts, story, music and badge flags.
# Align entry/exit and statue events with the original Azul Agua floorplan.
m["warp_events"]=[{"x":6,"y":15,"elevation":3,"dest_map":"MAP_PEWTER_CITY","dest_warp_id":"2"}]
for obj in m["bg_events"]:
    if obj["script"]=="PewterCity_Gym_EventScript_GymStatue":
        assert obj["y"]==12
        obj["y"]=13
mjson.write_text(json.dumps(m,indent=2)+"\n",encoding="utf-8",newline="\n")
# Add two private-trial-only tilesets without touching general Building FRLG assets.
H=ROOT/"src/data/tilesets/headers.h"
needle="const struct Tileset gTileset_PewterGym ="
extra="""// Azul Agua Brock interior — independent visual trial; no other maps use these.
const struct Tileset gTileset_AzulAguaBrockTrialPrimary =
{
    .isCompressed = TRUE,
    .isSecondary = FALSE,
    .tiles = gTilesetTiles_AzulAguaBrockTrialPrimary,
    .palettes = gTilesetPalettes_AzulAguaBrockTrialPrimary,
    .metatiles = gMetatiles_AzulAguaBrockTrialPrimary,
    .metatileAttributes = gMetatileAttributes_AzulAguaBrockTrialPrimary,
    .callback = NULL,
};
const struct Tileset gTileset_AzulAguaBrockTrialSecondary =
{
    .isCompressed = TRUE,
    .isSecondary = TRUE,
    .tiles = gTilesetTiles_AzulAguaBrockTrialSecondary,
    .palettes = gTilesetPalettes_AzulAguaBrockTrialSecondary,
    // No secondary metatile IDs are used by this donor layout.
    .metatiles = gMetatiles_PewterGym,
    .metatileAttributes = gMetatileAttributes_PewterGym,
    .callback = NULL,
};
"""
insert_before(H,needle,extra)
G=ROOT/"src/data/tilesets/graphics.h"
needle="const u32 gTilesetTiles_Building_Frlg[] = "
src=G.read_text(encoding="utf-8")
assert src.count(needle)==1
def graphics(name,typ,begin,n):
    folder=f"data/tilesets/{typ}/azul_agua_brock_trial"
    code=f'const u32 gTilesetTiles_AzulAguaBrockTrial{name}[] = INCGFX_U32("{folder}/tiles.png", ".4bpp.smol");\n'
    code+=f"const u16 gTilesetPalettes_AzulAguaBrockTrial{name}[][16] =\n{{\n"
    for idx in range(begin,begin+n):
        code+=f'    INCGFX_U16("{folder}/palettes/{idx:02}.pal", ".gbapal"),\n'
    code+="};\n\n"
    return code
insert_before(G,needle,graphics("Primary","primary",0,7)+graphics("Secondary","secondary",7,6))
M=ROOT/"src/data/tilesets/metatiles.h"
insert_before(M,'const u16 gMetatiles_Building_Frlg[] =',"""const u16 gMetatiles_AzulAguaBrockTrialPrimary[] =
    INCBIN_U16("data/tilesets/primary/azul_agua_brock_trial/metatiles.bin");
const u16 gMetatileAttributes_AzulAguaBrockTrialPrimary[] =
    INCBIN_U16("data/tilesets/primary/azul_agua_brock_trial/metatile_attributes.bin");
""")
README=ROOT/"docs/trials/AZUL_AGUA_BROCK_GYM_20261009.md"
README.parent.mkdir(parents=True,exist_ok=True)
README.write_text("""# Azul Agua Brock's Gym — isolated PRL trial

- Donor: user-supplied Pokémon Water Blue / Azul Agua Beta 1.4 ML (creator: gameboy_cl).
- Source gym: FireRed group 6, map 2; map header 0xA0A47C, layout 0x98EAA8.
- Full donor 13x17 layout, 2x2 map border, donor primary/secondary 4bpp tiles,
  palettes and primary metatile attributes, with original collision/elevation map blocks.
- Preserve PRL Brock/Camper Liam/Gym Guide dialogue, battles, flags and badge logic.
- Align the Brock, Gym Guide, statues and exit to the new floor plan.
- Deliberately not a canonical map change; don't merge without manual gameplay review.
- Azul Agua-specific extra events/scripts and graphics-linked NPCs are not copied,
  since their semantics have not been mapped to PRL's script engine.
- Verify sprites, elevations, passability, door exit, dialogue and Brock battle in mGBA.
- Before redistributing artwork, confirm the original artist's reuse terms.
""",encoding="utf-8")
print("DONE", "ROM_SHA256",hashlib.sha256(data).hexdigest(),"metatiles",len(used),"secondary_tile_refs",sum(x>=640 for x in tile_indices),"palettes",sorted(palette_indices))
print("MAP",w,h,"BLOCKS",len((layout_dir/"map.bin").read_bytes()),"BORDER",len((layout_dir/"border.bin").read_bytes()))
