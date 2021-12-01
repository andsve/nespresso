#include <stdio.h>

#include "nsp_nestest_logger.h"
#include "nsp_log.h"


static char log_file[] = "../data/nestest2.log";

static char NES_OPS_LUT_SHORT[256][24] = {
    " BRK",
    " ORA",
    " Future Expansion (3)",
    "*SLO",
    "*NOP", // UFO(4)(5)
    " ORA",
    " ASL",
    "*SLO",
    " PHP",
    " ORA",
    " ASL",
    " Future Expansion (12)",
    "*NOP",
    " ORA",
    " ASL",
    "*SLO",
    " BPL",
    " ORA",
    " Future Expansion (19)",
    "*SLO",
    "*NOP",
    " ORA",
    " ASL",
    "*SLO",
    " CLC",
    " ORA",
    "*NOP",
    "*SLO",
    "*NOP",
    " ORA",
    " ASL",
    "*SLO",
    " JSR",
    " AND",
    " Future Expansion (35)",
    "*RLA",
    " BIT",
    " AND",
    " ROL",
    "*RLA",
    " PLP",
    " AND",
    " ROL",
    " Future Expansion (44)",
    " BIT",
    " AND",
    " ROL",
    "*RLA",
    " BMI",
    " AND",
    " Future Expansion (51)",
    "*RLA",
    "*NOP",
    " AND",
    " ROL",
    "*RLA",
    " SEC",
    " AND",
    "*NOP",
    "*RLA",
    "*NOP",
    " AND",
    " ROL",
    "*RLA",
    " RTI",
    " EOR",
    " Future Expansion (67)",
    "*SRE",
    "*NOP",
    " EOR",
    " LSR",
    "*SRE",
    " PHA",
    " EOR",
    " LSR",
    " Future Expansion (76)",
    " JMP",
    " EOR",
    " LSR",
    "*SRE",
    " BVC",
    " EOR",
    " Future Expansion (83)",
    "*SRE",
    "*NOP",
    " EOR",
    " LSR",
    "*SRE",
    " CLI",
    " EOR",
    "*NOP",
    "*SRE",
    "*NOP",
    " EOR",
    " LSR",
    "*SRE",
    " RTS",
    " ADC",
    " Future Expansion (99)",
    "*RRA",
    "*NOP",
    " ADC",
    " ROR",
    "*RRA",
    " PLA",
    " ADC",
    " ROR",
    " Future Expansion (108)",
    " JMP",
    " ADC",
    " ROR",
    "*RRA",
    " BVS",
    " ADC",
    " Future Expansion (115)",
    "*RRA",
    "*NOP",
    " ADC",
    " ROR",
    "*RRA",
    " SEI",
    " ADC",
    "*NOP",
    "*RRA",
    "*NOP",
    " ADC",
    " ROR",
    "*RRA",
    "*NOP",
    " STA",
    " Future Expansion (131)",
    "*SAX",
    " STY",
    " STA",
    " STX",
    "*SAX",
    " DEY",
    " Future Expansion (138)",
    " TXA",
    " Future Expansion (140)",
    " STY",
    " STA",
    " STX",
    "*SAX",
    " BCC",
    " STA",
    " Future Expansion (147)",
    " Future Expansion (148)",
    " STY",
    " STA",
    " STX",
    "*SAX",
    " TYA",
    " STA",
    " TXS",
    " Future Expansion (156)",
    " Future Expansion (157)",
    " STA",
    " Future Expansion (159)",
    " Future Expansion (160)",
    " LDY",
    " LDA",
    " LDX",
    "*LAX",
    " LDY",
    " LDA",
    " LDX",
    "*LAX",
    " TAY",
    " LDA",
    " TAX",
    " Future Expansion (172)",
    " LDY",
    " LDA",
    " LDX",
    "*LAX",
    " BCS",
    " LDA",
    " Future Expansion (179)",
    "*LAX",
    " LDY",
    " LDA",
    " LDX",
    "*LAX",
    " CLV",
    " LDA",
    " TSX",
    " Future Expansion (188)",
    " LDY",
    " LDA",
    " LDX",
    "*LAX",
    " CPY",
    " CMP",
    " Future Expansion (195)",
    "*DCP",
    " CPY",
    " CMP",
    " DEC",
    "*DCP",
    " INY",
    " CMP",
    " DEX",
    " Future Expansion (204)",
    " CPY",
    " CMP",
    " DEC",
    "*DCP",
    " BNE",
    " CMP",
    " Future Expansion (211)",
    "*DCP",
    "*NOP",
    " CMP",
    " DEC",
    "*DCP",
    " CLD",
    " CMP",
    "*NOP",
    "*DCP",
    "*NOP",
    " CMP",
    " DEC",
    "*DCP",
    " CPX",
    " SBC",
    " Future Expansion (227)",
    "*ISB",
    " CPX",
    " SBC",
    " INC",
    "*ISB",
    " INX",
    " SBC",
    " NOP",
    "*SBC",
    " CPX",
    " SBC",
    " INC",
    "*ISB",
    " BEQ",
    " SBC",
    " Future Expansion (243)",
    "*ISB",
    "*NOP",
    " SBC",
    " INC",
    "*ISB",
    " SED",
    " SBC",
    "*NOP",
    "*ISB",
    "*NOP",
    " SBC",
    " INC",
    "*ISB",
};

