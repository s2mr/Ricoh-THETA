/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_ptpip_packets_h
#define ptpip_initiator_ptpip_packets_h

#include <map>
#include "codes.h"
#include "bytestream.h"
#include "exceptions.h"

using namespace std;


namespace PTP {
    namespace IP {
        class GUID {
        protected:
            uint32_t v1, v2, v3, v4;
            uint64_t v5;
            string _text;
            void assign(const char*);
        public:
            inline GUID() {};
            GUID(const char* v) { assign(v); }
            GUID(const string v) { assign(v.c_str()); }
            inline virtual ~GUID() {};
            void clear();
            const GUID& operator=(const GUID& v);
            const GUID& operator=(const char* v) { assign(v); return *this; }
            const GUID& operator=(const string& v) { assign(v.c_str()); return *this; }
            operator string&() const { to_str(*const_cast<string*>(&_text)); return *const_cast<string*>(&_text); }
            void to_str(string& ret) const;
            void to_packet(string& ret) const;
            static void from_s(GUID& ret, string& v);
        };
        
        typedef enum {
            ptUNDEF = 0,
            ptUINT16,
            ptUINT32,
            ptUINT64,
            ptGUID,
            ptSTRING,
            ptBINARY,
            ptUINT32_VECTOR_3,
            ptUINT32_VECTOR_5,
        } PARAM_TYPE;
        
        // Packet parameter classes.
        class Parameter {
        protected:
            PARAM_TYPE  _type;
            union {
                uint16_t    _u16;
                uint32_t    _u32;
                uint64_t    _u64;
            };
            GUID        _g;
            string      _s;
            //uint32_t    _v32[5];
            vector<uint32_t> _v32v;
            size_t      _required_length;
        public:
            Parameter();
            inline virtual ~Parameter() {}
            Parameter(PARAM_TYPE t);
            inline Parameter(uint16_t v) { assign(v); }
            inline Parameter(uint32_t v) { assign(v); }
            inline Parameter(uint64_t v) { assign(v); }
            inline Parameter(const GUID& v) { assign(v); }
            inline Parameter(const string& v) { assign(v); }
            inline Parameter(const char*v, size_t length) { assign(v, length); }
            inline Parameter(uint32_t v1, uint32_t v2, uint32_t v3) { assign(v1, v2, v3); }
            inline Parameter(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4, uint32_t v5) { assign(v1, v2, v3, v4, v5); }
            
            inline void required_length(size_t len) { _required_length = len; }
            
            void assign(uint16_t v);
            void assign(uint32_t v);
            void assign(uint64_t v);
            void assign(const GUID& v);
            void assign(const string& v);
            void assign(const char*v, size_t length);
            void assign(uint32_t v1, uint32_t v2, uint32_t v3);
            void assign(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4, uint32_t v5);
            
            inline PARAM_TYPE type() const { return _type; }
            void type(PARAM_TYPE t);
            
            uint16_t& operator=(const uint16_t& v);
            uint32_t& operator=(const uint32_t& v);
            uint64_t& operator=(const uint64_t& v);
            GUID& operator=(const GUID& v);
            string& operator=(const string& v);
            uint32_t& operator[](int index);
            
            operator uint16_t() const;
            operator uint32_t() const;
            operator uint64_t() const;
            operator const GUID&() const;
            operator const string&() const;
            operator const vector<uint32_t>&() const;
            
