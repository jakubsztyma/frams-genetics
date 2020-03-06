// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>
#include <string.h>
#include <common/nonstd.h>

//#define SLISTSTATS

#ifdef SLISTSTATS
#include <stdio.h>
class SListStats
{
public:
	int objects, allocations, copied, totalmem, usedmem;
	SListStats() :objects(0), allocations(0), copied(0), totalmem(0), usedmem() {}
	~SListStats()
	{
		printf("------------------------\n"
			" SListStats:\n"
			" objects     = %ld\n"
			" allocations = %ld\n"
			" copied      = %ld\n"
			" total mem   = %ld\n"
			" usage       = %ld %%\n"
			"------------------------\n",
			objects, allocations, copied, totalmem, (usedmem * 100) / totalmem);
	}
};
#endif


template<class T> class SListTempl
{
protected:
	int have, used, pos; // ile,zaj,poz
	T *mem;
	void resize(int x)
	{
		if (mem || x)
		{
			mem = (T*)realloc(mem, x*sizeof(T));
#ifdef SLISTSTATS
			SListStats::stats.allocations++;
			SListStats::stats.copied += sizeof(T)*min(x, have);
#endif	
		}
		have = x;
	}

public:

	SListTempl(const SListTempl<T>& src) :have(0), used(0), pos(0), mem(0)
	{
		(*this) = src;
	}
	SListTempl() :have(0), used(0), pos(0), mem(0)
	{}
	~SListTempl()
	{
#ifdef SLISTSTATS
		SListStats::stats.objects++;
		SListStats::stats.totalmem += sizeof(T)*have;
		SListStats::stats.usedmem += sizeof(T)*used;
#endif
		hardclear();
	}

	void needSize(int s)
	{
		if (s > have) resize(s + 3 + s / 8);
	}
	void operator+=(const T& e) ///< append one element
	{
		append(e);
	}
	void operator-=(const T& e) ///< remove one element
	{
		int i; if ((i = find(e)) >= 0) remove(i);
	}
	void remove(int i, int n = 1) ///< remove n elements from position i
	{
		if ((i >= size()) || (i<0)) return;
		if (i>(size() - n)) n = size() - i;
		if (pos >= i) pos -= n;
		memmove(mem + i, mem + i + n, sizeof(T)*(used - n - i));
		used -= n;
	}
	void operator-=(int i) { remove(i); } ///< remove element from position i
	T& operator()(int i) const ///< return element at position i
	{
		return mem[i];
	}
	bool hasMore() { return pos < size(); }
	int operator==(const SListTempl<T>& l) const ///< compare list
	{
		if (size() != l.size()) return 0;
		for (int i = 0; i < size(); i++) if (l.mem[i] != mem[i]) return 0;
		return 1;
	}
	int find(const T& e) const ///< return position of element, or -1 if not found
	{
		for (int i = 0; i<size(); i++) if (mem[i] == e) return i;
		return -1;
	}
	void append(const T& data) ///< add 1 element
	{
		needSize(size() + 1); mem[used++] = data;
	}
	void append(const T* data, int n) ///< add n elements
	{
		needSize(size() + n);
		memcpy(mem + used, data, sizeof(T)*n);
		used += n;
	}
	void insert(int p, T* data, int n) ///< insert n elements at position p
	{
		if (p>size()) p = size();
		needSize(size() + n);
		memmove(mem + p + n, mem + p, sizeof(T)*(size() - p));
		memcpy(mem + p, data, sizeof(T)*n);
		if (pos > p) pos += n;
	}
	void insert(int p, const T& data) ///< insert 1 element at position p
	{
		if (p > size()) p = size();
		needSize(size() + 1);
		memmove(mem + p + 1, mem + p, sizeof(T)*(size() - p));
		if (pos > p) pos++;
		mem[p] = data;
		used++;
	}
	void set(int p, const T& d)
	{
		needSize(p + 1); mem[p] = d; if (used < (p + 1)) used = p + 1;
	}
	void setSize(int s)
	{
		needSize(s); used = s;
	}
	T& get(int i) const { return operator()(i); }
	void clear() { used = 0; } ///< remove all elements
	void hardclear() { resize(0); used = 0; } ///< remove all elements and free mem
	int size() const { return used; } ///< list size 
	void operator=(const SListTempl<T>&src) ///duplicate
	{
		setSize(src.size());
		memcpy(mem, src.mem, src.size()*sizeof(T));
	}
	void operator+=(const SListTempl<T>&src) ///< append src contents
	{
		needSize(size() + src.size());
		memcpy(mem + used, src.mem, src.size()*sizeof(T));
		used += src.used;
	}
	void trim(int newsiz)
	{
		if (newsiz < used) used = newsiz;
	}
};

#define FOREACH(type,var,list) for(int _ ## var ## _i_=0,_ ## var ## _tmp_=0; _ ## var ## _i_==0;_ ## var ## _i_++) \
		for(type var;((_ ## var ## _tmp_=(_ ## var ## _i_<list.size()))?(var=(type)list(_ ## var ## _i_)):0),_ ## var ## _tmp_;_ ## var ## _i_++)

// usage example: FOREACH(char*,t,thelist) printf("t=%s\n",t);

template<class T> class PtrListTempl : public SListTempl < T >
{
public:
	T operator()(int i) const ///< return element at position i
	{
		if (i >= this->size()) return 0; else return this->mem[i];
	}

	T get(int i) const { return operator()(i); }
	T& getref(int i) const { return SListTempl<T>::get(i); }
};


	typedef PtrListTempl<void*> SList;

#endif
