// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROFACTORY_H_
#define _NEUROFACTORY_H_

#include <frams/model/modelparts.h>
#include <map>

class NeuroImpl;

class NeuroFactory
{
	std::map<NeuroClass*, NeuroImpl*> impl;
public:
	NeuroImpl* setImplementation(NeuroClass *nc, NeuroImpl *ni, bool deleteold = 1);
	NeuroImpl* setImplementation(const SString& classname, NeuroImpl *ni, bool deleteold = 1);
	NeuroImpl* getImplementation(NeuroClass *nc);
	class NeuroImpl* createNeuroImpl(Neuro *n)
	{
		NeuroClass *nc = n->getClass(); if (!nc) return 0; return createNeuroImpl(nc);
	}
	class NeuroImpl* createNeuroImpl(NeuroClass *nc);

	void setStandardImplementation();
	void freeImplementation();
	void removeUnimplemented();
};

#endif
