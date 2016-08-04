/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpipInitiator.h"
#import "PtpIpSession.h"



@implementation PtpIpSession

-(id)init
{
    @throw [NSException exceptionWithName:@"Invalid method call." reason:@"You MUST call `initWithInitiator` for init." userInfo:nil];
}

- (id)initWithInitiator:(PtpIpInitiator *)initiator
{
    self = [super init];
    if (self) {
        _session = new ::PTP::IP::Session;
        _initiator = initiator;
    }
    return self;
}

-(::PTP::IP::Session*)session
{
    return _session;
}

-(void)close
{
    if(_session) {
        ::PTP::IP::Session* s = _session;
        _session = NULL;
        s->closeSession();
        delete s;
    }
}

-(NSInteger)sid
{
    return _session->session_id();
}

-(NSInteger)tid
{
    return _session->transaction_id();
}

-(NSInteger)lastResponseCode
{
    return _session->last_response_code();
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpSession: %p", self];
    [s appendFormat:@" session=%p", _session];
    if(_session) {
        [s appendFormat:@" sid=%d", _session->session_id()];
        [s appendFormat:@" tid=%d", _session->transaction_id()];
    }
    [s appendString:@" >"];
    return s;
}

-(NSArray*)getStorageIDs
{
    vector<uint32_t> vector;
    if (!_session->getStorageIDs(vector)) return nil;

    NSMutableArray* array = [NSMutableArray arrayWithCapacity:vector.size()];
    for(auto it = vector.begin(); it != vector.end(); it++) {
        [array addObject:[[NSNumber alloc] initWithInteger:*it]];
    }
    return array;
}

-(PtpIpStorageInfo*)getStorageInfo
{
    ::PTP::StorageInfo_t r;
    if(!_session->getStorageInfo(r)) return nil;
    return [[PtpIpStorageInfo alloc] initWithObject:&r];
}

-(NSInteger)getNumObjects
{
    return [self getNumObjects: ::PTP::ObjectFormatCode::IMAGES_ONLY parent: ::PTP::ObjectHandle::ROOT];
}
-(NSInteger)getNumObjects:(NSInteger)formatCode parent:(NSInteger)parentObject
{
    uint32_t r = -1;
    _session->getNumObjects(::PTP::StorageId::DEFAULT, (uint32_t)formatCode, (uint32_t)parentObject, 0, r);
    return r;
}
-(NSInteger)getNumObjects:(NSInteger)parentObject DEPRECATED_ATTRIBUTE
{
    if (parentObject==0xFFFFFFFF) {
        return [self getNumObjects:(::PTP::ObjectFormatCode::IMAGES_ONLY) parent:(::PTP::ObjectHandle::ROOT)];
    } else {
        return [self getNumObjects:0 parent:parentObject];
    }
}

-(NSArray*)getObjectHandles
{
    return [self getObjectHandles:(::PTP::ObjectFormatCode::IMAGES_ONLY) parent:(::PTP::ObjectHandle::ROOT) from:nil];
}
-(NSArray*)getObjectHandles:(NSInteger)formatCode parent:(NSInteger)parentObject from:(NSDate*)date
{
  uint64_t d = date? (uint64_t)[date timeIntervalSince1970] : 0;
  vector<uint32_t> r;
  if(!_session->getObjectHandles(::PTP::StorageId::DEFAULT, (uint32_t)formatCode, (uint32_t)parentObject, d, r)) return nil;
  
  NSMutableArray* a = [NSMutableArray arrayWithCapacity:r.size()];
  vector<uint32_t>::const_iterator it = r.begin();
  for(; it!=r.end(); it++) {
    NSNumber *c = [[NSNumber alloc] initWithInteger:(*it)];
    [a addObject:c];
  }
  return a;
}
-(NSArray*)getObjectHandles:(NSInteger)parentObject DEPRECATED_ATTRIBUTE
{
    if (parentObject==0xFFFFFFFF) {
        return [self getObjectHandles:(::PTP::ObjectFormatCode::IMAGES_ONLY) parent:(::PTP::ObjectHandle::ROOT) from:nil];
    } else {
        return [self getObjectHandles:0 parent:parentObject from:nil];
    }
}
-(NSArray*)getObjectHandlesFrom:(NSDate*)date DEPRECATED_ATTRIBUTE
{
    return [self getObjectHandles:(::PTP::ObjectFormatCode::IMAGES_ONLY) parent:(::PTP::ObjectHandle::ROOT) from:date];
}
-(NSArray*)getObjectHandles:(NSInteger)parentObject from:(NSDate*)date DEPRECATED_ATTRIBUTE
{
    if (parentObject==0xFFFFFFFF) {
        return [self getObjectHandles:(::PTP::ObjectFormatCode::IMAGES_ONLY) parent:(::PTP::ObjectHandle::ROOT) from:date];
    } else {
        return [self getObjectHandles:0 parent:parentObject from:date];
    }
}

