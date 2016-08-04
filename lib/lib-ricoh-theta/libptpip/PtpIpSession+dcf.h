/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpSession.h"

@interface PtpIpSession (dcf)

-(NSArray*)getDcfObjectHandles:(NSInteger)formatCode
             imageObjectHandle:(NSInteger)imageObjectHandle
                          from:(NSDate*)date
                     dcfParent:(PtpIpObjectInfo**)parentNode;


-(NSArray*)getDcfObjectInfo:(NSInteger)imageObjectHandle
                  dcfParent:(PtpIpObjectInfo**)parentNode;


-(BOOL)deleteDcfObject:(NSInteger)imageObjectHandle
              onDelete:(BOOL(^)(BOOL result, const PtpIpObjectInfo* info))block;

@end
