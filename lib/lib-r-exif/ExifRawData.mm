/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#define DEFINE_EXIFTAG_CODE
#define LIBEXIFIO_LOG_MACROS
#import "RicohEXIF.h"
#import "ExifTags.h"
#import "ExifRawData.h"


@implementation ExifRawData
@synthesize isBigEndian = _bigendian;
@synthesize cursor = _cursor;

+(NSString*)hexdumpDescription:(const uint8_t*)data :(size_t)length
{
    NSMutableString* d = [[NSMutableString alloc] init];
    BOOL lastLF = NO;
    
    for(int p=0; p<length; p+=16) {
        lastLF = NO;
        int i;
        [d appendFormat:@"%08X:", p];
        for(i=0; (i<16) && (p+i<length); i++) {
            [d appendFormat:@" %02X", data[p+i]];
        }
        if(length>=16) {
            for(; i<16; i++) {
                [d appendString:@"   "];
            }
        }
        [d appendString:@" : "];
        for(i=0; (i<16) && (p+i<length); i++) {
            uint8_t c = data[p+i];
            [d appendFormat:@"%c", isprint(c)? c: '.'];
        }
        if(i==16) {
            [d appendString:@"\n"];
            lastLF = YES;
        }
    }
    if(length>=16 && !lastLF) {
        [d appendString:@"\n"];
    }
    
    return d;
}

+(void)hexdump:(const uint8_t*)data :(size_t)length
{
#if defined(DEBUG_DUMP_JPEG) && DEBUG_DUMP_JPEG
    printf("%s\n", [[ExifRawData hexdumpDescription:data :length] UTF8String]);
#endif
}


-(void)initializeMembers
{
    _cursor = 0;
    _baseAddress = 0;
    _ifd0Address = 0;
    _exifIfdAddress = 0;
    _makernoteAddress = 0;
    _makernoteReviseOffset = INT32_MAX;
    _ricohCameraInfoAddress = 0;
    _cursorStack = [[NSMutableArray alloc] init];
    _bigendian = YES;
    [self seekToIFD0];
    [ExifRawData hexdump:(const uint8_t*)[_data bytes] :[_data length]<1536? [_data length]: 1536];
}

-(id)initWithAssetRepresentation:(ALAssetRepresentation *)assetRepresentation
{
    self = [super init];
    if (self) {
        NSUInteger len = (NSUInteger)[assetRepresentation size];
        if(len>65536) len = 65536;
        _data = [[NSMutableData alloc] initWithLength:len];
        [assetRepresentation getBytes:(uint8_t*)[_data mutableBytes] fromOffset:0 length:len error:NULL];
        [self initializeMembers];
    }
    return self;
}

-(id)initWithNSData:(NSData *)data
{
    self = [super init];
    if (self) {
        if([data length]>65536) {
            _data = [[NSMutableData alloc] initWithBytes:[data bytes] length:65536];
        } else {
            _data = [[NSMutableData alloc] initWithData:data];
        }
        [self initializeMembers];
    }
    return self;
}

-(const void*)bytes
{
    return [_data bytes];
}

-(NSUInteger)length
{
    return [_data length];
}

-(void)pushCursor:(NSUInteger)nextCursor
{
    [_cursorStack addObject:[NSNumber numberWithInteger:_cursor]];
    _cursor = nextCursor;
}

-(void)popCursor
{
    if([_cursorStack count]>0) {
        _cursor = [(NSNumber*)[_cursorStack lastObject] unsignedIntegerValue];
        [_cursorStack removeLastObject];
    }
}

-(BOOL)read:(void*)buffer from:(NSUInteger)from length:(NSUInteger)length
{
    if([_data length]>from+length) {
        NSRange r = NSMakeRange(from, length);
        [_data getBytes:buffer range:r];
        return YES;
    }
    LOG_WARN(@"Not enough data. length=%zd, required=[%zd, %zd]", [_data length], from, length);
    return NO;
}

