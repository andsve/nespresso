#ifndef NSP_H
#define NSP_H

#include <stdlib.h>
#include <stdint.h>

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

    RESULT load_rom_file(const char* filepath, ines_rom_t& rom);
    RESULT load_rom_mem(const uint8_t* data, long int size, ines_rom_t& rom);
}

#endif /* NSP_H */
