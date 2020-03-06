// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _EXTVALUE_H_
#define _EXTVALUE_H_

#include "sstring.h"
#include <frams/param/param.h>
#include <common/nonstd_stl.h>
#include <common/threads.h>

#define EXTVALUEUNION
template <int A, int B> struct CompileTimeMax { enum { val = A > B ? A : B }; };
#define EXTVALUEUNIONSIZE CompileTimeMax<sizeof(ExtObject),sizeof(SString)>::val

//#define DEBUG_EXTOBJECT(txt) printf("%p ExtObj::" txt "\n",this)
#define DEBUG_EXTOBJECT(txt) 

enum ExtPType
{
	TUnknown = 0, TInt, TDouble, TString, TObj, TInvalid
};

/**
   destructable object
   */
class DestrBase
{
public:
	int refcount;
	DestrBase() :refcount(0) {}
	void incref() { refcount++; }
	void decref() { refcount--; if (refcount == 0) delete this; }
	virtual ~DestrBase() {}
};

enum SerializationFormat { NativeSerialization, JSONSerialization };

/**
   object reference.
   */
class ExtObject
{
	int subtype;                    //< 0/1=Generic/DPC Object,  0/2=Standalone/Shared Param
	void incref() const;
	void decref() const;
public:
	union {
		void* object;           //< generic object, will use param
		DestrBase *dbobject;	//< object with refcounting, will be deleted if refcount goes to 0
	};
	union {
		Param* param;           //< if object!=0
		ParamInterface *paraminterface; //< if object==0
	};

	void copyFrom(const ExtObject& src) { subtype = src.subtype; object = src.object; param = src.param; }

	void* operator new(size_t s, void* mem) { return mem; }
#ifdef _MSC_VER
	void operator delete(void* mem, void* t) {}
#endif
	void* operator new(size_t s) { return malloc(sizeof(ExtObject)); }
	void operator delete(void* mem) { free(mem); }
	///@param tmp_param can be used for temporary storage, the result ParamInterface* is only valid for as long as tmp_param is valid
	ParamInterface *getParamInterface(Param &tmp_param) const { if (subtype & 2) { tmp_param.setParamTab(param->getParamTab()); tmp_param.select(object); return &tmp_param; } return paraminterface; }
	const char* interfaceName() const { if (isEmpty()) return "Empty"; return (subtype & 2) ? param->getName() : paraminterface->getName(); }
	bool matchesInterfaceName(ParamInterface* pi) const { return !strcmp(interfaceName(), pi->getName()); }
	void* getTarget() const { return (subtype & 1) ? dbobject : object; }
	void* getTarget(const char* classname, bool through_barrier = true, bool warn = true) const;
	bool callDelegate(const char* delegate, ExtValue *args, ExtValue *ret);
	void setEmpty() { decref(); subtype = 0; param = NULL; object = NULL; }
	int isEmpty() const { return !param; }
	static const ExtObject& empty() { static const ExtObject e((ParamInterface*)NULL); return e; }
	ExtObject(const ExtObject& src) { DEBUG_EXTOBJECT("(const&)"); src.incref(); copyFrom(src); }
	void operator=(const ExtObject& src) { src.incref(); decref(); copyFrom(src); }
	bool makeUnique();//< @return false if nothing has changed

	bool operator==(const ExtObject& src) const;

	SString toString() const;
	SString serialize_inner(SerializationFormat format) const;
	SString serialize(SerializationFormat format) const;

	ExtObject(Param *p, void *o) :subtype(2), object(o), param(p) { DEBUG_EXTOBJECT("(Param,void)"); }
	ExtObject(ParamInterface *p = 0) :subtype(0), object(0), paraminterface(p) { DEBUG_EXTOBJECT("(ParamInterface)"); }
	ExtObject(Param *p, DestrBase *o) :subtype(1 + 2), dbobject(o), param(p) { DEBUG_EXTOBJECT("(Param,DestrBase)"); incref(); }
	ExtObject(ParamInterface *p, DestrBase *o) :subtype(1), dbobject(o), paraminterface(p) { DEBUG_EXTOBJECT("(ParamInterface,DestrBase)"); incref(); }

	~ExtObject() { DEBUG_EXTOBJECT("~"); decref(); }

	class Serialization
	{
		std::vector<ExtObject> refs;
		int level;
	public:
		Serialization() :level(0) {}
		void begin();
		void end();
		int add(const ExtObject& o);
		void replace(const ExtObject& o, const ExtObject& other);
		void remove(const ExtObject& o);
		const ExtObject* get(int ref);
	};

};

extern THREAD_LOCAL_DECL(ExtObject::Serialization, ExtObject_serialization);

class ExtValue
{
public:
	ExtPType type;
#ifdef EXTVALUEUNION
	intptr_t data[(EXTVALUEUNIONSIZE + sizeof(intptr_t) - 1) / sizeof(intptr_t)];
	paInt& idata() const { return (paInt&)data[0]; };
	double& ddata() const { return *(double*)data; };
	ExtObject& odata() const { return *(ExtObject*)data; };
	SString& sdata() const { return *(SString*)data; };
#else
	union {
		paInt i;
		double d;
		SString *s;
		ExtObject *o;
	};
	paInt& idata() const { return (paInt&)i; };
	double& ddata() const { return (double&)d; };
	ExtObject& odata() const { return *o; };
	SString& sdata() const { return *s; };
#endif

	void* operator new(size_t s, void* mem) { return mem; }
	void* operator new(size_t s) { return ::operator new(s); }

