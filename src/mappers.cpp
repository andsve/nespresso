#include "nsp.h"
#include "nsp_log.h"
#include "nsp_mappers.h"

using namespace nsp;

RESULT mapper_000_t::map_initial_prg(ines_rom_t& ines_rom, emu_t& emu) {
    cpu_t& cpu = emu.cpu;

    // Map PRG ROM
    if (ines_rom.prg_page_count == 1) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[0];
    } else if (ines_rom.prg_page_count == 2) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[1];
    } else {
        LOG_E("More than 2 PRG pages not supported for mapper 0.");
        return RESULT_ERROR;
    }

    return RESULT_OK;
}

RESULT mapper_001_t::map_initial_prg(ines_rom_t& ines_rom, emu_t& emu) {
    cpu_t& cpu = emu.cpu;
    // LOG_D("using mapper 1 - prg page count: %d", ines_rom.prg_page_count);
    // Map PRG ROM
    if (ines_rom.prg_page_count == 1) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[0];
    } else {
        // LOG_D(" -> ines_rom.prg_pages[0]: %p", ines_rom.prg_pages[0]);
        // LOG_D(" -> ines_rom.prg_pages[1]: %p", ines_rom.prg_pages[1]);
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[1];
    }

    return RESULT_OK;
}
