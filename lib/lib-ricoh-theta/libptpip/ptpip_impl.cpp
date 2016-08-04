/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include "ptpip.h"

using namespace PTP::IP;


namespace PTP {
    namespace IP {
        bool Initiator::getDeviceInfo(DeviceInfo_t& result)
        {
            Session z;
            z.start_transaction(__FILE__, __LINE__, __FUNCTION__);

            GetDeviceInfo o;
            PTP::ByteStream b;
            bool res = request_command(z, o, &b);
            if(res) {
                res = result.parse(b);
                if(!res) LOG_WARN("GetDeviceInfo: received data is invalid.");
            }
            
            z.end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Initiator::openSession(Session& s)
        {
            s.init();
            LOG_INFO("Opening session: sid=%u", s.session_id());

            Session z;
            z.start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            OpenSession o(s.session_id());
            bool res = request_command(z, o, NULL);
            s._initiator = NULL;
            if(res) {
                switch(o.response().code()) {
                    case PTP::ResponseCode::SESSION_ALREADY_OPEN:
                        LOG_WARN("PTP::ResponseCode::SESSION_ALREADY_OPEN.");
                        s._initiator = this;
                        break;
                    case PTP::ResponseCode::OK:
                        LOG_INFO("Opened session: sid=%u", s.session_id());
                        s._initiator = this;
                        break;
                    default:
                        LOG_ERROR("Open-session is failed: %s", o.response().name());
                        res = false;
                        break;
                }
            }
            s._opened = res;
            
            z.end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::closeSession()
        {
            if(!_initiator || !_initiator->connected() || _initiator->disconnected()) {
                init();
                return true;
            }
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            CloseSession o(_session_id, _transaction_id);
            bool res = _initiator->request_command(*this, o, NULL);
            if(res) {
                switch(o.response().code()) {
                    case PTP::ResponseCode::SESSION_NOT_OPEN:
                        LOG_WARN("PTP::ResponseCode::SESSION_NOT_OPEN.");
                        break;
                    case PTP::ResponseCode::OK:
                        LOG_INFO("Session closed: sid=%u", _session_id);
                        break;
                    default:
                        LOG_ERROR("CloseSession is failed: %s", o.response().name());
                        res = false;
                        break;
                }
            }
            init();            
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        bool Session::getStorageIDs(vector<uint32_t> &ids)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);

            GetStorageIDs o(_session_id, _transaction_id);
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &b);
            if(res) {
                try {
                    PTP::String::from_s(ids, b);
                } catch(RuntimeError x) {
                    res = false;
                }
            }
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }

        bool Session::getStorageInfo(StorageInfo_t& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetStorageInfo o(_session_id, _transaction_id, PTP::StorageId::DEFAULT);
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &b);
            if(res) {
                res = result.parse(b);
                if(!res) LOG_WARN("GetStorageInfo: received data is invalid.");
            }
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        bool Session::getNumObjects(uint32_t storage_id, uint32_t objectFormatCode, uint32_t parent, uint64_t dateFrom, uint32_t& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetNumObjects o(_session_id, _transaction_id, storage_id, objectFormatCode, parent, (uint32_t)dateFrom, (uint32_t)(dateFrom>>32));
            bool res = _initiator->request_command(*this, o, NULL);
            if(res) {
                result = o.response()[0];
            }
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::getObjectHandles(uint32_t storage_id, uint32_t objectFormatCode, uint32_t parent, uint64_t dateFrom, vector<uint32_t>& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetObjectHandles o(_session_id, _transaction_id, storage_id, objectFormatCode, parent, (uint32_t)dateFrom, (uint32_t)(dateFrom>>32));
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &b);
            if(res) {
                try {
                    PTP::String::from_s(result, b);
                } catch(RuntimeError x) {
                    res = false;
                }
            }
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::getObjectInfo(uint32_t objectHandle, ObjectInfo_t& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetObjectInfo o(_session_id, _transaction_id, objectHandle);
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &b);
            if(res) {
                res = result.parse(b);
                if(!res) LOG_WARN("GetObjectInfo: received data is invalid.");
            }
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::getObject(uint32_t objectHandle, PTP::ByteStream& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetObject o(_session_id, _transaction_id, objectHandle);
            bool res = _initiator->request_command(*this, o, &result);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res && (result.length()>0);
        }
        
        
        bool Session::getThumb(uint32_t objectHandle, PTP::ByteStream& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetThumb o(_session_id, _transaction_id, objectHandle);
            bool res = _initiator->request_command(*this, o, &result);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res && (result.length()>0);
        }
        
        
        bool Session::deleteObject(uint32_t objectHandle, uint32_t objectFormatCode)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            DeleteObject o(_session_id, _transaction_id, objectHandle, objectFormatCode);
            bool res = _initiator->request_command(*this, o, NULL);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        bool Session::initiateCapture(uint32_t storage_id, uint32_t objectFormatCode)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            InitiateCapture o(_session_id, _transaction_id, storage_id, objectFormatCode);
            bool res = _initiator->request_command(*this, o, NULL);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }

