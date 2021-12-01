#include "nsp.h"
#include "nsp_log.h"
#include "nsp_ops.h"
#include "nsp_nestest_logger.h"

nsp::NES_OP_ADDR_MODES nsp::NES_OP_ADDR_MODE_LUT[256] = {
    nsp::No_Address,               //   1
    nsp::PreIndex_Indirect_X,      //   2
    nsp::Unused,                   //   3
    nsp::PreIndex_Indirect_X,      //   4
    nsp::Absolute_ZP,              //   5
    nsp::Absolute_ZP,              //   6
    nsp::Absolute_ZP,              //   7
    nsp::Absolute_ZP,              //   8
    nsp::No_Address,               //   9
    nsp::Const,                    //  10
    nsp::Accumulator,              //  11
    nsp::Unused,                   //  12
    nsp::Absolute,                 //  13
    nsp::Absolute,                 //  14
    nsp::Absolute,                 //  15
    nsp::Absolute,                 //  16
    nsp::Relative,                 //  17
    nsp::PostIndex_Indirect_Y,     //  18
    nsp::Unused,                   //  19
    nsp::PostIndex_Indirect_Y,     //  20
    nsp::Index_ZP_X,               //  21
    nsp::Index_ZP_X,               //  22
    nsp::Index_ZP_X,               //  23
    nsp::Index_ZP_X,               //  24
    nsp::No_Address,               //  25
    nsp::Index_Y,                  //  26
    nsp::No_Address,               //  27
    nsp::Index_Y,                  //  28
    nsp::Index_X,                  //  29
    nsp::Index_X,                  //  30
    nsp::Index_X,                  //  31
    nsp::Index_X,                  //  32
    nsp::Absolute,                 //  33
    nsp::PreIndex_Indirect_X,      //  34
    nsp::Unused,                   //  35
    nsp::PreIndex_Indirect_X,      //  36
    nsp::Absolute_ZP,              //  37
    nsp::Absolute_ZP,              //  38
    nsp::Absolute_ZP,              //  39
    nsp::Absolute_ZP,              //  40
    nsp::No_Address,               //  41
    nsp::Const,                    //  42
    nsp::Accumulator,              //  43
    nsp::Unused,                   //  44
    nsp::Absolute,                 //  45
    nsp::Absolute,                 //  46
    nsp::Absolute,                 //  47
    nsp::Absolute,                 //  48
    nsp::Relative,                 //  49
    nsp::PostIndex_Indirect_Y,     //  50
    nsp::Unused,                   //  51
    nsp::PostIndex_Indirect_Y,     //  52
    nsp::Index_ZP_X,               //  53
    nsp::Index_ZP_X,               //  54
    nsp::Index_ZP_X,               //  55
    nsp::Index_ZP_X,               //  56
    nsp::No_Address,               //  57
    nsp::Index_Y,                  //  58
    nsp::No_Address,               //  59
    nsp::Index_Y,                  //  60
    nsp::Index_X,                  //  61
    nsp::Index_X,                  //  62
    nsp::Index_X,                  //  63
    nsp::Index_X,                  //  64
    nsp::No_Address,               //  65
    nsp::PreIndex_Indirect_X,      //  66
    nsp::Unused,                   //  67
    nsp::PreIndex_Indirect_X,      //  68
    nsp::Absolute_ZP,              //  69
    nsp::Absolute_ZP,              //  70
    nsp::Absolute_ZP,              //  71
    nsp::Absolute_ZP,              //  72
    nsp::No_Address,               //  73
    nsp::Const,                    //  74
    nsp::Accumulator,              //  75
    nsp::Unused,                   //  76
    nsp::Absolute,                 //  77
    nsp::Absolute,                 //  78
    nsp::Absolute,                 //  79
    nsp::Absolute,                 //  80
    nsp::Relative,                 //  81
    nsp::PostIndex_Indirect_Y,     //  82
    nsp::Unused,                   //  83
    nsp::PostIndex_Indirect_Y,     //  84
    nsp::Index_ZP_X,               //  85
    nsp::Index_ZP_X,               //  86
    nsp::Index_ZP_X,               //  87
    nsp::Index_ZP_X,               //  88
    nsp::No_Address,               //  89
    nsp::Index_Y,                  //  90
    nsp::No_Address,               //  91
    nsp::Index_Y,                  //  92
    nsp::Index_X,                  //  93
    nsp::Index_X,                  //  94
    nsp::Index_X,                  //  95
    nsp::Index_X,                  //  96
    nsp::No_Address,               //  97
    nsp::PreIndex_Indirect_X,      //  98
    nsp::Unused,                   //  99
    nsp::PreIndex_Indirect_X,      // 100
    nsp::Absolute_ZP,              // 101
    nsp::Absolute_ZP,              // 102
    nsp::Absolute_ZP,              // 103
    nsp::Absolute_ZP,              // 104
    nsp::No_Address,               // 105
    nsp::Const,                    // 106
    nsp::Accumulator,              // 107
    nsp::Unused,                   // 108
    nsp::Indirect,                 // 109
    nsp::Absolute,                 // 110
    nsp::Absolute,                 // 111
    nsp::Absolute,                 // 112
    nsp::Relative,                 // 113
    nsp::PostIndex_Indirect_Y,     // 114
    nsp::Unused,                   // 115
    nsp::PostIndex_Indirect_Y,     // 116
    nsp::Index_ZP_X,               // 117
    nsp::Index_ZP_X,               // 118
    nsp::Index_ZP_X,               // 119
    nsp::Index_ZP_X,               // 120
    nsp::No_Address,               // 121
    nsp::Index_Y,                  // 122
    nsp::No_Address,               // 123
    nsp::Index_Y,                  // 124
    nsp::Index_X,                  // 125
    nsp::Index_X,                  // 126
    nsp::Index_X,                  // 127
    nsp::Index_X,                  // 128
    nsp::Const,                    // 129
    nsp::PreIndex_Indirect_X,      // 130
    nsp::Unused,                   // 131
    nsp::PreIndex_Indirect_X,      // 132
    nsp::Absolute_ZP,              // 133
    nsp::Absolute_ZP,              // 134
    nsp::Absolute_ZP,              // 135
    nsp::Absolute_ZP,              // 136
    nsp::No_Address,               // 137
    nsp::Unused,                   // 138
    nsp::No_Address,               // 139
    nsp::Unused,                   // 140
    nsp::Absolute,                 // 141
    nsp::Absolute,                 // 142
    nsp::Absolute,                 // 143
    nsp::Absolute,                 // 144
    nsp::Relative,                 // 145
    nsp::PostIndex_Indirect_Y,     // 146
    nsp::Unused,                   // 147
    nsp::Unused,                   // 148
    nsp::Index_ZP_X,               // 149
    nsp::Index_ZP_X,               // 150
    nsp::Index_ZP_Y,               // 151
    nsp::Index_ZP_Y,               // 152
    nsp::No_Address,               // 153
    nsp::Index_Y,                  // 154
    nsp::No_Address,               // 155
    nsp::Unused,                   // 156
    nsp::Unused,                   // 157
    nsp::Index_X,                  // 158
    nsp::Unused,                   // 159
    nsp::Unused,                   // 160
    nsp::Const,                    // 161
    nsp::PreIndex_Indirect_X,      // 162
    nsp::Const,                    // 163
    nsp::PreIndex_Indirect_X,      // 164
    nsp::Absolute_ZP,              // 165
    nsp::Absolute_ZP,              // 166
    nsp::Absolute_ZP,              // 167
    nsp::Absolute_ZP,              // 168
    nsp::No_Address,               // 169
    nsp::Const,                    // 170
    nsp::No_Address,               // 171
    nsp::Unused,                   // 172
    nsp::Absolute,                 // 173
    nsp::Absolute,                 // 174
    nsp::Absolute,                 // 175
    nsp::Absolute,                 // 176
    nsp::Relative,                 // 177
    nsp::PostIndex_Indirect_Y,     // 178
    nsp::Unused,                   // 179
    nsp::PostIndex_Indirect_Y,     // 180
    nsp::Index_ZP_X,               // 181
    nsp::Index_ZP_X,               // 182
    nsp::Index_ZP_Y,               // 183
    nsp::Index_ZP_Y,               // 184
    nsp::No_Address,               // 185
    nsp::Index_Y,                  // 186
    nsp::No_Address,               // 187
    nsp::Unused,                   // 188
    nsp::Index_X,                  // 189
    nsp::Index_X,                  // 190
    nsp::Index_Y,                  // 191
    nsp::Index_Y,                  // 192
    nsp::Const,                    // 193
    nsp::PreIndex_Indirect_X,      // 194
    nsp::Unused,                   // 195
    nsp::PreIndex_Indirect_X,      // 196
    nsp::Absolute_ZP,              // 197
    nsp::Absolute_ZP,              // 198
    nsp::Absolute_ZP,              // 199
    nsp::Absolute_ZP,              // 200
    nsp::No_Address,               // 201
    nsp::Const,                    // 202
    nsp::No_Address,               // 203
    nsp::Unused,                   // 204
    nsp::Absolute,                 // 205
    nsp::Absolute,                 // 206
    nsp::Absolute,                 // 207
    nsp::Absolute,                 // 208
    nsp::Relative,                 // 209
    nsp::PostIndex_Indirect_Y,     // 210
    nsp::Unused,                   // 211
    nsp::PostIndex_Indirect_Y,     // 212
    nsp::Index_ZP_X,               // 213
    nsp::Index_ZP_X,               // 214
    nsp::Index_ZP_X,               // 215
    nsp::Index_ZP_X,               // 216
    nsp::No_Address,               // 217
    nsp::Index_Y,                  // 218
    nsp::No_Address,               // 219
    nsp::Index_Y,                  // 220
    nsp::Index_X,                  // 221
    nsp::Index_X,                  // 222
    nsp::Index_X,                  // 223
    nsp::Index_X,                  // 224
    nsp::Const,                    // 225
    nsp::PreIndex_Indirect_X,      // 226
    nsp::Unused,                   // 227
    nsp::PreIndex_Indirect_X,      // 228
    nsp::Absolute_ZP,              // 229
    nsp::Absolute_ZP,              // 230
    nsp::Absolute_ZP,              // 231
    nsp::Absolute_ZP,              // 232
    nsp::No_Address,               // 233
    nsp::Const,                    // 234
    nsp::No_Address,               // 235
    nsp::Const,                    // 236
    nsp::Absolute,                 // 237
    nsp::Absolute,                 // 238
    nsp::Absolute,                 // 239
    nsp::Absolute,                 // 240
    nsp::Relative,                 // 241
    nsp::PostIndex_Indirect_Y,     // 242
    nsp::Unused,                   // 243
    nsp::PostIndex_Indirect_Y,     // 244
    nsp::Index_ZP_X,               // 245
    nsp::Index_ZP_X,               // 246
    nsp::Index_ZP_X,               // 247
    nsp::Index_ZP_X,               // 248
    nsp::No_Address,               // 249
    nsp::Index_Y,                  // 250
    nsp::No_Address,               // 251
    nsp::Index_Y,                  // 252
    nsp::Index_X,                  // 253
    nsp::Index_X,                  // 254
    nsp::Index_X,                  // 255
    nsp::Index_X,                  // 256
};

