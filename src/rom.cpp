#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "nsp.h"
#include "nsp_log.h"

// NesDev iNES: https://wiki.nesdev.com/w/index.php/INES
//          v2: https://wiki.nesdev.com/w/index.php/NES_2.0
/*

iNES File Structure, simplified a bit..

           Offset (B)   Size (B)      Meaning
         +---------------------------------------+
         |         0 |        16 |        Header |
         +---------------------------------------+
         |        16 | 16384 * X |  PRG ROM data |
         +---------------------------------------+
         |         Z |  8192 * Y |  CHR ROM data |
         +---------------------------------------+

Details:

-> Header, 16 bytes

           Offset (B)                              Meaning
         +-------------------------------------------------+
         |       0-3 |     iNES file identification string |
         +-------------------------------------------------+
         |         4 |             PRG-ROM size/page count |
         +-------------------------------------------------+
         |         5 |             CHR-ROM size/page count |
         +-------------------------------------------------+
         |         6 |                            Flags 6: |
         |             Mapper, mirroring, battery, trainer |
         +-------------------------------------------------+
         |         7 |                            Flags 7: |
         |                Mapper, VS/Playchoice, iNES 2.0? |
         +-------------------------------------------------+
         |         8 |                            Flags 8: |
         |                                    PRG-RAM size |
         +-------------------------------------------------+
         |         9 |                            Flags 9: |
         |                                       TV system |
         +-------------------------------------------------+
         |     10-15 |        Unused padding (for iNES v1) |
         +-------------------------------------------------+

-> Trainer, if present, 0 or 512 bytes
-> PRG-ROM data, 16384 * (PRG-ROM size from header above)
-> CHR-ROM data, 8192 * (CHR-ROM size from header above)

*/

 // "NES" followed by MS-DOS end-of-file
static const char _ines_magic[4] = {'N', 'E', 'S', 0x1A};
static const uint32_t prg_page_size = 16 * 1024;
static const uint32_t chr_page_size = 8 * 1024;

nsp::RESULT nsp::load_rom_mem(const uint8_t* data, long int size, ines_rom_t& rom)
{
    // Verify iNES header
    if (strncmp((const char*)data, _ines_magic, 4) != 0)
    {
        LOG_E("Invalid iNES file header, magic does not match.");
        return RESULT_ERROR;
    }

    // Get PRG and CHR sizes from header bytes 4 and 5
    rom.prg_page_count = data[4];
    rom.chr_page_count = data[5];

    // Get most relevant data from header byte 6 and 7
    uint8_t mapper_id = (data[6] & 0xF0) >> 4;
    mapper_id = (data[7] & (0xF0 << 4)) | mapper_id;
    bool ines_v2 = (data[7] & (0x3 << 2)) == 0x08;

    if (ines_v2) {
        LOG_E("No support for iNES v2!");
        return RESULT_ERROR;
    }

    if (mapper_id != 0) {
        LOG_E("Only mapper 0 supported!");
        return RESULT_ERROR;
    }

    // Allocate memory to store PRG and CHR data from end of file
    rom.prg_pages = new uint8_t*[rom.prg_page_count];
    rom.chr_pages = new uint8_t*[rom.chr_page_count];

    // Copy PRG data
    const uint8_t* data_ptr = &data[16];
    for (int i = 0; i < rom.prg_page_count; ++i)
    {
        rom.prg_pages[i] = new uint8_t[prg_page_size];
        memcpy(rom.prg_pages[i], data_ptr, prg_page_size);
        data_ptr += prg_page_size;
    }

    // Copy CHR data
    for (int i = 0; i < rom.chr_page_count; ++i)
    {
        rom.chr_pages[i] = new uint8_t[chr_page_size];
        memcpy(rom.chr_pages[i], data_ptr, chr_page_size);
        data_ptr += chr_page_size;
    }

    return RESULT_OK;
}

nsp::RESULT nsp::load_rom_file(const char* filepath, ines_rom_t& rom)
{
    LOG_D("Loading iNES ROM '%s'", filepath);

    FILE* fp = fopen(filepath, "rb");
    long int file_size;

    if (fp == 0x0) {
        LOG_E("Could not open iNES ROM file '%s', reason: %s", filepath, strerror(errno));
        return RESULT_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);
    LOG_D("iNES ROM size: %ld bytes.", file_size);

    uint8_t* data = (uint8_t*)malloc(file_size * sizeof(uint8_t));
    fread(data, sizeof(uint8_t), file_size, fp);
    fclose(fp);

    RESULT res = load_rom_mem(data, file_size, rom);

    free(data);

    return res;
}

