#include <cstdio>
#include <stdarg.h>

#include "nsp.h"
#include "nsp_log.h"

namespace nsp
{
    cb_log_t cb_log = 0x0;
}

static void default_log_to_console(nsp::LOG_LEVEL level, const char* log)
{
    switch (level)
    {
        case nsp::LOG_LEVEL_ERROR:
            printf("\x1B[0;30;41m[ERROR]\x1B[0;;m %s\n", log);
        break;
        case nsp::LOG_LEVEL_WARNING:
            printf("\x1B[0;30;43m[WARN]\x1B[0;;m %s\n", log);
        break;
        case nsp::LOG_LEVEL_DEBUG:
            printf("\x1B[0;33;m[DEBUG]\x1B[0;;m %s\n", log);
        break;
        default:
            printf("%s\n", log);
        break;
    }
}

void nsp::_log(nsp::LOG_LEVEL level, const char* buffer, ...)
{
    static char _buffer[2048];
    static va_list va;

    if (!nsp::cb_log)
    {
        nsp::cb_log = default_log_to_console;
    }

    va_start(va, buffer);
    vsnprintf(_buffer, 2048, buffer, va);
    va_end(va);

    nsp::cb_log(level, _buffer);
}
