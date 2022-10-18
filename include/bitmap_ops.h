#ifndef BITMAP_OPS_H
#define BITMAP_OPS_H

#include <stdarg.h>

#include "nsp.h"

void clear_window_buffer(uint8_t r, uint8_t g, uint8_t b);
void clear_nt_window_buffer(uint8_t r, uint8_t g, uint8_t b);
void rect_nt_window_buffer(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b);
void dimm_window_buffer(float dimm);
void fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);
void draw_glyph(uint32_t x, uint32_t y, uint8_t glyph);
void draw_text(uint32_t x, uint32_t y, const char* text, ...);
void draw_glyph_nt(uint32_t x, uint32_t y, uint8_t glyph);
void draw_text_nt(uint32_t x, uint32_t y, const char* text, ...);

#endif /* BITMAP_OPS_H */
