// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "virtfile.h"
#include <common/util-string.h>

VirtFILE *VirtFILE::Vstdin = NULL;
VirtFILE *VirtFILE::Vstdout = NULL;
VirtFILE *VirtFILE::Vstderr = NULL;

VirtFileSystem *VirtFILE::vfs = NULL;

//#define DEBUG_VIRTFILE

VirtFILE *Vfopen(const char* path, const char* mode)
{
#ifdef DEBUG_VIRTFILE
	printf("VirtFILE::Vfopen %s %s (vfs=%p)\n",path,mode,VirtFILE::vfs);
#endif
	return VirtFILE::vfs ? VirtFILE::vfs->Vfopen(path, mode) : NULL;
}

VirtDIR *Vopendir(const char* path)
{
#ifdef DEBUG_VIRTFILE
	printf("VirtFILE::Vfopendir %s (vfs=%p)\n",path,VirtFILE::vfs);
#endif
	return VirtFILE::vfs ? VirtFILE::vfs->Vopendir(path) : NULL;
}

bool Vfexists(const char* path)
{
	return VirtFILE::vfs ? VirtFILE::vfs->Vfexists(path) : false;
}

bool Vdirexists(const char* path, bool is_writable)
{
	return VirtFILE::vfs ? VirtFILE::vfs->Vdirexists(path, is_writable) : false;
}

bool Vmkdir(const char* path)
{
	return VirtFILE::vfs ? VirtFILE::vfs->Vmkdir(path) : false;
}

bool Vmkdirs(const char* path)
{
	return VirtFILE::vfs ? VirtFILE::vfs->Vmkdirs(path) : false;
}

VirtFILE::~VirtFILE()
{}

void VirtFILE::selectFileSystem(VirtFileSystem *s)
{
	vfs = s;
#ifdef DEBUG_VIRTFILE
	::printf("VirtFILE::selectFileSystem: %p := %p\n",vfs,s);
#endif
}

int VirtFILE::Vprintf(const char *format, va_list args)
{
	string s = ssprintf_va(format, args);
	return (int)Vwrite(s.c_str(), 1, s.size());
}

int VirtFILE::printf(const char *format, ...)
{
	int ret; va_list argptr;
	va_start(argptr, format);
	ret = Vprintf(format, argptr);
	va_end(argptr);
	return ret;
}

int VirtFILE::getSize()
{
	auto saved_pos = Vtell();
	Vseek(0, SEEK_END);
	int size = (int)Vtell();
	Vseek(saved_pos, SEEK_SET);
	return size;
}

void VirtFILE::setVstdin(VirtFILE *f) { Vstdin = f; }
void VirtFILE::setVstdout(VirtFILE *f) { Vstdout = f; }
void VirtFILE::setVstderr(VirtFILE *f) { Vstderr = f; }
VirtFILE* VirtFILE::getVstdin() { return Vstdin; }
VirtFILE* VirtFILE::getVstdout() { return Vstdout; }
VirtFILE* VirtFILE::getVstderr() { return Vstderr; }
//////////////////////////////////////////////////////////////////////////

// base class only returns NULL/false/not supported - implementations perform the actual work
VirtFILE* VirtFileSystem::Vfopen(const char* path, const char* mode) { return NULL; }
bool VirtFileSystem::Vfexists(const char* path) { return false; }
VirtDIR* VirtFileSystem::Vopendir(const char* path) { return NULL; }
bool VirtFileSystem::Vmkdir(const char* path) { return false; }
bool VirtFileSystem::Vdirexists(const char* path, bool is_writable) { return false; }

//////////////////////////////////////////////////////////////////////////



size_t fread(void *ptr, size_t size, size_t nmemb, VirtFILE* f) { return f->Vread(ptr, size, nmemb); }
size_t fwrite(const void *ptr, size_t size, size_t nmemb, VirtFILE* f) { return f->Vwrite(ptr, size, nmemb); }


//since we want our own feof(VirtFILE*) function and some systems unfortunately define feof as a macro, we need to #undef it. Same as in virtfile.h
#if defined _MSC_VER || defined __CYGWIN__ || defined SHP || defined __ANDROID__
#pragma push_macro("feof")
#undef feof
#endif
#if defined __BORLANDC__ //does not support #pragma push_macro/pop_macro 
#undef feof
#endif

