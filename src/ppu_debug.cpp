#include "ppu_debug.h"
#include "nsp_log.h"

#include <MiniFB.h>

static uint8_t color_2c02[] = {
 84,  84,  84,    0,  30, 116,    8,  16, 144,   48,   0, 136,   68,   0, 100,   92,   0,  48,   84,   4,   0,   60,  24,   0,   32,  42,   0,    8,  58,   0,    0,  64,   0,    0,  60,   0,    0,  50,  60,    0,   0,   0, 0,0,0, 0,0,0,
152, 150, 152,    8,  76, 196,   48,  50, 236,   92,  30, 228,  136,  20, 176,  160,  20, 100,  152,  34,  32,  120,  60,   0,   84,  90,   0,   40, 114,   0,    8, 124,   0,    0, 118,  40,    0, 102, 120,    0,   0,   0, 0,0,0, 0,0,0,
236, 238, 236,   76, 154, 236,  120, 124, 236,  176,  98, 236,  228,  84, 236,  236,  88, 180,  236, 106, 100,  212, 136,  32,  160, 170,   0,  116, 196,   0,   76, 208,  32,   56, 204, 108,   56, 180, 204,   60,  60,  60, 0,0,0, 0,0,0,
236, 238, 236,  168, 204, 236,  188, 188, 236,  212, 178, 236,  236, 174, 236,  236, 174, 212,  236, 180, 176,  228, 196, 144,  204, 210, 120,  180, 222, 120,  168, 226, 144,  152, 226, 180,  160, 214, 228,  160, 162, 160, 0,0,0, 0,0,0,
};

static uint8_t gray_palette[] = {0,0,0, 85,85,85, 170,170,170, 255,255,255};

inline uint8_t palette_id_to_red(uint32_t id) {
    return color_2c02[id*3];
}

inline uint8_t palette_id_to_green(uint32_t id) {
    return color_2c02[id*3+1];
}

inline uint8_t palette_id_to_blue(uint32_t id) {
    return color_2c02[id*3+2];
}


static bool ppu_get_chr(nsp::emu_t& emu, uint16_t addr, uint8_t* out_data, bool bg)
{
    if (!emu.ppu.chr_rom) {
        return false;
    }

    uint16_t chr_offset = 0x0;
    if (bg) {
        if ((emu.ppu.ppuctrl >> 4) & 0x1) {
            chr_offset = 0x1000;
        }
    } else {
        if ((emu.ppu.ppuctrl >> 3) & 0x1) {
            chr_offset = 0x1000;
        }
    }

    uint8_t* chr_data = (uint8_t*)emu.ppu.chr_rom+addr+chr_offset;

    // low bits
    for (uint32_t y = 0; y < 8; ++y)
    {
        uint8_t d = *chr_data;
        for (uint32_t x = 0; x < 8; ++x)
        {
            // clear pix
            uint32_t pix_i = y*8+(7-x);
            out_data[pix_i] = 0x0;

            out_data[pix_i] = (d >> x) & 0x1;
        }
        chr_data++;
    }

    // high bits
    for (uint32_t y = 0; y < 8; ++y)
    {
        uint8_t d = *chr_data;
        for (uint32_t x = 0; x < 8; ++x)
        {
            // clear pix
            uint32_t pix_i = y*8+(7-x);

            out_data[pix_i] = out_data[pix_i] | (((d >> x) & 0x1) << 1);
        }
        chr_data++;
    }

    return true;
}

