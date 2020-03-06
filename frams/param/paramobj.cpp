// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/param/paramobj.h>
#include <frams/util/extvalue.h>
#include <common/nonstd_stl.h>

static const char* maybedup(bool dup, const char* src)
{
	return dup ? (src ? strdup(src) : 0) : src;
}
static void maybefree(void* mem)
{
	if (mem) free(mem);
}

int ParamObject::firstFieldOffset()
{
	static ParamObject dummy(0, NULL);
	return ((char*)&dummy.fields[0]) - (char*)&dummy;
}

ParamEntry* ParamObject::makeParamTab(ParamInterface *pi, bool stripgroups, bool stripproc,
	int firstprop, int maxprops, bool dupentries, int flagsexclude_data, int flagsexclude_tab, bool addnew, const char* rename, bool readonly_into_userreadonly)
{
	// flagsexclude_data - skip while calculating data offsets
	// flagsexclude_tab - skip while creating paramtab
	// usually _data==_tab, but vmneuron public properties need _data=0 and _tab=PARAM_USERHIDDEN (data object has all fields, paramtab skips private fields)
	ParamEntry *tab, *t;
	int i, n, offset;
	static ExtValue ex;
	int count = 0, gcount = 1;
	if (!stripgroups) gcount = pi->getGroupCount();
	if (stripproc || flagsexclude_tab)
		for (int i = firstprop; i < pi->getPropCount(); i++)
		{
		const char*t = pi->type(i);
		if ((!stripproc) || (strchr("dfsox", *t)))
			if ((!flagsexclude_tab) || (!(pi->flags(i)&flagsexclude_tab)))
				if (++count >= maxprops) break;
		}
	else count = pi->getPropCount() - firstprop;
	if (addnew) count++;
	t = tab = (ParamEntry*)malloc(sizeof(ParamEntry)*(count + gcount + 1));
	t->group = (paInt)gcount;
	t->flags = (paInt)count;
	t->name = maybedup(dupentries, rename ? rename : pi->getName());
	t->type = maybedup(dupentries, pi->getDescription());
	for (i = 0; i < gcount; i++)
	{
		t->id = maybedup(dupentries, pi->grname(i));
		t->offset = 0;
		t++;
	}
	n = 1;
	offset = firstFieldOffset();
	if (addnew)
	{
		t->id = maybedup(dupentries, "new");
		t->name = maybedup(dupentries, "create new object");
		SString tmp = SString::sprintf("p o%s()", pi->getName());
		t->type = maybedup(dupentries, tmp.c_str());
		t->help = maybedup(dupentries, pi->help(i));
		t->flags = 0;
		t->group = 0;
		t->offset = PARAM_ILLEGAL_OFFSET;
		t->fun1 = (void*)p_new;
		t->fun2 = 0;
		t++;
	}
	for (i = firstprop; i < pi->getPropCount(); i++)
	{
		const char* type = pi->type(i);
		if ((!stripproc) || (strchr("dfsox", type[0])))
		{
			paInt flag = pi->flags(i);
			int tmp_offset;
			if ((!flagsexclude_data) || (!(flag&flagsexclude_data)))
			{
				if (type[0] == 'p')
					tmp_offset = 0;
				else
				{
					tmp_offset = offset;
					if (type[0] != 'x') tmp_offset += (((char*)&ex.data[0]) - ((char*)&ex));
					offset += sizeof(ExtValue);
				}
			}

			if ((!flagsexclude_tab) || (!(flag&flagsexclude_tab)))
			{
				t->offset = tmp_offset;
				t->group = (paInt)(stripgroups ? 0 : pi->group(i));
				t->flags = (paInt)flag;
				if (readonly_into_userreadonly && (t->flags & PARAM_READONLY))
					t->flags = (t->flags & ~PARAM_READONLY) | PARAM_USERREADONLY;
				t->fun1 = 0;
				t->fun2 = 0;
				t->id = maybedup(dupentries, pi->id(i));
				t->name = maybedup(dupentries, pi->name(i));
				t->type = maybedup(dupentries, type);
				t->help = maybedup(dupentries, pi->help(i));
				t++; n++;
				if (n > count) break;
			}
		}
	}
	t->id = 0; t->group = 0; t->flags = dupentries ? MUTPARAM_ALLOCENTRY : 0;
	return tab;
}

void ParamObject::setParamTabText(ParamEntry *pe, const char* &ptr, const char* txt)
{
	if (!paramTabAllocatedString(pe))
		return;
	maybefree((char*)ptr);
	ptr = maybedup(true, txt);
}

