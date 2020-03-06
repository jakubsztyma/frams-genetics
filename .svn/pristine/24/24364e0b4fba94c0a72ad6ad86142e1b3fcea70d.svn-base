// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "nonstd_stdio.h"
#include "nonstd.h"
#include "Convert.h" //utf8ToUtf16()
#include <common/util-string.h>

#ifdef _WIN32
#include "Shlwapi.h" //PathIsRelative()
#ifdef __BORLANDC__
#pragma link "Shlwapi.lib" //PathIsRelative()
#endif
#include <sys/stat.h> //_stat
#else
#include <unistd.h>
#endif



#ifdef _WIN32
FILE* mfile_wfopen(const char *path, const char *mode) //to avoid converting args into wide char in all occurrences of mfopen()
{
	return _wfopen(Convert::utf8ToUtf16(path).c_str(), Convert::strTOwstr(mode).c_str());
}
#endif

bool fileExists(const char* path)
{
	//lepiej gdyby uzywalo stat bo mfopen mogloby cos niepotrzebnie wczytywac przy otwarciu pliku ale mfopen wiadomo ze zadziala wszedzie tak samo
	MFILE *f = mfopen(path, FOPEN_READ_BINARY);
	if (f == NULL) return false;
	mfclose(f);
	return true;
}

#ifdef _WIN32
bool isDirWritable(const char* path) //dir must not end with '\'
{
	wstring dir = Convert::utf8ToUtf16(path);
	CreateDirectoryW(dir.c_str(), 0);
	dir += L"\\test_file.write";
	_wunlink(dir.c_str());
	FILE *f = _wfopen(dir.c_str(), L"wt");
	if (f)
	{
		fclose(f);
		_wunlink(dir.c_str());
		return true;
	}
	else
		return false;
}
#endif

bool directoryExists(const char* path, bool is_writable)
{
	struct _stat s;
	if (path[0] == 0) path = ".";
#ifdef _WIN32
	if (_wstat(Convert::utf8ToUtf16(path).c_str(), &s) != 0) return false;
#else
	if (_stat(path, &s) != 0) return false;
#endif
	if (S_ISDIR(s.st_mode))
	{
		if (is_writable)
		{
#ifdef _WIN32
#ifndef W_OK
#define W_OK 2 //http://msdn.microsoft.com/en-us/library/1w06ktdy.aspx
#endif
			//under Windows, access() is not a reliable way to check if a directory is writable
			//http://stackoverflow.com/questions/198071/code-for-checking-write-permissions-for-directories-in-win2k-xp
			//bool writable_access = _waccess(Convert::utf8ToUtf16(path).c_str(), W_OK) == 0;
			bool writable_trial = isDirWritable(path);
			//printf("Checking '%s' for writing(%d) using access(): result=%d\n", path, is_writable, writable_access);
			//printf("File creation test: result=%d\n", writable_trial);
			//return writable_access;
			return writable_trial;
#else
			return access(path, W_OK) == 0;
#endif
		}
		else
			return true;
	}
	return false;
}

bool makeDirectory(const char* path)
{
#ifdef _WIN32
	return _wmkdir(Convert::utf8ToUtf16(path).c_str()) == 0;
#else
	return mkdir(path,0777) == 0;
#endif
}

bool makeDirectories(const char* path)
{
	if (directoryExists(path,false)) return true;
	string parentdir = getFileDir(path);
	if (!makeDirectories(parentdir.c_str())) return false;
	return makeDirectory(path);
}

int getFileSize(const char* path)
{
	int size;
	MFILE *f = mfopen(path, FOPEN_READ_BINARY);
	if (f == NULL) return -1;
	size = getFileSize(f);
	mfclose(f);
	return size;
}

int getFileSize(MFILE *f)
{
	int saved_pos = (int)mftell(f);
	mfseek(f, 0, SEEK_END);
	int size = (int)mftell(f);
	mfseek(f, saved_pos, SEEK_SET);
	return size;
}

bool removeFile(const char* path)
{
#ifdef _WIN32
	return _wunlink(Convert::utf8ToUtf16(path).c_str()) == 0;
#else
	return _unlink(path) == 0; //VS: "The POSIX name is deprecated. Instead, use the ISO C++ conformant name: _unlink"
#endif
}

bool isAbsolutePath(const char* fname)
{
	if (fname == NULL) return false; //SplitFileSystem never passes NULL but this function is public so we never know
#ifdef _WIN32
	return PathIsRelativeW(Convert::utf8ToUtf16(fname).c_str()) == FALSE; //http://msdn.microsoft.com/en-us/library/bb773660%28v=vs.85%29.aspx
#else
	return fname[0] == PATH_SEPARATOR_CHAR;
#endif
}

#if defined SHP && defined BADA_API_1

MFILE *mfopen(const char *path, const char *mode)
{
	Osp::Io::File *f = new Osp::Io::File();
	result r = f->Construct(path, mode);
	if (IsFailed(r))
	{
		delete f;
		f = NULL;
	}
	return f;
}

void mfclose(MFILE *f)
{
	delete f;
}

int mfread(void *ptr, int size, int count, MFILE *f)
{
	int bytes = size * count;
	int przeczytane = f->Read(ptr, bytes);
	return przeczytane != bytes ? przeczytane / size : count;
}

