/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#ifndef ptpip_initiator_codes_h
#define ptpip_initiator_codes_h
#include "codes_c.h"

namespace PTP {
    
    //--------------------------------------------------------
    //  PTP/IP
    //--------------------------------------------------------
    namespace IP {
        enum {
            DEFAULT_PORT     = PTPIP_DEFAULT_PORT,
            PROTOCOL_VERSION = 0x00010000,
        };

        // Packet Type (UINT32)
        namespace PacketCode {
            enum {
                InitCommandRequest  = 0x0001,
                InitCommandAck      = 0x0002,
                InitEventRequest    = 0x0003,
                InitEventAck        = 0x0004,
                InitFail            = 0x0005,
                OperationRequest    = 0x0006,
                OperationResponse   = 0x0007,
                Event               = 0x0008,
                StartData           = 0x0009,
                Data                = 0x000A,
                Cancel              = 0x000B,
                EndData             = 0x000C,
                ProbeRequest        = 0x000D,
                ProbeResponse       = 0x000E,
            };
        }
        
        // InitFail Reasons (UINT32) for `InitFail`
        namespace ReasonOfFail {
            enum {
                FAIL_REJECTED_INITIATOR = PTPIP_FAIL_REJECTED_INITIATOR,
                FAIL_BUSY               = PTPIP_FAIL_BUSY,
                FAIL_UNSPECIFIED        = PTPIP_FAIL_UNSPECIFIED,
                end
            };
            
        }
        
        // Data phase info (UINT32) for `OperationRequest`
        namespace DataPhaseInfo {
            enum {
                UNKNOWN             = 0x0000,
                NODATA_OR_DATAIN    = 0x0001,
                DATAOUT             = 0x0002,
            };
        }
    }
    
    //--------------------------------------------------------
    //  PTP
    //--------------------------------------------------------
    
    // Protocol version.
    enum {
        PROTOCOL_VERSION = 0x01001000,
    };

    // Object handle (UINT32)
    namespace ObjectHandle {
        enum {
            DEFAULT_HANDLE  = 0x00000000,
            ROOT            = 0xFFFFFFFF,
            NO_HANDLES      = 0xFFFFFFFF,
        };
    }
    // Storage ID (UINT32)
    namespace StorageId {
        enum {
            DEFAULT         = 0xFFFFFFFF,
        };
    }
    // Object format code (UINT16)
    namespace ObjectFormatCode {
        enum {
            IMAGES_ONLY     = PTPIP_FORMAT_IMAGES_ONLY,
            ASSOCIATION     = PTPIP_FORMAT_ASSOCIATION,
            TEXT            = PTPIP_FORMAT_TEXT,
            WAV             = PTPIP_FORMAT_WAV,
            MP3             = PTPIP_FORMAT_MP3,
            AVI             = PTPIP_FORMAT_AVI,
            JPEG            = PTPIP_FORMAT_JPEG,
            PNG             = PTPIP_FORMAT_PNG,
            TIFF            = PTPIP_FORMAT_TIFF,
            MPO             = PTPIP_FORMAT_MPO,
            ZIP             = PTPIP_FORMAT_ZIP,
        };
    }
    // AssociationType code (UINT16)
    namespace AssociationTypeCode {
        enum {
            Undefined       = PTPIP_ASSOCTYPE_UNDEFINED,
            GenericFolder   = PTPIP_ASSOCTYPE_GENERIC_FOLDER,
            AncillaryData   = PTPIP_ASSOCTYPE_ANCILLARY_DATA,
        };
    }

