#include <string.h>

#include <MiniFB.h>

#include "nsp.h"
#include "nsp_log.h"
#include "nsp_nestest_logger.h"

#include "bitmap_ops.h"
#include "ppu_debug.h"

#define NESTEST_PATH "../data/nestest.nes"

uint32_t nsp::window_buffer[NES_WIDTH * NES_HEIGHT * 4];

static void minifb_keyboard_cb(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    // We are only interested in key releases
    if (isPressed) return;

    switch (key) {
        case KB_KEY_ESCAPE:
            mfb_close(window);
            break;
        default:
            break;
    }
}

int main(int argc, char const *argv[])
{
    const char* rom_filepath = NESTEST_PATH;

    if (argc > 1) {
        rom_filepath = argv[1];
    }

    // Load ROM and dump info
    nsp::ines_rom_t rom;
    nsp::RESULT result = nsp::load_rom_file(rom_filepath, rom);
    if (nsp::RESULT_OK != result) return 1;
    LOG_D(" -> PRG page count: %d", rom.prg_page_count);
    LOG_D(" -> CHR page count: %d", rom.chr_page_count);


    // Pass ROM along to our emulator/system
    nsp::emu_t emu;
    result = nsp::init_emu(emu, rom);
    if (nsp::RESULT_OK != result) return 1;

    struct mfb_window *window = 0x0;

    clear_window_buffer(255, 0, 0);
    window = mfb_open_ex("nespresso", NES_WIDTH, NES_HEIGHT, WF_RESIZABLE);
    mfb_set_user_data(window, (void*)&emu);
    mfb_set_keyboard_callback(window, minifb_keyboard_cb);

    // Run the emulator!
    do
    {
        result = nsp::step_emu(emu, 29781); // around 60 NES frames per 60 "real" frames
        if (nsp::RESULT_OK != result) return 1;

        // this aint how it actually works, but we just want to see pretty pixels
        dimm_window_buffer(0.0f);
        // dump_chr_rom_debug(emu);
        dump_ppu_nametable_debug(emu);
        // dump_ppu_vram(emu);
        // dump_ppu_sprites(emu);

        int32_t state = mfb_update_ex(window, nsp::window_buffer, NES_WIDTH, NES_HEIGHT);
        if (state < 0)
            break;

    } while(mfb_wait_sync(window));
    mfb_close(window);

    return 0;
}
