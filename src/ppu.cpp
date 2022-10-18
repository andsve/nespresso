#include <string.h>

#include "nsp.h"
#include "nsp_log.h"

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

// static uint8_t gray_palette[] = {0,0,0, 85,85,85, 170,170,170, 255,255,255};

inline uint8_t palette_id_to_red(uint32_t id) {
    return color_2c02[id*3];
}

inline uint8_t palette_id_to_green(uint32_t id) {
    return color_2c02[id*3+1];
}

inline uint8_t palette_id_to_blue(uint32_t id) {
    return color_2c02[id*3+2];
}

static void check_sprite0_hit(nsp::ppu_t& ppu);

uint32_t nsp::step_ppu(emu_t& emu, uint32_t max_cycles)
{
    ppu_t& ppu = emu.ppu;

    uint32_t delta_cycles = 0x0;
    uint32_t start_cycles = emu.ppu.cycles;

    while (max_cycles > delta_cycles)
    {
        // raster one pixel
        ppu_raster(emu);

        // Advance rasterer state:
        //   On an NTSC version of the NES, there are 240 scanlines on the screen (although the top
        //   and bottom eight lines are cut off) and it takes an additional 3 scanlines worth of CPU cycles
        //   to enter V-Blank. The V-Blank period takes a further 20 scanlines worth before the next
        //   frame can be drawn.
        //
        //   The NTSC video signal is made up of 262 scanlines, and 20 of those are spent in vblank state.
        //   After the program has received an NMI, it has about 2270 cycles to update the palette,
        //   sprites, and nametables as necessary before rendering begins.
        ppu.x += 1;

        // Check if in pre-render line
        if (ppu.y == 261 && ppu.x == 1) {
            ppu.ppustatus &= ~0x80;
            // Sprite 0 Hit flag is cleared at dot 1 of the pre-render line.
            ppu.ppustatus &= ~0x40;
        }

        // check_sprite0_hit(ppu);
        (void)check_sprite0_hit;

        // Wrap to new scanline
        if (ppu.x > 340) {
            ppu.x = 0;
            ppu.y += 1;
            if (ppu.y > 261) {
                ppu.y = 0;
            }
        }

        // OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines.
        if (ppu.y == 261 || ppu.y == 340) {
            if (ppu.x >= 256 && ppu.x <= 320) {
                ppu.oamaddr = 0x0;
            }
        }

        // Time for vblank?
        if (ppu.y == 241 && ppu.x == 1) {

            // Vertical blank has started (0: not in vblank; 1: in vblank).
            // Set at dot 1 of line 241 (the line *after* the post-render
            // line); cleared after reading $2002 and at dot 1 of the
            // pre-render line.
            ppu.ppustatus |= 0x80;

            // PPUCTRL Bit 8; Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
            if (ppu.ppuctrl & 0x80) {
                stack_push_short(emu, emu.cpu.regs.PC);
                stack_push(emu, emu.cpu.regs.P);

                emu.cpu.regs.I = 1;
                emu.cpu.regs.PC = emu.cpu.vectors.NMI;

                if (emu.waiting_for_vblank) {
                    emu.waiting_for_vblank = false;
                }
            }
        }

        // TODO: actual rasterization, lol.

        ppu.cycles += 1;
        delta_cycles = ppu.cycles - start_cycles;
    }

    return delta_cycles;
}

#define MFB_RGB(r, g, b)    (((uint32_t) r) << 16) | (((uint32_t) g) << 8) | ((uint32_t) b)

bool nsp::ppu_raster(emu_t& emu)
{
    ppu_t& ppu = emu.ppu;

    // perform background and sprite data fetch and pixel output
    ppu_bg_pipeline(emu);
    ppu_sprite_pipeline(emu);

    uint16_t& x = ppu.x;
    uint16_t& y = ppu.y;

    uint16_t render_x = x - 1;
    uint16_t render_y = y;

    if (render_x < NES_WIDTH && render_y < NES_HEIGHT)
    {
        uint16_t pixel_index = render_y*NES_WIDTH+render_x;
        // ppu.screen[pixel_index] = MFB_RGB(ppu.pixel_bg, ppu.pixel_bg, ppu.pixel_bg);
        // ppu.screen[pixel_index] = MFB_RGB(ppu.color_bg, ppu.color_bg, ppu.color_bg);
        // ppu.screen[pixel_index] = ppu.color_bg;

        // debug: output red if sprite is visible?
        // if (ppu.color_sp > 0x0) {
        // if (ppu.pixel_sp > 0x0) {
        //     if (ppu.sprite_prio == 0)
        //         ppu.screen[pixel_index] = ppu.color_sp;
        // }

        // multiplex and prio logic

        ppu.screen[pixel_index] = ppu.color_bg;


        if (ppu.pixel_sp != 0)
        {
            if (ppu.pixel_bg == 0) {
                ppu.screen[pixel_index] = ppu.color_sp;
            } else if (ppu.sprite_prio == 0) {
                ppu.screen[pixel_index] = ppu.color_sp;
            } else {
                ppu.screen[pixel_index] = ppu.color_bg;
            }
        }
        /*
        if (ppu.pixel_bg == 0 && ppu.pixel_sp == 0) {
            ppu.screen[pixel_index] = ppu.color_bg;
        } else if (ppu.pixel_bg == 0) {
            ppu.screen[pixel_index] = ppu.color_sp;
        } else if (ppu.pixel_sp == 0) {
            ppu.screen[pixel_index] = ppu.color_bg;
        } else {
            if (ppu.sprite_prio == 0)
                ppu.screen[pixel_index] = ppu.color_sp;
            else
                ppu.screen[pixel_index] = ppu.color_bg;
        }
        */

        if (ppu.check_sprite0)
        {
            // if ((x+y) % 2 == 0)
            //     ppu.screen[pixel_index] = MFB_RGB(0, 0, 0);
            if (ppu.pixel_bg > 0 && ppu.pixel_sp > 0)
            {
                ppu.ppustatus |= 0x40;
            }
        }

        // if (ppu.sprite0_included)
        // {
        //     if ((x+y) % 2 == 0)
        //         ppu.screen[pixel_index] = MFB_RGB(255, 255, 255);
        // }

        // ppu.screen[pixel_index] = MFB_RGB(255, 255, 255);
        // if (ppu.pixel_sp != 0)
        //     ppu.screen[pixel_index] = ppu.color_sp;
    }

    return true;
}

