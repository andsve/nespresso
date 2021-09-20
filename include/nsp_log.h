#ifndef NSP_LOG_H
#define NSP_LOG_H

namespace nsp
{
    enum LOG_LEVEL
    {
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_ERROR,
    };

    typedef void (*cb_log_t)(LOG_LEVEL level, const char* log);
    extern cb_log_t cb_log;

    void _log(nsp::LOG_LEVEL level, const char* buffer, ...);
#ifdef DEBUG
    #define LOG_D(...) nsp::_log(nsp::LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
    #define LOG_D(...)
#endif
#define LOG_I(...) nsp::_log(nsp::LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_W(...) nsp::_log(nsp::LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_E(...) nsp::_log(nsp::LOG_LEVEL_ERROR, __VA_ARGS__)
}

#endif /* NSP_LOG_H */