    // Operation code (UINT16), Initiator -> Responder
    namespace OperationCode {
        enum {
            BEGIN = 0x1001,
            GET_DEVICE_INFO         = 0x1001,
            OPEN_SESSION            = 0x1002,
            CLOSE_SESSION           = 0x1003,
            GET_STORAGE_IDS         = 0x1004,
            GET_STORAGE_INFO        = 0x1005,
            GET_NUM_OBJECTS         = 0x1006,
            GET_OBJECT_HANDLES      = 0x1007,
            GET_OBJECT_INFO         = 0x1008,
            GET_OBJECT              = 0x1009,
            GET_THUMB               = 0x100A,
            DELETE_OBJECT           = 0x100B,
            SEND_OBJECT_INFO        = 0x100C,
            SEND_OBJECT             = 0x100D,
            INITIATE_CAPTURE        = 0x100E,
            FORMAT_STORE            = 0x100F,
            RESET_DEVICE            = 0x1010,
            SELF_TEST               = 0x1011,
            SET_OBJECT_PROTECTION   = 0x1012,
            POWER_DOWN              = 0x1013,
            GET_DEVICE_PROP_DESC    = 0x1014,
            GET_DEVICE_PROP_VALUE   = 0x1015,
            SET_DEVICE_PROP_VALUE   = 0x1016,
            RESET_DEVICE_PROP_VALUE = 0x1017,
            TERMINATE_OPEN_CAPTURE  = 0x1018,
            MOVE_OBJECT             = 0x1019,
            COPY_OBJECT             = 0x101A,
            GET_PARTIAL_OBJECT      = 0x101B,
            INITIATE_OPEN_CAPTURE   = 0x101C,
            // PTP v1.1
            START_ENUM_HANDLES          = 0x101D,
            ENUM_HANDLES                = 0x101E,
            STOP_ENUM_HANDLES           = 0x101F,
            GET_VENDOR_EXTENSION_MAPS   = 0x1020,
            GET_VENDOR_DEVICE_INFO      = 0x1021,
            GET_RESIZED_IMAGE_OBJECT    = 0x1022,
            GET_FILESYSTEM_MANIFEST     = 0x1023,
            GET_STREAM_INFO             = 0x1024,
            GET_STREAM                  = 0x1025,
            // PTP v1.5
            WLAN_POWER_CONTROL          = 0x99A1,
            END,
        };
    }
    
    // Response code (UINT16), Responder -> Initiator
    namespace ResponseCode {
        enum {
            BEGIN = 0x2001,
            OK                          = PTPIP_RESPONSE_OK,
            GENERAL_ERROR               = PTPIP_RESPONSE_GENERAL_ERROR,
            SESSION_NOT_OPEN            = PTPIP_RESPONSE_SESSION_NOT_OPEN,
            INVALID_TRANSACTION_ID      = PTPIP_RESPONSE_INVALID_TRANSACTION_ID,
            OPERATION_NOT_SUPPORTED     = PTPIP_RESPONSE_OPERATION_NOT_SUPPORTED,
            PARAMETER_NOT_SUPPORTED     = PTPIP_RESPONSE_PARAMETER_NOT_SUPPORTED,
            INCOMLETE_TRANSFER          = PTPIP_RESPONSE_INCOMLETE_TRANSFER,
            INVALID_STORAGE_ID          = PTPIP_RESPONSE_INVALID_STORAGE_ID,
            INVALID_OBJECT_HANDLE       = PTPIP_RESPONSE_INVALID_OBJECT_HANDLE,
            DEVICE_PROP_NOT_SUPPORTED   = PTPIP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED,
            INVALID_OBJECT_FORMAT_CODE  = PTPIP_RESPONSE_INVALID_OBJECT_FORMAT_CODE,
            STORE_FULL                  = PTPIP_RESPONSE_STORE_FULL,
            OBJECT_WRITE_PROTECTED      = PTPIP_RESPONSE_OBJECT_WRITE_PROTECTED,
            STORE_READ_ONLY             = PTPIP_RESPONSE_STORE_READ_ONLY,
            ACCESS_DENIED               = PTPIP_RESPONSE_ACCESS_DENIED,
            NO_THUMBNAIL_PRESENT        = PTPIP_RESPONSE_NO_THUMBNAIL_PRESENT,
            SELFTEST_FAILED             = PTPIP_RESPONSE_SELFTEST_FAILED,
            PARTIAL_DELETION            = PTPIP_RESPONSE_PARTIAL_DELETION,
            STORE_NOT_AVAILABLE         = PTPIP_RESPONSE_STORE_NOT_AVAILABLE,
            SPECIFICATION_BY_FORMAT_UNSUPPORTED = PTPIP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED,
            NO_VALID_OBJECT_INFO        = PTPIP_RESPONSE_NO_VALID_OBJECT_INFO,
            INVALID_CODE_FORMAT         = PTPIP_RESPONSE_INVALID_CODE_FORMAT,
            UNKNOWN_VENDOR_CODE         = PTPIP_RESPONSE_UNKNOWN_VENDOR_CODE,
            CAPTURE_ALREADY_TERMINATED  = PTPIP_RESPONSE_CAPTURE_ALREADY_TERMINATED,
            DEVICE_BUSY                 = PTPIP_RESPONSE_DEVICE_BUSY,
            INVALID_PARENT_OBJECT       = PTPIP_RESPONSE_INVALID_PARENT_OBJECT,
            INVALID_DEVICE_PROP_FORMAT  = PTPIP_RESPONSE_INVALID_DEVICE_PROP_FORMAT,
            INVALID_DEVICE_PROP_VALUE   = PTPIP_RESPONSE_INVALID_DEVICE_PROP_VALUE,
            INVALID_PARAMETER           = PTPIP_RESPONSE_INVALID_PARAMETER,
            SESSION_ALREADY_OPEN        = PTPIP_RESPONSE_SESSION_ALREADY_OPEN,
            TRANSACTION_CANCELED        = PTPIP_RESPONSE_TRANSACTION_CANCELED,
            SPECIFICATION_OF_DESTINATION_UNSUPPORTED  = PTPIP_RESPONSE_SPECIFICATION_OF_DESTINATION_UNSUPPORTED,
            END,
        };
    }
    