bool nsp::ppu_bg_pipeline(emu_t& emu)
{
    ppu_t& ppu = emu.ppu;

    if (!ppu.ppumask.show_bg) {
        // LOG_D("not rendering bg");
        return false;
    }

    uint16_t& x = ppu.x;
    uint16_t& y = ppu.y;

    uint16_t render_x = x - 1;
    uint16_t render_y = y;

    ppu.pixel_bg = 0x0;
    ppu.color_bg = 0x0;
    if (render_x < NES_WIDTH && render_y < NES_HEIGHT)
    {
        // uint8_t pixel = 0x0;

        uint8_t pattern_lo = ppu.pattern_lo.hi;
        uint8_t pattern_hi = ppu.pattern_hi.hi;
        uint8_t pattern = ((((pattern_lo >> (7-ppu.fine_x)) & 0x1) << 0) |
                           (((pattern_hi >> (7-ppu.fine_x)) & 0x1) << 1));

        uint8_t palette_lo = ppu.at.lo;
        uint8_t palette_hi = ppu.at.hi;
        uint8_t palette_id = ((((palette_lo >> (7-ppu.fine_x)) & 0x1) << 0) |
                              (((palette_hi >> (7-ppu.fine_x)) & 0x1) << 1));

        ppu.pixel_bg = pattern;

        if (pattern == 0x0) {
            uint32_t palette_bg = ppu.palette[0x00];
            ppu.color_bg = MFB_RGB(palette_id_to_red(palette_bg), palette_id_to_green(palette_bg), palette_id_to_blue(palette_bg));
        } else {
            static uint8_t palette_set[3];
            palette_set[0] = ppu.palette[0x01+palette_id*4];
            palette_set[1] = ppu.palette[0x02+palette_id*4];
            palette_set[2] = ppu.palette[0x03+palette_id*4];
            ppu.color_bg = MFB_RGB(palette_id_to_red(palette_set[pattern-1]), palette_id_to_green(palette_set[pattern-1]), palette_id_to_blue(palette_set[pattern-1]));
        }
    }

    if (((x >= 1 && x <= 256) ||
         (x >= 321 && x <= 336)) &&
        (y < 240 || y == 261)) // visible frames and pre render line
    {
        ppu.pattern_lo.val = ppu.pattern_lo.val << 1;
        ppu.pattern_hi.val = ppu.pattern_hi.val << 1;
        ppu.at.lo = ppu.at.lo << 1;
        ppu.at.hi = ppu.at.hi << 1;

        ppu.at.lo |= (ppu.at_latch & 0b01);
        ppu.at.hi |= (ppu.at_latch & 0b10)>>1;
    }

    // BG data fetch
    // https://www.nesdev.org/w/images/d/d1/Ntsc_timing.png
    if (x != 0 && // x/tick 0 is idle
        (y < 240 || y == 261)) // visible frames and pre render line
    {
        uint16_t fetch_tick = x % 8;

        if (x <= 256 ||
            (x >= 319 && x <= 340)) {

            if (fetch_tick == 0) {
                ppu.pattern_lo.lo = ppu.pattern_latch & 0x00FF;
                ppu.pattern_hi.lo = (ppu.pattern_latch & 0xFF00) >> 8;

                ppu.at_latch = (ppu.at_byte & 0b11);
                // LOG_D("ppu.at_latch: %x", ppu.at_latch);

            } else if (fetch_tick == 1) {


            } else if (fetch_tick == 2) {
                // NT byte
                uint16_t nt_base_ddr = 0x2000;
                // uint16_t nt_base_ddr = base_nt_addr(emu);
                // uint8_t ppuctrl_nt = ppu.ppuctrl & 0x3;
                // if (ppuctrl_nt == 0x1) {
                //     nt_base_ddr = 0x2400;
                // } else if (ppuctrl_nt == 0x2) {
                //     nt_base_ddr = 0x2800;
                // } else if (ppuctrl_nt == 0x3) {
                //     nt_base_ddr = 0x2C00;
                // }

                uint16_t nt_addr = nt_base_ddr | (ppu.LoopyV.val & 0x0FFF);
                ppu.nt_tile = ppu_read_vram(emu, nt_addr);

            } else if (fetch_tick == 4) {
                // AT byte
                // uint16_t at_addr = 0x23C0
                //                   | ((ppu.LoopyV.val >> 0) & 0x0C00)
                //                   | ((ppu.LoopyV.val >> 4) & 0x38)
                //                   | ((ppu.LoopyV.val >> 2) & 0x07);
                uint16_t at_addr = 0x23C0 | (ppu.LoopyV.val & 0x0C00) | ((ppu.LoopyV.val >> 4) & 0x38) | ((ppu.LoopyV.val >> 2) & 0x07);
                ppu.at_byte = ppu_read_vram(emu, at_addr);

                if (ppu.LoopyV.coarse_y & 2) ppu.at_byte >>= 4;
                if (ppu.LoopyV.coarse_x & 2) ppu.at_byte >>= 2;

            } else if (fetch_tick == 6) {
                // Low BG tile byte

                uint16_t fine_y = ppu.LoopyV.fine_y;
                uint16_t tile_addr = ppu.nt_tile * 16 + fine_y;
                if ((emu.ppu.ppuctrl >> 4) & 0x1) {
                    tile_addr += 0x1000;
                }
                // ppu.next_pattern_lo = ppu_read_vram(emu, tile_addr + 0);
                ppu.pattern_latch = ppu_read_vram(emu, tile_addr + 0);

            } else if (fetch_tick == 7) {
                // High BG tile byte

                uint16_t fine_y = ppu.LoopyV.fine_y;
                uint16_t tile_addr = ppu.nt_tile * 16 + fine_y;
                if ((emu.ppu.ppuctrl >> 4) & 0x1) {
                    tile_addr += 0x1000;
                }
                // ppu.next_pattern_hi = ppu_read_vram(emu, tile_addr + 8);
                ppu.pattern_latch |= (ppu_read_vram(emu, tile_addr + 8)) << 8;
            }
        }

        // inc hori(v)
        if (fetch_tick == 0 &&
            (x <= 256 || x >= 321)) {
            if ((ppu.LoopyV.coarse_x) == 31) { // if coarse X == 31
              ppu.LoopyV.coarse_x = 0x0; // coarse X = 0
              ppu.LoopyV.nt ^= 0x1;      // switch horizontal nametable
              // ppu.LoopyV.val ^= 0x0400;           // switch horizontal nametable
            } else {
              ppu.LoopyV.val += 1;       // increment coarse X
            }
        }


        // inc vert(v)
        if (x == 256) {
            if (ppu.LoopyV.fine_y != 0x7) {             // if fine Y < 7
              ppu.LoopyV.fine_y += 0x1;                 // increment fine Y
            } else {
              ppu.LoopyV.fine_y = 0x0;                  // fine Y = 0
              uint16_t ty = ppu.LoopyV.coarse_y;        // let y = coarse Y
              if (ty == 29) {
                ty = 0;                                 // coarse Y = 0
                // ppu.LoopyV.val ^= 0x10;                 // switch vertical nametable
                ppu.LoopyV.val ^= 0x0800;                    // switch vertical nametable
              } else if (ty == 31) {
                ty = 0;                                 // coarse Y = 0, nametable not switched
              } else {
                ty += 1;                                // increment coarse Y
              }
              // ppu.LoopyV.coarse_y = ty; // put coarse Y back into v
              ppu.LoopyV.val = (ppu.LoopyV.val & ~0x03E0) | (ty << 5);
            }
        } else if (x == 257) {
            // hori(v) = hori(t)
            // v: ....A.. ...BCDEF <- t: ....A.. ...BCDEF
            ppu.LoopyV.coarse_x = ppu.LoopyT.coarse_x;
            ppu.LoopyV.nt = (ppu.LoopyV.nt & 0b10) | (ppu.LoopyT.nt & 0b01);
        }
    }

    // vert(v) = vert(t) (during pre-render line)
    if (y == 261 && x >= 280 && x <= 304) {
        ppu.LoopyV.coarse_y = ppu.LoopyT.coarse_y;
        ppu.LoopyV.fine_y = ppu.LoopyT.fine_y;
        ppu.LoopyV.nt = (ppu.LoopyV.nt & 0b01) | (ppu.LoopyT.nt & 0b10);
    }

    return true;
}

