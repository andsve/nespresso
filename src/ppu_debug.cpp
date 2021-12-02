#include "ppu_debug.h"
#include "nsp_log.h"
#include "bitmap_ops.h"

#include <MiniFB.h>

static bool ppu_get_chr_debug(nsp::emu_t& emu, uint16_t chr_i, uint8_t* out_data)
{
    if (!emu.ppu.chr_rom) {
        return false;
    }

    uint8_t* chr_data = (uint8_t*)emu.ppu.chr_rom + chr_i;

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
            uint32_t pix_i = y*8+(7-x);
            out_data[pix_i] = out_data[pix_i] | (((d >> x) & 0x1) << 1);
        }
        chr_data++;
    }

    return true;
}

static void blit_chr_debug(nsp::emu_t& emu, uint32_t blit_x, uint32_t blit_y, uint32_t chr_index)
{
    uint8_t chr[8*8];
    if (!ppu_get_chr_debug(emu, chr_index*16, chr))
    {
        return;
    }

    for (uint32_t y = 0; y < 8; ++y)
    {
        for (uint32_t x = 0; x < 8; ++x)
        {
            uint8_t pix = chr[y*8+x];

            uint32_t tx = blit_x + x;
            uint32_t ty = blit_y + y;

            uint32_t ti = ty*NES_WIDTH + tx;

            // REMEMBER: pix is the palett index really, so no color!
            // But we know that only two bits are used, values are 0..4;
            // multiply the pix with 64 to get "grayscale"...
            nsp::window_buffer[ti] = MFB_RGB(pix*64, pix*64, pix*64);
        }
    }
}

void dump_chr_rom_debug(nsp::emu_t& emu)
{
    uint16_t chr_i = 0;
    for (uint32_t yi = 0; yi < 16; ++yi)
    {
        for (uint32_t xi = 0; xi < 32; ++xi)
        {
            blit_chr_debug(emu, xi*8, 1+yi*8, chr_i);
            chr_i++;
        }
    }
}

void dump_ppu_nametable_debug(nsp::emu_t& emu)
{
    for (uint32_t yi = 0; yi < 30; ++yi)
    {
        for (uint32_t xi = 0; xi < 32; ++xi)
        {

            uint32_t nti = yi*32+xi;
            if (nti >= 0x800) {
                LOG_E("trying to access vram out of bounds!\n");
            }
            uint8_t chr_i = emu.ppu.vram[nti];
            blit_chr_debug(emu, xi*8, yi*8, chr_i);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// debug stuff above only!

static uint8_t color_2c02[] = { 0x80, 0x80, 0x80, 0x00, 0x3D, 0xA6, 0x00, 0x12, 0xB0, 0x44, 0x00, 0x96, 0xA1, 0x00, 0x5E,
   0xC7, 0x00, 0x28, 0xBA, 0x06, 0x00, 0x8C, 0x17, 0x00, 0x5C, 0x2F, 0x00, 0x10, 0x45, 0x00,
   0x05, 0x4A, 0x00, 0x00, 0x47, 0x2E, 0x00, 0x41, 0x66, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05,
   0x05, 0x05, 0x05, 0xC7, 0xC7, 0xC7, 0x00, 0x77, 0xFF, 0x21, 0x55, 0xFF, 0x82, 0x37, 0xFA,
   0xEB, 0x2F, 0xB5, 0xFF, 0x29, 0x50, 0xFF, 0x22, 0x00, 0xD6, 0x32, 0x00, 0xC4, 0x62, 0x00,
   0x35, 0x80, 0x00, 0x05, 0x8F, 0x00, 0x00, 0x8A, 0x55, 0x00, 0x99, 0xCC, 0x21, 0x21, 0x21,
   0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0xFF, 0xFF, 0xFF, 0x0F, 0xD7, 0xFF, 0x69, 0xA2, 0xFF,
   0xD4, 0x80, 0xFF, 0xFF, 0x45, 0xF3, 0xFF, 0x61, 0x8B, 0xFF, 0x88, 0x33, 0xFF, 0x9C, 0x12,
   0xFA, 0xBC, 0x20, 0x9F, 0xE3, 0x0E, 0x2B, 0xF0, 0x35, 0x0C, 0xF0, 0xA4, 0x05, 0xFB, 0xFF,
   0x5E, 0x5E, 0x5E, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0xFF, 0xFF, 0xFF, 0xA6, 0xFC, 0xFF,
   0xB3, 0xEC, 0xFF, 0xDA, 0xAB, 0xEB, 0xFF, 0xA8, 0xF9, 0xFF, 0xAB, 0xB3, 0xFF, 0xD2, 0xB0,
   0xFF, 0xEF, 0xA6, 0xFF, 0xF7, 0x9C, 0xD7, 0xE8, 0x95, 0xA6, 0xED, 0xAF, 0xA2, 0xF2, 0xDA,
   0x99, 0xFF, 0xFC, 0xDD, 0xDD, 0xDD, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};

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

void dump_ppu_nametable_ids(nsp::emu_t& emu)
{
    for (uint32_t yi = 0; yi < 7; ++yi)
    {
        for (uint32_t xi = 0; xi < 7; ++xi)
        {

            uint32_t nti = yi*32+xi;
            if (nti >= 0x800) {
                LOG_E("trying to access vram out of bounds!\n");
            }
            uint8_t chr_i = emu.ppu.vram[nti];

            draw_text(xi*32, yi*32, "% 3u", chr_i);
        }
    }
}

void dump_chr_rom(nsp::emu_t& emu)
{
    uint16_t chr_i = 0;
    for (uint32_t yi = 0; yi < 16; ++yi)
    {
        for (uint32_t xi = 0; xi < 16; ++xi)
        {
            blit_chr(emu, xi*8, 1+yi*8, chr_i, true, false, false, 0x0); // ignore fliping for now

            chr_i++;
        }
    }
}

void dump_chr_bitfields(nsp::emu_t& emu, uint8_t chr_i)
{
    uint8_t chr[8*8];
    if (!ppu_get_chr(emu, chr_i*16, chr, true))
    {
        return;
    }

    for (uint32_t yi = 0; yi < 8; ++yi)
    {
        for (uint32_t xi = 0; xi < 8; ++xi)
        {
            uint8_t d = chr[yi*8+xi];
            draw_text(xi*32, 1+yi*32, "%s%s", (d & 0x2) == 0x2 ? "1" : "0", (d & 0x1) == 0x1 ? "1" : "0");
        }
    }
}
