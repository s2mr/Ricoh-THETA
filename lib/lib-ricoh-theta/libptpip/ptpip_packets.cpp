/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include <string.h>
#include "ptpip_packets.h"
#include "logger.h"


namespace PTP {
    namespace IP {
        const GUID& GUID::operator=(const GUID& v)
        {
            v1 = v.v1;
            v2 = v.v2;
            v3 = v.v3;
            v4 = v.v4;
            v5 = v.v5;
            return *this;
        }
        
        void GUID::clear()
        {
            v1 = 0;
            v2 = 0;
            v3 = 0;
            v4 = 0;
            v5 = 0;
        }

        void GUID::assign(const char* v)
        {
            // "8423BB09-13CB-4CEF-8D24-D38A647FBBD3"
            if(sscanf(v, "%08x-%04x-%04x-%04x-%12llx", &v1, &v2, &v3, &v4, &v5)!=5) {
                throw RUNTIME_ERROR("Bad string for GUID.");
            }
        }
        
        void GUID::to_str(string& ret) const
        {
            char buf[32+4+1];
            snprintf(buf, sizeof(buf), "%08X-%04X-%04X-%04X-%012llX", v1, v2, v3, v4, v5);
            ret.assign(buf);
        }
        
        void GUID::to_packet(string& ret) const
        {
            ret.assign(16, 0);
            ret.at(0) = v1 >> 24;
            ret.at(1) = v1 >> 16;
            ret.at(2) = v1 >>  8;
            ret.at(3) = v1;
            ret.at(4) = v2 >> 8;
            ret.at(5) = v2;
            ret.at(6) = v3 >> 8;
            ret.at(7) = v3;
            ret.at(8) = v4 >> 8;
            ret.at(9) = v4;
            ret.at(10) = v5 >> 40;
            ret.at(11) = v5 >> 32;
            ret.at(12) = v5 >> 24;
            ret.at(13) = v5 >> 16;
            ret.at(14) = v5 >> 8;
            ret.at(15) = v5;
        }
        
        void GUID::from_s(GUID& ret, string& stream)
        {
            if(stream.size()!=16) throw STANDARD_ERROR("Bad stream for GUID.", 0);
            ret.v1 = ((uint32_t)(uint8_t)stream[0] << 24) | ((uint32_t)(uint8_t)stream[1] << 16) \
                   | ((uint32_t)(uint8_t)stream[2] << 8) | (uint32_t)(uint8_t)stream[3];
            ret.v2 = ((uint16_t)(uint8_t)stream[4] << 8) | (uint16_t)(uint8_t)stream[5];
            ret.v3 = ((uint16_t)(uint8_t)stream[6] << 8) | (uint16_t)(uint8_t)stream[7];
            ret.v4 = ((uint16_t)(uint8_t)stream[8] << 8) | (uint16_t)(uint8_t)stream[9];
            ret.v5 = ((uint64_t)(uint8_t)stream[10] << 40) | ((uint64_t)(uint8_t)stream[11] << 32) \
                   | ((uint64_t)(uint8_t)stream[12] << 24) | ((uint64_t)(uint8_t)stream[13] << 16) \
                   | ((uint64_t)(uint8_t)stream[14] << 8) | (uint64_t)(uint8_t)stream[15];
        }
        
        Parameter::Parameter()
        {
            _type = ptUNDEF;
            _u64 = 0;
            _g.clear();
            _s.clear();
            _v32v.clear();
        }
        
        Parameter::Parameter(PARAM_TYPE t)
        {
            _type = t;
            _u64 = 0;
            _v32v.clear();
        }
        
        void Parameter::assign(uint16_t v)
        {
            _type = ptUINT16;
            _u16 = v;
        }
        
        void Parameter::assign(uint32_t v)
        {
            _type = ptUINT32;
            _u32 = v;
        }
        
        void Parameter::assign(uint64_t v)
        {
            _type = ptUINT64;
            _u64 = v;
        }
        
        void Parameter::assign(const GUID& v)
        {
            _type = ptGUID;
            _g = v;
        }
        
        void Parameter::assign(const string& v)
        {
            _type = ptSTRING;
            _s.assign(v);
        }
        
        void Parameter::assign(const char*v, size_t length)
        {
            _type = ptBINARY;
            _s.assign(v, length);
        }
        
        void Parameter::assign(uint32_t v1, uint32_t v2, uint32_t v3)
        {
            _type = ptUINT32_VECTOR_3;
            _v32v.assign(3, 0);
            _v32v.at(0) = v1;
            _v32v.at(1) = v2;
            _v32v.at(2) = v3;
        }
        
