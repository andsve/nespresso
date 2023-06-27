#include "nsp_mappers.h"
#include "nsp_log.h"

#include <string.h>

void nsp::mapper_t::load_ines_rom(ines_rom_t& rom, const uint8_t* rom_data) {
    static const uint32_t prg_page_size = 16 * 1024;
    static const uint32_t chr_page_size = 8 * 1024;

    prg_page_count = rom.prg_page_count;
    chr_page_count = rom.chr_page_count;

    // Allocate memory to store PRG and CHR data from end of file
    prg_pages = new uint8_t*[prg_page_count];
    chr_pages = new uint8_t*[chr_page_count > 0 ? chr_page_count : 1];

    // Copy PRG data
    const uint8_t* data_ptr = rom_data;
    for (int i = 0; i < prg_page_count; ++i)
    {
        prg_pages[i] = new uint8_t[prg_page_size];
        memcpy(prg_pages[i], data_ptr, prg_page_size);
        data_ptr += prg_page_size;
    }

    // Copy CHR data
    for (int i = 0; i < chr_page_count; ++i)
    {
        chr_pages[i] = new uint8_t[chr_page_size];
        memcpy(chr_pages[i], data_ptr, chr_page_size);
        data_ptr += chr_page_size;
    }

    if (chr_page_count == 0) {
        LOG_D("No CHR pages - creating 8k empty");
        chr_pages[0] = new uint8_t[8 * 1024];
    }
}

uint8_t* nsp::mapper_t::get_initial_lower_prg()
{
    return prg_pages[0];
}

uint8_t* nsp::mapper_t::get_initial_upper_prg()
{
    return prg_pages[prg_page_count > 1 ? 1 : 0];
}

uint8_t* nsp::mapper_t::get_initial_chr()
{
    return chr_pages[0];
}


struct nsp::mapper_001_t : nsp::mapper_t
{
    mapper_001_t() {
        mapper_id = 1;
    };
};


struct nsp::mapper_002_t : nsp::mapper_t
{
    mapper_002_t() {
        mapper_id = 2;
    };

    void load_ines_rom(nsp::ines_rom_t& rom, const uint8_t* rom_data) override
    {
        mapper_t::load_ines_rom(rom, rom_data);
    }

    uint8_t* get_initial_lower_prg() override
    {
        return prg_pages[prg_page_count-2];
    }

    uint8_t* get_initial_upper_prg() override
    {
        return prg_pages[prg_page_count-1];
    }

    uint8_t handle_mem_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled) override {
        /*
            Bank select ($8000-$FFFF)

            7  bit  0
            ---- ----
            xxxx pPPP
                 ||||
                 ++++- Select 16 KB PRG ROM bank for CPU $8000-$BFFF
                      (UNROM uses bits 2-0; UOROM uses bits 3-0)
        */
        if (addr >= 0x8000 && addr <= 0xFFFF) {
            *handled = true;

            uint8_t bank = 0b00001111 & data;
            emu.cpu.prgrom_lower = prg_pages[bank];

            return 0;
        }
        return 0;
    };
    // uint8_t handle_mem_read(emu_t &emu, uint16_t addr, bool *handled, bool peek) { return 0; };
};

nsp::mapper_t nsp::MAPPER_IMPL_default;
static nsp::mapper_001_t MAPPER_IMPL_001;
static nsp::mapper_002_t MAPPER_IMPL_002;

nsp::mapper_t* nsp::MAPPERS_LUT[MAPPERS_COUNT] = {
    &MAPPER_IMPL_default,
    &MAPPER_IMPL_001,
    &MAPPER_IMPL_002,
};
