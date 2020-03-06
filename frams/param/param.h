// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PARAM_H_
#define _PARAM_H_

#include <stdio.h>
#include <stdint.h>
#include <frams/util/sstring.h>
#include <frams/util/sstringutils.h>
#include <frams/util/list.h>
#include <frams/util/statrick.h>
#include <common/virtfile/virtfile.h>
#include <common/log.h>

class ExtValue;
class ExtObject;

// ParamInterface flags:
#define PARAM_READONLY	     1  //< Param is not able to change this member
#define PARAM_DONTSAVE	     2  //< Param will not save this member
#define PARAM_SETLEVEL(x) (((x)&3)<<2)  //< internal use
#define PARAM_LEVEL(x)    (((x)>>2)&3)  //< internal use
#define PARAM_USERREADONLY  16  //< GUI should not change this member (even if Param can)
#define PARAM_USERHIDDEN    32  //< GUI should not display this member
#define MUTPARAM_ALLOCENTRY 64  //< for mutableparam (private!)
#define MUTPARAM_ALLOCDATA 128  //< for mutableparam (private!)
#define PARAM_NOSTATIC  256     //< (FramScript) don't access this member in a static object (ClassName.field)
#define PARAM_CONST     512     //< (FramScript) constant value
#define PARAM_CANOMITNAME 1024  //< affects Param::saveSingleLine()/loadSingleLine() - for example one-liners in f0 genetic encoding
#define PARAM_DONTLOAD	  2048  //< Param::load() skips this field
#define PARAM_NOISOLATION 4096  //< don't use proxy object in master/slave interactions
#define PARAM_DEPRECATED  8192  //< this member is deprecated
#define PARAM_LINECOMMENT 16384 //< Param::load() adds "@line ..." comment when loading multiline (internal use)
#define PARAM_OBJECTSET 32768   //< setting this field is handled by the object's assign(...) function and cannot change the object reference

typedef int32_t paInt;
#define PA_INT_SCANF "%d"
#define PA_INT_SCANF_X "%x"

// the result of param::set() is a combination of bits:

// read-only: cannot modify
#define PSET_RONLY	1

// value has been modified
#define PSET_CHANGED	2

//value has been adjusted because it tried to exceed min or max
#define PSET_HITMIN	4
#define PSET_HITMAX	8

#define PSET_NOPROPERTY	16

#define PSET_PARSEFAILED	32

// useful combination: need to get and display the value so that a user knows that the value they tried to set has been rejected or changed
#define PSET_WARN (PSET_RONLY | PSET_HITMIN | PSET_HITMAX | PSET_PARSEFAILED)


struct ParamEntry;

/** Property get/set interface - runtime access to named properties */
class ParamInterface
{
public:
	virtual ~ParamInterface() {}

	virtual int getGroupCount() = 0; ///< @return the number of property groups
	virtual int getPropCount() = 0; ///< @return the number of properties

	virtual const char* getName() = 0;
	virtual const char* getDescription() { return 0; }
	virtual ParamEntry *getParamTab() const { return NULL; }

	int findId(const char *n);	///< find id number for internal name
	int findIdn(const char *naz, int n);

	virtual const char *id(int i) = 0;	///< get internal name
	virtual const char *name(int i) = 0;	///< get the human-readable name

	/** get type description.
		first character defines basic datatype:
		- d = integer
		- f = floating point
		- s = string
		- o = ExtObject
		- x = ExtValue (universal datatype)
		*/
	virtual const char *type(int i) = 0;

	virtual const char *help(int i) = 0;	///< get long description (tooltip)

	virtual int flags(int i) = 0;		///< get flags

	virtual int group(int i) = 0;		///< get group id for a property
	virtual const char *grname(int gi) = 0;	///< get group name
	virtual int grmember(int gi, int n) = 0;	///< get property id for n'th member of group "gi"

	virtual void call(int i, ExtValue* args, ExtValue *ret) = 0;

