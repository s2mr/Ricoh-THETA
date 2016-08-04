/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Availability.h>
#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
#import <CommonCrypto/CommonDigest.h>
#endif
#import "PtpIpObjectInfo.h"
#include "ptpip.h"

@implementation PtpIpObjectInfo

+ (NSString*)date2str:(NSDate*)date
{
    NSDateFormatter* df = [[NSDateFormatter alloc] init];
    [df setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
    [df setDateFormat:@"yyyyMMdd'T'HHmmss"];
    return [df stringFromDate:date];
}

- (id)init
{
    self = [super init];
    return self;
}

-(id)initWithObject:(::PTP::ObjectInfo_t*)source deviceInfo:(PtpIpDeviceInfo*)deviceInfo
{
    return [self initWithObject:source deviceInfo:deviceInfo objectHandle:0];
}

-(id)initWithObject:(::PTP::ObjectInfo_t *)source deviceInfo:(PtpIpDeviceInfo *)deviceInfo objectHandle:(NSUInteger)objectHandle_
{
    self = [super init];
    if (self && source) {
        _object_handle = objectHandle_;
        _object_format = source->object_format();
        _protection_status = source->protection_status();
        _object_compressed_size = source->object_compressed_size();
        _thumb_compressed_size = source->thumb_compressed_size();
        _thumb_pix_width = source->thumb_pix_width();
        _thumb_pix_height = source->thumb_pix_height();
        _image_pix_width = source->image_pix_width();
        _image_pix_height = source->image_pix_height();
        _parent_object = source->parent_object();
        _association_type = source->association_type();
        _filename = [NSString stringWithUTF8String:source->filename().c_str()];
        struct tm tm = source->capture_date();
        _capture_date = [NSDate dateWithTimeIntervalSince1970:timegm(&tm)];
        _keywords = [NSString stringWithUTF8String:source->keywords().c_str()];
        _uuid = [PtpIpObjectInfo buildUUID:_capture_date fileName:_filename cameraModel:[deviceInfo model] cameraSerialNumber:[deviceInfo serial_number]];
    }
    return self;
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpObjectInfo: %p", self];
    [s appendFormat:@" object_handle=%tu", _object_handle];
    [s appendFormat:@" protection_status=%zd", _protection_status];
    [s appendFormat:@" filename=%@", _filename];
    [s appendFormat:@" capture_date=%@", [PtpIpObjectInfo date2str:_capture_date]];
    [s appendFormat:@" uuid=%@", _uuid];
    [s appendFormat:@" object_compressed_size=%tu", _object_compressed_size];
    [s appendFormat:@" thumb_compressed_size=%tu", _thumb_compressed_size];
    [s appendFormat:@" thumb_pix_width=%tu", _thumb_pix_width];
    [s appendFormat:@" thumb_pix_height=%tu", _thumb_pix_height];
    [s appendFormat:@" image_pix_width=%tu", _image_pix_width];
    [s appendFormat:@" image_pix_height=%tu", _image_pix_height];
    [s appendFormat:@" object_format=%tx", _object_format];
    [s appendFormat:@" parent_object=%tu", _parent_object];
    [s appendFormat:@" association_type=%tx", _association_type];
    [s appendString:@" >"];
    return s;
}

+(NSString*)buildUUID:(NSDate*)createdAt fileName:(NSString*)fileName cameraModel:(NSString*)cameraModel cameraSerialNumber:(NSString*)cameraSerialNumber;
{
    if(cameraModel) {
        char buf[strlen([cameraModel UTF8String])+1], *p;
        strcpy(buf, [cameraModel UTF8String]);
        strtok_r(buf, " ", &p);
        cameraModel = [NSString stringWithUTF8String:buf];
    } else {
        cameraModel = @"RICOH THETA";
    }
    if(cameraSerialNumber) {
        char buf[strlen([cameraSerialNumber UTF8String])+1], *p;
        strcpy(buf, [cameraSerialNumber UTF8String]);
        strtok_r(buf, " ", &p);
        cameraSerialNumber = [NSString stringWithUTF8String:buf];
    } else {
        cameraSerialNumber = @"00-00-00-00";
    }
    NSString* is = [NSString stringWithFormat:@"RICOH://%@/%@/%@/%@", cameraModel, cameraSerialNumber, fileName, [PtpIpObjectInfo date2str:createdAt]];
    
#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
    unsigned char digest[CC_MD5_DIGEST_LENGTH];
    CC_MD5([is UTF8String], (uint32_t)strlen([is UTF8String]), digest);
    NSMutableString* md5 = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH*2];
    for(int i=0; i<CC_MD5_DIGEST_LENGTH; i++) {
        [md5 appendFormat:@"%02X", digest[i]];
    }
#else
    NSString* md5 = is;
#endif
    LOG_INFO("UUID is %s => %s", [is UTF8String], [md5 UTF8String]);
    return md5;
}

@end
