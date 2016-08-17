/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "PtpIpObjectInfo.h"

/**
 * Photo data abstraction class for list
 */
@interface PtpObject : NSObject

@property (readonly) UIImage* thumbnail;
@property (readonly) PtpIpObjectInfo* objectInfo;

- (id)initWithObjectInfo:(PtpIpObjectInfo*)objectInfo thumbnail:(UIImage*)thumbnail;

@end
