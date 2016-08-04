/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include <stdio.h>
#include "ptpip.h"

using namespace PTP::IP;

namespace PTP {
    namespace IP {
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
        int g_BenchmarkOutout = STDOUT_FILENO;
#endif
        Initiator::Initiator()
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
        :_benchmark("initiator")
#endif
        {
            pthread_mutex_init(&_event_listener_mutex, NULL);
            memset(&_event_thread, 0, sizeof(_event_thread));
            _eventListener = NULL;
            _eventListener_self = NULL;
            _connectionEventListener = NULL;
            _connectionEventListener_self = NULL;
            _timelimitForResponse = 20;
            cleanup();
        }
        
        void Initiator::cleanup()
        {
            _connected = false;
            _disconnected = false;
            _port = 0;
            _server.clear();
            _guid.assign("8DFB11BD-E445-47E7-A917-31F8FCC4347D");
            _friendly_name.assign("RICOH THETA iOS Player");
            _watchdogSendingInterval = 5;
            _watchdogLimit = 0; // default is `DISABLED`.
            _command_sock.clear();
            _event_sock.clear();
        }
        
        
        bool Initiator::connect_command_sock()
        {
            InitCommandAck* ica = NULL;
            InitFail* fail = NULL;
            vector<string> p;
            
            // Connect command socket.
            if(!_command_sock.connect(_server.c_str(), _port)) {
                LOG_FATAL_WITH_ERRNO(_command_sock.getlasterror(), "command_sock.connect failed.");
                callbackConnectionEventListener(_command_sock.getlasterror());
                return false;
            }
            InitCommandRequest icr;
            icr[pnGUID] = GUID(_guid);
            icr[pnFriendlyName] = _friendly_name;
            icr.to_packet(p);
            _command_sock.send(p);

            BasePacket* bp = _command_sock.receive_packet(2);
            if(bp==NULL) {
                LOG_FATAL("InitCommand timed out or brocken stream.");
                callbackConnectionEventListener(_command_sock.connected()? _command_sock.getlasterror(): PTPIP_PROTOCOL_SOCKET_CLOSED);
                return false;
            }
            
            switch(bp->id()) {
                case PacketCode::InitCommandAck:
                    ica = (InitCommandAck*)bp;
                    break;
                case PacketCode::InitFail:
                    fail = (InitFail*)bp;
                    _target_ReasonOfFail = (*fail)[pnReasonOfFail];
                    LOG_FATAL("Connection refused by responder (command sock:%d).", _target_ReasonOfFail);
                    callbackConnectionEventListener(PTPIP_PROTOCOL_REJECTED);
                    delete bp;
                    return false;
                default:
                    delete bp;
                    LOG_FATAL("Bad response from responder (command sock).");
                    callbackConnectionEventListener(PTPIP_PROTOCOL_UNRECOGNIZED_COMMAND);
                    return false;
            }
            _target_ConnectionNumber = (*ica)[pnConnectionNumber];
            _target_GUID = (const string&)(*ica)[pnGUID];
            _target_FriendlyName = (const string&)(*ica)[pnFriendlyName];
            delete bp;
            
            LOG_DEBUG("Command sock connected.");
            return true;
        }
        
        bool Initiator::connect_event_sock()
        {
            // InitEventAck* iea = NULL;
            InitFail* fail = NULL;
            vector<string> p;
            
            // Connect event socket.
            if(!_event_sock.connect(_server.c_str(), _port)) {
                LOG_FATAL_WITH_ERRNO(_event_sock.getlasterror(), "event_sock.connect failed.");
                callbackConnectionEventListener(_event_sock.getlasterror());
                return false;
            }
            InitEventRequest ier;
            ier[pnConnectionNumber] = _target_ConnectionNumber;
            ier.to_packet(p);
            _event_sock.send(p);
            
            BasePacket* bp = _event_sock.receive_packet(_timelimitForResponse);
            if(bp==NULL) {
                LOG_FATAL("InitEvent timed out or brocken stream.");
                callbackConnectionEventListener(_event_sock.connected()? _event_sock.getlasterror(): PTPIP_PROTOCOL_SOCKET_CLOSED);
                return false;
            }
            
            switch(bp->id()) {
                case PacketCode::InitEventAck:
                    // iea = (InitEventAck*)bp;
                    break;
                case PacketCode::InitFail:
                    fail = (InitFail*)bp;
                    _target_ReasonOfFail = (*fail)[pnReasonOfFail];
                    delete bp;
                    LOG_FATAL("Connection refused by responder (event sock:%d).", _target_ReasonOfFail);
                    callbackConnectionEventListener(PTPIP_PROTOCOL_REJECTED);
                    return false;
                default:
                    delete bp;
                    LOG_FATAL("Bad response from responder (event_sock).");
                    callbackConnectionEventListener(PTPIP_PROTOCOL_UNRECOGNIZED_COMMAND);
                    return false;
            }
            delete bp;
            
            LOG_DEBUG("Event sock connected.");
            return true;
        }
        