uint8_t nsp::NES_INSTR_CYCLES_LUT[256] = {
    7, // 0x00, implied       BRK
    6, // 0x01, (indirect,X)  ORA (oper,X)
    2, // 0x02, unknown
    8, // 0x03, unofficial, SLO (d,X)
    3, // 0x04, unofficial, IGN d
    3, // 0x05, zeropage      ORA oper
    5, // 0x06, zeropage      ASL oper
    5, // 0x07, unofficial, SLO d
    3, // 0x08, implied       PHP
    2, // 0x09, immidiate     ORA #oper
    2, // 0x0A, accumulator   ASL A
    2, // 0x0B, unofficial, ANC #i
    4, // 0x0C, unofficial, IGN a
    4, // 0x0D, absolute      ORA oper
    6, // 0x0E, absolute      ASL oper
    6, // 0x0F, unofficial, SLO a
    2, // 0x10, relative      BPL oper      (**)
    5, // 0x11, (indirect),Y  ORA (oper),Y  (*)
    2, // 0x12, unknown
    8, // 0x13, unofficial, SLO (d),Y
    4, // 0x14, unofficial, IGN d,X
    4, // 0x15, zeropage,X    ORA oper,X
    6, // 0x16, zeropage,X    ASL oper,X
    6, // 0x17, unofficial, SLO d,X
    2, // 0x18, implied       CLC
    4, // 0x19, absolute,Y    ORA oper,Y    (*)
    2, // 0x1A, unofficial, NOP
    7, // 0x1B, unofficial, SLO a,Y
    4, // 0x1C, unofficial, IGN a,X (*)
    4, // 0x1D, absolute,X    ORA oper,X    (*)
    7, // 0x1E, absolute,X    ASL oper,X
    7, // 0x1F, unofficial, SLO a,X
    6, // 0x20, absolute      JSR oper
    6, // 0x21, (indirect,X)  AND (oper,X)
    2, // 0x22, unknown
    8, // 0x23, unofficial, RLA (d,X)
    3, // 0x24, zeropage      BIT oper
    3, // 0x25, zeropage      AND oper
    5, // 0x26, zeropage      ROL oper
    5, // 0x27, unofficial, RLA d
    4, // 0x28, implied       PLP
    2, // 0x29, immidiate     AND #oper
    2, // 0x2A, accumulator   ROL A
    2, // 0x2B, unofficial, ANC #i
    4, // 0x2C, absolute      BIT oper
    4, // 0x2D, absolute      AND oper
    6, // 0x2E, absolute      ROL oper
    6, // 0x2F, unofficial, RLA a
    2, // 0x30, relative      BMI oper      (**)
    5, // 0x31, (indirect),Y  AND (oper),Y  (*)
    2, // 0x32, unknown
    8, // 0x33, unofficial, RLA (d),Y
    4, // 0x34, unofficial, IGN d,X
    4, // 0x35, zeropage,X    AND oper,X
    6, // 0x36, zeropage,X    ROL oper,X
    6, // 0x37, unofficial, RLA d,X
    2, // 0x38, implied       SEC
    4, // 0x39, absolute,Y    AND oper,Y    (*)
    2, // 0x3A, unofficial, NOP
    7, // 0x3B, unofficial, RLA a,Y
    4, // 0x3C, unofficial, IGN a,X (*)
    4, // 0x3D, absolute,X    AND oper,X    (*)
    7, // 0x3E, absolute,X    ROL oper,X
    7, // 0x3F, unofficial, RLA a,X
    6, // 0x40, implied       RTI
    6, // 0x41, (indirect,X)  EOR (oper,X)
    2, // 0x42, unknown
    8, // 0x43, unofficial, SRE (d,X)
    3, // 0x44, unofficial, IGN d
    3, // 0x45, zeropage      EOR oper
    5, // 0x46, zeropage      LSR oper
    5, // 0x47, unofficial, SRE d
    3, // 0x48, implied       PHA
    2, // 0x49, immidiate     EOR #oper
    2, // 0x4A, accumulator   LSR A
    2, // 0x4B, unofficial, ALR #i
    3, // 0x4C, absolute      JMP oper
    4, // 0x4D, absolute      EOR oper
    6, // 0x4E, absolute      LSR oper
    6, // 0x4F, unofficial, SRE a
    2, // 0x50, relative      BVC oper      (**)
    5, // 0x51, (indirect),Y  EOR (oper),Y  (*)
    2, // 0x52, unknown
    8, // 0x53, unofficial, SRE (d),Y
    4, // 0x54, unofficial, IGN d,X
    4, // 0x55, zeropage,X    EOR oper,X
    6, // 0x56, zeropage,X    LSR oper,X
    6, // 0x57, unofficial, SRE d,X
    2, // 0x58, implied       CLI
    4, // 0x59, absolute,Y    EOR oper,Y    (*)
    2, // 0x5A, unofficial, NOP
    7, // 0x5B, unofficial, SRE a,Y
    4, // 0x5C, unofficial, IGN a,X (*)
    4, // 0x5D, absolute,X    EOR oper,X    (*)
    7, // 0x5E, absolute,X    LSR oper,X
    7, // 0x5F, unofficial, SRE a,X
    6, // 0x60, implied       RTS
    6, // 0x61, (indirect,X)  ADC (oper,X)
    2, // 0x62, unknown
    8, // 0x63, unofficial, RRA (d,X)
    3, // 0x64, unofficial, IGN d
    3, // 0x65, zeropage      ADC oper
    5, // 0x66, zeropage      ROR oper
    5, // 0x67, unofficial, RRA d
    4, // 0x68, implied       PLA
    2, // 0x69, immidiate     ADC #oper
    2, // 0x6A, accumulator   ROR A
    2, // 0x6B, unofficial, ARR #i
    5, // 0x6C, indirect      JMP (oper)
    4, // 0x6D, absolute      ADC oper
    6, // 0x6E, absolute      ROR oper
    6, // 0x6F, unofficial, RRA a
    2, // 0x70, relative      BVC oper      (**)
    5, // 0x71, (indirect),Y  ADC (oper),Y  (*)
    2, // 0x72, unknown
    8, // 0x73, unofficial, RRA (d),Y
    4, // 0x74, unofficial, IGN d,X
    4, // 0x75, zeropage,X    ADC oper,X
    6, // 0x76, zeropage,X    ROR oper,X
    6, // 0x77, unofficial, RRA d,X
    2, // 0x78, implied       SEI
    4, // 0x79, absolute,Y    ADC oper,Y    (*)
    2, // 0x7A, unofficial, NOP
    7, // 0x7B, unofficial, RRA a,Y
    4, // 0x7C, unofficial, IGN a,X (*)
    4, // 0x7D, absolute,X    ADC oper,X    (*)
    7, // 0x7E, absolute,X    ROR oper,X
    7, // 0x7F, unofficial, RRA a,X
    2, // 0x80, unofficial, SKB #i
    6, // 0x81, (indirect,X)  STA (oper,X)
    2, // 0x82, unofficial, SKB #i
    6, // 0x83, unofficial, SAX (d,X)
    3, // 0x84, zeropage      STY oper
    3, // 0x85, zeropage      STA oper
    3, // 0x86, zeropage      STX oper
    3, // 0x87, unofficial, SAX d
    2, // 0x88, implied       DEC
    2, // 0x89, unofficial, SKB #i
    2, // 0x8A, implied       TXA
    2, // 0x8B, unknown
    4, // 0x8C, absolute      STY oper
    4, // 0x8D, absolute      STA oper
    4, // 0x8E, absolute      STX oper
    4, // 0x8F, unofficial, SAX a
    2, // 0x90, relative      BCC oper      (**)
    6, // 0x91, (indirect),Y  STA (oper),Y
    2, // 0x92, unknown
    2, // 0x93, unknown
    4, // 0x94, zeropage,X    STY oper,X
    4, // 0x95, zeropage,X    STA oper,X
    4, // 0x96, zeropage,Y    STX oper,Y
    4, // 0x97, unofficial, SAX d,Y
    2, // 0x98, implied       TYA
    5, // 0x99, absolute,Y    STA oper,Y
    2, // 0x9A, implied       TXS
    2, // 0x9B, unknown
    2, // 0x9C, unknown
    5, // 0x9D, absolute,X    STA oper,X
    2, // 0x9E, unknown
    2, // 0x9F, unknown
    2, // 0xA0, immidiate     LDY #oper
    6, // 0xA1, (indirect,X)  LDA (oper,X)
    2, // 0xA2, immidiate     LDX #oper
    6, // 0xA3, unofficial, LAX (d,X)
    3, // 0xA4, zeropage      LDY oper
    3, // 0xA5, zeropage      LDA oper
    3, // 0xA6, zeropage      LDX oper
    3, // 0xA7, unofficial, LAX d
    2, // 0xA8, implied       TAY
    2, // 0xA9, immidiate     LDA #oper
    2, // 0xAA, implied       TAX
    2, // 0xAB, unknown
    4, // 0xAC, absolute      LDY oper
    4, // 0xAD, absolute      LDA oper
    4, // 0xAE, absolute      LDX oper
    4, // 0xAF, unofficial, LAX a
    2, // 0xB0, relative      BCS oper      (**)
    5, // 0xB1, (indirect),Y  LDA (oper),Y  (*)
    2, // 0xB2, unknown
    5, // 0xB3, unofficial, LAX (d),Y
    4, // 0xB4, zeropage,X    LDY oper,X
    4, // 0xB5, zeropage,X    LDA oper,X
    4, // 0xB6, zeropage,Y    LDX oper,Y
    4, // 0xB7, unofficial, LAX d,Y
    2, // 0xB8, unofficial, CLV
    4, // 0xB9, absolute,Y    LDA oper,Y    (*)
    2, // 0xBA, implied       TSX
    2, // 0xBB, unknown
    4, // 0xBC, absolute,X    LDY oper,X    (*)
    4, // 0xBD, absolute,X    LDA oper,X    (*)
    4, // 0xBE, absolute,Y    LDX oper,Y    (*)
    4, // 0xBF, unofficial, LAX a,Y
    2, // 0xC0, immidiate     CPY #oper
    6, // 0xC1, (indirect,X)  CMP (oper,X)
    2, // 0xC2, unofficial, SKB #i
    8, // 0xC3, unofficial, DCP (d,X)
    3, // 0xC4, zeropage      CPY oper
    3, // 0xC5, zeropage      CMP oper
    5, // 0xC6, zeropage      DEC oper
    5, // 0xC7, unofficial, DCP d
    2, // 0xC8, implied       INY
    2, // 0xC9, immidiate     CMP #oper
    2, // 0xCA, implied       DEC
    2, // 0xCB, unofficial, AXS #i
    4, // 0xCC, absolute      CPY oper
    4, // 0xCD, absolute      CMP oper
    6, // 0xCE, absolute      DEC oper
    6, // 0xCF, unofficial, DCP a
    2, // 0xD0, relative      BNE oper      (**)
    5, // 0xD1, (indirect),Y  CMP (oper),Y  (*)
    2, // 0xD2, unknown
    8, // 0xD3, unofficial, DCP (d),Y
    4, // 0xD4, unofficial, IGN d,X
    4, // 0xD5, zeropage,X    CMP oper,X
    6, // 0xD6, zeropage,X    DEC oper,X
    6, // 0xD7, unofficial, DCP d,X
    2, // 0xD8, unofficial, CLD
    4, // 0xD9, absolute,Y    CMP oper,Y    (*)
    2, // 0xDA, unofficial, NOP
    7, // 0xDB, unofficial, DCP a,Y
    4, // 0xDC, unofficial, IGN a,X (*)
    4, // 0xDD, absolute,X    CMP oper,X    (*)
    7, // 0xDE, absolute,X    DEC oper,X
    7, // 0xDF, unofficial, DCP a,X
    2, // 0xE0, immidiate     CPX #oper
    6, // 0xE1, (indirect,X)  SBC (oper,X)
    2, // 0xE2, unofficial, SKB #i
    8, // 0xE3, unofficial, ISC (d,X)
    3, // 0xE4, zeropage      CPX oper
    3, // 0xE5, zeropage      SBC oper
    5, // 0xE6, zeropage      INC oper
    5, // 0xE7, unofficial, ISC d
    2, // 0xE8, implied       INX
    2, // 0xE9, immidiate     SBC #oper
    2, // 0xEA, implied       NOP
    2, // 0xEB, unknown
    4, // 0xEC, absolute      CPX oper
    4, // 0xED, absolute      SBC oper
    6, // 0xEE, absolute      INC oper
    6, // 0xEF, unofficial, ISC a
    2, // 0xF0, relative      BEQ oper      (**)
    5, // 0xF1, (indirect),Y  SBC (oper),Y  (*)
    2, // 0xF2, unknown
    8, // 0xF3, unofficial, ISC (d),Y
    4, // 0xF4, unofficial, IGN d,X
    4, // 0xF5, zeropage,X    SBC oper,X
    6, // 0xF6, zeropage,X    INC oper,X
    6, // 0xF7, unofficial, ISC d,X
    2, // 0xF8, unofficial, SED
    4, // 0xF9, absolute,Y    SBC oper,Y    (*)
    2, // 0xFA, unofficial, NOP
    7, // 0xFB, unofficial, ISC a,Y
    4, // 0xFC, unofficial, IGN a,X (*)
    4, // 0xFD, absolute,X    SBC oper,X    (*)
    7, // 0xFE, absolute,X    INC oper,X
    7, // 0xFF, unofficial, ISC a,X
};

