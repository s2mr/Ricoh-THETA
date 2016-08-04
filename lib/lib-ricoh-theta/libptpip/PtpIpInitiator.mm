/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpInitiator.h"

using namespace std;
using namespace PTP::IP;

static void eventListener_proc(const void* _this, const ::PTP::EventPacket& event);
static void connectionEventListener_proc(const void* _this, int event);

@implementation PtpIpInitiator

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
        _initiator = NULL;
        _server = @"127.0.0.1";
        _port = PTP::IP::DEFAULT_PORT;
        _guid = nil;
        _friendly_name = nil;
        _deviceInfo = nil;
        _listener_lock = [[NSLock alloc] init];
        _event_listener = nil;
        _timeLimitForResponse = 20;
    }
    
    return self;
}

-(void)server:(NSString*)server_ip
{
    _server = server_ip;
}

-(void)port:(int)port_number
{
    _port = port_number;
}

-(void)guid:(NSString*)guid_{
    _guid = guid_;
}

-(void)friendly_name:(NSString*)friendly_name_
{
    _friendly_name = friendly_name_;
}

-(BOOL)connect
{
    LOG_DEBUG("===== Entireing PTP/IP Initiator::Connect.");
    if(_initiator) {
        [self close];
    }
    
    _initiator = new Initiator;
    if(_guid) _initiator->guid([_guid UTF8String]);
    if(_friendly_name) _initiator->friendly_name([_friendly_name UTF8String]);
    _initiator->set_timelimit_for_response((int)_timeLimitForResponse);
    
    const void* _self = (__bridge const void*)self;
    _initiator->attachEventListener(_self, eventListener_proc);
    _initiator->attachConnectionEventListener(_self, connectionEventListener_proc);
    
    bool res = _initiator->connect([_server UTF8String], _port);
    if(!res) {
        delete _initiator;
        _initiator = NULL;
        return NO;
    }
    LOG_DEBUG("===== Done.");
    return YES;
}

-(BOOL)connected
{
    return (_initiator && _initiator->connected()) ? YES: NO;
}

-(void)close
{
    LOG_DEBUG("===== Entireing PTP/IP Initiator::Close.");
    
    if(_initiator) {
        ::PTP::IP::Initiator* i = _initiator;
        _initiator = NULL;
        i->attachConnectionEventListener(nil, nil);
        i->attachEventListener(nil, nil);
        i->close();
        delete i;
    }
    _deviceInfo = nil;
    LOG_DEBUG("===== Done.");
}

-(NSInteger)targetConnectionNumber
{
    return _initiator? _initiator->_target_ConnectionNumber: -1;
}

-(NSString*)targetGUID
{
    return _initiator? [NSString stringWithUTF8String:_initiator->_target_GUID.c_str()]: nil;
}

-(NSString*)targetFriendlyName
{
    return _initiator? [NSString stringWithUTF8String:_initiator->_target_FriendlyName.c_str()]: nil;
}

-(NSInteger)targetReasonOfFail
{
    return _initiator? _initiator->_target_ReasonOfFail: -1;
}

-(void)attachPtpIpEventListener:(id)listener
{
    [_listener_lock lock];
    _event_listener = listener;
    [_listener_lock unlock];
}

static void eventListener_proc(const void* _this, const ::PTP::EventPacket& event)
{
    LOG_DEBUG("eventListener_proc: code=0x%x, tid=%u, params=0x%x/0x%x/0x%x",
          event.code(), event.transaction_id(), event[0], event[1], event[2]);
    PtpIpInitiator* _self = (__bridge PtpIpInitiator*)_this;
    
    if(_self) {
        id<PtpIpEventListener> proc;
        [_self->_listener_lock lock];
        proc = _self->_event_listener;
        [_self->_listener_lock unlock];
        @autoreleasepool{
            [proc ptpip_eventReceived:event.code() :event[0] :event[1] :event[2]];
        }
    }
}

static void connectionEventListener_proc(const void* _this, int event)
{
    LOG_DEBUG("connectionEventListener_proc: code=%d", event);
    PtpIpInitiator * _self = (__bridge PtpIpInitiator*)_this;
    
    if(_self) {
        id<PtpIpEventListener> proc;
        [_self->_listener_lock lock];
        proc = _self->_event_listener;
        [_self->_listener_lock unlock];
        @autoreleasepool{
            [proc ptpip_socketError :event];
        }
    }
}

-(PtpIpDeviceInfo*)getDeviceInfo
{
    ::PTP::DeviceInfo_t v;
    if(_initiator && _initiator->connected()) {
        if(_deviceInfo) return _deviceInfo;
        if(_initiator->getDeviceInfo(v)) {
            _deviceInfo = [[PtpIpDeviceInfo alloc] initWithObject:&v];
            return _deviceInfo;
        }
    }
    return nil;
}
-(void)clearDeviceInfoCache
{
    _deviceInfo = nil;
}

-(PtpIpSession*)open
{
    if(_initiator && _initiator->connected()) {
        PtpIpSession* p = [[PtpIpSession alloc] initWithInitiator:self];
        if(p && _initiator->openSession(*[p session])) {
            return p;
        }
    }
    return nil;
}

-(void)setLoglevel:(NSInteger)level
{
    g_Logger.loglevel((int)level);
}

-(void)setTimeLimitForResponse:(NSInteger)timelimit
{
    _timeLimitForResponse = timelimit;
    if(_initiator) {
        _initiator->set_timelimit_for_response((int)_timeLimitForResponse);
    }
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpInitiator:%p\n", self];
    [s appendFormat:@" server=%@", _server];
    [s appendFormat:@" port=%d", _port];
    [s appendFormat:@" initiator GUID=%@", _guid ? _guid: @"<library default>"];
    [s appendFormat:@" initiator name=%@", _friendly_name ? _friendly_name: @"<library default>"];
    [s appendFormat:@" connected=%d", [self connected]];
    if(_initiator) {
        [s appendFormat:@" connection number=%d", _initiator->_target_ConnectionNumber];
        [s appendFormat:@" target GUID=%s", _initiator->_target_GUID.c_str()];
        [s appendFormat:@" target name=%s", _initiator->_target_FriendlyName.c_str()];
    }
    [s appendString:@" >"];
    return s;
}

@end
