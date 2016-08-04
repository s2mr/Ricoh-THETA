/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_bufferedsock_h
#define ptpip_initiator_bufferedsock_h

#include <errno.h>
#include <string>
#include <vector>
#include "bytestream.h"
#include "ptpip_packets.h"

using namespace std;

namespace PTP {
    namespace IP {
        class BufferedSocket {
        protected:
            int         _sock;
            string      _server;
            uint16_t    _port;
            PTP::ByteStream _buf;
            bool        _eof;
            bool recv_to_buffer();
            
            void set_error() { set_error(errno); }
            void set_error(int err);
            int         _err;
            string      _errstr;
            
        public:
            BufferedSocket();
            inline virtual ~BufferedSocket() { close(); }
            bool connect(const char* server, int port);
            void close();
            bool connected() const { return (_sock != -1) && (!_eof); }
            const string& getlasterror_str() const { return _errstr; }
            int getlasterror() const { return _err; }
            bool send(const char* data, size_t length);
            bool send(const string& data);
            bool send(const vector<string>& data);
            BasePacket* receive_packet(time_t timeLimit = 3);
            
            void clear() { _buf.clear(); }
            size_t length() __attribute__((deprecated));
            void read(size_t length, string& ret) __attribute__((deprecated));
        };
    }
}

#endif