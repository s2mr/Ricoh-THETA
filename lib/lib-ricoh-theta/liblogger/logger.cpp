/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include "logger.h"
#include <string.h>

PTP::Logger g_Logger(PTP::Logger::LL_DEBUG);
using namespace std;

namespace PTP {
    
    Logger::Logger(int level)
    {
        _loglevel = level;
        _writer = defaultWriter;
    }
    
    void Logger::attachLogWriter(logweiter_t func)
    {
        _writer = func;
    }
    
    void Logger::defaultWriter(const char*fmt, va_list va)
    {
        ::vfprintf(stderr, fmt, va);
    }
    
    void Logger::vlogWrite(const char*fmt , va_list va)
    {
        _writer(fmt, va);
    }
    
    void Logger::logWrite(const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        vlogWrite(fmt, va);
        va_end(va);
    }
    
    void Logger::write(int level, const char* file, int line, int e, const char* fmt, va_list va)
    {
        static const char* const label[] = {
            "VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
        };
        if(_loglevel>level) return;
        char buf[1024];
        vsnprintf(buf, sizeof(buf), fmt, va);
        if(file!=NULL) {
            const char* f = strrchr(file, '/') - 1;
            while((f>=file) && *f!='/') f--;
            if(e!=0) {
                char es[128];
                memset(es, 0, sizeof(es));
                ::strerror_r(e, es, sizeof(es));
                logWrite("===[%s](%s:%d) %s: %s(%d)\n", label[level], f+1, line, buf, es, e);
            } else {
                logWrite("===[%s](%s:%d) %s\n", label[level], f+1, line, buf);
            }
        } else {
            logWrite("===[%s](%d) %s\n", label[level], line, buf);
        }
    }
    
    void Logger::dump(const uint8_t* data, size_t length, int level)
    {
        if(_loglevel>level) return;
        
        for(size_t p=0; p<length; p+=16) {
            size_t i;
            char buf[128], *b = buf;
            b += sprintf(b, "%08X:", (unsigned int)p);
            for(i=0; (i<16) && (p+i<length); i++) {
                b += sprintf(b, " %02X", data[p+i]);
            }
            for(; i<16; i++) {
                b += sprintf(b, "   ");
            }
            b += sprintf(b, " : ");
            for(size_t i=0; (i<16) && (p+i<length); i++) {
                uint8_t c = data[p+i];
                b += sprintf(b, "%c", isprint(c)? c: '.');
            }
            sprintf(b, "\n");
            logWrite("%s", buf);
        }
        
    }
    
    void Logger::verbose(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_VERBOSE, file, line, e, fmt, va);
        va_end(va);
    }
    
    void Logger::debug(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_DEBUG, file, line, e, fmt, va);
        va_end(va);
    }
    
    void Logger::info(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_INFO, file, line, e, fmt, va);
        va_end(va);
    }
    
    void Logger::warn(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_WARN, file, line, e, fmt, va);
        va_end(va);
    }
    
    void Logger::error(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_ERROR, file, line, e, fmt, va);
        va_end(va);
    }
    
    void Logger::fatal(const char* file, int line, int e, const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        write(LL_FATAL, file, line, e, fmt, va);
        va_end(va);
    }
}
