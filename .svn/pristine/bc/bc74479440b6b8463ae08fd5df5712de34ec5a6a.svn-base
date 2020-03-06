// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PARAMOBJ_H_
#define _PARAMOBJ_H_

#include "param.h"
#include <frams/util/extvalue.h>

class ParamObject : public DestrBase
{
	ParamObject(int _numfields, ParamEntry *_tab);
public:
	int numfields;
	Param par;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4200) //Cannot generate copy-ctor or copy-assignment operator when UDT contains a zero-sized array
#endif
	ExtValue fields[0];
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	ParamObject() { numfields = 0; }
	~ParamObject();

	void* operator new(size_t s, int numfields){ return ::operator new(s + sizeof(ExtValue)*numfields); }
	void* operator new(size_t s){ return ::operator new(s); }
	void operator delete(void* ptr, int numfields) { ::operator delete(ptr); }
	void operator delete(void* ptr) { ::operator delete(ptr); }
	ParamObject *clone();
	static void p_new(void* obj, ExtValue *args, ExtValue *ret);

	void operator=(const ParamObject& src);

	static int firstFieldOffset();

	/** make a ParamEntry* array for use with Param object.
		offsets in the array are calculated for the ExtObject array as the target.
		valid array can be created with makeObject().
		sample code:
		@code
		ParamInterface *pi=...; // any param interface
		ParamEntry *tab=ParamObject::makeParamTab(pi);
		void* obj=ParamObject::makeObject(tab);
		void* obj2=ParamObject::makeObject(tab);
		Param par(tab,obj);
		par.set(...), par.get(...), par.load(...), par.saveSingle/MultiLine(...);
		par.select(obj);
		par.select(obj2);
		ParamObject::freeObject(obj);
		ParamObject::freeObject(obj2);
		*/
	static ParamEntry* makeParamTab(ParamInterface *pi, bool stripgroups = 0, bool stripproc = 0, int firstprop = 0, int maxprops = 9999, bool dupentries = false, int flagsexclude_data = 0, int flagsexclude_tab = 0, bool addnew = false, const char* rename = NULL, bool readonly_into_userreadonly = false);

	/** deallocate paramtab obtained from makeParamTab() */
	static void freeParamTab(ParamEntry *pe);

	static void setParamTabText(ParamEntry *pe, const char* &ptr, const char* txt);
	static bool paramTabAllocatedString(ParamEntry *pe);
	static bool paramTabEqual(ParamEntry *pe1, ParamEntry *pe2);

	/** @return the object, suitable for Param.select(...).
		@return NULL if 'pi' has no usable properties */
	static ParamObject* makeObject(ParamEntry *tab);

	/** copy data from src to dst (compatibility with older implementation), same as operator=  */
	static void copyObject(void* dst, void* src);

	/** duplicate object (compatibility with older implementation), same as clone()  */
	static void* dupObject(void* obj);

	/** delete all data in the array and deallocate it (compatibility with older implementation), same as delete */
	static void freeObject(void* obj);
};

class ParamTabOwner
{
public:
	ParamEntry *pe;
	ParamTabOwner(ParamEntry *_pe) :pe(_pe) {}
	~ParamTabOwner() { ParamObject::freeParamTab(pe); }
};

#endif
