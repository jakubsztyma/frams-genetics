// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "mutparamlist.h"
#include <frams/util/extvalue.h>

struct ParamInfo
{
	ParamInterface *pi;
	MutableParamInterface *mpi;
	CallbackNode *anode, *dnode, *ganode, *gdnode, *cnode, *gcnode, *panode;
	int firstprop, firstgroup;
	int propcount, groupcount;
};

ParamInfo* MutableParamList::getParamInfo(int i)
{
	return (ParamInfo*)list(i);
}

void MutableParamList::addPI(int pos, ParamInfo *pi)
{
	int propcount = pi->propcount;
	int groupcount = pi->groupcount;
	if (pos == 0)
	{
		pi->firstprop = 0;
		pi->firstgroup = 0;
	}
	else
	{
		ParamInfo *prev_pi = getParamInfo(pos - 1);
		pi->firstprop = prev_pi->firstprop + prev_pi->propcount;
		pi->firstgroup = prev_pi->firstgroup + prev_pi->groupcount;
	}
	list.insert(pos, pi);
	pi->propcount = 0;
	pi->groupcount = 0;
	for (int i = 0; i < groupcount; i++)
	{
		pi->groupcount = i + 1;
		adjustPI(pos + 1, 0, 1);
		ongroupadd.action(pi->firstgroup + i);
	}
	for (int i = 0; i < propcount; i++)
	{
		pi->propcount = i + 1;
		adjustPI(pos + 1, 1, 0);
		onadd.action(pi->firstprop + i);
	}
	if (pi->mpi)
	{
		pi->anode = pi->mpi->onadd.add(STATRICKCALLBACK(this, &MutableParamList::onPropAdd, pi));
		pi->ganode = pi->mpi->ongroupadd.add(STATRICKCALLBACK(this, &MutableParamList::onGroupAdd, pi));
		pi->dnode = pi->mpi->ondelete.add(STATRICKCALLBACK(this, &MutableParamList::onPropDelete, pi));
		pi->gdnode = pi->mpi->ongroupdelete.add(STATRICKCALLBACK(this, &MutableParamList::onGroupDelete, pi));
		pi->cnode = pi->mpi->onchange.add(STATRICKCALLBACK(this, &MutableParamList::onPropChange, pi));
		pi->gcnode = pi->mpi->ongroupchange.add(STATRICKCALLBACK(this, &MutableParamList::onGroupChange, pi));
		pi->panode = pi->mpi->onactivate.add(STATRICKCALLBACK(this, &MutableParamList::onPropActivate, pi));
	}
}

int MutableParamList::findPI(ParamInfo *pi)
{
	return list.find((void*)pi);
}

int MutableParamList::findPI(ParamInterface *p)
{
	ParamInfo *pi;
	for (int i = 0; pi = (ParamInfo*)list(i); i++)
		if ((!pi->mpi) && (pi->pi == p)) return i;
	return -1;
}

int MutableParamList::findPI(MutableParamInterface *p)
{
	ParamInfo *pi;
	for (int i = 0; pi = (ParamInfo*)list(i); i++)
		if ((pi->mpi) && (pi->mpi == p)) return i;
	return -1;
}

void MutableParamList::adjustPI(int firstPI, int addprop, int addgroup)
{
	ParamInfo *pi;
	for (int i = firstPI; pi = getParamInfo(i); i++)
	{
		pi->firstprop += addprop;
		pi->firstgroup += addgroup;
	}
}

void MutableParamList::removePI(int pi_index)
{
	if (pi_index < 0) return;
	ParamInfo *pi = getParamInfo(pi_index);
	for (int i = pi->propcount - 1; i >= 0; i--)
	{
		pi->propcount = i;
		adjustPI(pi_index + 1, -1, 0);
		ondelete.action(i);
	}
	for (int i = pi->groupcount - 1; i >= 0; i--)
	{
		pi->groupcount = i;
		adjustPI(pi_index + 1, 0, -1);
		ongroupdelete.action(i);
	}
	list -= (pi_index);
	if (pi->mpi)
	{
		pi->mpi->onadd.remove(pi->anode);
		pi->mpi->ongroupadd.remove(pi->ganode);
		pi->mpi->ondelete.remove(pi->dnode);
		pi->mpi->ongroupdelete.remove(pi->gdnode);
		pi->mpi->onchange.remove(pi->cnode);
		pi->mpi->ongroupchange.remove(pi->gcnode);
		pi->mpi->onactivate.remove(pi->panode);
	}
	delete pi;
}

MutableParamList::~MutableParamList()
{
	for (int i = list.size() - 1; i >= 0; i--)
		removePI(i);
}

void MutableParamList::onPropAdd(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	pi->propcount++;
	int j = findPI(pi);
	if (j >= 0)
		adjustPI(j + 1, 1, 0);
	onadd.action(pi->firstprop + i);
}

void MutableParamList::onPropDelete(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	pi->propcount--;
	int j = findPI(pi);
	if (j >= 0)
		adjustPI(j + 1, -1, 0);
	ondelete.action(pi->firstprop + i);
}

void MutableParamList::onPropChange(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	onchange.action(pi->firstprop + i);
}

void MutableParamList::onPropActivate(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	onactivate.action(pi->firstprop + i);
}

