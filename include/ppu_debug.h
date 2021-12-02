#ifndef PPU_DEBUG_H
#define PPU_DEBUG_H

#include "nsp.h"

void dump_ppu_vram(nsp::emu_t& emu);
void dump_ppu_sprites(nsp::emu_t& emu);

// debug functions for generating images for slides
void dump_ppu_nametable_ids(nsp::emu_t& emu);
void dump_chr_rom(nsp::emu_t& emu);
void dump_chr_rom_debug(nsp::emu_t& emu);
void dump_ppu_nametable_debug(nsp::emu_t& emu);
void dump_ppu_sprites_debug(nsp::emu_t& emu);
void dump_chr_bitfields(nsp::emu_t& emu, uint8_t chr_i);

#endif /* PPU_DEBUG_H */