uint8_t nsp::NES_INSTR_CYCLES_EXTRA_LUT[256] = {
    0, // 0x00, implied       BRK
    0, // 0x01, (indirect,X)  ORA (oper,X)
    0, // 0x02, unknown
    0, // 0x03, unknown
    0, // 0x04, unknown
    0, // 0x05, zeropage      ORA oper
    0, // 0x06, zeropage      ASL oper
    0, // 0x07, unknown
    0, // 0x08, implied       PHP
    0, // 0x09, immidiate     ORA #oper
    0, // 0x0A, accumulator   ASL A
    0, // 0x0B, unknown
    0, // 0x0C, unknown
    0, // 0x0D, absolute      ORA oper
    0, // 0x0E, absolute      ASL oper
    0, // 0x0F, unknown
    2, // 0x10, relative      BPL oper      (**)
    1, // 0x11, (indirect),Y  ORA (oper),Y  (*)
    0, // 0x12, unknown
    0, // 0x13, unknown
    0, // 0x14, unknown
    0, // 0x15, zeropage,X    ORA oper,X
    0, // 0x16, zeropage,X    ASL oper,X
    0, // 0x17, unknown
    0, // 0x18, implied       CLC
    1, // 0x19, absolute,Y    ORA oper,Y    (*)
    0, // 0x1A, unknown
    0, // 0x1B, unknown
    1, // 0x1C, unknown
    1, // 0x1D, absolute,X    ORA oper,X    (*)
    0, // 0x1E, absolute,X    ASL oper,X
    0, // 0x1F, unknown
    0, // 0x20, absolute      JSR oper
    0, // 0x21, (indirect,X)  AND (oper,X)
    0, // 0x22, unknown
    0, // 0x23, unknown
    0, // 0x24, zeropage      BIT oper
    0, // 0x25, zeropage      AND oper
    0, // 0x26, zeropage      ROL oper
    0, // 0x27, unknown
    0, // 0x28, implied       PLP
    0, // 0x29, immidiate     AND #oper
    0, // 0x2A, accumulator   ROL A
    0, // 0x2B, unknown
    0, // 0x2C, absolute      BIT oper
    0, // 0x2D, absolute      AND oper
    0, // 0x2E, absolute      ROL oper
    0, // 0x2F, unknown
    2, // 0x30, relative      BMI oper      (**)
    1, // 0x31, (indirect),Y  AND (oper),Y  (*)
    0, // 0x32, unknown
    0, // 0x33, unknown
    0, // 0x34, unknown
    0, // 0x35, zeropage,X    AND oper,X
    0, // 0x36, zeropage,X    ROL oper,X
    0, // 0x37, unknown
    0, // 0x38, implied       SEC
    1, // 0x39, absolute,Y    AND oper,Y    (*)
    0, // 0x3A, unknown
    0, // 0x3B, unknown
    1, // 0x3C, unknown
    1, // 0x3D, absolute,X    AND oper,X    (*)
    0, // 0x3E, absolute,X    ROL oper,X
    0, // 0x3F, unknown
    0, // 0x40, implied       RTI
    0, // 0x41, (indirect,X)  EOR (oper,X)
    0, // 0x42, unknown
    0, // 0x43, unknown
    0, // 0x44, unknown
    0, // 0x45, zeropage      EOR oper
    0, // 0x46, zeropage      LSR oper
    0, // 0x47, unknown
    0, // 0x48, implied       PHA
    0, // 0x49, immidiate     EOR #oper
    0, // 0x4A, accumulator   LSR A
    0, // 0x4B, unknown
    0, // 0x4C, absolute      JMP oper
    0, // 0x4D, absolute      EOR oper
    0, // 0x4E, absolute      LSR oper
    0, // 0x4F, unknown
    2, // 0x50, relative      BVC oper      (**)
    1, // 0x51, (indirect),Y  EOR (oper),Y  (*)
    0, // 0x52, unknown
    0, // 0x53, unknown
    0, // 0x54, unknown
    0, // 0x55, zeropage,X    EOR oper,X
    0, // 0x56, zeropage,X    LSR oper,X
    0, // 0x57, unknown
    0, // 0x58, implied       CLI
    1, // 0x59, absolute,Y    EOR oper,Y    (*)
    0, // 0x5A, unknown
    0, // 0x5B, unknown
    1, // 0x5C, unknown
    1, // 0x5D, absolute,X    EOR oper,X    (*)
    0, // 0x5E, absolute,X    LSR oper,X
    0, // 0x5F, unknown
    0, // 0x60, implied       RTS
    0, // 0x61, (indirect,X)  ADC (oper,X)
    0, // 0x62, unknown
    0, // 0x63, unknown
    0, // 0x64, unknown
    0, // 0x65, zeropage      ADC oper
    0, // 0x66, zeropage      ROR oper
    0, // 0x67, unknown
    0, // 0x68, implied       PLA
    0, // 0x69, immidiate     ADC #oper
    0, // 0x6A, accumulator   ROR A
    0, // 0x6B, unknown
    0, // 0x6C, indirect      JMP (oper)
    0, // 0x6D, absolute      ADC oper
    0, // 0x6E, absolute      ROR oper
    0, // 0x6F, unknown
    2, // 0x70, relative      BVC oper      (**)
    1, // 0x71, (indirect),Y  ADC (oper),Y  (*)
    0, // 0x72, unknown
    0, // 0x73, unknown
    0, // 0x74, unknown
    0, // 0x75, zeropage,X    ADC oper,X
    0, // 0x76, zeropage,X    ROR oper,X
    0, // 0x77, unknown
    0, // 0x78, implied       SEI
    1, // 0x79, absolute,Y    ADC oper,Y    (*)
    0, // 0x7A, unknown
    0, // 0x7B, unknown
    1, // 0x7C, unknown
    1, // 0x7D, absolute,X    ADC oper,X    (*)
    0, // 0x7E, absolute,X    ROR oper,X
    0, // 0x7F, unknown
    0, // 0x80, unknown
    0, // 0x81, (indirect,X)  STA (oper,X)
    0, // 0x82, unknown
    0, // 0x83, unknown
    0, // 0x84, zeropage      STY oper
    0, // 0x85, zeropage      STA oper
    0, // 0x86, zeropage      STX oper
    0, // 0x87, unknown
    0, // 0x88, implied       DEC
    0, // 0x89, unknown
    0, // 0x8A, implied       TXA
    0, // 0x8B, unknown
    0, // 0x8C, absolute      STY oper
    0, // 0x8D, absolute      STA oper
    0, // 0x8E, absolute      STX oper
    0, // 0x8F, unknown
    2, // 0x90, relative      BCC oper      (**)
    0, // 0x91, (indirect),Y  STA (oper),Y
    0, // 0x92, unknown
    0, // 0x93, unknown
    0, // 0x94, zeropage,X    STY oper,X
    0, // 0x95, zeropage,X    STA oper,X
    0, // 0x96, zeropage,Y    STX oper,Y
    0, // 0x97, unknown
    0, // 0x98, implied       TYA
    0, // 0x99, absolute,Y    STA oper,Y
    0, // 0x9A, implied       TXS
    0, // 0x9B, unknown
    0, // 0x9C, unknown
    0, // 0x9D, absolute,X    STA oper,X
    0, // 0x9E, unknown
    0, // 0x9F, unknown
    0, // 0xA0, immidiate     LDY #oper
    0, // 0xA1, (indirect,X)  LDA (oper,X)
    0, // 0xA2, immidiate     LDX #oper
    0, // 0xA3, unknown
    0, // 0xA4, zeropage      LDY oper
    0, // 0xA5, zeropage      LDA oper
    0, // 0xA6, zeropage      LDX oper
    0, // 0xA7, unknown
    0, // 0xA8, implied       TAY
    0, // 0xA9, immidiate     LDA #oper
    0, // 0xAA, implied       TAX
    0, // 0xAB, unknown
    0, // 0xAC, absolute      LDY oper
    0, // 0xAD, absolute      LDA oper
    0, // 0xAE, absolute      LDX oper
    0, // 0xAF, unknown
    2, // 0xB0, relative      BCS oper      (**)
    1, // 0xB1, (indirect),Y  LDA (oper),Y  (*)
    0, // 0xB2, unknown
    1, // 0xB3, unknown
    0, // 0xB4, zeropage,X    LDY oper,X
    0, // 0xB5, zeropage,X    LDA oper,X
    0, // 0xB6, zeropage,Y    LDX oper,Y
    0, // 0xB7, unknown
    0, // 0xB8, implied       CLV
    1, // 0xB9, absolute,Y    LDA oper,Y    (*)
    0, // 0xBA, implied       TSX
    0, // 0xBB, unknown
    1, // 0xBC, absolute,X    LDY oper,X    (*)
    1, // 0xBD, absolute,X    LDA oper,X    (*)
    1, // 0xBE, absolute,Y    LDX oper,Y    (*)
    0, // 0xBF, unknown
    0, // 0xC0, immidiate     CPY #oper
    0, // 0xC1, (indirect,X)  CMP (oper,X)
    0, // 0xC2, unknown
    0, // 0xC3, unknown
    0, // 0xC4, zeropage      CPY oper
    0, // 0xC5, zeropage      CMP oper
    0, // 0xC6, zeropage      DEC oper
    0, // 0xC7, unknown
    0, // 0xC8, implied       INY
    0, // 0xC9, immidiate     CMP #oper
    0, // 0xCA, implied       DEC
    0, // 0xCB, unknown
    0, // 0xCC, absolute      CPY oper
    0, // 0xCD, absolute      CMP oper
    0, // 0xCE, absolute      DEC oper
    0, // 0xCF, unknown
    2, // 0xD0, relative      BNE oper      (**)
    1, // 0xD1, (indirect),Y  CMP (oper),Y  (*)
    0, // 0xD2, unknown
    0, // 0xD3, unknown
    0, // 0xD4, unknown
    0, // 0xD5, zeropage,X    CMP oper,X
    0, // 0xD6, zeropage,X    DEC oper,X
    0, // 0xD7, unknown
    0, // 0xD8, implied       CLD
    1, // 0xD9, absolute,Y    CMP oper,Y    (*)
    0, // 0xDA, unknown
    0, // 0xDB, unknown
    1, // 0xDC, unknown
    1, // 0xDD, absolute,X    CMP oper,X    (*)
    0, // 0xDE, absolute,X    DEC oper,X
    0, // 0xDF, unknown
    0, // 0xE0, immidiate     CPX #oper
    0, // 0xE1, (indirect,X)  SBC (oper,X)
    0, // 0xE2, unknown
    0, // 0xE3, unknown
    0, // 0xE4, zeropage      CPX oper
    0, // 0xE5, zeropage      SBC oper
    0, // 0xE6, zeropage      INC oper
    0, // 0xE7, unknown
    0, // 0xE8, implied       INX
    0, // 0xE9, immidiate     SBC #oper
    0, // 0xEA, implied       NOP
    0, // 0xEB, unknown
    0, // 0xEC, absolute      CPX oper
    0, // 0xED, absolute      SBC oper
    0, // 0xEE, absolute      INC oper
    0, // 0xEF, unknown
    2, // 0xF0, relative      BEQ oper      (**)
    1, // 0xF1, (indirect),Y  SBC (oper),Y  (*)
    0, // 0xF2, unknown
    0, // 0xF3, unknown
    0, // 0xF4, unknown
    0, // 0xF5, zeropage,X    SBC oper,X
    0, // 0xF6, zeropage,X    INC oper,X
    0, // 0xF7, unknown
    0, // 0xF8, implied       SED
    1, // 0xF9, absolute,Y    SBC oper,Y    (*)
    0, // 0xFA, unknown
    0, // 0xFB, unknown
    1, // 0xFC, unknown
    1, // 0xFD, absolute,X    SBC oper,X    (*)
    0, // 0xFE, absolute,X    INC oper,X
    0, // 0xFF, unknown
};