-(BOOL)read:(void*)buffer length:(NSUInteger)length
{
    if([_data length]>_cursor+length) {
        NSRange r = NSMakeRange(_cursor, length);
        [_data getBytes:buffer range:r];
        _cursor += length;
        return YES;
    }
    LOG_WARN(@"Not enough data. length=%zd, required=[%zd, %zd]", [_data length], _cursor, length);
    return NO;
}

-(BOOL)read:(void*)buffer length:(NSUInteger)length withOffset:(NSUInteger)offset
{
    offset += _baseAddress;
    if([_data length]>offset+length) {
        NSRange r = NSMakeRange(offset, length);
        [_data getBytes:buffer range:r];
        return YES;
    }
    LOG_WARN(@"Not enough data. length=%zd, required=[%zd+%zd, %zd]", [_data length], _baseAddress, offset-_baseAddress, length);
    return NO;
}

-(NSUInteger)readWord
{
    unsigned short v = 0;
    [self read:&v length:2];
    
    if(_bigendian) v = ntohs(v);
    return v;
}

-(NSUInteger)readDWord
{
    unsigned long v = 0;
    [self read:&v length:4];
    
    if(_bigendian) v = ntohl(v);
    return v;
}

-(BOOL)readEntry:(NSUInteger*)code entryType:(NSUInteger*)entryType length:(NSUInteger*)length value:(NSUInteger*)value
{
    if([_data length]>_cursor+12) {
        *code = [self readWord];
        *entryType = [self readWord];
        *length = [self readDWord];
        *value = [self readDWord];
        return YES;
    }
    LOG_WARN(@"Not enough data. length=%zd, required=[%zd, 12]", [_data length], _cursor);
    return NO;
}

-(NSArray*)readEntry:(NSUInteger*)code entryType:(NSUInteger*)entryType length:(NSUInteger*)length
{
    NSUInteger value = 0;
    BOOL result = [self readEntry:code entryType:entryType length:length value:&value];
    if (!result) return nil;
    
    if (*length==0) {
        return @[];
    }
    size_t byteLength = *length;
    size_t unitLength = 1;
    switch (*entryType) {
        case IFDType_BYTE:
        case IFDType_ASCII:
        case IFDType_UNDEF:
        default:
            break;
        case IFDType_SHORT:
            unitLength = 2;
            break;
        case IFDType_LONG:
        case IFDType_SLONG:
            unitLength = 4;
            break;
        case IFDType_SRATIONAL:
        case IFDType_RATIONAL:
            unitLength = 8;
            break;
    }
    byteLength *= unitLength;
    
    uint8_t buf[byteLength];
    
    if (byteLength<=4) {
        [self pushCursor:_cursor-4];
        [self read:buf length:byteLength];
        [self popCursor];
    } else {
        [self read:buf length:byteLength withOffset:value+_makernoteReviseOffset];
    }
    
    switch (*entryType) {
        case IFDType_BYTE:
        case IFDType_UNDEF:
        case IFDType_ASCII:
            return @[[NSData dataWithBytes:buf length:byteLength]];
        default:
            break;
    }
    
    NSMutableArray* arr = [NSMutableArray array];
    for (int i=0; i<*length; ++i) {
        switch (*entryType) {
            case IFDType_SHORT:
            {
                short v;
                memcpy(&v, buf+i*unitLength, unitLength);
                if (_bigendian) v = ntohs(v);
                [arr addObject:[NSNumber numberWithInteger:v]];
                break;
            }
                
            case IFDType_LONG:
            {
                unsigned long v;
                memcpy(&v, buf+i*unitLength, unitLength);
                if (_bigendian) v = ntohl(v);
                [arr addObject:[NSNumber numberWithInteger:v]];
                break;
            }
                
            case IFDType_SLONG:
            {
                long v;
                memcpy(&v, buf+i*unitLength, unitLength);
                if (_bigendian) v = ntohl(v);
                [arr addObject:[NSNumber numberWithInteger:v]];
                break;
            }
                
            case IFDType_SRATIONAL:
            {
                long v1, v2;
                memcpy(&v1, buf+i*unitLength, unitLength/2);
                memcpy(&v2, buf+i*unitLength+unitLength/2, unitLength/2);
                if (_bigendian) {
                    v1 = ntohl(v1);
                    v2 = ntohl(v2);
                }
                [arr addObject:[NSNumber numberWithFloat:(float)v1/(float)v2]];
                break;
            }
                
            case IFDType_RATIONAL:
            {
                unsigned long v1, v2;
                memcpy(&v1, buf+i*unitLength, unitLength/2);
                memcpy(&v2, buf+i*unitLength+unitLength/2, unitLength/2);
                if (_bigendian) {
                    v1 = ntohl(v1);
                    v2 = ntohl(v2);
                }
                [arr addObject:[NSNumber numberWithFloat:(float)v1/(float)v2]];
                break;
            }
                
            default:
                break;
        }
    }
    return arr;
}