bool nsp::ppu_sprite_pipeline(emu_t& emu)
{
    ppu_t& ppu = emu.ppu;

    // this might not be correct
    if (!ppu.ppumask.show_sprites) {
        // LOG_D("not rendering sprites");
        return false;
    }


    uint16_t& x = ppu.x;
    uint16_t& y = ppu.y;

    // onlt done during visible frames
    if (y >= 240) {
        return false;
    }

    // uint16_t render_x = x - 1;
    // uint16_t render_y = y;

    ppu.pixel_sp = 0x0;
    ppu.color_sp = 0x0;
    ppu.sprite_prio = 0x0;

    /*
    Every cycle, the 8 x-position counters for the sprites are decremented by one.
    For each sprite, if the counter is still nonzero, nothing else happens.

    If the counter is zero, the sprite becomes "active", and the respective pair
    of shift registers for the sprite is shifted once every cycle. This output
    accompanies the data in the sprite's latch, to form a pixel. The current pixel
    for each "active" sprite is checked (from highest to lowest priority), and the
    first non-transparent pixel moves on to a multiplexer, where it joins the BG pixel.
    */
    if (x >= 1 && x <= 256) {
        for (uint8_t i = 0; i < 8; i++)
        {
            // output sprite px if x matches
            if (ppu.sprite_x_counters[i] != 0xff && ppu.sprite_x_counters[i] <= 0 && ppu.sprite_x_counters[i] > -8) {

                // shift out pattern
                uint8_t p0 = (ppu.sprite_shift_reg0[i] & (0b10000000)) >> 7;
                uint8_t p1 = (ppu.sprite_shift_reg1[i] & (0b10000000)) >> 7;
                // p0 &= 0b1;
                // p1 &= 0b1;
                ppu.sprite_shift_reg0[i] <<= 1;
                ppu.sprite_shift_reg1[i] <<= 1;
                uint8_t pattern = p0 | (p1 << 1);

                if (pattern && !ppu.pixel_sp)
                {
                    ppu.pixel_sp = pattern;

                    uint8_t palette_id = ppu.sprite_latches[i] & 0x3;
                    ppu.sprite_prio = ((ppu.sprite_latches[i] >> 5) & 0x1);

                    // uint8_t palette_ids = emu.ppu.palette[0x11+palette_id*3];
                    static uint8_t palette_set[3];
                    palette_set[0] = emu.ppu.palette[0x11+palette_id*4];
                    palette_set[1] = emu.ppu.palette[0x12+palette_id*4];
                    palette_set[2] = emu.ppu.palette[0x13+palette_id*4];

                    ppu.color_sp = MFB_RGB(palette_id_to_red(palette_set[pattern-1]), palette_id_to_green(palette_set[pattern-1]), palette_id_to_blue(palette_set[pattern-1]));
                    if (i == 0 && ppu.sprite0_included) {

                        ppu.sprite0_included = false;

                        // ppu.sprite0_included = true;
                        if ((ppu.ppustatus & 0x40) != 0x40) {
                            ppu.check_sprite0 = true;
                        }
                    }
                    // break;
                }
                // ppu.color_sp = MFB_RGB(64*pattern, 0, 0);

                // debug
                // ppu.color_sp = 0xff;
            }
        }
    }


    if (x >= 1 && x <= 256) {
        for (uint8_t i = 0; i < 8; i++)
        {
            // decrement x counters
            if (ppu.sprite_x_counters[i] != 0xff)
                ppu.sprite_x_counters[i]--;
        }
    }

    /*
    During all visible scanlines, the PPU scans through OAM to determine which
    sprites to render on the next scanline. Sprites found to be within range
    are copied into the secondary OAM, which is then used to initialize eight
    internal sprite output units.

    OAM[n][m] below refers to the byte at offset 4*n + m within OAM,
    i.e. OAM byte m (0-3) of sprite n (0-63).

    During each pixel clock (341 total per scanline), the PPU accesses OAM
    in the following pattern:

    1. Cycles 1-64: Secondary OAM (32-byte buffer for current sprites on scanline)
       is initialized to $FF - attempting to read $2004 will return $FF.
       Internally, the clear operation is implemented by reading from the OAM and
       writing into the secondary OAM as usual, only a signal is active that
       makes the read always return $FF.
    */

    // todo - might need to only do this during the visible scanlines

    if (x <= 64) {
        memset(ppu.oam_buffer, 0xff, 32);
        // ppu.sprite0_included = false;
        ppu.next_scanline_includes_sprite0 = false;

    } else if (x >= 65 && x <= 256) {
        if (x == 65) {
            ppu.oam_read_n = 0;
            ppu.oam_read_m = 0;
            ppu.oam_buffer_counter = 0;
        }
        bool even_cycle = x % 2 == 0;

        if (even_cycle) {
            // note: this is where we would read from primary OAM
        } else {
            if (ppu.oam_read_n < 64) {

                uint8_t prim_oam_b0 = ppu.oam[ppu.oam_read_n*4+0]; // y
                uint8_t prim_oam_b1 = ppu.oam[ppu.oam_read_n*4+1]; // tile index
                uint8_t prim_oam_b2 = ppu.oam[ppu.oam_read_n*4+2]; // attributes
                uint8_t prim_oam_b3 = ppu.oam[ppu.oam_read_n*4+3]; // x

                // 1) copy y data from primary to secondary OAM
                ppu.oam_buffer[ppu.oam_buffer_counter*4] = prim_oam_b0;

                // 1a) check if y position is in range
                if (y >= prim_oam_b0 && y <= prim_oam_b0 + 7)
                {
                    // if (ppu.oam_read_n == 0) {
                    //     LOG_D("sprite zero Y: %d [tile: %d]", prim_oam_b0, prim_oam_b1);
                    // }
                    // LOG_D("prim_oam_b1 [%d]: %d", ppu.oam_read_n, prim_oam_b1 & 0b1);
                    // verify that secondary OAM isn't full!
                    if (ppu.oam_buffer_counter >= 8) {
                        // todo: overflow!
                    } else {
                        // copy rest of primary OAM bytes
                        ppu.oam_buffer[ppu.oam_buffer_counter*4+1] = prim_oam_b1;
                        ppu.oam_buffer[ppu.oam_buffer_counter*4+2] = prim_oam_b2;
                        ppu.oam_buffer[ppu.oam_buffer_counter*4+3] = prim_oam_b3;

                        ppu.oam_buffer_counter++;
                    }

                    if (ppu.oam_read_n == 0) {
                        ppu.next_scanline_includes_sprite0 = true;
                        // ppu.sprite0_included = true;
                        // if ((ppu.ppustatus & 0x40) != 0x40) {
                        //     ppu.check_sprite0 = true;
                        // }
                    }
                }

                // 2) increment n
                ppu.oam_read_n++;

                // 2a)
                if (ppu.oam_read_n >= 64) {
                    // todo: all 64 sprites evaluated, go to "4"
                } else {

                    // 2b) less than 8 sprites found, go to "1"
                    if (ppu.oam_buffer_counter < 8) {

                    // 2c) exactly 8 sprites has been found! - disable writes to secondary OAM
                    } else if (ppu.oam_buffer_counter == 8) {

                    }
                }
            }

            // 3) todo: overflow bug, I think?
        }
    /*
    2. Cycles 65-256: Sprite evaluation
        * On odd cycles, data is read from (primary) OAM
        * On even cycles, data is written to secondary OAM (unless secondary OAM
          is full, in which case it will read the value in secondary OAM instead)

        1. Starting at n = 0, read a sprite's Y-coordinate (OAM[n][0], copying it
           to the next open slot in secondary OAM (unless 8 sprites have been
           found, in which case the write is ignored).
            1a. If Y-coordinate is in range, copy remaining bytes of sprite data
                (OAM[n][1] thru OAM[n][3]) into secondary OAM.

        2. Increment n
            2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4
            2b. If less than 8 sprites have been found, go to 1
            2c. If exactly 8 sprites have been found, disable writes to secondary
                OAM because it is full. This causes sprites in back to drop out.

        3. Starting at m = 0, evaluate OAM[n][m] as a Y-coordinate.
            3a. If the value is in range, set the sprite overflow flag in $2002
                and read the next 3 entries of OAM (incrementing 'm' after each
                byte and incrementing 'n' when 'm' overflows); if m = 3, increment n
            3b. If the value is not in range, increment n *and* m (without carry).
                If n overflows to 0, go to 4; otherwise go to 3

                * The m increment is a hardware bug - if only n was incremented,
                  the overflow flag would be set whenever more than 8 sprites
                  were present on the same scanline, as expected.

        4. Attempt (and fail) to copy OAM[n][0] into the next free slot in
           secondary OAM, and increment n (repeat until HBLANK is reached)
    */

        // clear sprite fetch counter for next sprite logic
        ppu.sprite_fetch = 0;

    } else if (x >= 257 && x <= 320) {

        // clear stuff for next scanline?
        if (x == 257) {
            for (uint8_t i = 0; i < 8; i++)
            {
                ppu.sprite_x_counters[i] = 0xff;
            }
        }

        uint8_t fetch_cycle = (x - 1) % 8;

        // note: this should be spread out over the 8 cycles,
        //       but should probably be fine to do it all once here?
        if (fetch_cycle == 6) {
            // todo fetch tile info

            uint8_t sprite_data0 = ppu.oam_buffer[ppu.sprite_fetch*4+0]; // y
            uint8_t sprite_data1 = ppu.oam_buffer[ppu.sprite_fetch*4+1]; // tile
            uint8_t sprite_data2 = ppu.oam_buffer[ppu.sprite_fetch*4+2]; // attr
            uint8_t sprite_data3 = ppu.oam_buffer[ppu.sprite_fetch*4+3]; // x

            // check if sprite fetch has valid sprite
            // if (sprite_data0 != 0xff &&
            //     sprite_data1 != 0xff &&
            //     sprite_data2 != 0xff &&
            //     sprite_data3 != 0xff) {

                // fill x coord and fill sprite_x_counters
                ppu.sprite_x_counters[ppu.sprite_fetch] = (uint16_t)sprite_data3;

                // read attribs
                bool flip_x = !!(sprite_data2 & (1 << 6));
                bool flip_y = !!(sprite_data2 & (1 << 7));
                // bool prio = !!(sprite_data2 & (1 << 7));

                // fill sprite_shift_reg0 and sprite_shift_reg1 with pattern data
                uint16_t chr_offset = 0x0;
                if ((emu.ppu.ppuctrl >> 3) & 0x1) {
                    chr_offset = 0x1000;
                }

                uint8_t y_offset = (y - sprite_data0);
                if (flip_y) {
                    y_offset = 7 - y_offset;
                }
                uint8_t tile_index = sprite_data1;
                uint8_t* chr_data = (uint8_t*)emu.ppu.chr_rom+(tile_index*16)+chr_offset+y_offset;

                // clear reg
                ppu.sprite_shift_reg0[ppu.sprite_fetch] = 0x0;
                ppu.sprite_shift_reg1[ppu.sprite_fetch] = 0x0;

                // bits
                uint8_t lo = *chr_data;
                uint8_t hi = *(chr_data+8);
                if (flip_x) {
                    for (uint8_t i = 0; i < 8; i++)
                    {
                        ppu.sprite_shift_reg0[ppu.sprite_fetch] |= ((lo >> (7 - i)) & 0x1) << i;
                        ppu.sprite_shift_reg1[ppu.sprite_fetch] |= ((hi >> (7 - i)) & 0x1) << i;
                    }
                } else {
                    ppu.sprite_shift_reg0[ppu.sprite_fetch] = lo;
                    ppu.sprite_shift_reg1[ppu.sprite_fetch] = hi;
                }

                // if (ppu.sprite_fetch == 0) {
                //     LOG_D("pattern for [tile: %d]: %d %d", sprite_data1, ppu.sprite_shift_reg0[ppu.sprite_fetch], ppu.sprite_shift_reg1[ppu.sprite_fetch]);
                // }


                // todo fill sprite_latches with attribute bytes
                ppu.sprite_latches[ppu.sprite_fetch] = sprite_data2;
                // uint8_t palette_id = sprite_data2 & 0x3;

                // // uint8_t palette_ids = emu.ppu.palette[0x11+palette_id*3];
                // palette_set[0] = emu.ppu.palette[0x11+palette_id*4];
                // palette_set[1] = emu.ppu.palette[0x12+palette_id*4];
                // palette_set[2] = emu.ppu.palette[0x13+palette_id*4];
            // }

            if (x == 319) {
                ppu.check_sprite0 = false;

                ppu.sprite0_included = ppu.next_scanline_includes_sprite0;
            }

        } else if (fetch_cycle == 7) {
            ppu.sprite_fetch++;
        }

    /*
    3. Cycles 257-320: Sprite fetches (8 sprites total, 8 cycles per sprite)
        * 1-4: Read the Y-coordinate, tile number, attributes, and X-coordinate
               of the selected sprite from secondary OAM
        * 5-8: Read the X-coordinate of the selected sprite from secondary
               OAM 4 times (while the PPU fetches the sprite tile data)
        * For the first empty sprite slot, this will consist of sprite #63's
          Y-coordinate followed by 3 $FF bytes; for subsequent empty sprite
          slots, this will be four $FF bytes
    */
    }
    /*
    4. Cycles 321-340+0: Background render pipeline initialization
        * Read the first byte in secondary OAM (while the PPU fetches the
          first two background tiles for the next scanline)
    */

    return true;
}

