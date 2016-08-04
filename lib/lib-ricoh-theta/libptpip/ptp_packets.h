/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_ptp_packets_h
#define ptpip_initiator_ptp_packets_h

#include "codes.h"
#include "basictypes.h"
#include "variant.h"
#include "bytestream.h"
#include "exceptions.h"

using namespace std;


namespace PTP {
    
    class BasePacket {
    protected:
        uint16_t    _code;  // requestCode, responseCode, eventCode
        uint32_t    _session_id;
        uint32_t    _transaction_id;
        uint32_t    _params[5];
        virtual int num_of_params() const = 0;
    public:
        BasePacket(int code = 0);
        inline virtual ~BasePacket() {}
        void code(int code_) { _code = code_; }
        int code() const { return _code; }
        uint32_t session_id() const { return _session_id; }
        uint32_t transaction_id() const { return _transaction_id; }
        uint32_t& operator[](int index) const;
        void to_packet(string& ret) const;
        bool parse(PTP::ByteStream& stream);
        virtual const char* name() const = 0;
    };
    
    class ResponsePacket : public BasePacket {
    protected:
        virtual int num_of_params() const { return 5; };
        string _name;
    public:
        ResponsePacket(uint16_t code = 0,
                       uint32_t session_id = 0,
                       uint32_t transaction_id = 0,
                       uint32_t param1 = 0,
                       uint32_t param2 = 0,
                       uint32_t param3 = 0,
                       uint32_t param4 = 0,
                       uint32_t param5 = 0) {
            _code = code;
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = param1;
            _params[1] = param2;
            _params[2] = param3;
            _params[3] = param4;
            _params[4] = param5;
        }
        inline virtual ~ResponsePacket() {}
        virtual const char* name() const;
    };
    
    class EventPacket : public BasePacket {
    protected:
        virtual int num_of_params() const { return 3; };
        string _name;
    public:
        EventPacket(uint16_t code = 0,
                    uint32_t session_id = 0,
                    uint32_t transaction_id = 0,
                    uint32_t param1 = 0,
                    uint32_t param2 = 0,
                    uint32_t param3 = 0) {
            _code = code;
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = param1;
            _params[1] = param2;
            _params[2] = param3;
        }
        inline virtual ~EventPacket() {}
        virtual const char* name() const;
    };
    
    class RequestPacket : public BasePacket {
    protected:
        virtual int num_of_params() const { return 5; };
        ResponsePacket _response;
        PTP::ByteStream* _data;
    public:
        inline RequestPacket(int code) : BasePacket(code) { _data = NULL; }
        inline virtual ~RequestPacket() { }
        virtual bool data_required() const = 0;
        virtual bool receive_data() const = 0;
        void to_packet(string& ret) const;
        inline ResponsePacket& response() const { return *const_cast<ResponsePacket*>(&_response); }
        void payload(PTP::ByteStream* data) { _data = data; }
        PTP::ByteStream* payload() const { return _data; }
    };
    
    
    class GetDeviceInfo : public RequestPacket {
    protected:
    public:
        inline GetDeviceInfo()
        : RequestPacket(OperationCode::GET_DEVICE_INFO) {}
        inline virtual ~GetDeviceInfo() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetDeviceInfo"; }
    };
    
