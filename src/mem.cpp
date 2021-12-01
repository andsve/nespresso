#include "nsp.h"
#include "nsp_log.h"

/*
NES CPU address space
https://wiki.nesdev.com/w/index.php/CPU_memory_map

    +---------------+ (0x10000)
    |    PRG ROM    |
    |  (upper bank) |          char* prgrom_upper;
    +---------------+ 0xC000
    |    PRG ROM    |
    |  (lower bank) |          char* prgrom_lower;
    +---------------+ 0x8000
    |      SRAM     |
    +---------------+ 0x6000
    | Expansion ROM |
    +---------------+ 0x4020
    | I/O Registers |
    +---------------+ 0x4000
    |    Mirrors    |
    | 0x2000-0x2007 |
    +---------------+ 0x0800
    |      RAM      |
    +---------------+ 0x0200
    |     Stack     |
    +---------------+ 0x0100
    |   Zero Page   |
    +---------------+ 0x0000

*/

uint8_t nsp::memory_read(emu_t& emu, uint16_t addr, bool peek)
{
    cpu_t& cpu = emu.cpu;

    if (addr < 0x0100) {
        return cpu.ram[addr];
    } else if (addr < 0x0200) {
        return cpu.stack[addr - 0x0100];
    } else if (addr < 0x4000) {
        return cpu.ram[addr - 0x0100];
    } else if (addr < 0xC000 && addr >= 0x8000) {
        return cpu.prgrom_lower[addr - 0x8000];
    } else if (addr >= 0xC000) {
        return cpu.prgrom_upper[addr - 0xC000];
    }

    return 0xFF;
}

uint8_t nsp::memory_write(emu_t& emu, uint16_t addr, uint8_t data)
{
    cpu_t& cpu = emu.cpu;
    uint8_t prev = 0xff;

    // zero page
    if (addr < 0x0100) {
        prev = cpu.ram[addr];
        cpu.ram[addr] = data;

    // stack
    } else if (addr < 0x0200) {
        prev = cpu.stack[addr - 0x0100];
        cpu.stack[addr - 0x0100] = data;

    // non-zero page RAM
    } else if (addr < 0x4000) {
        prev = cpu.ram[addr - 0x0100];
        cpu.ram[addr - 0x0100] = data;
    }

    return prev;
}

void nsp::stack_push(emu_t& emu, uint8_t data)
{
    memory_write(emu, 0x0100 + emu.cpu.regs.S, data);
    emu.cpu.regs.S -= 0x1;
}