static bool get_chr_row(nsp::ppu_t& ppu, uint16_t addr, uint8_t row, uint8_t* out_pixel_row, bool bg)
{
    if (!ppu.chr_rom) {
        return false;
    }

    uint16_t chr_offset = 0x0;
    if (bg) {
        if ((ppu.ppuctrl >> 4) & 0x1) {
            chr_offset = 0x1000;
        }
    } else {
        if ((ppu.ppuctrl >> 3) & 0x1) {
            chr_offset = 0x1000;
        }
    }

    uint8_t* chr_data = (uint8_t*)ppu.chr_rom+addr+chr_offset;

    chr_data += row;

    // low bits
    uint8_t d = *chr_data;
    for (uint32_t x = 0; x < 8; ++x)
    {
        // clear pix
        uint32_t pix_i = 7-x;
        out_pixel_row[pix_i] = 0x0;

        out_pixel_row[pix_i] = (d >> x) & 0x1;
    }

    chr_data += 8;

    // high bits
    d = *chr_data;
    for (uint32_t x = 0; x < 8; ++x)
    {
        // clear pix
        uint32_t pix_i = 7-x;

        out_pixel_row[pix_i] = out_pixel_row[pix_i] | (((d >> x) & 0x1) << 1);
    }

    return true;
}