-(PtpIpObjectInfo*)getObjectInfo:(NSInteger)objectHandle
{
    ::PTP::ObjectInfo_t r;
    if(!_session->getObjectInfo((uint32_t)objectHandle, r)) return nil;
    return [[PtpIpObjectInfo alloc] initWithObject:&r deviceInfo:[_initiator getDeviceInfo] objectHandle:objectHandle];
}

// getObject
// - Blocking I/O.
-(PtpIpObjectContainer*)getObject:(NSInteger)objectHandle
{
    PtpIpObjectContainer* c = [[PtpIpObjectContainer alloc] init];
    int fd = [c fd];
    ::PTP::InboundDataStream r(NULL, NULL, fd);
    if(!_session->getObject((uint32_t)objectHandle, r)) {
        [c unlink];
        return nil;
    }
    ::lseek(fd, SEEK_SET, 0);
    return c;
}

// getObject
// - Blocking I/O with callback.
-(BOOL)getObject:(NSInteger)objectHandle onChunkReceived:(BOOL(^)(NSData* data))block
{
    return [self getObject:objectHandle onStartData:nil onChunkReceived:block];
}

// getObject
// - Blocking I/O with callback, onStart and onData.
-(BOOL)getObject:(NSInteger)objectHandle onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block
{
    PtpIpObjectContainer* c = [[PtpIpObjectContainer alloc] initWithBlock:startBlock onChunkReceived:block];
    return _session->getObject((uint32_t)objectHandle, [c inboundStream]);
}

// getThumb
// - Blocking I/O.
-(PtpIpObjectContainer*)getThumb:(NSInteger)objectHandle
{
    PtpIpObjectContainer* c = [[PtpIpObjectContainer alloc] init];
    int fd = [c fd];
    ::PTP::InboundDataStream r(NULL, NULL, fd);
    if(!_session->getThumb((uint32_t)objectHandle, r)) {
        [c unlink];
        return nil;
    }
    ::lseek(fd, SEEK_SET, 0);
    return c;
}

// getThumb
// - Blocking I/O with callback.
-(BOOL)getThumb:(NSInteger)objectHandle onChunkReceived:(BOOL(^)(NSData* data))block
{
    return [self getThumb:objectHandle onStartData:nil onChunkReceived:block];
}

// getThumb
// - Blocking I/O with callback, onStart and onData.
-(BOOL)getThumb:(NSInteger)objectHandle onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block
{
    PtpIpObjectContainer* c = [[PtpIpObjectContainer alloc] initWithBlock:startBlock onChunkReceived:block];
    return _session->getThumb((uint32_t)objectHandle, [c inboundStream]);
}

-(BOOL)deleteObject:(NSInteger)objectHandle
{
    return _session->deleteObject((uint32_t)objectHandle, 0)? YES: NO;
}

-(BOOL)initiateCapture
{
    return _session->initiateCapture(0, 0)? YES: NO;
}

-(BOOL)terminateOpenCapture:(NSInteger)tid
{
    return _session->terminateOpenCapture((uint32_t)tid)? YES: NO;
}