-(BOOL)seekToIFD0
{
    if(_ifd0Address) {
        _cursor = _ifd0Address;
        return YES;
    }
    _bigendian = YES;
    _cursor = 0;
    NSUInteger soi = [self readWord];
    if(soi!=EXIFRD_jpegSOI) {
        LOG_WARN(@"SOI not found.");
        return NO;
    }
    
    int i;
    for(i=0; i<32; i++) {
        NSUInteger mark = [self readWord];
        LOG_VERBOSE("seekToIFD0: mark=%04x", mark);
        if((mark==EXIFRD_jpegEOI) || (mark==EXIFRD_jpegSOI) || (mark==EXIFRD_jpegSOS)) {
            LOG_WARN(@"SOI/EOI/SOS found. aborted.");
            break;
        }
        if(mark==EXIFRD_jpegAPP1) {
            _cursor += 2;   // length parameter. drop.
            
            char sig[6];
            if(![self read:sig length:6]) {
                LOG_WARN(@"Read APP1-signature failed.");
                break;
            }
            if(memcmp(sig, "Exif\0\0", 6)!=0) {
                LOG_WARN(@"APP1 is not EXIF. aborted.");
                break;
            }
            _baseAddress = _cursor;
            
            if(![self read:sig length:2]) {
                LOG_WARN(@"Read endian signature failed.");
                break;
            }
            _bigendian = ((sig[0]=='M') && (sig[1]=='M'))? YES: NO;
            
            NSUInteger v = [self readWord];
            if(v!=0x2a) {
                LOG_WARN(@"Invalid TIFF ID(%04tX). 002A is required.", v);
                break;
            }
            v = [self readDWord];
            _ifd0Address = _cursor + v - 8;
            return YES;
        }
        if((0xff00<=mark) && (mark<=0xfffe)) {
            // APP0, APP2~APPD, etc.
            NSUInteger len = [self readWord];
            _cursor += len - 2;
            continue;
        }
        if(mark==0xffff) continue;
        LOG_WARN(@"seekToIFD0: Unknown marker(%04tX) found. aborted.", mark);
        break;
    }
    LOG_WARN(@"APP1 marker not found.");
    return NO;
}

-(BOOL)findTag:(NSUInteger*)count targetCode:(NSUInteger)targetCode entryType:(NSUInteger*)entryType length:(NSUInteger*)length value:(NSUInteger*)value
{
    NSUInteger c=0, t=0, l=0, v=0;
    
    while((*count)>0) {
        AT_VERBOSE(NSUInteger a = _cursor);
        if(![self readEntry:&c entryType:&t length:&l value:&v]) return NO;
        LOG_VERBOSE("IDF Entry(%2d) %08X: %04X %2d %4d %08X", *count, a, c, t, l, v);
        if(c==targetCode) {
            *entryType = t;
            *length = l;
            *value = v;
            return YES;
        }
        (*count)--;
    }
    return NO;
}

-(BOOL)seekToExifIFD
{
    if(_exifIfdAddress) {
        _cursor = _exifIfdAddress;
        return YES;
    }
    
    if([self seekToIFD0]) {
        NSUInteger count = [self readWord];
        NSUInteger entryType, length, value;
        if([self findTag:&count targetCode:EXIFRD_ifdExif entryType:&entryType length:&length value:&value]) {
            _cursor = _exifIfdAddress = _baseAddress + value;
            return YES;
        }
    }
    LOG_WARN(@"IFD::EXIF is not found.");
    return NO;
}

