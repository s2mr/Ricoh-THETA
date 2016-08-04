/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_exceptions_h
#define ptpip_initiator_exceptions_h

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>

using namespace std;

#define RUNTIME_ERROR(...) RuntimeError(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define STANDARD_ERROR(...) StandardError(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

namespace PTP {
    
    class Exception {
    protected:
        string _name;
        string _file;
        int    _line;
        string _func;
        string _text;
        string _stream;
        void init(const string& text, int errcode, const char* stream, size_t length);
    public:
        Exception(const string& name, const string& file, int line, const string& func) {
            _name.assign(name);
            _file.assign(file);
            _line = line;
            _func = func;
        }
        virtual ~Exception() {}
        void to_s(string& s) const;
    };
    
    class RuntimeError : public Exception {
    public:
        inline RuntimeError(const string& file, int line, const string& func, const string& text)
        : Exception("RuntimeError", file, line, func) {
            init(text, 0, NULL, 0);
        }
        inline RuntimeError(const string& file, int line, const string& func, const string& text, int errcode)
        : Exception("RuntimeError", file, line, func) {
            init(text, errcode, NULL, 0);
        }
        inline RuntimeError(const string& file, int line, const string& func, const string& text, const char* stream, size_t length)
        : Exception("RuntimeError", file, line, func) {
            init(text, 0, stream, length);
        }
        inline RuntimeError(const string& file, int line, const string& func, const string& text, int errcode, const char* stream, size_t length)
        : Exception("RuntimeError", file, line, func) {
            init(text, errcode, stream, length);
        }
        virtual ~RuntimeError() {}
    };
    
    class StandardError : public Exception {
    public:
        inline StandardError(const string& file, int line, const string& func, const string& text)
        : Exception("StandardError", file, line, func) {
            init(text, 0, NULL, 0);
        }
        inline StandardError(const string& file, int line, const string& func, const string& text, int errcode)
        : Exception("StandardError", file, line, func) {
            init(text, errcode, NULL, 0);
        }
        inline StandardError(const string& file, int line, const string& func, const string& text, const char* stream, size_t length)
        : Exception("StandardError", file, line, func) {
            init(text, 0, stream, length);
        }
        inline StandardError(const string& file, int line, const string& func, const string& text, int errcode, const char* stream, size_t length)
        : Exception("StandardError", file, line, func) {
            init(text, errcode, stream, length);
        }
        virtual ~StandardError() {}
    };
}

#endif
