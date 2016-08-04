/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_ptpip_h
#define ptpip_initiator_ptpip_h

#include <string>
#include <vector>
#include <pthread.h>
#include <sys/time.h>

#include "codes.h"
#include "basictypes.h"
#include "ptpip_packets.h"
#include "ptp_packets.h"
#include "bufferedsock.h"
#include "logger.h"

using namespace std;

namespace PTP {
    namespace IP {
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
        extern int g_BenchmarkOutout;
        class Benchmark {
        protected:
            float _delay;
            struct timeval _start;
            std::string _header, _text;
            inline float timediff(const struct timeval& t1, const struct timeval& t2) const {
                return (t2.tv_sec-t1.tv_sec)*1000.0+(t2.tv_usec-t1.tv_usec)/1000.0;
            }
        public:
            inline Benchmark(const char* header) {
                struct timeval b1, b2;
                ::gettimeofday(&b1, NULL);
                ::gettimeofday(&b2, NULL);
                _delay = timediff(b1, b2);
                _header = header;
                begin("");
            }
            inline ~Benchmark() {
                end();
            }
            inline void begin(const char* text) {
                _text = text;
                ::gettimeofday(&_start, NULL);
            }
            inline void lap(const char* desc) {
                if (g_BenchmarkOutout<0) return;
                char buf[1024];
                struct timeval end;
                ::gettimeofday(&end, NULL);
                float diff = timediff(_start, end);
                struct timeval b1, b2;
                ::gettimeofday(&b1, NULL);
                int l = ::snprintf(buf, sizeof(buf), "[Benchmark] %s:%s(%s): %0.2f[ms]\n", _header.c_str(), _text.c_str(), desc, diff);
                ::write(g_BenchmarkOutout, buf, l);
                ::gettimeofday(&b2, NULL);
                _delay += timediff(b1, b2);
            }
            inline void end() {
                if (_start.tv_sec>0) {
                    lap("");
                    _start.tv_sec = 0;
                }
            }
        };
#endif
        
        class Initiator;
        typedef void*(*PTHREAD_PROC_T)(void*);
        typedef void(*eventListenr_t)(const void* _this, const EventPacket& event);
        typedef void(*connectionEventListener_t)(const void* _this, int e);
        
        class Session;
        
        class Initiator {
        protected:
            bool _connected;
            bool _disconnected;
            int _port;
            string _server;
            string _guid;
            string _friendly_name;
            
            BufferedSocket _command_sock;
            BufferedSocket _event_sock;
            
            ::pthread_mutex_t _event_listener_mutex;
            eventListenr_t _eventListener;
            const void* _eventListener_self;
            connectionEventListener_t _connectionEventListener;
            const void* _connectionEventListener_self;
            ::pthread_t _event_thread;
            time_t _watchdogSendingInterval;
            int _watchdogLimit;
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            Benchmark _benchmark;
#endif
            int _timelimitForResponse;

        public:
            uint32_t _target_ConnectionNumber;
            string   _target_GUID;
            string   _target_FriendlyName;
            uint32_t _target_ReasonOfFail;
            
        protected:
            bool connect_command_sock();
            bool connect_event_sock();
            void callbackEventListener(EventPacket& ep);
            void callbackConnectionEventListener(int e);
            static void* event_socket_proc(Initiator* self);
            static void* watchdog_proc(Initiator* self);
            
        public:
            bool send_operation_request(Session& session, RequestPacket& rp);
            bool send_payload(Session& session, PTP::ByteStream& payload);
            bool recv_payload(Session& session, ResponsePacket& rp, PTP::ByteStream& recv_data);
            bool recv_operation_response(Session& session, ResponsePacket& rp);
            
        public:
            bool request_command(Session& session, RequestPacket& rp, PTP::ByteStream* recv_data);
            
        public:
            Initiator();
            inline virtual ~Initiator() { close(); cleanup(); }
            
            inline void guid(const char* g) { _guid.assign(g); }
            inline void friendly_name(const char* f) { _friendly_name.assign(f); }
            bool connect(const char* server, int port = DEFAULT_PORT);
            bool connected() const { return _connected; }
            bool disconnected() const { return _disconnected; }
            void close();
            void set_watchdog_limit(int limit=5) { _watchdogLimit = limit; }
            void set_timelimit_for_response(int limit=20) { _timelimitForResponse = limit; }
            
        public:
            void attachEventListener(const void* _this, eventListenr_t listener);
            void attachConnectionEventListener(const void* _this, connectionEventListener_t listener);
            
        public:
            bool getDeviceInfo(DeviceInfo_t& result);
            bool openSession(Session& s);
            
        protected:
            void cleanup();
        };
        
        
        class Session {
            friend class Initiator; // #openSession only.
        protected:
            bool _opened;
            uint32_t _session_id;
            uint32_t _transaction_id;
            uint16_t _last_response_code;
            Initiator* _initiator;
            bool _closeSession_at_destructor;
            void init(uint32_t sid = 1) {
                _opened = false;
                _session_id = sid;
                _transaction_id = 0;
                _last_response_code = -1;
                _initiator = NULL;
                _closeSession_at_destructor = true;
            }
            
        public:
            Session(Initiator* ini = NULL)
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            :_transactoin_benchmark("session")
#endif
            {
                init(0);
                _closeSession_at_destructor = (ini!=NULL)? true: false;
                _initiator = ini;
            }
            virtual ~Session() {
                if(_closeSession_at_destructor && _initiator) {
                    try {
                        closeSession();
                    }
                    catch(...) {}
                }
            }
            inline uint32_t session_id() const { return _session_id; }
            inline uint32_t transaction_id() const { return _transaction_id; }
            inline void set_last_response_code(uint16_t code) { _last_response_code = code; }
            inline uint16_t last_response_code() const { return _last_response_code; }
            inline bool opened() const { return _opened; }
            
        public:
            void start_transaction(const char*file, int line, const char* func);
            void end_transaction(const char*file, int line, const char* func);
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            Benchmark _transactoin_benchmark;
#endif

        public:
            bool closeSession();
            bool getStorageIDs(vector<uint32_t>& ids);
            bool getStorageInfo(StorageInfo_t& result);
            bool getNumObjects(uint32_t storage_id, uint32_t objectFormatCode, uint32_t parent, uint64_t dateFrom, uint32_t& result);
            bool getObjectHandles(uint32_t storage_id, uint32_t objectFormatCode, uint32_t parent, uint64_t dateFrom, vector<uint32_t>& result);
            bool getObjectInfo(uint32_t objectHandle, ObjectInfo_t& result);
            bool getObject(uint32_t objectHandle, PTP::ByteStream& result);
            bool getThumb(uint32_t objectHandle, PTP::ByteStream& result);
            bool deleteObject(uint32_t objectHandle, uint32_t objectFormatCode);
            bool initiateCapture(uint32_t storage_id, uint32_t objectFormatCode);
            bool getDevicePropDesc(uint16_t devicePropCode, PTP::ByteStream& result);
            bool getDevicePropValue(uint16_t devicePropCode, PTP::ByteStream& result);
            bool setDevicePropValue(uint16_t devicePropCode, const std::string& value);
            bool terminateOpenCapture(uint32_t transaction_id);
            bool initiateOpenCapture(uint32_t storage_id, uint32_t objectFormatCode, uint32_t& transaction_id);
            bool getResizedImageObject(uint32_t objectHandle, uint32_t width, uint32_t height, PTP::ByteStream& result);
            bool wlanPowerControl();
        };
    }

    uint16_t getDevicePropType(uint16_t devicePropCode);
}

#endif
