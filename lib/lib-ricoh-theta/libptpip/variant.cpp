/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#include "variant.h"
#include "logger.h"


namespace PTP {
    void Variant::to_ptp(string& ret)
    {
#define DEFINE_TO_PTP_CASE(_T_) case _T_::SYM: { u._##_T_->to_ptp(ret); } break
        switch(_sym) {
            DEFINE_TO_PTP_CASE(INT8);
            DEFINE_TO_PTP_CASE(INT16);
            DEFINE_TO_PTP_CASE(INT32);
            DEFINE_TO_PTP_CASE(INT64);
            DEFINE_TO_PTP_CASE(INT128);
            DEFINE_TO_PTP_CASE(UINT16);
            DEFINE_TO_PTP_CASE(UINT32);
            DEFINE_TO_PTP_CASE(UINT64);
            DEFINE_TO_PTP_CASE(UINT128);
            DEFINE_TO_PTP_CASE(AINT16);
            DEFINE_TO_PTP_CASE(AINT32);
            DEFINE_TO_PTP_CASE(AINT64);
            DEFINE_TO_PTP_CASE(AINT128);
            DEFINE_TO_PTP_CASE(AUINT16);
            DEFINE_TO_PTP_CASE(AUINT32);
            DEFINE_TO_PTP_CASE(AUINT64);
            DEFINE_TO_PTP_CASE(AUINT128);
            case DeviceInfo_t::SYM:
            case ObjectInfo_t::SYM:
            case StorageInfo_t::SYM:
            case Binary_t::SYM:
                throw STANDARD_ERROR("Not supported.");
                break;
            default:
                if(u._void!=NULL) throw RUNTIME_ERROR("Invalid _sym.");
                break;
        }
#undef DEFINE_TO_PTP_CASE
    }
    
    
    void Variant::remove()
    {
#define DEFINE_DELETE_CASE(_T_) case _T_::SYM: { delete u._##_T_; } break
        switch(_sym) {
            DEFINE_DELETE_CASE(INT8);
            DEFINE_DELETE_CASE(INT16);
            DEFINE_DELETE_CASE(INT32);
            DEFINE_DELETE_CASE(INT64);
            DEFINE_DELETE_CASE(INT128);
            DEFINE_DELETE_CASE(UINT8);
            DEFINE_DELETE_CASE(UINT16);
            DEFINE_DELETE_CASE(UINT32);
            DEFINE_DELETE_CASE(UINT64);
            DEFINE_DELETE_CASE(UINT128);
            DEFINE_DELETE_CASE(AINT8);
            DEFINE_DELETE_CASE(AINT16);
            DEFINE_DELETE_CASE(AINT32);
            DEFINE_DELETE_CASE(AINT64);
            DEFINE_DELETE_CASE(AINT128);
            DEFINE_DELETE_CASE(AUINT8);
            DEFINE_DELETE_CASE(AUINT16);
            DEFINE_DELETE_CASE(AUINT32);
            DEFINE_DELETE_CASE(AUINT64);
            DEFINE_DELETE_CASE(AUINT128);
            DEFINE_DELETE_CASE(STRING);
            DEFINE_DELETE_CASE(DATETIME);
            DEFINE_DELETE_CASE(DeviceInfo_t);
            DEFINE_DELETE_CASE(ObjectInfo_t);
            DEFINE_DELETE_CASE(StorageInfo_t);
            DEFINE_DELETE_CASE(Binary_t);
            default:
                if(u._void!=NULL) throw RUNTIME_ERROR("Invalid _sym.");
                break;
        }
#undef DEFINE_DELETE_CASE
        init();
    }
    
#define DEFINE_PARSE(_T_, _NAME_) \
    try {  _T_::parse(stream, _##_NAME_); } catch(...) { LOG_FATAL("Invalid stream for `" #_NAME_ "`."); }
    
    bool DeviceInfo_t::parse(PTP::ByteStream& stream)
    {
        DEFINE_PARSE(UINT16,  standard_version);            // 01
        DEFINE_PARSE(UINT32,  vendor_extension_id);         // 02
        DEFINE_PARSE(UINT16,  vendor_extension_version);    // 03
        DEFINE_PARSE(STRING,  vendor_extension_desc);       // 04
        DEFINE_PARSE(UINT16,  functional_mode);             // 05 FunctionalMode
        DEFINE_PARSE(AUINT16, operations_supported);        // 06 array of OperationCode
        DEFINE_PARSE(AUINT16, events_supported);            // 07 array of EventCode
        DEFINE_PARSE(AUINT16, device_properties_supported); // 08 array of DevicePropCode
        DEFINE_PARSE(AUINT16, capture_formats);             // 09 array of ObjectFormatCode
        DEFINE_PARSE(AUINT16, image_formats);               // 10 array of ObjectFormatCode
        DEFINE_PARSE(STRING,  manufactuer);                 // 11
        DEFINE_PARSE(STRING,  model);                       // 12
        DEFINE_PARSE(STRING,  device_version);              // 13
        DEFINE_PARSE(STRING,  serial_number);               // 14
        return true;
    }

    bool ObjectInfo_t::parse(PTP::ByteStream& stream)
    {
        DEFINE_PARSE(UINT32, storage_id);               // 01
        DEFINE_PARSE(UINT16, object_format);            // 02 ObjectFormatCode
        DEFINE_PARSE(UINT16, protection_status);        // 03 ProtectionStatus
        DEFINE_PARSE(UINT32, object_compressed_size);   // 04
        DEFINE_PARSE(UINT16, thumb_format);             // 05 ObjectFormatCode
        DEFINE_PARSE(UINT32, thumb_compressed_size);    // 06
        DEFINE_PARSE(UINT32, thumb_pix_width);          // 07
        DEFINE_PARSE(UINT32, thumb_pix_height);         // 08
        DEFINE_PARSE(UINT32, image_pix_width);          // 09
        DEFINE_PARSE(UINT32, image_pix_height);         // 10
        DEFINE_PARSE(UINT32, image_bit_depth);          // 11
        DEFINE_PARSE(UINT32, parent_object);            // 12 ObjectHandle
        DEFINE_PARSE(UINT16, association_type);         // 13 AssociationCode
        DEFINE_PARSE(UINT32, association_desc);         // 14 AccociationDesc
        DEFINE_PARSE(UINT32, sequence_number);          // 15
        DEFINE_PARSE(STRING,   filename);               // 16
        DEFINE_PARSE(DATETIME, capture_date);           // 17
        DEFINE_PARSE(DATETIME, modification_date);      // 18
        DEFINE_PARSE(STRING,   keywords);               // 19
        return true;
    }

    bool StorageInfo_t::parse(PTP::ByteStream& stream)
    {
        DEFINE_PARSE(UINT16, storage_type);             // 01 StorageType
        DEFINE_PARSE(UINT16, filesystem_type);          // 02 FilesystemType
        DEFINE_PARSE(UINT16, access_capability);        // 03 AccessCapability
        DEFINE_PARSE(UINT64, max_capacity);             // 04
        DEFINE_PARSE(UINT64, free_space_in_bytes);      // 05
        DEFINE_PARSE(UINT32, free_space_in_images);     // 06
        DEFINE_PARSE(STRING, storage_description);      // 07
        DEFINE_PARSE(STRING, volume_label);             // 08
        return true;
    }
#undef DEFINE_PARSE
    
    void Binary_t::append(const char* data, size_t length)
    {
        _length += length;
        _data.push_back(string(data, length));
    }
}