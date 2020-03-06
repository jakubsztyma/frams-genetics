// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _RNDUTIL_H_
#define _RNDUTIL_H_

#include <common/nonstd.h>
#include "math.h"

/** @file rndutil.h various functions */

/** @param x change seed if x<=0
	@return random value [0..x-1] if x>0 */
unsigned short pseudornd(short x);

double CustomRnd(double*);
// the parameter is a table which describes the random distribution:
// e.g.
// double x[]={ 3,     // 3 intervals
//  -10,-1,            // (each of them has always uniform distribution and equal probability of being selected)
//  -1, 1,
//   1,10, };

class RandomGener
{
public:
	RandomGener() { isNextGauss = 0; }
	static double Uni(double begin, double end); ///< uniform excluding 'end' boundary
	double GaussStd();
	double Gauss(double m, double s); ///< usually will not return further than 5*stdd
private:
	int isNextGauss;
	double nextGauss;
};

extern RandomGener RndGen;

#endif
