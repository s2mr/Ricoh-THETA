/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import <Foundation/Foundation.h>

enum {
    OCLL_VERBOSE = 0,
    OCLL_DEBUG,
    OCLL_INFO,
    OCLL_WARN,
    OCLL_ERROR,
    OCLL_FATAL,
};

typedef void(*ObjcLogger_logwriter_t)(const char*fmt, va_list va);

@interface ObjcLogger : NSObject

+ (ObjcLogger*)newObjcLogger;
+ (ObjcLogger*)newObjcLogger:(const int)level;

- (void)verbose:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;
- (void)debug:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;
- (void)info:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;
- (void)warn:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;
- (void)error:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;
- (void)fatal:(const char*)file line:(int)line e:(int)e format:(NSString*)format, ...;

- (BOOL)isLevelEnabled:(const int)level;
- (BOOL)isVerboseEnabled;
- (BOOL)isDebugEnabled;
- (BOOL)isInfoEnabled;

- (int)loglevel;
- (void)loglevel:(const int)level;

//- (void)attachLogWriter:(void(*)(const char*, va_list))writer;
- (void)attachLogWriter:(ObjcLogger_logwriter_t)writer;


+ (void(*)(const char*, va_list))getNSLogWriter;

@end


#ifdef DEBUG
#define NSLogDebug(fmt, ...) NSLog((fmt), ##__VA_ARGS__)
#else
#define NSLogDebug(fmt, ...)
#endif
