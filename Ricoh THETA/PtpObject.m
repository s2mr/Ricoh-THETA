/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpObject.h"

@implementation PtpObject

@synthesize objectInfo = _objectInfo;
@synthesize thumbnail = _thumbnail;

/**
 * Function for data creation
 * @param objectInfo PtpIpObjectInfo Object abstraction class at PTP communication
 * @param thumbnail Thumbnail object
 */
- (id)initWithObjectInfo:(PtpIpObjectInfo*)objectInfo thumbnail:(UIImage*)thumbnail
{
    self = [super init];
    if (self) {
        _objectInfo = objectInfo;
        _thumbnail = thumbnail;
    }
    return self;    
}

@end
