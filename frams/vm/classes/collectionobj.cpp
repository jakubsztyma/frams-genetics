// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "collectionobj.h"
#include <common/nonstd_math.h> //sqrt in borland
#include <frams/util/validitychecks.h>
#include <common/nonstd_stl.h>
#include <frams/util/sstringutils.h>
#ifndef NO_VMACHINE
#include <frams/vm/vmachine.h>
#endif

#define FIELDSTRUCT VectorObject
ParamEntry vector_paramtab[] =
{
	{ "Vector", 1, 15, "Vector", "Vector is a 1-dimensional array indexed by an integer value (starting from 0). "
	"Multidimensional arrays can be simulated by putting other Vector objects into a Vector.\n"
	"Examples:\n"
	"\tvar v1=Vector.new();\n"
	"\tv1.add(123);\n"
	"\tv1.add(\"string\");\n"
	"A short way of doing the same (square brackets create a vector):\n"
	"\tvar v2=[123,\"string\"];\n"
	"Simulate a 2D array:\n"
	"\tvar v3=[[1,2,3],[4,5],[6]];\n"
	"You can iterate directly over values of a Vector using for(...in...) loops:\n"
	"\tfor(var element in v3) Simulator.print(element);"
	},
	{ "clear", 0, PARAM_NOSTATIC, "Clear data", "p()", PROCEDURE(p_clear), },
	{ "size", 0, PARAM_READONLY | PARAM_NOSTATIC, "Element count", "d", GETONLY(size), },
	{ "remove", 0, PARAM_NOSTATIC, "Remove at position", "p(d position)", PROCEDURE(p_remove), },
	{ "get", 0, PARAM_NOSTATIC, "Get value at position", "p x(d position)", PROCEDURE(p_get), "object[position] can be always used instead of object.get(position)" },
	{ "set", 0, PARAM_NOSTATIC, "Set value at position", "p(d position,x value)", PROCEDURE(p_set), "object[position]=value can be always used instead of object.set(position,value)" },
	{ "insert", 0, PARAM_NOSTATIC, "Insert value at position", "p(d position,x value)", PROCEDURE(p_insert), },
	{ "add", 0, PARAM_NOSTATIC, "Append at the end", "p(x value)", PROCEDURE(p_add), },
	{ "find", 0, PARAM_NOSTATIC, "Find", "p d(x value)", PROCEDURE(p_find), "returns the element index or -1 if not found" },
	{ "avg", 0, PARAM_READONLY | PARAM_NOSTATIC, "Average", "f", GETONLY(avg) },
	{ "stdev", 0, PARAM_READONLY | PARAM_NOSTATIC, "Standard deviation", "f", GETONLY(stdev), "=sqrt(sum((element[i]-avg)^2)/(size-1)) which is estimated population std.dev. from sample std.dev." },
	{ "toString", 0, PARAM_READONLY | PARAM_NOSTATIC, "Textual form", "s", GETONLY(toString), },
	{ "new", 0, 0, "Create new Vector", "p oVector()", STATICPROCEDURE(p_new), },
	{ "sort", 0, PARAM_NOSTATIC, "Sort elements (in place)", "p(oFunctionReference comparator)", PROCEDURE(p_sort), "comparator can be null, giving the \"natural\" sorting order (depending on element type), otherwise it must be a function reference obtained from the 'function' operator.\n\nExample:\nfunction compareLastDigit(a,b) {return (a%10)<(b%10);}\nvar v=[16,23,35,42,54,61];\nv.sort(function compareLastDigit);" },
	{ "iterator", 0, PARAM_NOSTATIC | PARAM_READONLY, "Iterator", "o", GETONLY(iterator), },
	{ "clone", 0, PARAM_NOSTATIC, "Create a clone", "p oVector()", PROCEDURE(p_clone), "The resulting clone is a shallow copy (contains the same object references as the original). A deep copy can be obtained through serialization: String.deserialize(String.serialize(object));" },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

#define FIELDSTRUCT DictionaryObject
ParamEntry dictionary_paramtab[] =
{
	{ "Dictionary", 1, 14, "Dictionary", "Dictionary associates stored values with string keys "
	"(\"key\" is the first argument in get/set/remove functions). Integer key can be "
	"used to enumerate all elements (note that while iterating, the elements are returned in no particular order).\n"
	"Examples:\n"
	"\tvar d;\n"
	"\td=Dictionary.new();\n"
	"\td.set(\"name\",\"John\");\n"
	"\td.set(\"age\",44);\n"
	"Another way of doing the same:\n"
	"\td={};\n"
	"\td[\"name\"]=\"John\";\n"
	"\td[\"age\"]=44;\n"
	"And the most concise way:\n"
	"\td={ \"name\":\"John\", \"age\":44 };\n"
	"Iterating:\n"
	"\tfor(var v in d) Simulator.print(v); //values\n"
	"\tfor(var k in d.keys) Simulator.print(k+\" is \"+d[k]); //keys\n"
	"\tfor(var i=0;i<d.size;i++) Simulator.print(d.getKey(i)+\" is \"+d.get(i)); //by index",
	},
	{ "clear", 0, PARAM_NOSTATIC, "Clear data", "p()", PROCEDURE(p_clear), },
	{ "size", 0, PARAM_NOSTATIC | PARAM_READONLY, "Element count", "d", GETONLY(size), },
	{ "remove", 0, PARAM_NOSTATIC, "Remove", "p(x key)", PROCEDURE(p_remove), "Removes the named or indexed element (depending on the argument type: string or int)." },
	{ "get", 0, PARAM_NOSTATIC, "Get element", "p x(x key)", PROCEDURE(p_get), "Retrieves the named or indexed element (depending on the argument type: string or int). Accessing nonexistent keys is an error (use hasKey() if necessary).\nobject.get(key) can be shortened to object[key]." },
	{ "getKey", 0, PARAM_NOSTATIC, "Get a key", "p s(d index)", PROCEDURE(p_getKey), "Returns the key of the indexed element (0 <= index < size)." },
	{ "hasKey", 0, PARAM_NOSTATIC, "Check if key exists", "p d(s key)", PROCEDURE(p_hasKey), "Returns 1 (interpreted as true) if dictionary contains the supplied key, or 0 (false) otherwise.\nExample:\n   if (obj.hasKey(\"a\"))\n      x = obj->a;" },
	{ "set", 0, PARAM_NOSTATIC, "Set element", "p x(x key,x value)", PROCEDURE(p_set), "Set element value for the specified key or index (depending on the argument type: string or int).\n"
	"Returns the value previously associated with the given key (or index).\n"
	"object.set(key,value) can be shortened to object[key]=value. Literal string keys can use even shorter notation: object->key=value instead of object.set(\"key\",value)\n"
	"Note the difference in the returned value:\n"
	"  var old_value=object.set(\"key\",new_value); //'old_value' gets the value previously associated with \"key\"\n"
	"  var x=object[\"key\"]=new_value; //'x' becomes 'new_value', consistently with the semantics of the assignment operator. The value previously associated with \"key\" is lost." },
	{ "find", 0, PARAM_NOSTATIC, "Find", "p x(x value)", PROCEDURE(p_find), "Returns the element key or null if not found." },
	{ "new", 0, 0, "Create a Dictionary", "p oDictionary()", STATICPROCEDURE(p_new), "Empty directory can be also created using the {} expression." },
	{ "toString", 0, PARAM_READONLY | PARAM_NOSTATIC, "Textual form", "s", GETONLY(toString), },
	{ "clone", 0, PARAM_NOSTATIC, "Create a clone", "p oDictionary()", PROCEDURE(p_clone), "The resulting clone is a shallow copy (contains the same object references as the original). A deep copy can be obtained through serialization: String.deserialize(String.serialize(object));" },
	{ "assign", 0, PARAM_NOSTATIC, "Assign from another object", "p(x)", PROCEDURE(p_assign), "Replaces current dictionary with dictionary contents from another object." },
	{ "iterator", 0, PARAM_NOSTATIC | PARAM_READONLY, "Iterator", "o", GETONLY(iterator), },
	{ "keys", 0, PARAM_NOSTATIC | PARAM_READONLY, "Keys", "o", GETONLY(keys), "Iterate over this object to get all keys: for(k in dict.keys) ..." },

	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

Param VectorObject::par(vector_paramtab);
Param DictionaryObject::par(dictionary_paramtab);

/////////////////////////////////////////

VectorObject::VectorObject(Pt3D &pt)
	:readonly(0), owndata(1)
{
	set_or_insert(0, ExtValue(pt.x), false);
	set_or_insert(1, ExtValue(pt.y), false);
	set_or_insert(2, ExtValue(pt.z), false);
}

void VectorObject::clear()
{
	if (owndata)
		for (int i = data.size() - 1; i >= 0; i--)
		{
			ExtValue *v = (ExtValue*)data.get(i);
			if (v) delete v;
		}
	data.clear();
}

void VectorObject::p_remove(PARAMPROCARGS)
{
	if (readonly) return;
	int i = args->getInt();
	if (!listIndexCheck(&data, i, "VectorObject", "remove")) return;
	ExtValue *v = (ExtValue*)data.get(i);
	if (v) delete v;
	data -= i;
}

void VectorObject::set_or_insert(int i, const ExtValue& val, bool insert)
{
	if (i < 0) return;
	int oldsize = data.size();
	if (i > oldsize)
	{
		data.setSize(i);
		while (i > oldsize)
			data.set(oldsize++, 0);
	}
	if (insert)
		data.insert(i, new ExtValue(val));
	else
	{
		ExtValue *v = (ExtValue*)data.get(i);
		if (v) delete v;
		data.set(i, new ExtValue(val));
	}
}

void VectorObject::p_get(PARAMPROCARGS)
{
	int i = args->getInt();
	if (listIndexCheck(&data, i, "VectorObject", "get"))
	{
		ExtValue *v = get(i);
		if (v)
		{
			*ret = *v;
			return;
		}
	}
	*ret = ExtValue();
}

void VectorObject::get_avg(ExtValue* ret)
{
	if (!data.size()) { ret->setEmpty(); return; }
	double s = 0.0;
	for (int i = data.size() - 1; i >= 0; i--)
		s += ((ExtValue*)data.get(i))->getDouble();
	s /= data.size();
	ret->setDouble(s);
}

SString VectorObject::serialize(SerializationFormat format) const
{
	SString out = "[";
	{
		for (int i = 0; i < data.size(); i++)
		{
			ExtValue* v = (ExtValue*)data.get(i);
			if (i) out += ",";
			if (v)
				out += v->serialize(format);
			else
				out += "null";
		}
	}
	out += "]";
	//sprintf(out.directAppend(20),"<Vector@%p>",this);out.endAppend();
	return out;
}

static THREAD_LOCAL_DEF(SList, VectorObject_tostring_trace);

void VectorObject::get_toString(ExtValue* ret)
{
	SString out = "[";
	//static SListTempl<VectorObject*> trace;
	if (tlsGetRef(VectorObject_tostring_trace).find(this) >= 0)
		out += "...";
	else
	{
		tlsGetRef(VectorObject_tostring_trace) += this;
		for (int i = 0; i < data.size(); i++)
		{
			ExtValue* v = (ExtValue*)data.get(i);
			if (i) out += ",";
			if (v)
				out += v->getString();
			else
				out += "null";
		}
		tlsGetRef(VectorObject_tostring_trace) -= this;
	}
	out += "]";
	ret->setString(out);
}

void VectorObject::get_stdev(ExtValue* ret)
{
	if (!data.size()) { ret->setEmpty(); return; }
	get_avg(ret);
	double a = ret->getDouble();
	double s = 0.0;
	for (int i = data.size() - 1; i >= 0; i--)
	{
		double d = a - ((ExtValue*)data.get(i))->getDouble();
		s += d * d;
	}
	ret->setDouble(sqrt(s / max(1, data.size() - 1)));
}

void VectorObject::p_find(PARAMPROCARGS)
{
	short i;
	for (i = 0; i < data.size(); i++)
	{
		if ((*args) == (*get(i)))
		{
			ret->setInt(i); return;
		}
	}
	ret->setInt(-1);
}

void VectorObject::p_clone(PARAMPROCARGS)
{
	VectorObject *c = new VectorObject;
	c->data.setSize(data.size());
	for (int i = 0; i < data.size(); i++)
	{
		ExtValue *v = (ExtValue*)get(i);
		if (v)
			c->data.set(i, new ExtValue(*v));
	}
	ret->setObject(ExtObject(&par, c));
}

class VEComparator
{
public:
	bool operator()(const ExtValue *a, const ExtValue *b) { return a->compare(*b) == ExtValue::ResultLower; }
};

#ifndef NO_VMACHINE
class VMVEComparator
{
public:
	VMachine::JumpTargetObject *jto;
	VMachine *vm;
	VMVEComparator(VMachine::JumpTargetObject *_jto) :jto(_jto), vm(jto->vm) {}
	bool operator()(const ExtValue *a, const ExtValue *b);
};

bool VMVEComparator::operator()(const ExtValue *a, const ExtValue *b)
{
	if (!VMCode::prepareDynamicJumpTarget(jto->pc, jto->code))
		return false;

	vm->push(*a);
	vm->push(*b);
	vm->pushNewCallState();
	vm->jumpDynamicJumpTarget(jto->pc);
	vm->run();
	vm->popCallState();
	bool ret;
	ExtValue& retval = vm->getValue();
	if (retval.type == TInvalid)
	{
		ret = false;
		logPrintf("VectorElementComparator", "", LOG_ERROR, "Comparison function returned no value");
	}
	else
		ret = (retval.getInt() != 0);
	vm->drop(2);
	return ret;
}
#endif

void VectorObject::p_sort(PARAMPROCARGS)
{
#ifndef NO_VMACHINE
	VMachine::JumpTargetObject *jto = VMachine::JumpTargetObject::fromObject(args->getObject(), false);
	if (jto)
	{
		VMVEComparator cmp(jto);
		ExtValue **first = (ExtValue**)&data.getref(0);
		std::sort(first, first + data.size(), cmp);
	}
	else
#endif
	{
		VEComparator cmp;
		ExtValue **first = (ExtValue**)&data.getref(0);
		std::sort(first, first + data.size(), cmp);
	}
	ret->setEmpty();
}

void VectorObject::get_iterator(ExtValue* ret)
{
	ret->setObject(VectorIterator::makeFrom(this));
}

VectorObject* VectorObject::fromObject(const ExtObject& o, bool warn)
{
	return (VectorObject*)o.getTarget(par.getName(), true, warn);
}

/////////////////////////////

void DictionaryObject::clear()
{
	for (HashEntryIterator it(hash); it.isValid();)
	{
		ExtValue *v = (ExtValue*)hash.remove(it);
		if (v) delete v;
	}
	hash.clear();
	hash.init();
}

void DictionaryObject::p_find(PARAMPROCARGS)
{
	for (HashEntryIterator it(hash); it.isValid(); it++)
	{
		if (((ExtValue*)it->value) == NULL)
		{
			if (args->getType() != TUnknown) continue;
			ret->setString(it->key);
			return;
		}
		if ((*args) == (*((ExtValue*)it->value)))
		{
			ret->setString(it->key);
			return;
		}
	}
	ret->setEmpty();
}

HashEntryIterator* DictionaryObject::getIndexIterator(int i)
{
	if (i < 0) return 0;
	if (i >= hash.getSize()) return 0;

	if ((!it.isValid()) || (it_index > i))
	{
		it = HashEntryIterator(hash);
		it_index = 0;
	}
	while (it.isValid())
	{
		if (it_index == i)
			return &it;
		it_index++;
		it++;
	}
	return 0;
}

void DictionaryObject::p_remove(PARAMPROCARGS)
{
	if ((args->type == TInt) || (args->type == TDouble))
	{
		HashEntryIterator* iter = getIndexIterator(args->getInt());
		if (iter)
		{
			ExtValue *oldval = (ExtValue*)hash.remove(*iter);
			if (oldval) { *ret = *oldval; delete oldval; }
			else *ret = ExtValue();
		}
	}
	else
	{
		ExtValue *oldval = (ExtValue*)hash.remove(args[0].getString());
		if (oldval) { *ret = *oldval; delete oldval; }
		else *ret = ExtValue();
	}
}

ExtValue DictionaryObject::get(SString key)
{
	int found = 0;
	ExtValue *val = (ExtValue*)hash.get(key, &found);
	if (found == 0)
	{
		logPrintf("Dictionary", "get", LOG_ERROR, "Key '%s' not found", key.c_str());
		return ExtValue::invalid();
	}
	else
	{
		if (val)
			return *val;
		return ExtValue::empty();
	}
}

ExtValue DictionaryObject::get(int index)
{
	HashEntryIterator* iter = getIndexIterator(index);
	if (iter && (*iter)->value)
		return *((ExtValue*)(*iter)->value);
	return ExtValue::empty();
}

void DictionaryObject::p_get(PARAMPROCARGS)
{
	if ((args->type == TInt) || (args->type == TDouble))
		*ret = get(args->getInt());
	else
		*ret = get(args[0].getString());
}

void DictionaryObject::p_getKey(PARAMPROCARGS)
{
	HashEntryIterator* iter = getIndexIterator(args->getInt());
	if (iter)
	{
		*ret = (*iter)->key;
		return;
	}
	*ret = ExtValue();
}

void DictionaryObject::p_hasKey(PARAMPROCARGS)
{
	int found = 0;
	hash.get(args->getString(), &found);
	ret->setInt(found);
}

ExtValue DictionaryObject::set(SString key, ExtValue new_value)
{
	ExtValue ret;
	ExtValue *new_ext = (new_value.getType() == TUnknown) ? NULL : new ExtValue(new_value);
	ExtValue *old_ext = (ExtValue*)hash.put(key, new_ext);
	if (old_ext) { ret = *old_ext; delete old_ext; }
	return ret;
}

void DictionaryObject::p_set(PARAMPROCARGS)
{
	*ret = set(args[1].getString(), args[0]);
}

SString DictionaryObject::serialize(SerializationFormat format) const
{
	SString out = "{";
	{
		for (HashEntryIterator it(hash); it.isValid();)
		{
			out += "\"";
			SString q = it->key; sstringQuote(q);
			out += q;
			out += "\":";
			if (it->value != NULL)
				out += ((ExtValue*)it->value)->serialize(format);
			else
				out += "null";
			it++;
			if (it.isValid()) out += ",";
		}
	}
	out += "}";
	return out;
}

void DictionaryObject::get_toString(ExtValue* ret)
{
	SString out = "{";
	//static SListTempl<DictionaryObject*> trace;
	if (tlsGetRef(VectorObject_tostring_trace).find(this) >= 0)
		out += "...";
	else
	{
		tlsGetRef(VectorObject_tostring_trace) += this;
		for (HashEntryIterator it(hash); it.isValid();)
		{
			out += it->key;
			out += ":";
			if (it->value != NULL)
				out += ((ExtValue*)it->value)->getString();
			else
				out += "null";
			it++;
			if (it.isValid()) out += ",";
		}
		tlsGetRef(VectorObject_tostring_trace) -= this;
	}
	out += "}";
	ret->setString(out);
}

void DictionaryObject::copyFrom(DictionaryObject *other)
{
	for (HashEntryIterator it(other->hash); it.isValid(); it++)
	{
		ExtValue *v = (ExtValue*)it->value;
		hash.put(it->key, v ? new ExtValue(*v) : NULL);
	}
}

void DictionaryObject::p_clone(PARAMPROCARGS)
{
	DictionaryObject *c = new DictionaryObject;
	c->copyFrom(this);
	ret->setObject(ExtObject(&par, c));
}

void DictionaryObject::p_assign(PARAMPROCARGS)
{
	clear();
	DictionaryObject *other = DictionaryObject::fromObject(args[0].getObject(), false);
	if (other)
		copyFrom(other);
	ret->setEmpty();
}

DictionaryObject* DictionaryObject::fromObject(const ExtObject& o, bool warn)
{
	return (DictionaryObject*)o.getTarget(par.getName(), true, warn);
}

void DictionaryObject::get_iterator(ExtValue* ret)
{
	ret->setObject(DictionaryIterator::makeFrom(this));
}

void DictionaryObject::get_keys(ExtValue* ret)
{
	ret->setObject(DictionaryIterator::makeFrom(this));
}

////////////////

VectorIterator::VectorIterator(VectorObject* v)
{
	vec = v;
	vec->incref();
	pos = -1;
}

#define FIELDSTRUCT VectorIterator
ParamEntry vectoriterator_paramtab[] =
{
	{ "VectorIterator", 1, 2, "VectorIterator", "VectorIterator" },
	{ "next", 0, PARAM_READONLY | PARAM_NOSTATIC, "next", "d 0 1", GETONLY(next), },
	{ "value", 0, PARAM_READONLY | PARAM_NOSTATIC, "value", "x", GETONLY(value), },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

ExtObject VectorIterator::makeFrom(VectorObject *v)
{
	static Param par(vectoriterator_paramtab);
	return ExtObject(&par, new VectorIterator(v));
}

VectorIterator::~VectorIterator()
{
	vec->decref();
}

void VectorIterator::get_next(ExtValue* ret)
{
	pos++;
	ret->setInt((pos < vec->data.size()) ? 1 : 0);
}

void VectorIterator::get_value(ExtValue* ret)
{
	ExtValue *v = (ExtValue*)(((pos >= 0) && (pos < vec->data.size())) ? vec->data(pos) : NULL);
	if (v)
		*ret = *v;
	else
		ret->setEmpty();
}

/////////////////

#define FIELDSTRUCT DictionaryIterator
ParamEntry dictionaryiterator_paramtab[] =
{
	{ "DictionaryIterator", 1, 3, "DictionaryIterator", "DictionaryIterator" },
	{ "next", 0, PARAM_READONLY | PARAM_NOSTATIC, "next", "d 0 1", GETONLY(next), },
	{ "value", 0, PARAM_READONLY | PARAM_NOSTATIC, "value", "x", GETONLY(value), },
	{ "iterator", 0, PARAM_READONLY | PARAM_NOSTATIC, "keys iterator", "x", GETONLY(iterator), },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

DictionaryIterator::DictionaryIterator(DictionaryObject* d, bool _keys)
	:it(d->hash)
{
	dic = d;
	dic->incref();
	initial = true;
	keys = _keys;
}

ExtObject DictionaryIterator::makeFrom(DictionaryObject *d, bool _keys)
{
	static Param par(dictionaryiterator_paramtab);
	return ExtObject(&par, new DictionaryIterator(d, _keys));
}

DictionaryIterator::~DictionaryIterator()
{
	dic->decref();
}

void DictionaryIterator::get_next(ExtValue* ret)
{
	if (initial)
		initial = false;
	else
		it++;
	ret->setInt(it.isValid());
}

void DictionaryIterator::get_value(ExtValue* ret)
{
	if ((!initial) && it.isValid())
	{
		if (keys)
		{
			ret->setString(it->key);
		}
		else
		{
			ExtValue *v = (ExtValue*)it->value;
			if (v == NULL)
				ret->setEmpty();
			else
				*ret = *v;
		}
	}
	else
		ret->setEmpty();
}

void DictionaryIterator::get_iterator(ExtValue* ret)
{
	ret->setObject(makeFrom(dic, true));
}

//////////////

// not actually needed for deserialization (vector and dict are special cases) but findDeserializableClass can be also used in other contexts
REGISTER_DESERIALIZABLE(VectorObject)
REGISTER_DESERIALIZABLE(DictionaryObject)
