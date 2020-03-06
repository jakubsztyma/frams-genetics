// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "loggertostdout.h"
#include <common/console.h>
#include <assert.h>
#ifdef SHP
#include <FBaseSys.h> //AppLog
#else
#include <stdio.h>
#endif

static const char* default_log_level_ansicolor[] =
{ "[\033[2mDEBUG\033[0m] \033[2m", "", "[\033[33mWARN\033[0m] \033[1m", "[\033[1;31mERROR\033[0m] \033[1m", "[\033[1;31mCRITICAL\033[0m] \033[1m" };

const char** LoggerToStdout::default_log_level[] =
{ LOG_LEVEL_ARRAY, default_log_level_ansicolor };

const char* LoggerToStdout::default_log_format[] = //note trailing %s (so it's easy to append "\n" later)
{ LOG_FORMAT "%s", "%s%s.%s: %s\033[0m%s" };

LoggerToStdout::LoggerToStdout(int opts, VirtFILE *_file)
	:LoggerBase(opts), file(_file)
{
	log_format = default_log_format[console_color_mode];
	log_level = default_log_level[console_color_mode];
}

void LoggerToStdout::handleSingleLine(const char *obj, const char *method, int level, const char *msg)
{
#ifdef SHP
	AppLog(LOG_FORMAT "\n", logLevelName(level), obj, method, msg);
#else
	if (file)
		file->printf(LOG_FORMAT "\n", logLevelName(level), obj, method, msg);
	else
	{
		assert((level>=LOG_MIN) && (level<=LOG_MAX));
		level = min(LOG_MAX, max(LOG_MIN, level));
		printf(log_format, log_level[level + 1], obj, method, msg, "\n");
	}
#endif
}
