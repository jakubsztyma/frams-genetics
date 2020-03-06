// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _STRINGFILE_H_
#define _STRINGFILE_H_

#include "virtfile.h"
#include <common/nonstd_stl.h>
#include <string.h>

class StringFILE : public VirtFILE
{
protected:
	string& str;
	int pos;
public:
	StringFILE(string& s,int _pos=0): VirtFILE(""), str(s), pos(_pos) {}
	size_t Vread(void *ptr, size_t size, size_t nmemb);
	size_t Vwrite(const void *ptr, size_t size, size_t nmemb) { str.append((const char*)ptr, (int)(size*nmemb)); return nmemb; }
	int Veof() { return pos >= int(str.size()); }
	int Vputc(int c) { str += (char)c; return c; }
	int Vputs(const char *s) { str.append(s, (int)strlen(s)); return 0; }
	int Vgetc();
	char *Vgets(char *s, int size);
	int Vseek(long offset, int whence);
	long Vtell() { return pos; }
	int Vflush() { return 0; }

	const string& getString() { return str; }
};

/** this version owns the string object */
class StringFILE2 : public StringFILE
{
	string str;
public:
	StringFILE2(const string& s,int pos=0) :StringFILE(str,pos), str(s) {}
	StringFILE2() :StringFILE(str) {}
};

class StringFileSystem : public ChainFileSystem
{
public:
	StringFileSystem(VirtFileSystem *_chain = NULL):ChainFileSystem(_chain) {}
	VirtFILE *Vfopen(const char* path, const char*mode);
	static const char PREFIX[];
	static bool isStringPath(const char* path);
};

#endif
