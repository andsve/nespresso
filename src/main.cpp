#include <cstring>

#include <MiniFB.h>

#include "nsp.h"
#include "nsp_log.h"
#include "nsp_nestest_logger.h"

#include "bitmap_ops.h"
#include "ppu_debug.h"

#define NESTEST_PATH "../data/nestest.nes"

uint32_t nsp::window_buffer[NES_WIDTH * NES_HEIGHT * 4];

static void draw_spinner(nsp::emu_t& emu)
{
    static uint8_t spinner = 0;
    spinner++;
    // fill(0, 0, 4, 4, (spinner % 4 == 0) * 255, 0, 0);
    // fill(4, 0, 4, 4, (spinner % 4 == 1) * 255, 0, 0);
    // fill(4, 4, 4, 4, (spinner % 4 == 2) * 255, 0, 0);
    // fill(0, 4, 4, 4, (spinner % 4 == 3) * 255, 0, 0);

    fill(0, 0, spinner, 4, 255, 0, 0);
}

static bool show_debug = false;
static bool running = true;

static void minifb_keyboard_cb(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    nsp::emu_t* emup = (nsp::emu_t*)mfb_get_user_data(window);

    // We are only interested in key releases
    if (isPressed) return;

    switch (key) {
        case KB_KEY_ESCAPE:
            mfb_close(window);
            break;
        case KB_KEY_TAB:
            show_debug = !show_debug;
            break;
        case KB_KEY_SPACE:
            running = !running;
            break;
        case KB_KEY_RIGHT:
        {
            // step ~one frame
            nsp::RESULT result = nsp::step_emu(*emup, 29781);
            if (nsp::RESULT_OK != result) {
                mfb_close(window);
            }
            break;
        }
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

    bool validate_nestest_log = strncmp(rom_filepath, NESTEST_PATH, sizeof(NESTEST_PATH)) == 0;

    // Load ROM and dump info
    nsp::ines_rom_t rom;
    nsp::RESULT result = nsp::load_rom_file(rom_filepath, rom);
    if (nsp::RESULT_OK != result) return 1;
    LOG_D("iNES Header %s", rom.ines_v2 ? "v2" : "v1");
    LOG_D(" -> Mapper ID: %d", rom.mapper_id);
    LOG_D(" -> PRG page count: %d", rom.prg_page_count);
    LOG_D(" -> CHR page count: %d", rom.chr_page_count);


    // Pass ROM along to our emulator/system
    nsp::emu_t emu;
    result = nsp::init_emu(emu, rom);
    if (nsp::RESULT_OK != result) return 1;

    struct mfb_window *window = 0x0;
    if (validate_nestest_log)
    {
        // Attach nestest logging mechanism
        nsp::attach_nestest_logger(&emu, &emu.cpu);

        // If we are running nestest, make sure we start on C000 instead of reset vector!
        // see nestest.txt
        emu.cpu.regs.PC = 0xC000;
        emu.cpu.cycles = 7;
        emu.ppu.x = 21;
        emu.ppu.y = 0;
    } else {
        clear_window_buffer(255, 0, 0);
        window = mfb_open_ex("nespresso", NES_WIDTH, NES_HEIGHT, WF_RESIZABLE);
        mfb_set_user_data(window, (void*)&emu);
        mfb_set_keyboard_callback(window, minifb_keyboard_cb);
    }


    // Run the emulator!
    if (validate_nestest_log)
    {
        while (true)
        {
            result = nsp::step_emu(emu, 1);
            if (nsp::RESULT_OK != result || !nsp::validate_nestest()) return 1;
        }

    } else {

        do
        {
            if (running)
            {
                result = nsp::step_emu(emu, 29781); // around 60 NES frames per 60 "real" frames
                if (nsp::RESULT_OK != result) return 1;
            }

            // this aint how it actually works, but we just want to see pretty pixels
            dump_ppu_vram(emu);
            dump_ppu_sprites(emu);

            if (show_debug)
            {
                dimm_window_buffer(0.25f);
                draw_spinner(emu);
                draw_text(8, 12, "CPU Cycles: %u", emu.cpu.cycles);
                draw_text(8, 22, "PPU Cycles: %u", emu.ppu.cycles);
            }

            int32_t state = mfb_update_ex(window, nsp::window_buffer, NES_WIDTH, NES_HEIGHT);
            if (state < 0)
                break;

        } while(mfb_wait_sync(window));
    }

    mfb_close(window);

    return 0;
}