    // Event code (UINT16), Responder -> Initiator
    namespace EventCode {
        enum {
            BEGIN = 0x4001,
            CANCEL_TRANSACTION        = PTPIP_CANCEL_TRANSACTION,
            OBJECT_ADDED              = PTPIP_OBJECT_ADDED,
            OBJECT_REMOVED            = PTPIP_OBJECT_REMOVED,
            STORE_ADDED               = PTPIP_STORE_ADDED,
            STORE_REMOVED             = PTPIP_STORE_REMOVED,
            DEVICE_PROP_CHANGED       = PTPIP_DEVICE_PROP_CHANGED,
            OBJECT_INFO_CHANGED       = PTPIP_DEVICE_INFO_CHANGED,
            DEVICE_INFO_CHANGED       = PTPIP_DEVICE_INFO_CHANGED,
            REQUEST_OBJECT_TRANSFER   = PTPIP_REQUEST_OBJECT_TRANSFER,
            STORE_FULL                = PTPIP_STORE_FULL,
            DEVICE_RESET              = PTPIP_DEVICE_RESET,
            STORAGE_INFO_CHANGED      = PTPIP_STORAGE_INFO_CHANGED,
            CAPTURE_COMPLETE          = PTPIP_CAPTURE_COMPLETE,
            UNREPORTED_STATUS         = PTPIP_UNREPORTED_STATUS,
            END,
        };
    }

    // Device property code (UINT16)
    namespace DevicePropCode {
        // => PTP_DEVICE_PROP_CODE
    }

    // While balance mode (UINT16)
    namespace WhiteBalanceMode {
        enum {
            MANUAL       = PTPIP_WHITE_BALANCE_MANUAL,
            AUTOMATIC    = PTPIP_WHITE_BALANCE_AUTOMATIC,
            ONE_PUSH_AUTOMATIC  = PTPIP_WHITE_BALANCE_ONE_PUSH_AUTOMATIC,
            DAYLIGHT     = PTPIP_WHITE_BALANCE_DAYLIGHT,
            TUNGSTEN1    = PTPIP_WHITE_BALANCE_TUNGSTEN1,
            FLASH        = PTPIP_WHITE_BALANCE_FLASH,
            SHADE        = PTPIP_WHITE_BALANCE_SHADE,
            CLOUDY       = PTPIP_WHITE_BALANCE_CLOUDY,
            FLUORESCENT1 = PTPIP_WHITE_BALANCE_FLUORESCENT1,
            FLUORESCENT2 = PTPIP_WHITE_BALANCE_FLUORESCENT2,
            FLUORESCENT3 = PTPIP_WHITE_BALANCE_FLUORESCENT3,
            FLUORESCENT4 = PTPIP_WHITE_BALANCE_FLUORESCENT4,
            TUNGSTEN2    = PTPIP_WHITE_BALANCE_TUNGSTEN2,
        };
    }
    
    // Focus mode (UINT16)
    namespace FocusMode {
        enum {
            MANUAL          = PTPIP_FOCUS_MODE_MANUAL,
            AUTOMATIC       = PTPIP_FOCUS_MODE_AUTOMATIC,
            AUTOMATIC_MACRO = PTPIP_FOCUS_MODE_AUTOMATIC_MACRO,
        };
    }
    
