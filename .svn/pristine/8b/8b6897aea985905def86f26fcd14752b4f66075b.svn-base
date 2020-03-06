// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "Convert.h"

#include <sstream>

#if defined __ANDROID__ || defined __BORLANDC__
#include <ctype.h> //toupper, tolower
#endif

#ifdef SHP
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <stdio.h>


int Convert::toInt(string s) { return atoi(s.c_str()); }
float Convert::toFloat(string s) { return (float)atof(s.c_str()); }
string Convert::toLowerCase(string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower);  return s; }
string Convert::toUpperCase(string s) { std::transform(s.begin(), s.end(), s.begin(), ::toupper);  return s; }
char Convert::toLowerCase(char c) { return (char)tolower(c); }
char Convert::toUpperCase(char c) { return (char)toupper(c); }

template<class T> const char* printf_format_for(const T& value) { return "unknown type"; }
template<> const char* printf_format_for(const unsigned int& value) { return "%u"; }
template<> const char* printf_format_for(const int& value) { return "%d"; }
template<> const char* printf_format_for(const short& value) { return "%d"; }
template<> const char* printf_format_for(const float& value) { return "%g"; }
template<> const char* printf_format_for(const double& value) { return "%g"; }

template<class T> string Convert::_toString(const T& value)
{
	char buf[30];
	sprintf(buf, printf_format_for(value), value);
	return string(buf);
	/*
	#ifndef MULTITHREADED
	static
	#endif
	std::ostringstream oss; //pod VS tworzenie go trwa dlugo nawet w wersji release (szczegolnie jak np konwertuje sie cos setki tysiecy razy)
	//dlatego robimy go raz (static) i potem tylko czyscimy
	//ciekawostka: kiedy nie byl static, czasy wykonania bogatego w konwersje kawa³ka kodu oscylowa³y w trybie debug
	//(5.5s lub 55s) a w release zawsze 57s. Po uzyciu static czas tego samego kodu jest zawsze debug: 0.72s release: 0.33s
	oss.clear(); //clear error flag
	oss.str(""); //set empty string
	oss << value;
	return oss.str();
	*/
}

string Convert::toString(unsigned int v) { return _toString(v); }
string Convert::toString(int v) { return _toString(v); }
string Convert::toString(short v) { return _toString(v); }
string Convert::toString(float v) { return _toString(v); }
string Convert::toString(double v) { return _toString(v); }

uint32_t Convert::hexToInt(const string& col)
{
	uint32_t value;
	std::istringstream iss(col);
	iss >> std::hex >> value;
	return value;
}

#ifdef MULTITHREADED
//jezeli jest tu a nie jako static w funkcji, to inicjalizacja
//nastapi na samym poczatku (w nieprzewidywalnym momencie, ale nie szkodzi(?))
//gdyby byla w funkcji to teoretycznie dwa watki moglyby wejsc
//do niej rownoczesnie i zaczac inicjalizacje po czym jeden korzystalby
//z mutexa gdy drugi dalej by go inicjalizowal
#if ! ((defined LINUX) || (defined _WIN32 && !defined __BORLANDC__ ))
// only for the "borland?" cases in localtime() and asctime() below
#include "threads.h"
static pthread_mutex_t fix_unsafe_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#endif

struct tm Convert::localtime(const time_t &timep)
{
#ifndef MULTITHREADED

	return *::localtime(&timep);

#else

	struct tm ret;
#if defined LINUX // || android?
	return *::localtime_r(&timep,&ret);
#elif defined _WIN32 && !defined __BORLANDC__
	::localtime_s(&ret, &timep);
	return ret;
#else //borland?
	pthread_mutex_lock(&fix_unsafe_mutex);
	ret=*::localtime(&timep);
	pthread_mutex_unlock(&fix_unsafe_mutex);
	return ret;
#endif

#endif
}

string Convert::asctime(const struct tm &tm)
{
	char *ret;
#ifndef MULTITHREADED

	ret=::asctime(&tm);

#else //MULTITHREADED

	char buf[26];
#if defined LINUX // || android?
	ret=::asctime_r(&tm,buf);
#elif defined _WIN32 && !defined __BORLANDC__
	asctime_s(buf, sizeof(buf), &tm);
	ret = buf;
#else //borland?
	pthread_mutex_lock(&fix_unsafe_mutex);
	strcpy(buf,::asctime(&tm));
	ret=buf;
	pthread_mutex_unlock(&fix_unsafe_mutex);
#endif
#endif

	return string(ret, 24); //24 znaki z pominieciem ostatniego \n
}

string Convert::wstrToUtf8(const wchar_t *str)
{
	if (str == NULL) return "";
	string res;
	wchar_t *wcp = (wchar_t*)str;
	while (*wcp != 0)
	{
		int c = *wcp;
		if (c < 0x80) res += c;
		else if (c < 0x800) { res += 192 + c / 64; res += 128 + c % 64; }
		else if (c - 0xd800u < 0x800) res += "<ERROR-CHAR>";
		else if (c < 0x10000) { res += 224 + c / 4096; res += 128 + c / 64 % 64; res += 128 + c % 64; }
		else if (c < 0x110000) { res += 240 + c / 262144; res += 128 + c / 4096 % 64; res += 128 + c / 64 % 64; res += 128 + c % 64; }
		else res += "<ERROR-CHAR>";
		wcp++;
	}
	return res;
}

#ifdef _WIN32
wstring Convert::utf8ToUtf16(const char *str)
{
	wstring wstr;
	int nOffset = 0;
	int nDataLen = strlen(str); //ending \0 is not converted, but resize() below sets the proper length of wstr
	int nLenWide = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(str + nOffset),
		(int)(nDataLen - nOffset), NULL, 0);
	wstr.resize(nLenWide);
	if (MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(str + nOffset),
		(int)(nDataLen - nOffset),
		&wstr[0], nLenWide) != nLenWide)
	{
		//ASSERT(false); //some conversion error
		return wstr + L"<UTF8_CONV_ERROR>";
	}
	return wstr;
}
#endif