static nsp::emu_t* emu;
static nsp::cpu_t* cpu;
static FILE* logfp = 0x0;
static char log_line[256];
static char emu_line[256];
static char emu_regstate[256];
static uint32_t line_number = 0;
static uint8_t emu_line_cursor = 0;
static uint32_t emu_ticks = 0;
static uint8_t emu_instr = 0;
static nsp::NES_OP_ADDR_MODES emu_addr_mode = nsp::Unused;
static bool validate = true;

static void tmp_cb_debug_fetch_instr(uint8_t instruction, uint32_t ticks)
{
    emu_ticks = ticks;
    emu_instr = instruction;
    emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%04X  %02X ", cpu->regs.PC, instruction);
}

static void tmp_cb_debug_addr_mode(nsp::NES_OP_ADDR_MODES mode)
{
    emu_addr_mode = mode;
}

static void tmp_cb_debug_mem_write(uint16_t addr, uint8_t prev_data, uint8_t new_data)
{
    emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "= %02X", prev_data);
}

static void tmp_cb_debug_reg_write(char reg)
{
    emu_line_cursor += sprintf(&emu_line[emu_line_cursor], " %c", reg);
}

static void tmp_cb_debug_mem_read(uint16_t addr1, uint16_t addr2, uint16_t data)
{
    // printf("tmp_cb_debug_mem_read\n");
    switch (emu_addr_mode)
    {
        case nsp::Const:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s #$%02X ", data, NES_OPS_LUT_SHORT[emu_instr], data);
        break;

        case nsp::Absolute:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X %02X %s $%04X ", (0x00FF & addr1), (0xFF00 & addr1) >> 8, NES_OPS_LUT_SHORT[emu_instr], addr1);
        break;

        case nsp::Absolute_ZP:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s $%02X ", addr1, NES_OPS_LUT_SHORT[emu_instr], 0xFF & addr1);
        break;

        case nsp::Index_X:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X %02X %s $%04X,X @ %04X ", (0x00FF & addr1), (0xFF00 & addr1) >> 8, NES_OPS_LUT_SHORT[emu_instr], addr1, (unsigned short)(addr1 + cpu->regs.X));
        break;

        case nsp::Index_Y:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X %02X %s $%04X,Y @ %04X ", (0x00FF & addr1), (0xFF00 & addr1) >> 8, NES_OPS_LUT_SHORT[emu_instr], addr1, (unsigned short)(addr1 + cpu->regs.Y));
        break;

        case nsp::Index_ZP_X:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s $%02X,X @ %02X ", addr1, NES_OPS_LUT_SHORT[emu_instr], addr1, addr2);
        break;

        case nsp::Index_ZP_Y:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s $%02X,Y @ %02X ", addr1, NES_OPS_LUT_SHORT[emu_instr], 0xFF & addr1, addr2);
        break;

        case nsp::Indirect:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X %02X %s ($%04X) = %04X ", (0x00FF & addr1), (0xFF00 & addr1) >> 8, NES_OPS_LUT_SHORT[emu_instr], data, addr2);
        break;

        case nsp::PreIndex_Indirect_X:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s ($%02X,X) @ %02X = %04X ", addr1, NES_OPS_LUT_SHORT[emu_instr], addr1, (unsigned char)(addr1 + cpu->regs.X), addr2);
        break;

        case nsp::PostIndex_Indirect_Y:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s ($%02X),Y = %04X @ %04X ", addr1, NES_OPS_LUT_SHORT[emu_instr], addr1, addr2, data);
        break;

        case nsp::Relative:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "%02X    %s $%04X ", addr1, NES_OPS_LUT_SHORT[emu_instr], addr2);
        break;

        case nsp::Accumulator:
            data = cpu->regs.A;
        case nsp::No_Address:
        case nsp::Unused:
            emu_line_cursor += sprintf(&emu_line[emu_line_cursor], "      %s", NES_OPS_LUT_SHORT[emu_instr]);
            break;

        default:
            LOG_E("Unknown addressing mode.");
            break;
    };
}

