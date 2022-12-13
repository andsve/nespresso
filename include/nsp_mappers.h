#ifndef NSP_MAPPERS_H
#define NSP_MAPPERS_H

#include "nsp.h"
#include "nsp_log.h"

namespace nsp
{

// https://www.nesdev.org/wiki/NROM
struct mapper_000_t : nsp::mapper_t
{
    mapper_000_t() {
        mapper_id = 0;
        LOG_D("Using mapper 0");
    }

    RESULT map_initial_prg(ines_rom_t& ines_rom, emu_t& emu) override;
};

// https://www.nesdev.org/wiki/MMC1
struct mapper_001_t : nsp::mapper_t
{
    mapper_001_t() {
        mapper_id = 1;
        LOG_D("Using mapper 1");
    }

    RESULT map_initial_prg(ines_rom_t& ines_rom, emu_t& emu) override;
};

}

#endif // NSP_MAPPERS_H
