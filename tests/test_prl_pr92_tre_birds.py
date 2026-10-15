"""PR92: authenticated donor art must survive clean FireRed ROM compilation.

Never silently use icons from TRE's mislabelled 955-957 Icon archive; those
are other green creatures. These checks cover the battle art only.
"""
import base64
import hashlib
import struct
import subprocess
import sys
import unittest
import zlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
DONOR=ROOT/"assets/prl_tre_birds/tre_primal_birds.b64"


class TestTREPrimalBirdArt(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        subprocess.run([sys.executable, str(ROOT/"tools/prl_tre_bird_assets.py")],
                       cwd=ROOT, check=True)

    def test_verified_original_source(self):
        compressed=base64.b64decode(DONOR.read_text("ascii").strip(),validate=True)
        raw=zlib.decompress(compressed)
        self.assertEqual(len(raw),12576)
        self.assertEqual(hashlib.sha256(raw).hexdigest(),
                         "011c0368c41940e2ef9deb7bd7247fed191ff3e9711808ce61012dc2a57008cf")
        self.assertEqual(raw[0],0)

    def test_bird_front_back_palettes_are_generated_in_both_asset_modes(self):
        for species in ("articuno","zapdos","moltres"):
            directory=ROOT/"graphics/pokemon"/species
            for name, height in (("anim_front.png",128),
                                 ("anim_front_gba.png",128),
                                 ("back.png",64),
                                 ("back_gba.png",64)):
                data=(directory/name).read_bytes()
                self.assertEqual(data[:8],b"\x89PNG\r\n\x1a\n")
                width,out_height,depth,ctype=struct.unpack(">IIBB",data[16:26])
                self.assertEqual((width,out_height,depth,ctype),(64,height,8,3))
                self.assertIn(b"PLTE",data)
                self.assertIn(b"tRNS",data)
            for name in ("normal.pal","normal_gba.pal","shiny.pal","shiny_gba.pal"):
                pal=(directory/name).read_text("ascii")
                self.assertTrue(pal.startswith("JASC-PAL\n0100\n16\n"))

    def test_no_visuals_or_mechanics_outside_three_species(self):
        m=(ROOT/"Makefile").read_text()
        self.assertIn("tools/prl_tre_bird_assets.py",m)
        src=(ROOT/"tools/prl_tre_bird_assets.py").read_text()
        self.assertEqual(src.count('"articuno"'),1)
        self.assertEqual(src.count('"zapdos"'),1)
        self.assertEqual(src.count('"moltres"'),1)
        self.assertNotIn('write_if_changed(directory / "icon.png"',src)
        self.assertNotIn("SPECIES_",src)


if __name__=="__main__":
    unittest.main()
