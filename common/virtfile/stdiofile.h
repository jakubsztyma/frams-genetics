// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _STDIOFILE_H_
#define _STDIOFILE_H_

#include "virtfile.h"
#include <common/nonstd_stdio.h>
#include <common/nonstd_dir.h>

class StdioFileSystem : public VirtFileSystem
{
public:
	VirtFILE *Vfopen(const char *path, const char *mode);
	bool Vfexists(const char* path);
	VirtDIR *Vopendir(const char* path);
	bool Vmkdir(const char* path) { return makeDirectory(path); }
	bool Vdirexists(const char* path, bool is_writable);
};

#ifdef USE_MFILE
class StdioFILE : public VirtFILE
{
protected:
	MFILE *file;
public:
	StdioFILE(MFILE *f) :VirtFILE("") { file = f; }
	StdioFILE(MFILE *f, const char* p) :VirtFILE(p) { file = f; }
#ifndef NO_STD_IN_OUT_ERR
	static void setStdio();
#endif
	size_t Vread(void *ptr, size_t size, size_t nmemb) { return mfread(ptr, size, nmemb, file); }
	size_t Vwrite(const void *ptr, size_t size, size_t nmemb) { return mfwrite(ptr, size, nmemb, file); }
	int Veof() { return mfeof(file); }
	int Vputs(const char *s) { return mfputs(s, file); }
	char *Vgets(char *s, int size) { return mfgets(s, size, file); }
	int Vseek(long offset, int whence) { return mfseek(file, offset, whence); }
	long Vtell() { return mftell(file); }
	int Vflush() { return 0; /*NOT IMPLEMENTED!*/ }

	~StdioFILE() { if (file) mfclose(file); }
};
#else
class StdioFILE : public VirtFILE
{
protected:
	FILE *file;
public:
	StdioFILE(FILE *f) :VirtFILE("") { file = f; }
	StdioFILE(FILE *f, const char* p) :VirtFILE(p) { file = f; }
#ifndef NO_STD_IN_OUT_ERR
	static void setStdio();
#endif
	size_t Vread(void *ptr, size_t size, size_t nmemb) { return fread(ptr, size, nmemb, file); }
	size_t Vwrite(const void *ptr, size_t size, size_t nmemb) { return fwrite(ptr, size, nmemb, file); }
	int Veof() { return feof(file); }
	int Vputc(int c) { return fputc(c, file); }
	int Vputs(const char *s) { return fputs(s, file); }
	int Vgetc() { return fgetc(file); }
	char *Vgets(char *s, int size) { return fgets(s, size, file); }
	int Vprintf(const char *format, va_list args) { return vfprintf(file, format, args); }
	int Vseek(long offset, int whence) { return fseek(file, offset, whence); }
	long Vtell() { return ftell(file); }
	void Vrewind() { rewind(file); }
	int Vflush() { return fflush(file); }

	~StdioFILE() { if (file) fclose(file); }
};
#endif


#ifdef _WIN32
#ifdef __BORLANDC__
typedef wDIR DIRTYPE;
#else
typedef WDIR DIRTYPE;
#endif
#else
typedef DIR DIRTYPE;
#endif

class StdioDIR : public VirtDIR
{
	DIRTYPE *dir;
#ifdef _WIN32
	dirent de; //only used to convert wide string names (wdirent) to utf8 (dirent)
#endif
public:
	StdioDIR(DIRTYPE *d) : dir(d) {}
	~StdioDIR()
	{
#ifdef _WIN32
		if (dir) wclosedir(dir);
#else
		if (dir) closedir(dir);
#endif
	}
	dirent* Vreaddir();
};

class StdioFILEDontClose : public StdioFILE
{
public:
#ifdef USE_MFILE
	StdioFILEDontClose(MFILE *f) : StdioFILE(f) {}
#else
	StdioFILEDontClose(FILE *f) : StdioFILE(f) {}
#endif
	~StdioFILEDontClose() { file = 0; }
};

class StdioFileSystem_autoselect : public StdioFileSystem
{
public:
	StdioFileSystem_autoselect()
	{
		VirtFILE::selectFileSystem(this);
	}
};

#endif