    // Flash mode (UINT16)
    namespace FlashMode {
        enum {
            AUTO_FLASH    = PTPIP_FLASH_MODE_AUTO_FLASH,
            FLASH_OFF     = PTPIP_FLASH_MODE_FLASH_OFF,
            FILL_FLASH    = PTPIP_FLASH_MODE_FILL_FLASH,
            RED_EYE_AUTO  = PTPIP_FLASH_MODE_RED_EYE_AUTO,
            RED_EYE_FILL  = PTPIP_FLASH_MODE_RED_EYE_FILL,
            EXTERNAL_SYNC = PTPIP_FLASH_MODE_EXTERNAL_SYNC,
        };
    }
    
    // Exposure program mode (UINT16)
    namespace ExposureProgramMode {
        enum {
            MANUAL            = PTPIP_EXPOSURE_PROGRAM_MODE_MANUAL,
            AUTOMATIC         = PTPIP_EXPOSURE_PROGRAM_MODE_AUTOMATIC,
            APERTURE_PRIORITY = PTPIP_EXPOSURE_PROGRAM_MODE_APERTURE_PRIORITY,
            SHUTTER_PRIPRITY  = PTPIP_EXPOSURE_PROGRAM_MODE_SHUTTER_PRIPRITY,
            PROGRAM_CREATIVE  = PTPIP_EXPOSURE_PROGRAM_MODE_PROGRAM_CREATIVE,
            PROGRAM_ACTION    = PTPIP_EXPOSURE_PROGRAM_MODE_PROGRAM_ACTION,
            PORTRAIT          = PTPIP_EXPOSURE_PROGRAM_MODE_PORTRAIT,
        };
    }
    
    // Still capture mode (UINT16)
    namespace StillCaptureMode {
        enum {
            NORMAL    = PTPIP_STILL_CAPTURE_MODE_NORMAL,
            BURST     = PTPIP_STILL_CAPTURE_MODE_BURST,
            TIMELAPSE = PTPIP_STILL_CAPTURE_MODE_TIMELAPSE,
        };
    }
    
    // Effect mode (UINT16)
    namespace EffectMode {
        enum {
            STANDARD        = PTPIP_EFFECT_MODE_STANDARD,
            BLACK_AND_WHITE = PTPIP_EFFECT_MODE_BLACK_AND_WHITE,
            SEPIA           = PTPIP_EFFECT_MODE_SEPIA,
        };
    }
    
    // Focus metering mode (UINT16)
    namespace FocusMeteringMode {
        enum {
            CENTER_SPOT = PTPIP_FOCUS_METERING_MODE_CENTER_SPOT,
            MULTI_SPOT  = PTPIP_FOCUS_METERING_MODE_MULTI_SPOT,
        };
    }
    
    // DeviceInfo
    namespace DeviceInfo {
        // FunctionalMode (UINT16)
        namespace FunctionalMode {
            enum {
                STANDARD = 0x0000,
                SLEEP    = 0x0001,
            };
        }
    }
    
    // ObjectInfo
    namespace ObjectInfo {
        // ProtectionStatus (UINT16)
        namespace ProtectionStatus {
            enum {
                NO_PROTECTION = PTPIP_PROTECTION_STATUS_NO_PROTECTION,
                READ_ONLY     = PTPIP_PROTECTION_STATUS_READ_ONLY,
            };
        }
    }
    
    // StorageInfo
    namespace StorageInfo {
        // StorageType (UINT16)
        namespace StorageType {
            enum {
                FIXED_ROM     = 0x0001,
                REMOVABLE_ROM = 0x0002,
                FIXED_RAM     = 0x0003,
                REMOVABLE_RAM = 0x0004,
            };
        }
        
        // FilesystemType (UINT16)
        namespace FilesystemType {
            enum {
                GENERIC_FLAT          = 0x0001,
                GENERIC_HIERARCHICAL  = 0x0002,
                DCF                   = 0x0003,
            };
        }
        
        // AccessCapability (UINT16)
        namespace AccessCapability {
            enum {
                READ_WRITE                    = 0x0000,
                READONLY                      = 0x0001, // w/o object deletion
                READONLY_WITH_OBJECT_DELETION = 0x0002,
            };
        }
    }
    
    // DeviceProperty
    namespace DeviceProp {
        // from flag (UINT8)
        namespace FromFlag {
            enum {
                NONE        = 0x00,
                RANGE       = 0x01,
                ENUMERATION = 0x02,
            };
        }
    }
}

#endif
