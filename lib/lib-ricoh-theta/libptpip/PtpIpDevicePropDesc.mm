/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpDevicePropDesc.h"
#import "basictypes.h"

@interface PtpIpDevicePropDesc ()
@property (nonatomic, readwrite) NSUInteger devicePropertyCode;
@property (nonatomic, readwrite) kDatatype datatype;
@property (nonatomic, readwrite) kGetSet getSet;
@property (nonatomic, readwrite) NSNumber* factoryDefaultValueOfNumber;
@property (nonatomic, readwrite) NSString* factoryDefaultValueOfString;
@property (nonatomic, readwrite) NSNumber* currentValueOfNumber;
@property (nonatomic, readwrite) NSString* currentValueOfString;
@property (nonatomic, readwrite) kFormFlag formFlag;
@property (nonatomic, readwrite) NSNumber* maximumValue;
@property (nonatomic, readwrite) NSNumber* minimumValue;
@property (nonatomic, readwrite) NSNumber* stepSize;
@property (nonatomic, readwrite) NSInteger numberOfValues;
@property (nonatomic, readwrite) NSArray* supportedValues;
@end

NSNumber* readNumber(PTP::ByteStream& stream, kDatatype datatype)
{
    switch (datatype) {
        case kDatatypeINT8:
            int8_t int8;
            stream.read(int8);
            return [NSNumber numberWithChar:int8];
        case kDatatypeINT16:
            int16_t int16;
            stream.read(int16);
            return [NSNumber numberWithShort:int16];
        case kDatatypeINT32:
            int32_t int32;
            stream.read(int32);
            return [NSNumber numberWithLong:int32];
        case kDatatypeINT64:
            int64_t int64;
            stream.read(int64);
            return [NSNumber numberWithLongLong:int64];
        case kDatatypeUINT8:
            uint8_t uint8;
            stream.read(uint8);
            return [NSNumber numberWithUnsignedChar:uint8];
        case kDatatypeUINT16:
            uint16_t uint16;
            stream.read(uint16);
            return [NSNumber numberWithUnsignedShort:uint16];
        case kDatatypeUINT32:
            uint32_t uint32;
            stream.read(uint32);
            return [NSNumber numberWithUnsignedLong:uint32];
        case kDatatypeUINT64:
            uint64_t uint64;
            stream.read(uint64);
            return [NSNumber numberWithUnsignedLongLong:uint64];
        default:
            return nil;
    }
}

@implementation PtpIpDevicePropDesc

+ (PtpIpDevicePropDesc*)devicePropDescWithStream:(PTP::ByteStream&)stream
{
    int8_t int8 = 0;
    uint16_t uint16 = 0;

    PtpIpDevicePropDesc* zelf = [PtpIpDevicePropDesc new];
    stream.read(uint16);
    zelf.devicePropertyCode = uint16;
    stream.read(uint16);
    zelf.datatype = (kDatatype)uint16;
    if (zelf.datatype == kDatatypeUNDEF) {
        return zelf;
    }
    stream.read(int8);
    zelf.getSet = (kGetSet)int8;
    if (zelf.datatype == kDatatypeSTR) {
        PTP::STRING str;
        PTP::STRING::parse(stream, str);
        zelf.factoryDefaultValueOfString = [NSString stringWithUTF8String:str.value().c_str()];
        PTP::STRING::parse(stream, str);
        zelf.currentValueOfString = [NSString stringWithUTF8String:str.value().c_str()];
    } else {
        zelf.factoryDefaultValueOfNumber = readNumber(stream, zelf.datatype);
        zelf.currentValueOfNumber = readNumber(stream, zelf.datatype);
    }
    stream.read(int8);
    zelf.formFlag = (kFormFlag)int8;
    switch (zelf.formFlag) {
        case kFormFlagRangeForm:
            zelf.minimumValue = readNumber(stream, zelf.datatype);
            zelf.maximumValue = readNumber(stream, zelf.datatype);
            zelf.stepSize = readNumber(stream, zelf.datatype);
            break;
        case kFormFlagEnumerationForm: {
            stream.read(uint16);
            zelf.numberOfValues = uint16;
            NSMutableArray* array = [NSMutableArray new];

            for (int i = 0; i < zelf.numberOfValues; i++) {
                [array addObject:readNumber(stream, zelf.datatype)];
            }
            zelf.supportedValues = array;
            break;
        }
        default:
            break;
    }
    return zelf;
}


@end