        bool Session::getDevicePropDesc(uint16_t devicePropCode, PTP::ByteStream& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetDevicePropDesc o(_session_id, _transaction_id, devicePropCode);
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &result);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }

        bool Session::getDevicePropValue(uint16_t devicePropCode, PTP::ByteStream& result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetDevicePropValue o(_session_id, _transaction_id, devicePropCode);
            PTP::ByteStream b;
            bool res = _initiator->request_command(*this, o, &result);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::setDevicePropValue(uint16_t devicePropCode, const std::string& value)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            SetDevicePropValue o(_session_id, _transaction_id, devicePropCode);
            PTP::ByteStream b;
            b.write(value.c_str(), value.size());
            o.payload(&b);
            bool res = _initiator->request_command(*this, o, NULL);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        
        bool Session::terminateOpenCapture(uint32_t transaction_id)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            TerminateOpenCapture o(_session_id, _transaction_id, transaction_id);
            bool res = _initiator->request_command(*this, o, NULL);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }

        bool Session::initiateOpenCapture(uint32_t storage_id, uint32_t objectFormatCode, uint32_t& transaction_id)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            InitiateOpenCapture o(_session_id, _transaction_id, storage_id, objectFormatCode);
            bool res = _initiator->request_command(*this, o, NULL);
            if(res) transaction_id = _transaction_id;
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        // [v1.1 0x1022]
        bool Session::getResizedImageObject(uint32_t objectHandle, uint32_t width, uint32_t height, PTP::ByteStream &result)
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            GetResizedImageObject o(_session_id, _transaction_id, objectHandle, width, height);
            bool res = _initiator->request_command(*this, o, &result);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
        
