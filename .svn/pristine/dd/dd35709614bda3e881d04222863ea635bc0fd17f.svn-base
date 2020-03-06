// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "validitychecks.h"
#include <frams/util/sstringutils.h>
#include <common/log.h>

bool listIndexCheck(SList* list, int index, const char* msgobj, const char* msgfun)
{
	int size = list->size();
	if ((index < 0) || (index >= size))
	{
		if (size>0)
			logPrintf(msgobj, msgfun, LOG_ERROR, "Invalid index %d (allowed range is 0..%d)", index, size - 1);
		else
			logPrintf(msgobj, msgfun, LOG_ERROR, "Invalid index %d (this list is empty)", index);
		return false;
	}
	return true;
}

SString stringCheck(SString& in, const char* msgobj, const char* msgfun, const char* msg, SString(*checker)(const SString& in))
{
	if (!checker)
		checker = trim;
	SString corrected = checker(in);
	if (corrected != in)
	{
		SString msg2 = SString(msg) + ": \"%s\" (adjusted to \"%s\")";
		logPrintf(msgobj, msgfun, LOG_WARN, msg2.c_str(), in.c_str(), corrected.c_str());
	}
	return corrected;
}
