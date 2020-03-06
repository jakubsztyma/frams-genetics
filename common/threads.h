// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _THREADS_H_
#define _THREADS_H_

#ifdef MULTITHREADED

#include <pthread.h>
#include <stdio.h>

int sysGetCPUCount();

#ifdef LINUX
//#define USE_CPP_TLS
//#define CPP_TLS __thread
#endif

#ifdef __BORLANDC__ //zakladamy ze wszyscy uzywaja pthreadsowych, bo w tych wbudowanych w c++ w obecnym standardzie nie ma destrukcji obiektow (tylko proste struktury)
//#define USE_CPP_TLS 
//#define CPP_TLS __declspec(thread)
#endif

template<class T> class ThreadSingleton
{
	pthread_key_t mt_key;
	bool destroyed;
	
public:

	ThreadSingleton()
	{
		pthread_key_create(&mt_key, &destructor);
		destroyed=false;
	}

	~ThreadSingleton()
	{
		T* o = set(NULL);
		if (o) delete o;
		destroyed=true;
	}

	static void destructor(void* o)
	{
		if (o)
			delete (T*)o;
	}

	T* set(T* new_o)
	{
		T* o = (T*)pthread_getspecific(mt_key);
		pthread_setspecific(mt_key, new_o);
		return o;
	}

	T* get()
	{
		if (destroyed) { printf("Fatal Error: accessing ThreadSingleton after destroying it\n"); return NULL; } // this will never happen
		T* o = (T*)pthread_getspecific(mt_key);
		if (!o)
		{
			o = new T();
			pthread_setspecific(mt_key, o);
		}
		return o;
	}

	T& getref() { return *get(); }
};

template<class T> class ThreadSingletonPtr
{
	pthread_key_t mt_key;

public:

	ThreadSingletonPtr()
	{
		pthread_key_create(&mt_key, NULL);
	}

	T* setptr(T* new_o)
	{
		T* o = (T*)pthread_getspecific(mt_key);
		pthread_setspecific(mt_key, new_o);
		return o;
	}

	T* get()
	{
		return (T*)pthread_getspecific(mt_key);
	}
};

#else //ifdef MULTITHREADED

#include <cstddef> //NULL

#define tlsGet(var) (&ThreadLocal_ ## var ## _getref())
#define tlsGetRef(var) ThreadLocal_ ## var ## _getref()
#define tlsSet(var,o) var.set(o)

#define THREAD_LOCAL_DECL(cls,var) cls& ThreadLocal_ ## var ## _getref()
#define THREAD_LOCAL_DEF(cls,var) cls& ThreadLocal_ ## var ## _getref() \
	{ \
	static cls object; \
	return object; \
	}

template<class T> class ThreadSingletonPtr
{
	T *object;
public:
	ThreadSingletonPtr():object(NULL) {}
	T* get() {return object;}
	T* setptr(T* o) {T* previous=object; object=o; return previous;}
};

#endif //ifdef MULTITHREADED

////////////////////////////////////

// THREAD_LOCAL(cls) - behaves like object of class cls (automatic creation/destruction)
// THREAD_LOCAL(cls)..._PTR - behaves like pointer to cls (initial NULL, no autocreation/destruction)
// var is a (global) name, can't be a class member anymore because it might be implemented as function
// _PTR can only be accessed using tls...Ptr() variant of Get/Set, _ptr suffix is internally used in variable name to avoid mistakes
#ifdef USE_CPP_TLS

// use c++ implementation (CPP_TLS must also be defined)

#define THREAD_LOCAL_DECL(cls,var) CPP_TLS cls* ThreadLocal_ ## var
#define THREAD_LOCAL_DEF(cls,var) CPP_TLS cls* ThreadLocal_ ## var=NULL
#define THREAD_LOCAL_DECL_PTR(cls,var) CPP_TLS cls* ThreadLocal_ ## var ## _ptr
#define THREAD_LOCAL_DEF_PTR(cls,var) CPP_TLS cls* ThreadLocal_ ## var ## _ptr=NULL

template<class T> T* _tlsGet(T*& var)
{
	if (!var)
		var=new T();
	return var;
}

template<class T> T* _tlsGetPtr(T*& var)
{
	return var;
}

#define tlsGet(var) _tlsGet(ThreadLocal_ ## var)
#define tlsGetRef(var) (*_tlsGet(ThreadLocal_ ## var))

template<class T> T* _tlsSet(T*& var,T* new_o)
{
	T* o=var;
	var=new_o;
	return o;
}

#define tlsGetPtr(var) _tlsGetPtr(ThreadLocal_ ## var)
#define tlsSetPtr(var,o) _tlsSet(ThreadLocal_ ## var ## _ptr,o)

#else //#ifdef USE_CPP_TLS

// use pthreads implementation

#ifdef MULTITHREADED
#define THREAD_LOCAL_DECL(cls,var) ThreadSingleton<cls> ThreadLocal_ ## var
#define THREAD_LOCAL_DEF(cls,var) ThreadSingleton<cls> ThreadLocal_ ## var
#define tlsGet(var) ThreadLocal_ ## var.get()
#define tlsGetRef(var) ThreadLocal_ ## var.getref()
#define tlsSet(var,o) ThreadLocal_ ## var.set(o)
#endif

#define THREAD_LOCAL_DECL_PTR(cls,var) extern ThreadSingletonPtr<cls> ThreadLocal_ ## var ## _ptr
#define THREAD_LOCAL_DEF_PTR(cls,var) ThreadSingletonPtr<cls> ThreadLocal_ ## var ## _ptr
#define tlsGetPtr(var) ThreadLocal_ ## var ## _ptr.get()
#define tlsSetPtr(var,o) ThreadLocal_ ## var ## _ptr.setptr(o)

#endif //#ifdef USE_CPP_TLS


#endif
