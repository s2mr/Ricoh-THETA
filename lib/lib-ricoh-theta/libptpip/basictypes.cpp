/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include <errno.h>
#include <time.h>
#include "basictypes.h"

namespace PTP {
    void STRING::to_ptp(string& ret) const
    {
        String::utf8_to_ptp(value(), ret);
        ret.insert(ret.begin(), ret.size()/2);
    }
    
    void STRING::parse(PTP::ByteStream& stream, STRING& ret)
    {
        uint8_t chars = 0;
        stream.read(chars);
        string s;
        stream.read(chars*2L, s);
        string u;
        String::ptp_to_utf8(s, u);
        ret.assign(u);
    }
    
    
    void DATETIME::to_ptp(string& ret) const
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%Y%m%dT%H%M%S", &_value);
        String::utf8_to_ptp(buf, ret);
        ret.insert(ret.begin(), ret.size()/2);
    }
    
    void DATETIME::parse(PTP::ByteStream &stream, DATETIME& ret)
    {
        int year, month, date, hour, minute, second, subsec, zone;
        STRING s;
        STRING::parse(stream, s);
        if (s.size()==0) {
            memset(&ret._value, 0, sizeof(ret._value));
            return;
        }
        if(sscanf(s.value().c_str(), "%04d%02d%02dT%02d%02d%02d.%1d%05d", &year, &month, &date, &hour, &minute, &second, &subsec, &zone)==8) {
            // OK
        } else  if(sscanf(s.value().c_str(), "%04d%02d%02dT%02d%02d%02d.%1dZ", &year, &month, &date, &hour, &minute, &second, &subsec)==7) {
            // OK
            zone = 0;
        } else if(sscanf(s.value().c_str(), "%04d%02d%02dT%02d%02d%02d%05d", &year, &month, &date, &hour, &minute, &second, &zone)==7) {
            // OK
        } else if(sscanf(s.value().c_str(), "%04d%02d%02dT%02d%02d%02dZ", &year, &month, &date, &hour, &minute, &second)==6) {
            // OK
            zone = 0;
        } else {
            throw RUNTIME_ERROR("PTP::DATETIME : Invalid stream. : ", s.c_str(), s.size());
        }
        
        memset(&ret._value, 0, sizeof(tm));
        ret._value.tm_year = year - 1900;
        ret._value.tm_mon = month - 1;
        ret._value.tm_mday = date;
        ret._value.tm_hour = hour;
        ret._value.tm_min = minute;
        ret._value.tm_sec = second;
    }
}