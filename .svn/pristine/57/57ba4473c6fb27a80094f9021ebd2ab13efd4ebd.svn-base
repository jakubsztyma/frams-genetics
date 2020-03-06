// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "util-file.h"
#include "nonstd_stdio.h"
#include "nonstd.h"
#include "log.h"
#ifdef USE_VIRTFILE
#include <common/virtfile/virtfile.h>
#endif

bool readCompleteFile(const char* filename, vector<char>& data, bool warn_on_missing_file)
{
	bool ok = false;
#ifdef USE_VIRTFILE
//	if (!isAbsolutePath(filename))
	{
		VirtFILE *f=Vfopen(filename,FOPEN_READ_BINARY);
		if (f)
		{
			int size=f->getSize();
			data.resize(size);
			int przeczytane = (int)f->Vread(&data[0], size, 1);
			ok = (przeczytane == 1);
			delete f;
		}
	}
//	else
#endif
	{
		MFILE *f = mfopen(filename, FOPEN_READ_BINARY);
		if (f)
		{
			int size = getFileSize(f);
			data.resize(size);
			int przeczytane = (int)mfread(&data[0], size, 1, f);
			mfclose(f);
			ok = (przeczytane == 1);
		}
	}
	if (warn_on_missing_file && !ok)
		logPrintf("stl-util", "readCompleteFile", LOG_WARN, "Couldn't open file '%s'", filename);
	return ok;
}

bool readCompleteFile(const char* filename, string& out, bool warn_on_missing_file)
{
	vector<char> data;
	if (readCompleteFile(filename, data, warn_on_missing_file))
	{
		out = string(&data[0], data.size());
		return true;
	}
	return false;
}

bool writeCompleteFile(const char* filename, const string& text, bool warn_on_fail)
{
#ifdef USE_VIRTFILE
	VirtFILE *f = Vfopen(filename, FOPEN_WRITE_BINARY);
	bool ok = f != NULL;
	if (f)
	{
		int zapisane = (int)f->Vwrite(text.c_str(), text.length(), 1);
		delete f;
		ok &= (zapisane == 1);
	}
#else
	MFILE *f = mfopen(filename, FOPEN_WRITE_BINARY);
	bool ok = f != NULL;
	if (f)
	{
		int zapisane = (int)mfwrite(text.c_str(), text.length(), 1, f);
		mfclose(f);
		ok &= (zapisane == 1);
	}
#endif
	if (warn_on_fail && !ok)
		logPrintf("stl-util", "writeCompleteFile", LOG_WARN, "Couldn't write file '%s'", filename);
	return ok;
}

bool writeCompleteFile(const char* filename, vector<char>& data, bool warn_on_fail)
{
	string s(&data[0], data.size());
	return writeCompleteFile(filename, s, warn_on_fail);
}
