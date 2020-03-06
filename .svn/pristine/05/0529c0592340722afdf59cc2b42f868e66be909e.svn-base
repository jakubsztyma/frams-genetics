// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neurofactory.h"
#include <frams/util/sstring.h>
#include <frams/param/param.h>
#include "neuroimpl.h"
#include "neurolibrary.h"

#include NEURO_IMPL_FILES

NeuroImpl* NeuroFactory::getImplementation(NeuroClass *nc)
{
	if (nc != NULL)
	{
		std::map<NeuroClass*, NeuroImpl*>::iterator it = impl.find(nc);
		if (it != impl.end())
			return it->second;
	}
	return NULL;
}

NeuroImpl* NeuroFactory::createNeuroImpl(NeuroClass *nc)
{
	if (!nc) return 0;
	if (!nc->active) return 0;
	NeuroImpl* ni = getImplementation(nc);
	if (!ni) return 0;
	ni = ni->makeNew();
	if (ni) ni->neuroclass = nc;
	return ni;
}

NeuroImpl* NeuroFactory::setImplementation(const SString& classname, NeuroImpl *ni, bool deleteold)
{
	NeuroClass *nc = Neuro::getClass(classname);
	if (!nc) return ni;
	return setImplementation(nc, ni, deleteold);
}

NeuroImpl* NeuroFactory::setImplementation(NeuroClass *nc, NeuroImpl *ni, bool deleteold)
{
	if (nc == NULL) return NULL;
	std::map<NeuroClass*, NeuroImpl*>::iterator it = impl.find(nc);
	NeuroImpl* old_ni = NULL;
	if (it == impl.end())
	{
		if (ni != NULL)
		{
			impl[nc] = ni;
			nc->impl_count++;
		}
		return NULL;
	}
	else
	{
		old_ni = it->second;
		if (ni)
			it->second = ni;
		else
		{
			impl.erase(it);
			nc->impl_count--;
		}
	}
	if (deleteold && old_ni) delete old_ni;
	return old_ni;
}

#include NEURO_CLS_FACTORY

void NeuroFactory::setStandardImplementation()
{
	SETIMPLEMENTATION
}

void NeuroFactory::freeImplementation()
{
	for (int i = 0; i < Neuro::getClassCount(); i++)
		setImplementation(Neuro::getClass(i), 0);
}

void NeuroFactory::removeUnimplemented()
{
	SString removed;
	for (int i = 0; i < Neuro::getClassCount(); i++)
	{
		NeuroClass *nc = Neuro::getClass(i);
		if (nc->impl_count == 0)
		{
			removed += nc->getName();
			removed += " ";
			NeuroLibrary::staticlibrary.classes -= i;
			i--;
			delete nc;
		}
	}
	if (removed.len())
		logPrintf("NeuroFactory", "removeUninmplemented", LOG_INFO,
		"Removed Neuro classes: %s", removed.c_str());
}
