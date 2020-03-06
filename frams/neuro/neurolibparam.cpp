// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <ctype.h>
#include "neurolibparam.h"
#include <frams/neuro/neurolibrary.h>

NeuroLibParam::NeuroLibParam(const char* gr, const char* name, const char* pref)
	:mygroup(gr), myname(name), myprefix(pref)
{
	anode = NeuroLibrary::staticlibrary.classes.l_add.add(
		STATRICKCALLBACK(this, &NeuroLibParam::neuroclassAdded, 0));
	dnode = NeuroLibrary::staticlibrary.classes.l_postdel.add(
		STATRICKCALLBACK(this, &NeuroLibParam::neuroclassRemoved, 0));
}

NeuroLibParam::~NeuroLibParam()
{
	NeuroLibrary::staticlibrary.classes.l_add.removeNode(anode);// remove(anode) tez powinno byc ok - do sprawdzenia
	NeuroLibrary::staticlibrary.classes.l_postdel.removeNode(dnode);
}

void NeuroLibParam::neuroclassAdded(void* data, intptr_t i)
{
	onadd.action(i);
}
void NeuroLibParam::neuroclassRemoved(void* data, intptr_t i)
{
	ondelete.action(i);
}

static bool isGoodName(const SString& name)
{
	for (int i = 0; i < name.len(); i++)
		if (!isalnum(name[i])) return false;
	return true;
}

const char *NeuroLibParam::id(int i)
{
	static SString t;
	if (i >= Neuro::getClassCount()) return 0;
	t = myprefix;
	SString n = Neuro::getClass(i)->getName();
	if (isGoodName(n))
		t += n;
	else
	{//jezeli w nazwie klasy neuronu sa "dziwne" znaki to zamiast tego uzywamy long name
		// * -> Constant, | -> Bend_muscle, @ -> Rotation_muscle
		n = Neuro::getClass(i)->getLongName();
		for (char* p = n.directWrite(); *p; p++)
			if (*p == ' ') *p = '_';
		n.endWrite();
		t += n;
	}
	return t.c_str();
}

const char *NeuroLibParam::name(int i)
{
	static SString t;
	t = Neuro::getClass(i)->getLongName();
	t += " ("; t += Neuro::getClassName(i); t += ")";
	return t.c_str();
}

const char *NeuroLibParam::help(int i)
{
	static SString t;
	t = Neuro::getClass(i)->getSummary();
	return t.c_str();
}

int NeuroLibParam::grmember(int gi, int n)
{
	return (gi == 0) ?
		((n >= getPropCount()) ? -9999 : n)
		:
		-9999;
}