#define SET_NEGATIVE(n)  cpu.regs.N = n;
#define SET_OVERFLOW(n)  cpu.regs.O = n;
#define SET_BREAK(n)     cpu.regs.B = n;
#define SET_DECIMAL(n)   cpu.regs.D = n;
#define SET_INTERRUPT(n) cpu.regs.I = n;
#define SET_ZERO(n)      cpu.regs.Z = n;
#define SET_CARRY(n)     cpu.regs.C = n;


#define GET_NEGATIVE     (cpu.regs.N)
#define GET_OVERFLOW     (cpu.regs.O)
#define GET_BREAK        (cpu.regs.B)
#define GET_DECIMAL      (cpu.regs.D)
#define GET_INTERRUPT    (cpu.regs.I)
#define GET_ZERO         (cpu.regs.Z)
#define GET_CARRY        (cpu.regs.C)


#define CALC_CARRY(n)          SET_CARRY( (n > 0xFF) )
#define CALC_ZERO(n)           SET_ZERO( (n & 0xFF) == 0x0 )
#define CALC_OVERFLOW(a, b, c) SET_OVERFLOW( (!((a ^ b) & 0x80) && ((a ^ c) & 0x80)) )
#define CALC_NEGATIVE(n)       SET_NEGATIVE( n & (1 << 7) ? 1 : 0 )


