// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _COLLECTIONOBJ_H_
#define _COLLECTIONOBJ_H_

#include <frams/param/param.h>
#include <frams/util/extvalue.h>
#include <frams/util/hashtable.h>
#include <frams/util/3d.h>

/** object collection, indexed by int */
class VectorObject : public DestrBase
{
public:
	SList data;
	unsigned int readonly : 1, owndata : 1;
	void clear();
	ExtValue *get(int i) { return (ExtValue*)data.get(i); }
	void set_or_insert(int i, const ExtValue& val, bool insert);

	static Param par;
	VectorObject(Pt3D& pt);
	VectorObject() :readonly(0), owndata(1) {}
	~VectorObject() { clear(); }
	static Param& getStaticParam() { return par; }
#define STATRICKCLASS VectorObject
	PARAMPROCDEF(p_clear) { if (readonly) return; clear(); }
	PARAMGETDEF(size) { arg1->setInt(data.size()); }
	PARAMGETDEF(avg);
	PARAMGETDEF(stdev);
	PARAMGETDEF(iterator);
	PARAMPROCDEF(p_remove);
	PARAMPROCDEF(p_get);
	PARAMPROCDEF(p_find);
	PARAMPROCDEF(p_set) { if (!readonly) set_or_insert(arg1[1].getInt(), arg1[0], false); }
	PARAMPROCDEF(p_add) { if (readonly) return; /*ExtValue tmp; get_toString(&tmp); printf("%s += %s",(const char*)tmp.getString(),(const char*)arg1[0].getString());*/ data += new ExtValue(arg1[0]); /*get_toString(&tmp); printf(" -> %s\n",(const char*)tmp.getString());*/ arg2->setInt(data.size() - 1); }
	PARAMPROCDEF(p_insert) { if (!readonly) set_or_insert(arg1[1].getInt(), arg1[0], true); }
	PARAMGETDEF(toString);
	PARAMPROCDEF(p_sort);
	PARAMPROCDEF(p_clone);
#undef STATRICKCLASS
	static void p_new(void*, ExtValue*args, ExtValue*ret)
	{
		ret->setObject(ExtObject(&par, new VectorObject));
	}
	SString serialize(SerializationFormat format) const;
	ExtObject makeObject() { return ExtObject(&par, this); }

	static VectorObject* fromObject(const ExtObject& o, bool warn = true);
};

/** object collection, indexed by name */
class DictionaryObject : public DestrBase
{
public:
	HashTable hash;
	HashEntryIterator it;
	int it_index;

	void clear();
	HashEntryIterator* getIndexIterator(int i);

	static Param par;
	DictionaryObject() :it(hash), it_index(-1) {}
	~DictionaryObject() { clear(); }
	static Param& getStaticParam() { return par; }
#define STATRICKCLASS DictionaryObject
	PARAMPROCDEF(p_clear) { clear(); }
	PARAMGETDEF(size) { arg1->setInt(hash.getSize()); }
	PARAMPROCDEF(p_remove);
	PARAMPROCDEF(p_get);
	PARAMPROCDEF(p_getKey);
	PARAMPROCDEF(p_hasKey);
	PARAMPROCDEF(p_set);
	PARAMPROCDEF(p_find);
	PARAMGETDEF(toString);
	PARAMPROCDEF(p_clone);
	PARAMPROCDEF(p_assign);
	PARAMGETDEF(iterator);
	PARAMGETDEF(keys);
#undef STATRICKCLASS
	ExtValue get(SString key);
	ExtValue get(int index);
	ExtValue set(SString key, ExtValue new_value);
	void copyFrom(DictionaryObject *other);
	SString serialize(SerializationFormat format) const;
	static void p_new(void*, ExtValue*args, ExtValue*ret)
	{
		ret->setObject(ExtObject(&par, new DictionaryObject));
	}
	static DictionaryObject* fromObject(const ExtObject& v, bool warn = true);
	ExtObject makeObject() { return ExtObject(&par, this); }
};

class VectorIterator : public DestrBase
{
public:
	VectorObject *vec;
	int pos;
	VectorIterator(VectorObject* v);
	~VectorIterator();
#define STATRICKCLASS VectorIterator
	PARAMGETDEF(next);
	PARAMGETDEF(value);
#undef STATRICKCLASS
	static ExtObject makeFrom(VectorObject *v);
};

class DictionaryIterator : public DestrBase
{
public:
	DictionaryObject *dic;
	HashEntryIterator it;
	bool initial, keys;
	DictionaryIterator(DictionaryObject* d, bool _keys);
	~DictionaryIterator();
#define STATRICKCLASS DictionaryIterator
	PARAMGETDEF(next);
	PARAMGETDEF(value);
	PARAMGETDEF(iterator);
#undef STATRICKCLASS
	static ExtObject makeFrom(DictionaryObject *v, bool _keys = false);
};

#endif
