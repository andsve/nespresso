#include <string.h>

#include <MiniFB.h>

#include "nsp.h"
#include "nsp_log.h"
#include "nsp_nestest_logger.h"

#include "bitmap_ops.h"
#include "ppu_debug.h"

#define NESTEST_PATH "../data/nestest.nes"

uint32_t nsp::window_buffer[NES_WIDTH * NES_HEIGHT * 4];
static uint8_t gray_palette[] = {0,0,0, 85,85,85, 170,170,170, 255,255,255};

static void blit_chr(const uint8_t* chr_tile, uint32_t blit_x, uint32_t blit_y)
{
    uint8_t tile_pixels[8*8];

    // low bits
    for (uint32_t y = 0; y < 8; ++y)
    {
        uint8_t d = *chr_tile;
        for (uint32_t x = 0; x < 8; ++x)
        {
            // clear pix
            uint32_t pix_i = y*8+(7-x);
            tile_pixels[pix_i] = 0x0;

            tile_pixels[pix_i] = (d >> x) & 0x1;
        }
        chr_tile++;
    }

    // high bits
    for (uint32_t y = 0; y < 8; ++y)
    {
        uint8_t d = *chr_tile;
        for (uint32_t x = 0; x < 8; ++x)
        {
            // clear pix
            uint32_t pix_i = y*8+(7-x);

            tile_pixels[pix_i] = tile_pixels[pix_i] | (((d >> x) & 0x1) << 1);
        }
        chr_tile++;
    }

    // nothing special here, just copy over pixels from tile_pixels to window buffer
    for (uint32_t y = 0; y < 8; ++y)
    {
        for (uint32_t x = 0; x < 8; ++x)
        {
            uint8_t pix = tile_pixels[y*8+x];

            uint32_t tx = blit_x + x;
            uint32_t ty = blit_y + y;

            uint32_t ti = ty*NES_WIDTH + tx;

            nsp::window_buffer[ti] = MFB_RGB(gray_palette[pix*3], gray_palette[pix*3], gray_palette[pix*3]);
        }
    }
}

static void dump_chr_rom(nsp::ines_rom_t& rom)
{
    const uint8_t* chr_data = rom.chr_pages[0];
    for (uint32_t yi = 0; yi < 16; ++yi)
    {
        for (uint32_t xi = 0; xi < 32; ++xi)
        {
            const uint8_t* chr_tile = &chr_data[(yi*32 + xi)*16];
            blit_chr(chr_tile, xi*8, yi*8);
        }
    }
}

int main(int argc, char const *argv[])
{
    const char* rom_filepath = NESTEST_PATH;

    if (argc > 1) {
        rom_filepath = argv[1];
    }

    // Load ROM and dump info
    nsp::ines_rom_t rom;
    nsp::RESULT result = nsp::load_rom_file(rom_filepath, rom);
    if (nsp::RESULT_OK != result) return 1;
    LOG_D(" -> PRG page count: %d", rom.prg_page_count);
    LOG_D(" -> CHR page count: %d", rom.chr_page_count);

    struct mfb_window *window = 0x0;

    clear_window_buffer(255, 0, 0);
    window = mfb_open_ex("nespresso", NES_WIDTH, NES_HEIGHT, WF_RESIZABLE);

    do
    {
        dimm_window_buffer(0.0f);

        dump_chr_rom(rom);


        int32_t state = mfb_update_ex(window, nsp::window_buffer, NES_WIDTH, NES_HEIGHT);
        if (state < 0)
            break;

    } while(mfb_wait_sync(window));

    mfb_close(window);

    return 0;
}