bool ParamObject::paramTabAllocatedString(ParamEntry *pe)
{
	return (pe[pe->flags + pe->group].flags & MUTPARAM_ALLOCENTRY) ? true : false;
}

void ParamObject::freeParamTab(ParamEntry *pe)
{
	if (paramTabAllocatedString(pe))
	{
		int i;
		ParamEntry *e;
		maybefree((void*)pe->name);
		maybefree((void*)pe->type);
		for (i = 0, e = pe; i < pe->group; i++, e++)
			maybefree((void*)e->id);
		for (i = pe->group, e = pe + i; i < pe->group + pe->flags; i++, e++)
		{
			maybefree((void*)e->id);
			maybefree((void*)e->name);
			maybefree((void*)e->type);
			maybefree((void*)e->help);
		}
	}
	free(pe);
}

bool ParamObject::paramTabEqual(ParamEntry *pe1, ParamEntry *pe2)
{
	if (pe1->flags != pe2->flags) return false;
	ParamEntry *e1 = pe1 + pe1->group, *e2 = pe2 + pe2->group;
	for (int i = 0; i < pe1->flags; i++, e1++, e2++)
	{
		if (strcmp(e1->id, e2->id)) return false;
		if (strcmp(e1->name, e2->name)) return false;
		if (strcmp(e1->type, e2->type)) return false;
		if (e1->offset != e2->offset) return false;
	}
	return true;
}

void ParamObject::p_new(void* obj, ExtValue *args, ExtValue *ret)
{
	ParamObject *this_obj = (ParamObject*)obj;
	ParamObject *po = makeObject(this_obj->par.getParamTab());
	ret->setObject(ExtObject(&this_obj->par, po));
	po->par.setDefault();
}

ParamObject::ParamObject(int _numfields, ParamEntry *_tab)
{
	numfields = _numfields;
	par.setParamTab(_tab);
	par.select(this);
	for (int i = 0; i < numfields; i++)
		new(&fields[i])ExtValue();
}

ParamObject::~ParamObject()
{
	for (int i = 0; i < numfields; i++)
		fields[i].~ExtValue();
}

ParamObject* ParamObject::makeObject(ParamEntry *tab)
{
	if (!tab) return NULL;
	int n = tab->flags, used_fields = 0;
	for (ParamEntry *t = tab + tab->group; n > 0; n--, t++)
		if (strchr("dfsox", t->type[0]))
			used_fields++;

	if (used_fields == 0) return NULL;
	ParamObject *obj = new(used_fields)ParamObject(used_fields, tab); // new(n): allocate n fields ; ParamObject(n,...): tell the object it has n fields
	ExtValue *v = &obj->fields[0];
	n = tab->flags;
	for (ParamEntry *t = tab + tab->group; n > 0; n--, t++)
		switch (*t->type)
	{
		case 'd': v->setInt(0); v++; break;
		case 'f': v->setDouble(0); v++; break;
		case 's': v->setString(SString::empty()); v++; break;
		case 'o':
		{
			ExtObject new_obj;
			if (t->flags & PARAM_OBJECTSET)
			{
				ParamInterface *cls = ExtValue::findDeserializableClass(t->type + 1);
				if (cls)
				{
					int new_fun = cls->findId("new");
					if (new_fun >= 0)
					{
						ExtValue dummy, new_value;
						cls->call(new_fun, &dummy, &new_value);
						new_obj = new_value.getObject();
					}
				}
			}
			v->setObject(new_obj);
			v++;
		}
			break;
		case 'x': v++; break;
	}
	return obj;
}

void ParamObject::operator=(const ParamObject& src)
{
	const ExtValue *s = &src.fields[0];
	ExtValue *d = &fields[0];
	int n = min(numfields, src.numfields);
	for (int i = 0; i < n; i++, d++, s++)
		*d = *s;
}

ParamObject* ParamObject::clone()
{
	ParamObject *c = new(numfields)ParamObject(numfields, par.getParamTab());
	*c = *this;
	return c;
}

void ParamObject::copyObject(void* dst, void* src)
{
	if ((!dst) || (!src)) return;
	ParamObject *s = (ParamObject*)src;
	ParamObject *d = (ParamObject*)dst;
	*d = *s;
}

void* ParamObject::dupObject(void* src)
{
	if (!src) return NULL;
	ParamObject *s = (ParamObject*)src;
	return s->clone();
}

void ParamObject::freeObject(void* obj)
{
	if (!obj) return;
	ParamObject *o = (ParamObject*)obj;
	delete o;
}
