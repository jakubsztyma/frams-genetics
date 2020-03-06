// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "sstring.h"
#include <common/nonstd.h> //to be sure the vsnprintf-related stuff gets included

#ifdef SSTRING_SIMPLE

// simple sstring implementation using direct character arrays
// - duplicate = copy all characters
// - no mutex needed

#include "sstring-simple.cpp"

#else
///////////////////////////////////////////////////////////////////////////
// old sstring implementation using SBuf references
// - duplicate = copy buffer pointer
// - mutex required to be thread safe

#include <common/nonstd_stl.h>
#include "extvalue.h"
#include <assert.h>
#ifdef USE_PRINTFLOAT_DRAGON4
#include <PrintFloat/PrintFloat.h>
#endif

#ifdef MULTITHREADED
#include <pthread.h>
static pthread_mutex_t sstring_ref_lock = PTHREAD_MUTEX_INITIALIZER;
#define REF_LOCK pthread_mutex_lock(&sstring_ref_lock);
#define REF_UNLOCK pthread_mutex_unlock(&sstring_ref_lock)
#else
#define REF_LOCK 
#define REF_UNLOCK 
#endif

static int guessMemSize(int request)
{
	return request + min(request / 2, 10000) + 8;
}

SBuf::SBuf()
{
	txt = (char*)"";
	size = used = 0;
	refcount = 1;
}

SBuf::SBuf(int initsize)
{
	size = guessMemSize(initsize);
	if (size > 0) { txt = (char*)malloc(size + 1); txt[0] = 0; }
	else	txt = (char*)"";
	used = 0;
	refcount = 1;
}

SBuf::~SBuf()
{
	freeBuf();
}

void SBuf::initEmpty()
{
	txt = (char*)"";
	used = size = 0;
	refcount = 1;
}

void SBuf::freeBuf()
{
	if (!size) return;
	free(txt); used = 0;
}

void SBuf::copyFrom(const char *ch, int chlen)
{
	if (chlen == -1) chlen = strlen(ch);
	if (chlen > 0)
	{
		if (chlen < size)
		{
			memmove(txt, ch, chlen);
		}
		else
		{
			size = guessMemSize(chlen);
			char *newtxt = (char*)malloc(size + 1);
			memcpy(newtxt, ch, chlen);
			free(txt);
			txt = newtxt;
		}
	}
	txt[chlen] = 0;
	used = chlen;
}

void SBuf::append(const char *ch, int chlen)
{ // doesn't check anything!
	memmove(txt + used, ch, chlen);
	used += chlen;
	txt[used] = 0;
}

void SBuf::ensureSize(int needed)
{
	if (size >= needed) return;
	needed = guessMemSize(needed);
	txt = (char*)realloc(txt, needed + 1);
	size = needed;
}

/////////////////////////////////////////////

SString::SString()
{
	initEmpty();
}

SString::~SString()
{
	REF_LOCK;
	detach();
	REF_UNLOCK;
}

SString::SString(int x)
{
	buf = new SBuf(x);
}

SString::SString(const char *t, int t_len)
{
	initEmpty();
	if (!t) return;
	copyFrom(t, t_len);
}

SString::SString(SString&& from)
{
	buf = from.buf;
	from.buf = &SBuf::empty();
}

SString::SString(const SString &from)
{
	if (from.buf == &SBuf::empty())
		buf = &SBuf::empty();
	else
	{
		REF_LOCK;
		buf = from.buf;
		if (buf->size)
			buf->refcount++;
		REF_UNLOCK;
	}
}

void SString::initEmpty()
{
	buf = &SBuf::empty();
}

void SString::memoryHint(int howbig)
{
	detachCopy(howbig);
}

void SString::detachEmpty(int ensuresize)
{
	if (buf == &SBuf::empty()) { buf = new SBuf(ensuresize); return; }
	if (buf->refcount < 2) buf->ensureSize(ensuresize);
	else
	{
		buf->refcount--;
		buf = new SBuf(ensuresize);
	}
}