	void get(int, ExtValue &retval);	///< most universal get, can be used for every datatype

	virtual SString getString(int) = 0;	///< get string value, you can only use this for "s" type property
	virtual paInt getInt(int) = 0;	///< get long value, you can only use this for "d" type property
	virtual double getDouble(int) = 0;	///< get double value, you can only use this for "f" type property
	virtual ExtObject getObject(int) = 0;	///< get object reference, you can only use this for "o" type property
	virtual ExtValue getExtValue(int) = 0;	///< get extvalue object, you can only use this for "x" type property

	SString get(int);		///< old style get, can convert long or double to string
	SString getText(int);		///< like getString, returns enumeration label for subtype "d 0 n ~enum1~enum2...

	SString getStringById(const char*prop);  ///< get string value, you can only use this for "s" type property
	paInt getIntById(const char* prop);    ///< get long value, you can only use this for "d" type property
	double getDoubleById(const char* prop);///< get double value, you can only use this for "f" type property
	ExtObject getObjectById(const char* prop);///< get object reference, you can only use this for "o" type property
	ExtValue getExtValueById(const char* prop);///< get extvalue object, you can only use this for "x" type property
	ExtValue getById(const char* prop);

	int setIntFromString(int i, const char* str, bool strict = false);
	int setDoubleFromString(int i, const char* str);
	virtual int setInt(int, paInt) = 0;		///< set long value, you can only use this for "d" type prop
	virtual int setDouble(int, double) = 0;	///< set double value, you can only use this for "f" type prop
	virtual int setString(int, const SString &) = 0; 	///< set string value, you can only use this for "s" type prop
	virtual int setObject(int, const ExtObject &) = 0; 	///< set object reference, you can only use this for "o" type prop
	virtual int setExtValue(int, const ExtValue &) = 0;	///< 4 in 1

	int set(int, const ExtValue &);///< most universal set, can be used for every datatype

	int setFromString(int, const char*, bool strict = false);		///< oldstyle set, can convert string to long or double

	int setIntById(const char* prop, paInt);///< set long value, you can only use this for "d" type prop
	int setDoubleById(const char* prop, double);///< set double value, you can only use this for "f" type prop
	int setStringById(const char* prop, const SString &);///< set string value, you can only use this for "s" type prop
	int setObjectById(const char* prop, const ExtObject &);///< set object reference, you can only use this for "o" type prop
	int setExtValueById(const char* prop, const ExtValue &); ///< for ExtValue types only
	int setById(const char* prop, const ExtValue &);///< can be used for all property types

	/** get valid minimum, maximum and default value for property 'prop'
		@return 0 if min/max/def information is not available */
	int getMinMaxInt(int prop, paInt& minumum, paInt& maximum, paInt& def);
	/** get valid minimum, maximum and default value for property 'prop'
		@return 0 if min/max/def information is not available */
	int getMinMaxDouble(int prop, double& minumum, double& maximum, double& def);
	int getMinMaxString(int prop, int& minumum, int& maximum, SString& def);
	static int getMinMaxIntFromTypeDef(const char* type, paInt& minumum, paInt& maximum, paInt& def);
	static int getMinMaxDoubleFromTypeDef(const char* type, double& minumum, double& maximum, double& def);
	static int getMinMaxStringFromTypeDef(const char* type, int& minumum, int& maximum, SString& def);

	virtual void setDefault();
	virtual void setDefault(int i);
	void setMin();
	void setMax();
	void setMin(int i);
	void setMax(int i);

	/** return the human readable description of the given type */
	static SString friendlyTypeDescrFromTypeDef(const char* type);
	SString friendlyTypeDescr(int i) { return friendlyTypeDescrFromTypeDef(type(i)); }

	/** copy all property values from other ParamInterface object */
	void copyFrom(ParamInterface *src);

	/** Copy all property values from compatible ParamInterface object.
		This method is more efficient than copyFrom,
		but can be used only if the other object has the same properties sequence, e.g.:
		- any two Param objects having common paramtab
		- any ParamInterface object and the Param with paramtab constructed by ParamObject::makeParamTab
		*/
	void quickCopyFrom(ParamInterface *src);

