/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpObjectContainer.h"
#include "logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static bool receiveBlock_proc(const void* _this, const char* data, size_t length);

@implementation PtpIpObjectContainer
@synthesize filename = _filename;
@synthesize fd = _fd;

- (id)init
{
    self = [super init];
    if (self) {
        _startDataBlock = nil;
        _receiveBlock = nil;
        _inboundStream.attachStreamListener(NULL, NULL);
        NSString* tmpl = [NSString stringWithFormat:@"%@PtpIpObjectContainer.XXXXXXXX", NSTemporaryDirectory()];
        char name[[tmpl length]+1];
        strncpy(name, [tmpl UTF8String], [tmpl length]+1);
        _fd = mkstemp(name);
        _filename = [NSString stringWithUTF8String:name];
        if(_fd<0) {
            LOG_FATAL("mkstemp is failed. %d", errno);
        }
    }
    return self;
}

- (id)initWithBlock:(void(^)(NSUInteger))startBlock onChunkReceived:(BOOL (^)(NSData *))onChunkBlock
{
    self = [super init];
    if (self) {
        _startDataBlock = startBlock;
        _receiveBlock = onChunkBlock;
        const void* _self = (__bridge const void*)self;
        _inboundStream.attachStreamListener(_self, receiveBlock_proc);
        _fd = -1;
        _filename = nil;
    }
    return self;
}

static bool receiveBlock_proc(const void* _this, const char* data, size_t length)
{
    PtpIpObjectContainer* _self = (__bridge PtpIpObjectContainer*)_this;
    
    if(_self) {
        if(data) {
            if(_self->_receiveBlock) {
                NSMutableData* d = [NSMutableData dataWithBytes:data length:length];
                return _self->_receiveBlock(d)? true: false;
            }
        } else {
            if(_self->_startDataBlock) {
                _self->_startDataBlock(length);
            }
            return true;
        }
    }
    return false;
}

-(::PTP::InboundDataStream&)inboundStream
{
    return _inboundStream;
}

- (void)close
{
    if(_fd>=0) {
        ::close(_fd);
        _fd = -1;
    }
}

- (void)unlink
{
    [self close];
    if(_filename) {
        ::unlink([_filename UTF8String]);
    }
    _filename = nil;
}

-(NSData*)readBytes:(NSInteger)length
{
    if(_fd<0) return nil;
    
    NSMutableData* d = [NSMutableData dataWithLength:length];
    void* p = [d mutableBytes];
    if(::read(_fd, p, length)!=length) {
        LOG_DEBUG("PtpIpObjectContainer::readBytes failed.(%d)", errno);
        return nil;
    }
    return d;
}

-(NSInteger)length
{
    if(_fd>=0) {
        struct stat s;
        if(::fstat(_fd, &s)==0) {
            return (NSInteger)s.st_size;
        }
    }
    return -1;
}

-(NSString*)description
{
    NSMutableString* s = [NSMutableString stringWithFormat:@"<PtpIpObjectContainer: %p", self];
    [s appendFormat:@" moden=%@", (_filename)? @"FileIO": @"Callback"];
    if(_filename) {
        if(_fd>=0) {
            [s appendFormat:@" fd=%d", _fd];
        } else {
            [s appendFormat:@" fd=<closed>"];
        }
        [s appendFormat:@" fileName=%@", _filename];
    } else {
        [s appendFormat:@" startDataBlock=%@", _startDataBlock];
        [s appendFormat:@" receiveBlock=%@", _receiveBlock];
    }
    [s appendString:@" >"];
    return s;
}

@end