void SString::detach()
{
	if (buf == &SBuf::empty()) return;
	if (!--buf->refcount) delete buf;
}

void SString::detachCopy(int ensuresize)
{
	if (buf == &SBuf::empty()) { buf = new SBuf(ensuresize); return; }
	if (buf->refcount < 2)
	{
		buf->ensureSize(ensuresize);
		return;
	}
	buf->refcount--;
	SBuf *newbuf = new SBuf(ensuresize);
	newbuf->copyFrom(buf->txt, min(ensuresize, buf->used));
	buf = newbuf;
}

char *SString::directWrite(int ensuresize)
{
	if (ensuresize < 0) ensuresize = len();
	REF_LOCK;
	detachCopy(ensuresize);
	REF_UNLOCK;
	appending = buf->used;
	return buf->txt;
}

/*
char *SString::directWrite()
{
return directWrite(buf->used);
}
*/
char *SString::directAppend(int maxappend)
{
	REF_LOCK;
	detachCopy(buf->used + maxappend);
	REF_UNLOCK;
	appending = buf->used;
	return buf->txt + appending;
}

void SString::endWrite(int newlength)
{
	if (newlength < 0) newlength = strlen(buf->txt);
	else buf->txt[newlength] = 0;
	buf->used = newlength;
}

void SString::endAppend(int newappend)
{
	if (newappend < 0) newappend = strlen(buf->txt + appending);
	else buf->txt[appending + newappend] = 0;
	buf->used = appending + newappend;
}

////////////// append /////////////////

void SString::operator+=(const char *s)
{
	if (!s) return;
	int x = strlen(s);
	if (!x) return;
	append(s, x);
}

void SString::append(const char *txt, int count)
{
	if (!count) return;
	REF_LOCK;
	detachCopy(buf->used + count);
	REF_UNLOCK;
	buf->append(txt, count);
}

void SString::operator+=(const SString&s)
{
	append(s.c_str(), s.len());
}

SString SString::operator+(const SString& s) const
{
	SString ret(*this);
	ret += s;
	return ret;
}

/////////////////////////////

void SString::copyFrom(const char *ch, int chlen)
{
	if (!ch) chlen = 0;
	else if (chlen < 0) chlen = strlen(ch);
	REF_LOCK;
	detachEmpty(chlen);
	REF_UNLOCK;
	memmove(buf->txt, ch, chlen);
	buf->txt[chlen] = 0; buf->used = chlen;
}

void SString::operator=(const char *ch)
{
	copyFrom(ch);
}

void SString::operator=(const SString&s)
{
	if (s.buf == buf) return;
	REF_LOCK;
	detach();
	buf = s.buf;
	if (buf->size) buf->refcount++;
	REF_UNLOCK;
}
///////////////////////////////////////

SString SString::substr(int begin, int length) const
{
	if (begin < 0) { length += begin; begin = 0; }
	if (length >= (len() - begin)) length = len() - begin;
	if (length <= 0) return SString();
	if (length == len()) return *this;
	return SString((*this)(begin), length);
}

///////////////////////////////////////

bool SString::equals(const SString& s) const
{
	if (s.buf == buf) return true;
	return strcmp(buf->txt, s.buf->txt) == 0;
}

///////////////////////////////////////

int SString::indexOf(int character, int start) const
{
	const char *found = strchr(buf->txt + start, character);
	return found ? found - buf->txt : -1;
}

int SString::indexOf(const char *substring, int start) const
{
	char *found = strstr(buf->txt + start, substring);
	return found ? found - buf->txt : -1;
}

int SString::indexOf(const SString & substring, int start) const
{
	char *found = strstr(buf->txt + start, substring.c_str());
	return found ? found - buf->txt : -1;
}

