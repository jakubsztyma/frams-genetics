// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _COMMON_LOG_H_
#define _COMMON_LOG_H_

#include <stdarg.h>

extern const char* LOG_LEVEL_ARRAY[];
#define LOG_FORMAT "%s%s.%s: %s"
#define LOG_MULTILINE_CONTINUATION "..."

void logPrintf(const char *obj, const char *method, int level, const char *msgf, ...);
void logPrintf_va(const char *obj, const char *method, int level, const char *msgf, va_list va); //a different name than logPrintf - otherwise the compiler could confuse the "string" parameter with va_list and could call the wrong function
void log_printf(const char *msgf, ...); //a shorthand for printf (a different name again to avoid the risk of confusion with the two functions above. This would be unlikely but possible when the argument types would match)
void logMessage(const char *obj, const char *method, int level, const char *msg);
const char* logLevelName(int level);


//level (importance) of a message
#define LOG_MIN LOG_DEBUG
#define LOG_DEBUG -1 //debugging information, not needed for final users
#define LOG_INFO 0 //information
#define LOG_WARN 1 //warning or corrected error
#define LOG_ERROR 2 //uncorrected error, can cause malfunction
#define LOG_CRITICAL 3 //serious error, causes side effects. User should save what can be saved and restart the application
#define LOG_MAX LOG_CRITICAL

#endif
