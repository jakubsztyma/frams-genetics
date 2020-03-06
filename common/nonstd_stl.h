// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NONSTD_STL_H_
#define _NONSTD_STL_H_

//stl jak sama nazwa glosi wcale nie jest nonstd

#include <string>
using std::string;
#ifndef SHP //bada nie ma wstring
using std::wstring;
#endif

#include <vector>
using std::vector;

#include <algorithm> //std::min,max,swap
using std::min;
using std::max;
using std::swap;


// ------------------- ARRAY_LENGTH -------------------

//staromodne makro, niezabezpieczone przed uzyciem wskaznika w roli "x"
//#define ARRAY_LENGTH(x) (sizeof(x)/sizeof((x)[0]))

//hakerskie makro ktore wykrywa czesc pomy³kowych przypadkow uzycia
//#define ARRAY_LENGTH(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

//szablonowa funkcja pisana przez sredniozaawansowanych, jak to funkcja - nie daje niestety sta³ej w czasie kompilacji
//template<typename T, std::size_t N> inline std::size_t ARRAY_LENGTH( T(&)[N] ) { return N; } //"constexpr" dopiero w C++0x

//szablony hakerskie: tablica bajtow o dlugosci N - tak dluga jak tablica o któr¹ pytamy...
template <typename T, std::size_t N>
char (&array_temp(T (&a)[N]))[N];

// As litb noted in comments, you need this overload to handle array rvalues
// correctly (e.g. when array is a member of a struct returned from function),
// since they won't bind to non-const reference in the overload above.
template <typename T, std::size_t N>
char (&array_temp(const T (&a)[N]))[N];

//...ktor¹ mozna potem uzyc normalnie w sizeof i dzieki temu mamy const w compile-time. tak uzyteczne jak staromodne makro ale z pelna kontrola bledow
#define ARRAY_LENGTH(x) sizeof(array_temp(x))


#endif
