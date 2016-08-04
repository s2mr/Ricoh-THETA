/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#ifdef DEFINE_EXIFTAG_STRING_SUBTANCE
// for ExifTags.m
#   define DEFINE_EXIF_KEY(_code_, _key_)  NSString* const _key_ = @#_key_
#else
#ifdef DEFINE_EXIFTAG_CODE 
// for RicohExif.mm, ExifRawData.mm
#   define DEFINE_EXIF_KEY(_code_, _key_)  extern NSString* const _key_; static const NSUInteger _key_##_code = _code_
#else
#   define DEFINE_EXIF_KEY(_code_, _key_)  extern NSString* const _key_
#endif
#endif

DEFINE_EXIF_KEY(0x0001, ricohExifRdc);                  // ASCII,   4 : "Rdc\0"
DEFINE_EXIF_KEY(0x0002, ricohExifCpuVer);               // ASCII,   8
DEFINE_EXIF_KEY(0x0003, ricohExifCameraModel);          // LONG,    1
DEFINE_EXIF_KEY(0x0005, ricohExifCameraSerialNumber);   // UNDEF,   16
DEFINE_EXIF_KEY(0x0006, ricohExifCardId);               // UNDEF,   16
DEFINE_EXIF_KEY(0x1001, ricohExifPlayInfo);             // UNDEF,   *
DEFINE_EXIF_KEY(0x2001, ricohExifAnalyzeScheme);        // UNDEF,   * : "[Ricoh Camera Info]\0", (uint16_t)count, {(uint16_t)id, (uint16_t)type, (uint32_t)code} * count
DEFINE_EXIF_KEY(0x4001, ricohExifRicohTheta);           // UNDEF,   *

DEFINE_EXIF_KEY(0x0001, ricohThetaPanoramaImageType);    // SHORT,       1 : 1:Equi
DEFINE_EXIF_KEY(0x0002, ricohThetaHdrMode);              // SHORT,       1 : 0:OFF, 1:ON
DEFINE_EXIF_KEY(0x0003, ricohThetaZenith);               // SRATIONAL,   2 : roll, pitch [degree]
DEFINE_EXIF_KEY(0x0004, ricohThetaCompass);              // RATIONAL,    1 : yaw [degree]
DEFINE_EXIF_KEY(0x0005, ricohThetaAbnormalAcceleration); // SHORT,       1 : 0:no 1:collided 2:free-fall
DEFINE_EXIF_KEY(0x0101, ricohThetaFilmISO);              // SHORT,       4
DEFINE_EXIF_KEY(0x0102, ricohThetaAperture);             // SRATIONAL,   2
DEFINE_EXIF_KEY(0x0103, ricohThetaExposureTime);         // RATIONAL,    2
DEFINE_EXIF_KEY(0x0104, ricohThetaSensorSerial1);        // ASCII,       *
DEFINE_EXIF_KEY(0x0105, ricohThetaSensorSerial2);        // ASCII,       *

DEFINE_EXIF_KEY(0x0006, ricohThetaZenith_SHORT);         // SHORT,       2 : roll, pitch [degree]
DEFINE_EXIF_KEY(0x0007, ricohThetaCompass_SHORT);        // SHORT,       1 : yaw [degree]

#undef DEFINE_EXIF_KEY