bool SString::getNextToken(int& pos, SString &token, char separator) const
{
	if (pos >= len()) { token = 0; return false; }
	int p1 = pos, p2;
	const char *t1 = buf->txt + pos;
	const char *t2 = strchr(t1, separator);
	if (t2) pos = (p2 = (t2 - buf->txt)) + 1; else p2 = pos = len();
	strncpy(token.directWrite(p2 - p1), t1, p2 - p1);
	token.endWrite(p2 - p1);
	return true;
}

bool SString::startsWith(const char *pattern) const
{
	const char *t = this->c_str();
	for (; *pattern; pattern++, t++)
		if (*t != *pattern) return false;
	return true;
}

SString SString::valueOf(int i)
{
	return SString::sprintf("%d", i);
}
SString SString::valueOf(long i)
{
	return SString::sprintf("%d", i);
}
SString SString::valueOf(double d)
{
#ifdef USE_PRINTFLOAT_DRAGON4
	SString tmp;
	char* here = tmp.directWrite(30);
	tmp.endWrite(PrintFloat64(here, 30, d,
		((d < -1e17) || (d > 1e17) || ((d < 1e-4) && (d > -1e-4) && (d != 0.0)))
		? PrintFloatFormat_Scientific : PrintFloatFormat_Positional,
		-1));//http://www.ryanjuckett.com/programming/printing-floating-point-numbers/
#else
	SString tmp = SString::sprintf("%.17g", d); //https://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
#endif
	if ((!strchr(tmp.c_str(), '.')) && (!strchr(tmp.c_str(), 'e'))) tmp += ".0";
	return tmp;
}
SString SString::valueOf(const SString& s)
{
	return s;
}

#if 0 //testing _vscprintf
#define USE_VSCPRINTF
int _vscprintf(const char *format, va_list argptr)
{
	return vsnprintf("", 0, format, argptr);
}
#endif

SString SString::sprintf(const char* format, ...)
{
	int n, size = 30;
	va_list ap;

	SString ret;

#ifdef USE_VSCPRINTF
	va_start(ap, format);
	size = _vscprintf(format, ap);
	va_end(ap);
#endif

	while (1)
	{
		char* p = ret.directWrite(size);
		assert(p != NULL);
		size = ret.directMaxLen() + 1;
		/* Try to print in the allocated space. */
		va_start(ap, format);
		n = vsnprintf(p, size, format, ap);
		va_end(ap);
		/* If that worked, return the string. */

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
			ret.endWrite(n);
			return ret;
		}
		/* Else try again with more space. */
#ifdef VSNPRINTF_RETURNS_REQUIRED_SIZE
		if (n > -1)    /* glibc 2.1 */
			size = n; /* precisely what is needed */
		else           /* glibc 2.0 */
#endif
			size *= 2;  /* twice the old size */
	}
}

SString &SString::empty()
{
	static SString empty;
	return empty;
}

SBuf &SBuf::empty()
{
	static SBuf empty;
	return empty;
}



#endif //#ifdef SSTRING_SIMPLE

//////////////////////////////////////////////////
// to be moved somewhere else?
// public domain source: http://isthe.com/chongo/src/fnv
typedef uint32_t Fnv32_t;

#define FNV_32_PRIME ((Fnv32_t)0x01000193)
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5)
#define FNV1_32A_INIT FNV1_32_INIT

Fnv32_t fnv_32a_buf(void *buf, size_t len, Fnv32_t hval)
{
	unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
	unsigned char *be = bp + len;		/* beyond end of buffer */

	while (bp < be) {

		/* xor the bottom with the current octet */
		hval ^= (Fnv32_t)*bp++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_32_PRIME;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
#endif

	}

	/* return our new hash value */
	return hval;
}
//////////////////////////////////////////////////

#ifdef SSTRING_SIMPLE
uint32_t SString::hash() const
{
	return fnv_32a_buf(txt, used, FNV1_32A_INIT);
}
#else
uint32_t SBuf::hash() const
{
	return fnv_32a_buf(txt, used, FNV1_32A_INIT);
}
#endif
