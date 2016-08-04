/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "PtpIpStorageInfo.h"
#import "PtpIpObjectInfo.h"
#import "PtpIpObjectContainer.h"
#import "PtpIpDevicePropDesc.h"

#ifdef __cplusplus
#   include "ptpip.h"
#else
#   include "codes_c.h"
#endif

typedef struct {
    NSUInteger denominator, numerator;
} PtpIpRational;

NS_INLINE PtpIpRational PtpIpRationalMake(NSUInteger numerator, NSUInteger denominator)
{
    return (PtpIpRational){
        .numerator = numerator,
        .denominator = denominator,
    };
}


@class PtpIpInitiator;

/**
 *  PTP/IP session management
 */
@interface PtpIpSession : NSObject
{
    PtpIpInitiator* _initiator;
#ifdef __cplusplus
    ::PTP::IP::Session* _session;
#else
    void* _session;
#endif
}
-(id)init;
-(id)initWithInitiator:(PtpIpInitiator*)initiator;

#ifdef __cplusplus
-(::PTP::IP::Session*)session;
#else
-(void*)session;
#endif
-(NSInteger)sid;
-(NSInteger)tid;
-(NSInteger)lastResponseCode;

-(void)close;

/**
 * Returns list of currently valid storage IDs.
 *
 * @return Storage ID list
 */
-(NSArray*)getStorageIDs;

/**
 *  Returns storage information.
 *
 *  @return Storage information
 */
-(PtpIpStorageInfo*)getStorageInfo;

/**
 *  Returns object count.
 *
 *  @return Object count
 */
-(NSInteger)getNumObjects;
-(NSInteger)getNumObjects:(NSInteger)parentObject DEPRECATED_ATTRIBUTE;
-(NSInteger)getNumObjects:(NSInteger)formatCode parent:(NSInteger)parentObject;

/**
 *  Returns ObjectHandle.
 *
 *  @return ObjectHandle array
 */
-(NSArray*)getObjectHandles;
-(NSArray*)getObjectHandles:(NSInteger)parentObject DEPRECATED_ATTRIBUTE;
-(NSArray*)getObjectHandlesFrom:(NSDate*)date DEPRECATED_ATTRIBUTE;
-(NSArray*)getObjectHandles:(NSInteger)parentObject from:(NSDate*)date DEPRECATED_ATTRIBUTE;
-(NSArray*)getObjectHandles:(NSInteger)formatCode parent:(NSInteger)parentObject from:(NSDate*)date;

/**
 *  Returns object information corresponding to specified object handle.
 *
 *  @param objectHandle Object handle
 *
 *  @return Object information
 */
-(PtpIpObjectInfo*)getObjectInfo:(NSInteger)objectHandle;

/**
 *  Returns objects corresponding to specified object handle.
 *
 *  @param objectHandle Object handle
 *
 *  @return Object
 */
-(PtpIpObjectContainer*)getObject:(NSInteger)objectHandle;
-(BOOL)getObject:(NSInteger)objectHandle onChunkReceived:(BOOL(^)(NSData* data))block;
-(BOOL)getObject:(NSInteger)objectHandle onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block;

/**
 *  Returns thumbnail in specified object handle.
 *
 *  @param objectHandle Object handle
 *
 *  @return Thumbnail
 */
-(PtpIpObjectContainer*)getThumb:(NSInteger)objectHandle;
-(BOOL)getThumb:(NSInteger)objectHandle onChunkReceived:(BOOL(^)(NSData* data))block;
-(BOOL)getThumb:(NSInteger)objectHandle onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block;

/**
 *  Deletes objects corresponding to specified object handle.
 *
 *  @param objectHandle Object handle
 *
 *  @return Successful/failed
 */
-(BOOL)deleteObject:(NSInteger)objectHandle;

/**
 *  Shoots a still image.
 *
 *  @return Successful/failed
 */
-(BOOL)initiateCapture;

/**
 *  Ends continuous shooting.
 *
 *  @param tid Transaction ID
 *
 *  @return Successful/failed
 */
-(BOOL)terminateOpenCapture:(NSInteger)tid;

/**
 *  Starts continuous shooting. Video (RICOH THETA m15) and interval.
 *
 *  @return Transaction ID
 */
