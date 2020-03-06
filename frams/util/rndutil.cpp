// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "rndutil.h"
#include <common/nonstd_math.h>
#ifndef IPHONE
#include <cstdint>
#endif
#include <stdlib.h>

unsigned short pseudornd(short x)
{
	static int32_t seed = 0;
	int32_t y;
	if (x <= 0) { seed = -x; return 0; }
	seed = (y = (3677 * seed + 3680) & 0x7fffffff) - 1;
	return (unsigned short)(((unsigned short)y) % (x)); //rzutowanie y->unsigned short to pewnie blad bo zmniejsza wartosc ktorej sie potem robi modulo, ale pseudornd sluzy chyba tylko do generowania randomowych world map? i modulo i tak jest tam bardzo male, lepiej niczego nie zmieniac bo po co maja pliki z ustawieniami zmienic swoje przypadkowe znaczenie
}

double CustomRnd(double *tab)
{
	double *range = tab + 1 + 2 * rndUint((int)(0.5 + tab[0]));
	return range[0] + rndDouble(range[1] - range[0]);
}

double RandomGener::Uni(double begin, double end)
{
	return begin + rndDouble(end - begin);
}

double RandomGener::GaussStd()
{
	if (isNextGauss) { isNextGauss = 0; return nextGauss; }
	double v1, v2, s;
	do {
		v1 = rndDouble(2) - 1; //-1..1
		v2 = rndDouble(2) - 1; //-1..1
		s = v1*v1 + v2*v2;
	} while (s >= 1);
	double mult = sqrt(-2 * log(s) / s);
	nextGauss = v2*mult;
	isNextGauss = 1;
	return v1*mult;
}

double RandomGener::Gauss(double m, double s)
{
	return m + s*GaussStd();
}

RandomGener RndGen;
