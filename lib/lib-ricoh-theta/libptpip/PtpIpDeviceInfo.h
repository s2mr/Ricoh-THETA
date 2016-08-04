/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>

#ifdef __cplusplus
#   include "ptpip.h"
#else
#   include "codes_c.h"
#endif

/**
 *  Device information
 */
@interface PtpIpDeviceInfo : NSObject
{
    NSString* _model;
    NSString* _device_version;
    NSString* _serial_number;
}

#ifdef __cplusplus
-(id)initWithObject:(::PTP::DeviceInfo_t*)source;
#else
-(id)initWithObject:(void*)source;
#endif

@property (readonly) NSString* model;
@property (readonly) NSString* device_version;
@property (readonly) NSString* serial_number;

@end