        void Parameter::assign(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4, uint32_t v5)
        {
            _type = ptUINT32_VECTOR_5;
            _v32v.assign(5, 0);
            _v32v.at(0) = v1;
            _v32v.at(1) = v2;
            _v32v.at(2) = v3;
            _v32v.at(3) = v4;
            _v32v.at(4) = v5;
        }
        
        void Parameter::type(PARAM_TYPE t)
        {
            if(_type!=ptUNDEF) throw RUNTIME_ERROR("Parameter type is fixed.");
            _type = t;
            switch(_type) {
                case ptUINT32_VECTOR_3:
                    _v32v.assign(3, 0);
                    break;
                case ptUINT32_VECTOR_5:
                    _v32v.assign(5 ,0);
                    break;
                default:
                    _v32v.clear();
                    break;
            }
        }

        uint16_t& Parameter::operator=(const uint16_t& v)
        {
            if(_type!=ptUINT16) throw RUNTIME_ERROR("Invalid type.");
            _u16 = v;
            return _u16;
        }
        
        uint32_t& Parameter::operator=(const uint32_t& v)
        {
            if(_type!=ptUINT32) throw RUNTIME_ERROR("Invalid type.");
            _u32 = v;
            return _u32;
        }
        
        uint64_t& Parameter::operator=(const uint64_t& v)
        {
            if(_type!=ptUINT64) throw RUNTIME_ERROR("Invalid type.");
            _u64 = v;
            return _u64;
        }
        
        GUID& Parameter::operator=(const GUID& v)
        {
            if(_type!=ptGUID) throw RUNTIME_ERROR("Invalid type.");
            _g = v;
            return _g;
        }
        
        string& Parameter::operator=(const string& v)
        {
            if(_type==ptSTRING) {
                _s = v;
            } else if(_type==ptBINARY) {
                _s.assign(v.c_str(), v.size());
            } else {
                throw RUNTIME_ERROR("Invalid type.");
            }
            return _s;
        }
        
        uint32_t& Parameter::operator[](int index)
        {
            switch(_type) {
                case ptUINT32_VECTOR_3:
                case ptUINT32_VECTOR_5:
                    if((index<0) || (index>=(int)_v32v.size())) throw RUNTIME_ERROR("Bad index.");
                    break;
                default:
                    throw RUNTIME_ERROR("Invalid type");
            }
            return _v32v.at(index);
        }
        
        Parameter::operator uint16_t() const
        {
            if(_type!=ptUINT16) throw RUNTIME_ERROR("Invalid cast.");
            return _u16;
        }
        
        Parameter::operator uint32_t() const
        {
            if(_type!=ptUINT32) throw RUNTIME_ERROR("Invalid cast.");
            return _u32;
        }
        
        Parameter::operator uint64_t() const
        {
            if(_type!=ptUINT64) throw RUNTIME_ERROR("Invalid cast.");
            return _u64;
        }
        
        Parameter::operator const GUID&() const
        {
            if(_type!=ptGUID) throw RUNTIME_ERROR("Invalid cast.");
            return _g;
        }
        
        Parameter::operator const string&() const
        {
            if(_type==ptGUID) return _g;
            if((_type!=ptSTRING) && (_type!=ptBINARY))
                throw RUNTIME_ERROR("Invalid cast.");
            return _s;
        }
        
        Parameter::operator const vector<uint32_t>&() const
        {
            switch(_type) {
                case ptUINT32_VECTOR_3:
                case ptUINT32_VECTOR_5:
                    return _v32v;
                default:
                    throw RUNTIME_ERROR("Invalid cast.");
            }
        }

        void Parameter::to_packet(string& ret) const
        {
            switch(_type) {
                case ptUINT16:
                    PTP::String::to_s(_u16, ret);
                    return;
                case ptUINT32:
                    PTP::String::to_s(_u32, ret);
                    return;
                case ptUINT64:
                    PTP::String::to_s(_u64, ret);
                    return;
                case ptSTRING:
                    PTP::String::utf8_to_ptp(_s, ret);
                    return;
                case ptBINARY:
                    ret = _s;
                    return;
                case ptGUID:
                    _g.to_packet(ret);
                    return;
                case ptUINT32_VECTOR_3:
                case ptUINT32_VECTOR_5:
                    ret.clear();
                    for(int i=0; i<(int)_v32v.size(); i++) {
                        string t;
                        PTP::String::to_s(_v32v.at(i), t);
                        ret.append(t);
                    }
                    return;
                case ptUNDEF:
                default:
                    throw STANDARD_ERROR("Invalid types to to_packet().");
            }
        }
        
