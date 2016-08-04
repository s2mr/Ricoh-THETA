/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpDeviceInfo.h"

@implementation PtpIpDeviceInfo
@synthesize model = _model;
@synthesize device_version = _device_version;
@synthesize serial_number = _serial_number;

- (id)init
{
    self = [super init];
    if (self) {
        _model = nil;
        _device_version = nil;
        _serial_number = nil;
    }
    return self;
}

-(id)initWithObject:(::PTP::DeviceInfo_t*)source
{
    self = [super init];
    if (self && source) {
        _model = [NSString stringWithUTF8String:source->model().c_str()];
        _device_version = [NSString stringWithUTF8String:source->device_version().c_str()];
        _serial_number = [NSString stringWithUTF8String:source->serial_number().c_str()];
    }
    return self;
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpDeviceInfo: %p", self];
    [s appendFormat:@" model=%@", _model];
    [s appendFormat:@" device_version=%@", _device_version];
    [s appendFormat:@" serial_number=%@", _serial_number];
    [s appendString:@" >"];
    return s;
}

@end
