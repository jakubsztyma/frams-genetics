// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "advlist.h"

void AdvList::remove(int i)
{
	l_del.action(i);
	SList::remove(i);
	l_postdel.action(i);
}

void AdvList::clear()
{
	int i;
	for (i = size() - 1; i >= 0; i--) remove(i);
	resize(0);
	used = 0;
}

void AdvList::operator-=(void* e)
{
	int i = find(e);
	if (i >= 0) remove(i);
}

int AdvList::operator+=(void* e)
{
	int p = size();
	SList::operator+=(e);
	l_add.action(p);
	return p;
}

void AdvList::mod(int x)
{
	if (x < -1) x = -1;
	l_mod.action(x);
}
