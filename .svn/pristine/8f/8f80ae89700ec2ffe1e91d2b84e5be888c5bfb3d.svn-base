// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "nn_layout.h"
#include "common/nonstd_math.h"
#include <stdlib.h>

static void randomlayout(NNLayoutState*);
static void arraylayout(NNLayoutState*);

extern void smartlayout(NNLayoutState*);

struct NNLayoutFunction nn_layout_functions[] =
{
	{ "Random", randomlayout, },
	{ "Dumb array", arraylayout, },
	{ "Smart", smartlayout, },
	{ 0, }
};

static void randomlayout(NNLayoutState*nn)
{
	int i;
	int N = nn->GetElements();
	for (i = 0; i < N; i++)
	{
		nn->SetXYWH(i, rndUint(300), rndUint(300), 50, 50);
	}
}

static void arraylayout(NNLayoutState*nn)
{
	int e = 0, i, j;
	int N = nn->GetElements();
	int a = ((int)(sqrt(double(N)) - 0.0001)) + 1;
	for (j = 0; j < a; j++)
		for (i = 0; i < a; i++)
		{
		if (e >= N) return;
		nn->SetXYWH(e, 70 * i + ((i + j) & 3) * 4, 70 * j + ((2 + i + j) & 3) * 4, 50, 50);
		e++;
		}
}
