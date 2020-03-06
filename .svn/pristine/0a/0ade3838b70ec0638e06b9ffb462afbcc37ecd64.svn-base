// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neurolibrary.h"
#include <frams/param/param.h>
#include <frams/model/modelparts.h>

NeuroLibrary NeuroLibrary::staticlibrary;

int NeuroLibrary::findClassIndex(const SString & classname, bool activeonly)
{
	NeuroClass* cl;
	for (int i = 0; cl = (NeuroClass*)classes(i); i++)
	{
		if (activeonly && !cl->active) continue;
		if (classname == cl->getName()) return i;
	}
	return -1;
}

NeuroClass* NeuroLibrary::findClass(const SString & classname, bool activeonly)
{
	int i = findClassIndex(classname, activeonly);
	if (i < 0) return 0;
	return getClass(i);
}

SString NeuroLibrary::getClassName(int classindex)
{
	NeuroClass *cl = getClass(classindex);
	return cl ? cl->getName() : SString();
}

NeuroClass *NeuroLibrary::addClass(NeuroClass *cls, bool replace)
{
	NeuroClass *old = findClass(cls->getName());
	if (old)
	{
		if (replace)
		{
			classes -= old;
			classes += cls;
		}
	}
	else
		classes += cls;
	return old;
}

void NeuroLibrary::addStandardClasses()
{
#include NEURO_CLS_LIBRARY
}

NeuroLibrary::NeuroLibrary()
{
	addStandardClasses();
}

NeuroLibrary::~NeuroLibrary()
{
	FOREACH(NeuroClass*, cl, classes)
		delete cl;
}

void NeuroLibrary::removeClass(int i)
{
	classes -= i;
}

void NeuroLibrary::clear()
{
	while (getClassCount() > 0)
		removeClass(getClassCount() - 1);
}