-(NSInteger)initiateOpenCapture
{
    uint32_t tid;
    return _session->initiateOpenCapture(0, 0, tid)? tid: -1;
}

-(BOOL)getResizedImageObject:(NSInteger)objectHandle
                       width:(NSInteger)width height:(NSInteger)height
                 onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block
{
    PtpIpObjectContainer* c = [[PtpIpObjectContainer alloc] initWithBlock:startBlock onChunkReceived:block];
    return _session->getResizedImageObject((uint32_t)objectHandle, (uint32_t)width, (uint32_t)height, [c inboundStream]);
}

-(BOOL)wlanPowerControl
{
    return _session->wlanPowerControl()? YES: NO;
}

-(PtpIpDevicePropDesc*)getDevicePropDesc:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    PTP::ByteStream s;
    if(!_session->getDevicePropDesc(devicePropCode, s)) return nil;
    return [PtpIpDevicePropDesc devicePropDescWithStream:s];
}

#pragma mark - DeviceProperty basic accessor

-(NSInteger)getInt8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    int8_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSUInteger)getUint8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    uint8_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSInteger)getInt16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    int16_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSUInteger)getUint16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    uint16_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSInteger)getInt32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    int32_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSUInteger)getUint32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return -1;
    uint32_t r;
    ::PTP::String::from_s(r, s);
    return r;
}

-(NSString*)getStringPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return nil;
    ::PTP::STRING v;
    ::PTP::STRING::parse(s, v);
    return [NSString stringWithUTF8String:v.value().c_str()];
}

-(PtpIpRational)getRationalPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode
{
    PtpIpRational ra = PtpIpRationalMake(PTPIP_SHUTTER_SPEED_AUTOMATIC_DENOMINATOR,
                                         PTPIP_SHUTTER_SPEED_AUTOMATIC_NUMERATOR);

    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(devicePropCode, s)) return ra;
    uint64_t r;
    ::PTP::String::from_s(r, s);
    ra.denominator = r >> 32;
    ra.numerator = 0x0000FFFFLL & r;
    return ra;
}

