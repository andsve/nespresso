#include <cstring>

#include "nsp.h"
#include "nsp_log.h"

nsp::RESULT nsp::init_emu(emu_t& emu, ines_rom_t& ines_rom)
{
    cpu_t& cpu = emu.cpu;
    cpu_t::regs_t& regs = cpu.regs;
    cpu_t::vectors_t& vectors = cpu.vectors;

    // Set emulator in powerup state, see
    // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
    regs.PC = 0xC000; // handy for nestest!
    regs.S = 0xFD;
    regs.P = 0x24;
    regs.A = regs.X = regs.Y = 0x0;

    // Clear RAM and stack
    memset(cpu.ram, 0, 0x700);
    memset(cpu.stack, 0, 0x100);

    // Map PRG ROM
    if (ines_rom.prg_page_count == 1) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[0];
    } else if (ines_rom.prg_page_count == 2) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[1];
    } else {
        LOG_E("TODO Solve mapping for more than three PRG ROM bank.");
        return RESULT_ERROR;
    }

    // Try to grab the interrupt vectors
    // NOTE: this might not work at all with different mappers!
    vectors.NMI = memory_read_short(emu, 0xFFFA);
    vectors.RESET = memory_read_short(emu, 0xFFFC);
    vectors.IRQBRK = memory_read_short(emu, 0xFFFE);

    LOG_D("Reset vectors:");
    LOG_D(" -> NMI    = 0x%04X", vectors.NMI);
    LOG_D(" -> RESET  = 0x%04X", vectors.RESET);
    LOG_D(" -> IRQBRK = 0x%04X", vectors.IRQBRK);

    // Reset program counter to reset vector
    emu.cpu.regs.PC = vectors.RESET;

    return RESULT_OK;
}

nsp::RESULT nsp::step_emu(emu_t& emu, uint32_t cycles)
{
    uint32_t t_tot_cycles = 0;
    while (t_tot_cycles < cycles)
    {
        uint32_t incr_cycles = step_cpu(emu, 1);
        // step_ppu(emu, incr_cycles*3);
        t_tot_cycles += incr_cycles;
    }
    return RESULT_OK;
}
