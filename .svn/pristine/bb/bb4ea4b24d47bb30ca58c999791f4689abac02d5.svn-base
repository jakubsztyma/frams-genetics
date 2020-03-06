// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _VALIDITY_CHECKS_H_
#define _VALIDITY_CHECKS_H_

#include <frams/util/list.h>
#include <frams/util/sstring.h>

bool listIndexCheck(SList* list, int index, const char* msgobj, const char* msgfun);
SString stringCheck(SString& in, const char* msgobj, const char* msgfun, const char* msg, SString(*checker)(const SString& in) = NULL);

#endif
