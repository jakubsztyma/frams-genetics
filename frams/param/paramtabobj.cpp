// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "paramtabobj.h"

int ParamTab::measureTab(const ParamEntry *pe)
{
	int i = 0;
	while (pe->id) { i++; pe++; }
	return i;
}

void ParamTab::resize(int s)
{
	if (s == siz) return;
	tab = (ParamEntry*)realloc(tab, sizeof(ParamEntry)*(s + 1));
	siz = s;
}

int ParamTab::add(const ParamEntry* p, int count)
{
	if (count < 0) count = measureTab(p);
	resize(siz + count);
	memmove(tab + siz - count, p, sizeof(ParamEntry)*count);
	memset(tab + siz, 0, sizeof(ParamEntry));
	if (siz > 0) tab[0].flags = (paInt)(siz - tab[0].group);
	return siz - 1;
}

void ParamTab::remove(int i, int count)
{
	memmove(tab + i, tab + i + count, sizeof(ParamEntry)*count);
	resize(siz - count);
	memset(tab + siz, 0, sizeof(ParamEntry));
	if (siz > 0) tab[0].flags = (paInt)(siz - tab[0].group);
}
