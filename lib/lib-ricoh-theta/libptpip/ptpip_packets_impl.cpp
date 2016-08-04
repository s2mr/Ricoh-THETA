/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include "ptpip_packets.h"


namespace PTP {
    namespace IP {
        
        // ==> InitCommandRequest
        InitCommandRequest::InitCommandRequest(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnGUID].type(ptGUID);
            _params[pnFriendlyName].type(ptSTRING);
            _params[pnProtocolVersion].type(ptUINT32);
            _params[pnProtocolVersion] = (uint32_t)PTP::IP::PROTOCOL_VERSION;
        }
        
        const PARAM_NAME* InitCommandRequest::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnGUID, pnFriendlyName, pnProtocolVersion, pnEND
            };
            return ary;
        }
        
        
        // <== InitCommandAck
        InitCommandAck::InitCommandAck(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnConnectionNumber].type(ptUINT32);
            _params[pnGUID].type(ptGUID);
            _params[pnFriendlyName].type(ptSTRING);
            _params[pnProtocolVersion].type(ptUINT32);
        }
        
        const PARAM_NAME* InitCommandAck::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnConnectionNumber, pnGUID, pnFriendlyName, pnProtocolVersion, pnEND
            };
            return ary;
        }
        
        
        // ==> InitEventRequest
        InitEventRequest::InitEventRequest(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnConnectionNumber].type(ptUINT32);
        }
        
        const PARAM_NAME* InitEventRequest::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnConnectionNumber, pnEND
            };
            return ary;
        }
        
        
        // <== InitEventAck
        InitEventAck::InitEventAck(size_t packet_length) : BasePacket(packet_length)
        {
        }
        
        const PARAM_NAME* InitEventAck::param_list() const
        {
            static const PARAM_NAME ary[] = { pnEND };
            return ary;
        }
        
        
        // <== InitFail
        InitFail::InitFail(size_t packet_length)
        {
            _params[pnReasonOfFail].type(ptUINT32);
        }
        
        const PARAM_NAME* InitFail::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnReasonOfFail, pnEND
            };
            return ary;
        }
        
        
        // ==> OperationRequest
        OperationRequest::OperationRequest(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnDataPhaseInfo].type(ptUINT32);
            _params[pnOperationCode].type(ptUINT16);
            _params[pnTransactionId].type(ptUINT32);
            _params[pnParams].type(ptUINT32_VECTOR_5);
        }
        
        const PARAM_NAME* OperationRequest::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnDataPhaseInfo, pnOperationCode, pnTransactionId, pnParams, pnEND
            };
            return ary;
        }
        
        
        // <== OperationResponse
        OperationResponse::OperationResponse(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnResponseCode].type(ptUINT16);
            _params[pnTransactionId].type(ptUINT32);
            _params[pnParams].type(ptUINT32_VECTOR_5);
        }
        
        const PARAM_NAME* OperationResponse::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnResponseCode, pnTransactionId, pnParams, pnEND
            };
            return ary;
        }
        
        
        // <== Event
        Event::Event(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnEventCode].type(ptUINT16);
            _params[pnTransactionId].type(ptUINT32);
            _params[pnParams].type(ptUINT32_VECTOR_3);
        }
        
        const PARAM_NAME* Event::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnEventCode, pnTransactionId, pnParams, pnEND
            };
            return ary;
        }
        
        
        // <== StartData
        StartData::StartData(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnTransactionId].type(ptUINT32);
            _params[pnTotalDataLength].type(ptUINT64);
        }
        
        const PARAM_NAME* StartData::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnTransactionId, pnTotalDataLength, pnEND
            };
            return ary;
        }
        
        
        // <== Data
        Data::Data(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnTransactionId].type(ptUINT32);
            _params[pnDataPayload].type(ptBINARY);
            _params[pnDataPayload].required_length(packet_length - 12); // header(4+4) + transactionId(4)
        }
        
        const PARAM_NAME* Data::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnTransactionId, pnDataPayload, pnEND
            };
            return ary;
        }
        
        
        // <==> Cancel
        Cancel::Cancel(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnTransactionId].type(ptUINT32);
        }
        
        const PARAM_NAME* Cancel::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnTransactionId, pnEND
            };
            return ary;
        }
        
        
        // <== EndData
        EndData::EndData(size_t packet_length) : BasePacket(packet_length)
        {
            _params[pnTransactionId].type(ptUINT32);
            _params[pnDataPayload].type(ptBINARY);
            _params[pnDataPayload].required_length(packet_length - 12); // header(4+4) + transactionId(4)
        }
        
        const PARAM_NAME* EndData::param_list() const
        {
            static const PARAM_NAME ary[] = {
                pnTransactionId, pnDataPayload, pnEND
            };
            return ary;
        }
        
        
        // ==> ProbeRequest
        ProbeRequest::ProbeRequest(size_t packet_length) : BasePacket(packet_length)
        {
        }
        
        const PARAM_NAME* ProbeRequest::param_list() const
        {
            static const PARAM_NAME ary[] = { pnEND };
            return ary;
        }
        
        
        // <== ProbeResponse
        ProbeResponse::ProbeResponse(size_t packet_length) : BasePacket(packet_length)
        {
        }
        
        const PARAM_NAME* ProbeResponse::param_list() const
        {
            static const PARAM_NAME ary[] = { pnEND };
            return ary;
        }

    }
}