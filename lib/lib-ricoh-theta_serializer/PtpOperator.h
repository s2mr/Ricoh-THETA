/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "PtpIpInitiator.h"
#import "PtpIpSession.h"

@interface PtpOperator : NSObject
{
@private
    PtpIpInitiator* initiator;
    PtpIpSession* session;
}

- (id)initWithInitiator:(PtpIpInitiator*)initiator;
- (void)connect:(void(^)(BOOL connected))block;
- (BOOL)connected;
- (NSString*)targetGUID;
- (NSString*)targetFriendlyName;
- (NSInteger)targetReasonOfFail;
- (void)close:(void(^)())block;
- (void)getDeviceInfo:(void(^)(const PtpIpDeviceInfo* info))block;
- (void)operateSession:(void(^)(PtpIpSession* session))block;

- (void)setTargetIp:(NSString*)server_ip;
- (void)setTargetIp:(NSString*)server_ip port:(int)port_number;
- (void)setInitiatorGUID:(NSString*)guid friendlyName:(NSString*)name;
- (void)setLoglevel:(NSInteger)level;
- (void)setTimeLimitForResponse:(NSInteger)timelimit;

@end