#define NES_OP_FUNC_IMPL(name) \
    static void op_##name(nsp::emu_t& emu, nsp::cpu_t& cpu, nsp::NES_OP_ADDR_MODES addr_mode, \
                             uint8_t instr, uint16_t& addr, uint16_t* addr_ptr)


#define MEM_READ_ADDR() memory_read_from_addr_ptr(emu, addr_ptr)

static unsigned short data0 = 0;
static unsigned short data1 = 0;

NES_OP_FUNC_IMPL(INVALID)
{
    LOG_E("Unknown OPCODE: %02X at PC: 0x%02X", instr, cpu.regs.PC);
    exit(1);
}

/////////////////////////////////////////////////////////
// ADC group
// A + M + C -> A, C
//
// N Z C I D V
// | | | _ _ |
//
NES_OP_FUNC_IMPL(ADC)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data1 = data0;
    data0 = cpu.regs.A + data0 + GET_CARRY;

    CALC_CARRY(data0);
    CALC_ZERO(data0);
    CALC_NEGATIVE(data0);
    CALC_OVERFLOW(cpu.regs.A, data1, data0);

    cpu.regs.A = data0;
}

/////////////////////////////////////////////////////////
// AND group
// A /\ M -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(AND)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data0 = cpu.regs.A & data0;
    cpu.regs.A = data0;

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// ASL group - Shift Left One Bit (Memory or Accumulator)
//
// Operation: C <- |7|6|5|4|3|2|1|0| <- 0
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(ASL)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode == nsp::Accumulator)
    {
        if (nsp::cb_debug_reg_write) nsp::cb_debug_reg_write('A');
        data0 = data0 << 1;
        cpu.regs.A = data0;

    } else {
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
        data0 = data0 << 1;
        memory_write(emu, addr, data0);
    }

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    CALC_CARRY(data0);
}