static void tmp_cb_debug_pre_exec()
{
    sprintf(emu_regstate, "A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%d", cpu->regs.A, cpu->regs.X, cpu->regs.Y, cpu->regs.P, cpu->regs.S, emu->ppu.y, emu->ppu.x, emu_ticks);
}

static void tmp_cb_debug_post_exec()
{
    emu_line_cursor += sprintf(emu_line, "%-48s%s", emu_line, emu_regstate);
}

static void tmp_cb_debug_instr_done()
{
    emu_line_cursor = 0;
}

nsp::cb_debug_fetch_instr_t nsp::cb_debug_fetch_instr = 0x0;
nsp::cb_debug_addr_mode_t   nsp::cb_debug_addr_mode = 0x0;
nsp::cb_debug_reg_write_t   nsp::cb_debug_reg_write = 0x0;
nsp::cb_debug_mem_write_t   nsp::cb_debug_mem_write = 0x0;
nsp::cb_debug_mem_read_t    nsp::cb_debug_mem_read = 0x0;
nsp::cb_debug_pre_exec_t    nsp::cb_debug_pre_exec = 0x0;
nsp::cb_debug_post_exec_t   nsp::cb_debug_post_exec = 0x0;
nsp::cb_debug_instr_done_t  nsp::cb_debug_instr_done = 0x0;




bool nsp::attach_nestest_logger(nsp::emu_t* pemu, nsp::cpu_t* pcpu, bool validate_log)
{
    emu = pemu;
    cpu = pcpu;
    cb_debug_fetch_instr = tmp_cb_debug_fetch_instr;
    cb_debug_addr_mode = tmp_cb_debug_addr_mode;
    cb_debug_reg_write = tmp_cb_debug_reg_write;
    cb_debug_mem_write = tmp_cb_debug_mem_write;
    cb_debug_mem_read = tmp_cb_debug_mem_read;
    cb_debug_pre_exec = tmp_cb_debug_pre_exec;
    cb_debug_post_exec = tmp_cb_debug_post_exec;
    cb_debug_instr_done = tmp_cb_debug_instr_done;

    validate = validate_log;

    if (validate)
        logfp = fopen(log_file, "r");

    return true;
}

static bool read_log_line()
{
    if (!validate)
        return true;

    return fgets(log_line, 256, logfp) != NULL;
}

static uint32_t print_diff(uint32_t line_number, const char* outbuffer, const char* logbuffer)
{
    int c = 0;
    uint32_t misses = 0;
    printf("L%04d ", line_number);

    if (!validate)
    {
        printf("%s\n", outbuffer);
        return 0;
    }

    while (true
        && outbuffer[c] != '\n'
        && logbuffer[c] != '\n'
        && outbuffer[c] != '\0'
        && logbuffer[c] != '\0'
        )
    {
        if (outbuffer[c] != '-' && outbuffer[c] != logbuffer[c]) // treat - as ignore for now
        {
#ifdef _WIN32
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | BACKGROUND_RED );
#else
            printf("\x1B[0;30;41m");
#endif
            misses += 1;
        }

        printf("%c", outbuffer[c]);

        if (outbuffer[c] != logbuffer[c])
        {
#ifdef _WIN32
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#else
            printf("\x1B[0;;m");
#endif
        }

        c += 1;
    }

    printf("\n");

    return misses;
}

bool nsp::validate_nestest()
{
    if (!read_log_line())
    {
        LOG_I("End of log, all \x1B[0;30;42mOK\x1B[0;;m!");
        return false;
    }

    if (print_diff(++line_number, emu_line, log_line) > 0)
    {
        LOG_E("Failed at line line_number: %d", line_number);
        LOG_E("EMU: %s", emu_line);
        LOG_E("LOG: %s", log_line);
        return false;
    }

    return true;
}