static void check_sprite0_hit(nsp::ppu_t& ppu)
{
    ppu.ppustatus |= 0x40;
    if ((ppu.ppustatus & 0x40) == 0x40) {
        return;
    }

    // Reading the status register will clear bit 7 mentioned above and also the address latch used by PPUSCROLL and PPUADDR. It does not clear the sprite 0 hit or overflow bit.
    // Once the sprite 0 hit flag is set, it will not be cleared until the end of the next vertical blank. If attempting to use this flag for raster timing, it is important to ensure that the sprite 0 hit check happens outside of vertical blank, otherwise the CPU will "leak" through and the check will fail. The easiest way to do this is to place an earlier check for bit 6 = 0, which will wait for the pre-render scanline to begin.
    // If using sprite 0 hit to make a bottom scroll bar below a vertically scrolling or freely scrolling playfield, be careful to ensure that the tile in the playfield behind sprite 0 is opaque.
    // Sprite 0 hit is not detected at x=255, nor is it detected at x=0 through 7 if the background or sprites are hidden in this area.

    uint8_t sprite_y    = ppu.oam[0];
    uint8_t sprite_x    = ppu.oam[3];
    uint8_t sprite_tile = ppu.oam[1];

    if (ppu.x >= sprite_x && ppu.y >= sprite_y &&
        ppu.x < sprite_x+8 && ppu.y < sprite_y+8) {

        uint8_t sprite_row[8];
        get_chr_row(ppu, sprite_tile*16, sprite_y-ppu.y, sprite_row, false);

        uint8_t bg_row[8];
        uint32_t xi = ppu.x / 8;
        uint32_t yi = ppu.y / 8;
        uint32_t nti = yi*32+xi;
        uint8_t bg_tile = ppu.vram[nti];
        get_chr_row(ppu, bg_tile*16, ppu.y-(yi*8), bg_row, true);

        uint8_t sprite_compare_idx = ppu.x - sprite_x;
        uint8_t sprite_compare_pix = sprite_row[sprite_compare_idx];

        if (sprite_compare_pix > 0) {
            uint8_t bg_compare_idx = ppu.x-(xi*8);
            uint8_t bg_compare_pix = bg_row[bg_compare_idx];

            if (bg_compare_pix > 0) {
                ppu.ppustatus |= 0x40;
            }
        }
    }
}

