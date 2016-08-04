/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpOperator.h"
#import "PtpLogging.h"

@implementation PtpOperator

#pragma mark initializer

- (id)initWithInitiator:(PtpIpInitiator* const)initiator_
{
    self = [super init];
    if (self) {
        initiator = initiator_;
    }

    return self;
}

#pragma mark connect

- (void)connect:(void(^ const)(BOOL connected))block
{
    PLOG_DEBUG("===== Connecting initiator and session.");
    BOOL result = [initiator connect];
    if (result) {
        PLOG_DEBUG("  PtpIpInitiator#connect() succeeded.");
        PtpIpSession* const ses = [initiator open];
        if (ses) {
            PLOG_DEBUG("  PtpIpInitiator#open() succedded.");
            session = ses;
        } else {
            PLOG_DEBUG("  PtpIpInitiator#open() failed.");
            [initiator close];
            result = NO;
        }
    } else {
        PLOG_DEBUG("  PtpIpInitiator#connect() failed.");
    }
    PLOG_VERBOSE("  Call open block with %d.", result);
    block(result);
    PLOG_VERBOSE("  Open block returned.");
    PLOG_DEBUG("===== Finished connecting initiator and session.");
}

#pragma mark available information after connect().

#define DEFINE_INITIATOR_DELEGATOR(ret_type__, func__) \
    - (ret_type__)func__ {\
        return [initiator func__];\
    }

#define DEFINE_INITIATOR_DELEGATOR_1(ret_type__, func__, arg_type__) \
    - (ret_type__)func__:(arg_type__)param1 {\
        return [initiator func__:param1];\
    }

DEFINE_INITIATOR_DELEGATOR(BOOL, connected)

DEFINE_INITIATOR_DELEGATOR(NSString*, targetGUID)

DEFINE_INITIATOR_DELEGATOR(NSString*, targetFriendlyName)

DEFINE_INITIATOR_DELEGATOR(NSInteger, targetReasonOfFail)

- (void)setTargetIp:(NSString* const)server_ip
{
    [initiator server:server_ip];
}

- (void)setTargetIp:(NSString* const)server_ip port:(const int)port_number
{
    [initiator server:server_ip];
    [initiator port:port_number];
}

- (void)setInitiatorGUID:(NSString* const)guid friendlyName:(NSString* const)name
{
    [initiator guid:guid];
    [initiator friendly_name:name];
}

#pragma mark PtpIpInitiator operations.

- (void)close:(void(^ const)())block
{
    PLOG_DEBUG("===== Disconnecting initiator and session.");
    if (session) {
        PLOG_DEBUG("  Closing open session.");
        [session close];
        session = nil;
    } else {
        PLOG_DEBUG("  No open sessions.");
    }
    PLOG_DEBUG("  Closing initiator.");
    [initiator close];
    PLOG_DEBUG("  Closed initiator.");
    if (block) {
        PLOG_IF_VERBOSE("  Call close block.");
        block();
        PLOG_IF_VERBOSE("  Close block returned.");
    }
    PLOG_DEBUG("===== Finished disconnecting initiator and session.");
}

- (void)getDeviceInfo:(void(^const )(const PtpIpDeviceInfo* const info))block
{
    PLOG_DEBUG("===== Getting DeviceInfo.");
    const PtpIpDeviceInfo* const info = [initiator getDeviceInfo];
    PLOG_DEBUG("  Called PtpIpInitiator#getDeviceInfo(). - %@", info);
    PLOG_VERBOSE("  Call info block.");
    block(info);
    PLOG_VERBOSE("  Info block returned.");
    PLOG_DEBUG("===== Finished getting DeviceInfo.");
}

#pragma mark PtpIpSession operation.

- (void)operateSession:(void(^ const)(PtpIpSession* const session))block
{
    PLOG_DEBUG("===== Execute session operation.");
    PLOG_VERBOSE("  Call session block - %@.", session);
    block(session);
    PLOG_VERBOSE("  Session block returned.");
    PLOG_DEBUG("===== Finished executing session operation.");
}

#pragma mark Log settings.

DEFINE_INITIATOR_DELEGATOR_1(void, setLoglevel, const NSInteger)

DEFINE_INITIATOR_DELEGATOR_1(void, setTimeLimitForResponse, const NSInteger);

@end
