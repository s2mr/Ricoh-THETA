/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_variant_h
#define ptpip_initiator_variant_h

#include "codes.h"
#include "basictypes.h"
#include "bytestream.h"
#include "exceptions.h"


namespace PTP {
    
#define DEFINE_PARAM_VAL(_TYPE_, _T_,  _NAME_)    \
    protected: _TYPE_ _##_NAME_; \
    public:    _T_ _NAME_() const { return const_cast<_TYPE_*>(&_##_NAME_)->value(); } \
    public:    _TYPE_& _NAME_##_ref() const { return *const_cast<_TYPE_*>(&_##_NAME_); }
#define DEFINE_PARAM_REF(_TYPE_, _NAME_)    \
    protected: _TYPE_ _##_NAME_; \
    public:    _TYPE_& _NAME_() const { return *const_cast<_TYPE_*>(&_##_NAME_); } \
    public:    _TYPE_& _NAME_##_ref() const { return *const_cast<_TYPE_*>(&_##_NAME_); }

    class DeviceInfo_t {
        DEFINE_PARAM_VAL(UINT16,  uint16_t, standard_version);            // 01
        DEFINE_PARAM_VAL(UINT32,  uint32_t, vendor_extension_id);         // 02
        DEFINE_PARAM_VAL(UINT16,  uint16_t, vendor_extension_version);    // 03
        DEFINE_PARAM_REF(STRING,            vendor_extension_desc);       // 04
        DEFINE_PARAM_VAL(UINT16,  uint16_t, functional_mode);             // 05 FunctionalMode
        DEFINE_PARAM_REF(AUINT16,           operations_supported);        // 06 array of OperationCode
        DEFINE_PARAM_REF(AUINT16,           events_supported);            // 07 array of EventCode
        DEFINE_PARAM_REF(AUINT16,           device_properties_supported); // 08 array of DevicePropCode
        DEFINE_PARAM_REF(AUINT16,           capture_formats);             // 09 array of ObjectFormatCode
        DEFINE_PARAM_REF(AUINT16,           image_formats);               // 10 array of ObjectFormatCode
        DEFINE_PARAM_REF(STRING,            manufactuer);                 // 11
        DEFINE_PARAM_REF(STRING,            model);                       // 12
        DEFINE_PARAM_REF(STRING,            device_version);              // 13
        DEFINE_PARAM_REF(STRING,            serial_number);               // 14
    public:
        enum { ID = 0x10000, SYM = ID };
        DeviceInfo_t() {}
        virtual ~DeviceInfo_t() {}
        bool parse(PTP::ByteStream& stream);
    };
    
    class ObjectInfo_t {
        DEFINE_PARAM_VAL(UINT32, uint32_t,  storage_id);                // 01
        DEFINE_PARAM_VAL(UINT16, uint16_t,  object_format);             // 02 ObjectFormatCode
        DEFINE_PARAM_VAL(UINT16, uint16_t,  protection_status);         // 03 ProtectionStatus
        DEFINE_PARAM_VAL(UINT32, uint32_t,  object_compressed_size);    // 04
        DEFINE_PARAM_VAL(UINT16, uint16_t,  thumb_format);              // 05 ObjectFormatCode
        DEFINE_PARAM_VAL(UINT32, uint32_t,  thumb_compressed_size);     // 06
        DEFINE_PARAM_VAL(UINT32, uint32_t,  thumb_pix_width);           // 07
        DEFINE_PARAM_VAL(UINT32, uint32_t,  thumb_pix_height);          // 08
        DEFINE_PARAM_VAL(UINT32, uint32_t,  image_pix_width);           // 09
        DEFINE_PARAM_VAL(UINT32, uint32_t,  image_pix_height);          // 10
        DEFINE_PARAM_VAL(UINT32, uint32_t,  image_bit_depth);           // 11
        DEFINE_PARAM_VAL(UINT32, uint32_t,  parent_object);             // 12 ObjectHandle
        DEFINE_PARAM_VAL(UINT16, uint16_t,  association_type);          // 13 AssociationCode
        DEFINE_PARAM_VAL(UINT32, uint32_t,  association_desc);          // 14 AccociationDesc
        DEFINE_PARAM_VAL(UINT32, uint32_t,  sequence_number);           // 15
        DEFINE_PARAM_REF(STRING,            filename);                  // 16
        DEFINE_PARAM_VAL(DATETIME, struct tm, capture_date);            // 17
        DEFINE_PARAM_VAL(DATETIME, struct tm, modification_date);       // 18
        DEFINE_PARAM_REF(STRING,            keywords);                  // 19
    public:
        enum { ID = 0x10001, SYM = ID };
        ObjectInfo_t() {}
        virtual ~ObjectInfo_t() {}
        bool parse(PTP::ByteStream& stream);
    };
    
    class StorageInfo_t {
        DEFINE_PARAM_VAL(UINT16, uint16_t,  storage_type);              // 01 StorageType
        DEFINE_PARAM_VAL(UINT16, uint16_t,  filesystem_type);           // 02 FilesystemType
        DEFINE_PARAM_VAL(UINT16, uint16_t,  access_capability);         // 03 AccessCapability
        DEFINE_PARAM_VAL(UINT64, uint64_t,  max_capacity);              // 04
        DEFINE_PARAM_VAL(UINT64, uint64_t,  free_space_in_bytes);       // 05
        DEFINE_PARAM_VAL(UINT32, uint32_t,  free_space_in_images);      // 06
        DEFINE_PARAM_REF(STRING,            storage_description);       // 07
        DEFINE_PARAM_REF(STRING,            volume_label);              // 08
    public:
        enum { ID = 0x10002, SYM = ID };
        StorageInfo_t() {}
        virtual ~StorageInfo_t() {}
        bool parse(PTP::ByteStream& stream);
    };
    
    // Bynary stream
    class Binary_t {
    protected:
        vector<string> _data;
        size_t  _length;
    public:
        enum { ID = 0x10003, SYM = ID };
        Binary_t() { _length = 0; }
        virtual ~Binary_t() {}
        vector<string>& data() const { return *const_cast<vector<string>*>(&_data); }
        void append(const char* data, size_t length);
        inline size_t length() const { return _length; }
    };
    
    
    // Varinat
    class Variant {
    protected:
        int _sym;
        union {
            INT8*       _INT8;
            INT16*      _INT16;
            INT32*      _INT32;
            INT64*      _INT64;
            INT128*     _INT128;
            UINT8*      _UINT8;
            UINT16*     _UINT16;
            UINT32*     _UINT32;
            UINT64*     _UINT64;
            UINT128*    _UINT128;
            AINT8*      _AINT8;
            AINT16*     _AINT16;
            AINT32*     _AINT32;
            AINT64*     _AINT64;
            AINT128*    _AINT128;
            AUINT8*     _AUINT8;
            AUINT16*    _AUINT16;
            AUINT32*    _AUINT32;
            AUINT64*    _AUINT64;
            AUINT128*   _AUINT128;
            STRING*     _STRING;
            DATETIME*   _DATETIME;
            DeviceInfo_t*   _DeviceInfo_t;
            ObjectInfo_t*   _ObjectInfo_t;
            StorageInfo_t*  _StorageInfo_t;
            Binary_t*   _Binary_t;
            void*       _void;
        } u;
        bool is_valid(int sym) const { return (_sym == sym); }
        void remove();
        void init() {
            _sym = -1;
            memset(&u, 0, sizeof(u));
        }
    public:
        inline Variant() { init(); }
        inline virtual ~Variant() { remove(); }
        void to_ptp(string& ret);
        const Variant& value() const { return *this; }
#define DEFINE_OPERATORS(_T_) \
        void set(const _T_& value) { remove(); _sym = _T_::SYM; u._##_T_ = new _T_(value); } \
        operator _T_*() const { return (is_valid(_T_::SYM) ? u._##_T_: NULL); }
        DEFINE_OPERATORS(INT8);
        DEFINE_OPERATORS(INT16);
        DEFINE_OPERATORS(INT32);
        DEFINE_OPERATORS(INT64);
        DEFINE_OPERATORS(INT128);
        DEFINE_OPERATORS(UINT8);
        DEFINE_OPERATORS(UINT16);
        DEFINE_OPERATORS(UINT32);
        DEFINE_OPERATORS(UINT64);
        DEFINE_OPERATORS(UINT128);
        DEFINE_OPERATORS(AINT8);
        DEFINE_OPERATORS(AINT16);
        DEFINE_OPERATORS(AINT32);
        DEFINE_OPERATORS(AINT64);
        DEFINE_OPERATORS(AINT128);
        DEFINE_OPERATORS(AUINT8);
        DEFINE_OPERATORS(AUINT16);
        DEFINE_OPERATORS(AUINT32);
        DEFINE_OPERATORS(AUINT64);
        DEFINE_OPERATORS(AUINT128);
        DEFINE_OPERATORS(DeviceInfo_t);
        DEFINE_OPERATORS(ObjectInfo_t);
        DEFINE_OPERATORS(StorageInfo_t);
        DEFINE_OPERATORS(Binary_t);
#undef DEFINE_OPERATORS
    };
    
    
    class DevicePropDesc_t {
        DEFINE_PARAM_VAL(UINT16, uint16_t,      device_property_code);      // 01 DevicePropCode
        DEFINE_PARAM_VAL(UINT16, uint16_t,      data_type);                 // 02 
        DEFINE_PARAM_VAL(UINT8,  uint8_t,       get_set);                   // 03 Get(0x00) or Get/Set(0x01)
        DEFINE_PARAM_REF(Variant,               factory_default_value);     // 04 <depend on `data_type`>
        DEFINE_PARAM_REF(Variant,               current_value);             // 05 <depend on `data_type`>
        DEFINE_PARAM_VAL(UINT8,  uint8_t,       from_flag);                 // 06 None(0x00), Range(0x01), Enumeration(0x02)
    public:
        enum { ID = 0x10003, SYM = ID };
        DevicePropDesc_t() {}
        virtual ~DevicePropDesc_t() {}
        bool parse(PTP::ByteStream& stream) {
            throw STANDARD_ERROR("Not supported.", 0);
        }
    };
    class DevicePropDescRange_t : public DevicePropDesc_t {
        DEFINE_PARAM_REF(Variant, minimum_value);           // 07 <depend on `data_type`>
        DEFINE_PARAM_REF(Variant, maximum_value);           // 08 <depend on `data_type`>
        DEFINE_PARAM_REF(Variant, step_size);               // 09 <depend on `data_type`>
    public:
        enum { ID = 0x10004, SYM = ID };
        DevicePropDescRange_t() {}
        virtual ~DevicePropDescRange_t() {}
        bool parse(PTP::ByteStream& stream) {
            throw STANDARD_ERROR("Not supported.", 0);
        }
    };
    class DevicePropDescEnum_t : public DevicePropDesc_t {
        DEFINE_PARAM_VAL(UINT16, uint16_t,      number_of_values);        // 07
        DEFINE_PARAM_REF(Variant,               supported_values);        // 08 <depend on `data_type`>[], but <Array.length> is not useless.
    public:
        enum { ID = 0x10005, SYM = ID };
        DevicePropDescEnum_t() {}
        virtual ~DevicePropDescEnum_t() {}
        bool parse(PTP::ByteStream& stream) {
            throw STANDARD_ERROR("Not supported.", 0);
        }
    };
    
#undef DEFINE_PARAM
}

#endif
