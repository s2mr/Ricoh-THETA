/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>
#import "ObjcLogger.h"

@interface LibLogging : NSObject

+ (ObjcLogger*)l;

@end

#define LIBLOG_LOGGER          [LibLogging l]
#ifdef DEBUG
#define LIBLOG_VERBOSE(...)    [LIBLOG_LOGGER verbose:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_LOGS_VERBOSE()  [LIBLOG_LOGGER isVerboseEnabled]
#define LIBLOG_IF_VERBOSE(...) if (LIBLOG_LOGS_VERBOSE()) { LIBLOG_VERBOSE(__VA_ARGS__); }
#define LIBLOG_DEBUG(...)      [LIBLOG_LOGGER debug:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_LOGS_DEBUG()    [LIBLOG_LOGGER isDebugEnabled]
#define LIBLOG_IF_DEBUG(...)   if (LIBLOG_LOGS_DEBUG()) { LIBLOG_DEBUG(__VA_ARGS__); }
#else   // #ifdef DEBUG
#define LIBLOG_VERBOSE(...)
#define LIBLOG_LOGS_VERBOSE()  (NO)
#define LIBLOG_IF_VERBOSE(...)
#define LIBLOG_DEBUG(...)
#define LIBLOG_LOGS_DEBUG()    (NO)
#define LIBLOG_IF_DEBUG(...)
#endif  // #ifdef DEBUG
#define LIBLOG_INFO(...)       [LIBLOG_LOGGER info:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_LOGS_INFO()     [LIBLOG_LOGGER isInfoEnabled]
#define LIBLOG_IF_INFO(...)    if (LIBLOG_LOGS_INFO()) { LIBLOG_INFO(__VA_ARGS__); }
#define LIBLOG_WARN(...)       [LIBLOG_LOGGER warn:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_ERROR(...)      [LIBLOG_LOGGER error:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_ERROR_WITH_ERRNO(en, ...)   [LIBLOG_LOGGER error:__FILE__ line:__LINE__ e:en format:__VA_ARGS__]
#define LIBLOG_FATAL(...)      [LIBLOG_LOGGER fatal:__FILE__ line:__LINE__ e:0 format:__VA_ARGS__]
#define LIBLOG_FATAL_WITH_ERRNO(en, ...)   [LIBLOG_LOGGER fatal:__FILE__ line:__LINE__ e:en format:__VA_ARGS__]
