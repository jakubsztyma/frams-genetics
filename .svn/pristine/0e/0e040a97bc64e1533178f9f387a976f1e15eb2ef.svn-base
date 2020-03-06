// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "stdiofile.h"
#include <common/nonstd_dir.h>
#include <common/nonstd_stdio.h>
#include <common/log.h>
#include <common/Convert.h>
#ifdef __ANDROID__
#include <common/dirs.h>
#include <common/platform/android/AndroidAPK_DIR.h>
#endif

VirtFILE* StdioFileSystem::Vfopen(const char *path, const char *mode)
{
	//log_printf("Vfopen %s %s",path,mode);
#if defined USE_MFILE || defined _WIN32
	MFILE *f = mfopen(path, mode);
#else
	FILE *f = fopen(path, mode);
#endif
	//log_printf("%p",f);
	if (f) return new StdioFILE(f, path); else return NULL;
}

VirtDIR* StdioFileSystem::Vopendir(const char* path)
{
	//log_printf("Vopendir %s",path);
#ifdef __ANDROID__
	int resources_prefix_length = getAppResourcesDir().length();
	if (strncmp(path, getAppResourcesDir().c_str(), resources_prefix_length) == 0) //it is a resources dir
	{
		VirtDIR *vd = AndroidAPK_DIR::opendir(path + resources_prefix_length + 1); //+1 because we also skip '/' and start with a "relative" dir, otherwise it does not work.
		return vd;
	}
#endif

#ifdef _WIN32
	DIRTYPE *d = wopendir(Convert::utf8ToUtf16(path).c_str());
#else
	DIR *d = opendir(path);
#endif
	//log_printf("%p",d);
	if (d) return new StdioDIR(d); else return NULL;
}

bool StdioFileSystem::Vfexists(const char* path)
{
	return fileExists(path);
}

bool StdioFileSystem::Vdirexists(const char* path, bool is_writable)
{
#ifdef __ANDROID__
	int resources_prefix_length = getAppResourcesDir().length();
	if (strncmp(path, getAppResourcesDir().c_str(), resources_prefix_length) == 0) //it is a resources dir
	{
		if (is_writable)
			return false;
		VirtDIR *vd = AndroidAPK_DIR::opendir(path + resources_prefix_length + 1); //+1 because we also skip '/' and start with a "relative" dir, otherwise it does not work.
		if (vd != NULL)
		{
			delete vd;
			return true;
		}
		else
		{
			return false;
		}
	}
#endif
	return directoryExists(path, is_writable);
}






#ifndef NO_STD_IN_OUT_ERR
void StdioFILE::setStdio()
{
	static StdioFILEDontClose si(stdin);
	static StdioFILEDontClose so(stdout);
	static StdioFILEDontClose se(stderr);
	setVstdin(&si);
	setVstdout(&so);
	setVstderr(&se);
}
#endif

dirent* StdioDIR::Vreaddir()
{
	//log_printf("Vreaddir %s",dir);
#ifdef _WIN32
	wdirent *wde = wreaddir(dir);
	if (wde == NULL) return NULL;
	strcpy(de.d_name, Convert::wstrToUtf8(wde->d_name).c_str());
	return &de;
#else
	return readdir(dir);
#endif
}
