#!/usr/bin/env python3
"""Generate three indexed 64x256 PRL trial animations from approved battle-sprite art.
Only writes graphics/intro/prl_trial/*.png, never changes canonical battle sprites.
"""
import base64
import io
from pathlib import Path
from PIL import Image

NOX_APPROVED_PNG_BASE64 = (
"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"
)

def animation(source, kind):
    frames = []
    for phase in range(4):
        out = source.copy()
        pix = source.load()
        if kind == "nox":
            masks = [(2,54,21,64), (18,56,36,64), (34,55,47,64), (45,53,62,64)]
            moves = [
                [(0,0)]*4,
                [(-2,-2),(2,0),(1,-2),(-1,1)],
                [(0,0),(0,-1),(0,0),(0,0)],
                [(2,0),(-2,-2),(-1,1),(1,-2)]
            ][phase]
            for (x0,y0,x1,y1),(dx,dy) in zip(masks, moves):
                for y in range(y0,y1):
                    for x in range(x0,x1):
                        out.putpixel((x,y),0)
                for y in range(y0,y1):
                    for x in range(x0,x1):
                        xx,yy=x+dx,y+dy
                        if 0<=xx<64 and 0<=yy<64 and pix[x,y]:
                            out.putpixel((xx,yy),pix[x,y])
        elif kind == "astra":
            boxes=[(2,39,16,63),(48,39,62,63),(16,45,27,62),(37,45,49,62)]
            d=[0,2,0,-2][phase]
            for x0,y0,x1,y1 in boxes:
                for y in range(y0,y1):
                    for x in range(x0,x1):
                        out.putpixel((x,y),0)
            for i,(x0,y0,x1,y1) in enumerate(boxes):
                for y in range(y0,y1):
                    shift=round(d*(y-y0)/max(1,y1-y0-1)*(1 if i%2==0 else -1))
                    for x in range(x0,x1):
                        if pix[x,y] and 0<=x+shift<64:
                            out.putpixel((x+shift,y),pix[x,y])
        else:
            t=[0,2,0,-2][phase]
            for y in range(26):
                spread=round(abs(t)*(26-y)/26)
                if t<0:spread=-spread
                for x in list(range(19))+list(range(45,64)):
                    if pix[x,y]:out.putpixel((x,y),0)
                for x in list(range(19))+list(range(45,64)):
                    dest=x+(-spread if x<32 else spread)
                    if pix[x,y] and 0<=dest<64:out.putpixel((dest,y),pix[x,y])
        frames.append(out)
    sheet=Image.new("P",(64,256),0)
    sheet.putpalette(source.getpalette())
    sheet.info["transparency"]=0
    for index,frame in enumerate(frames):
        sheet.paste(frame,(0,index*64))
    assert len(set(sheet.getdata()))<=16
    return sheet

def main():
    root=Path("graphics/intro/prl_trial")
    root.mkdir(parents=True,exist_ok=True)
    sources={
        "noxichu_run":(Image.open(io.BytesIO(base64.b64decode(NOX_APPROVED_PNG_BASE64))),"nox"),
        "astrachi_fly":(Image.open("graphics/pokemon/astrachi/front.png"),"astra"),
        "ho_oh_relic_fly":(Image.open("graphics/pokemon/ho_oh_relic/front.png"),"hooh"),
    }
    for name,(source,kind) in sources.items():
        source.load()
        assert source.size==(64,64) and source.mode=="P", (name,source.mode,source.size)
        source.info["transparency"]=0
        dst=animation(source,kind)
        dst.save(root/(name+".png"),transparency=0,optimize=False)
        print("INTRO_ASSET_PASS",name,dst.size,len(set(dst.getdata())))
if __name__=="__main__":
    main()
