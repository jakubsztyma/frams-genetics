// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MUTABLEPARAM_H_
#define _MUTABLEPARAM_H_

#include "mutparamiface.h"
#include <frams/util/extvalue.h>
#include "param.h"
#include <frams/util/callbacks.h>

class VMachine;
class VMCode;

class MutableParam : public SimpleAbstractParam, public MutableParamInterface
{
	static const int staticprops;
	static ParamEntry pe_tab[];
	/** group #0 cannot be removed by scripting  */
	int persistgroup0;
	SString grprefix;
	SList entries;
	SList groups;
	int changed;
	ParamEntry *entry(int i) { return (i < staticprops) ? pe_tab + i : ((ParamEntry*)entries(i - staticprops)); }
	void *getTarget(int i) { return (i < staticprops) ? SimpleAbstractParam::getTarget(i) : (void*)entry(i)->offset; }
	void call(int i, ExtValue* args, ExtValue *ret);
public:
	void clear(int everything = 0);
	int firstMutableIndex() { return staticprops; }
	SString& groupname(int g) { return *((SString*)groups(g)); }
	MutableParam(const char*n = 0, const char*g = 0, int gr0 = 1);
	void setGroupName(const SString &n, int g = 0) { groupname(g) = n; }
	~MutableParam() { clear(1); }
	int getGroupCount() { return groups.size(); }
	int getPropCount() { return entries.size() + staticprops; }
	const char *grname(int i) { return (i >= groups.size()) ? 0 : groupname(i).c_str(); }
	int grmember(int g, int a);

	int addGroup(const SString& gname, int noprefix = 0);
	void removeGroup(int pos);

	int findGroup(const SString name, int ignoreprefix = 0);

	/** @param data pointer to the variable. 0 will allocate new variable
		@param position -1 = after the last one  */
	int addProperty(void* data, const char* id, const char* type, const char* name, const char* help = 0, int flags = 0, int group = 0, int position = -1);

	int addProperty(ParamEntry *pe, int position = -1);
	ParamEntry * removeProperty(ParamEntry *pe);
	ParamEntry * removeProperty(int i);

	bool changeProperty(int i, const char* id, const char* type, const char* name, const char* help, int flags, int group);

	void notify(int id);

	int setInt(int, paInt);
	int setDouble(int, double);
	int setString(int, const SString &);
	int setObject(int, const ExtObject &);
	int setExtValue(int, const ExtValue &);

#define STATRICKCLASS MutableParam
	PARAMPROCDEF(p_clear);
	PARAMPROCDEF(p_addprop);
	PARAMPROCDEF(p_remprop);
	PARAMPROCDEF(p_changeprop);
	PARAMPROCDEF(p_addgroup);
	PARAMPROCDEF(p_remgroup);
	PARAMPROCDEF(p_exists);
	PARAMGETDEF(changedname) { arg1->setString(id(changed)); }
#undef STATRICKCLASS
};

class ParamSaver
{
	SList store;
public:
	virtual bool shouldLoad(ParamInterface &pi, int i) { return true; }
	ParamSaver() {}
	ParamSaver(ParamInterface &pi) { loadFrom(pi); }
	virtual ~ParamSaver() { clear(); }
	void loadFrom(ParamInterface& p);
	void saveTo(MutableParam& p);
	void clear();
};


#endif
