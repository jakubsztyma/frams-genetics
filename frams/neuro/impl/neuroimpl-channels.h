// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROIMPLCHANNELS_H
#define _NEUROIMPLCHANNELS_H

#include <frams/neuro/neuroimpl.h>

class NI_Channelize : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_Channelize(); } // for NeuroFactory
	void go();
};

class NI_ChMux : public NeuroImpl
{
public:
	NeuroImpl* makeNew() { return new NI_ChMux(); } // for NeuroFactory
	void go();
};

extern ParamEntry NI_ChSel_tab[];

class NI_ChSel : public NeuroImpl
{
public:
	int ch; // channel
	NI_ChSel() :ch(0) { paramentries = NI_ChSel_tab; }
	NeuroImpl* makeNew() { return new NI_ChSel(); } // for NeuroFactory
	void go();
};


#endif
