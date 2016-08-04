/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#ifdef __cplusplus
#   include "bytestream.h"
#endif

@interface PtpIpObjectContainer : NSObject
{
    NSString* _filename;
    int _fd;
    void (^_startDataBlock)(NSUInteger totalLength);
    BOOL (^_receiveBlock)(NSData* data);
#ifdef __cplusplus
    ::PTP::InboundDataStream _inboundStream;
#endif
}
-(id)initWithBlock:(void(^)(NSUInteger))startBlock onChunkReceived:(BOOL(^)(NSData *))onChunkBlock;
#ifdef __cplusplus
-(::PTP::InboundDataStream&)inboundStream;
#endif

@property (readonly) NSString* filename;
@property (readonly) int fd;

-(void)close;

-(void)unlink;

-(NSData*)readBytes:(NSInteger)length;

-(NSInteger)length;

@end
