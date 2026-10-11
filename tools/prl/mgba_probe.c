/* Headless mGBA harness for reproducible ROM QA, never linked into the game. */
#include <mgba/flags.h>
#include <mgba/core/core.h>
#include <mgba/core/log.h>
#include <mgba-util/vfs.h>
#include <stdio.h>
#include <stdlib.h>

static struct mCore *core;
static color_t pixels[240 * 160];

int prl_open(const char *rom, const char *save)
{
    core = mCoreFind(rom);
    if (!core || !core->init(core)) return 0;
    mCoreInitConfig(core, "prl-qa");
    mCoreConfigSetDefaultIntValue(&core->config, "mute", 1);
    mCoreConfigSetDefaultIntValue(&core->config, "videoSync", 0);
    mCoreConfigSetDefaultIntValue(&core->config, "audioSync", 0);
    core->loadConfig(core, &core->config);
    core->setVideoBuffer(core, pixels, 240);
    if (!mCoreLoadFile(core, rom)) return 0;
    if (save && *save && !mCoreLoadSaveFile(core, save, true)) return 0;
    core->reset(core);
    return 1;
}

void prl_close(void) { if (core) { core->deinit(core); core = NULL; } }
void prl_reset(void) { core->reset(core); }
void prl_frame(unsigned keys, unsigned count) {
    core->setKeys(core, keys);
    for (unsigned i = 0; i < count; ++i) core->runFrame(core);
}
const void *prl_pixels(void) { return pixels; }
unsigned prl_read(unsigned address, unsigned size) {
    if (size == 1) return core->busRead8(core, address);
    if (size == 2) return core->busRead16(core, address);
    return core->busRead32(core, address);
}
void prl_write(unsigned address, unsigned value, unsigned size) {
    if (size == 1) core->busWrite8(core, address, value);
    else if (size == 2) core->busWrite16(core, address, value);
    else core->busWrite32(core, address, value);
}
int prl_state(const char *path, int save) {
    struct VFile *vf = VFileOpen(path, save ? O_CREAT | O_TRUNC | O_WRONLY : O_RDONLY);
    if (!vf) return 0;
    int result = save ? mCoreSaveStateNamed(core, vf, 31) : mCoreLoadStateNamed(core, vf, 31);
    vf->close(vf);
    return result;
}
int prl_save(const char *path) {
    void *data = NULL;
    size_t size = core->savedataClone(core, &data);
    FILE *f = fopen(path, "wb");
    if (!f || !data) { free(data); return 0; }
    int result = fwrite(data, 1, size, f) == size;
    fclose(f); free(data); return result;
}
unsigned prl_reg(const char *name) {
    unsigned value = 0; core->readRegister(core, name, &value); return value;
}
int prl_setreg(const char *name, unsigned value) { return core->writeRegister(core, name, &value); }
void prl_step(unsigned count) { for (unsigned i = 0; i < count; ++i) core->step(core); }

/* Preserve the real CPU pipeline while invoking a native routine for QA setup.
 * Keep elapsed cycles/events: only the caller's CPU registers/pipeline are restored.
 */
#include <mgba/internal/arm/arm.h>
#include <mgba/internal/arm/isa-inlines.h>
unsigned prl_call(unsigned function, const unsigned *args, unsigned argc)
{
    struct ARMCore *cpu = core->cpu;
    struct ARMCore backup = *cpu;
    unsigned stack[8], result, count;
    for (unsigned i = 4; i < argc; i++) {
        stack[i - 4] = core->busRead32(core, cpu->gprs[13] + (i - 4) * 4);
        core->busWrite32(core, cpu->gprs[13] + (i - 4) * 4, args[i]);
    }
    cpu->halted = 0;
    cpu->cpsr.i = 0;
    cpu->cpsr.f = 1;
    _ARMSetMode(cpu, MODE_THUMB);
    for (unsigned i = 0; i < argc && i < 4; i++) cpu->gprs[i] = args[i];
    cpu->gprs[14] = 0x09FFFF01;
    cpu->gprs[15] = function & ~1;
    ThumbWritePC(cpu);
    for (count = 0; count < 1000000; count++) {
        if ((unsigned)cpu->gprs[15] >= 0x09FFFF00 && (unsigned)cpu->gprs[15] < 0x0A000000) break;
        core->step(core);
    }
    result = cpu->gprs[0];
    for (unsigned i = 4; i < argc; i++) core->busWrite32(core, backup.gprs[13] + (i - 4) * 4, stack[i - 4]);
    backup.cycles = cpu->cycles;
    backup.nextEvent = cpu->nextEvent;
    *cpu = backup;
    if (count == 1000000) { fprintf(stderr, "Native QA call timed out: %08X\n", function); abort(); }
    return result;
}