            void to_packet(string& ret) const;
            void parse(PTP::ByteStream& stream);
        };
        
        
        typedef enum {
            // (centinel)
            pnEND = 0,
            
            // ==> InitCommandRequest
            pnGUID,
            pnFriendlyName,
            pnProtocolVersion,
            
            // <== InitCommandAck
            pnConnectionNumber,
            // pnGUID,
            // pnFriendlyName,
            // pnProtocolversion,
            
            // ==> InitEventRequest
            // pnConnectionNumber,
            
            // <== InitEventAck
            // (none)
            
            // <== InitFail
            pnReasonOfFail,
            
            // ==> OperationRequest
            pnDataPhaseInfo,
            pnOperationCode,
            pnTransactionId,
            pnParams,
            
            // <== OperationResponse
            pnResponseCode,
            // pnTransactionId,
            // pnParams,
            
            // <== Event
            pnEventCode,
            // pnTransactionId,
            // pnParams,
            
            // <== StartData
            // pnTransactionId,
            pnTotalDataLength,
            
            // <== Data
            // pnTransactionId,
            pnDataPayload,
            
            // <==> Cancel
            // pnTransactionId,
            
            // <== EndData
            // pnTransactionId,
            // pnDataPayload,
            
            // ==> ProbeRequest
            // (none)
            
            // <== ProbeResponse
            // (none)
        } PARAM_NAME;
        
        
        class BasePacket {
        public:
            inline BasePacket(size_t packet_length = 0) {
                _packet_length = packet_length;
            }
            inline virtual ~BasePacket() {}
            virtual int id() const = 0;
            Parameter& operator[](PARAM_NAME param_name) const;
            void to_packet(vector<string>& ret) const;
            void parse(PTP::ByteStream& stream);
        protected:
            map<PARAM_NAME, Parameter> _params;
            size_t _packet_length;
            virtual const PARAM_NAME* param_list() const = 0;
        };
        
        
        class InitCommandRequest : public BasePacket {
        public:
            InitCommandRequest(size_t packet_length = 0);
            inline virtual ~InitCommandRequest() {}
            inline virtual int id() const { return PTP::IP::PacketCode::InitCommandRequest; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class InitCommandAck : public BasePacket {
        public:
            InitCommandAck(size_t packet_length = 0);
            inline virtual ~InitCommandAck() {}
            inline virtual int id() const { return PTP::IP::PacketCode::InitCommandAck; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class InitEventRequest : public BasePacket {
        public:
            InitEventRequest(size_t packet_length = 0);
            inline virtual ~InitEventRequest() {}
            inline virtual int id() const { return PTP::IP::PacketCode::InitEventRequest; }
        protected:
            virtual const PARAM_NAME* param_list() const;
       };
        
        class InitEventAck : public BasePacket {
        public:
            InitEventAck(size_t packet_length = 0);
            inline virtual ~InitEventAck() {}
            inline virtual int id() const { return PTP::IP::PacketCode::InitEventAck; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class InitFail : public BasePacket {
        public:
            InitFail(size_t packet_length = 0);
            inline virtual ~InitFail() {}
            inline virtual int id() const { return PTP::IP::PacketCode::InitFail; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class OperationRequest : public BasePacket {
        public:
            OperationRequest(size_t packet_length = 0);
            inline virtual ~OperationRequest() {}
            inline virtual int id() const { return PTP::IP::PacketCode::OperationRequest; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class OperationResponse : public BasePacket {
        public:
            OperationResponse(size_t packet_length = 0);
            inline virtual ~OperationResponse() {}
            inline virtual int id() const { return PTP::IP::PacketCode::OperationResponse; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class Event : public BasePacket {
        public:
            Event(size_t packet_length = 0);
            inline virtual ~Event() {}
            inline virtual int id() const { return PTP::IP::PacketCode::Event; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class StartData : public BasePacket {
        public:
            StartData(size_t packet_length = 0);
            inline virtual ~StartData() {}
            inline virtual int id() const { return PTP::IP::PacketCode::StartData; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class Cancel : public BasePacket {
        public:
            Cancel(size_t packet_length = 0);
            inline virtual ~Cancel() {}
            inline virtual int id() const { return PTP::IP::PacketCode::Cancel; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class Data : public BasePacket {
        public:
            Data(size_t packet_length = 0);
            inline virtual ~Data() {}
            inline virtual int id() const { return PTP::IP::PacketCode::Data; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };
        
        class EndData : public BasePacket {
        public:
            EndData(size_t packet_length = 0);
            inline virtual ~EndData() {}
            inline virtual int id() const { return PTP::IP::PacketCode::EndData; }
        protected:
            virtual const PARAM_NAME* param_list() const;
        };

        class ProbeRequest : public BasePacket {
        public:
            ProbeRequest(size_t packet_length = 0);
            inline virtual ~ProbeRequest() {};
        protected:
            inline virtual int id() const { return PTP::IP::PacketCode::ProbeRequest; }
            virtual const PARAM_NAME* param_list() const;
        };
        
        class ProbeResponse : public BasePacket {
        public:
            ProbeResponse(size_t packet_length = 0);
            inline virtual ~ProbeResponse() {}
        protected:
            inline virtual int id() const { return PTP::IP::PacketCode::ProbeResponse; }
            virtual const PARAM_NAME* param_list() const;
        };
        
        BasePacket* parse(PTP::ByteStream& stream);
    }
}


#endif