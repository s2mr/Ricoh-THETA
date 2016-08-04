/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include "exceptions.h"
#include "bufferedsock.h"
#include "logger.h"


namespace PTP {
    namespace IP {
        
        BufferedSocket::BufferedSocket()
        {
            _sock = -1;
            _err = 0;
            _errstr = "";
            _port = 0;
            _eof = false;
            _server.clear();
        }
        
        void BufferedSocket::set_error(int err)
        {
            char buf[128];
            _err = err;
            ::strerror_r(_err, buf, sizeof(buf));
            _errstr.assign(buf);
            snprintf(buf, sizeof(buf), "(%d)", _err);
            _errstr.append(buf);
        }
        
        bool BufferedSocket::connect(const char* server, int port)
        {
            if(_sock>=0) {
                LOG_ERROR("Socket is opened.");
                return false;
            }
            _server.assign(server);
            _port = port;
            _buf.clear();
            _eof = false;
            
            struct sockaddr_in saddr;
            memset(&saddr, 0, sizeof(saddr));
            
            _sock = ::socket(PF_INET, SOCK_STREAM, 0);
            int sockopt = ::fcntl(_sock, F_GETFL);
            ::fcntl(_sock, F_SETFL, O_NONBLOCK);
            saddr.sin_port = htons(_port);
            saddr.sin_family = AF_INET;
            saddr.sin_addr.s_addr = ::inet_addr(_server.c_str());
            
            if(::connect(_sock, (const struct ::sockaddr*)&saddr, sizeof(saddr))>=0) {
                return true;
            }
            set_error();
            if(_err!=EINPROGRESS) {
                close();
                LOG_ERROR_WITH_ERRNO(_err, "connect() failed.");
                return false;
            }
            ::fcntl(_sock, F_SETFL, sockopt&(~O_NONBLOCK));
            int on = 1;
#ifdef SO_NOSIGPIPE
            ::setsockopt(_sock, SOL_SOCKET, SO_NOSIGPIPE, (void*)&on, sizeof(on)); // alternative send(MSG_NOSIGNAL)
#endif
            while(true) {
                struct timeval tmout = {0, 0};
                tmout.tv_sec = 2;
                fd_set connectfds;
                FD_ZERO(&connectfds);
                FD_SET(_sock, &connectfds);
                int r = ::select(_sock+1, NULL, &connectfds, NULL, &tmout);
                if(r<0) {
                    set_error();
                    if((_err==EAGAIN)||(_err==EINTR)) continue;
                    close();
                    LOG_ERROR_WITH_ERRNO(_err, "select()@connect is failed.");
                    return false;
                } else if(r>0) {
                    if(::connect(_sock, (const struct ::sockaddr*)&saddr, sizeof(saddr))<0) {
                        set_error();
                        if(_err!=EISCONN) {
                            close();
                            LOG_ERROR_WITH_ERRNO(_err, "connect()#2 failed.");
                            return false;
                        }
                    }
                }
                break;
            }
            return true;
        }
        
        void BufferedSocket::close()
        {
            if(_sock>=0) ::close(_sock);
            _sock = -1;
            _eof = false;
        }
        
        bool BufferedSocket::recv_to_buffer()
        {
            struct timeval tmout = {0, 0};
            tmout.tv_usec = 100;
            fd_set recvfds;
            char buf[2048];
            volatile int sock = _sock;
            
            if(_eof) return false;
            while(sock>=0) {
                FD_ZERO(&recvfds);
                FD_SET(sock, &recvfds);
                _err = 0;
                if(::select(sock+1, &recvfds, NULL, NULL, &tmout)<0) {
                    set_error();
                    LOG_DEBUG("select()@recv is failed. %s", _errstr.c_str());
                    if((_err==EAGAIN)||(_err==EINTR)) continue;
                    if((_err==EBADF)||(_err==ENOTCONN)) {
                        _eof = true;
                        return false;
                    }
                    if(_err!=0) return false;
                };
                if(!FD_ISSET(sock, &recvfds)) return false;
                
                ssize_t len = ::recv(sock, buf, sizeof(buf), 0);
                if(len<0) {
                    set_error();
                    LOG_DEBUG("recv() is failed. %s", _errstr.c_str());
                    if((_err==EAGAIN)||(_err==EINTR)) continue;
                    if((_err==EBADF)||(_err==ENOTCONN)) {
                        _eof = true;
                        return false;
                    }
                    if(_err!=0) {
                        return false;
                    }
                } else if(len==0) {
                    LOG_DEBUG("recv() is read 0 bytes ... TCP peer has closed.");
                    _eof = true;
                    return false;
                } else {
                    LOG_VERBOSE("recv() %u bytes received.", len);
                    _buf.write(buf, len);
                    continue;
                }
                break;
            }
            return true;
        }

        bool BufferedSocket::send(const char* data, size_t length)
        {
            volatile int sock = _sock;
            if(sock>=0) {
                if(sock==0) {
                    LOG_FATAL("Invalid sock.`0`.");
                }
                _err = 0;
                LOG_DEBUG("Sending packet(%d): length=%u", sock, length);
                LOG_DUMP(data, length);
                if(::send(sock, data, length, 0)<0) {
                    set_error();
                    return false;
                }
                return true;
            }
            return false;
        }
        bool BufferedSocket::send(const string& data)
        {
            return send(data.c_str(), data.size());
        }
        bool BufferedSocket::send(const vector<string>& data)
        {
            vector<string>::const_iterator it = data.begin();
            for(; it!=data.end(); it++) {
                if(!send(*it)) return false;
            }
            return true;
        }
        
        size_t BufferedSocket::length() //__attribute__((deprecated))
        {
            recv_to_buffer();
            return _buf.length();
        }
        
        void BufferedSocket::read(size_t length, string& ret) //__attribute__((deprecated))
        {
            recv_to_buffer();
            _buf.read(length, ret);
        }

        BasePacket* BufferedSocket::receive_packet(time_t timeLimit)
        {
            BasePacket* bp = NULL;
            time_t limit = time(NULL) + timeLimit;
            
            while(::time(NULL)<=limit) {
                if(recv_to_buffer()) {
                    // limit = time(NULL) + timeLimit; // reset-timer
                }
                try {
                    bp = parse(_buf);
                } catch(...) {}
                if(bp) return bp;
                if(_eof) return NULL;
            }
            set_error(ETIMEDOUT);
            return NULL;
        }
    }
}