static void blit_chr(nsp::emu_t& emu, uint32_t blit_x, uint32_t blit_y, uint32_t chr_index, bool bg, bool flip_x, bool flip_y, uint8_t* palette_set)
{
    uint8_t chr[8*8];
    if (!ppu_get_chr(emu, chr_index*16, chr, bg))
    {
        return;
    }

    for (uint32_t y = 0; y < 8; ++y)
    {
        for (uint32_t x = 0; x < 8; ++x)
        {
            uint8_t pix = chr[y*8+x];

            if (!bg && pix == 0x0)
                continue;

            uint32_t tx = blit_x + (flip_x ? 7 - x : x);
            uint32_t ty = blit_y + (flip_y ? 7 - y : y);

            uint32_t ti = ty*NES_WIDTH + tx;

            if (bg && pix == 0x0) {
                uint32_t palette_bg = emu.ppu.palette[0x00];
                nsp::window_buffer[ti] = MFB_RGB(palette_id_to_red(palette_bg), palette_id_to_green(palette_bg), palette_id_to_blue(palette_bg));
            } else if (palette_set == 0x0) {
                nsp::window_buffer[ti] = MFB_RGB(gray_palette[pix*3], gray_palette[pix*3], gray_palette[pix*3]);
            } else {
                nsp::window_buffer[ti] = MFB_RGB(palette_id_to_red(palette_set[pix-1]), palette_id_to_green(palette_set[pix-1]), palette_id_to_blue(palette_set[pix-1]));
            }
        }
    }
}

void dump_ppu_vram(nsp::emu_t& emu)
{
    uint8_t palette_set[3];
    for (uint32_t yi = 0; yi < 30; ++yi)
    {
        for (uint32_t xi = 0; xi < 32; ++xi)
        {

            uint32_t nti = yi*32+xi;
            if (nti >= 0x800) {
                LOG_E("trying to access vram out of bounds!\n");
            }
            uint8_t chr_i = emu.ppu.vram[nti];

            // find correct attribute
            unsigned short attribute_addr = ((yi / 4)*8)+(xi / 4);
            uint8_t attribute = emu.ppu.vram[attribute_addr+0x3C0];

            uint32_t sub_x = (xi / 2) % 2;
            uint32_t sub_y = (yi / 2) % 2;
            uint32_t palette_id = 0;

            uint32_t pr = 0, pg = 0, pb = 0;

            if (sub_x == 0 && sub_y == 0) {
                palette_id = attribute & 0x3;
                pb = 255;
            } else if (sub_x == 1 && sub_y == 0) {
                pr = 255;
                palette_id = (attribute >> 2) & 0x3;
            } else if (sub_x == 0 && sub_y == 1) {
                pg = 255;
                palette_id = (attribute >> 4) & 0x3;
            } else if (sub_x == 1 && sub_y == 1) {
                pr = 255;
                pg = 255;
                palette_id = (attribute >> 6) & 0x3;
            }
            palette_set[0] = emu.ppu.palette[0x01+palette_id*4];
            palette_set[1] = emu.ppu.palette[0x02+palette_id*4];
            palette_set[2] = emu.ppu.palette[0x03+palette_id*4];
            blit_chr(emu, xi*8, yi*8, chr_i, true, false, false, palette_set); // ignore fliping for now
        }
    }
}

void dump_ppu_sprites(nsp::emu_t& emu)
{
    uint8_t palette_set[3];
    for (uint32_t sprite_i = 0; sprite_i < 64; ++sprite_i)
    {
        uint8_t sprite_data0 = emu.ppu.oam[sprite_i*4+0]; // y
        uint8_t sprite_data1 = emu.ppu.oam[sprite_i*4+1]; // tile index
        uint8_t sprite_data2 = emu.ppu.oam[sprite_i*4+2]; // attributes
        uint8_t sprite_data3 = emu.ppu.oam[sprite_i*4+3]; // x

        bool flip_x = !!(sprite_data2 & (1 << 6));
        bool flip_y = !!(sprite_data2 & (1 << 7));
        uint8_t palette_id = sprite_data2 & 0x3;

        // uint8_t palette_ids = emu.ppu.palette[0x11+palette_id*3];
        palette_set[0] = emu.ppu.palette[0x11+palette_id*4];
        palette_set[1] = emu.ppu.palette[0x12+palette_id*4];
        palette_set[2] = emu.ppu.palette[0x13+palette_id*4];

        blit_chr(emu, sprite_data3, sprite_data0+1, sprite_data1, false, flip_x, flip_y, palette_set);
    }
}