	enum FileFormat { FormatMultiLine, FormatSingleLine }; // FormatJSON in the future?
	struct LoadOptions {
		bool warn_unknown_fields; bool *abortable; int *linenum; int offset; bool parse_failed;
		LoadOptions() : warn_unknown_fields(false), abortable(NULL), linenum(NULL), offset(0), parse_failed(false) {}
	};

	int saveMultiLine(VirtFILE*, const char* altname = NULL, bool force = 0);
	int saveprop(VirtFILE*, int i, const char* p, bool force = 0);

	int load(FileFormat format, VirtFILE*, LoadOptions *load_options = NULL);///< @return the number of fields loaded
	int load(FileFormat format, const SString &, LoadOptions *load_options = NULL);///< @return the number of fields loaded
protected:
	int loadMultiLine(VirtFILE*, LoadOptions &options);///< @return the number of fields loaded
	int loadSingleLine(const SString &, LoadOptions &options);///< @return the number of fields loaded
public:

	static const char* SERIALIZATION_PREFIX;

	static bool isValidTypeDescription(const char* t);
#ifdef _DEBUG
	virtual void sanityCheck(int i) {}
#endif
};

// implementations:

extern char MakeCodeGuardHappy;

#define PROCOFFSET(_proc_) ( (void (*)(void*,ExtValue*,ExtValue*)) &(FIELDSTRUCT :: _proc_ ## _statrick))
#define STATICPROCOFFSET(_proc_) ( (void (*)(void*,ExtValue*,ExtValue*)) &(FIELDSTRUCT :: _proc_))
#define GETOFFSET(_proc_) ( (void (*)(void*,ExtValue*)) &(FIELDSTRUCT :: _proc_ ## _statrick))
#define SETOFFSET(_proc_) ( (int (*)(void*,const ExtValue*)) &(FIELDSTRUCT :: _proc_ ## _statrick))

#define FIELDOFFSET(_fld_) ((intptr_t)((char*)(&((FIELDSTRUCT*)&MakeCodeGuardHappy)->_fld_)-((char*)((FIELDSTRUCT*)&MakeCodeGuardHappy))))

#ifdef _DEBUG
#define PARAM_ILLEGAL_OFFSET ((intptr_t)0xdeadbeef)
#else
#define PARAM_ILLEGAL_OFFSET 0
#endif

#define FIELD(_fld_) FIELDOFFSET(_fld_),0,0
#define LONGOFFSET(_o_) (_o_),0,0
#define PROCEDURE(_proc_) PARAM_ILLEGAL_OFFSET,(void*)PROCOFFSET(_proc_),0
#define STATICPROCEDURE(_proc_) PARAM_ILLEGAL_OFFSET,(void*)STATICPROCOFFSET(_proc_),0
#define GETSET(_proc_) PARAM_ILLEGAL_OFFSET,(void*)GETOFFSET(get_ ## _proc_),(void*)SETOFFSET(set_ ## _proc_)
#define GETFIELD(_proc_) FIELDOFFSET(_proc_),(void*)GETOFFSET(get_ ## _proc_),0
#define SETFIELD(_proc_) FIELDOFFSET(_proc_),0,(void*)SETOFFSET(set_ ## _proc_)
#define GETONLY(_proc_) PARAM_ILLEGAL_OFFSET,(void*)GETOFFSET(get_ ## _proc_),0
#define SETONLY(_proc_) PARAM_ILLEGAL_OFFSET,0,(void*)SETOFFSET(set_ ## _proc_)

#define PARAMPROCARGS ExtValue* args,ExtValue* ret
#define PARAMSETARGS const ExtValue* arg
#define PARAMGETARGS ExtValue* ret