int feof(VirtFILE* f) { return f->Veof(); }

//...and then restore the original macro:
#if defined _MSC_VER || defined __CYGWIN__ || defined SHP || defined __ANDROID__
#pragma pop_macro("feof")
#endif
#if defined __BORLANDC__
#define feof(__f)     ((__f)->flags & _F_EOF)
#endif


int fputc(int c, VirtFILE* f) { return f->Vputc(c); }
int fputs(const char *s, VirtFILE* f) { return f->Vputs(s); }
int fgetc(VirtFILE* f) { return f->Vgetc(); }
int fseek(VirtFILE* f, long offset, int whence) { return f->Vseek(offset, whence); }
int ftell(VirtFILE* f) { return (int)f->Vtell(); }
void rewind(VirtFILE* f) { f->Vrewind(); }
int fflush(VirtFILE* f) { return f->Vflush(); }
char *fgets(char *s, int size, VirtFILE* f) { return f->Vgets(s, size); }
int fprintf(VirtFILE* f, const char *format, ...)
{
	int ret; va_list argptr;
	va_start(argptr, format);
	ret = f->Vprintf(format, argptr);
	va_end(argptr);
	return ret;
}
int fclose(VirtFILE* f) { delete f; return 0; }

int closedir(VirtDIR* d) { delete d; return 0; }
dirent* readdir(VirtDIR* d) { return d->Vreaddir(); }

/////////

bool VirtFileSystem::Vmkdirs(const char* path)
{
	if (Vdirexists(path, true)) return true;
	string parentdir = getFileDir(path);
	if (!Vmkdirs(parentdir.c_str())) return false;
	return Vmkdir(path);
}

//////////


ChainFileSystem::ChainFileSystem(VirtFileSystem *_chain)
{
	chain = _chain;
#ifdef DEBUG_VIRTFILE
	printf("ChainFileSystem constructor: %p := %p\n",chain,_chain);
#endif
}


VirtFILE *ChainFileSystem::Vfopen(const char* path, const char* mode)
{
#ifdef DEBUG_VIRTFILE
	printf("ChainFileSystem::Vfopen %s %s (chain=%p)\n",path,mode,chain);
#endif
	return (chain != NULL) ? chain->Vfopen(path, mode) : NULL;
}

bool ChainFileSystem::Vfexists(const char* path)
{
	return (chain != NULL) ? chain->Vfexists(path) : false;
}

VirtDIR *ChainFileSystem::Vopendir(const char* path)
{
#ifdef DEBUG_VIRTFILE
	printf("ChainFileSystem::Vfopendir %s (chain=%p)\n",path,chain);
#endif
	if (chain==NULL) return internalopendir(path);
	return new Dir(string(path),this,chain);
}

bool ChainFileSystem::Vmkdir(const char* path)
{
	return (chain != NULL) ? chain->Vmkdir(path) : false;
}

bool ChainFileSystem::Vmkdirs(const char* path)
{
	return (chain != NULL) ? chain->Vmkdirs(path) : false;
}

bool ChainFileSystem::Vdirexists(const char* path, bool is_writable)
{
	return (chain != NULL) ? chain->Vdirexists(path, is_writable) : false;
}

ChainFileSystem::Dir::~Dir()
{
if (dir) delete dir;
}

dirent* ChainFileSystem::Dir::Vreaddir()
{
dirent *de;
  retry:
if (!dir)
	{
	if (first)
		{
		dir=first->internalopendir(path.c_str());
		first=NULL;
		}
	else if (second)
		{
		dir=second->Vopendir(path.c_str());
		second=NULL;
		}
	else
		return NULL;
	}
de=dir ? dir->Vreaddir() : NULL;
if (de==NULL)
	{if (dir) delete dir; dir=NULL; goto retry;}

// no need to check for duplicates if no names are saved and scanning the last location (most common case)
if (! (duplicates.empty() && (first==NULL) && (second==NULL)) )
{
string s(de->d_name);
if (duplicates.find(s)==duplicates.end())
	duplicates.insert(s);
else
	goto retry;
}

return de;
}
