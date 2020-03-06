// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "list.h"
#include "statrick.h"
#include <stdint.h>

//#define USEMEMBERCALLBACK

class CallbackNode
{
public:
	virtual ~CallbackNode() {}
	virtual void action(intptr_t calldata) = 0;
	virtual int equals(CallbackNode*n) { return (this == n); }
};

#ifdef USEMEMBERCALLBACK
class CallBase;
class MemberCallbackNode :public CallbackNode
{
	void *userdata;
	CallBase *object;
	void (CallBase::*member)(void*, intptr_t);
public:
	MemberCallbackNode(CallBase *o, void (CallBase::*m)(void*, intptr_t), void *d) :object(o), member(m), userdata(d) {}
	void action(intptr_t calldata) { (object->*member)(userdata, calldata); }
	int equals(CallbackNode*);
};
#define MEMBERCALLBACK(obj,mem,dat) new MemberCallbackNode((CallBase*)(obj),(void (CallBase::*)(void*,intptr_t))(mem),(void*)(dat))
#endif

class FunctionCallbackNode :public CallbackNode
{
	void *userdata;
	void(*fun)(void*, intptr_t);
public:
	FunctionCallbackNode(void(*f)(void*, intptr_t), void *d) :userdata(d), fun(f) {}
	void action(intptr_t calldata) { (*fun)(userdata, calldata); }
	int equals(CallbackNode*);
};
#define FUNCTIONCALLBACK(fun,dat) new FunctionCallbackNode((void (*)(void*,intptr_t))(fun),(void*)(dat))

class StatrickCallbackNode :public CallbackNode
{
	void *object;
	void *userdata;
	void(*fun)(void*, void*, intptr_t);
public:
	StatrickCallbackNode(void *o, void(*f)(void*, void*, intptr_t), void *d) :object(o), userdata(d), fun(f) {}
	void action(intptr_t calldata) { (*fun)(object, userdata, calldata); }
	int equals(CallbackNode*);
};
#define STATRICKCALLBACK(obj,name,dat) new StatrickCallbackNode((void*)(obj),(void (*)(void*,void*,intptr_t))STATRICKNAME(name),(void*)(dat))

/**
   Like in old 'DuoList' you can register for an event giving function pointer
   add(Function* fun, void* anydata)
   'fun' will be called with your pointer as the first argument (void*)
   and event specific value as the second argument (intptr_t)
   fun(void* anydata,intptr_t eventdata)

   'StatrickCallbackNode' uses static functions to emulate object member calls.
   @see statrick.h

   Unregistering callbacks:
   The old remove(...) still works, but removeNode() is more efficient.
   To use it you have to store what you get from add(...);
   CallbackNode* node=thelist.l_add.add(&fun,data);
   // thelist.l_add.remove(&fun,data);
   thelist.l_add.removeNode(node); // this is better!

   */

class Callback : protected SList
{
public:
	~Callback();
	CallbackNode* add(CallbackNode*n);
	CallbackNode* add(void(*f)(void*, intptr_t), void *d)
	{
		return add(new FunctionCallbackNode(f, d));
	}
	void remove(void(*f)(void*, intptr_t), void *d)
	{
		remove(new FunctionCallbackNode(f, d));
	}
	void remove(CallbackNode*n);
	void removeNode(CallbackNode*n);
	void operator+=(void* fun) { add((void(*)(void*, intptr_t))fun, 0); }
	void operator-=(void* fun) { remove((void(*)(void*, intptr_t))fun, 0); }
	void action(intptr_t data);
	void action() { action(0); }
	int size() { return SList::size(); }
	void clear() { SList::clear(); }
};

///////////////////

#define STCALLBACKDEF(name) STATRICKDEF2(name,void*,intptr_t)
#define STCALLBACKDEFC(cls,name) STATRICKSTUB2(cls,name,void*,intptr_t)  \
	void name(void* arg1,intptr_t arg2)
#define VIRTCALLBACKDEF(name) STATRICKSTUB2(STATRICKCLASS,name,void*,intptr_t)  \
	virtual void name(void* arg1,intptr_t arg2)
#define VIRTCALLBACKDEFC(cls,name) STATRICKSTUB2(cls,name,void*,intptr_t)  \
	virtual void name(void* arg1,intptr_t arg2)

/* STCALLBACKDEFC(Class,name)

	 |
	 v

	 #define STATRICKCLASS Class
	 STCALLBACKDEF(name)
	 #undef STATRICKCLASS
	 */

#define CALLBACKARGS void* arg1,intptr_t arg2

#endif
