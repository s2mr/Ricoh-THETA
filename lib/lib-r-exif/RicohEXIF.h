/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import <AssetsLibrary/AssetsLibrary.h>

extern NSString* ptpipPhotoUUID;

/**
 *  RICOH Exif Information
 */
@interface RicohEXIF : NSObject
{
    ALAssetRepresentation* _representation;
    NSData* _nsdata;
    NSDictionary* _tiff;
    NSDictionary* _exif;
    NSDictionary* _gps;
    NSDictionary* _ricoh;
}
@property (readonly) NSDictionary* exif;
@property (readonly) NSInteger reviseOffset;
@property (readonly) NSUInteger abnormalAcceleration;
@property (readonly) float yaw;
@property (readonly) float pitch;
@property (readonly) float roll;

-(id)initWithAsset:(ALAsset*)asset;

/**
 *  Initializes with image data
 *
 *  @param data Image data
 *
 *  @return Instance
 */
-(id)initWithNSData:(NSData*)data;

/**
 *  Output Exif information in NSDictionary format
 *
 *  @return Exif information converted to NSDictionary format
 */
-(NSDictionary*)toDictionary;
-(NSDictionary*)toDictionaryWithOverwriteExifTags:(NSDictionary*)overwriteExifTags;

@end

#ifdef LIBEXIFIO_LOG_MACROS
#   define IS_VERBOSE      (0)
#   define LOG_VERBOSE(...)
#   define LOG_DEBUG(...)   //printf(__VA_ARGS__); puts("")
#   define LOG_WARN(...)    NSLog(__VA_ARGS__)
#   define AT_DEBUG(k)      //k
#   define AT_VERBOSE(k)
#endif
