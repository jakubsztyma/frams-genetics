// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neuroimpl-channels.h"
#include <common/nonstd_stl.h>

void NI_Channelize::go()
{
	setChannelCount(getInputCount());
	for (int i = 0; i < getInputCount(); i++)
		setState(getWeightedInputState(i), i);
}

void NI_ChMux::go()
{
	int c = getInputChannelCount(1);
	if (c < 2) { setState(getWeightedInputState(1)); return; }
	double s = getWeightedInputState(0);
	s = (max(-1.0, min(1.0, s)) + 1.0) / 2.0; // 0..1
	int i1;
	i1 = (int)(s * (c - 1)); i1 = max(0, min(i1, c - 2));
	double sw = 1.0 / (c - 1);
	double s1 = sw * i1;
	double w1 = fabs((s - s1) / sw);
	double w2 = 1.0 - w1;
	double is1 = getWeightedInputState(1, i1);
	double is2 = getWeightedInputState(1, i1 + 1);
	setState(is1 * w2 + is2 * w1);
}

void NI_ChSel::go()
{
	setState(getWeightedInputState(0, ch));
}
