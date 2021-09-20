#ifndef NSP_OPS_H
#define NSP_OPS_H

#include "nsp.h"

namespace nsp
{
    enum NES_OP_ADDR_MODES {
        Const                = 0,  // _const  imm = #$00
        Absolute             = 1,  // _abs    abs = $0000
        Absolute_ZP          = 2,  // _zp      zp = $00
        Accumulator          = 3,  // _a
        Index_X              = 4,  // _abs_x  abx = $0000,X
        Index_Y              = 5,  // _abs_y  aby = $0000,Y
        Index_ZP_X           = 6,  // _zp_x   zpx = $00,X
        Index_ZP_Y           = 7,  // _zp_y   zpy = $00,Y
        Indirect             = 8,  // _ind    ind = ($0000)
        PreIndex_Indirect_X  = 9,  // _ind_x  izx = ($00,X)
        PostIndex_Indirect_Y = 10, // _ind_y  izy = ($00),Y
        Relative             = 11, //         rel = $0000
        No_Address           = 12, //
        Unused               = 13  //
    };

    typedef void (nes_op_func_t)(emu_t& emu, cpu_t& cpu, NES_OP_ADDR_MODES addr_mode,
                             uint8_t instr, uint16_t& addr, uint16_t* addr_ptr);

    extern NES_OP_ADDR_MODES NES_OP_ADDR_MODE_LUT[256];
    extern uint8_t NES_INSTR_CYCLES_LUT[256];
    extern uint8_t NES_INSTR_CYCLES_EXTRA_LUT[256];
    extern nes_op_func_t* NES_OP_JT[256];
}



#endif /* NSP_OPS_H */
