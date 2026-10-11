import pathlib, subprocess, tempfile, unittest
ROOT = pathlib.Path(__file__).resolve().parents[1]

class CrystalRestoreTests(unittest.TestCase):
    def test_continuous_scenery_reveal_preserves_link_and_partner_intros(self):
        config = (ROOT / 'include/config/battle.h').read_text()
        setting = next(line for line in config.splitlines()
                       if line.startswith('#define B_FAST_INTRO_NO_SLIDE '))
        program = '''#include <assert.h>
#define FALSE 0
#define IS_FRLG firered
#define BATTLE_TYPE_LINK 1
#define BATTLE_TYPE_INGAME_PARTNER 2
unsigned gBattleTypeFlags;
int firered;
''' + setting + '''
int main(void) {
 firered=1; gBattleTypeFlags=0; assert(B_FAST_INTRO_NO_SLIDE);
 gBattleTypeFlags=BATTLE_TYPE_LINK; assert(!B_FAST_INTRO_NO_SLIDE);
 gBattleTypeFlags=BATTLE_TYPE_INGAME_PARTNER; assert(!B_FAST_INTRO_NO_SLIDE);
 firered=0; gBattleTypeFlags=0; assert(!B_FAST_INTRO_NO_SLIDE);
 return 0;
}'''
        with tempfile.TemporaryDirectory() as d:
            source=pathlib.Path(d)/'test.c'; source.write_text(program)
            subprocess.run(['gcc',str(source),'-o',d+'/test'],check=True)
            subprocess.run([d+'/test'],check=True)

    def test_standard_firered_trainers_use_pokeball_transition(self):
        code=(ROOT/'src/battle_setup.c').read_text()
        self.assertTrue('if (IS_FRLG)\n        return B_TRANSITION_BIG_POKEBALL;' in code)

    def test_visual_selector_preserves_water_and_uses_approved_locations(self):
        header = ROOT / 'src/data/prl_crystal_visuals.h'
        self.assertTrue(header.exists(), 'Crystal graphics are not integrated into source')
        program = '''#include <assert.h>
typedef unsigned char u8;
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#include "src/data/prl_crystal_visuals.h"
int main(void) {
 assert(PrlCrystalVisualSlot(0,35,0)==27);
 assert(PrlCrystalVisualSlot(4,35,0)==4);
 assert(PrlCrystalVisualSlot(5,37,20)==5);
 assert(PrlCrystalVisualSlot(11,40,2)==11);
 assert(PrlCrystalVisualSlot(12,48,3)==33);
 assert(PrlCrystalVisualSlot(0,37,29)==35);
 assert(PrlCrystalVisualSlot(19,0,0)==19);
 return 0;
}'''
        with tempfile.TemporaryDirectory() as d:
            source=pathlib.Path(d)/'test.c'; source.write_text(program)
            subprocess.run(['gcc','-I',str(ROOT),str(source),'-o',d+'/test'],check=True)
            subprocess.run([d+'/test'],check=True)

    def test_all_38_assets_are_loaded_without_changing_mechanical_environments(self):
        code=(ROOT/'src/battle_bg.c').read_text()
        self.assertIn('GetPrlCrystalVisual',code)
        data=(ROOT/'src/data/prl_crystal_assets.h').read_text()
        for i in range(38):
            self.assertIn(f'graphics/prl_crystal_redux/{i:02d}/tiles.4bpp.lz',data)
            self.assertIn(f'graphics/prl_crystal_redux/{i:02d}/palette_48colors.gbapal',data)
        self.assertNotIn('PRL_CRYSTAL', (ROOT/'include/constants/battle.h').read_text())

if __name__=='__main__': unittest.main()
