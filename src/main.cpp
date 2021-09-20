#include <cstring>

#include "nsp.h"
#include "nsp_log.h"
#include "nsp_nestest_logger.h"

#define NESTEST_PATH "../data/nestest.nes"

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
    if (nsp::RESULT_ROM_LOAD_OK != result) return 1;
    LOG_D("iNES Header %s", rom.ines_v2 ? "v2" : "v1");
    LOG_D(" -> Mapper ID: %d", rom.mapper_id);
    LOG_D(" -> PRG page count: %d", rom.prg_page_count);
    LOG_D(" -> CHR page count: %d", rom.chr_page_count);


    // Pass ROM along to our emulator/system
    nsp::emu_t emu;
    result = nsp::init_emu(emu, rom);
    if (nsp::RESULT_OK != result) return 1;

    if (validate_nestest_log)
    {
        // Attach nestest logging mechanism
        nsp::attach_nestest_logger(&emu, &emu.cpu);

        // If we are running nestest, make sure we start on C000 instead of reset vector!
        // see nestest.txt
        emu.cpu.regs.PC = 0xC000;
        emu.cpu.cycles = 7;
    }

    // Run the emulator!
    while (true)
    {
        result = nsp::step_emu(emu, 1);
        if (nsp::RESULT_OK != result) return 1;

        if (validate_nestest_log && !nsp::validate_nestest()) return 1;
    }

    return 0;
}
