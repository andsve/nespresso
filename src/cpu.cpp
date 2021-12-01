#include "nsp.h"
#include "nsp_log.h"
#include "nsp_ops.h"

nsp::RESULT nsp::step_cpu(emu_t& emu)
{
    cpu_t& cpu = emu.cpu;

    uint8_t instr;
    uint16_t addr;

    // Fetch instruction
    instr = memory_read(emu, cpu.regs.PC);
    cpu.regs.PC += 1;

    // Address mode resolve
    NES_OP_ADDR_MODES addr_mode = NES_OP_ADDR_MODE_LUT[instr];

    // Fetch address (or data) based on address mode of instruction
    switch (addr_mode)
    {
        case Absolute:
            LOG_D("Absolute addressing mode!");
            addr = ((uint16_t)memory_read(emu, cpu.regs.PC+1) << 8) | memory_read(emu, cpu.regs.PC);
            cpu.regs.PC += 0x2;
        break;

        default:
            LOG_E("Unknown addressing mode.");
            break;
    };

    // Interpret and execute instructions
    switch (instr)
    {
        // JSR
        case 0x20:
        {
            LOG_D("Executing JSR!");
            uint16_t prev_PC = cpu.regs.PC-1;
            stack_push(emu, (0xFF00 & (prev_PC)) >> 8 );
            stack_push(emu, 0x00FF & prev_PC );
            cpu.regs.PC = addr;
        }
        break;
        default:
            LOG_E("Unknown instruction: 0x%02X", instr);
            return RESULT_ERROR;
            break;
    }

    return RESULT_OK;
}
