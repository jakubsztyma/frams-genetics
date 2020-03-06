// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NONSTD_MATH_H_
#define _NONSTD_MATH_H_


#ifdef _MSC_VER
 #define _USE_MATH_DEFINES //after this is defined, the next #include <math.h> or <cmath> will define M_PI etc.
 #include <math.h> //w vc2008 dzia³a³o tu <cmath>, ale w vc2010 juz nie bo "coœ" (jakiœ inny .h stl'a?) includuje wczeœniej <cmath> bez _USE_MATH_DEFINES, a <cmath> includuje <math.h> (ale tylko raz bo ma "include guards" jak kazdy .h)
 #include <float.h>
 //#define isnan(x) _isnan(x) //since 2014 we use std::isnan()
 #define finite(x) _finite(x)
#else //m.in. __BORLANDC__
 #include <math.h>
#endif



//random number generator:
#include "random.h"
RandomGenerator &rndGetInstance();

inline double rndDouble(double limit_exclusive) { return rndGetInstance().getDouble() * limit_exclusive; }
inline unsigned int rndUint(unsigned int limit_exclusive) { return (unsigned int)(rndGetInstance().getDouble() * limit_exclusive); } //returns random from 0..limit_exclusive-1
inline void rndSetSeed(unsigned int seed) { rndGetInstance().setSeed(seed); }
inline unsigned int rndRandomizeSeed() { return rndGetInstance().randomize(); }

//floating point specific numbers
#include "stdlib.h"

#ifdef __BORLANDC__
	#include <float.h>
	#define isnan(x) _isnan(x) //http://stackoverflow.com/questions/570669/checking-if-a-double-or-float-is-nan-in-c
	#define finite(x) _finite(x)
#endif

#ifdef LINUX
  #define _isnan(a) isnan(a)
#endif

#ifdef IPHONE
	#define finite(x) (!isinf(x))
  #define _isnan(a) isnan(a)
#endif


#if defined SHP
 //#define __assert_func(a,b,c,d) 0 //Currently, we are sorry to inform you that assert() is not yet supported. We have considered your request for internal discussion. Na szczêœcie jest w³asna (byle by by³a, bo i tak zak³adamy ze assert ktore przeciez dziala tylko w trybie debug nie jest potrzebne na bada) implementacja w "bada-assert.cpp"
 #define isnan(x) false //isnan() sie nie linkuje
 #define finite(x) true //j.w.
 //#include <cstdlib> //RAND_MAX defined incorrectly
 //#ifdef BADA_SIMULATOR //...but only in simulator libs
 // #undef RAND_MAX
 // #define RAND_MAX 32768 //...this is the actual value used by rand()
 //#endif
#endif


//handling floating point exceptions
void fpExceptInit(); //call once, before ...Enable/Disable
void fpExceptEnable();
void fpExceptDisable();

// std::lerp can be used since c++20 (and has some guaranteed properties probably better than this basic formula) but apparently it is not a template
template <typename Value, typename Linear> Value universal_lerp(Value a,Value b,Linear t) {return a*(1-t)+b*t;}

#endif
