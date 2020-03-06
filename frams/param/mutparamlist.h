// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MUTPARAMLIST_H_
#define _MUTPARAMLIST_H_

#include "param.h"
#include "mutparamiface.h"
#include <frams/util/list.h>

struct ParamInfo;

class MutableParamList : public virtual ParamInterface, public MutableParamInterface
{
	SList list;
	const char *objname;
	int getSubParam(int i, ParamInterface **sub_p, int *sub_i);
	int getSubGroup(int i, ParamInterface **sub_p, int *sub_i);

	ParamInfo* getParamInfo(int i);
	void addPI(int pos, ParamInfo *pi);
	int findPI(ParamInfo *pi);
	int findPI(ParamInterface *p);
	int findPI(MutableParamInterface *p);
	void adjustPI(int firstPI, int addprop, int addgroup);
	void removePI(int pi_index);

#define STATRICKCLASS MutableParamList
	STCALLBACKDEF(onPropAdd);
	STCALLBACKDEF(onPropDelete);
	STCALLBACKDEF(onGroupAdd);
	STCALLBACKDEF(onGroupDelete);
	STCALLBACKDEF(onPropChange);
	STCALLBACKDEF(onGroupChange);
	STCALLBACKDEF(onPropActivate);
#undef STATRICKCLASS

public:

	void firePropChange(int i) { onchange.action(i); }
	void fireGroupChange(int i) { ongroupchange.action(i); }

	MutableParamList(const char* n = 0, const char* d = 0) :objname(n), description(d) {}
	~MutableParamList();

	void operator+=(ParamInterface *p);
	void operator-=(ParamInterface *p);
	void operator+=(MutableParamInterface *p);
	void operator-=(MutableParamInterface *p);
	void insert(int i, ParamInterface *p);
	void insert(int i, MutableParamInterface *p);
	void operator-=(int i);

	/** remove all sub params */
	void clear();

	const char* getName() { return objname; }
	const char* description;
	const char* getDescription() { return description; }

	int getGroupCount();
	int getPropCount();

	const char *id(int i);
	const char *name(int i);
	const char *type(int i);
	const char *help(int i);
	int flags(int i);
	int group(int i);
	void call(int i, ExtValue* args, ExtValue *ret);
	const char *grname(int i);

	int grmember(int gi, int n);

	SString getString(int);
	paInt getInt(int);
	double getDouble(int);
	ExtValue getExtValue(int);
	ExtObject getObject(int);

	int setInt(int, paInt);
	int setDouble(int, double);
	int setString(int, const SString &);
	int setObject(int, const ExtObject&);
	int setExtValue(int, const ExtValue&);
};

#endif
