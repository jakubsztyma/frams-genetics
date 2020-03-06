// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _UTIL_FILES_H_
#define _UTIL_FILES_H_

#include "nonstd_stl.h"

bool readCompleteFile(const char* filename, vector<char>& data, bool warn_on_missing_file = true);
bool readCompleteFile(const char* filename, string& out, bool warn_on_missing_file = true);
bool writeCompleteFile(const char* filename, const std::string& text, bool warn_on_fail = true);
bool writeCompleteFile(const char* filename, vector<char>& data, bool warn_on_fail = true);

#endif
