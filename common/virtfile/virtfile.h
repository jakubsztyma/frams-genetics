// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _VIRTFILE_H_
#define _VIRTFILE_H_

#include <stdio.h>
#include <stdarg.h>
#include <common/nonstd_dir.h>
#include <string>
#include <set>
using std::string;

#ifdef DLLEXPORTACTIVE  //defined in the project that makes the DLL
#define DLLEXP __declspec(dllexport)
#else
#ifdef __BORLANDC__ //assuming that all executables produced by borland use the DLL
#define DLLEXP __declspec(dllimport) //without dllimport, fields in objects would have separate instances in DLL and in EXE
#else
#define DLLEXP
#endif
#endif

class DLLEXP VirtFileSystem;

class DLLEXP VirtFILE
{
protected:
	string path;
public:
	virtual size_t Vread(void *ptr, size_t size, size_t nmemb) = 0;
	virtual size_t Vwrite(const void *ptr, size_t size, size_t nmemb) = 0;
	virtual int Veof() = 0;
	virtual int Vputc(int c) { unsigned char data = (unsigned char)c; return (Vwrite(&data, 1, 1) == 1) ? data : EOF; }
	virtual int Vputs(const char *s) = 0;
	virtual int Vgetc() { unsigned char data; if (Vread(&data, 1, 1) == 1) return data; else return EOF; }
	virtual int Vseek(long offset, int whence) = 0;
	virtual long Vtell() = 0;
	virtual void Vrewind() { Vseek(0, SEEK_SET); }
	virtual int Vflush() = 0;
	virtual char *Vgets(char *s, int size) = 0;
	virtual int Vprintf(const char *format, va_list args);
	int printf(const char *format, ...);
	virtual const char *VgetPath() { return path.c_str(); }
	virtual int getSize();
	VirtFILE(const char* _path) :path(_path) {}
	virtual ~VirtFILE();
	static VirtFILE *Vstdin, *Vstdout, *Vstderr;
	static void setVstdin(VirtFILE *);
	static void setVstdout(VirtFILE *);
	static void setVstderr(VirtFILE *);
	static VirtFILE* getVstdin();
	static VirtFILE* getVstdout();
	static VirtFILE* getVstderr();
	static VirtFileSystem *vfs;
	static void selectFileSystem(VirtFileSystem *s);
};

/** can be used directly or as a base class for implementations delegating VirtFILE calls to another VirtFILE object */
class DLLEXP DelegatedFILE : public VirtFILE
{
	VirtFILE *delegate;
public:
	size_t Vread(void *ptr, size_t size, size_t nmemb) { return delegate->Vread(ptr, size, nmemb); }
	size_t Vwrite(const void *ptr, size_t size, size_t nmemb) { return delegate->Vwrite(ptr, size, nmemb); }
	int Veof() { return delegate->Veof(); }
	int Vputc(int c) { return delegate->Vputc(c); }
	int Vputs(const char *s) { return delegate->Vputs(s); }
	int Vgetc() { return delegate->Vgetc(); }
	int Vseek(long offset, int whence) { return delegate->Vseek(offset, whence); }
	long Vtell() { return delegate->Vtell(); }
	void Vrewind() { delegate->Vrewind(); }
	int Vflush() { return delegate->Vflush(); }
	char *Vgets(char *s, int size) { return delegate->Vgets(s, size); }
	int Vprintf(const char *format, va_list args) { return delegate->Vprintf(format, args); }
	int getSize() { return delegate->getSize(); }
	// not overriden: VgetPath()

	DelegatedFILE(const char* _path, VirtFILE *_delegate) :VirtFILE(_path), delegate(_delegate) {}
	virtual ~DelegatedFILE() { if (delegate) delete delegate; delegate = NULL; }
};

class DLLEXP VirtDIR
{
public:
	virtual ~VirtDIR() {}
	virtual dirent* Vreaddir() { return NULL; }
};

class DLLEXP VirtFileSystem
{
public:
	virtual VirtFILE *Vfopen(const char* path, const char*mode);
	virtual bool Vfexists(const char* path);
	virtual VirtDIR *Vopendir(const char* path);
	virtual bool Vmkdir(const char* path);
	virtual bool Vmkdirs(const char* path);
	virtual bool Vdirexists(const char* path, bool is_writable);
};

/// base class for chained filesystems - redirect unimplemented calls -> chain
class DLLEXP ChainFileSystem : public VirtFileSystem
{
public:
	VirtFileSystem *chain;
	ChainFileSystem(VirtFileSystem *_chain = NULL);
	VirtFILE *Vfopen(const char* path, const char*mode);
	bool Vfexists(const char* path);
	VirtDIR *Vopendir(const char* path);
	virtual VirtDIR *internalopendir(const char* path) { return NULL; }
	bool Vmkdir(const char* path);
	bool Vmkdirs(const char* path);
	bool Vdirexists(const char* path, bool is_writable);

	class Dir : public VirtDIR
	{
		ChainFileSystem *first;
		VirtFileSystem *second;
		string path;
		std::set<string> duplicates;
		VirtDIR *dir;
	public:
		Dir(string _path, ChainFileSystem *_first, VirtFileSystem *_second) :first(_first), second(_second), path(_path), dir(NULL) {}
		~Dir();
		dirent* Vreaddir();
	};
};


DLLEXP VirtFILE *Vfopen(const char* path, const char*mode);
DLLEXP VirtDIR *Vopendir(const char* path);
DLLEXP bool Vfexists(const char* path);
DLLEXP bool Vmkdir(const char* path);
DLLEXP bool Vmkdirs(const char* path);
DLLEXP bool Vdirexists(const char* path, bool is_writable);

#ifdef VIRTFILE_OVERLOADING
//required for redefine_stdio.h and for building virtfile.dll, can also be used if someone prefers the standard f-functions instead of explicitly using virtfile objects

DLLEXP size_t fread(void *ptr, size_t size, size_t nmemb, VirtFILE* f);
DLLEXP size_t fwrite(const void *ptr, size_t size, size_t nmemb, VirtFILE* f);

//since we want our own feof(VirtFILE*) function and some systems unfortunately define feof as a macro, we need to #undef it. Same as in virtfile.cpp
#if defined _MSC_VER || defined __CYGWIN__ || defined SHP || defined __ANDROID__
#pragma push_macro("feof")
#undef feof
#endif
#if defined __BORLANDC__ //does not support #pragma push_macro/pop_macro 
#undef feof
#endif

DLLEXP int feof(VirtFILE* f);// {return f->Veof();}

//...and then restore the original macro:
#if defined _MSC_VER || defined __CYGWIN__ || defined SHP || defined __ANDROID__
#pragma pop_macro("feof")
#endif
#if defined __BORLANDC__
#define feof(__f)     ((__f)->flags & _F_EOF)
#endif


DLLEXP int fputc(int c, VirtFILE* f);
DLLEXP int fputs(const char *s, VirtFILE* f);
DLLEXP int fgetc(VirtFILE* f);
DLLEXP int fseek(VirtFILE* f, long offset, int whence);
DLLEXP int ftell(VirtFILE* f);
DLLEXP void rewind(VirtFILE* f);
DLLEXP int fflush(VirtFILE* f);
DLLEXP char *fgets(char *s, int size, VirtFILE* f);
DLLEXP int fprintf(VirtFILE* f, const char *format, ...);
DLLEXP int fclose(VirtFILE* f);

DLLEXP dirent* readdir(VirtDIR* d);
DLLEXP int closedir(VirtDIR* d);

#endif //VIRTFILE_OVERLOADING

#endif
