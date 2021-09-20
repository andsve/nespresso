#include "nsp.h"
#include "nsp_log.h"
#include "nsp_ops.h"
#include "nsp_nestest_logger.h"

nsp::RESULT nsp::step_cpu(emu_t& emu, uint32_t max_cycles)
{
    cpu_t& cpu = emu.cpu;

    // move all these into emu struct?
    uint8_t instr;
    uint16_t temp;
    uint16_t addr;
    uint16_t* addr_ptr;

    // static uint32_t total_cycles = 7;
    uint32_t delta_cycles = 0;
    uint32_t start_cycles = emu.cpu.cycles;
    uint32_t instr_cycles = 0;

    while (max_cycles > delta_cycles)
    {
        instr_cycles = 0;
        cpu.page_wraps = 0;
        cpu.extra_cycles = 0;

        // Fetch instruction
        instr = memory_read(emu, cpu.regs.PC);
        if (cb_debug_fetch_instr) {
            cb_debug_fetch_instr(instr, emu.cpu.cycles);
        }
        cpu.regs.PC += 1;

        // Address mode resolve
        NES_OP_ADDR_MODES addr_mode = NES_OP_ADDR_MODE_LUT[instr];
        if (cb_debug_addr_mode) {
            cb_debug_addr_mode(addr_mode);
        }

        // Default to using the addr variable as operator address
        addr_ptr = &addr;

        // Fetch address (or data) based on address mode of instruction
        switch (addr_mode)
        {
            case Const:
                addr = cpu.regs.PC;
                if (cb_debug_mem_read) cb_debug_mem_read(0x0, 0x0, memory_read(emu, addr, true));
                cpu.regs.PC += 0x1;
            break;

            case Absolute:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                if (cb_debug_mem_read) cb_debug_mem_read(addr, 0x0, memory_read(emu, addr, true));
                cpu.regs.PC += 0x2;
            break;

            case Absolute_ZP:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                if (cb_debug_mem_read) cb_debug_mem_read(addr, 0x0, memory_read(emu, addr, true));
                cpu.regs.PC += 0x1;
            break;

            case Index_X:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                temp = addr;

                check_page_boundary_index(emu, addr, cpu.regs.X);
                addr += cpu.regs.X;

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, memory_read(emu, addr, true));
                cpu.regs.PC += 0x2;
            break;

            case Index_Y:
                addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
                temp = addr;

                check_page_boundary_index(emu, addr, cpu.regs.Y);
                addr += cpu.regs.Y;

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, memory_read(emu, addr, true));
                cpu.regs.PC += 0x2;
            break;

            case Index_ZP_X:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = (unsigned char)(addr + cpu.regs.X);

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, memory_read(emu, addr, true));
                cpu.regs.PC += 0x1;
            break;

            case Index_ZP_Y:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = (unsigned char)(addr + cpu.regs.Y);

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, memory_read(emu, addr, true));
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

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, temp);
                cpu.regs.PC += 0x2;
            break;

            case PreIndex_Indirect_X:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = memory_read_short_zp_wrap(emu, addr + cpu.regs.X);

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, memory_read(emu, addr, true));
                cpu.regs.PC += 0x1;
            break;

            case PostIndex_Indirect_Y:
                addr = (uint16_t)memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = memory_read_short_zp_wrap(emu, addr);
                check_page_boundary_index(emu, addr, cpu.regs.Y);
                addr = addr + cpu.regs.Y;

                if (cb_debug_mem_read) cb_debug_mem_read(temp, memory_read_short_zp_wrap(emu, temp), addr);
                cpu.regs.PC += 0x1;
            break;

            case Relative:
                addr = memory_read(emu, cpu.regs.PC);
                temp = addr;
                addr = cpu.regs.PC + 0x1 + (int8_t)addr;

                check_page_boundary(emu, cpu.regs.PC + 0x1, addr);

                if (cb_debug_mem_read) cb_debug_mem_read(temp, addr, addr);
                cpu.regs.PC += 0x1;
            break;

            case Accumulator:
                addr_ptr = (uint16_t*)&cpu.regs.A;
            case No_Address:
            case Unused:
                if (cb_debug_mem_read) cb_debug_mem_read(0x0, 0x0, addr);
                break;

            default:
                LOG_E("Unknown addressing mode.");
                break;
        };

        // Interpret and execute instruction
        if (cb_debug_pre_exec) cb_debug_pre_exec();
        NES_OP_JT[instr](emu, cpu, addr_mode, instr, addr, addr_ptr);

        // Get approx cycle count for current instruction
        instr_cycles += NES_INSTR_CYCLES_LUT[instr];

        // Add any extra page boundary wrap cycles
        if (NES_INSTR_CYCLES_EXTRA_LUT[instr] == 1)
        {
            instr_cycles += cpu.page_wraps;
        }
        else if (NES_INSTR_CYCLES_EXTRA_LUT[instr] == 2)
        {
            // Add any extra instruction cycles (mostly cycles from successful branching)
            instr_cycles += cpu.extra_cycles;

            // If branch was taken, we need to take into account page wraps
            if (cpu.extra_cycles) {
                instr_cycles += cpu.page_wraps;
            }
        }

        if (cb_debug_post_exec) cb_debug_post_exec();
        if (cb_debug_instr_done) cb_debug_instr_done();

        // if (sys.ppu.nmi_occurred) {
        //     sys.ppu.nmi_occurred = false;
        //     trigger_vblank_nmi(sys);
        //     // LOG_D("NMI TRIGGERD");

        //     t_ticks += 7;
        // }

        emu.cpu.cycles += instr_cycles;
        delta_cycles = emu.cpu.cycles - start_cycles;
        if (delta_cycles == 0) {
            delta_cycles = 1;
        }
    }

    return RESULT_OK;
}
