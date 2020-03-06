// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neuroimpl-simple.h"

#define NEURO_MAX 10.0
#define OVERLOAD 0.1
#define MUSCLESPEED   0.08
#define MUSCLEACC   0.01

int NI_StdNeuron::lateinit()
{
	istate = newstate + neuro->state; // neuro->state -> random initialization
	calcOutput();
	neuro->state = newstate;
	return 1;
}

void NI_StdNeuron::calcInternalState()
{
	double sum = getWeightedInputSum();
	velocity = force * (sum - istate) + inertia * velocity;
	istate += velocity;
	if (istate > NEURO_MAX) istate = NEURO_MAX;
	else if (istate < -NEURO_MAX) istate = -NEURO_MAX;
}

void NI_StdNeuron::go()
{
	calcInternalState();
	calcOutput();
}

void NI_StdNeuron::calcOutput()
{
	double s = istate * sigmo;
	if (s < -30.0) setState(-1);
	else setState(2.0 / (1.0 + exp(-s)) - 1.0); // -1...1
}

void NI_StdUNeuron::calcOutput()
{
	double s = istate * sigmo;
	if (s < -30.0) setState(0);
	else setState(1.0 / (1.0 + exp(-s))); // 0...1
}
