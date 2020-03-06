// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NILIBRARY_H_
#define _NILIBRARY_H_

#include <frams/util/advlist.h>
#include <frams/param/param.h>

class NeuroClass;

class NeuroLibrary
{
public:
	static NeuroLibrary staticlibrary;
	NeuroLibrary();
	~NeuroLibrary();
	AdvList classes;
	NeuroClass* findClass(const SString& classname, bool activeonly = 1);
	void addStandardClasses();
	int findClassIndex(const SString& classname, bool activeonly = 1);
	NeuroClass* getClass(int classindex) { return (NeuroClass*)classes(classindex); }
	SString getClassName(int classindex);
	int getClassCount() { return classes.size(); }
	void removeClass(int i);
	void clear();
	NeuroClass *addClass(NeuroClass *cls, bool replace = 1);
};

#endif
