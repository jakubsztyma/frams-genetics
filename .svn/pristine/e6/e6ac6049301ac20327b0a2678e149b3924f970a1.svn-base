// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geneticneuroparam.h"
#include <frams/neuro/neurolibrary.h>

GeneticNeuroParam::GeneticNeuroParam(const char* groupname, const char* myname,
	const char* prefix, const char* typ)
	:NeuroLibParam(groupname, myname, prefix), types(typ)
{}

paInt GeneticNeuroParam::getInt(int i)
{
	return Neuro::getClass(i)->genactive ? 1 : 0;
}

int GeneticNeuroParam::setInt(int i, paInt v)
{
	Neuro::getClass(i)->genactive = (v != 0); return PSET_CHANGED;
}

