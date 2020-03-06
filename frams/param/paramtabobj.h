// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PARAMTABOBJ_H_
#define _PARAMTABOBJ_H_

#include "param.h"

class ParamTab
{
	ParamEntry *tab;
	int siz;
	void init() { siz = 0; tab = (ParamEntry*)calloc(sizeof(ParamEntry), 1); }
	void resize(int s);
	int measureTab(const ParamEntry *pe);

public:
	ParamTab(const ParamEntry* pe = 0, int maxcount = -1) { init(); if (pe) add(pe, maxcount); }
	ParamTab(const ParamTab& src) { init(); add(src); }
	~ParamTab() { clear(); }

	int size() const { return siz; }
	ParamEntry* getParamTab() const { return tab; }
	ParamEntry* operator()() const { return tab; }

	/** @return position of the last added entry */
	int add(const ParamEntry*, int count = 1);
	int add(const ParamTab& src) { return add(src.getParamTab(), src.size()); }
	void remove(int i, int count = 1);
	void clear() { resize(-1); }
};

#endif