/////////////////////////////////////////////////////////
// BCC - Branch on Carry Clear
//
// Operation: Branch on C = 0
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BCC)
{
    if (!GET_CARRY) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BCS - Branch on carry set
//
// Operation: Branch on C = 1
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BCS)
{
    if (GET_CARRY) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BEQ - Branch on result zero
//
// Operation: Branch on Z = 1
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BEQ)
{
    if (GET_ZERO) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BIT - Test bits in memory with accumulator
//
// Operation: A /\ M, M7 -> N, M6 -> V
//
// N Z C I D V
// M7| _ _ _ M6
//
// Bit 6 and 7 are transferred to the status register.
// If the result of A /\ M is zero then Z = 1, otherwise Z = 0
//
NES_OP_FUNC_IMPL(BIT)
{
    data0 = MEM_READ_ADDR();

    SET_NEGATIVE( ((data0 >> 7) & 0x1) );
    SET_OVERFLOW( ((data0 >> 6) & 0x1) );

    data0 = cpu.regs.A & data0;
    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// BMI - Branch on result minus
//
// Operation: Branch on N = 1
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BMI)
{
    if (GET_NEGATIVE) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BNE - Branch on result not zero
//
// Operation: Branch on Z = 0
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BNE)
{
    if (!GET_ZERO) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BPL - Branch on result plus
//
// Operation: Branch on N = 0
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BPL)
{
    if (!GET_NEGATIVE) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BRK - Force Break
//
// Operation: Forced Interrupt PC + 2 toS P toS
//
// N Z C I D V
// _ _ _ 1 _ _
//
NES_OP_FUNC_IMPL(BRK)
{
    /*
    // data1 FIX
    stack_push_short(emu, cpu.regs.PC+1);

    stack_push(emu, cpu.regs.P | 0x10);

    SET_INTERRUPT(1);
    */

    // cpu_t& cpu = emu.cpu;
    stack_push_short(emu, cpu.regs.PC);
    stack_push(emu, emu.cpu.regs.P);

    SET_INTERRUPT(1);
    cpu.regs.PC = cpu.vectors.NMI;
}

/////////////////////////////////////////////////////////
// BVC - Branch on overflow clear
//
// Operation: Branch on V = 0
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BVC)
{
    if (!GET_OVERFLOW) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// BVS - Branch on overflow set
//
// Operation: Branch on V = 1
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(BVS)
{
    if (GET_OVERFLOW) {
        cpu.regs.PC = addr;
        cpu.extra_cycles += 1;
    }
}

/////////////////////////////////////////////////////////
// CLC - Clear carry flag
//
// Operation: 0 -> C
//
// N Z C I D V
// _ _ 0 _ _ _
//
NES_OP_FUNC_IMPL(CLC)
{
    SET_CARRY(0);
}

/////////////////////////////////////////////////////////
// CLD - Clear decimal mode
//
// Operation: 0 -> D
//
// N Z C I D V
// _ _ _ _ 0 _
//
NES_OP_FUNC_IMPL(CLD)
{
    SET_DECIMAL(0);
}

/////////////////////////////////////////////////////////
// CLI - Clear interrupt disable bit
//
// Operation: 0 -> I
//
// N Z C I D V
// _ _ _ 0 _ _
//
NES_OP_FUNC_IMPL(CLI)
{
    SET_INTERRUPT(0);
}

/////////////////////////////////////////////////////////
// CLV - Clear overflow flag
//
// Operation: 0 -> V
//
// N Z C I D V
// _ _ _ _ _ 0
//
NES_OP_FUNC_IMPL(CLV)
{
    SET_OVERFLOW(0);
}

/////////////////////////////////////////////////////////
// CMP - Compare memory and accumulator
//
// Operation: A - M
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(CMP)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data1 = data0;
    data0 = cpu.regs.A - (data0 & 0xFF);

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    SET_CARRY( (cpu.regs.A >= data1) ? 1 : 0 );
}

/////////////////////////////////////////////////////////
// CPX - Compare Memory and Index X
//
// Operation: X - M
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(CPX)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data1 = data0;
    data0 = cpu.regs.X - data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    SET_CARRY( (cpu.regs.X >= data1) ? 1 : 0 );
}

/////////////////////////////////////////////////////////
// CPY - Compare memory and index Y
//
// Operation: Y - M
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(CPY)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data1 = data0;
    data0 = cpu.regs.Y - data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    SET_CARRY( (cpu.regs.Y >= data1) ? 1 : 0 );
}

/////////////////////////////////////////////////////////
// DEC - Decrement memory by one
//
// Operation: M - 1 -> M
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(DEC)
{
    data0 = MEM_READ_ADDR();

    data0 = data0 - 1;
    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), data0);
    memory_write(emu, addr, data0);
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// DEX - Decrement index X by one
//
// Operation: X - 1 -> X
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(DEX)
{
    data0 = cpu.regs.X - 1;
    cpu.regs.X = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// DEY - Decrement index Y by one
//
// Operation: Y - 1 -> Y
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(DEY)
{
    data0 = cpu.regs.Y - 1;
    cpu.regs.Y = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// EOR - "Exclusive-Or" memory with accumulator
//
// Operation: A EOR M -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(EOR)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data0 = cpu.regs.A ^ data0;
    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// INC - Increment memory by one
//
// Operation: M + 1 -> M
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(INC)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
    data0 = data0 + 1;
    memory_write(emu, addr, data0);
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// INX - Increment Index X by one
//
// Operation: X + 1 -> X
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(INX)
{
    data0 = cpu.regs.X + 1;
    cpu.regs.X = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// INY - Increment Index Y by one
//
// Operation: Y + 1 -> Y
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(INY)
{
    data0 = cpu.regs.Y + 1;
    cpu.regs.Y = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// JMP - Jump to new location
//
// Operation: (PC + 1) -> PCL
//            (PC + 2) -> PCH
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(JMP)
{
    cpu.regs.PC = addr;
}

/////////////////////////////////////////////////////////
// JSR - Jump to new location saving return address
//
// Operation: PC + 2 toS, (PC + 1) -> PCL
//                        (PC + 2) -> PCH
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(JSR)
{
    stack_push_short(emu, cpu.regs.PC-1);
    cpu.regs.PC = addr;
}

/////////////////////////////////////////////////////////
// LDA - Load accumulator with memory
//
// Operation: M -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(LDA)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
        {
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr, true), 0x0);
            // nsp::cb_debug_mem_write(addr, memory_read(emu, (addr & 0xFFF) - 0x2000 + 0x8000), 0x0);
        }

    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}


/////////////////////////////////////////////////////////
// LDX - Load index X with memory
//
// Operation: M -> X
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(LDX)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    cpu.regs.X = data0;
    // FIXME(andsve)
    // CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// LDY - Load index Y with memory
//
// Operation: M -> Y
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(LDY)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    cpu.extra_cycles += 1;

    cpu.regs.Y = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// LSR  - Shift right one bit (memory or accumulator)
//
// Operation: 0 -> |7|6|5|4|3|2|1|0| -> C
//
// N Z C I D V
// 0 | | _ _ _
//
NES_OP_FUNC_IMPL(LSR)
{
    data0 = MEM_READ_ADDR();

    SET_CARRY(data0 & 0x1);

    if (addr_mode == nsp::Accumulator)
    {
        if (nsp::cb_debug_reg_write) nsp::cb_debug_reg_write('A');
        data0 = data0 >> 1;

        cpu.regs.A = data0;
    } else {
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
        data0 = data0 >> 1;

        memory_write(emu, addr, data0);
    }
    SET_NEGATIVE(0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// NOP - No operation
//
// Operation: No Operation (2 cycles)
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(NOP)
{
    return;
}

/////////////////////////////////////////////////////////
// ORA - "OR" memory with accumulator
//
// Operation: A V M -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(ORA)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write)
            nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data0 = cpu.regs.A | data0;
    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// PHA - Push accumulator on stack
//
// Operation: A toS
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(PHA)
{
    stack_push(emu, cpu.regs.A);
}

/////////////////////////////////////////////////////////
// PHP - Push processor status on stack
//
// Operation: P toS
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(PHP)
{
    stack_push(emu, cpu.regs.P | 0x10);
}

/////////////////////////////////////////////////////////
// PLA - Pull accumulator from stack
//
// Operation: A fromS
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(PLA)
{
    cpu.regs.A = stack_pop(emu);
    CALC_NEGATIVE(cpu.regs.A);
    CALC_ZERO(cpu.regs.A);
}

/////////////////////////////////////////////////////////
// PLP - Pull accumulator from stack
//
// Operation: P fromS
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(PLP)
{
    cpu.regs.P = (stack_pop(emu) | 0x20) & 0xEF;
}

/////////////////////////////////////////////////////////
// ROL - Rotate one bit left (memory or accumulator)
//
//            +------------------------------+
//            |         M or A               |
//            |   +-+-+-+-+-+-+-+-+    +-+   |
// Operation: +-< |7|6|5|4|3|2|1|0| <- |C| <-+
//                +-+-+-+-+-+-+-+-+    +-+
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(ROL)
{
    data0 = MEM_READ_ADDR();

    data0 = data0 << 1;
    data0 |= GET_CARRY;

    if (addr_mode == nsp::Accumulator) {
        if (nsp::cb_debug_reg_write) nsp::cb_debug_reg_write('A');
        cpu.regs.A = data0;
    } else {
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
        memory_write(emu, addr, data0);
    }

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    CALC_CARRY(data0);
}

/////////////////////////////////////////////////////////
// ROR - Rotate one bit right (memory or accumulator)
//
//            +------------------------------+
//            |         M or A               |
//            |   +-+    +-+-+-+-+-+-+-+-+   |
// Operation: +-> |C| -> |7|6|5|4|3|2|1|0| >-+
//                +-+    +-+-+-+-+-+-+-+-+
//
// N Z C I D V
// | | | _ _ _
//
NES_OP_FUNC_IMPL(ROR)
{
    data0 = MEM_READ_ADDR();

    data1 = GET_CARRY;
    SET_CARRY(data0 & 0x1);
    data0 = data0 >> 1;
    data0 |= (data1 << 7);

    if (addr_mode == nsp::Accumulator) {
        if (nsp::cb_debug_reg_write) nsp::cb_debug_reg_write('A');
        cpu.regs.A = data0;
    } else {
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
        memory_write(emu, addr, data0);
    }

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

/////////////////////////////////////////////////////////
// RTI - Return from interrupt
//
// Operation: P fromS, PC fromS
//
// N Z C I D V
// From Stack
//
NES_OP_FUNC_IMPL(RTI)
{
    cpu.regs.P = stack_pop(emu) | 0x20;
    cpu.regs.PC = stack_pop_short(emu);
}

/////////////////////////////////////////////////////////
// RTS - Return from subroutine
//
// Operation: PC fromS, PC + 1 -> PC
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(RTS)
{
    cpu.regs.PC = stack_pop_short(emu);
    cpu.regs.PC += 0x1;
}

/////////////////////////////////////////////////////////
// SBC - Subtract memory from accumulator with borrow
//                    _
// Operation: A - M - C -> A
//       _
// Note: C = Borrow
//
// N Z C I D V
// | | | _ _ |
//
NES_OP_FUNC_IMPL(SBC)
{
    data0 = MEM_READ_ADDR();

    if (addr_mode != nsp::Const)
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data0 ^= 0xFF;
    data1 = data0;
    data0 = cpu.regs.A + data0 + GET_CARRY;

    CALC_CARRY(data0);
    CALC_ZERO(data0);
    CALC_NEGATIVE(data0);
    CALC_OVERFLOW(cpu.regs.A, data1, data0);

    cpu.regs.A = data0;
}

/////////////////////////////////////////////////////////
// SEC - Set carry flag
//
// Operation: 1 -> C
//
// N Z C I D V
// _ _ 1 _ _ _
//
NES_OP_FUNC_IMPL(SEC)
{
    SET_CARRY(1);
}

/////////////////////////////////////////////////////////
// SED - Set decimal mode
//
// Operation: 1 -> D
//
// N Z C I D V
// _ _ _ _ 1 _
//
NES_OP_FUNC_IMPL(SED)
{
    SET_DECIMAL(1);
}

/////////////////////////////////////////////////////////
// SEI - Set interrupt disable status
//
// Operation: 1 -> I
//
// N Z C I D V
// _ _ _ 1 _ _
//
NES_OP_FUNC_IMPL(SEI)
{
    SET_INTERRUPT(1);
}

/////////////////////////////////////////////////////////
// STA - Store accumulator in memory
//
// Operation: A -> M
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(STA)
{
    if (nsp::cb_debug_mem_write) {
        nsp::cb_debug_mem_write(addr, memory_read(emu, addr, true), cpu.regs.A);
    }
    data1 = memory_write(emu, addr, cpu.regs.A);
}

/////////////////////////////////////////////////////////
// STX - Store index X in memory
//
// Operation: X -> M
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(STX)
{
    if (nsp::cb_debug_mem_write) {
        nsp::cb_debug_mem_write(addr, memory_read(emu, addr, true), cpu.regs.X);
    }
    data1 = memory_write(emu, addr, cpu.regs.X);
}

/////////////////////////////////////////////////////////
// STY - Store index Y in memory
//
// Operation: Y -> M
//
// N Z C I D V
// _ _ _ _ _ _
//
NES_OP_FUNC_IMPL(STY)
{
    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr, true), 0x0);
    data1 = memory_write(emu, addr, cpu.regs.Y);
}

/////////////////////////////////////////////////////////
// TAX - Transfer accumulator to index X
//
// Operation: A -> X
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TAX)
{
    cpu.regs.X = cpu.regs.A;
    CALC_NEGATIVE(cpu.regs.X);
    CALC_ZERO(cpu.regs.X);
}

/////////////////////////////////////////////////////////
// TAY - Transfer accumulator to index Y
//
// Operation: A -> Y
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TAY)
{
    cpu.regs.Y = cpu.regs.A;
    CALC_NEGATIVE(cpu.regs.Y);
    CALC_ZERO(cpu.regs.Y);
}