        bool Initiator::connect(const char* server, int port)
        {
            if(_connected) RUNTIME_ERROR("Already connected.");
            
            if(server) _server.assign(server);
            if(port>0) _port = port;
            
            LOG_INFO("Connecting %s:%d...", _server.c_str(), _port);
            _target_ConnectionNumber = 0;
            _target_GUID = "";
            _target_FriendlyName = "";
            _target_ReasonOfFail = 0;
            
            if(connect_command_sock() && connect_event_sock()) {
                _connected = true;
                _disconnected = false;
                ::pthread_create(&_event_thread, NULL, (PTHREAD_PROC_T)event_socket_proc, (void*)this);
                return true;
            }
            
            _command_sock.close();
            _event_sock.close();
            return false;
        }
        
        
        void Initiator::close()
        {
            if(_connected) {
                _command_sock.close();
                _event_sock.close();
                void* r;
                ::pthread_join(_event_thread, &r);
            }
            _connected = false;
            _disconnected = false;
        }
        
        
        void Initiator::attachEventListener(const void* _this, eventListenr_t listener)
        {
            ::pthread_mutex_lock(&_event_listener_mutex);
            {
                _eventListener = listener;
                _eventListener_self = _this;
            }
            ::pthread_mutex_unlock(&_event_listener_mutex);
        }
        
        
        void Initiator::callbackEventListener(EventPacket& ep)
        {
            eventListenr_t func = NULL;
            const void* param = NULL;
            ::pthread_mutex_lock(&_event_listener_mutex);
            {
                func = _eventListener;
                param = _eventListener_self;
            }
            ::pthread_mutex_unlock(&_event_listener_mutex);
            if(func) func(param, ep);
        }
        
        
        void Initiator::attachConnectionEventListener(const void* _this, connectionEventListener_t listener)
        {
            ::pthread_mutex_lock(&_event_listener_mutex);
            {
                _connectionEventListener = listener;
                _connectionEventListener_self = _this;
            }
            ::pthread_mutex_unlock(&_event_listener_mutex);
        }
        
        
        void Initiator::callbackConnectionEventListener(int e)
        {
            connectionEventListener_t func = NULL;
            const void* param = NULL;
            ::pthread_mutex_lock(&_event_listener_mutex);
            {
                func = _connectionEventListener;
                param = _connectionEventListener_self;
            }
            ::pthread_mutex_unlock(&_event_listener_mutex);
            if(func) func(param, e);
        }

        
        void* Initiator::event_socket_proc(Initiator* self)
        {
            time_t last_event = ::time(NULL);
            int watchdog_expired_count = 0;
            
            while(self->_event_sock.connected()) {
                BasePacket* bp = NULL;
                try {
                    bp = self->_event_sock.receive_packet(1);
                    if(!bp) {
                        if (self->_watchdogLimit!=0) {
                            time_t now = ::time(NULL);
                            if(now > last_event + self->_watchdogSendingInterval) {
                                ProbeRequest pr;
                                vector<string> p;
                                pr.to_packet(p);
                                self->_event_sock.send(p);
                                last_event = now;
                                ++watchdog_expired_count;
                            } else
                            if(watchdog_expired_count >= self->_watchdogLimit) {
                                self->callbackConnectionEventListener(PTPIP_PROTOCOL_WATCHDOG_EXPIRED);
                                watchdog_expired_count = 0;
                            }
                        }
                        continue;
                    }
                    watchdog_expired_count = 0;
                    last_event = ::time(NULL);
                    LOG_DEBUG("event_socket_proc.loop: tick=%u", last_event);
                    switch(bp->id()) {
                        case PacketCode::Event:
                            {
                                Event& e = *(Event*)bp;
                                const vector<uint32_t>& p = e[pnParams];
                                EventPacket ep(e[pnEventCode], 0, e[pnTransactionId], p[0], p[1], p[2]);
                                LOG_DEBUG("Incomming %s...", ep.name());
                                self->callbackEventListener(ep);
                            }
                            break;
                        case PacketCode::ProbeResponse:
                            LOG_DEBUG("ProbeResponsed.");
                            break;
                        default:
                            LOG_WARN("Invalid packet in event stream. id=%d", bp->id());
                            self->callbackConnectionEventListener(PTPIP_PROTOCOL_UNRECOGNIZED_COMMAND);
                            break;
                    }
                } catch(Exception e) {
                }
                delete bp;
            }
            self->_disconnected = true;
            self->callbackConnectionEventListener(PTPIP_PROTOCOL_SOCKET_CLOSED);
            return NULL;
        }
        
        
        bool Initiator::send_operation_request(Session& session, RequestPacket& rp)
        {
            vector<string> p;
            
            OperationRequest r;
            r[pnDataPhaseInfo] = (uint32_t)(rp.data_required() ? DataPhaseInfo::DATAOUT : DataPhaseInfo::NODATA_OR_DATAIN);
            r[pnOperationCode] = (uint16_t)rp.code();
            r[pnTransactionId] = session.transaction_id();
            for(int i=0; i<5; i++) {
                r[pnParams][i] = rp[i];
            }
            r.to_packet(p);
            return _command_sock.send(p);
        }
        
