/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include <string.h>
#include "ptp_packets.h"

namespace PTP {
    BasePacket::BasePacket(int code)
    {
        _code = code;
        _session_id = 0;
        _transaction_id = 0;
        memset(_params, 0, sizeof(_params));
    }
    
    uint32_t& BasePacket::operator[](int index) const
    {
        if((index<0) || (index>num_of_params())) throw RUNTIME_ERROR("Bad index");
        return *const_cast<uint32_t*>(&_params[index]);
    }
    
    void BasePacket::to_packet(string& ret) const
    {
        string t;
        ret.clear();
        PTP::String::to_s(_code, t);
        ret.append(t);
        PTP::String::to_s(_session_id, t);
        ret.append(t);
        PTP::String::to_s(_transaction_id, t);
        ret.append(t);
        for(int i = 0; i<num_of_params(); i++) {
            PTP::String::to_s(_params[i], t);
            ret.append(t);
        }
    }
    
    bool BasePacket::parse(PTP::ByteStream& stream)
    {
        uint16_t code = 0;
        size_t params = 0;
        
        if(!stream.peek(code)) return false;
        if((OperationCode::BEGIN<=code) && (code<OperationCode::END)) {
            params = 5;
        } else if((ResponseCode::BEGIN<=code) && (code<ResponseCode::END)) {
            params = 5;
        } else if((EventCode::BEGIN<=code) && (code<EventCode::END)) {
            params = 3;
        } else {
            throw RUNTIME_ERROR("Bad packet code");
        }
        
        if(stream.length()<(2+4+4+params*4)) return false;
        stream.read(_code);
        stream.read(_session_id);
        stream.read(_transaction_id);
        for(int i = 0; i<(int)params; i++) {
            stream.read(_params[i]);
        }
        return true;
    }
    
    const char* ResponsePacket::name() const
    {
#define T(_NAME_) {#_NAME_ "(0x%04X)", ResponseCode::_NAME_}
        static const struct {
            const char* f;
            int v;
        } ary[] = {
            T(OK),                              // 0x2001
            T(GENERAL_ERROR),                   // 0x2002
            T(SESSION_NOT_OPEN),                // 0x2003
            T(INVALID_TRANSACTION_ID),          // 0x2004
            T(OPERATION_NOT_SUPPORTED),         // 0x2005
            T(PARAMETER_NOT_SUPPORTED),         // 0x2006
            T(INCOMLETE_TRANSFER),              // 0x2007
            T(INVALID_STORAGE_ID),              // 0x2008
            T(INVALID_OBJECT_HANDLE),           // 0x2009
            T(DEVICE_PROP_NOT_SUPPORTED),       // 0x200A
            T(INVALID_OBJECT_FORMAT_CODE),      // 0x200B
            T(STORE_FULL),                      // 0x200C
            T(OBJECT_WRITE_PROTECTED),          // 0x200D
            T(STORE_READ_ONLY),                 // 0x200E
            T(ACCESS_DENIED),                   // 0x200F
            T(NO_THUMBNAIL_PRESENT),            // 0x2010
            T(SELFTEST_FAILED),                 // 0x2011
            T(PARTIAL_DELETION),                // 0x2012
            T(STORE_NOT_AVAILABLE),             // 0x2013
            T(SPECIFICATION_BY_FORMAT_UNSUPPORTED), // 0x2014
            T(NO_VALID_OBJECT_INFO),            // 0x2015
            T(INVALID_CODE_FORMAT),             // 0x2016
            T(UNKNOWN_VENDOR_CODE),             // 0x2017
            T(CAPTURE_ALREADY_TERMINATED),      // 0x2018
            T(DEVICE_BUSY),                     // 0x2019
            T(INVALID_PARENT_OBJECT),           // 0x201A
            T(INVALID_DEVICE_PROP_FORMAT),      // 0x201B
            T(INVALID_DEVICE_PROP_VALUE),       // 0x201C
            T(INVALID_PARAMETER),               // 0x201D
            T(SESSION_ALREADY_OPEN),            // 0x201E
            T(TRANSACTION_CANCELED),            // 0x201F
            T(SPECIFICATION_OF_DESTINATION_UNSUPPORTED), // 0x2020
        };
        char buf[64];
        if(_name.empty()) {
            if((ResponseCode::BEGIN<=code()) && (code()<ResponseCode::END)) {
                snprintf(buf, sizeof(buf), ary[code()-ResponseCode::BEGIN].f, code());
            } else {
                snprintf(buf, sizeof(buf), "ResponsePacket::<<undef>>(0x%04X)", code());
            }
            const_cast<string*>(&_name)->assign(buf);
        }
        return _name.c_str();
#undef T
    }
    
    const char* EventPacket::name() const
    {
#define T(_NAME_) {#_NAME_ "(0x%04X)", EventCode::_NAME_}
        static const struct {
            const char* f;
            int v;
        } ary[] = {
            T(CANCEL_TRANSACTION),          // 0x4001
            T(OBJECT_ADDED),                // 0x4002
            T(OBJECT_REMOVED),              // 0x4003
            T(STORE_ADDED),                 // 0x4004
            T(STORE_REMOVED),               // 0x4005
            T(DEVICE_PROP_CHANGED),         // 0x4006
            T(OBJECT_INFO_CHANGED),         // 0x4007
            T(DEVICE_INFO_CHANGED),         // 0x4008
            T(REQUEST_OBJECT_TRANSFER),     // 0x4009
            T(STORE_FULL),                  // 0x400A
            T(DEVICE_RESET),                // 0x400B
            T(STORAGE_INFO_CHANGED),        // 0x400C
            T(CAPTURE_COMPLETE),            // 0x400D
            T(UNREPORTED_STATUS),           // 0x400E
        };
        char buf[64];
        if(_name.empty()) {
            if((EventCode::BEGIN<=code()) && (code()<EventCode::END)) {
                snprintf(buf, sizeof(buf), ary[code()-EventCode::BEGIN].f, code());
            } else {
                snprintf(buf, sizeof(buf), "EventCode::<<undef>>(0x%04X)", code());
            }
            const_cast<string*>(&_name)->assign(buf);
        }
        return _name.c_str();
#undef T
    }
    
    
    void RequestPacket::to_packet(string& ret) const
    {
        if(data_required() && (_data==NULL)) throw STANDARD_ERROR("Transfer data is required.", 0);
        BasePacket::to_packet(ret);
    }
}
