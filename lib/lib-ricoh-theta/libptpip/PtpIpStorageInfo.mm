/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpStorageInfo.h"

@implementation PtpIpStorageInfo
@synthesize max_capacity = _max_capacity;
@synthesize free_space_in_bytes = _free_space_in_bytes;
@synthesize free_space_in_images = _free_space_in_images;

- (id)init
{
    self = [super init];
    if (self) {
        _max_capacity = 0;
        _free_space_in_bytes = 0;
        _free_space_in_images = 0;
    }
    return self;
}

-(id)initWithObject:(::PTP::StorageInfo_t*)source
{
    self = [super init];
    if (self && source) {
        _max_capacity = source->max_capacity();
        _free_space_in_bytes = source->free_space_in_bytes();
        _free_space_in_images = source->free_space_in_images();
    }
    return self;
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpStorageInfo: %p", self];
    [s appendFormat:@" max_capacity=%llu", _max_capacity];
    [s appendFormat:@" free_space_in_bytes=%llu", _free_space_in_bytes];
    [s appendFormat:@" free_space_in_images=%u", _free_space_in_images];
    [s appendString:@" >"];
    return s;
}

@end
