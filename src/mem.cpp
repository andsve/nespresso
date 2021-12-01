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
    bool memmap_reg_handled = false;
    uint8_t memmap_res = handle_memmap_reg_read(emu, addr, &memmap_reg_handled, peek);
    if (memmap_reg_handled) {
        return memmap_res;
    }

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

uint16_t nsp::memory_read_short(emu_t& emu, uint16_t addr)
{
    return ((uint16_t)memory_read(emu, addr+1) << 8) | memory_read(emu, addr);
}

uint16_t nsp::memory_read_short_zp_wrap(emu_t& emu, uint8_t addr)
{
    uint8_t hi_addr = addr+1;
    uint8_t lo_addr = addr;
    return ((uint16_t)memory_read(emu, hi_addr) << 8) | memory_read(emu, lo_addr);
}

uint8_t nsp::memory_read_from_addr_ptr(emu_t &emu, uint16_t* addr_ptr)
{
    if (addr_ptr == (uint16_t*)&emu.cpu.regs.A) {
        return emu.cpu.regs.A;
    } else if (addr_ptr == (uint16_t*)&emu.cpu.regs.X) {
        return emu.cpu.regs.X;
    } else if (addr_ptr == (uint16_t*)&emu.cpu.regs.Y) {
        return emu.cpu.regs.Y;
    } else if (addr_ptr == (uint16_t*)&emu.cpu.regs.S) {
        return emu.cpu.regs.S;
    } else {
        return memory_read(emu, *addr_ptr);
    }
}

uint8_t nsp::memory_write(emu_t& emu, uint16_t addr, uint8_t data)
{
    cpu_t& cpu = emu.cpu;
    uint8_t prev = 0xff;

    bool memmap_reg_handled = false;
    prev = handle_memmap_reg_write(emu, addr, data, &memmap_reg_handled);
    if (memmap_reg_handled) {
        return prev;
    }

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

uint8_t nsp::stack_pop(emu_t& emu)
{
    emu.cpu.regs.S += 0x1;
    return(memory_read(emu, 0x0100 + emu.cpu.regs.S));
}

void nsp::stack_push_short(emu_t& emu, uint16_t data)
{
    stack_push(emu, (0xFF00 & data) >> 8 );
    stack_push(emu, 0x00FF & data );
}

uint16_t nsp::stack_pop_short(emu_t& emu)
{
    uint16_t lo = (uint16_t)stack_pop(emu);
    uint16_t hi = (uint16_t)stack_pop(emu) << 8;
    return hi | lo;
}

void nsp::check_page_boundary(emu_t& emu, uint16_t addr1, uint16_t addr2)
{
    if ((addr1 & (0xFF00)) != (addr2 & (0xFF00)))
    {
        cpu_t& cpu = emu.cpu;
        cpu.page_wraps += 1;
    }
}

void nsp::check_page_boundary_index(emu_t& emu, uint16_t base, uint8_t index)
{
    if ((base & 0xFF) + index > 0xFF)
    {
        cpu_t& cpu = emu.cpu;
        cpu.page_wraps += 1;
    }
}

uint8_t nsp::handle_memmap_reg_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled)
{
    *handled = false;

    if ((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014) {

        *handled = true;
        return ppu_reg_write(emu, addr, data);

    } else if (addr == 0x4016 || addr == 0x4017) {
        *handled = true;
        LOG_D("Unhandled Joypads write!");
    }

    return 0x00;
}

uint8_t nsp::handle_memmap_reg_read(emu_t &emu, uint16_t addr, bool *handled, bool peek)
{
    *handled = false;

    if ((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014) {

        *handled = true;
        return ppu_reg_read(emu, addr, peek);

    } else if (addr == 0x4016 || addr == 0x4017) {
        LOG_D("Unhandled Joypads read!");
        *handled = true;
        return 0x40;
    }

    return 0x0;
}

uint8_t* nsp::dma_ptr(emu_t &emu, uint16_t addr)
{
    cpu_t& cpu = emu.cpu;
    if (addr < 0x0100)
    {
        return (uint8_t*)&cpu.ram[addr];
    } else if (addr < 0x0200) {
        return (uint8_t*)&cpu.stack[addr - 0x0100];
    } else if (addr < 0x4000) {
        return (uint8_t*)&cpu.ram[addr - 0x0100];
    } else if (addr < 0x6000) {
        // TODO Fix I/O reg read and mirroring
    } else if (addr < 0x8000) {
        // TODO Fix save RAM read
    } else if (addr < 0xC000) {
        return (uint8_t*)&cpu.prgrom_lower[addr - 0x8000];
    } else {
        return (uint8_t*)&cpu.prgrom_upper[addr - 0xC000];
    }

    return 0x0;
}
