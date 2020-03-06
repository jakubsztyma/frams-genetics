// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "log.h"
#include <common/nonstd_stdio.h>
#include "util-string.h"
#include "Convert.h"
#include <assert.h>

const char* LOG_LEVEL_ARRAY[] = { "[DEBUG] ", "", "[WARN] ", "[ERROR] ", "[CRITICAL] " };

void logPrintf_va(const char *obj, const char *method, int level, const char *msgf, va_list va)
{
	string buf = ssprintf_va(msgf, va);
	logMessage(obj, method, level, buf.c_str());
}

void logPrintf(const char *obj, const char *method, int level, const char *msgf, ...)
{
	va_list argptr;
	va_start(argptr, msgf);
	logPrintf_va(obj, method, level, msgf, argptr);
	va_end(argptr);
}

void log_printf(const char *msgf, ...)
{
	va_list argptr;
	va_start(argptr, msgf);
	logPrintf_va("Message", "printf", LOG_INFO, msgf, argptr);
	va_end(argptr);
}

const char* logLevelName(int level)
{
	assert((level>=LOG_MIN) && (level<=LOG_MAX));
	level = min(LOG_MAX, max(LOG_MIN, level));
	return LOG_LEVEL_ARRAY[level + 1];
}
