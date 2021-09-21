#include <cstring>

#include "nsp.h"
#include "nsp_log.h"

nsp::RESULT nsp::init_emu(emu_t& emu, ines_rom_t& ines_rom)
{
    cpu_t& cpu = emu.cpu;
    ppu_t& ppu = emu.ppu;
    cpu_t::regs_t& regs = cpu.regs;
    cpu_t::vectors_t& vectors = cpu.vectors;

    // Set emulator in powerup state, see
    // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
    regs.PC = 0xC000; // handy for nestest!
    regs.S = 0xFD;
    regs.P = 0x24;
    regs.A = regs.X = regs.Y = 0x0;
    cpu.cycles = 0;

    // Reset PPU
    ppu.cycles = ppu.x = ppu.y = 0;
    ppu.ppuctrl = ppu.ppumask = ppu.ppustatus = 0x0;
    ppu.ppuaddr_msb = 1;

    // Clear RAM, stack and VRAM
    memset(cpu.ram, 0, 0x700);
    memset(cpu.stack, 0, 0x100);
    memset(ppu.vram, 0, 0x800);
    memset(ppu.palette, 0, 0xFF);

    // Map PRG ROM
    if (ines_rom.prg_page_count == 1) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[0];
    } else if (ines_rom.prg_page_count == 2) {
        cpu.prgrom_lower = ines_rom.prg_pages[0];
        cpu.prgrom_upper = ines_rom.prg_pages[1];
    } else {
        LOG_E("TODO: Solve mapping for more than two PRG ROM bank.");
        return RESULT_ERROR;
    }

    // Map CHR ROM
    if (ines_rom.chr_page_count == 1) {
        ppu.chr_rom = ines_rom.chr_pages[0];
    } else {
        LOG_E("TODO: Solve mapping for zero or more than one CHR ROM bank.");
        return RESULT_ERROR;
    }

    // Try to grab the interrupt vectors
    vectors.NMI = memory_read_short(emu, 0xFFFA);
    vectors.RESET = memory_read_short(emu, 0xFFFC);
    vectors.IRQBRK = memory_read_short(emu, 0xFFFE);

    LOG_D("Reset vectors:");
    LOG_D(" -> NMI    = 0x%04X", vectors.NMI);
    LOG_D(" -> RESET  = 0x%04X", vectors.RESET);
    LOG_D(" -> IRQBRK = 0x%04X", vectors.IRQBRK);

    // Reset program counter to reset vector
    cpu.regs.PC = vectors.RESET;

    return RESULT_OK;
}

nsp::RESULT nsp::step_emu(emu_t& emu, uint32_t max_cycles)
{
    uint32_t delta_cycles = 0;
    while (delta_cycles < max_cycles)
    {
        uint32_t cpu_cycles = step_cpu(emu, 1);
        step_ppu(emu, cpu_cycles*3);
        delta_cycles += cpu_cycles;
    }
    return RESULT_OK;
}
