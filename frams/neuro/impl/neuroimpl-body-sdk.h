// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROIMPLBODY_H_
#define _NEUROIMPLBODY_H_

#include <frams/neuro/neuroimpl.h>

class NI_Gyro : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Gyro(); } // for NeuroFactory
	int lateinit() { if (!neuro->joint) return 0; simorder = 0; return 1; }
	void go() { setState(0); }
};

extern ParamEntry NI_Touch_tab[];
class NI_Touch : public NeuroImpl
{
public:
	double range;
	NeuroImpl* makeNew() { return new NI_Touch(); } // for NeuroFactory
	int lateinit() { if (!neuro->part) return 0; simorder = 0; return 1; }
	void go() { setState(0); }
	NI_Touch() :range(1) { paramentries = NI_Touch_tab; }
};

class NI_Smell : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Smell(); } // for NeuroFactory
	void go() { setState(0); }
	int lateinit() { if (!neuro->part) return 0; simorder = 0; return 1; }
};

extern ParamEntry NI_BendMuscle_tab[];
class NI_BendMuscle : public NeuroImpl
{
public:
	double power, bendrange;
	NeuroImpl* makeNew() { return new NI_BendMuscle(); } // for NeuroFactory
	NI_BendMuscle() { paramentries = NI_BendMuscle_tab; }
	int lateinit() { if (!neuro->joint) return 0; simorder = 2; return 1; }
	void go() {}
};

extern ParamEntry NI_RotMuscle_tab[];
class NI_RotMuscle : public NeuroImpl
{
public:
	double power;
	NeuroImpl* makeNew() { return new NI_RotMuscle(); } // for NeuroFactory
	NI_RotMuscle() { paramentries = NI_RotMuscle_tab; }
	int lateinit() { if (!neuro->joint) return 0; simorder = 2; return 1; }
	void go() {}
};

extern ParamEntry NI_LinearMuscle_tab[];
class NI_LinearMuscle : public NeuroImpl
{
public:
	double power;
	NeuroImpl* makeNew() { return new NI_RotMuscle(); } // for NeuroFactory
	NI_LinearMuscle() { paramentries = NI_RotMuscle_tab; }
	int lateinit() { if (!neuro->joint) return 0; simorder = 2; return 1; }
	void go() {}
};

class NI_Sticky : public NeuroImpl
{
public:
	double power;
	NeuroImpl* makeNew() { return new NI_Sticky(); } // for NeuroFactory
	int lateinit() { if (!neuro->part) return 0; simorder = 0; return 1; }
	void go() {}
};

class NI_WaterDetect : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_WaterDetect(); } // for NeuroFactory
	int lateinit() { if (!neuro->part) return 0; simorder = 0; return 1; }
	void go() { setState(0); }
};

class NI_Energy : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Energy(); } // for NeuroFactory
	void go() { setState(0); }
};

#endif
