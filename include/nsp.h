#ifndef NSP_H
#define NSP_H

#include <stdlib.h>
#include <stdint.h>

namespace nsp
{
    enum RESULT
    {
        RESULT_ERROR = 0,
        RESULT_OK = 1,
    };

    struct ines_rom_t
    {
        uint8_t prg_page_count;
        uint8_t chr_page_count;

        uint8_t** prg_pages;
        uint8_t** chr_pages;
    };

    struct cpu_t
    {
        // Registers
        struct regs_t
        {
            union {
                struct {
                    /*
                    7  bit  0
                    ---- ----
                    NVss DIZC
                    |||| ||||
                    |||| |||+- Carry
                    |||| ||+-- Zero
                    |||| |+--- Interrupt Disable
                    |||| +---- Decimal
                    ||++------ No CPU effect, see: the B flag
                    |+-------- Overflow
                    +--------- Negative
                    */
                    uint8_t C : 1;
                    uint8_t Z : 1;
                    uint8_t I : 1;
                    uint8_t D : 1;
                    uint8_t B : 2;
                    uint8_t O : 1;
                    uint8_t N : 1;
                };
                uint8_t P;
            };
            uint8_t  A, X, Y;
            uint16_t PC;
            uint8_t  S;
        } regs;

        struct vectors_t
        {
            uint16_t NMI;
            uint16_t RESET;
            uint16_t IRQBRK;
        } vectors;


        // RAM and Stack
        uint8_t ram[0x700];
        uint8_t stack[0x100];

        // Currently mapped prg rom banks
        uint8_t* prgrom_lower;
        uint8_t* prgrom_upper;

        uint8_t _4015;

        // Total cycles performed
        uint32_t cycles;

        // Per instruction
        uint8_t page_wraps;
        uint8_t extra_cycles;
    };

    struct emu_t
    {
        cpu_t cpu;
    };

    RESULT load_rom_file(const char* filepath, ines_rom_t& rom);
    RESULT load_rom_mem(const uint8_t* data, long int size, ines_rom_t& rom);

    RESULT init_emu(emu_t& emu, ines_rom_t& ines_rom);
    RESULT step_emu(emu_t& emu, uint32_t cycles);
    uint32_t step_cpu(emu_t& emu, uint32_t cycles);

    uint8_t memory_read(emu_t& emu, uint16_t addr, bool peek = false);
    uint16_t memory_read_short(emu_t& emu, uint16_t addr);
    uint16_t memory_read_short_zp_wrap(emu_t& emu, uint8_t addr);
    uint8_t memory_read_from_addr_ptr(emu_t &emu, uint16_t* addr_ptr);

    uint8_t memory_write(emu_t& emu, uint16_t addr, uint8_t data);

    void stack_push(emu_t& emu, uint8_t data);
    uint8_t stack_pop(emu_t& emu);
    void stack_push_short(emu_t& emu, uint16_t data);
    uint16_t stack_pop_short(emu_t& emu);

    void check_page_boundary(emu_t& emu, uint16_t addr1, uint16_t addr2);
    void check_page_boundary_index(emu_t& emu, uint16_t base, uint8_t index);

    uint8_t handle_register_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled);
    uint8_t handle_register_read(emu_t &emu, uint16_t addr, bool *handled, bool peek);
}

#endif /* NSP_H */
