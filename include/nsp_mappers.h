#ifndef NSP_MAPPERS
#define NSP_MAPPERS

#include "nsp.h"

namespace nsp
{
    enum MAPPERS
    {
        MAPPER_000,
        MAPPER_001,
        MAPPER_002,
        MAPPER_003,
        MAPPERS_COUNT
    };

    struct mapper_t
    {
        uint32_t mapper_id;

        uint8_t prg_page_count;
        uint8_t chr_page_count;
        uint8_t** prg_pages;
        uint8_t** chr_pages;

        mapper_t() : mapper_id(0) {}
        virtual void load_ines_rom(ines_rom_t& rom, const uint8_t* rom_data);
        virtual uint8_t* get_initial_lower_prg();
        virtual uint8_t* get_initial_upper_prg();
        virtual uint8_t* get_initial_chr();
        virtual uint8_t handle_mem_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled) { return 0; };
        virtual uint8_t handle_mem_read(emu_t &emu, uint16_t addr, bool *handled, bool peek) { return 0; };
    };

    extern mapper_t MAPPER_IMPL_default;
    struct mapper_001_t;
    struct mapper_002_t;
    struct mapper_003_t;

    // extern nsp::mapper_t nsp::MAPPERS_LUT[256];
    extern mapper_t* MAPPERS_LUT[MAPPERS_COUNT];
}
// typedef void (nes_op_func_t)(emu_t& emu, cpu_t& cpu, NES_OP_ADDR_MODES addr_mode,
//                              uint8_t instr, uint16_t& addr, uint16_t* addr_ptr);

//     extern NES_OP_ADDR_MODES NES_OP_ADDR_MODE_LUT[256];
//     extern uint8_t NES_INSTR_CYCLES_LUT[256];
//     extern uint8_t NES_INSTR_CYCLES_EXTRA_LUT[256];
//     extern nes_op_func_t* NES_OP_JT[256];


// nsp::NES_OP_ADDR_MODES nsp::NES_OP_ADDR_MODE_LUT[256]


#endif /* NSP_MAPPERS_H */