int mfwrite(const void *ptr, int size, int count, MFILE *f)
{
	result r = f->Write(ptr, size * count);
	if (IsFailed(r))
		return 0; //nie mozemy wykryc jesli udalo sie zapisac czêœæ
	else
		return count;
}

int mfputs(const char *txt, MFILE *f)
{
	int len = strlen(txt);
	int res = mfwrite(txt, len, 1, f);
	return res == 1 ? 1 : EOF;
}

char* mfgets(char *str, int num, MFILE *f)
{
	bool err = false;
	int przeczytane = 0;
	num--; //zeby zawsze zostalo miejsce na wpisanie koncz¹cego NULL
	do
	{
		err = f->Read(str, 1) != 1;
		if (!err)
		{
			str++;
			przeczytane++;
		}
	} while (!err && przeczytane<num && *str != '\n');
	if (*str == '\n' && przeczytane<num)
		*(str + 1) = 0;
	return przeczytane == 0 ? NULL : str;
}

int mfeof(MFILE *f)
{
	//brzydkie obejscie zeby w bada wykryc czy FILE jest w stanie EOF
	static char buf[1];
	int pos = f->Tell();
	int przeczytane = f->Read(&buf, 1);
	f->Seek(Osp::Io::FILESEEKPOSITION_BEGIN,pos);
	return przeczytane == 1 ? 0 : 1;
}

int mfseek(MFILE *f, long position, int type)
{
	result r;
	if (type == SEEK_SET)
		r = f->Seek(Osp::Io::FILESEEKPOSITION_BEGIN, position);
	else if (type == SEEK_CUR)
		r = f->Seek(Osp::Io::FILESEEKPOSITION_CURRENT, position);
	else if (type == SEEK_END)
		r = f->Seek(Osp::Io::FILESEEKPOSITION_END, position);
	else
		return 1;
	return IsFailed(r) ? 1 : 0;
}

long mftell(MFILE *f)
{
	return f->Tell();
}

#endif





#ifdef __ANDROID__
#include "log.h"
#include "dirs.h"
#include "nonstd_stl.h"
MFILE *mfopen(const char *path, const char *mode)
{
	string respath=getAppResourcesDir();
	//log_printf("Opening '%s', mode='%s'",path,mode);
	//log_printf("getAppResourcesDir()='%s'",respath.c_str());
	//log_printf("getAppWritableDir()='%s'",getAppWritableDir().c_str());
	NvFile *rfile=NULL; //can only read
	FILE *rwfile=NULL;
	if (strstr(path,respath.c_str())==path) //opening resource! so we use a dedicated way to read from assets
	{
		path+=respath.length(); //strip the prefix, we need a relative path in assets
		if (path[0]=='/') path++; //also strip slash if it was there (the prefix has no trailing slash so it doesn't get stipped above)
		if (strstr(mode,"w"))
			log_printf("Warning: attempt to open a read-only resource '%s' in writable mode '%s'",path,mode);
		rfile=NvFOpen(path); //"mode" not supported! can only read
		//log_printf("Opened RES file as %p",rfile);
		if (rfile==NULL) return NULL;
	} else //a "normal" access (HOME)
	{
		rwfile=fopen(path,mode);
		//log_printf("Opened HOME file as %p",rwfile);
		if (rwfile==NULL) return NULL;
	}
	MFILE *mfile=new MFILE;
	mfile->rfile=rfile;
	mfile->rwfile=rwfile;
	return mfile;
}

void mfclose(MFILE *f)
{
	if (f->rfile)
		NvFClose(f->rfile);
	else
		fclose(f->rwfile);

	delete f;
}

int mfread(void *ptr, int size, int count, MFILE *f)
{
	if (f->rfile)
		return NvFRead(ptr, size, count, f->rfile); //nvidia introduced my corrections in SDK v10.14, so a fix is no longer needed here
	else
		return fread(ptr, size, count, f->rwfile);
}

int mfwrite(const void *ptr, int size, int count, MFILE *f)
{
	if (f->rfile)
		return 0; //write not supported in assets using nvidia functions
	else
		return fwrite(ptr, size, count, f->rwfile);
}

int mfputs(const char *txt, MFILE *f)
{
	int len = strlen(txt);
	if (len==0) //when writing X 0-byte elements, fwrite() will return 0 ("0 elements written" instead of "X elements") but this is not an error
		return 1; //for simplicity, we don't even try to write 0 bytes and report a success
	int res = mfwrite(txt, len, 1, f); //write 1 element of len bytes
	return res == 1 ? 1 : EOF;
}

char* mfgets(char *str, int num, MFILE *f)
{
	if (f->rfile)
	{
		char *ret=NvFGets(str, num, f->rfile);
		//fixing nvidia inconsistency... their function never returns NULL (fix submitted)
		if (ret!=NULL && *ret==0 && num>0) //nothing has been read, must have been eof
			return NULL;
		return ret;
	}
	else
		return fgets(str,num,f->rwfile);
}

int mfeof(MFILE *f)
{
	if (f->rfile)
		return NvFEOF(f->rfile);
	else
		return feof(f->rwfile);
}

int mfseek(MFILE *f, long position, int type)
{
	if (f->rfile)
		return NvFSeek(f->rfile, position, type); //nvidia introduced my corrections in SDK v10.14, so a fix is no longer needed here
	else
		return fseek(f->rwfile, position, type);
}

long mftell(MFILE *f)
{
	if (f->rfile)
		return NvFTell(f->rfile);
	else
		return ftell(f->rwfile);
}
#endif
