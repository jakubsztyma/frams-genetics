// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROLIBPARAM_H_
#define _NEUROLIBPARAM_H_

#include <frams/param/mutparamiface.h>
#include <frams/model/modelparts.h>

class NeuroLibParam : public MutableParamInterface
{
	const char* mygroup;
	const char* myname;
	const char* myprefix;
	CallbackNode *anode, *dnode;

public:
	NeuroLibParam(const char* gr = 0, const char* name = 0, const char* pref = 0);
	~NeuroLibParam();

#define STATRICKCLASS NeuroLibParam
	VIRTCALLBACKDEF(neuroclassAdded);
	VIRTCALLBACKDEF(neuroclassRemoved);
#undef STATRICKCLASS

	int getGroupCount() { return 1; }
	int getPropCount()  { return Neuro::getClassCount(); }

	const char* getName() { return myname; }
	const char *grname(int gi) { return (gi == 0) ? mygroup : 0; }

	const char *id(int i);
	const char *name(int i);
	const char *help(int i);
	const char *type(int i) { return "d 0 1"; }
	int flags(int i)        { return 0; }
	int group(int i)        { return 0; }
	int grmember(int gi, int n);
	void call(int i, ExtValue* args, ExtValue *ret) {}

	SString getString(int i) { return SString(); }
	double getDouble(int) { return 0; }
	ExtObject getObject(int i) { return ExtObject(); }
	ExtValue getExtValue(int i) { return ExtValue(); }
	int setDouble(int i, double) { return 0; }
	int setString(int i, const SString &v) { return 0; }
	int setObject(int i, const ExtObject &v) { return 0; }
	int setExtValue(int i, const ExtValue &v) { return 0; }
};

#endif