-(BOOL)seekToMakerNote
{
    if(_makernoteAddress) {
        _cursor = _makernoteAddress;
        return YES;
    }
    
    if([self seekToExifIFD]) {
        NSUInteger count = [self readWord];
        NSUInteger entryType, length, value;
        if([self findTag:&count targetCode:EXIFRD_ifdMakerNote entryType:&entryType length:&length value:&value]) {
            _makernoteAddress = _baseAddress + value;
            _makernoteLength = length;
#ifdef LOG_VERBOSE
            if(IS_VERBOSE) {
                count--;
                LOG_VERBOSE("-- others --");
                [self findTag:&count targetCode:-1 entryType:&entryType length:&length value:&value];
                LOG_VERBOSE("------------");
            }
#endif
            _cursor = _makernoteAddress;
            return YES;
        }
    }
    LOG_WARN(@"IFD::EXIF::MakerNote is not found.");
    return NO;
}

-(BOOL)seekAndReviseMakerNoteOffset:(NSInteger*)reviseValue
{
    char makerid[8];
    
    if(![self seekToMakerNote]) return NO;
    if(_makernoteReviseOffset!=INT32_MAX) {
        _cursor += sizeof(makerid);
        *reviseValue = _makernoteReviseOffset;
        return YES;
    }
    
    if(![self read:makerid length:sizeof(makerid)]) return NO;
    if(memcmp(makerid, "Ricoh\0\0\0", sizeof(makerid))!=0) {
        LOG_WARN(@"Not a RICOH image.");
        return NO;
    }
    NSUInteger c = [self readWord];
    NSUInteger t=0, l=0, v=0;
    if(![self findTag:&c targetCode:ricohExifAnalyzeScheme_code entryType:&t length:&l value:&v]) return NO;
    
    static const char sigkey[] = EXIFRD_MAGIC_ricohCameraInfo;
    char sig[sizeof(sigkey)];
    NSInteger originalAddress = v + _baseAddress;
    if (![self read:sig from:originalAddress length:sizeof(sig)]) {
        LOG_WARN(@"Invalid offset value:%zd", originalAddress);
        return NO;
    }
    if(memcmp(sig, sigkey, sizeof(sigkey))==0) {
        [self seekToMakerNote];
        _cursor += sizeof(makerid);
        _makernoteReviseOffset = *reviseValue = 0;
        _ricohCameraInfoAddress = originalAddress + _makernoteReviseOffset;
        return YES;
    }
    char* m = (char*)[_data bytes] + _makernoteAddress;
    char* e = m + _makernoteLength - sizeof(sigkey);
    for(; m<e; m++) {
        if(memcmp(m, sigkey, sizeof(sigkey))==0) {
            [self seekToMakerNote];
            _cursor += sizeof(makerid);
            _makernoteReviseOffset = *reviseValue = (m - (char*)[_data bytes]) - originalAddress;
            _ricohCameraInfoAddress = originalAddress + _makernoteReviseOffset;
            return YES;
        }
    }
    
    LOG_WARN(@"`%s` is not found.", EXIFRD_MAGIC_ricohCameraInfo);
    return NO;
}

-(BOOL)seekToRicohCameraInfo:(NSInteger*)reviseValue
{
    if(![self seekAndReviseMakerNoteOffset:reviseValue]) return NO;
    _cursor = _ricohCameraInfoAddress + sizeof(EXIFRD_MAGIC_ricohCameraInfo);
    return YES;
}

-(BOOL)seekToMakerNoteSubTags:(NSInteger*)reviseValue parentTag:(NSUInteger)code
{
    if(![self seekAndReviseMakerNoteOffset:reviseValue]) return NO;
    NSUInteger count = [self readWord];
    NSUInteger entryType, length, value;
    if([self findTag:&count targetCode:code entryType:&entryType length:&length value:&value]) {
        _cursor = _baseAddress + _makernoteReviseOffset + value;
        return YES;
    }
    return NO;
}

