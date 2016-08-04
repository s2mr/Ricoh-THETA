/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "PtpIpDeviceInfo.h"

#ifdef __cplusplus
#   include "ptpip.h"
#else
#   include "codes_c.h"
#endif

/**
 *  Object information
 */
@interface PtpIpObjectInfo : NSObject
{
}

#ifdef __cplusplus
-(id)initWithObject:(::PTP::ObjectInfo_t*)source deviceInfo:(PtpIpDeviceInfo*)deviceInfo DEPRECATED_ATTRIBUTE;
-(id)initWithObject:(::PTP::ObjectInfo_t*)source deviceInfo:(PtpIpDeviceInfo*)deviceInfo objectHandle:(NSUInteger)objectHandle_;
#else
-(id)initWithObject:(void*)source deviceInfo:(PtpIpDeviceInfo*)deviceInfo DEPRECATED_ATTRIBUTE;
-(id)initWithObject:(void*)source deviceInfo:(PtpIpDeviceInfo*)deviceInfo objectHandle:(NSUInteger)objectHandle_;
#endif
+(NSString*)buildUUID:(NSDate*)createdAt fileName:(NSString*)fileName cameraModel:(NSString*)cameraModel cameraSerialNumber:(NSString*)cameraSerialNumber;

@property (readonly) NSUInteger object_handle;
@property (readonly) NSUInteger object_format;
@property (readonly) NSUInteger protection_status;
@property (readonly) NSUInteger object_compressed_size;
@property (readonly) NSUInteger thumb_compressed_size;
@property (readonly) NSUInteger parent_object;
@property (readonly) NSUInteger thumb_pix_width;
@property (readonly) NSUInteger thumb_pix_height;
@property (readonly) NSUInteger image_pix_width;
@property (readonly) NSUInteger image_pix_height;
@property (readonly) NSInteger association_type;
@property (readonly) NSString* filename;
@property (readonly) NSDate* capture_date;
@property (readonly) NSString* keywords;
@property (readonly) NSString* uuid;

@end
