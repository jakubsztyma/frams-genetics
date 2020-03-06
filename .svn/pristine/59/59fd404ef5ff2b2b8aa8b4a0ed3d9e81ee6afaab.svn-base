// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _ADVLIST_H_
#define _ADVLIST_H_

#include "list.h"
#include "common/nonstd.h"
#include "callbacks.h"

////////////////////////////////////////////
/// no more DuoList -> now we have Callback class (callbacks.h)

/// list with notification support.
/// you can register callbacks which will be activated
/// as the list contents changes

/// sorting functions moved to SortView class

class AdvList : public SList
{
public:
	Callback l_add, l_del, l_mod, l_postdel;
	void remove(int);
	virtual void mod(int = -1); ///< call this when you change one or more elements in list (-1 means all elements)
	void clear();
	AdvList() {}
	~AdvList() { clear(); }
	int operator+=(void*);
	void operator-=(void*);
	void operator-=(int i) { remove(i); }
};

#endif
