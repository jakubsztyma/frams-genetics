// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _SSTRINGUTILS_H_
#define _SSTRINGUTILS_H_

#include "sstring.h"
#include <common/virtfile/virtfile.h>

/// return: 1=ok 0=error
int loadSString(const char* filename, SString& s, const char* framsgmodule = 0, const char* error = 0, bool remove_cr = true);
void loadSString(VirtFILE *f, SString& s, bool remove_cr = true);
bool loadSStringLine(VirtFILE* f, SString &s);

int quoteTilde(SString &target);
int unquoteTilde(SString &target);

bool strContainsOneOf(const char* str, const char* chars);
bool sstringQuote(SString& target);
SString sstringDelimitAndShorten(const SString &in, int maxlen, bool show_length, const SString& before, const SString& after);
const char* skipQuoteString(const char* txt, const char* limit);
int sstringUnquote(SString &target);

int strFindField(const SString& txt, const SString& name, int &end);
SString strGetField(const SString& txt, const SString& name);
void strSetField(SString& txt, const SString& name, const SString& value);

SString trim(const SString& s); ///< remove leading/trailing whitespace
SString concatPath(const SString& in1,const SString& in2); ///< concatenate path components inserting PATH_SEPARATOR_CHAR if not already present
bool removeCR(SString& s); ///< remove '\r' return true if changed
bool matchWildcard(const SString& word, const SString& pattern);///< '*' in pattern matches any substring
bool matchWildcardList(const SString& word, const SString& patterns);///< patterns is a list of patterns (separated by ',')
SString getUIDString(uint64_t uid, char prefix);
bool parseUIDString(const char* str, char prefix, uint64_t &uid, bool err);

#endif