-(BOOL)setInt8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value
{
    ::PTP::INT8 v(value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setUint8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value
{
    ::PTP::UINT8 v(value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setInt16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value
{
    ::PTP::INT16 v(value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setUint16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value
{
    ::PTP::UINT16 v(value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setInt32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value
{
    ::PTP::INT32 v((uint32_t)value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setUint32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value
{
    ::PTP::UINT32 v((uint32_t)value);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setStringPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSString*)value
{
    ::PTP::STRING v([value UTF8String]);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}

-(BOOL)setRationalPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(PtpIpRational)value
{
    uint64_t r = ((uint64_t)value.denominator) << 32;
    r = r | (0xFFFFFFFFLL & ((uint64_t)value.numerator));
    ::PTP::UINT64 v(r);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(devicePropCode, s)? YES: NO;
}


#pragma mark - code-specific DeviceProperty accessor

-(NSUInteger)getBatteryLevel
{
    return [self getUint8PropValue:PTPDP_BATTERY_LEVEL];
}

-(NSInteger)getWhiteBalance
{
    return [self getUint16PropValue:PTPDP_WHITE_BALANCE];
}
-(BOOL)setWhiteBalance:(NSInteger)value
{
    return [self setUint16PropValue:PTPDP_WHITE_BALANCE value:value];
}

-(NSInteger)getExposureIndex
{
    return [self getUint16PropValue:PTPDP_EXPOSURE_INDEX];
}
-(BOOL)setExposureIndex:(NSInteger)value
{
    return [self setUint16PropValue:PTPDP_EXPOSURE_INDEX value:value];
}

-(NSInteger)getExposureBiasCompensation
{
    return [self getInt16PropValue:PTPDP_EXPOSURE_BIAS_COMPENSATION];
}
-(BOOL)setExposureBiasCompensation:(NSInteger)value
{
    return [self setInt16PropValue:PTPDP_EXPOSURE_BIAS_COMPENSATION value:value];
}

-(NSDate*)getDateTime
{
    ::PTP::ByteStream s;
    if(!_session->getDevicePropValue(PTPDP_DATE_TIME, s)) return nil;
    ::PTP::DATETIME v;
    ::PTP::DATETIME::parse(s, v);
    struct tm tm = v.value();
    return [NSDate dateWithTimeIntervalSince1970:timegm(&tm)];
}
-(BOOL)setDateTime:(NSDate*)value
{
    time_t t = [value timeIntervalSince1970];
    struct tm tm;
    localtime_r(&t, &tm);
    
    ::PTP::DATETIME v(tm);
    string s;
    v.to_ptp(s);
    return _session->setDevicePropValue(PTPDP_DATE_TIME, s)? YES: NO;
}

-(NSInteger)getStillCaptureMode
{
    return [self getUint16PropValue:PTPDP_STILL_CAPTURE_MODE];
}
-(BOOL)setStillCaptureMode:(NSInteger)value
{
    return [self setUint16PropValue:PTPDP_STILL_CAPTURE_MODE value:value];
}

-(NSInteger)getTimelapseNumber
{
    return [self getUint16PropValue:PTPDP_TIMELAPSE_NUMBER];
}
-(BOOL)setTimelapseNumber:(NSInteger)value
{
    return [self setUint16PropValue:PTPDP_TIMELAPSE_NUMBER value:value];
}

-(NSInteger)getTimelapseInterval
{
    return [self getUint32PropValue:PTPDP_TIMELAPSE_INTERVAL];
}
-(BOOL)setTimelapseInterval:(NSInteger)value
{
    return [self setUint32PropValue:PTPDP_TIMELAPSE_INTERVAL value:value];
}

-(NSUInteger)getAudioVolume
{
    return [self getUint32PropValue:PTPDP_AUDIO_VOLUME];
}
-(BOOL)setAudioVolume:(NSUInteger)value
{
    return [self setUint32PropValue:PTPDP_AUDIO_VOLUME value:value];
}

-(NSUInteger)getErrorInfo
{
    return [self getUint32PropValue:PTPDP_ERROR_INFO];
}

-(PtpIpRational)getShutterSpeed
{
    return [self getRationalPropValue:PTPDP_SHUTTER_SPEED];
}
-(BOOL)setShutterSpeed:(PtpIpRational)value
{
    return [self setRationalPropValue:PTPDP_SHUTTER_SPEED value:value];
}

-(NSString*)getGPSInfo
{
    return [self getStringPropValue:PTPDP_GPS_INFO];
}
-(BOOL)setGPSInfo:(NSString*)value;
{
    return [self setStringPropValue:PTPDP_GPS_INFO value:value];
}

-(NSUInteger)getAutoPowerOffDelay
{
    return [self getUint8PropValue:PTPDP_AUTO_POWEROFF_DELAY];
}
-(BOOL)setAutoPowerOffDelay:(NSUInteger)value
{
    return [self setUint8PropValue:PTPDP_AUTO_POWEROFF_DELAY value:value];
}

-(NSUInteger)getSleepDelay
{
    return [self getUint16PropValue:PTPDP_SLEEP_DELAY];
}
-(BOOL)setSleepDelay:(NSUInteger)value
{
    return [self setUint16PropValue:PTPDP_SLEEP_DELAY value:value];
}

-(NSUInteger)getChannelNumber
{
    return [self getUint8PropValue:PTPDP_CHANNEL_NUMBER];
}
-(BOOL)setChannelNumber:(NSUInteger)value
{
    return [self setUint8PropValue:PTPDP_CHANNEL_NUMBER value:value];
}

-(NSUInteger)getCaptureStatus
{
    return [self getUint8PropValue:PTPDP_CAPTURE_STATUS];
}

-(NSUInteger)getRecordingTime
{
    return [self getUint16PropValue:PTPDP_RECORDING_TIME];
}

-(NSUInteger)getRemainingRecordingTime
{
    return [self getUint16PropValue:PTPDP_REMAINING_RECORDING_TIME];
}

@end