#define PARAMPROCDEF(name) STATRICKDEF2(name,ExtValue*,ExtValue*)
#define PARAMGETDEF(name) STATRICKDEF1(get_ ## name,ExtValue*)
#define PARAMSETDEF(name) STATRICKRDEF1(int,set_ ## name,const ExtValue*)

///////////////////////////////

struct ParamEntry
{
	const char *id;
	paInt group, flags;
	const char *name, *type;
	intptr_t offset;
	void *fun1; ///< procedure or get
	void *fun2; ///< set
	const char *help;
};

struct ParamEntryConstructor : public ParamEntry
{
public:
	ParamEntryConstructor(const char *_id, paInt _group = 0, paInt _flags = 0, const char *_name = 0, const char *_type = 0, intptr_t _offset = 0, void *_fun1 = 0, void *_fun2 = 0, const char *_help = 0)
	{
		id = _id; group = _group; flags = _flags; name = _name; type = _type; offset = _offset; fun1 = _fun1; fun2 = _fun2; help = _help;
	}
};

class SimpleAbstractParam : public virtual ParamInterface
{
protected:
	virtual void *getTarget(int i);
	const char* myname;
	bool dontcheckchanges;

public:
	void *object;

	const char* getName() { return myname; }
	void setName(const char* n) { myname = n; }

	/**
		@param t ParamEntry table
		@param o controlled object
		@param n Param's name
		*/
	SimpleAbstractParam(void* o = 0, const char*n = 0) :myname(n), dontcheckchanges(0), object(o) {}
	void setDontCheckChanges(bool x) { dontcheckchanges = x; }

	void select(void *o) { object = o; }
	void* getSelected() { return object; }

	virtual ParamEntry *entry(int i) = 0;
	const char *id(int i) { return (i >= getPropCount()) ? 0 : entry(i)->id; }
	const char *name(int i) { return entry(i)->name; }
	const char *type(int i) { return entry(i)->type; }
	const char *help(int i) { return entry(i)->help; }
	int flags(int i) { return entry(i)->flags; }
	int group(int i) { return entry(i)->group; }
	void call(int i, ExtValue* args, ExtValue *ret);

	SString getString(int);
	paInt getInt(int);
	double getDouble(int);
	ExtObject getObject(int);
	ExtValue getExtValue(int);

	void messageOnExceedRange(int i, int setflags, ExtValue& valuetoset);
	int setInt(int, paInt);
	int setDouble(int, double);
	int setString(int, const SString &);
	int setObject(int, const ExtObject &);
	int setExtValue(int, const ExtValue &);

	int isequal(int i, void* defdata);
	void saveSingleLine(SString&, void *defdata, bool addcr = true, bool all_names = true);

	virtual void setDefault();
	virtual void setDefault(int i);

#ifdef _DEBUG
	void sanityCheck(int i);
#endif
};

class Param : public SimpleAbstractParam
{
protected:
	ParamEntry *entry(int i) { return tab + tab[0].group + i; }
public:
	ParamEntry *tab;
	/**
		@param t ParamEntry table
		@param o controlled object
		@param n Param's name
		*/

	Param(ParamEntry *t = 0, void* o = 0, const char*n = 0) :SimpleAbstractParam(o, n), tab(t)
	{
		if (!n&&tab) myname = tab[0].name;
	}

	Param(const Param& p) :SimpleAbstractParam(p.object, p.myname), tab(p.tab) {}
	void operator=(const Param&p) { object = p.object; myname = p.myname; tab = p.tab; }

	const char* getDescription() { return tab[0].type; }

	int getGroupCount() { return tab[0].group; }
	int getPropCount() { return tab[0].flags; }
	const char *grname(int i) { return (i < getGroupCount()) ? tab[i].id : 0; }
	int grmember(int, int);
	void setParamTab(ParamEntry *t, int dontupdatename = 0) { tab = t; if ((!dontupdatename) && tab) myname = tab[0].name; }
	ParamEntry *getParamTab() const { return tab; }
};

extern ParamEntry empty_paramtab[];

#endif