uint16_t nsp::base_nt_addr(emu_t& emu)
{
    ppu_t& ppu = emu.ppu;
    uint8_t base_nt_r = ppu.ppuctrl & 0x3;
    switch (base_nt_r)
    {
        case 0: return 0x2000;
        case 1: return 0x2400;
        case 2: return 0x2800;
        case 3: return 0x2C00;
    }

    return 0x2000;
}

uint8_t nsp::ppu_reg_write(emu_t& emu, uint16_t addr, uint8_t data)
{
    ppu_t& ppu = emu.ppu;

    switch (addr) {
        case 0x2000:
        {
            /*
            7  bit  0
            ---- ----
            VPHB SINN
            |||| ||||
            |||| ||++- Base nametable address
            |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
            |||| |+--- VRAM address increment per CPU read/write of PPUDATA
            |||| |     (0: add 1, going across; 1: add 32, going down)
            |||| +---- Sprite pattern table address for 8x8 sprites
            ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
            |||+------ Background pattern table address (0: $0000; 1: $1000)
            ||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
            |+-------- PPU master/slave select
            |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
            +--------- Generate an NMI at the start of the
                       vertical blanking interval (0: off; 1: on)
            */
            uint8_t t = ppu.ppuctrl;

            // After power/reset, writes to this register are ignored for about 30,000 cycles.
            if (ppu.cycles / 3 < 27384) {
                return t;
            }
            ppu.ppuctrl = data;

            // pass NT bits to T
            // t: ...GH.. ........ <- d: ......GH
            //    <used elsewhere> <- d: ABCDEF..
            ppu.LoopyT.nt = data & 0b11;

            return t;
        }
        case 0x2001:
        {
            /*
            7  bit  0
            ---- ----
            BGRs bMmG
            |||| ||||
            |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
            |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
            |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
            |||| +---- 1: Show background
            |||+------ 1: Show sprites
            ||+------- Emphasize red (green on PAL/Dendy)
            |+-------- Emphasize green (red on PAL/Dendy)
            +--------- Emphasize blue
            */
            uint8_t t = ppu.ppumask.val;
            ppu.ppumask.val = data;
            return t;
        }
        case 0x2003:
        {
            uint8_t t = ppu.oamaddr;
            ppu.oamaddr = data;
            return t;
        }
        case 0x2004:
        {
            uint8_t t = ppu.oam[ppu.oamaddr];
            ppu.oam[ppu.oamaddr] = data;
            ppu.oamaddr++;
            return t;
        }
        case 0x2005:
        {
            uint8_t prev = 0x0;

            // first write - X scroll
            if (ppu.scroll_toggle == 0) {
                // t: ....... ...ABCDE <- d: ABCDE...
                // x:              FGH <- d: .....FGH
                // w:                  <- 1
                ppu.LoopyT.coarse_x = (data & 0b11111000) >> 3;
                ppu.fine_x = (data & 0b111);

                ppu.scroll_toggle = 1;
                emu.force_red = true;

            // second write - Y scroll
            } else if (ppu.scroll_toggle == 1) {
                // t: FGH..AB CDE..... <- d: ABCDEFGH
                // w:                  <- 0
                // ppu.LoopyT.coarse_y = (data & 0b11111000) << 2;
                ppu.LoopyT.coarse_y = (data & 0b11111000) >> 3;
                // LOG_D("ppu.LoopyT.coarse_y: %d [data: %d]", ppu.LoopyT.coarse_y, data);
                ppu.LoopyT.fine_y = data & 0b111;


                ppu.scroll_toggle = 0;
                emu.force_green = true;
            }


            return prev;
        }
        case 0x2006:
        {
            // first write
            if (ppu.scroll_toggle == 0) {
                // t: .CDEFGH ........ <- d: ..CDEFGH
                //        <unused>     <- d: AB......
                // t: Z...... ........ <- 0 (bit Z is cleared)
                // w:                  <- 1
                ppu.LoopyT.val &= 0b000000011111111;
                ppu.LoopyT.val |= ((data & 0b00111111) << 8);

                ppu.scroll_toggle = 1;

            // second write
            } else if (ppu.scroll_toggle == 1) {
                // t: ....... ABCDEFGH <- d: ABCDEFGH
                // v: <...all bits...> <- t: <...all bits...>
                // w:                  <- 0
                ppu.LoopyT.val &= 0b0111111100000000;
                ppu.LoopyT.val |= (data & 0b11111111);
                ppu.LoopyV.val = ppu.LoopyT.val;

                ppu.scroll_toggle = 0;
            }

            uint8_t prev = 0x0;
            return prev;
        }
        case 0x2007:
        {
            uint16_t addr = ppu.LoopyV.val;
            bool is_palette = addr >= 0x3F00 && addr <= 0x3F1F;
            ppu_write_vram(emu, addr, data);

            // check vram address increment
            if (!is_palette && (ppu.ppuctrl >> 2) & 0x1) {
                addr+=32;
            } else {
                addr++;
            }
            if (!is_palette)
            {
                addr = addr % 0x3F00;
            }
            ppu.LoopyV.val = addr;
            return 0x0;
        }
        case 0x4014:
        {
            // oam addr is 0xXX00 where XX is data
            uint16_t full_oam_source_addr = (data << 8);
            uint8_t* dma_data_ptr = dma_ptr(emu, full_oam_source_addr);
            if (dma_data_ptr == 0x0) {
                LOG_E("Invalid DMA data pointer!");
                return 0x0;
            }

            // todo take care of cycles here, somehow!
            // The CPU is suspended during the transfer, which will take 513 or 514 cycles after the $4014 write tick.
            // (1 wait state cycle while waiting for writes to complete, +1 if on an odd CPU cycle, then 256 alternating read/write cycles.)
            memcpy(&ppu.oam[ppu.oamaddr], dma_data_ptr, 256);
            break;
        }
        default:
            LOG_D("Unhandled PPU register write 0x%x to 0x%x!", data, addr);
            break;
    }
    return 0x0;
}

