/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#define DEFINE_EXIFTAG_CODE
#define LIBEXIFIO_LOG_MACROS
#import <ImageIO/ImageIO.h>
#import <CommonCrypto/CommonDigest.h>
#import "RicohEXIF.h"
#import "ExifTags.h"
#import "ExifRawData.h"


NSString* ptpipPhotoUUID = @"ptpipPhotoUUID";


@implementation RicohEXIF

-(id)init
{
    self = [super init];
    if (self) {
        _representation = nil;
        _nsdata = nil;
        _reviseOffset = 0;
    }
    return self;
}

-(NSDictionary*)parseRicohThetaTags:(ExifRawData*)data
{
    NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
    
    if([data seekToMakerNoteSubTags:&_reviseOffset parentTag:ricohExifRicohTheta_code]) {
        NSUInteger count = [data readWord];
        NSUInteger code=0, type=0, l=0;
        while(count>0) {
            AT_DEBUG(NSUInteger a = [data cursor]);
            NSArray* value = [data readEntry:&code entryType:&type length:&l];
            if (!value) break;
            LOG_DEBUG("RICOH THETA Entry(%2d) %08X(%d): %04X %2d %4d %s", count, a, _reviseOffset, code, type, l, value.description.UTF8String);
            switch(code) {
                case ricohThetaPanoramaImageType_code:
                    [dict setObject:value[0] forKey:ricohThetaPanoramaImageType];
                    break;
                case ricohThetaHdrMode_code:
                    [dict setObject:value[0] forKey:ricohThetaHdrMode];
                    break;
                case ricohThetaZenith_code:
                    [dict setObject:value forKey:ricohThetaZenith];
                    break;
                case ricohThetaCompass_code:
                    [dict setObject:value[0] forKey:ricohThetaCompass];
                    break;
                case ricohThetaAbnormalAcceleration_code:
                    [dict setObject:value[0] forKey:ricohThetaAbnormalAcceleration];
                    break;
                case ricohThetaFilmISO_code:
                    [dict setObject:value forKey:ricohThetaFilmISO];
                    break;
                case ricohThetaAperture_code:
                    [dict setObject:value forKey:ricohThetaAperture];
                    break;
                case ricohThetaExposureTime_code:
                    [dict setObject:value[0] forKey:ricohThetaExposureTime];
                    break;
                case ricohThetaSensorSerial1_code:
                    [dict setObject:value[0] forKey:ricohThetaSensorSerial1];
                    break;
                case ricohThetaSensorSerial2_code:
                    [dict setObject:value[0] forKey:ricohThetaSensorSerial2];
                    break;
                    
                case ricohThetaZenith_SHORT_code:
                    if (type==IFDType_SHORT) {
                        [dict setObject:value forKey:ricohThetaZenith_SHORT];
                    }
                    break;
                case ricohThetaCompass_SHORT_code:
                    if (type==IFDType_SHORT) {
                        [dict setObject:value[0] forKey:ricohThetaCompass_SHORT];
                    }
                    break;
                default:
                    break;
            }
            count--;
        }
    }
    return dict;
}

-(NSDictionary*)parseRicohMakerNote:(ExifRawData*)data
{
    NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
    
    if(![data seekAndReviseMakerNoteOffset:&_reviseOffset]) {
        return dict;
    }
    
    NSUInteger count = [data readWord];
    NSUInteger code=0, t=0, l=0, v=0;
    char buf[64];
    
    if(count>1024) {
        [data seekToMakerNote];
        LOG_WARN(@"Missing MakerNote, maybe. aborted. cursor=%08tX, offset=%zd", [data cursor], _reviseOffset);
        return dict;
    }
    
    while(count>0) {
        AT_DEBUG(NSUInteger a = [data cursor]);
        if(![data readEntry:&code entryType:&t length:&l value:&v]) break;
        LOG_DEBUG("RICOH Entry(%2d) %08X: %04X %2d %4d %08X / %d", count, a, code, t, l, v, _reviseOffset);
        
        switch(code) {
            case ricohExifCpuVer_code:
                if(l<sizeof(buf)) {
                    memset(buf, 0, sizeof(buf));
                    if([data read:buf length:l withOffset:v+_reviseOffset]) {
                        [dict setObject:[NSString stringWithUTF8String:buf] forKey:ricohExifCpuVer];
                    }
                } else {
                    LOG_WARN(@"ricohExifCpuVer is too long(%zd). ignored.", l);
                }
                break;
            case ricohExifCameraModel_code:
                [dict setObject:[NSNumber numberWithInteger:v] forKey:ricohExifCameraModel];
                break;
            case ricohExifCameraSerialNumber_code:
                if(l<sizeof(buf)-1) {
                    memset(buf, 0, sizeof(buf));
                    if([data read:buf length:l withOffset:v+_reviseOffset]) {
                        buf[l] = '\0';
                        [dict setObject:[NSString stringWithUTF8String:buf] forKey:ricohExifCameraSerialNumber];
                    }
                } else {
                    LOG_WARN(@"ricohExifCameraSerialNumber is too long(%zd). ignored.", l);
                }
                break;
            case ricohExifRicohTheta_code:
                {
                    [data pushCursor:v+_reviseOffset];
                    [dict addEntriesFromDictionary:[self parseRicohThetaTags:data]];
                    [data popCursor];
                }
                break;
            default:
                break;
        }
        count--;
    }
    return dict;
}

