/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include "exceptions.h"
#include "logger.h"


namespace PTP {
    void Exception::init(const string& text, int errcode, const char* stream, size_t length)
    {
        _text.assign(text);
        if(errcode!=0) {
            char buf[128];
            ::strerror_r(errcode, buf, sizeof(buf));
            _text.append("(");
            _text.append(buf);
            _text.append(":");
            snprintf(buf, sizeof(buf), "%d", errcode);
            _text.append(buf);
            _text.append(")");
        }
        if(stream) _stream.assign(stream, length);
        string s;
        to_s(s);
        g_Logger.error(_file.c_str(), _line, 0, "%s(): %s\n", _func.c_str(), s.c_str());
    }
    
    void Exception::to_s(string& s) const
    {
        s.assign(_text);
        if(_stream.size()>0) {
            char buf[32];
            snprintf(buf, sizeof(buf), " : %d[bytes]\n", (int)_stream.size());
            s.append(buf);
            for(string::const_iterator it=_stream.begin(); it!=_stream.end(); it++) {
                snprintf(buf, sizeof(buf), "%02X ", (uint8_t)(*it));
                s.append(buf);
            }
        }
    }
}