void MutableParamList::onGroupAdd(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	pi->groupcount++;
	int j = findPI(pi);
	if (j >= 0)
		adjustPI(j + 1, 0, 1);
	ongroupadd.action(pi->firstgroup + i);
}

void MutableParamList::onGroupDelete(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	pi->groupcount--;
	int j = findPI(pi);
	if (j >= 0)
		adjustPI(j + 1, 0, -1);
	ongroupdelete.action(pi->firstgroup + i);
}

void MutableParamList::onGroupChange(void* data, intptr_t i)
{
	ParamInfo *pi = (ParamInfo*)data;
	ongroupchange.action(pi->firstgroup + i);
}

void MutableParamList::insert(int pos, MutableParamInterface *p)
{
	ParamInfo *pi = new ParamInfo();
	pi->pi = (ParamInterface*)p;
	pi->mpi = p;
	pi->propcount = p->getPropCount();
	pi->groupcount = p->getGroupCount();
	addPI(pos, pi);
}

void MutableParamList::insert(int pos, ParamInterface *p)
{
	ParamInfo *pi = new ParamInfo();
	pi->pi = p;
	pi->mpi = 0;
	pi->propcount = p->getPropCount();
	pi->groupcount = p->getGroupCount();
	addPI(pos, pi);
}


void MutableParamList::operator+=(ParamInterface *p)
{
	insert(list.size(), p);
}

void MutableParamList::operator+=(MutableParamInterface *p)
{
	insert(list.size(), p);
}


void MutableParamList::operator-=(ParamInterface *p)
{
	int i = findPI(p);
	removePI(i);
}

void MutableParamList::operator-=(MutableParamInterface *p)
{
	int i = findPI(p);
	removePI(i);
}

void MutableParamList::operator-=(int i)
{
	removePI(i);
}

int MutableParamList::getGroupCount()
{
	int count = 0;
	FOREACH(ParamInfo*, pi, list)
		count += pi->groupcount;
	return count;
}

int MutableParamList::getPropCount()
{
	int count = 0;
	FOREACH(ParamInfo*, pi, list)
		count += pi->propcount;
	return count;
}

int MutableParamList::getSubParam(int i, ParamInterface **sub_p, int *sub_i)
{
	int n;
	FOREACH(ParamInfo*, pi, list)
	{
		if (i < (n = pi->propcount))
		{
			*sub_p = pi->pi;
			*sub_i = i;
			return 1;
		}
		i -= n;
	}
	return 0;
}

int MutableParamList::getSubGroup(int i, ParamInterface **sub_p, int *sub_i)
{
	int n;
	FOREACH(ParamInfo*, pi, list)
	{
		if (i < (n = pi->groupcount))
		{
			*sub_p = pi->pi;
			*sub_i = i;
			return 1;
		}
		i -= n;
	}
	return 0;
}

#define FUN(_type_,_name_,_ret_) \
_type_ MutableParamList:: _name_ (int i) \
{ \
int j; ParamInterface *pi; \
if (!getSubParam(i,&pi,&j)) return _ret_; \
return pi-> _name_ (j); \
}

FUN(const char*, id, 0)
FUN(const char*, name, 0)
FUN(const char*, type, 0)
FUN(const char*, help, 0)
FUN(int, flags, 0)
FUN(SString, getString, SString())
FUN(paInt, getInt, 0)
FUN(double, getDouble, 0)
FUN(ExtValue, getExtValue, ExtValue((paInt)0))
FUN(ExtObject, getObject, ExtObject())

int MutableParamList::group(int i)
{
	int n;
	int g = 0;
	FOREACH(ParamInfo*, pi, list)
	{
		if (i < (n = pi->propcount))
			return g + pi->pi->group(i);
		g += pi->groupcount;
		i -= n;
	}
	return 0;
}

#define FUN2(_type_,_name_,_argt_) \
_type_ MutableParamList:: _name_ (int i,_argt_ v) \
{ \
int j; ParamInterface *pi; \
if (!getSubParam(i,&pi,&j)) return 0; \
return pi-> _name_ (j,v); \
}

FUN2(int, setInt, paInt)
FUN2(int, setDouble, double)
FUN2(int, setString, const SString &)
FUN2(int, setObject, const ExtObject &)
FUN2(int, setExtValue, const ExtValue &)

void MutableParamList::call(int i, ExtValue* args, ExtValue *ret)
{
	int j; ParamInterface *pi;
	if (!getSubParam(i, &pi, &j)) return;
	pi->call(j, args, ret);
}

const char *MutableParamList::grname(int i)
{
	int j; ParamInterface *pi;
	if (!getSubGroup(i, &pi, &j)) return 0;
	return pi->grname(j);
}

int MutableParamList::grmember(int gi, int i)
{
	int n;
	int count = 0;
	FOREACH(ParamInfo*, pi, list)
	{
		if (gi < (n = pi->groupcount))
		{
			int prop = pi->pi->grmember(gi, i);
			if (prop >= pi->propcount) return -9999;
			return count + prop;
		}
		count += pi->propcount;
		gi -= n;
	}
	return -9999;
}

void MutableParamList::clear()
{
	for (int i = list.size() - 1; i >= 0; i--)
		operator-=(i);
}
