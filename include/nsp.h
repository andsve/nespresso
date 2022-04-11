#ifndef NSP_H
#define NSP_H

#include <stdlib.h>
#include <stdint.h>

#define NES_WIDTH 256
#define NES_HEIGHT 240

namespace nsp
{
    enum RESULT
    {
        RESULT_ERROR = 0,
        RESULT_OK = 1,
    };

    struct ines_rom_t
    {
        uint8_t prg_page_count;
        uint8_t chr_page_count;

        uint8_t** prg_pages;
        uint8_t** chr_pages;
    };

    struct cpu_t
    {
        // Registers
        struct regs_t
        {
            union {
                struct __attribute__((packed)) {
                    /*
                    7  bit  0
                    ---- ----
                    NVss DIZC
                    |||| ||||
                    |||| |||+- Carry
                    |||| ||+-- Zero
                    |||| |+--- Interrupt Disable
                    |||| +---- Decimal
                    ||++------ No CPU effect, see: the B flag
                    |+-------- Overflow
                    +--------- Negative
                    */
                    uint8_t C : 1;
                    uint8_t Z : 1;
                    uint8_t I : 1;
                    uint8_t D : 1;
                    uint8_t B : 2;
                    uint8_t O : 1;
                    uint8_t N : 1;
                };
                uint8_t P;
            };
            uint8_t  A, X, Y;
            uint16_t PC;
            uint8_t  S;
        } regs;

        struct vectors_t
        {
            uint16_t NMI;
            uint16_t RESET;
            uint16_t IRQBRK;
        } vectors;


        // RAM and Stack
        uint8_t ram[0x700];
        uint8_t stack[0x100];

        // Currently mapped prg rom banks
        uint8_t* prgrom_lower;
        uint8_t* prgrom_upper;

        uint8_t _4015;

        // Total cycles performed
        uint32_t cycles;

        // Per instruction
        uint8_t page_wraps;
        uint8_t extra_cycles;
    };

    union loopy_reg_t {
        struct __attribute__((packed)) {
            // ? yyy NN YYYYY XXXXX
            // | ||| || ||||| +++++-- coarse X scroll
            // | ||| || +++++-------- coarse Y scroll
            // | ||| ++-------------- nametable select
            // | +++----------------- fine Y scroll
            // +--------------------- unused?
            uint16_t coarse_x : 5;
            uint16_t coarse_y : 5;
            uint16_t nt : 2;
            uint16_t fine_y : 3;
            uint16_t unused : 1;
        };
        uint16_t val;
    };

    union uint16_split_t {
        struct __attribute__((packed)) {
            uint8_t lo;
            uint8_t hi;
        };
        uint16_t val;
    };

    struct ppu_t
    {
        // most important registers to get up and running
        uint8_t ppuctrl;
        // uint8_t ppumask;
        union {
            struct __attribute__((packed)) {
                // BGRs bMmG
                // |||| ||||
                // |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
                // |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
                // |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
                // |||| +---- 1: Show background
                // |||+------ 1: Show sprites
                // ||+------- Emphasize red (green on PAL/Dendy)
                // |+-------- Emphasize green (red on PAL/Dendy)
                // +--------- Emphasize blue
                uint8_t grayscale : 1;
                uint8_t show_bg_left : 1;
                uint8_t show_sprites_left : 1;
                uint8_t show_bg : 1;
                uint8_t show_sprites : 1;
                uint8_t emph_R : 1;
                uint8_t emph_G : 1;
                uint8_t emph_B : 1;
            };
            uint8_t val;
        } ppumask;

        uint8_t ppustatus;
        uint8_t oamaddr;

        // ppu addr and scroll
        loopy_reg_t LoopyV;
        loopy_reg_t LoopyT;
        uint8_t fine_x;
        uint8_t scroll_toggle;

        // ppu<->cpu bus buffer
        uint8_t read_buffer;

        // rendering pointers/scanline
        uint16_t x, y;
        uint8_t nt_tile;
        uint8_t at_byte;
        uint16_split_t at;
        uint8_t at_latch;
        uint16_t pattern_latch;
        uint16_split_t pattern_lo;
        uint16_split_t pattern_hi;

        uint8_t pixel_bg;
        uint8_t pixel_sp;
        uint32_t color_bg;
        uint32_t color_sp;
        uint32_t screen[NES_WIDTH * NES_HEIGHT * 4];

        // VRAM
        uint8_t vram[0x800]; // 2kb vram
        uint8_t palette[0xFF];
        uint8_t oam[64*4];

        // Mapped CHR ROM
        uint8_t* chr_rom;

        // Total cycles performed
        uint32_t cycles;
    };

    struct emu_t
    {
        cpu_t cpu;
        ppu_t ppu;
        bool waiting_for_vblank;
        bool force_red;
        bool force_green;
        bool force_blue;
        // apu?
    };

    RESULT load_rom_file(const char* filepath, ines_rom_t& rom);
    RESULT load_rom_mem(const uint8_t* data, long int size, ines_rom_t& rom);

    RESULT init_emu(emu_t& emu, ines_rom_t& ines_rom);
    RESULT step_emu(emu_t& emu, uint32_t cycles);
    RESULT step_emu_until_frame_done(emu_t& emu);
    uint32_t step_cpu(emu_t& emu, uint32_t cycles);
    uint32_t step_ppu(emu_t& emu, uint32_t cycles);

    uint8_t memory_read(emu_t& emu, uint16_t addr, bool peek = false);
    uint16_t memory_read_short(emu_t& emu, uint16_t addr);
    uint16_t memory_read_short_zp_wrap(emu_t& emu, uint8_t addr);
    uint8_t memory_read_from_addr_ptr(emu_t &emu, uint16_t* addr_ptr);

    uint8_t memory_write(emu_t& emu, uint16_t addr, uint8_t data);

    void stack_push(emu_t& emu, uint8_t data);
    uint8_t stack_pop(emu_t& emu);
    void stack_push_short(emu_t& emu, uint16_t data);
    uint16_t stack_pop_short(emu_t& emu);

    void check_page_boundary(emu_t& emu, uint16_t addr1, uint16_t addr2);
    void check_page_boundary_index(emu_t& emu, uint16_t base, uint8_t index);

    uint8_t handle_memmap_reg_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled);
    uint8_t ppu_reg_write(emu_t& emu, uint16_t addr, uint8_t data);
    uint8_t ppu_write_vram(emu_t& emu, uint16_t addr, uint8_t data);
    uint8_t ppu_read_vram(emu_t& emu, uint16_t addr);
    bool ppu_raster(emu_t& emu);
    bool ppu_bg_pipeline(emu_t& emu);
    uint8_t* dma_ptr(emu_t &emu, uint16_t addr);

    uint8_t handle_memmap_reg_read(emu_t &emu, uint16_t addr, bool *handled, bool peek);
    uint8_t ppu_reg_read(emu_t& emu, uint16_t addr, bool peek);

    extern uint32_t window_buffer[];
}

#endif /* NSP_H */
