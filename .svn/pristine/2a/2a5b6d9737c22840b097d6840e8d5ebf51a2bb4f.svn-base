// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "util-string.h"
#include <stdarg.h>
#include "nonstd_stdio.h"
#include "nonstd.h"
#include <assert.h>
#include <cstdlib> //malloc()
#ifdef USE_VIRTFILE
#include <common/virtfile/virtfile.h>
#endif
#ifdef __ANDROID__
#include <android/log.h> //only needed to print error messages related to a workaround for Android bug
#endif

string ssprintf_va(const char* format, va_list ap)
{
	string s; //clang crashed when this declaration was in s=buf
	int size = 256;
	char* buf;
	va_list ap_copy; // "va_list ap" can only by used once by printf-type functions as they advance the current argument pointer (crashed on linux x86_64)
	// (does not apply to SString::sprintf, it does not have the va_list variant)

	//almost like SString::sprintf, but there is no common code to share because SString can use its directWrite to avoid double allocating/copying
#ifdef USE_VSCPRINTF
	va_copy(ap_copy, ap);
	size = _vscprintf(format, ap_copy) + 1; //+1 for terminating null character
	va_end(ap_copy);
#endif

	while (1)
	{
		buf = (char*)malloc(size);
		assert(buf != NULL);
		va_copy(ap_copy, ap);
		int n = vsnprintf(buf, size, format, ap_copy);
		va_end(ap_copy);

#ifdef __ANDROID__
		//Workaround for Android bug. /system/lib64/libc.so? maybe only arm 64-bit? "If an encoding error occurs, a negative number is returned". On some devices keeps returning -1 forever.
		//https://github.com/android-ndk/ndk/issues/879 but unfortunately during google play tests (Firebase Test Lab) this problem turned out to be not limited to Chinese devices and occurred in Mate 9, Galaxy S9, Pixel, Pixel 2, Moto Z (even with the en_GB locale; the locale is not important but the problem seem to be utf8 non-ascii chars in the format string).
		if (n < 0 && size >= (1 << 24)) //wants more than 16M
		{
			buf[size - 1] = 0; //just to ensure there is at least some ending \0 in memory... who knows what buggy vsnprintf() did.
			__android_log_print(ANDROID_LOG_ERROR, LOG_APP_NAME, "Giving up due to Android bug: vsnprintf() wants more than %d bytes, it used %zu bytes, for format='%s'", size, strlen(buf), format);
			//in my tests, it always used 0 bytes, so it produced a 0-length string: ""
			va_copy(ap_copy, ap);
			n = vsprintf(buf, format, ap_copy); //hoping 16M is enough
			va_end(ap_copy);
			__android_log_print(ANDROID_LOG_INFO, LOG_APP_NAME, "Fallback to vsprintf() produced string: '%s'", buf);
			if (n < 0) //vsprintf was also buggy. If we were strict, we should abort the app now.
			{
				strcpy(buf, "[STR_ERR] "); //a special prefix just to indicate the returned string is incorrect
				strcat(buf, format); //append and return the original formatting string
				__android_log_print(ANDROID_LOG_ERROR, LOG_APP_NAME, "vsprintf() also failed, using the incorrect resulting string: '%s'", buf);
			}
			n = strlen(buf); //pretend vsnprintf() or vsprintf() was OK to exit the endless loop
		}
#endif

		if (n > -1 && n < size)
		{
			s = buf;
			free(buf);
			return s;
		}
#ifdef VSNPRINTF_RETURNS_REQUIRED_SIZE
		if (n > -1)    /* glibc 2.1 */
			size = n + 1; /* precisely what is needed */
		else           /* glibc 2.0 */
#endif
			size *= 2;  /* twice the old size */
		free(buf);
	}
}

char* strmove(char *a, char *b) //strcpy that works well for overlapping strings ("Source and destination overlap")
{
	if (a == NULL || b == NULL)
		return NULL;
	memmove(a, b, strlen(b) + 1);
	return a;
}

string ssprintf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	string ret = ssprintf_va(format, ap); //is it too wasteful? copying the string again... unless the compiler can handle it better
	va_end(ap);
	return ret;
}

string stripExt(const string& filename)
{
	size_t dot = filename.rfind('.');
	if (dot == string::npos) return filename;
	size_t sep = filename.rfind(PATH_SEPARATOR_CHAR);
	if ((sep == string::npos) || (sep < dot))
		return filename.substr(0, dot);
	return filename;
}

string stripFileDir(const string& filename)
{
	size_t sep = filename.rfind(PATH_SEPARATOR_CHAR);
	if (sep == string::npos) return filename;
	return filename.substr(sep + 1);
}

string getFileExt(const string& filename)
{
	size_t dot = filename.rfind('.');
	if (dot == string::npos) return string("");
	size_t sep = filename.rfind(PATH_SEPARATOR_CHAR);
	if ((sep == string::npos) || (sep < dot))
		return filename.substr(dot);
	return string("");
}

string getFileDir(const string& filename)
{
	size_t slash = filename.rfind(PATH_SEPARATOR_CHAR);
	if (slash == string::npos) return string("");
	return filename.substr(0, slash);
}