/////////////////////////////////////////////////////////
// TSX - Transfer stack pointer to index X
//
// Operation: S -> X
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TSX)
{
    cpu.regs.X = cpu.regs.S;
    CALC_NEGATIVE(cpu.regs.X);
    CALC_ZERO(cpu.regs.X);
}

/////////////////////////////////////////////////////////
// TXA - Transfer index X to accumulator
//
// Operation: X -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TXA)
{
    cpu.regs.A = cpu.regs.X;
    CALC_NEGATIVE(cpu.regs.A);
    CALC_ZERO(cpu.regs.A);
}

/////////////////////////////////////////////////////////
// TXS - Transfer index X to stack pointer
//
// Operation: X -> S
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TXS)
{
    cpu.regs.S = cpu.regs.X;
}

/////////////////////////////////////////////////////////
// TYA - Transfer index Y to accumulator
//
// Operation: Y -> A
//
// N Z C I D V
// | | _ _ _ _
//
NES_OP_FUNC_IMPL(TYA)
{
    cpu.regs.A = cpu.regs.Y;
    CALC_NEGATIVE(cpu.regs.A);
    CALC_ZERO(cpu.regs.A);
}

/////////////////////////////////////////////////////////
// Unoffical Opcodes (UFO_*)
//

NES_OP_FUNC_IMPL(UFO_NOP)
{
    if (addr_mode != nsp::Const && addr_mode != nsp::No_Address)
        if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
}

// LAX
// Shortcut for LDA value then TAX. Saves a byte and two cycles
// and allows use of the X register with the (d),Y addressing mode.
// Notice that the immediate is missing; the opcode that would have
// been LAX is affected by line noise on the data0 bus.
// MOS 6502: even the bugs have bugs.
NES_OP_FUNC_IMPL(UFO_LAX)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    cpu.regs.X = cpu.regs.A;
    CALC_NEGATIVE(cpu.regs.X);
    CALC_ZERO(cpu.regs.X);
}

// SAX
// Stores the bitwise AND of A and X. As with STA and STX, no flags are affected.
NES_OP_FUNC_IMPL(UFO_SAX)
{
    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), cpu.regs.A);
    memory_write(emu, addr, cpu.regs.A & cpu.regs.X);
}

// DCP
// Equivalent to DEC value then CMP value, except supporting more addressing modes.
// LDA #$FF followed by DCP can be used to check if the decrement underflows,
// which is useful for multi-byte decrements.
NES_OP_FUNC_IMPL(UFO_DCP)
{
    data0 = MEM_READ_ADDR();

    // DEC
    data1 = data0; // uncertain if this is correct
    data0 = data0 - 1;
    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), data0);
    memory_write(emu, addr, data0);
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);

    // CMP
    data0 = cpu.regs.A - (data0 & 0xFF);

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    SET_CARRY( (cpu.regs.A >= data1) ? 1 : 0 );
}

// SLO
// Equivalent to ASL value then ORA value, except supporting more addressing modes.
// LDA #0 followed by SLO is an efficient way to shift a variable while also loading it in A.
NES_OP_FUNC_IMPL(UFO_SLO)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data0 = data0 << 1;
    memory_write(emu, addr, data0);

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    CALC_CARRY(data0);

    data0 = cpu.regs.A | data0;
    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

// RLA
// Equivalent to ROL value then AND value, except supporting more addressing modes.
// LDA #$FF followed by RLA is an efficient way to rotate a variable while also loading it in A.
NES_OP_FUNC_IMPL(UFO_RLA)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
    data0 = data0 << 1;
    data0 |= GET_CARRY;
    memory_write(emu, addr, data0);

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
    CALC_CARRY(data0);

    data0 = cpu.regs.A & data0;
    cpu.regs.A = data0;

    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

