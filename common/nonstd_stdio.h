// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NONSTD_STDIO_H_
#define _NONSTD_STDIO_H_

bool fileExists(const char* path);
bool directoryExists(const char* path,bool is_writable);
bool makeDirectory(const char* path);
bool makeDirectories(const char* path);
bool removeFile(const char* path);
bool isAbsolutePath(const char* fname);
int getFileSize(const char* path);

#ifdef _WIN32


#ifndef _MSC_VER
 #include <dir.h>
#else
 #include <direct.h>
 #define mkdir _mkdir
 #define S_ISDIR(m)	(((m)&S_IFDIR)==S_IFDIR)
#endif

#include <io.h> //borland compiler: include <io.h> before <dir.h> causes the SimWorld class in "simul.h" be unrecognized, for unknown reason :O moreover, this problem is only pertinent to the CLI project, not GUI. Maybe this is caused by global defines like NOVCL, NO_STRICT etc.?
// #define makeDirectory(name) mkdir(name)

#else

#include <unistd.h>
#include <sys/stat.h>
// #define makeDirectory(name) mkdir(name,0777)
#define _unlink unlink //_unlink jest ISO-conformant, unlink jest POSIX-deprecated
#define _stat stat
#endif


#include <stdio.h>

#if (defined SHP && defined BADA_API_1) || defined __ANDROID__

#ifdef __ANDROID__
#include <nv_file/nv_file.h>
struct rwFILE //jedno z dwoch pol jest zainicjowane w zaleznosci od tego gdzie jest plik
{ //nvidia uses a similar trick in nv_file.h (STD_FILE and APK_FILE), maybe doing a similar thing here is redundant? but their trick uses some trial-and-error code (see NvFOpen())
	NvFile *rfile; //can only read
	FILE *rwfile;
	rwFILE() {rfile=rwfile=NULL;}
};
typedef rwFILE MFILE;
#else //SHP:
//z <stdio.h> wzielismy sprintfy i inne ktore dzia³aj¹...
#include <FIo.h>
// wklejone z sailora w ramach integracji frams+engine
// ale to nie sprawia ze framsy korzystaja z mfile - potrzebna jest implementacja virtfile dla bady! (patrz: stdiofile.h)
// i wtedy bedzie mozna mfile wywalic tez z sailora
typedef Osp::Io::File MFILE;
#endif

MFILE *mfopen(const char*path,const char*mode);
void mfclose(MFILE *f);
int mfread(void *ptr, int size, int n, MFILE *f);
int mfwrite(const void *ptr, int size, int n, MFILE *f);
int mfputs(const char *, MFILE *);
int	mfseek(MFILE *, long, int);
long mftell(MFILE *);
char *mfgets(char *str, int num, MFILE *f);
int mfeof(MFILE *f);

//#define	SEEK_SET	0	/* set file offset to offset */
//#define	SEEK_CUR	1	/* set file offset to current plus offset */
//#define	SEEK_END	2	/* set file offset to EOF plus offset */
//int	sprintf(char *, const char *, ...);
//int	vsnprintf(char *,int, const char *, ...);

#else
typedef FILE MFILE;
#ifdef _WIN32
 FILE* mfile_wfopen(const char *path, const char *mode);
 #define mfopen mfile_wfopen
#else
 #define mfopen fopen
#endif
#define mfclose fclose
#define mfread fread
#define mfwrite fwrite
#define mfputs fputs
#define mfgets fgets
#define mfeof feof
#define mfseek fseek
#define mftell ftell
#endif


#ifndef _WIN32
#define _strdup strdup //_strdup jest ISO-conformant, strdup jest POSIX deprecated
#include <string.h> //strdup
#endif

int getFileSize(MFILE *f);

#endif
