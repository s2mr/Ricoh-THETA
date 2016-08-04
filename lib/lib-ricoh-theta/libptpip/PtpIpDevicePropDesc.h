/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#ifdef __cplusplus
#   import "bytestream.h"
#endif

typedef NS_ENUM(NSUInteger, kDatatype) {
    kDatatypeUNDEF = 0x0000,
    kDatatypeINT8 = 0x0001,
    kDatatypeUINT8 = 0x0002,
    kDatatypeINT16 = 0x0003,
    kDatatypeUINT16 = 0x0004,
    kDatatypeINT32 = 0x0005,
    kDatatypeUINT32 = 0x0006,
    kDatatypeINT64 = 0x0007,
    kDatatypeUINT64 = 0x0008,
    kDatatypeINT128 = 0x0009,
    kDatatypeUINT128 = 0x000A,
    kDatatypeAINT8 = 0x4001,
    kDatatypeAUINT8 = 0x4002,
    kDatatypeAINT16 = 0x4003,
    kDatatypeAUINT16 = 0x4004,
    kDatatypeAINT32 = 0x4005,
    kDatatypeAUINT32 = 0x4006,
    kDatatypeAINT64 = 0x4007,
    kDatatypeAUINT64 = 0x4008,
    kDatatypeAINT128 = 0x4009,
    kDatatypeAUINT128 = 0x400A,
    kDatatypeSTR = 0xFFFF
};

typedef NS_ENUM(NSUInteger, kFormFlag) {
    kFormFlagNone = 0x00,
    kFormFlagRangeForm = 0x01,
    kFormFlagEnumerationForm = 0x02
};

typedef NS_ENUM(NSUInteger, kGetSet) {
    kGetSetGet = 0x00,
    kGetSetGetSet = 0x01
};

/**
 * Device Property Description
 */
@interface PtpIpDevicePropDesc : NSObject
@property (nonatomic, readonly) NSUInteger devicePropertyCode;
@property (nonatomic, readonly) kDatatype datatype;
@property (nonatomic, readonly) kGetSet getSet;
@property (nonatomic, readonly) NSNumber* factoryDefaultValueOfNumber;
@property (nonatomic, readonly) NSString* factoryDefaultValueOfString;
@property (nonatomic, readonly) NSNumber* currentValueOfNumber;
@property (nonatomic, readonly) NSString* currentValueOfString;
@property (nonatomic, readonly) kFormFlag formFlag;
@property (nonatomic, readonly) NSNumber* maximumValue;
@property (nonatomic, readonly) NSNumber* minimumValue;
@property (nonatomic, readonly) NSNumber* stepSize;
@property (nonatomic, readonly) NSInteger numberOfValues;
@property (nonatomic, readonly) NSArray* supportedValues;

#ifdef __cplusplus
+ (PtpIpDevicePropDesc*)devicePropDescWithStream:(PTP::ByteStream&)byteStream;
#endif

@end
