#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "nsp.h"
#include "nsp_mappers.h"
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

    bool has_prg_ram = ((data[6] >> 1) & 0b1) == 0b1;
    LOG_D("Has PRG RAM: %d", has_prg_ram);

    // Get most relevant data from header byte 6 and 7
    rom.mirroring = data[6] & 0x1;
    LOG_D("Mirroring: %d", rom.mirroring);
    uint8_t mapper_id = (data[7] & 0xF0) | ((data[6] & 0xF0) >> 4);
    bool ines_v2 = (data[7] & (0x3 << 2)) == 0x08;

    // Get PRG RAM size
    rom.ram_size = data[8];
    // uint8_t ram_available = (0b1) & (data[10] >> 4);
    LOG_D("RAM size: %d", rom.ram_size);

    if (ines_v2) {
        LOG_E("No support for iNES v2!");
        return RESULT_ERROR;
    }

    if (mapper_id > MAPPERS_COUNT) {
        LOG_E("Mapper %d not supported!", mapper_id);
        return RESULT_ERROR;
    }

    rom.mapper = MAPPERS_LUT[mapper_id];

    LOG_D("Mapper: %d", rom.mapper->mapper_id);
    rom.mapper->load_ines_rom(rom, &data[16]);


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

