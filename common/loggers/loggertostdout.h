// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _STDOUT_LOGGER_H_
#define _STDOUT_LOGGER_H_

#include "loggers.h"
#include <common/virtfile/virtfile.h>

class LoggerToStdout : public LoggerBase
{
	VirtFILE *file;
public:
	LoggerToStdout(int opts = 0, VirtFILE *_file = NULL);
	void handleSingleLine(const char *obj, const char *method, int level, const char *msg);

	static const char* default_log_format[];
	static const char** default_log_level[];
	const char** log_level;
	const char* log_format;
};

#endif
