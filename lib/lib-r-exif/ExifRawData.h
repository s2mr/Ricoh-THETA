/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import <AssetsLibrary/AssetsLibrary.h>

enum {
    IFDType_BYTE        = 1,
    IFDType_ASCII       = 2,
    IFDType_SHORT       = 3,
    IFDType_LONG        = 4,
    IFDType_RATIONAL    = 5,
    IFDType_UNDEF       = 7,
    IFDType_SLONG       = 9,
    IFDType_SRATIONAL   = 10,
};

#define EXIFRD_jpegSOI      (0xffd8)
#define EXIFRD_jpegEOI      (0xffd9)
#define EXIFRD_jpegSOS      (0xffda)
#define EXIFRD_jpegAPPX     (0xffe0)
#define EXIFRD_jpegAPP1     (0xffe1)
#define EXIFRD_jpegCOM      (0xfffe)
#define EXIFRD_ifdExif      (0x8769)
#define EXIFRD_ifdMakerNote (0x927c)
#define EXIFRD_MAGIC_ricohCameraInfo    ("[Ricoh Camera Info]")

@interface ExifRawData : NSObject
{
    NSMutableData* _data;
    NSUInteger  _baseAddress;        // 0x0C
    NSUInteger  _ifd0Address;        // 0x14
    NSUInteger  _cursor;
    NSUInteger  _exifIfdAddress;
    NSUInteger  _makernoteAddress;
    NSUInteger  _makernoteLength;
    NSInteger   _ricohCameraInfoAddress;
    NSInteger   _makernoteReviseOffset;
    
    BOOL _bigendian;
    NSMutableArray* _cursorStack;
}
@property (readonly) NSUInteger cursor;
@property (readonly) BOOL isBigEndian;
-(id)initWithAssetRepresentation:(ALAssetRepresentation*)assetRepresentation;
-(id)initWithNSData:(NSData*)data;
-(const void*)bytes;
-(NSUInteger)length;
-(void)pushCursor:(NSUInteger)nextCursor;
-(void)popCursor;
-(BOOL)read:(void*)buffer length:(NSUInteger)length;
-(BOOL)read:(void*)buffer length:(NSUInteger)length withOffset:(NSUInteger)offset;
-(NSUInteger)readWord;
-(NSUInteger)readDWord;
-(BOOL)readEntry:(NSUInteger*)code entryType:(NSUInteger*)entryType length:(NSUInteger*)length value:(NSUInteger*)value;
-(NSArray*)readEntry:(NSUInteger*)code entryType:(NSUInteger*)entryType length:(NSUInteger*)length;
-(BOOL)findTag:(NSUInteger*)count targetCode:(NSUInteger)targetCode entryType:(NSUInteger*)entryType length:(NSUInteger*)length value:(NSUInteger*)value;
-(BOOL)seekToIFD0;
-(BOOL)seekToExifIFD;
-(BOOL)seekToMakerNote;
-(BOOL)seekAndReviseMakerNoteOffset:(NSInteger*)reviseValue;
-(BOOL)seekToRicohCameraInfo:(NSInteger*)reviseValue;
-(BOOL)seekToMakerNoteSubTags:(NSInteger*)reviseValue parentTag:(NSUInteger)code;
-(BOOL)each:(BOOL(^)(NSUInteger index, NSUInteger code, NSUInteger length, NSUInteger offset))block;

-(BOOL)dumpIFD0Tgas;
-(BOOL)dumpRICOHMakerNoteTgas;
-(BOOL)dumpExifIfdTgas;
-(BOOL)dumpMarkers;
+(NSString*)hexdumpDescription:(const uint8_t*)data :(size_t)length;
+(void)hexdump:(const uint8_t*)data :(size_t)length;
+(NSString*)markerDescription:(NSUInteger)code;

@end
