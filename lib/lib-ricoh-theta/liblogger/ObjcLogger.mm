/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "logger.h"
#import "ObjcLogger.h"
#import "ObjcLogger+Extension.h"

static void writeToNSLogv(const char*fmt, va_list va)
{
    NSLogv([[NSString alloc] initWithUTF8String:fmt], va);
}

@implementation ObjcLogger

+ (ObjcLogger*)newObjcLogger
{
    return [self newObjcLogger:OCLL_WARN];
}

+ (ObjcLogger*)newObjcLogger:(const int)level
{
    ObjcLogger* const logger = [[ObjcLogger alloc] init];
    [logger loglevel:level];
#ifdef DEBUG
    NSLog(@"ObjcLogger with level - %d is allocated.", level);
#endif
    return logger;
}

- (id)init
{
    self = [super init];
    if (self) {
        log.attachLogWriter(writeToNSLogv);
    }
    return self;
}

#define DEFINE_DELEGATOR(func__, level) \
    - (void)func__:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ... {\
        if (PTP::Logger::LL_##level >= log.loglevel()) {\
            va_list varglist;\
            va_start(varglist, format);\
            NSString* const fmt = [[NSString alloc] initWithFormat:format arguments:varglist];\
            va_end(varglist);\
            log.func__(file, line, e, "%s", [fmt UTF8String]);\
        }\
    }

DEFINE_DELEGATOR(verbose, VERBOSE)
DEFINE_DELEGATOR(debug, DEBUG)
DEFINE_DELEGATOR(info, INFO)
DEFINE_DELEGATOR(warn, WARN)
DEFINE_DELEGATOR(error, ERROR)
DEFINE_DELEGATOR(fatal, FATAL)

- (BOOL)isLevelEnabled:(const int)level
{
    return level >= log.loglevel();
}

- (BOOL)isVerboseEnabled
{
    return [self isLevelEnabled:OCLL_VERBOSE];
}

- (BOOL)isDebugEnabled
{
    return [self isLevelEnabled:OCLL_DEBUG];
}

- (BOOL)isInfoEnabled
{
    return [self isLevelEnabled:OCLL_INFO];
}

- (int)loglevel
{
    return log.loglevel();
}

- (void)loglevel:(int)level
{
    log.loglevel(level);
}

- (void)attachLogWriter:(ObjcLogger_logwriter_t const)writer
{
    if (NULL == writer) {
        log.attachLogWriter(writeToNSLogv);
    } else {
        log.attachLogWriter(writer);
    }
}

+ (void(*)(const char*, va_list))getNSLogWriter
{
    return writeToNSLogv;
}

@end
