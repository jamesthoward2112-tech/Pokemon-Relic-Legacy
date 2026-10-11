#!/usr/bin/env python3
"""Drive actual mGBA frames and save QA evidence; no ROM patches are applied."""
import argparse
import ctypes as C
import json
import os
from pathlib import Path
import subprocess
from PIL import Image, ImageDraw

KEYS = {'A':1, 'B':2, 'SELECT':4, 'START':8, 'RIGHT':16, 'LEFT':32,
        'UP':64, 'DOWN':128, 'R':256, 'L':512}

class Emulator:
    def __init__(self, rom, save=None, probe=None):
        self.lib=C.CDLL(str(Path(probe or os.environ['PRL_MGBA_PROBE']).resolve()))
        self.lib.prl_open.argtypes=[C.c_char_p,C.c_char_p]
        self.lib.prl_pixels.restype=C.c_void_p
        self.lib.prl_reg.argtypes=[C.c_char_p]
        self.lib.prl_reg.restype=C.c_uint32
        self.lib.prl_setreg.argtypes=[C.c_char_p,C.c_uint32]
        self.lib.prl_read.restype=C.c_uint32
        self.lib.prl_state.argtypes=[C.c_char_p,C.c_int]
        self.lib.prl_save.argtypes=[C.c_char_p]
        assert self.lib.prl_open(os.fsencode(Path(rom).resolve()), os.fsencode(Path(save).resolve()) if save else None)
        self.frames=0
    def frame(self, count=1, keys=0):
        if isinstance(keys,str):keys=sum(KEYS[x] for x in keys.split('+'))
        self.lib.prl_frame(keys,count);self.frames+=count
    def press(self,keys,held=1,after=30):
        self.frame(held,keys);self.frame(after)
    def pixels(self):return C.string_at(self.lib.prl_pixels(),240*160*4)
    def image(self):return Image.frombytes('RGBA',(240,160),self.pixels()).convert('RGB')
    def shot(self,path):self.image().save(path)
    def read(self,address,size=4):return self.lib.prl_read(address,size)
    def write(self,address,value,size=4):self.lib.prl_write(address,value,size)
    def readbytes(self,address,size):return bytes(self.read(address+i,1) for i in range(size))
    def writebytes(self,address,data):
        for i,b in enumerate(data):self.write(address+i,b,1)
    def state(self,path,save=True):assert self.lib.prl_state(os.fsencode(path),save)
    def reg(self,name):return self.lib.prl_reg(name.encode())
    def setreg(self,name,value):assert self.lib.prl_setreg(name.encode(),value)
    def close(self):self.lib.prl_close()

def coldboot(args):
    out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    emu=Emulator(args.rom,probe=args.probe)
    video=subprocess.Popen(['ffmpeg','-v','error','-y','-f','rawvideo','-pixel_format','rgba','-video_size','240x160','-framerate','59.7275','-i','-','-an','-vf','scale=720:480:flags=neighbor','-c:v','libx264','-crf','18','-pix_fmt','yuv420p',str(out/'cold-boot.mp4')],stdin=subprocess.PIPE)
    samples=[]
    for f in range(args.frames):
        emu.frame();video.stdin.write(emu.pixels())
        if f%120==0:
            im=emu.image();im.save(out/f'boot-{f:04}.png');samples.append((f,im))
    video.stdin.close();assert video.wait()==0
    sheet=Image.new('RGB',(240*5,180*((len(samples)+4)//5)),'#242424');draw=ImageDraw.Draw(sheet)
    for i,(f,im) in enumerate(samples):
        x=i%5*240;y=i//5*180;sheet.paste(im,(x,y));draw.text((x+4,y+162),f'Frame {f} / {f/59.7275:.1f}s',fill='white')
    sheet.save(out/'cold-boot-contact.png')
    emu.press('START',after=120);emu.shot(out/'title-menu.png')
    emu.state(str(out/'title-menu.state'))
    emu.close()
    print(json.dumps({'frames':args.frames,'video':str(out/'cold-boot.mp4'),'contact':str(out/'cold-boot-contact.png')}))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rom');p.add_argument('--probe',required=True);p.add_argument('--out',required=True);p.add_argument('--frames',type=int,default=2400);coldboot(p.parse_args())
