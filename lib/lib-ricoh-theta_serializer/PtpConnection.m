/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpLogging.h"
#import "PtpConnection.h"
#import "PtpConnection+Extension.h"

@implementation PtpOperation(PrivateMethods)

+ (PtpOperation*)operate:(NSOperation* const)op
{
    assert(op);

    PtpOperation* const result = [[PtpOperation alloc] init];
    result->operation = op;
#ifdef DEBUG
    result->requested = [NSDate date];
#endif
    return result;
}

@end

@implementation PtpOperation

- (void)cancel
{
    PLOG_DEBUG("===== Cancelling PtpOperation.");
    [operation cancel];
}

-(NSString*)description
{
    NSMutableString* const s = [NSMutableString stringWithFormat:@"<PtpOperation: %p", self];
#ifdef DEBUG
    [s appendFormat:@" requested %@", requested];
    [s appendFormat:@" NSOperation %@", [operation debugDescription]];
#else
    [s appendFormat:@", canceled ", [operation isCancelled]];
    [s appendFormat:@", finished ", [operation isFinished]];
#endif
    [s appendString:@">"];
    return s;
}

@end


@implementation PtpConnection(PrivateMethods)

- (void)onFatal:(NSNumber* const)error
{
    PLOG_DEBUG("===== Handling deferred fatal error - %@.", error);
    [operator_ close:nil];
    [self.eventListener ptpip_socketError:[error intValue]];
    PLOG_DEBUG("  Event handler returned.");
}

- (void)onError:(NSNumber* const)error
{
    PLOG_DEBUG("===== Handling deferred error - %@.", error);
    [self.eventListener ptpip_socketError:[error intValue]];
    PLOG_DEBUG("  Error handler returned.");
}

- (PtpOperation*)offerOperation:(NSOperation* const)op
{
    [op setQueuePriority:NSOperationQueuePriorityLow];
    PtpOperation* const result = [PtpOperation operate:op];
    [gcd addOperation:op];
    PLOG_DEBUG("  PtpOperation deferred.");
    return result;
}

@end

@implementation PtpConnection

@synthesize eventListener = _realEventListener;

- (id)init
{
    self = [super init];
    if (self) {
        PtpIpInitiator* const initiator = [[PtpIpInitiator alloc] init];
        [initiator attachPtpIpEventListener:self];
        operator_ = [[PtpOperator alloc] initWithInitiator:initiator];

        gcd = [[NSOperationQueue alloc] init];
        [gcd setMaxConcurrentOperationCount:1];
    }
    return self;
}

-(NSString*)description
{
    NSMutableString* const s = [NSMutableString stringWithFormat:@"<PtpConnection: %p", self];
    [s appendFormat:@" eventListener %@", self.eventListener];
    [s appendFormat:@" queued operations %zd, %@", [gcd operationCount], [gcd debugDescription]];
    [s appendString:@">"];
    return s;
}


#define DEFINE_OPERATOR_DELEGATOR(ret_type__, func__) \
    - (ret_type__)func__ {\
        return [operator_ func__];\
    }

#define DEFINE_OPERATOR_DELEGATOR_1(ret_type__, func__, arg_type__) \
    - (ret_type__)func__:(arg_type__)param1 {\
        return [operator_ func__:param1];\
    }

#define DEFINE_OPERATOR_DELEGATOR_2(ret_type__, func__, arg1_type__, arg2_name__, arg2_type__) \
    - (ret_type__)func__:(arg1_type__)param1 arg2_name__:(arg2_type__)param2 {\
        return [operator_ func__:param1 arg2_name__:param2];\
    }

#define DEFINE_OPERATOR_DELEGATOR_B(func__, ...) \
    - (PtpOperation*)func__:(void(^ const)(__VA_ARGS__))block {\
        PLOG_DEBUG("===== PtpConnection " #func__ " requested.");\
        assert(block);\
        NSOperation* const op = [NSBlockOperation blockOperationWithBlock:^() {\
            [operator_ func__:block];\
        }];\
        return [self offerOperation:op];\
    }

DEFINE_OPERATOR_DELEGATOR_B(connect, BOOL connected)

DEFINE_OPERATOR_DELEGATOR(BOOL, connected)

DEFINE_OPERATOR_DELEGATOR(NSString*, targetGUID)

DEFINE_OPERATOR_DELEGATOR(NSString*, targetFriendlyName)

DEFINE_OPERATOR_DELEGATOR(NSInteger, targetReasonOfFail)

DEFINE_OPERATOR_DELEGATOR_B(close)

DEFINE_OPERATOR_DELEGATOR_B(getDeviceInfo, const PtpIpDeviceInfo* info)

DEFINE_OPERATOR_DELEGATOR_B(operateSession, PtpIpSession* session)

DEFINE_OPERATOR_DELEGATOR_1(void, setTargetIp, NSString* const)

DEFINE_OPERATOR_DELEGATOR_2(void, setTargetIp, NSString* const, port, const int)

DEFINE_OPERATOR_DELEGATOR_2(void, setInitiatorGUID, NSString* const, friendlyName, NSString* const)

- (void)setLoglevel:(const NSInteger)level
{
    [operator_ setLoglevel:level];
    [PLOG_LOGGER loglevel:(int)level];
}

- (void)setTimeLimitForResponse:(NSInteger)timelimit
{
    [operator_ setTimeLimitForResponse:timelimit];
}

#pragma mark PtpIpEventListener methods.

-(void)ptpip_eventReceived:(const int)code :(const uint32_t)param1 :(const uint32_t)param2 :(const uint32_t)param3
{
    PLOG_DEBUG("===== PTP event received - %d.", code);
    NSBlockOperation* const op = [NSBlockOperation blockOperationWithBlock:^(void) {
        PLOG_DEBUG("===== Handling deferred PTP event.");
        [self.eventListener ptpip_eventReceived:code :param1 :param2 :param3];
        PLOG_DEBUG("  PTP event handler returned.");
    }];
    [op setQueuePriority:NSOperationQueuePriorityNormal];
    [gcd addOperation:op];
    PLOG_DEBUG("  PTP event deferred.");
}

-(void)ptpip_socketError:(int)error
{
    PLOG_DEBUG("===== Socket error received - %d.", error);
    SEL sel = nil;
    NSOperationQueuePriority priority = NSOperationQueuePriorityNormal;
    switch (error) {
        case PTPIP_PROTOCOL_SOCKET_CLOSED:
        case PTPIP_PROTOCOL_BROCKEN_PACKET:
        case PTPIP_PROTOCOL_INVALID_SESSION_ID:
        case PTPIP_PROTOCOL_WATCHDOG_EXPIRED:
            PLOG_WARN("  Fatal error(%d) received!", error);
            sel = @selector(onFatal:);
            priority = NSOperationQueuePriorityHigh;
            error |= PTPIP_SOCKETERROR_CLOSED_BIT;
            break;
            
        default:
            PLOG_DEBUG("  Error received!");
            sel = @selector(onError:);
            break;
    }
    NSInvocationOperation* const op = [[NSInvocationOperation alloc] initWithTarget:self selector:sel object:[NSNumber numberWithInt:error]];
    [op setQueuePriority:priority];
    [gcd addOperation:op];
    PLOG_DEBUG("  Socket error deferred.");
}

@end
