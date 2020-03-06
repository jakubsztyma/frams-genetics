// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENETICNEUROPARAM_H_
#define _GENETICNEUROPARAM_H_

#include "neurolibparam.h"

class GeneticNeuroParam : public NeuroLibParam
{
protected:
	const char* types;
public:
	GeneticNeuroParam(const char* groupname, const char* myname, const char* prefix, const char* typ = 0);

	paInt getInt(int i);
	int setInt(int i, paInt v);
	const char *type(int i) { return types ? types : NeuroLibParam::type(i); }
};

#endif
