// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "stringfile.h"
#include <stdio.h>
#include <errno.h> //EINVAL

size_t StringFILE::Vread(void *ptr, size_t size, size_t nmemb)
{
	int have = int(str.size()) - pos;
	if (have <= 0) return 0;
	int need = (int)(size*nmemb);
	if (need > have) { nmemb = have / size; need = (int)(size*nmemb); }
	memcpy(ptr, str.c_str() + pos, need);
	pos += need;
	return nmemb;
}

int StringFILE::Vgetc()
{
	if (pos >= int(str.size()))
		return EOF;
	else
		return str.operator[]((int)pos++);
}

char *StringFILE::Vgets(char *s, int size)
{
	int have = int(str.size()) - pos;
	if (have <= 0) return 0;
	if (size < 0) size = 0;
	if (have > size) have = size - 1;
	const char* src = str.c_str() + pos;
	char *dest = s;
	while (have-- > 0)
	{
		*(dest++) = *(src++); pos++;
		if (dest[-1] == '\n') break;
	}
	*dest = 0;
	return s;
}

int StringFILE::Vseek(long offset, int whence)
{
	switch (whence)
	{
	case SEEK_SET: pos = offset; break;
	case SEEK_CUR: pos += offset; break;
	case SEEK_END: pos = int(str.size()) - offset; break;
	default: return EINVAL;
	}
	if (pos < 0) pos = 0; else if (pos > int(str.size())) pos = int(str.size());
	return 0;
}

const char StringFileSystem::PREFIX[] = "string://";

bool StringFileSystem::isStringPath(const char* path)
{
	return !strncmp(path, PREFIX, sizeof(PREFIX) - 1);
}

VirtFILE *StringFileSystem::Vfopen(const char* path, const char*mode)
{
	if ((*mode == 'r') && isStringPath(path))
	{
		return new StringFILE2(string(path + sizeof(PREFIX) - 1));
	}
	return (chain != NULL) ? chain->Vfopen(path, mode) : NULL;
}
