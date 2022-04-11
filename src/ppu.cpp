#include <string.h>

#include "nsp.h"
#include "nsp_log.h"

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

        check_sprite0_hit(ppu);

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

bool nsp::ppu_raster(emu_t& emu) {
    ppu_t& ppu = emu.ppu;

    if (!ppu.ppumask.show_bg) {
        // LOG_D("not rendering bg");
        return false;
    }

    uint16_t& x = ppu.x;
    uint16_t& y = ppu.y;

    uint16_t render_x = x - 1;
    uint16_t render_y = y;
    if (render_x < NES_WIDTH && render_y < NES_HEIGHT) {
        uint16_t pixel_index = render_y*NES_WIDTH+render_x;

        uint8_t pixel = 0x0;

        uint8_t lo = ppu.pattern_lo.hi;
        uint8_t hi = ppu.pattern_hi.hi;

        pixel = (((lo >> (7-ppu.fine_x)) & 0x1) |
                 (((hi >> (7-ppu.fine_x)) & 0x1) << 1));

        pixel = pixel * (255/3);

        ppu.screen[pixel_index] = MFB_RGB(pixel, pixel, pixel);

        // if (emu.force_red || emu.force_green || emu.force_blue) {
        //     ppu.screen[pixel_index] = MFB_RGB(emu.force_red * 255, emu.force_green * 255, emu.force_blue * 255);
        //     if (emu.force_red)
        //         emu.force_red = false;
        //     if (emu.force_green)
        //         emu.force_green = false;
        //     if (emu.force_blue)
        //         emu.force_blue = false;
        // }
        // ppu.screen[pixel_index] = MFB_RGB(40, 0, 0);
    }

    if (((x >= 1 && x <= 256) ||
         (x >= 321 && x <= 336)) &&
        (y < 240 || y == 261)) // visible frames and pre render line
    {
        ppu.pattern_lo.val = ppu.pattern_lo.val << 1;
        ppu.pattern_hi.val = ppu.pattern_hi.val << 1;
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
            } else if (fetch_tick == 1) {
                // NT byte
                // ppu.nt_data_next = ppu_read_vram(emu, ppu.LoopyV);
                uint16_t nt_addr = 0x2000 | (ppu.LoopyV.val & 0x0FFF);
                // LOG_D("nt_addr: %x", nt_addr);
                ppu.nt_tile = ppu_read_vram(emu, nt_addr);

            } else if (fetch_tick == 3) {
                // AT byte

            } else if (fetch_tick == 5) {
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
                ppu.LoopyV.val ^= 0x10;                 // switch vertical nametable
              } else if (ty == 31) {
                ty = 0;                                 // coarse Y = 0, nametable not switched
              } else {
                ty += 1;                                // increment coarse Y
              }
              ppu.LoopyV.coarse_y = ty; // put coarse Y back into v
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
                ppu.LoopyT.coarse_y = (data & 0b11111000) << 2;
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
        LOG_E("WRITING to CHR ROM: 0x%04x", addr);
        // ppu.chr_rom[addr] = data;
        return old_data;

    } else if (addr < 0x3F00) { // nametables
        uint16_t wrapped_addr = (addr - 0x2000) % 0x800;

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
        uint16_t wrapped_addr = (addr - 0x2000) % 0x800;
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