        // [v1.5 0x99A1]
        bool Session::wlanPowerControl()
        {
            if(!_initiator || !_initiator->connected()) return false;
            start_transaction(__FILE__, __LINE__, __FUNCTION__);
            
            WlanPowerControl o(_session_id, _transaction_id);
            bool res = _initiator->request_command(*this, o, NULL);
            
            end_transaction(__FILE__, __LINE__, __FUNCTION__);
            return res;
        }
    }

    const uint16_t DEVICEPROP_TYPE_MAP[] = {
        PTPDP_BATTERY_LEVEL               , PTP::UINT8::SYM,  // UINT8,  Enum/Range, R
        PTPDP_FUNCTIONAL_MODE             , PTP::UINT16::SYM, // UINT16, Enum, R/W
        PTPDP_IMAGE_SIZE                  , PTP::STRING::SYM, // String, Enum/Range, R/W
        PTPDP_COMPRESSION_SETTING         , PTP::UINT8::SYM,  // UINT8,  Enum/Range, R/W
        PTPDP_WHITE_BALANCE               , PTP::UINT16::SYM, // UINT16, Enum, R/W, WhiteBalanceMode
        PTPDP_RGB_GAIN                    , PTP::STRING::SYM, // String, Enum/Range, R/W
        PTPDP_F_NUMBER                    , PTP::UINT16::SYM, // UINT16, Enum, R/W
        PTPDP_FOCAL_LENGTH                , PTP::UINT32::SYM, // UINT32, Enum/Range, R/W
        PTPDP_FOCUS_DISTANCE              , PTP::UINT16::SYM, // UINT16, Enum/Range, R/W
        PTPDP_FOCUS_MODE                  , PTP::UINT16::SYM, // UINT16, Enum, R/W, FocusMode
        PTPDP_FLASH_MODE                  , PTP::UINT16::SYM, // UINT16, Enum, R/W, FlashMode
        PTPDP_EXPOSURE_TIME               , PTP::UINT32::SYM, // UINT32, Enum/Range, R/W
        PTPDP_EXPOSURE_PROGRAM_MODE       , PTP::UINT16::SYM, // UINT16, Enum, R/W, ExposureProgramMode
        PTPDP_EXPOSURE_INDEX              , PTP::UINT16::SYM, // UINT16, Enum/Range, R/W
        PTPDP_EXPOSURE_BIAS_COMPENSATION  , PTP::INT16::SYM,  // INT16, Enum/Range, R/W
        PTPDP_DATE_TIME                   , PTP::DATETIME::SYM, // String, R/W, DateTime
        PTPDP_CAPTURE_DELAY               , PTP::UINT32::SYM, // UINT32, Enum/Range, R/W
        PTPDP_STILL_CAPTURE_MODE          , PTP::UINT16::SYM, // UINT16, Enum, R/W, StillCaptureMode
        PTPDP_CONTRAST                    , PTP::UINT8::SYM,  // UINT8,  Enum/Range, R/W
        PTPDP_SHARPNESS                   , PTP::UINT8::SYM,  // UINT8,  Enum/Range, R/W
        PTPDP_DIGITAL_ZOOM                , PTP::UINT8::SYM,  // UINT8,  Enum/Range
        PTPDP_EFFECT_MODE                 , PTP::UINT16::SYM, // UINT16, Enum, R/W, EffectMode
        PTPDP_BURST_NUMBER                , PTP::UINT16::SYM, // UINT16, Enum/Range, R/W
        PTPDP_BURST_INTERVAL              , PTP::UINT16::SYM, // UINT16, Enum/Range, R/W
        PTPDP_TIMELAPSE_NUMBER            , PTP::UINT16::SYM, // UINT16, Enum/Range, R/W
        PTPDP_TIMELAPSE_INTERVAL          , PTP::UINT32::SYM, // UINT32, Enum/Range, R/W
        PTPDP_FOCUS_METERING_MODE         , PTP::UINT16::SYM, // UINT16, Enum, R/W. FocusMeteringMode
        PTPDP_UPLOAD_URL                  , PTP::STRING::SYM, // String, R/W
        PTPDP_ARTIST                      , PTP::STRING::SYM, // String, R/W
        PTPDP_COPYRIGHT_INFO              , PTP::STRING::SYM, // String, R/W
      
        // (1.1)
        PTPDP_AUDIO_VOLUME                , PTP::UINT32::SYM, // UINT32, Enum/Range, R/W

        // Vendor-extended property
        PTPDP_ERROR_INFO                  , PTP::UINT32::SYM, // UINT32, Bitmap, R
        PTPDP_SHUTTER_SPEED               , PTP::UINT64::SYM, // UINT64(UINT32x2 rational) R/W
        PTPDP_GPS_INFO                    , PTP::STRING::SYM, // String, R/W
        PTPDP_AUTO_POWEROFF_DELAY         , PTP::UINT8::SYM,  // UINT8, Range(0..30), R/W
        PTPDP_SLEEP_DELAY                 , PTP::UINT16::SYM, // UINT16, Range(0..1800), R/W
        PTPDP_CHANNEL_NUMBER              , PTP::UINT8::SYM, // UINT8, Range(0, 1, 6, 11), R/W
        PTPDP_CAPTURE_STATUS              , PTP::UINT8::SYM,  // UINT8, Range(0/1), R
        PTPDP_RECORDING_TIME              , PTP::UINT16::SYM, // UINT16, Range(0..179), R
        PTPDP_REMAINING_RECORDING_TIME    , PTP::UINT16::SYM, // UINT16, Range(0..180), R
        
        // centinel
        0, 0
    };

    uint16_t getDevicePropType(uint16_t devicePropCode)
    {
      const uint16_t* p = DEVICEPROP_TYPE_MAP;
      for(; p[0]!=0 ; p+=2) {
        if(p[0]==devicePropCode) return p[1];
      }
      return 0;
    }
}
