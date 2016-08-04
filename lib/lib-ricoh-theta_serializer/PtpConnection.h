/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "PtpIpInitiator.h"

/**
 *  PTP operation
 */
@interface PtpOperation : NSObject

/**
 *  Cancels operation
 */
- (void)cancel;

/**
 *  Acquires detailed information
 *
 *  @return Detailed information
 */
- (NSString*)description;
@end

@class PtpOperator;

/**
 *  RICOH THETA connection management
 */
@interface PtpConnection : NSObject<PtpIpEventListener>

@property (atomic, readwrite, retain) id<PtpIpEventListener> eventListener;

/**
 *  Instance initialization
 *
 *  @return Instance
 */
- (id)init;

/**
 *  Acquires detailed information
 *
 *  @return Detailed information
 */
- (NSString*)description;

/**
 *  Connects to RICOH THETA
 *
 *  @param block Post-process block (connection successful/connection failed)
 *
 *  @return PTP operation
 */
- (PtpOperation*)connect:(void(^)(BOOL connected))block;

/**
 *  Connected to RICOH THETA?
 *
 *  @return Connected/not connected
 */
- (BOOL)connected;
- (NSString*)targetGUID;
- (NSString*)targetFriendlyName;
- (NSInteger)targetReasonOfFail;

/**
 *  Closes connection with RICOH THETA
 *
 *  @param block Post-process block
 *
 *  @return PTP operation
 */
- (PtpOperation*)close:(void(^)())block;

/**
 *  Acquires device information
 *
 *  @param block Post-process block (device information)
 *
 *  @return PTP operation
 */
- (PtpOperation*)getDeviceInfo:(void(^)(const PtpIpDeviceInfo* info))block;

/**
 *  Starts operation session
 *
 *  @param block Post-process block (PTP/IP session)
 *
 *  @return PTP operation
 */
- (PtpOperation*)operateSession:(void(^)(PtpIpSession* session))block;

/**
 *  IP address setting of RICOH THETA
 *
 *  @param server_ip IP address
 */
- (void)setTargetIp:(NSString*)server_ip;
- (void)setTargetIp:(NSString*)server_ip port:(int)port_number;
- (void)setInitiatorGUID:(NSString*)guid friendlyName:(NSString*)name;

/**
 *  Log level setting
 *
 *  @param level Log level
 */
- (void)setLoglevel:(NSInteger)level;

/**
 *  Sets maximum response time
 *
 *  @param timelimit Response time (seconds)
 */
- (void)setTimeLimitForResponse:(NSInteger)timelimit;
@end

#define PTPIP_SOCKETERROR_CLOSED_BIT (1 << 16)
#define PTP_CONNECTION_CLOSED(error) (0 != (error & PTPIP_SOCKETERROR_CLOSED_BIT))
#define PTP_ORIGINAL_PTPIPERROR(error) (error & (PTPIP_SOCKETERROR_CLOSED_BIT - 1))
