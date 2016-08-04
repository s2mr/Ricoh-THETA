/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "PtpIpDeviceInfo.h"
#import "PtpIpObjectInfo.h"
#import "PtpIpSession.h"
#include "logger.h"

#ifdef __cplusplus
#   include "ptpip.h"
#else
#   include "codes_c.h"
#endif

/**
 *  PTP/IP event listener
 */
@protocol PtpIpEventListener <NSObject>
@required

/**
 *  Receives event PTP/IP event
 *
 *  @param code   Event code
 *  @param param1 Parameter 1
 *  @param param2 Parameter 2
 *  @param param3 Parameter 3
 */
-(void)ptpip_eventReceived:(int)code :(uint32_t)param1 :(uint32_t)param2 :(uint32_t)param3;

/**
 *  Event socket error
 *
 *  @param err Error code
 */
-(void)ptpip_socketError:(int)err;

@end

/**
 *  PTP/IP initiator
 */
@interface PtpIpInitiator : NSObject
{
    NSString*     _server;
    int           _port;
    NSString*     _guid;
    NSString*     _friendly_name;
    PtpIpDeviceInfo* _deviceInfo;
    NSInteger     _timeLimitForResponse;
    
    NSLock* _listener_lock;
    id<PtpIpEventListener> _event_listener; // Locked.
    
#ifdef __cplusplus
    ::PTP::IP::Initiator* _initiator;       // Locked.
#else
    void* _initiator;
#endif
}

/**
 *  IP address setting of connection destination of RICOH THETA
 *
 *  @param server_ip IP address of connection destination of RICOH THETA
 */
-(void)server:(NSString*)server_ip;

/**
 *  Port number setting of connection destination of RICOH THETA
 *
 *  @param port_number Port number of connection destination of RICOH THETA
 */
-(void)port:(int)port_number;

-(void)guid:(NSString*)guid_;

-(void)friendly_name:(NSString*)friendly_name_;

/**
 *  Connects to RICOH THETA
 *
 *  @return Successful/failed
 */
-(BOOL)connect;

/**
 *  Connecting to RICOH THETA?
 *
 *  @return Connecting/not connecting
 */
-(BOOL)connected;

/**
 *  Closes connection with RICOH THETA
 */
-(void)close;

-(NSInteger)targetConnectionNumber;
-(NSString*)targetGUID;
-(NSString*)targetFriendlyName;
-(NSInteger)targetReasonOfFail;

-(PtpIpSession*)open;

/**
 *  Acquires device information
 *
 *  @return Device information
 */
-(PtpIpDeviceInfo*)getDeviceInfo;

/**
 *  Discards device information in cache
 */
-(void)clearDeviceInfoCache;

/**
 *  References PTP/IP event
 *
 *  @param listener Event destination instance
 */
-(void)attachPtpIpEventListener:(id<PtpIpEventListener>)listener;

/**
 *  Log level setting
 *
 *  @param level Log level
 */
-(void)setLoglevel:(NSInteger)level;

/**
 *  Sets maximum response time
 *
 *  @param timelimit Response time (seconds)
 */
-(void)setTimeLimitForResponse:(NSInteger)timelimit;

@end