    class OpenSession : public RequestPacket {
    protected:
    public:
        OpenSession(uint32_t session_id)
        : RequestPacket(OperationCode::OPEN_SESSION) {
            _params[0] = session_id;
        }
        inline virtual ~OpenSession() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "OpenSession"; }
    };
    
    class CloseSession : public RequestPacket {
    protected:
    public:
        CloseSession(uint32_t session_id, uint32_t transaction_id)
        : RequestPacket(OperationCode::CLOSE_SESSION) {
            _session_id = session_id;
            _transaction_id = transaction_id;
        }
        inline virtual ~CloseSession() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "CloseSession"; }
    };
    
    class GetStorageIDs : public RequestPacket {
    protected:
    public:
        GetStorageIDs(uint32_t session_id, uint32_t transaction_id)
        : RequestPacket(OperationCode::GET_STORAGE_IDS) {
            _session_id = session_id;
            _transaction_id = transaction_id;
        }
        inline virtual ~GetStorageIDs() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetStorageIDs"; }
    };
    
    class GetStorageInfo : public RequestPacket {
    protected:
    public:
        GetStorageInfo(uint32_t session_id, uint32_t transaction_id,
                       uint32_t storage_id)
        : RequestPacket(OperationCode::GET_STORAGE_INFO) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = storage_id;
        }
        inline virtual ~GetStorageInfo() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetStorageInfo"; }
    };
    
    class GetNumObjects : public RequestPacket {
    protected:
    public:
        GetNumObjects(uint32_t session_id, uint32_t transaction_id,
                      uint32_t storage_id,
                      uint32_t object_format_code = 0xFFFFFFFFU,
                      uint32_t parent_object_handle = 0xFFFFFFFFU,
                      uint32_t extend1 = 0, uint32_t extend2 = 0)
        : RequestPacket(OperationCode::GET_NUM_OBJECTS) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = storage_id;
            _params[1] = object_format_code;
            _params[2] = parent_object_handle;
            _params[3] = extend1;
            _params[4] = extend2;
        }
        inline virtual ~GetNumObjects() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "GetNumObjects"; }
    };
    
    class GetObjectHandles : public RequestPacket {
    protected:
    public:
        GetObjectHandles(uint32_t session_id, uint32_t transaction_id,
                         uint32_t storage_id,
                         uint32_t object_format_code = 0xFFFFFFFFU,
                         uint32_t parent_object_handle = 0xFFFFFFFFU,
                         uint32_t extend1 = 0, uint32_t extend2 = 0)
        : RequestPacket(OperationCode::GET_OBJECT_HANDLES) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = storage_id;
            _params[1] = object_format_code;
            _params[2] = parent_object_handle;
            _params[3] = extend1;
            _params[4] = extend2;
        }
        inline virtual ~GetObjectHandles() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetObjectHandles"; }
    };
    
    class GetObjectInfo : public RequestPacket {
    protected:
    public:
        GetObjectInfo(uint32_t session_id, uint32_t transaction_id,
                      uint32_t object_handle)
        : RequestPacket(OperationCode::GET_OBJECT_INFO) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = object_handle;
        }
        inline virtual ~GetObjectInfo() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetObjectInfo"; }
    };
    
    class GetObject : public RequestPacket {
    protected:
    public:
        GetObject(uint32_t session_id, uint32_t transaction_id,
                  uint32_t object_handle)
        : RequestPacket(OperationCode::GET_OBJECT) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = object_handle;
        }
        inline virtual ~GetObject() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetObject"; }
    };
    
    class GetThumb : public RequestPacket {
    protected:
    public:
        GetThumb(uint32_t session_id, uint32_t transaction_id,
                 uint32_t object_handle)
        : RequestPacket(OperationCode::GET_THUMB) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = object_handle;
        }
        inline virtual ~GetThumb() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetThumb"; }
    };
    
    class DeleteObject : public RequestPacket {
    protected:
    public:
        DeleteObject(uint32_t session_id, uint32_t transaction_id,
                     uint32_t object_handle,
                     uint16_t object_format_code)
        : RequestPacket(OperationCode::DELETE_OBJECT) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = object_handle;
            _params[1] = object_format_code;
        }
        inline virtual ~DeleteObject() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "DeleteObject"; }
    };
    
    class InitiateCapture : public RequestPacket {
    protected:
    public:
        InitiateCapture(uint32_t session_id, uint32_t transaction_id,
                        uint32_t storage_id, uint32_t object_format_code)
        : RequestPacket(OperationCode::INITIATE_CAPTURE) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = storage_id;
            _params[1] = object_format_code;
        }
        inline virtual ~InitiateCapture() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "InitiateCapture"; }
    };
    
    class GetDevicePropDesc : public RequestPacket {
    protected:
    public:
        GetDevicePropDesc(uint32_t session_id, uint32_t transaction_id,
                          uint16_t device_prop_code)
        : RequestPacket(OperationCode::GET_DEVICE_PROP_DESC) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = (uint32_t)device_prop_code;
        }
        inline virtual ~GetDevicePropDesc() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetDevicePropDesc"; }
    };
    
    class GetDevicePropValue : public RequestPacket {
    protected:
    public:
        GetDevicePropValue(uint32_t session_id, uint32_t transaction_id,
                           uint16_t device_prop_code)
        : RequestPacket(OperationCode::GET_DEVICE_PROP_VALUE) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = (uint32_t)device_prop_code;
        }
        inline virtual ~GetDevicePropValue() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetDevicePropValue"; }
    };

    class SetDevicePropValue : public RequestPacket {
    protected:
    public:
        SetDevicePropValue(uint32_t session_id, uint32_t transaction_id,
                           uint16_t device_prop_code)
        : RequestPacket(OperationCode::SET_DEVICE_PROP_VALUE) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = (uint32_t)device_prop_code;
        }
        inline virtual ~SetDevicePropValue() {}
        inline virtual bool data_required() const { return true; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "SetDevicePropValue"; }
    };
    
    class TerminateOpenCapture : public RequestPacket {
    protected:
    public:
        TerminateOpenCapture(uint32_t session_id, uint32_t transaction_id,
                             uint32_t terminate_transaction_id)
        : RequestPacket(OperationCode::TERMINATE_OPEN_CAPTURE) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = terminate_transaction_id;
        }
        inline virtual ~TerminateOpenCapture() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "TerminateOpenCapture"; }
    };
    
    class InitiateOpenCapture : public RequestPacket {
    protected:
    public:
        InitiateOpenCapture(uint32_t session_id, uint32_t transaction_id,
                            uint32_t storage_id,
                            uint16_t object_format_code)
        : RequestPacket(OperationCode::INITIATE_OPEN_CAPTURE) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = storage_id;
            _params[1] = (uint16_t)object_format_code;
        }
        inline virtual ~InitiateOpenCapture() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "InitiateOpenCapture"; }
    };
    
    class GetResizedImageObject : public RequestPacket {
    protected:
    public:
        GetResizedImageObject(uint32_t session_id, uint32_t transaction_id,
                              uint32_t object_handle,
                              uint32_t width,
                              uint32_t height
                              )
        : RequestPacket(OperationCode::GET_RESIZED_IMAGE_OBJECT) {
            _session_id = session_id;
            _transaction_id = transaction_id;
            _params[0] = object_handle;
            _params[1] = width;
            _params[2] = height;
        }
        inline virtual ~GetResizedImageObject() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return true; }
        inline virtual const char* name() const { return "GetResizedImageObject"; }
    };

    class WlanPowerControl : public RequestPacket {
    protected:
    public:
        WlanPowerControl(uint32_t session_id, uint32_t transaction_id)
        : RequestPacket(OperationCode::WLAN_POWER_CONTROL) {
            _session_id = session_id;
            _transaction_id = transaction_id;
        }
        inline virtual ~WlanPowerControl() {}
        inline virtual bool data_required() const { return false; }
        inline virtual bool receive_data() const { return false; }
        inline virtual const char* name() const { return "WlanPowerControl"; }
    };
}

#endif