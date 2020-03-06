// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _UTIL_STRINGS_H_
#define _UTIL_STRINGS_H_

#include "nonstd_stl.h"
#include <stdarg.h>

char* strmove(char *a, char *b); //strcpy that works well for overlapping strings ("Source and destination overlap")

string ssprintf(const char* format, ...);
string ssprintf_va(const char* format, va_list ap);

string stripExt(const string& filename); // strip extension from filename
string getFileExt(const string& filename); // get extension (starting with ".") from filename
string getFileDir(const string& filename); // get path component excluding filename ("" if no dir in file)
string stripFileDir(const string& filename); // strip path component from filename

#endif