        bool Initiator::send_payload(Session& session, PTP::ByteStream& payload)
        {
            vector<string> p;
            
            StartData s;
            s[pnTransactionId] = session.transaction_id();
            s[pnTotalDataLength] = (uint64_t)payload.length();
            s.to_packet(p);
            if(!_command_sock.send(p)) {
                return false;
            }
            
            // Data not required
            
            string d;
            payload.read(payload.length(), d);
            EndData e;
            e[pnTransactionId] = session.transaction_id();
            e[pnDataPayload] = d;
            e.to_packet(p);
            return _command_sock.send(p);
        }
        
        bool Initiator::recv_payload(Session& session, ResponsePacket& rp, PTP::ByteStream& stream_in)
        {
            /*
            StartData           = 0x0009,
            Data                = 0x000A,
            Cancel              = 0x000B,
            EndData             = 0x000C,
             */
            uint32_t next_phase = 1 << PacketCode::StartData;
            uint64_t require_length = 0;
            uint32_t tid = 0;
            stream_in.clear();
            
            // [ToDo] we are interrupting the received discarded at that point in the case of an invalid packet code, transaction ID.
            while(next_phase!=0) {
                BasePacket* b = _command_sock.receive_packet(_timelimitForResponse);
                if(b==NULL) {
                    LOG_FATAL("Receive payload is timed out.");
                    callbackConnectionEventListener(_command_sock.connected()? _command_sock.getlasterror(): PTPIP_PROTOCOL_SOCKET_CLOSED);
                    return false;
                }
                if(b->id()==PacketCode::OperationResponse) {
                    OperationResponse* r = (OperationResponse*)b;
                    rp.code((uint16_t)(*r)[pnResponseCode]);
                    LOG_INFO("Received OperationResponse(%s). data-in transder is canceled.", rp.name());
                    stream_in.clear();
                    delete b;
                    session.set_last_response_code(rp.code());
                    return false;
                }
                
                uint32_t this_phase = 1 << b->id();
                if((next_phase & this_phase)==0) {
                    LOG_FATAL("Invalid payload receive state. Except %08x but %08x(%d).", next_phase, this_phase, b->id());
                    callbackConnectionEventListener(PTPIP_PROTOCOL_INVALID_RECEIVE_STATE);
                    delete b;
                    return false;
                }
                switch(b->id()) {
                    case PacketCode::StartData:
                        next_phase = (1<<PacketCode::Data) | (1<<PacketCode::Cancel) | (1<<PacketCode::EndData);
                        {
                            StartData* s = (StartData*)b;
                            tid = (*s)[pnTransactionId];
                            if(tid==session.transaction_id()) {
                                require_length = (*s)[pnTotalDataLength];
                                stream_in.start((size_t)require_length);
                            } else {
                                next_phase = 0;
                            }
                        }
                        break;
                    case PacketCode::Data:
                        next_phase = (1<<PacketCode::Data) | (1<<PacketCode::Cancel) | (1<<PacketCode::EndData);
                        {
                            Data* d = (Data*)b;
                            tid = (*d)[pnTransactionId];
                            if(tid==session.transaction_id()) {
                                const string& s = (*d)[pnDataPayload];
                                if(!stream_in.write(s.c_str(), s.size())) {
                                    // cancel.
                                    LOG_INFO("Cancelling inbound payload...");
                                    stream_in.clear();
                                    Cancel c;
                                    c[pnTransactionId] = session.transaction_id();
                                    vector<string> p;
                                    c.to_packet(p);
                                    _command_sock.send(p);
                                    next_phase = 0;
                                }
                            } else {
                                next_phase = 0;
                            }
                        }
                        break;
                    case PacketCode::EndData:
                        next_phase = 0;
                        {
                            EndData* e = (EndData*)b;
                            tid = (*e)[pnTransactionId];
                            if(tid==session.transaction_id()) {
                                const string& s = (*e)[pnDataPayload];
                                stream_in.write(s.c_str(), s.size());
                            }
                        }
                        break;
                    case PacketCode::Cancel:
                        next_phase = 0;
                        {
                            Cancel* c = (Cancel*)b;
                            tid = (*c)[pnTransactionId];
                            stream_in.clear();
                        }
                        break;
                    default:
                        LOG_FATAL("Invalid packet is received (%d).", b->id());
                        callbackConnectionEventListener(PTPIP_PROTOCOL_UNRECOGNIZED_COMMAND);
                        delete b;
                        return false;
                }
                delete b;
            }
            if(tid!=session.transaction_id()) {
                LOG_FATAL("Invalid transaction id. Except %d but %d.", session.transaction_id(), tid);
                callbackConnectionEventListener(PTPIP_PROTOCOL_INVALID_TRANSACTION_ID);
                // return false;
            }
            if((require_length!=(uint64_t)-1) && (require_length!=stream_in.length())) {
                LOG_FATAL("Transfer length mismatch. Except %llu but %llu.", (uint64_t)require_length, (uint64_t)stream_in.length());
                callbackConnectionEventListener(PTPIP_PROTOCOL_INVALID_DATA_LENGTH);
                // return false;
            }
            return true;
        }
        
        
        bool Initiator::recv_operation_response(Session& session, ResponsePacket& rp)
        {
            BasePacket* b = _command_sock.receive_packet(_timelimitForResponse);
            if(b==NULL) {
                LOG_FATAL("Receive response is timed out.");
                callbackConnectionEventListener(_command_sock.connected()? _command_sock.getlasterror(): PTPIP_PROTOCOL_SOCKET_CLOSED);
                return false;
            }
            if(b->id()!=PacketCode::OperationResponse) {
                LOG_FATAL("Invalid response packet. Except %d but %d.", PacketCode::OperationResponse, b->id());
                callbackConnectionEventListener(PTPIP_PROTOCOL_UNRECOGNIZED_COMMAND);
                delete b;
                return false;
            }
            
            OperationResponse* r = (OperationResponse*)b;
            uint32_t tid = (*r)[pnTransactionId];
            if(tid!=session.transaction_id()) {
                LOG_FATAL("Invalid transaction id. Except %d but %d.", session.transaction_id(), tid);
                callbackConnectionEventListener(PTPIP_PROTOCOL_INVALID_TRANSACTION_ID);
                delete b;
                return false;
            }
            
            rp.code((*r)[pnResponseCode]);
            for(int i=0; i<5; i++) {
                rp[i] = (*r)[pnParams][i];
            }
            delete b;
            session.set_last_response_code(rp.code());
            return true;
        }
        
        
        bool Initiator::request_command(Session& session, RequestPacket& rp, PTP::ByteStream* stream_in)
        {
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            Benchmark benchmark(rp.name());
#endif
            session.set_last_response_code(-1);
            LOG_DEBUG("Sending command: %s ...", rp.name());
            if(!send_operation_request(session, rp)) {
                return false;
            }
            
            if(rp.data_required()) {
                LOG_DEBUG("Sending payload...");
                if(rp.payload()==NULL || !send_payload(session, *rp.payload())) {
                    return false;
                }
            }
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            benchmark.lap("SendCommandAndPayload");
#endif
            
            if(rp.receive_data()) {
                LOG_DEBUG("Receiving payload...");
                PTP::ByteStream bs;
                if(!stream_in) {
                    LOG_INFO("*** Dropping payload for %s ***", rp.name());
                    stream_in = &bs;
                }
                if(!recv_payload(session, rp.response(), *stream_in)) {
                    return false;
                }
            }
            
            LOG_DEBUG("Receiving response...");
            if(!recv_operation_response(session, rp.response())) {
                return false;
            }
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            benchmark.end();
#endif
            LOG_DEBUG("Command(%s) response is: %s.", rp.name(), rp.response().name());
            return (rp.response().code()==ResponseCode::OK)? true: false;
        }
        
        
        void Session::start_transaction(const char*file, int line, const char* func)
        {
            g_Logger.info(file, line, 0, "Start transaction for %s: sid=%u, tid=%u.",
                          func, _session_id, _transaction_id);
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            _transactoin_benchmark.begin(func);
#endif
        }
        
        
        void Session::end_transaction(const char*file, int line, const char* func) {
#if defined(__PTPIP_BENCHMARK__) && __PTPIP_BENCHMARK__
            _transactoin_benchmark.end();
#endif
            g_Logger.info(file, line, 0, "End transaction for %s: sid=%u, tid=%u.",
                          func, _session_id, _transaction_id);
            _transaction_id++;
        }
    }
}
