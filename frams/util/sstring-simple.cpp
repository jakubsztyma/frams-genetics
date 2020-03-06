#include "sstring.h"
#include <common/nonstd_stl.h>
#include "extvalue.h"
#include <assert.h>
#ifdef USE_PRINTFLOAT_DRAGON4
#include <PrintFloat/PrintFloat.h>
#endif

void SString::initEmpty()
{
	txt = NULL; used = 0; size = 0;
}

SString::SString()
{
	initEmpty();
}

SString::~SString()
{
	resize(0);
}

SString::SString(int x)
{
	initEmpty();
	if (x)
		ensureSize(x + 1);
}

SString::SString(const char *t, int t_len)
{
	initEmpty();
	if (!t) return;
	copyFrom(t, t_len);
}

SString::SString(const SString &from)
{
	initEmpty();
	operator=(from);
}

SString::SString(SString&& from)
{
	txt = from.txt; size = from.size; used = from.used;
	from.txt = NULL; from.size = 0; from.used = 0;
}

void SString::resize(int newsize)
{
	if (newsize == size) return;
	txt = (char*)realloc(txt, newsize);
	size = newsize;
}

void SString::ensureSize(int needed)
{
	if (size > needed) return;
	resize((size > 0) ? (needed + needed / 2 + 1) : (needed + 1));
}

char *SString::directWrite(int ensuresize)
{
	ensureSize(ensuresize);
	appending = used;
	return txt;
}

char *SString::directAppend(int maxappend)
{
	ensureSize(used + maxappend);
	appending = used;
	return txt + appending;
}

void SString::endWrite(int newlength)
{
	if (newlength < 0) newlength = strlen(txt);
	else txt[newlength] = 0;
	used = newlength;
	assert(used < size);
}

void SString::endAppend(int newappend)
{
	if (newappend < 0) newappend = strlen(txt + appending);
	else txt[appending + newappend] = 0;
	used = appending + newappend;
	assert(used < size);
}

////////////// append /////////////////

void SString::operator+=(const char *s)
{
	if (!s) return;
	int x = strlen(s);
	if (!x) return;
	append(s, x);
}

void SString::append(const char *t, int n)
{
	if (!n) return;
	ensureSize(used + n);
	memmove(txt + used, t, n);
	used += n;
	txt[used] = 0;
}

void SString::operator+=(const SString&s)
{
	append(s.c_str(), s.len());
}

SString SString::operator+(const SString& s) const
{
	SString ret(len() + s.len());
	ret = *this;
	ret += s;
	return ret;
}

/////////////////////////////

void SString::copyFrom(const char *ch, int chlen)
{
	if (!ch) chlen = 0;
	else if (chlen < 0) chlen = strlen(ch);
	if (chlen)
	{
		ensureSize(chlen);
		memmove(txt, ch, chlen);
		txt[chlen] = 0;
		used = chlen;
	}
	else
	{
		if (txt)
		{
			txt[0] = 0;
			used = 0;
		}
	}
}

void SString::operator=(const char *ch)
{
	copyFrom(ch);
}

void SString::operator=(const SString&s)
{
	if (&s == this) return;
	copyFrom(s.c_str(), s.len());
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
	if (this == &s) return true;
	if (len() != s.len()) return false;
	return strcmp(getPtr(), s.getPtr()) == 0;
}

///////////////////////////////////////

int SString::indexOf(int character, int start) const
{
	const char *found = strchr(getPtr() + start, character);
	return found ? found - getPtr() : -1;
}

int SString::indexOf(const char *substring, int start) const
{
	const char *found = strstr(getPtr() + start, substring);
	return found ? found - getPtr() : -1;
}

int SString::indexOf(const SString & substring, int start) const
{
	const char *found = strstr(getPtr() + start, substring.c_str());
	return found ? found - getPtr() : -1;
}

bool SString::getNextToken(int& pos, SString &token, char separator) const
{
	if (pos >= len()) { token = 0; return false; }
	int p1 = pos, p2;
	const char *t1 = getPtr() + pos;
	const char *t2 = strchr(t1, separator);
	if (t2) pos = (p2 = (t2 - getPtr())) + 1; else p2 = pos = len();
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
