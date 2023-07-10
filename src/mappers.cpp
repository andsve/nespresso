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
    // uint8_t shift_reg;
    // uint8
    uint8_t MMC1_SR;
    uint8_t MMC1_PB;

    uint8_t MMC1_control;
    uint8_t mirroring;
    uint8_t prg_rom_bank_mode;
    uint8_t chr_rom_bank_mode;

    uint8_t MMC1_chr1;
    uint8_t MMC1_chr2;
    uint8_t MMC1_prg;

    mapper_001_t() {
        mapper_id = 1;
        // shift_reg = 0x0;
        MMC1_SR = 0b00010000;
        MMC1_PB = 0x00;
    };

    uint8_t* get_initial_lower_prg() override
    {
        return prg_pages[0];
    }

    uint8_t* get_initial_upper_prg() override
    {
        return prg_pages[prg_page_count-1];
    }

    uint8_t handle_mem_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled) override {
        /*
            Bank select ($8000-$FFFF)

            Unlike almost all other mappers, the MMC1 is configured through a serial
            port in order to reduce its pin count. CPU $8000-$FFFF is connected to a
            common shift register. Writing a value with bit 7 set ($80 through $FF)
            to any address in $8000-$FFFF clears the shift register to its initial
            state.

            To change a register's value, the CPU writes five times with bit
            7 clear and one bit of the desired value in bit 0 (starting with the low
            bit of the value).

            * On the first four writes, the MMC1 shifts bit 0 into a shift register.
            * On the fifth write, the MMC1 copies bit 0 and the shift register contents
              into an internal register selected by bits 14 and 13 of the address,
              and then it clears the shift register. Only on the fifth write does the
              address matter, and even then, only bits 14 and 13 of the address matter
              because the mapper doesn't see the lower address bits (similar to the
              mirroring seen with PPU registers).

            After the fifth write, the shift register is cleared automatically,
            so writing again with bit 7 set to clear the shift register is not needed.

        */
        if (addr >= 0x8000 && addr <= 0xFFFF) {
            *handled = true;

            bool bit7 = (0b10000000 & data) == 0b10000000;
            // bool bit0 = (0x01 & data) == 0x01;

            if (bit7) {
                // shift_reg = 0x0;
                LOG_D("Resetting MMC1_SR!");
                MMC1_SR = 0b00010000;
            } else {
                // shift_reg = (shift_reg << 1) | (0x1 & data);
                uint8_t bit0_MMC1_SR = (0b1) & MMC1_SR;

                // LOG_D("Shifting MMC1_SR: %d", bit0);

                // shift
                MMC1_SR = (MMC1_SR >> 1) | ((0b1 & data) << 4);

                if (bit0_MMC1_SR == 0b1) {
                    MMC1_PB = MMC1_SR;
                    MMC1_SR = 0b00010000;

                    // LOG_D("MMC1 - 5th write: 0x%X [PB: %d (%d)", addr, MMC1_PB, MMC1_PB & 0b1111);

                    if (addr >= 0x8000 && addr <= 0x9FFF) {
                        /*
                            Control (internal, $8000-$9FFF)

                            4bit0
                            -----
                            CPPMM
                            |||||
                            |||++- Mirroring (0: one-screen, lower bank; 1: one-screen, upper bank;
                            |||               2: vertical; 3: horizontal)
                            |++--- PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
                            |                         2: fix first bank at $8000 and switch 16 KB bank at $C000;
                            |                         3: fix last bank at $C000 and switch 16 KB bank at $8000)
                            +----- CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
                        */
                        MMC1_control = MMC1_PB;
                        mirroring = MMC1_control & 0b11;
                        prg_rom_bank_mode = (MMC1_control >> 2) & 0b11;
                        chr_rom_bank_mode = (MMC1_control >> 4) & 0b1;

                        LOG_D("MMC1 - Control reg update");
                        LOG_D("MMC1 - Mirroring: %d", mirroring);
                        LOG_D("MMC1 - PRG Bank Mode: %d", prg_rom_bank_mode);
                        LOG_D("MMC1 - CHR Bank Mode: %d", chr_rom_bank_mode);

                        switch (prg_rom_bank_mode) {
                            case 2:
                                // 2: fix first bank at $8000 and switch 16 KB bank at $C000;
                                emu.cpu.prgrom_lower = prg_pages[0];
                                break;
                            case 3:
                                // 3: fix last bank at $C000 and switch 16 KB bank at $8000)
                                emu.cpu.prgrom_upper = prg_pages[prg_page_count-1];
                                break;
                            case 0:
                            case 1:
                            default:
                                // 0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
                                LOG_E("NOT IMPL - MMC1 - PRG bank switching 0, 1");
                                break;
                        }
                        // LOG_E("NOT IMPL - MMC1 Control");

                    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
                        /*
                            CHR bank 0 (internal, $A000-$BFFF)

                            4bit0
                            -----
                            CCCCC
                            |||||
                            +++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)

                            MMC1 can do CHR banking in 4KB chunks. Known carts with CHR RAM have 8 KiB, so that makes 2 banks.
                            RAM vs ROM doesn't make any difference for address lines. For carts with 8 KiB of CHR (be it ROM or RAM),
                            MMC1 follows the common behavior of using only the low-order bits: the bank number is in effect ANDed with 1.
                        */
                        uint8_t bank_select = MMC1_PB;
                        if (chr_rom_bank_mode == 0) {
                            LOG_D("MMC1 - CHR bank 0 switch: %d", bank_select);
                            emu.ppu.chr_rom = chr_pages[bank_select];
                        } else {
                            LOG_E("NOT IMPL - CHR bank 0 switching: %d [%d] [chr_rom_bank_mode: %d]", bank_select, MMC1_PB, chr_rom_bank_mode);
                        }

                    } else if (addr >= 0xC000 && addr <= 0xDFFF) {
                        /*
                            CHR bank 1 (internal, $C000-$DFFF)

                            4bit0
                            -----
                            CCCCC
                            |||||
                            +++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
                        */
                        LOG_E("NOT IMPL - CHR bank 1 switching: %d [%d]", data, MMC1_PB);

                    } else if (addr >= 0xE000 && addr <= 0xFFFF) {
                        /*
                            PRG bank (internal, $E000-$FFFF)

                            4bit0
                            -----
                            RPPPP
                            |||||
                            |++++- Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
                            +----- MMC1B and later: PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
                                   MMC1A: Bit 3 bypasses fixed bank logic in 16K mode (0: affected; 1: bypassed)
                        */
                        uint8_t bank_select = 0b1111 & MMC1_PB;
                        bool mmc1a_b = (0b10000 & MMC1_PB) == 0b10000;
                        // todo(andsve): figure out if we are MMC1A or MMC1B??
                        if (mmc1a_b) {
                            // PGR RAM chip enable!
                            LOG_D("MMC1B - PRG RAM chip enabled!");
                        }

                        // LOG_D("MMC1 - PRG switched to: %d", bank_select);
                        if (bank_select > prg_page_count-1) {
                            LOG_E("MMC1 - PRG bank select bigger than available! [available: %d, setting: %d]", prg_page_count, bank_select);
                        }

                        if (prg_rom_bank_mode == 3) {
                            LOG_D("MMC1 - PRG bank switch: %d", bank_select);
                            emu.cpu.prgrom_lower = prg_pages[bank_select];
                        } else if (prg_rom_bank_mode == 2) {
                            LOG_D("MMC1 - PRG bank switch: %d", bank_select);
                            emu.cpu.prgrom_upper = prg_pages[bank_select];
                        } else {
                            LOG_E("NOT IMPL - PRG bank switching mode not supported: %d", prg_rom_bank_mode);
                        }

                    }

                }

            }

        //     uint8_t bank = 0b00001111 & data;
        //     emu.cpu.prgrom_lower = prg_pages[bank];

            return 0;
        }
        return 0;
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

struct nsp::mapper_003_t : nsp::mapper_t
{
    mapper_003_t() {
        mapper_id = 3;
    };


    uint8_t handle_mem_write(emu_t &emu, uint16_t addr, uint16_t data, bool *handled) override {
        /*
            Bank select ($8000-$FFFF)

            7  bit  0
            ---- ----
            cccc ccCC
            |||| ||||
            ++++-++++- Select 8 KB CHR ROM bank for PPU $0000-$1FFF
        */
        if (addr >= 0x8000 && addr <= 0xFFFF) {
            *handled = true;

            uint8_t bank = 0b00000011 & data;
            emu.ppu.chr_rom = chr_pages[bank];

            return 0;
        }
        return 0;
    };
};

nsp::mapper_t nsp::MAPPER_IMPL_default;
static nsp::mapper_001_t MAPPER_IMPL_001;
static nsp::mapper_002_t MAPPER_IMPL_002;
static nsp::mapper_003_t MAPPER_IMPL_003;

nsp::mapper_t* nsp::MAPPERS_LUT[MAPPERS_COUNT] = {
    &MAPPER_IMPL_default,
    &MAPPER_IMPL_001,
    &MAPPER_IMPL_002,
    &MAPPER_IMPL_003,
};
