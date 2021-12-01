#include "nsp.h"
#include "nsp_log.h"
#include "nsp_ops.h"

uint32_t nsp::step_cpu(emu_t& emu, uint32_t max_cycles)
{
    cpu_t& cpu = emu.cpu;

    uint8_t instr;
    uint16_t temp;
    uint16_t addr;
    uint16_t* addr_ptr;

    uint32_t delta_cycles = 0;
    uint32_t start_cycles = cpu.cycles;
    uint32_t instr_cycles = 0;

    while (max_cycles > delta_cycles)
    {
        // Clear instruction specific cycle counters
        instr_cycles = 0;
        cpu.page_wraps = 0;
        cpu.extra_cycles = 0;

        // Fetch instruction
        instr = memory_read(emu, cpu.regs.PC);
        cpu.regs.PC += 1;

        // Address mode resolve
        NES_OP_ADDR_MODES addr_mode = NES_OP_ADDR_MODE_LUT[instr];

        // Default to using the addr variable as operator address
        addr_ptr = &addr;

        // Fetch address (or data) based on address mode of instruction
        switch (addr_mode)
        {
            case Const:
                addr = cpu.regs.PC;
                cpu.regs.PC += 0x1;
            break;

            case Absolute:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                cpu.regs.PC += 0x2;
            break;

            case Absolute_ZP:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                cpu.regs.PC += 0x1;
            break;

            case Index_X:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                temp = addr;

                check_page_boundary_index(emu, addr, cpu.regs.X);
                addr += cpu.regs.X;

                cpu.regs.PC += 0x2;
            break;

            case Index_Y:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                temp = addr;

                check_page_boundary_index(emu, addr, cpu.regs.Y);
                addr += cpu.regs.Y;

                cpu.regs.PC += 0x2;
            break;

            case Index_ZP_X:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = (uint8_t)(addr + cpu.regs.X);

                cpu.regs.PC += 0x1;
            break;

            case Index_ZP_Y:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = (uint8_t)(addr + cpu.regs.Y);

                cpu.regs.PC += 0x1;
            break;

            case Indirect:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = memory_read_short(emu, addr);

                /*
                Take care of the JMP-bug on the NES cpu:
                An original 6502 has does not correctly fetch the target address if
                the indirect vector falls on a page boundary (e.g. $xxFF where xx is
                and value from $00 to $FF). In this case fetches the LSB from $xxFF
                as expected but takes the MSB from $xx00.
                */
                if ((temp & 0x00FF) == 0x00FF)
                {
                    addr = ((uint16_t)memory_read(emu, temp & 0xFF00) << 8) | memory_read(emu, temp);
                }

                cpu.regs.PC += 0x2;
            break;

            case PreIndex_Indirect_X:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = memory_read_short_zp_wrap(emu, addr + cpu.regs.X);

                cpu.regs.PC += 0x1;
            break;

            case PostIndex_Indirect_Y:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = memory_read_short_zp_wrap(emu, addr);
                check_page_boundary_index(emu, addr, cpu.regs.Y);
                addr = addr + cpu.regs.Y;

                cpu.regs.PC += 0x1;
            break;

            case Relative:
                addr = memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = cpu.regs.PC + 0x1 + (int8_t)addr;

                check_page_boundary(emu, cpu.regs.PC + 0x1, addr);

                cpu.regs.PC += 0x1;
            break;

            case Accumulator:
                addr_ptr = (uint16_t*)&cpu.regs.A;
            case No_Address:
            case Unused:
                break;

            default:
                LOG_E("Unknown addressing mode.");
                break;
        };

        // Interpret and execute instruction
        NES_OP_JT[instr](emu, cpu, addr_mode, instr, addr, addr_ptr);

        // Get approx cycle count for current instruction
        instr_cycles += NES_INSTR_CYCLES_LUT[instr];

        cpu.cycles += instr_cycles;
        delta_cycles = cpu.cycles - start_cycles;
        if (delta_cycles == 0) {
            delta_cycles = 1;
        }
    }

    // Return how many cycles we were actually able to execute during this step.
    // (We will never be able to execute the expected cycle count due to our implementation!)
    return delta_cycles;
}