	ExtValue() :type(TUnknown) {}
	~ExtValue() { setEmpty(); }
	ExtValue(paInt v) { seti(v); }
	ExtValue(double v) { setd(v); }
	ExtValue(const SString &v) { sets(v); }
	ExtValue(const ExtObject &srco) { seto(srco); }
	static ExtValue invalid() { ExtValue v; v.setInvalid(); return v; }
	static const ExtValue& empty() { static const ExtValue v; return v; }
	static const ExtValue& zero() { static const ExtValue v(0); return v; }

	enum CompareResult
	{
		ResultLower = -1, ResultEqual = 0, ResultHigher = 1,
		ResultEqualUnordered,
		ResultUnequal_RelaxedEqual,
		ResultUnequal_RelaxedUnequal,
		ResultMismatch_RelaxedUnequal,
		ResultMismatch
	};
	// performs all script value comparisons.
	// relaxed comparison (internal use only, not available in scripts) works like regular == with additional null~=0, notnull!~0
	// and is used for pseudo-boolean conversion allowing for expressions like "if (null) ..."
	CompareResult compare(const ExtValue& src) const;

	enum CmpOperator { CmpFIRST, CmpEQ = CmpFIRST, CmpNE, CmpGE, CmpLE, CmpGT, CmpLT,/*relaxed (not)equal*/CmpREQ, CmpRNE };
	static const char* cmp_op_names[];
	struct CmpContext { const ExtValue *v1, *v2; };
	// interpret compare() result, optional context controls error messages
	// @return 0=false, 1=true, -1=undefined (null in script)
	static int interpretCompare(CmpOperator op, CompareResult result, CmpContext *context = NULL);

	void divInt(paInt a);
	void divDouble(double a);
	void modInt(paInt a);
	void modDouble(double a);

	int operator==(const ExtValue& src) const;
	void operator+=(const ExtValue& src);
	void operator-=(const ExtValue& src);
	void operator*=(const ExtValue& src);
	void operator/=(const ExtValue& src);
	void operator%=(const ExtValue& src);
	void operator=(const ExtValue& src)
	{
		setr(src);
	}
	ExtValue(const ExtValue& src)
		:type(TUnknown) {
		set(src);
	}
	void setEmpty();
	void setInvalid() { setEmpty(); type = TInvalid; }
	void setError(const SString& msg);
	bool makeUnique() { return (type == TObj) && odata().makeUnique(); } //< @return false if nothing has changed
	ExtPType getType() const { return type; }
	void *getObjectTarget(const char* classname, bool warn = true) const;
	void setInt(paInt v) { if (type != TInt) setri(v); else idata() = v; }
	void setDouble(double v) { if (type != TDouble) setrd(v); else ddata() = v; }
	void setString(const SString &v) { if (type != TString) setrs(v); else sdata() = v; }
	void setObject(const ExtObject &src) { if (type != TObj) setro(src); else odata() = src; }
	static bool parseInt(const char* s, paInt &result, bool strict, bool error);
	static bool parseDouble(const char* s, double &result, bool error);
	static paInt getInt(const char* s, bool strict = false);//< @param strict=true will fail on floating point
	static double getDouble(const char* s);
	paInt getInt() const;
	double getDouble() const;
	SString getString() const;
	const SString* getStringPtr() const;//< @return pointer to the internal sstring object or NULL if the current type is not string
	SString serialize(SerializationFormat format) const;
	ExtObject getObject() const;
	bool isNull() const { return (type == TUnknown) || ((type == TObj) && odata().isEmpty()); }
	SString typeDescription() const;//< @return human readable type name (used in error messages)
	SString typeAndValue() const;//< @return type and value (used in error messages)
	const char* parseNumber(const char* in, ExtPType strict_type = TUnknown);
	const char* deserialize(const char* in);//< @return first character after the succesfully parsed string or NULL if failed
	const char* deserialize_inner(const char* in);
	static ParamInterface *findDeserializableClass(const char* name);
	static PtrListTempl<ParamInterface*> &getDeserializableClasses();
	template<typename T> class AddDeserializable
	{
	public:
		AddDeserializable() { ExtValue::getDeserializableClasses() += &T::getStaticParam(); }
	};

	static SString formatTime(char fmt, double value);
	static SString format(const SString& fmt, const ExtValue **values, int count);

	ExtValue getExtType();

private: // setrx - release and set, setx - assume released
	void setr(const ExtValue& src) { setEmpty(); set(src); }
	void set(const ExtValue& src);
	void setri(paInt v) { setEmpty(); seti(v); }
	void setrd(double v) { setEmpty(); setd(v); }
	void seti(paInt v) { type = TInt; idata() = v; }
	void setd(double v) { type = TDouble; ddata() = v; }
#ifdef EXTVALUEUNION
	void setrs(const SString &v) { setEmpty(); sets(v); }
	void setro(const ExtObject &src) { setEmpty(); seto(src); }
	void sets(const SString &v) { type = TString; new(data)SString(v); }
	void seto(const ExtObject &src) { type = TObj; new(data)ExtObject(src); }
#else
	void setrs(const SString &v) { setEmpty(); sets(v); }
	void setro(const ExtObject &src) { setEmpty(); seto(src); }
	void sets(const SString &v) { type = TString; s = new SString(v); }
	void seto(const ExtObject &src) { type = TObj; o = new ExtObject(src); }
#endif

};

#define REGISTER_DESERIALIZABLE(name) ExtValue::AddDeserializable<name> deserializable_autoinit_ ## name;

class ErrorObject : public DestrBase
{
public:
	SString message;
	static Param& getParam();
	static Param& getStaticParam() { return getParam(); }
	static ExtObject makeDynamicObject(ErrorObject* e);
	static const SString TO_STRING_PREFIX;
#define STATRICKCLASS ErrorObject
	PARAMGETDEF(toString);
	PARAMPROCDEF(p_newfromstring);
#undef STATRICKCLASS
};

#endif
