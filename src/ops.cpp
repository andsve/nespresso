#include "nsp.h"
#include "nsp_log.h"
#include "nsp_ops.h"

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

