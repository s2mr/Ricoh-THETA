/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "ObjcLogger.h"

@interface PtpLogging : NSObject

+ (ObjcLogger*)l;

@end

#define PLOG_LOGGER          [PtpLogging l]
#define PLOG_VERBOSE(...)    [PLOG_LOGGER verbose:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_LOGS_VERBOSE()  [PLOG_LOGGER isVerboseEnabled]
#define PLOG_IF_VERBOSE(...) if (PLOG_LOGS_VERBOSE()) { PLOG_VERBOSE(__VA_ARGS__); }
#define PLOG_DEBUG(...)      [PLOG_LOGGER debug:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_LOGS_DEBUG()    [PLOG_LOGGER isDebugEnabled]
#define PLOG_IF_DEBUG(...)   if (PLOG_LOGS_DEBUG()) { PLOG_DEBUG(__VA_ARGS__); }
#define PLOG_INFO(...)       [PLOG_LOGGER info:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_LOGS_INFO()     [PLOG_LOGGER isInfoEnabled]
#define PLOG_IF_INFO(...)    if (PLOG_LOGS_INFO()) { PLOG_INFO(__VA_ARGS__); }
#define PLOG_WARN(...)       [PLOG_LOGGER warn:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_ERROR(...)      [PLOG_LOGGER error:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_ERROR_WITH_ERRNO(en, ...)   [PLOG_LOGGER error:__FILE__ line:__LINE__ e:en format:@"" __VA_ARGS__]
#define PLOG_FATAL(...)      [PLOG_LOGGER fatal:__FILE__ line:__LINE__ e:0 format:@"" __VA_ARGS__]
#define PLOG_FATAL_WITH_ERRNO(en, ...)   [PLOG_LOGGER fatal:__FILE__ line:__LINE__ e:en format:@"" __VA_ARGS__]
