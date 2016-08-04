/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_logger_h
#define ptpip_initiator_logger_h

#include <stdint.h>

enum {
    // Log level
    PTPIP_LOGLEVEL_VERBOSE          = 0,
    PTPIP_LOGLEVEL_DEBUG,
    PTPIP_LOGLEVEL_INFO,
    PTPIP_LOGLEVEL_WARN,
    PTPIP_LOGLEVEL_ERROR,
    PTPIP_LOGLEVEL_FATAL,
};

#ifdef __cplusplus

#include <stdio.h>
#include <string>
#include <stdarg.h>

#define LOG_VERBOSE(...)    g_Logger.verbose(__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_DEBUG(...)      g_Logger.debug(__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_INFO(...)       g_Logger.info (__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_WARN(...)       g_Logger.warn (__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_ERROR(...)      g_Logger.error(__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_ERROR_WITH_ERRNO(e, ...)   g_Logger.error(__FILE__, __LINE__, e, __VA_ARGS__)
#define LOG_FATAL(...)      g_Logger.fatal(__FILE__, __LINE__, 0, __VA_ARGS__)
#define LOG_FATAL_WITH_ERRNO(e, ...)   g_Logger.fatal(__FILE__, __LINE__, e, __VA_ARGS__)
#define LOG_DUMP(d, l)      g_Logger.dump((const uint8_t*)(d), (l))
#define LOG_DUMP_V(d, l)    g_Logger.dump((const uint8_t*)(d), (l), ::PTP::Logger::LL_VERBOSE)

namespace PTP {
    
    class Logger {
        typedef void(*logweiter_t)(const char*fmt, va_list va);
    protected:
        int _loglevel;
        logweiter_t _writer;
        static void defaultWriter(const char* fmt, va_list va);
        void vlogWrite(const char*fmt, va_list va);
        void logWrite(const char*fmt, ...);

    public:
        enum {
            LL_VERBOSE  = PTPIP_LOGLEVEL_VERBOSE,
            LL_DEBUG    = PTPIP_LOGLEVEL_DEBUG,
            LL_INFO     = PTPIP_LOGLEVEL_INFO,
            LL_WARN     = PTPIP_LOGLEVEL_WARN,
            LL_ERROR    = PTPIP_LOGLEVEL_ERROR,
            LL_FATAL    = PTPIP_LOGLEVEL_FATAL,
        };
        Logger(int level = LL_WARN);
        inline virtual ~Logger() { logWrite("\n"); }
        inline int loglevel() { return _loglevel; }
        inline void loglevel(int level) { _loglevel = level; }
        void attachLogWriter(logweiter_t func);
        
        void dump(const uint8_t* data, size_t length, int level=LL_DEBUG);
        void verbose(const char* file, int line, int e, const char* fmt, ...);
        void debug(const char* file, int line, int e, const char* fmt, ...);
        void info(const char* file, int line, int e, const char* fmt, ...);
        void warn(const char* file, int line, int e, const char* fmt, ...);
        void error(const char* file, int line, int e, const char* fmt, ...);
        void fatal(const char* file, int line, int e, const char* fmt, ...);
        void write(int level, const char* file, int line, int e, const char* fmt, va_list va);
    };
}
extern PTP::Logger g_Logger;
#endif

#endif
