/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "LibLogging.h"

@implementation LibLogging

+ (ObjcLogger*)l
{
    static ObjcLogger* const logger = [ObjcLogger newObjcLogger:OCLL_DEBUG];
    return logger;
}

@end
