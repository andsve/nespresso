#include <string.h>

#include "nsp.h"
#include "nsp_log.h"

#define NESTEST_PATH "../data/nestest.nes"

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

    // Run the emulator!
    while (true)
    {
        result = nsp::step_emu(emu, 1);
        if (nsp::RESULT_OK != result) return 1;
    }

    return 0;
}
