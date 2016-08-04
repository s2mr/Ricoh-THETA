/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpLogging.h"

@implementation PtpLogging

+ (ObjcLogger*)l
{
    static ObjcLogger* const logger = [ObjcLogger newObjcLogger:OCLL_DEBUG];
    return logger;
}

@end