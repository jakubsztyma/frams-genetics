// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NONSTD_H_
#define _NONSTD_H_

#define SAFEDELETE(p) {if (p) {delete p; p=NULL;}}
#define SAFEDELETEARRAY(p) {if (p) {delete[] p; p=NULL;}}

#define roundToInt(x) ((int)(floor((x)+0.5)))


#define CPP_STR(s) CPP_XSTR(s)
#define CPP_XSTR(s) #s


#define DB(x) //output debug info. If needed, use #define DB(x) x
//#define DB(x) x



/////////////////////////////////////////////////////// path separators and other macros
#ifdef _WIN32
	#define PATH_SEPARATOR_CHAR '\\'
	#define PATH_SEPARATOR_STRING "\\"
	#define FPU_THROWS_EXCEPTIONS
#else
	#define PATH_SEPARATOR_CHAR '/'
	#define PATH_SEPARATOR_STRING "/"
#endif

#define FOPEN_READ_BINARY "rb"
// no FOPEN_READ_TEXT
#define FOPEN_WRITE_BINARY "wb"
#define FOPEN_APPEND_BINARY "ab"
#define FOPEN_WRITE_TEXT "wt"
#define FOPEN_APPEND_TEXT "at"

#ifdef LINUX
 #include <strings.h>
 #ifndef __CYGWIN__
  #define stricmp(a,b) strcasecmp(a,b)
  #define strnicmp(a,b,c) strncasecmp(a,b,c)
#endif
#endif

#if defined MACOS || defined __ANDROID__ || defined IPHONE
 #define stricmp(a,b) strcasecmp(a,b)
 #define strnicmp(a,b,c) strncasecmp(a,b,c)
#endif


//typedef unsigned char boolean; //niestety nie mozna uzyc 'bool' bo VC w rpcndr.h wlasnie tak definiuje booleana, jako unsigned char
//typedef char byte; //rozne srodowiska c++ definiuja byte jako unsigned char! w javie jest inaczej -> trzeba i tak zmienic w portowanych zrodlach byte na char.



#ifdef __BORLANDC__
 #define va_copy(to,from) ((to)=(from)) //borland's implementation of va_copy (the _Vacopy() function) copies from="" to to=NULL, and this behavior crashes our ssprintf_va(), so we prefer our simple macro since va_list is just a char* pointer in borland
#endif


//vsnprintf implementations support different standards, some only return -1 instead of the required number of characters
//these definitions are used by stl-util.cpp/ssprintf_va and SString::sprintf
#ifdef LINUX
#define VSNPRINTF_RETURNS_REQUIRED_SIZE
#endif
#if defined _WIN32 && !defined __BORLANDC__
#define USE_VSCPRINTF
#endif


#endif