-(NSInteger)initiateOpenCapture;

/**
 *  Acquires object corresponding to specified object handle, resized to 2048x1024 pixels.
 *
 *  @param objectHandle Object handle
 *  @param width  Width
 *  @param height  Height
 *  @param startBlock Start information acquisition block
 *  @param block  Partial data acquisition block
 *
 *  @return Successful/failed
 */
-(BOOL)getResizedImageObject:(NSInteger)objectHandle
                       width:(NSInteger)width height:(NSInteger)height
                 onStartData:(void(^)(NSUInteger totalLength))startBlock onChunkReceived:(BOOL(^)(NSData* data))block;

/**
 *  Returns device property description for specified property.
 *
 *  @param devicePropCode Property Code
 *
 *  @return Device Property Description
 */
-(PtpIpDevicePropDesc*)getDevicePropDesc:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Turns Wi-Fi OFF.
 *
 *  @return Successful/failed
 */
-(BOOL)wlanPowerControl;

/**
 *  Acquire property value (Int8 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSInteger)getInt8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (Uint8 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSUInteger)getUint8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (Int16 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSInteger)getInt16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (Uint16 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSUInteger)getUint16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (Int32 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSInteger)getInt32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (Uint32 format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSUInteger)getUint32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (String format)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(NSString*)getStringPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Acquire property value (RATIONAL format Exif2.3 standard)
 *
 *  @param devicePropCode Property Code
 *
 *  @return Property value
 */
-(PtpIpRational)getRationalPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode;

/**
 *  Property value settings (Int8 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setInt8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value;

/**
 *  Property value settings (Uint8 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setUint8PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value;

/**
 *  Property value settings (Int16 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setInt16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value;

/**
 *  Property value settings (Uint16 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setUint16PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value;

/**
 *  Property value settings (Int32 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setInt32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSInteger)value;

/**
 *  Property value settings (Uint32 format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setUint32PropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSUInteger)value;

/**
 *  Property value settings (String format)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setStringPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(NSString*)value;

/**
 *  Property value settings (RATIONAL format Exif2.3 standard)
 *
 *  @param devicePropCode Property Code
 *  @param value Property value
 *
 *  @return Successful/failed
 */
-(BOOL)setRationalPropValue:(PTP_DEVICE_PROP_CODE)devicePropCode value:(PtpIpRational)value;

/**
 *  Acquires charge level.  There are four detection levels.
 *
 *  @return Charge level
 */
-(NSUInteger)getBatteryLevel;

/**
 *  Acquires white balance.
 *
 *  @return White balance
 */
-(NSInteger)getWhiteBalance;

/**
 *  Sets white balance.
 *
 *  @param value White balance
 *
 *  @return Successful/failed
 */
-(BOOL)setWhiteBalance:(NSInteger)value;

/**
 *  Acquires ISO sensitivity.
 *
 *  @return ISO sensitivity
 */
-(NSInteger)getExposureIndex;

/**
 *  Sets ISO sensitivity.
 *
 *  @param value ISO sensitivity
 *
 *  @return Successful/failed
 */
-(BOOL)setExposureIndex:(NSInteger)value;

/**
 *  Acquires exposure correction value.
 *
 *  @return Exposure compensation value
 */
-(NSInteger)getExposureBiasCompensation;

/**
 *  Sets exposure correction value.
 *
 *  @param value Exposure compensation value
 *
 *  @return Successful/failed
 */
-(BOOL)setExposureBiasCompensation:(NSInteger)value;

/**
 *  Acquires the date and time.
 *
 *  @return Date and time
 */
-(NSDate*)getDateTime;

/**
 *  Sets date and time.
 *
 *  @param value Date and time
 *
 *  @return Successful/failed
 */
-(BOOL)setDateTime:(NSDate*)value;

/**
 *  Acquires shooting mode.
 *
 *  @return Shooting mode
 */
-(NSInteger)getStillCaptureMode;

/**
 *  Sets the shooting mode.
 *
 *  @param value Shooting mode
 *
 *  @return Successful/failed
 */
-(BOOL)setStillCaptureMode:(NSInteger)value;

/**
 *  Acquires maximum number of shots for interval shooting.
 *
 *  @return Maximum number of shots
 */
