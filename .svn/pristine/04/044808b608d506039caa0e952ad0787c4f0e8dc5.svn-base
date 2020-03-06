// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "mutableparam.h"
#include <frams/util/extvalue.h>

#define PROPERTY_WARNING "Using most _property functions is restricted for internal purposes. Use \"property:\" or \"state:\" definitions in your script files to change object properties."

#define FIELDSTRUCT MutableParam
ParamEntry MutableParam::pe_tab[] =
{
	{ "_propertyClear", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Remove all properties", "p()", PROCEDURE(p_clear), PROPERTY_WARNING },
	{ "_propertyAdd", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Add property (id,type,name,help)", "p(s id,s type description,s name,d flags,s help text)", PROCEDURE(p_addprop), PROPERTY_WARNING },
	{ "_propertyRemove", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Remove property", "p(d index)", PROCEDURE(p_remprop), PROPERTY_WARNING },
	{ "_propertyChange", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Change property", "p(s id,s type description,s name,d flags,s help text)", PROCEDURE(p_changeprop),  PROPERTY_WARNING },
	{ "_propertyAddGroup", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Add property group", "p(s name)", PROCEDURE(p_addgroup), PROPERTY_WARNING },
	{ "_propertyRemoveGroup", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Remove property group", "p(d index)", PROCEDURE(p_remgroup), PROPERTY_WARNING },
	{ "_propertyExists", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Check for property existence", "p d(s name)", PROCEDURE(p_exists) },
	{ "_property_changed_index", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN | PARAM_READONLY, "Last changed property index", "d", FIELD(changed), },
	{ "_property_changed_id", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN | PARAM_READONLY, "Last changed property id", "s", GETONLY(changedname), },
};
#undef FIELDSTRUCT
const int MutableParam::staticprops = ARRAY_LENGTH(pe_tab);

MutableParam::MutableParam(const char*n, const char*g, int gr0)
	:SimpleAbstractParam(this, n), persistgroup0(gr0), grprefix(g)
{
	if (persistgroup0)
		addGroup(grprefix, 1);
}

int MutableParam::findGroup(const SString name, int ignoreprefix)
{
	int skipprefix = grprefix.len() ? grprefix.len() + 2 : 0;
	for (int i = 0; i < groups.size(); i++)
	{
		if (ignoreprefix)
		{
			const char *noprefix = groupname(i).c_str();
			if ((int)strlen(noprefix) < skipprefix) continue;
			noprefix += skipprefix;
			if (!strcmp(noprefix, name.c_str())) return i;
		}
		else
			if (groupname(i) == name) return i;
	}
	return -1;
}

int MutableParam::addGroup(const SString& gname, int noprefix)
{
	SString tmp;
	if (noprefix)
		tmp = gname;
	else
	{
		tmp = grprefix;
		if (tmp.len()) tmp += ": ";
		tmp += gname;
	}
	groups += new SString(tmp);
	int position = groups.size() - 1;
	ongroupadd.action(position);
	return position;
}

void MutableParam::removeGroup(int g)
{
	if ((g < 0) || (g >= MutableParam::getGroupCount())) return;
	ParamEntry *e;
	for (int i = MutableParam::getPropCount() - 1; i >= staticprops; i--)
	{
		e = entry(i);
		if (g == e->group)
			removeProperty(i);
	}
	SString *s = (SString *)groups(g);
	if (s) delete s;
	groups -= g;
	ongroupdelete.action(g);
}

int MutableParam::grmember(int g, int a)
{
	if (g == 0)
	{
		if (getGroupCount() < 2)
			return (a < getPropCount()) ? a : -9999;
		if (a < staticprops) return a;
		a -= staticprops;
	}
	ParamEntry *e;
	for (int i = staticprops; e = entry(i); i++)
		if (g == e->group)
			if (!a--) return i;
	return -9999;
}

int MutableParam::addProperty(ParamEntry *pe, int position)
{
	DB(printf("MutableParam::add(%s)\n", pe->id));
	if (position < 0)
		position = entries.size() + staticprops;
	entries.insert(position - staticprops, pe);

	if (pe->offset)
		pe->flags &= ~MUTPARAM_ALLOCDATA;
	else
	{
		pe->flags |= MUTPARAM_ALLOCDATA;
		void *d = 0;
		switch (pe->type[0])
		{
		case 'd': d = new paInt(); *((paInt*)d) = 0; break;
		case 'f': d = new double(); *((double*)d) = 0; break;
		case 's': d = new SString(); break;
		case 'x': d = new ExtValue(); break;
		case 'o': d = new ExtObject(); break;
		}
		pe->offset = (intptr_t)d;
	}
	onadd.action(position);
	return position;
}

ParamEntry * MutableParam::removeProperty(ParamEntry *pe)
{
	int index = entries.find(pe);
	if (index >= 0) return removeProperty(index); else return pe;
}

ParamEntry * MutableParam::removeProperty(int i)
{
	ParamEntry *pe = (ParamEntry *)entries(i - staticprops);
	DB(printf("MutableParam::remove(%d)\n", i));
	void *d = (void*)pe->offset;
	if (d && (pe->flags & MUTPARAM_ALLOCDATA))
		switch (pe->type[0])
		{
		case 'd': delete (paInt*)d; break;
		case 'f': delete (double*)d; break;
		case 's': delete (SString*)d; break;
		case 'x': delete (ExtValue*)d; break;
		case 'o': delete (ExtObject*)d; break;
		}
	entries -= i - staticprops;
	if (pe->flags & MUTPARAM_ALLOCENTRY)
	{
		if (pe->name) free((void*)pe->name);
		if (pe->id) free((void*)pe->id);
		if (pe->help) free((void*)pe->help);
		if (pe->type) free((void*)pe->type);
		delete pe;
	}
	ondelete.action(i);
	return pe;
}

void MutableParam::clear(int everything)
{
	DB(printf("MutableParam::clear\n"));
	for (int i = entries.size() - 1; i >= 0; i--)
		removeProperty(i + staticprops);
	int lastgroup = (everything || (persistgroup0 == 0)) ? 0 : 1;
	for (int i = groups.size() - 1; i >= lastgroup; i--)
		removeGroup(i);
}

void MutableParam::p_clear(ExtValue *args, ExtValue *ret)
{
	clear();
}

int MutableParam::addProperty(void* data, const char* id, const char* type, const char* name, const char* help, int flags, int group, int position)
{
	if ((!id) && (!type)) return -1;
	if (!isValidTypeDescription(type)) return -1;
	ParamEntry *pe = new ParamEntry();
	pe->fun1 = 0; pe->fun2 = 0;
	pe->group = (paInt)group;
	pe->flags = (paInt)(flags | MUTPARAM_ALLOCENTRY);
	pe->offset = (intptr_t)data;
	pe->id = strdup(id);
	pe->type = strdup(type);
	pe->name = name ? strdup(name) : 0;
	pe->help = help ? strdup(help) : 0;
	return addProperty(pe, position);
}

static void changeString(const char* (&s), const char* newstr)
{
	if ((newstr != NULL) && (newstr[0] == 0)) newstr = NULL;
	if ((s == NULL) && (newstr == NULL)) return;
	if ((s != NULL) && (newstr != NULL) && (strcmp(s, newstr) == 0)) return;
	if (s != NULL) { free((void*)s); s = NULL; }
	if (newstr != NULL) s = strdup(newstr);
}

bool MutableParam::changeProperty(int i, const char* id, const char* type, const char* name, const char* help, int flags, int group)
{
	ParamEntry *pe = entry(i);
	if ((!id) && (!type)) return false;
	if (!isValidTypeDescription(type)) return false;
	pe->group = (paInt)group;
	pe->flags = (pe->flags & (MUTPARAM_ALLOCENTRY | MUTPARAM_ALLOCDATA)) | (flags & ~(MUTPARAM_ALLOCENTRY | MUTPARAM_ALLOCDATA));
	changeString(pe->id, id);
	changeString(pe->name, name);
	changeString(pe->type, type);
	changeString(pe->help, help);
	onchange.action(i);
	return true;
}

void MutableParam::p_addprop(ExtValue *args, ExtValue *ret)
{
	int i = addProperty(0, args[4].getString().c_str(), args[3].getString().c_str(), args[2].getString().c_str(), args[0].getString().c_str(), args[1].getInt());
	ret->setInt(i);
}

void MutableParam::p_changeprop(ExtValue *args, ExtValue *ret)
{
	int i = findId(args[4].getString().c_str());
	if (i >= staticprops)
	{
		changeProperty(i, args[4].getString().c_str(), args[3].getString().c_str(), args[2].getString().c_str(), args[0].getString().c_str(), args[1].getInt(), entry(i)->group);
		ret->setInt(i);
	}
	else
		ret->setEmpty();
}

void MutableParam::p_remprop(ExtValue *args, ExtValue *ret)
{
	removeProperty(args[0].getInt());
}

void MutableParam::p_addgroup(ExtValue *args, ExtValue *ret)
{
	int i = addGroup(args[0].getString());
	ret->setInt(i);
}

void MutableParam::p_remgroup(ExtValue *args, ExtValue *ret)
{
	removeGroup(args[0].getInt());
}

void MutableParam::p_exists(ExtValue *args, ExtValue *ret)
{
	ret->setInt(findId(args->getString().c_str()) >= 0);
}

void MutableParam::notify(int id)
{
	changed = id;
	onactivate.action(id);
}

int MutableParam::setInt(int i, paInt v)
{
	int ret = SimpleAbstractParam::setInt(i, v);
	if (ret & PSET_CHANGED) notify(i);
	return ret;
}

int MutableParam::setDouble(int i, double v)
{
	int ret = SimpleAbstractParam::setDouble(i, v);
	if (ret & PSET_CHANGED) notify(i);
	return ret;
}

int MutableParam::setString(int i, const SString &v)
{
	int ret = SimpleAbstractParam::setString(i, v);
	if (ret & PSET_CHANGED) notify(i);
	return ret;
}

int MutableParam::setObject(int i, const ExtObject &v)
{
	int ret = SimpleAbstractParam::setObject(i, v);
	if (ret & PSET_CHANGED) notify(i);
	return ret;
}

int MutableParam::setExtValue(int i, const ExtValue &v)
{
	int ret = SimpleAbstractParam::setExtValue(i, v);
	if (ret & PSET_CHANGED) notify(i);
	return ret;
}

void MutableParam::call(int i, ExtValue* args, ExtValue *ret)
{
	if (i < staticprops) return SimpleAbstractParam::call(i, args, ret);
	notify(i);
}

///////////////////

void ParamSaver::clear()
{
	for (int i = 0; i < store.size(); i += 2)
	{
		SString *n = (SString*)store(i);
		ExtValue *v = (ExtValue*)store(i + 1);
		delete n;
		delete v;
	}
	store.clear();
}

void ParamSaver::loadFrom(ParamInterface& p)
{
	int N = p.getPropCount();
	for (int i = 0; i < N; i++)
	{
		if (shouldLoad(p, i))
		{
			ExtValue v;
			p.get(i, v);
			store += new SString(p.id(i));
			store += new ExtValue(v);
		}
	}
}

void ParamSaver::saveTo(MutableParam& p)
{
	for (int i = 0; i < store.size(); i += 2)
	{
		SString *n = (SString*)store(i);
		int prop = p.findId(n->c_str());
		if (prop < 0)
			prop = p.addProperty(0, n->c_str(), "x", 0, 0, 0, 0, -1);
		p.setExtValue(prop, *(ExtValue*)store(i + 1));
	}
}
