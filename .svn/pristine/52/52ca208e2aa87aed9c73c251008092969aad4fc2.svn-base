#ifndef _PARAMTRANS_H_
#define _PARAMTRANS_H_

#include "mutparamiface.h"
#include "param.h"

class TwoWayMap
{
	SListTempl<int> map, invmap;
	void printList(const SListTempl<int>&);
public:
	void reset(int size);
	int get(int pos);
	int invget(int pos);
	void insert(int pos);
	void remove(int pos);
	void print();
	int size() { return map.size(); }
	int invsize() { return invmap.size(); }
};

/** wrapper for MutableParamInterface providing constant property#'s.
	\warn group membership can change if the property is removed!

	properties handling:
	- adding and removing will not change property count and property#
	- the description (ParamInterface::id/name/type) of the removed property is always "?"
	- group membership of the removed property is always 0
	- requests for property# are redirected to the new property# or ignored
	groups handling:
	- group count is constant
	- accessing group name of the removed group will return "?"
	- removed properties are moved to group 0
	*/
class ParamTransaction : public ParamInterface
{
	MutableParamInterface &par;
	bool changed, grchanged;
	CallbackNode *panode, *pdnode, *pcnode, *ganode, *gdnode, *gcnode;

#define STATRICKCLASS ParamTransaction
	STCALLBACKDEF(onPropAdd);
	STCALLBACKDEF(onPropDelete);
	STCALLBACKDEF(onGroupAdd);
	STCALLBACKDEF(onGroupDelete);
	STCALLBACKDEF(onPropChange);
	STCALLBACKDEF(onGroupChange);
#undef STATRICKCLASS

	TwoWayMap propmap, groupmap;

	void resetMaps();

public:
	ParamTransaction(MutableParamInterface &mpi);
	~ParamTransaction();

	void reset();
	bool propChanged() { return changed; }
	bool groupChanged() { return grchanged; }

	int propertyPosition(int prop);
	int groupPosition(int group);

	int getGroupCount() { return groupmap.invsize(); }
	int getPropCount() { return propmap.invsize(); }
	const char * getName() { return par.getName(); }

	const char * id(int);
	const char * name(int);
	const char * type(int);
	const char * help(int);
	int flags(int);
	int group(int);
	const char * grname(int);
	int grmember(int, int);
	void call(int, class ExtValue *, class ExtValue *);
	class SString getString(int);
	paInt getInt(int);
	double getDouble(int);
	class ExtObject getObject(int);
	class ExtValue getExtValue(int);
	int setInt(int, paInt);
	int setDouble(int, double);
	int setString(int, const class SString &);
	int setObject(int, const class ExtObject &);
	int setExtValue(int, const class ExtValue &);

	void print()
	{
		printf("props(");
		propmap.print();
		printf(") groups(");
		groupmap.print();
		printf(")\n");
	}
};

#endif