-(NSInteger)getTimelapseNumber;

/**
 *  Sets maximum number of shots for interval shooting.
 *
 *  @param value Maximum number of shots
 *
 *  @return Successful/failed
 */
-(BOOL)setTimelapseNumber:(NSInteger)value;

/**
 *  Acquires shooting interval for interval shooting.
 *
 *  @return Shooting interval for interval shooting (milliseconds)
 */
-(NSInteger)getTimelapseInterval;

/**
 *  Sets shooting interval for interval shooting.
 *
 *  @param value Shooting interval for interval shooting (milliseconds)
 *
 *  @return Successful/failed
 */
-(BOOL)setTimelapseInterval:(NSInteger)value;

/**
 *  Acquires shutter sound volume.
 *
 *  @return Shutter sound volume (%)
 */
-(NSUInteger)getAudioVolume;

/**
 *  Sets shutter sound volume.
 *
 *  @param value Shutter sound volume (%)
 *
 *  @return Successful/failed
 */
-(BOOL)setAudioVolume:(NSUInteger)value;

/**
 *  Acquires error information.
 *
 *  @return Error information
 */
-(NSUInteger)getErrorInfo;

/**
 *  Acquires shutter speed. <br>
 *  The data type is the same as the Exif 2.3 standard rational type. For example, (1, 8000) is 1/8000 of a second.
 *
 *  @return Shutter speed
 */
-(PtpIpRational)getShutterSpeed;

/**
 *  Sets shutter speed. <br>
 *  The data type is the same as the Exif 2.3 standard rational type. For example, (1, 8000) is 1/8000 of a second.
 *
 *  @param value Shutter speed
 *
 *  @return Successful/failed
 */
-(BOOL)setShutterSpeed:(PtpIpRational)value;

/**
 *  Acquires GPS information.
 *
 *  @return GPS Information
 */
-(NSString*)getGPSInfo;

/**
 *  Sets GPS information.
 *
 *  @param value GPS information
 *
 *  @return Successful/failed
 */
-(BOOL)setGPSInfo:(NSString*)value;

/**
 *  Acquires the auto power off setting of the camera (minutes). <br>
 *  Auto power off is not set if 0 is set.
 *
 *  @return Camera auto power off (minutes)
 */
-(NSUInteger)getAutoPowerOffDelay;

/**
 *  Sets the auto power off setting of the camera (minutes). <br>
 *  Auto power off is not set if 0 is set.
 *
 *  @param value Camera auto power off (minutes)
 *
 *  @return Successful/failed
 */
-(BOOL)setAutoPowerOffDelay:(NSUInteger)value;

/**
 *  Acquires the sleep mode (seconds) of the camera. <br>
 *  Sleep mode is not activated if 0 is set
 *
 *  @return Camera sleep mode (seconds)
 */
-(NSUInteger)getSleepDelay;

/**
 *  Sets the sleep mode (seconds) of the camera. <br>
 *  Sleep mode is not activated if 0 is set
 *
 *  @param value Camera sleep mode (seconds)
 *
 *  @return Successful/failed
 */
-(BOOL)setSleepDelay:(NSUInteger)value;

/**
 *  Acquires wireless LAN channel.
 *
 *  @return Wireless LAN channel
 */
-(NSUInteger)getChannelNumber;

/**
 *  Sets wireless LAN channel. <br>
 *  Reflected when the Wi-Fi is turned OFF/ON. <br>
 *  Random operation occurs if 0 is set.
 *
 *  @param value Wireless LAN channel
 *
 *  @return Successful/failed
 */
-(BOOL)setChannelNumber:(NSUInteger)value;

/**
 *  Acquires shooting operation status of camera.
 *
 *  @return Shooting operation status of camera
 */
-(NSUInteger)getCaptureStatus;

/**
 *  Acquires video recording time (seconds). (RICOH THETA m15)
 *
 *  @return Video recording time (seconds)
 */
-(NSUInteger)getRecordingTime;

/**
 *  Acquires available video shooting time (seconds). (RICOH THETA m15)
 *
 *  @return Available video shooting time (seconds)
 */
-(NSUInteger)getRemainingRecordingTime;

@end