-(void)loadExif
{
    ExifRawData* data = nil;
    if(_representation) {
        data = [[ExifRawData alloc] initWithAssetRepresentation:_representation];
    } else if(_nsdata) {
        data = [[ExifRawData alloc] initWithNSData:_nsdata];
    } else {
        return;
    }
    
    if(data) {
        CFDataRef cfd = ::CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)[data bytes], [data length], kCFAllocatorNull);
        CGImageSourceRef imgsrc = CGImageSourceCreateWithData(cfd, 0);
        CFDictionaryRef cfdict = CGImageSourceCopyPropertiesAtIndex(imgsrc, 0, nil);
        
        NSDictionary* meta = (__bridge NSDictionary*)cfdict;
        _tiff = [meta objectForKey:(__bridge id)kCGImagePropertyTIFFDictionary];
        _exif = [meta objectForKey:(__bridge id)kCGImagePropertyExifDictionary];
        _gps  = [meta objectForKey:(__bridge id)kCGImagePropertyGPSDictionary];
        _ricoh = [self parseRicohMakerNote:data];

        CFRelease(cfd);
        CFRelease(imgsrc);
        if (cfdict) {
          CFRelease(cfdict);
        }
    }
}

-(id)initWithAsset:(ALAsset*)asset
{
    self = [super init];
    if (self) {
        _representation = [asset defaultRepresentation];
        _nsdata = nil;
        [self loadExif];
    }
    return self;
}

-(id)initWithNSData:(NSData *)data
{
    self = [super init];
    if (self) {
        _representation = nil;
        _nsdata = data;
        [self loadExif];
    }
    return self;
}

- (NSDictionary*)exif
{
    NSMutableDictionary* tags = [NSMutableDictionary dictionary];
    [tags addEntriesFromDictionary:_tiff];
    [tags addEntriesFromDictionary:_exif];
    [tags addEntriesFromDictionary:_gps];
    [tags addEntriesFromDictionary:_ricoh];
    
    return tags;
}

- (float)yaw
{
    NSNumber* v = [_ricoh objectForKey:ricohThetaCompass];
    if (!v) v = [_ricoh objectForKey:ricohThetaCompass_SHORT];
    return v? v.floatValue: NAN;
}

- (float)roll
{
    NSArray* v = [_ricoh objectForKey:ricohThetaZenith];
    if (v) return ((NSNumber*)[v objectAtIndex:0]).integerValue;
    v = [_ricoh objectForKey:ricohThetaZenith_SHORT];
    return v? 360.0-((NSNumber*)[v objectAtIndex:0]).integerValue: NAN;
}

- (float)pitch
{
    NSArray* v = [_ricoh objectForKey:ricohThetaZenith];
    if (v) return ((NSNumber*)[v objectAtIndex:1]).integerValue;
    v = [_ricoh objectForKey:ricohThetaZenith_SHORT];
    return v? ((NSNumber*)[v objectAtIndex:1]).integerValue: NAN;
}

- (NSUInteger)abnormalAcceleration
{
    NSNumber* v = [_ricoh objectForKey:ricohThetaAbnormalAcceleration];
    return (v)? v.unsignedIntegerValue: 0;
}

- (NSDictionary*)toDictionaryWithOverwriteExifTags:(NSDictionary *)overwriteExifTags
{
    NSMutableDictionary* exif = [NSMutableDictionary dictionaryWithDictionary:_exif];
    [exif addEntriesFromDictionary:overwriteExifTags];
    return [NSDictionary dictionaryWithObjectsAndKeys:
            _tiff, kCGImagePropertyTIFFDictionary,
            exif, kCGImagePropertyExifDictionary,
            _gps, kCGImagePropertyGPSDictionary,
            nil];
}

-(NSDictionary*)toDictionary
{
    return [self toDictionaryWithOverwriteExifTags:@{}];
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<RicohEXIF: %p", self];
    [s appendFormat:@" exifn=%@", _exif];
    [s appendString:@" >"];
    return s;
}

@end
