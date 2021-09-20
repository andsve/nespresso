#include "nsp.h"
#include "nsp_log.h"

/*
NES CPU visible memory
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

// NOTE: do we need peek if we havent implemented memmap?
uint8_t nsp::memory_read(emu_t& emu, uint16_t addr, bool peek)
{
    cpu_t& cpu = emu.cpu;
    // bool memmap_reg_handled = false;
    // unsigned char memmap_res = handle_memmap_reg_read(cpu, addr, &memmap_reg_handled, peek);
    // if (memmap_reg_handled) {
    //     return memmap_res;
    // }

    // zero page
    if (addr < 0x0100) {
        return cpu.ram[addr];

    // stack
    } else if (addr < 0x0200) {
        return cpu.stack[addr - 0x0100];

    // PPU registers
    } else if (addr >= 0x2000 && addr <= 0x2007) {
        // TODO

    // non-zero page RAM
    } else if (addr < 0x4000) {
        return cpu.ram[addr - 0x0100];

    // I/O registers
    } else if (addr < 0x6000) {
        // TODO Fix I/O reg read and mirroring

    // Save RAM
    } else if (addr < 0x8000) {
        // TODO Fix save RAM read

    // PRG ROM lower
    } else if (addr < 0xC000) {
        return cpu.prgrom_lower[addr - 0x8000];

    // PRG ROM upper
    } else {
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
        if (addr_ptr == (unsigned short*)&emu.cpu.regs.A) {
            return emu.cpu.regs.A;
        } else if (addr_ptr == (unsigned short*)&emu.cpu.regs.X) {
            return emu.cpu.regs.X;
        } else if (addr_ptr == (unsigned short*)&emu.cpu.regs.Y) {
            return emu.cpu.regs.Y;
        } else if (addr_ptr == (unsigned short*)&emu.cpu.regs.S) {
            return emu.cpu.regs.S;
        } else {
            return memory_read(emu, *addr_ptr);
        }
    }

uint8_t nsp::memory_write(emu_t& emu, uint16_t addr, uint8_t data)
{
    cpu_t& cpu = emu.cpu;
    uint8_t prev = 0xff;

    // bool memmap_reg_handled = false;
    // prev = handle_memmap_reg_write(cpu, addr, data, &memmap_reg_handled);
    // if (memmap_reg_handled) {
    //     return prev;
    // }

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

    // i/o registers
    } //else if (addr < 0x6000) {
    //     cpu._4015 = data;
    // }

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