uint8_t nsp::ppu_write_vram(emu_t& emu, uint16_t addr, uint8_t data)
{
    ppu_t& ppu = emu.ppu;

    if (addr < 0x2000) // pattern tables
    {
        uint8_t old_data = ppu.chr_rom[addr];
        // LOG_E("WRITING to CHR ROM: 0x%04x", addr);
        ppu.chr_rom[addr] = data;
        return old_data;

    } else if (addr < 0x3F00) { // nametables
        uint16_t t_addr = addr - 0x2000;
        if (ppu.mirroring == 0) {
            if (addr < 0x2800) {
                t_addr = t_addr % 0x400;
            } else {
                t_addr = (t_addr - 0x800) % 0x400;
                t_addr = t_addr + 0x400;
            }
        }

        uint16_t wrapped_addr = t_addr % 0x800;

        uint8_t old_data = ppu.vram[wrapped_addr];
        ppu.vram[wrapped_addr] = data;
        return old_data;

    } else if (addr < 0x3FFF) { // palettes
        uint16_t wrapped_addr = (addr - 0x3F00) % 0x20;

        if (wrapped_addr == 0x10 ||
            wrapped_addr == 0x14 ||
            wrapped_addr == 0x18 ||
            wrapped_addr == 0x1C) {
            wrapped_addr = wrapped_addr - 0x10;
        }

        uint8_t old_data = ppu.palette[wrapped_addr];
        ppu.palette[wrapped_addr] = data;
        return old_data;

    } else {
        LOG_E("WRITING OUTSIDE PPU MEM: 0x%04x", addr);
        return 0x0;
    }
}

