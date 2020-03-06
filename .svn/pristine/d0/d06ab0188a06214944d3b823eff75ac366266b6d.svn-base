// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROIMPLSIMPLE_H_
#define _NEUROIMPLSIMPLE_H_

#include <frams/neuro/neuroimpl.h>
#include <common/nonstd_math.h>

extern ParamEntry NI_StdNeuron_tab[];

class NI_StdNeuron : public NeuroImpl
{
protected:
	double istate, velocity;
	void calcInternalState();
	virtual void calcOutput();
public:
	double inertia, force, sigmo;
	NI_StdNeuron() :velocity(0), inertia(0), force(0), sigmo(0)
	{
		paramentries = NI_StdNeuron_tab;
	}
	NeuroImpl* makeNew() { return new NI_StdNeuron(); } // for NeuroFactory
	int lateinit();
	void go();
};

extern ParamEntry NI_StdUNeuron_tab[];

class NI_StdUNeuron : public NI_StdNeuron
{
public:
	NI_StdUNeuron()
	{
		paramentries = NI_StdUNeuron_tab;
	}
	NeuroImpl* makeNew() { return new NI_StdUNeuron(); } // for NeuroFactory
	void calcOutput();
};

class NI_Const : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Const(); } // for NeuroFactory
	int lateinit()
	{
		neuro->state = newstate = 1.0;
		simorder = 0;
		return 1;
	}
};

class NI_Diff : public NeuroImpl
{
	double previous;
public:
	NeuroImpl* makeNew() { return new NI_Diff(); };

	void go()
	{
		double s = getWeightedInputSum();
		setState(s - previous);
		previous = s;
	}
	int lateinit()
	{
		NeuroImpl::lateinit();
		previous = neuro->state;
		return 1;
	}
};

class NI_Random : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Random(); };
	void go() { setState(rndDouble(2) - 1.0); }
};

extern ParamEntry NI_Sinus_tab[];

class NI_Sinus : public NeuroImpl
{
public:
	double f0, t;
	NeuroImpl* makeNew() { return new NI_Sinus(); };
	NI_Sinus() :f0(0), t(0)
	{
		paramentries = NI_Sinus_tab;
	}
	void go()
	{
		t += f0 + getWeightedInputSum();
		setState(sin(t));
	}
};

#endif
