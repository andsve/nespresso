#ifndef NSP_NESTEST_LOGGER_H
#define NSP_NESTEST_LOGGER_H

#include "nsp.h"
#include "nsp_ops.h"

namespace nsp
{
    bool attach_nestest_logger(nsp::emu_t* emu, nsp::cpu_t* cpu);
    bool validate_nestest();

    typedef void (*cb_debug_fetch_instr_t)(uint8_t instruction, uint32_t ticks);
    typedef void (*cb_debug_addr_mode_t)(NES_OP_ADDR_MODES mode);
    typedef void (*cb_debug_mem_read_t)(uint16_t addr1, uint16_t addr2, uint16_t data);
    typedef void (*cb_debug_mem_write_t)(uint16_t addr, uint8_t prev_data, uint8_t new_data);
    typedef void (*cb_debug_reg_write_t)(char reg);
    typedef void (*cb_debug_pre_exec_t)();
    typedef void (*cb_debug_post_exec_t)();
    typedef void (*cb_debug_instr_done_t)();

    extern cb_debug_fetch_instr_t cb_debug_fetch_instr;
    extern cb_debug_addr_mode_t   cb_debug_addr_mode;
    extern cb_debug_reg_write_t   cb_debug_reg_write;
    extern cb_debug_mem_write_t   cb_debug_mem_write;
    extern cb_debug_mem_read_t    cb_debug_mem_read;
    extern cb_debug_pre_exec_t    cb_debug_pre_exec;
    extern cb_debug_post_exec_t   cb_debug_post_exec;
    extern cb_debug_instr_done_t  cb_debug_instr_done;


}

#endif /* NSP_NESTEST_LOGGER_H */
