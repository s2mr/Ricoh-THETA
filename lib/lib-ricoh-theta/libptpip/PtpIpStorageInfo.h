/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>

#ifdef __cplusplus
#   include "ptpip.h"
#else
#   include "codes_c.h"
#endif

/**
 *  Storage information
 */
@interface PtpIpStorageInfo : NSObject
{
    
}

#ifdef __cplusplus
-(id)initWithObject:(::PTP::StorageInfo_t*)source;
#else
-(id)initWithObject:(void*)source;
#endif

@property (readonly) uint64_t max_capacity;
@property (readonly) uint64_t free_space_in_bytes;
@property (readonly) uint32_t free_space_in_images;

@end
