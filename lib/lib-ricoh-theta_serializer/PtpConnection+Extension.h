/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpConnection.h"
#import "PtpOperator.h"

@interface PtpOperation ( )
{
@private
    NSOperation* operation;
    
#ifdef DEBUG
    NSDate* requested;
#endif
}
@end

@interface PtpConnection ( )
{
@private
    id<PtpIpEventListener> realEventListener;
    PtpOperator* operator_;
    NSOperationQueue* gcd;
}
@end