-(BOOL)dumpTags:(NSString*)ifdName;
{
    NSUInteger c=0, t=0, l=0, v=0, a;
    NSUInteger i = [self readWord];
    NSMutableString* x = [[NSMutableString alloc] initWithFormat:@"%@: %zd entries.\n", ifdName, i];
    
    for(; i>0; i--) {
        a = _cursor;
        if(![self readEntry:&c entryType:&t length:&l value:&v]) return NO;
        [x appendFormat:@"(%2zd) %08tX: %04tX %2zd %6zd %08tX\n", i, a, c, t, l, v];
    }
    NSLog(@"\n%@", x);
    return YES;
}

-(BOOL)dumpIFD0Tgas
{
    if([self seekToIFD0]) {
        [self dumpTags:@"IFD0 Tags"];
        return YES;
    }
    return NO;
}

-(BOOL)dumpExifIfdTgas
{
    if([self seekToExifIFD]) {
        [self dumpTags:@"EXIF Tags"];
        return YES;
    }
    return NO;
}

-(BOOL)dumpRICOHMakerNoteTgas
{
    if([self seekToMakerNote]) {
        _cursor += 8;
        [self dumpTags:@"RICOH MakerNote Tags"];
        return YES;
    }
    return NO;
}

-(BOOL)each:(BOOL(^)(NSUInteger index, NSUInteger code, NSUInteger length, NSUInteger offset))block
{
    _cursor = 0;
    
    for(NSUInteger i=0; i<256; i++) {
        if(_cursor>= [_data length]) break;
        NSUInteger mark = [self readWord];
        NSUInteger len;
        switch(mark) {
            case EXIFRD_jpegSOS:
                len = [_data length] - _cursor;
                if(!block(i, mark, len, _cursor)) return NO;
                return YES;
            case EXIFRD_jpegSOI:
            case EXIFRD_jpegEOI:
            case 0xFFFF:
                if(!block(i, mark, 0, _cursor)) return NO;
                break;
            default:
                if(mark>=0xFF00) {
                    len = [self readWord];
                    if(!block(i, mark, len, _cursor-2)) return NO;
                    _cursor += len - 2;
                } else {
                    block(i, mark, 0, _cursor-2);
                    return NO;
                }
                break;
        }
    }
    return YES;
}

-(BOOL)dumpMarkers
{
    return [self each:^BOOL(NSUInteger index, NSUInteger code, NSUInteger length, NSUInteger offset) {
        AT_DEBUG(
            const char* d = [[ExifRawData markerDescription:code] UTF8String];
            if(length==0) {
                LOG_DEBUG("%2d %s", index, d);
            } else {
                LOG_DEBUG("%2d %s - %d", index, d, length);
            }
        );
        return YES;
    }];
}

+(NSString*)markerDescription:(NSUInteger)code
{
    switch(code) {
        case EXIFRD_jpegSOI:    return @"SOI ";
        case EXIFRD_jpegEOI:    return @"EOI ";
        case EXIFRD_jpegSOS:    return @"SOS ";
        case 0xffdb:            return @"DQT ";
        case 0xffdc:            return @"DNL ";
        case 0xffdd:            return @"DRI ";
        case 0xffc4:            return @"DHT ";
        case 0xffc0: case 0xffc1: case 0xffc2: case 0xffc3:              case 0xffc5: case 0xffc6: case 0xffc7:
        case 0xffc8: case 0xffc9: case 0xffca: case 0xffcb: case 0xffcc: case 0xffcd: case 0xffce: case 0xffcf:
            return [NSString stringWithFormat:@"SOF%tx", code - 0xffc0];
        case 0xffe0: case 0xffe1: case 0xffe2: case 0xffe3: case 0xffe4: case 0xffe5: case 0xffe6: case 0xffe7:
        case 0xffe8: case 0xffe9: case 0xffea: case 0xffeb: case 0xffec: case 0xffed: case 0xffee: case 0xffef:
            return [NSString stringWithFormat:@"APP%tX", code - 0xffe0];
        case EXIFRD_jpegCOM:    return @"COM ";
        case 0xffff:            return @"FFFF";
    }
    return [NSString stringWithFormat:@"%04tX", code];
}


@end
