#include "paramtrans.h"
#include <frams/util/extvalue.h>

void TwoWayMap::printList(const SListTempl<int>& L)
{
	for (int i = 0; i < L.size(); i++)
	{
		int x = L(i);
		if (x >= 0) printf("%d", x);
		else printf("x");
	}
}

void TwoWayMap::print()
{
	printList(map);
	printf("/");
	printList(invmap);
}

void TwoWayMap::reset(int size)
{
	map.clear(); invmap.clear();
	for (int i = 0; i < size; i++)
	{
		map.set(i, i);
		invmap.set(i, i);
	}
}

/** @return -2 = out of bounds */
int TwoWayMap::get(int pos)
{
	if ((pos < 0) || (pos >= map.size())) return -2;
	return map(pos);
}

/** @return -2 = out of bounds */
int TwoWayMap::invget(int pos)
{
	if ((pos < 0) || (pos >= invmap.size())) return -2;
	return invmap(pos);
}

void TwoWayMap::insert(int i)
{
	// map/invmap: 01234/01234 add(1) -> 0-1234/02345
	//             1--40-2/406-3 add(1) -> 1---40-2/507-4
	map.insert(i, -1);
	for (int p = i + 1; p < map.size(); p++)
	{
		int x = map(p);
		if (x >= 0)
			invmap.set(x, p);
	}
}

void TwoWayMap::remove(int i)
{
	// map/invmap: 01234/01234 rem(1) -> 0234/0-123
	//             1--40-2/406-3 rem(1) -> 1-40-2/305-2
	int x = map(i);
	if (x >= 0) invmap.set(x, -1);
	map.remove(i);
	for (int p = i; p < map.size(); p++)
	{
		int x = map(p);
		if (x >= 0)
			invmap.set(x, p);
	}
}

///////////////////////////

ParamTransaction::ParamTransaction(MutableParamInterface &mpi)
	:par(mpi)
{
	panode = par.onadd.add(STATRICKCALLBACK(this, &ParamTransaction::onPropAdd, 0));
	pdnode = par.ondelete.add(STATRICKCALLBACK(this, &ParamTransaction::onPropDelete, 0));
	pcnode = par.onchange.add(STATRICKCALLBACK(this, &ParamTransaction::onPropChange, 0));
	ganode = par.ongroupadd.add(STATRICKCALLBACK(this, &ParamTransaction::onGroupAdd, 0));
	gdnode = par.ongroupdelete.add(STATRICKCALLBACK(this, &ParamTransaction::onGroupDelete, 0));
	gcnode = par.ongroupchange.add(STATRICKCALLBACK(this, &ParamTransaction::onGroupChange, 0));
	reset();
}

ParamTransaction::~ParamTransaction()
{
	par.onadd.removeNode(panode);
	par.ondelete.removeNode(pdnode);
	par.onchange.removeNode(pcnode);
	par.ongroupadd.removeNode(ganode);
	par.ongroupdelete.removeNode(gdnode);
	par.ongroupchange.removeNode(gcnode);
}

void ParamTransaction::reset()
{
	resetMaps();
	changed = false;
	grchanged = false;
}

void ParamTransaction::onPropAdd(void* data, intptr_t i)
{
	propmap.insert((int)i);
	changed = true;
}

void ParamTransaction::onPropDelete(void* data, intptr_t i)
{
	propmap.remove((int)i);
	changed = true;
}

void ParamTransaction::onGroupAdd(void* data, intptr_t i)
{
	groupmap.insert((int)i);
	grchanged = true;
}

void ParamTransaction::onGroupDelete(void* data, intptr_t i)
{
	groupmap.remove((int)i);
	grchanged = true;
}

void ParamTransaction::onPropChange(void* data, intptr_t i)
{
	changed = true;
}

void ParamTransaction::onGroupChange(void* data, intptr_t i)
{
	grchanged = true;
}

void ParamTransaction::resetMaps()
{
	propmap.reset(par.getPropCount());
	groupmap.reset(par.getGroupCount());
}

int ParamTransaction::propertyPosition(int prop)
{
	return propmap.invget(prop);
}

int ParamTransaction::groupPosition(int group)
{
	return groupmap.invget(group);
}

const char * ParamTransaction::id(int i)
{
	i = propertyPosition(i);
	if (i == -2) return 0;
	if (i >= 0) return par.id(i); else return "?";
}

const char * ParamTransaction::name(int i)
{
	i = propertyPosition(i);
	if (i == -2) return 0;
	if (i >= 0) return par.name(i); else return "?";
}

const char * ParamTransaction::type(int i)
{
	i = propertyPosition(i);
	if (i == -2) return 0;
	if (i >= 0) return par.type(i); else return "?";
}

const char * ParamTransaction::help(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.help(i) : 0;
}

int ParamTransaction::flags(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.flags(i) : 0;
}

int ParamTransaction::group(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.group(i) : 0;
}

const char * ParamTransaction::grname(int i)
{
	i = groupPosition(i);
	if (i == -2) return 0;
	return (i >= 0) ? par.grname(i) : "?";
}

int ParamTransaction::grmember(int g, int i)
{
	g = groupPosition(g);
	if (g < 0) return -9999;
	for (int p = 0; p < getPropCount(); p++)
		if (group(p) == g)
			if (!i--)
				return p;
	return -9999;
}

void ParamTransaction::call(int i, ExtValue *a, ExtValue *b)
{
	i = propertyPosition(i);
	if (i >= 0) par.call(i, a, b);
}

SString ParamTransaction::getString(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.getString(i) : SString();
}

paInt ParamTransaction::getInt(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.getInt(i) : 0;
}

double ParamTransaction::getDouble(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.getDouble(i) : 0;
}

ExtObject ParamTransaction::getObject(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.getObject(i) : ExtObject();
}

ExtValue ParamTransaction::getExtValue(int i)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.getExtValue(i) : ExtValue();
}

int ParamTransaction::setInt(int i, paInt v)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.setInt(i, v) : PSET_NOPROPERTY;
}

int ParamTransaction::setDouble(int i, double v)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.setDouble(i, v) : PSET_NOPROPERTY;
}

int ParamTransaction::setString(int i, const SString &v)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.setString(i, v) : PSET_NOPROPERTY;
}

int ParamTransaction::setObject(int i, const ExtObject &v)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.setObject(i, v) : PSET_NOPROPERTY;
}

int ParamTransaction::setExtValue(int i, const ExtValue &v)
{
	i = propertyPosition(i);
	return (i >= 0) ? par.setExtValue(i, v) : PSET_NOPROPERTY;
}