// SRE
// Equivalent to LSR value then EOR value, except supporting more addressing modes.
// LDA #0 followed by SRE is an efficient way to shift a variable while also loading it in A.
NES_OP_FUNC_IMPL(UFO_SRE)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    SET_CARRY(data0 & 0x1);
    data0 = data0 >> 1;
    memory_write(emu, addr, data0);

    SET_NEGATIVE(0);
    CALC_ZERO(data0);

    data0 = cpu.regs.A ^ data0;
    cpu.regs.A = data0;
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);
}

// RRA
// Equivalent to ROR value then ADC value, except supporting more addressing modes.
// Essentially this computes A + value / 2, where value is 9-bit and the division is rounded up.
NES_OP_FUNC_IMPL(UFO_RRA)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);

    data1 = GET_CARRY;
    SET_CARRY(data0 & 0x1);
    data0 = data0 >> 1;
    data0 |= (data1 << 7);
    memory_write(emu, addr, data0);
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);

    data1 = data0;
    data0 = cpu.regs.A + data0 + GET_CARRY;
    //cpu.regs.A = data0;

    CALC_CARRY(data0);
    CALC_ZERO(data0);
    CALC_NEGATIVE(data0);
    CALC_OVERFLOW(cpu.regs.A, data1, data0);

    cpu.regs.A = data0;
}

// SBC
// Duplicates of ADC
// (0x69 and 0xE9 are official; 0xEB is not.
//  These three opcodes are nearly equivalent, except that 0xE9 and 0xEB add 255-i instead of i.)
// http://forums.nesdev.com/viewtopic.php?p=19080#19080
NES_OP_FUNC_IMPL(UFO_SBC)
{
    data0 = MEM_READ_ADDR();

    data0 = data0 ^ 0xFF;
    data1 = data0;
    data0 = cpu.regs.A + data0 + GET_CARRY;

    CALC_CARRY(data0);
    CALC_ZERO(data0);
    CALC_NEGATIVE(data0);
    CALC_OVERFLOW(cpu.regs.A, data1, data0);

    cpu.regs.A = data0;
}

// ISB
// Equivalent to INC value then SBC value, except supporting more addressing modes.
NES_OP_FUNC_IMPL(UFO_ISB)
{
    data0 = MEM_READ_ADDR();

    if (nsp::cb_debug_mem_write) nsp::cb_debug_mem_write(addr, memory_read(emu, addr), 0x0);
    data0 = data0 + 1;
    memory_write(emu, addr, data0);
    CALC_NEGATIVE(data0);
    CALC_ZERO(data0);

    op_UFO_SBC(emu, cpu, addr_mode, instr, addr, addr_ptr);
}


nsp::nes_op_func_t* nsp::NES_OP_JT[256] = {
    op_BRK,
    op_ORA,
    op_INVALID,
    op_UFO_SLO,
    op_UFO_NOP,
    op_ORA,
    op_ASL,
    op_UFO_SLO,
    op_PHP,
    op_ORA,
    op_ASL,
    op_INVALID,
    op_UFO_NOP,
    op_ORA,
    op_ASL,
    op_UFO_SLO,
    op_BPL,
    op_ORA,
    op_INVALID,
    op_UFO_SLO,
    op_UFO_NOP,
    op_ORA,
    op_ASL,
    op_UFO_SLO,
    op_CLC,
    op_ORA,
    op_UFO_NOP,
    op_UFO_SLO,
    op_UFO_NOP,
    op_ORA,
    op_ASL,
    op_UFO_SLO,
    op_JSR,
    op_AND,
    op_INVALID,
    op_UFO_RLA,
    op_BIT,
    op_AND,
    op_ROL,
    op_UFO_RLA,
    op_PLP,
    op_AND,
    op_ROL,
    op_INVALID,
    op_BIT,
    op_AND,
    op_ROL,
    op_UFO_RLA,
    op_BMI,
    op_AND,
    op_INVALID,
    op_UFO_RLA,
    op_UFO_NOP,
    op_AND,
    op_ROL,
    op_UFO_RLA,
    op_SEC,
    op_AND,
    op_UFO_NOP,
    op_UFO_RLA,
    op_UFO_NOP,
    op_AND,
    op_ROL,
    op_UFO_RLA,
    op_RTI,
    op_EOR,
    op_INVALID,
    op_UFO_SRE,
    op_UFO_NOP,
    op_EOR,
    op_LSR,
    op_UFO_SRE,
    op_PHA,
    op_EOR,
    op_LSR,
    op_INVALID,
    op_JMP,
    op_EOR,
    op_LSR,
    op_UFO_SRE,
    op_BVC,
    op_EOR,
    op_INVALID,
    op_UFO_SRE,
    op_UFO_NOP,
    op_EOR,
    op_LSR,
    op_UFO_SRE,
    op_CLI,
    op_EOR,
    op_UFO_NOP,
    op_UFO_SRE,
    op_UFO_NOP,
    op_EOR,
    op_LSR,
    op_UFO_SRE,
    op_RTS,
    op_ADC,
    op_INVALID,
    op_UFO_RRA,
    op_UFO_NOP,
    op_ADC,
    op_ROR,
    op_UFO_RRA,
    op_PLA,
    op_ADC,
    op_ROR,
    op_INVALID,
    op_JMP,
    op_ADC,
    op_ROR,
    op_UFO_RRA,
    op_BVS,
    op_ADC,
    op_INVALID,
    op_UFO_RRA,
    op_UFO_NOP,
    op_ADC,
    op_ROR,
    op_UFO_RRA,
    op_SEI,
    op_ADC,
    op_UFO_NOP,
    op_UFO_RRA,
    op_UFO_NOP,
    op_ADC,
    op_ROR,
    op_UFO_RRA,
    op_UFO_NOP,
    op_STA,
    op_INVALID,
    op_UFO_SAX,
    op_STY,
    op_STA,
    op_STX,
    op_UFO_SAX,
    op_DEY,
    op_INVALID,
    op_TXA,
    op_INVALID,
    op_STY,
    op_STA,
    op_STX,
    op_UFO_SAX,
    op_BCC,
    op_STA,
    op_INVALID,
    op_INVALID,
    op_STY,
    op_STA,
    op_STX,
    op_UFO_SAX,
    op_TYA,
    op_STA,
    op_TXS,
    op_INVALID,
    op_INVALID,
    op_STA,
    op_INVALID,
    op_INVALID,
    op_LDY,
    op_LDA,
    op_LDX,
    op_UFO_LAX,
    op_LDY,
    op_LDA,
    op_LDX,
    op_UFO_LAX,
    op_TAY,
    op_LDA,
    op_TAX,
    op_INVALID,
    op_LDY,
    op_LDA,
    op_LDX,
    op_UFO_LAX,
    op_BCS,
    op_LDA,
    op_INVALID,
    op_UFO_LAX,
    op_LDY,
    op_LDA,
    op_LDX,
    op_UFO_LAX,
    op_CLV,
    op_LDA,
    op_TSX,
    op_INVALID,
    op_LDY,
    op_LDA,
    op_LDX,
    op_UFO_LAX,
    op_CPY,
    op_CMP,
    op_INVALID,
    op_UFO_DCP,
    op_CPY,
    op_CMP,
    op_DEC,
    op_UFO_DCP,
    op_INY,
    op_CMP,
    op_DEX,
    op_INVALID,
    op_CPY,
    op_CMP,
    op_DEC,
    op_UFO_DCP,
    op_BNE,
    op_CMP,
    op_INVALID,
    op_UFO_DCP,
    op_UFO_NOP,
    op_CMP,
    op_DEC,
    op_UFO_DCP,
    op_CLD,
    op_CMP,
    op_UFO_NOP,
    op_UFO_DCP,
    op_UFO_NOP,
    op_CMP,
    op_DEC,
    op_UFO_DCP,
    op_CPX,
    op_SBC,
    op_INVALID,
    op_UFO_ISB,
    op_CPX,
    op_SBC,
    op_INC,
    op_UFO_ISB,
    op_INX,
    op_SBC,
    op_NOP,
    op_UFO_SBC,
    op_CPX,
    op_SBC,
    op_INC,
    op_UFO_ISB,
    op_BEQ,
    op_SBC,
    op_INVALID,
    op_UFO_ISB,
    op_UFO_NOP,
    op_SBC,
    op_INC,
    op_UFO_ISB,
    op_SED,
    op_SBC,
    op_UFO_NOP,
    op_UFO_ISB,
    op_UFO_NOP,
    op_SBC,
    op_INC,
    op_UFO_ISB,
};