uint8_t nsp::ppu_read_vram(emu_t& emu, uint16_t addr)
{
    ppu_t& ppu = emu.ppu;

    if (addr < 0x2000) // pattern tables
    {
        return ppu.chr_rom[addr];

    } else if (addr < 0x3F00) { // nametables
        uint16_t t_addr = addr - 0x2000;
        if (ppu.mirroring == 0) {
            if (addr < 0x2800) {
                t_addr = t_addr % 0x400;
            } else {
                t_addr = (t_addr - 0x800) % 0x400;
                t_addr = t_addr + 0x400;
            }
        }
        // uint16_t wrapped_addr = (t_addr - 0x2000) % 0x800;
        uint16_t wrapped_addr = t_addr % 0x800;
        return ppu.vram[wrapped_addr];

    } else if (addr < 0x3FFF) { // palettes
        uint16_t wrapped_addr = (addr - 0x3F00) % 0x20;

        if (wrapped_addr == 0x10 ||
            wrapped_addr == 0x14 ||
            wrapped_addr == 0x18 ||
            wrapped_addr == 0x1C) {
            wrapped_addr = wrapped_addr - 0x10;
        }
        return ppu.palette[wrapped_addr];

    } else {
        LOG_E("READING OUTSIDE PPU MEM: 0x%04x", addr);
        return 0x0;
    }
}

uint8_t nsp::ppu_reg_read(emu_t &emu, uint16_t addr, bool peek)
{
    ppu_t& ppu = emu.ppu;

    switch (addr) {
        case 0x2000:
        {
            return ppu.ppuctrl;
        }
        case 0x2001:
        {
            return ppu.ppumask.val;
        }
        case 0x2002:
        {
            /*
            7  bit  0
            ---- ----
            VSO. ....
            |||| ||||
            |||+-++++- Least significant bits previously written into a PPU register
            |||        (due to register not being updated for this address)
            ||+------- Sprite overflow. The intent was for this flag to be set
            ||         whenever more than eight sprites appear on a scanline, but a
            ||         hardware bug causes the actual behavior to be more complicated
            ||         and generate false positives as well as false negatives; see
            ||         PPU sprite evaluation. This flag is set during sprite
            ||         evaluation and cleared at dot 1 (the second dot) of the
            ||         pre-render line.
            |+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
            |          a nonzero background pixel; cleared at dot 1 of the pre-render
            |          line.  Used for raster timing.
            +--------- Vertical blank has started (0: not in vblank; 1: in vblank).
                       Set at dot 1 of line 241 (the line *after* the post-render
                       line); cleared after reading $2002 and at dot 1 of the
                       pre-render line.
            */
            uint8_t ppustatus_cpy = ppu.ppustatus;

            if (!peek) {
                ppu.ppustatus &= ~0x80; // clear vblank status bit if register is being read
                // ppu.LoopyV = 0x0;       // PPUADDR is also zeroed
                emu.force_blue = true;

                ppu.scroll_toggle = 0; // clear scroll toggle on PPUSTATUS read
            }

            return ppustatus_cpy;
        }
        case 0x2003:
        {
            return ppu.oamaddr;
        }
        case 0x2004:
        {
            return ppu.oam[ppu.oamaddr];
        }
        case 0x2007:
        {
            uint16_t addr = ppu.LoopyV.val;
            bool is_palette = addr >= 0x3F00 && addr <= 0x3F1F;
            uint8_t prev_data = ppu.read_buffer;
            uint8_t read_data = ppu_read_vram(emu, addr);

            // check vram address increment
            if (!is_palette && (ppu.ppuctrl >> 2) & 0x1) {
                addr+=32;
            } else {
                addr++;
            }
            if (!is_palette)
            {
                addr = addr % 0x3F00;
            }
            ppu.LoopyV.val = addr;

            ppu.read_buffer = read_data;
            return prev_data;
        }
        default:
            LOG_D("Unhandled PPU register read at 0x%x!", addr);
            break;
    }

    return 0x0;
}