        void Parameter::parse(PTP::ByteStream& stream)
        {
            switch(_type) {
                case ptUINT16:
                    PTP::String::from_s(_u16, stream);
                    return;
                case ptUINT32:
                    PTP::String::from_s(_u32, stream);
                    return;
                case ptUINT64:
                    PTP::String::from_s(_u64, stream);
                    return;
                case ptSTRING:
                    PTP::String::to_unicodez_str(stream, _s);
                    return;
                case ptBINARY:
                    stream.read(_required_length, _s);
                    return;
                case ptGUID:
                    {
                        string t;
                        stream.read(16, t);
                        GUID::from_s(_g, t);
                    }
                    return;
                case ptUINT32_VECTOR_3:
                    _v32v.assign(3, 0);
                    stream.read(_v32v.at(0));
                    stream.read(_v32v.at(1));
                    stream.read(_v32v.at(2));
                    return;
                case ptUINT32_VECTOR_5:
                    _v32v.assign(5, 0);
                    stream.read(_v32v.at(0));
                    stream.read(_v32v.at(1));
                    stream.read(_v32v.at(2));
                    stream.read(_v32v.at(3));
                    stream.read(_v32v.at(4));
                    return;
                case ptUNDEF:
                default:
                    throw STANDARD_ERROR("Invalid types to parse().");
            }
        }
        
        
        void BasePacket::parse(PTP::ByteStream &stream)
        {
            const PARAM_NAME* ary = param_list();
            for(int i = 0; ary[i]!=pnEND; i++) {
                Parameter* p = &(*this)[ary[i]];
                p->parse(stream);
            }
        }
        
        Parameter& BasePacket::operator[](PARAM_NAME param_name) const
        {
            map<PARAM_NAME, Parameter>::const_iterator it = _params.find(param_name);
            if(it==_params.end()) {
                g_Logger.fatal(__FILE__, __LINE__, 0, "%s: Invalid parameter index: %d. availables are:", __FUNCTION__, param_name);
                int i;
                for(i=1, it=_params.begin(); it!=_params.end(); i++, it++) {
                    g_Logger.fatal(NULL, i, 0, "  %d", (*it).first);
                }
                throw STANDARD_ERROR("Invalid parameter inxex.");
            }
            return *(const_cast<Parameter*>(&(*it).second));
        }

        void BasePacket::to_packet(vector<string>& ret) const
        {
            ret.assign(1, "");
            const PARAM_NAME* ary = param_list();
            size_t len = 0;
            
            for(int i = 0; ary[i]!=pnEND; i++) {
                string t;
                (*this)[ary[i]].to_packet(t);
                len += t.size();
                ret.push_back(t);
            }
            
            string h;
            PTP::String::to_s((uint32_t)(8+len), h);
            string i;
            PTP::String::to_s((uint32_t)id(), i);
            h.append(i);
            ret.at(0).assign(h);
        }
        
        BasePacket* parse(PTP::ByteStream& stream)
        {
            uint32_t length = 0, code = 0;
            BasePacket* res;
            
            if(!stream.peek(length)) return NULL;
            if(stream.length()<length) return NULL;
            
            LOG_VERBOSE("Receiving packet: length=%u", length);
            LOG_DUMP_V(stream.peek(), length);
            stream.read(length);
            stream.read(code);
            switch(code) {
                case PacketCode::InitCommandRequest:
                    res = new InitCommandRequest;
                    break;
                case PacketCode::InitCommandAck:
                    res = new InitCommandAck;
                    break;
                case PacketCode::InitEventRequest:
                    res = new InitEventRequest;
                    break;
                case PacketCode::InitEventAck:
                    res = new InitEventAck;
                    break;
                case PacketCode::InitFail:
                    res = new InitFail;
                    break;
                case PacketCode::OperationRequest:
                    res = new OperationRequest;
                    break;
                case PacketCode::OperationResponse:
                    res = new OperationResponse;
                    break;
                case PacketCode::Event:
                    res = new Event;
                    break;
                case PacketCode::StartData:
                    res = new StartData;
                    break;
                case PacketCode::Data:
                    res = new Data(length);
                    break;
                case PacketCode::Cancel:
                    res = new Cancel;
                    break;
                case PacketCode::EndData:
                    res = new EndData(length);
                    break;
                case PacketCode::ProbeRequest:
                    res = new ProbeRequest;
                    break;
                case PacketCode::ProbeResponse:
                    res = new ProbeResponse;
                    break;
                default:
                    throw RUNTIME_ERROR("Invalid PTP/IP Packet code.");
            }
            res->parse(stream);
            return res;
        }
    }
}
