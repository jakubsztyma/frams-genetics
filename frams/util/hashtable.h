// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "sstring.h"

class HashEntry
{
public:
	int hash;
	SString key;
	void *value;
	HashEntry *next;

	HashEntry(int h, const SString& k, void *v) :hash(h), key(k), value(v), next(0){}
};

class HashEntryIterator;

class HashTable
{
	friend class HashEntryIterator;
	HashEntry **tab;
	int size;
	int count;
	int threshold;
	float load;
	int sync;

	int hash(const SString &s);
	void rehash(int newsize);
public:

	HashTable(int initsize, float lo) { init(initsize, lo); }
	HashTable(int initsize) { init(initsize, 0.75); }
	HashTable() { init(); }
	~HashTable();

	/** always use init() after clear() ! */
	void clear();
	void init(int initsize = 11, float lo = 0.75);

	int getSize() { return count; }
	void* put(const SString& key, void *value);
	void* get(const SString& key, int *reallygot = 0);
	void* remove(const SString& key);
	/** can be used inside iteration loop:
		for(HashEntryIterator it(hashtable);it;) hashtable.remove(it);
		\note iterator is "iterated" to the next entry when the current one is removed (no "it++"!)
		*/
	void* remove(HashEntryIterator& it);

	void debugprint();
	void getstats(float *);
};

/** for(HashEntryIterator it(hashtable);it;it++)
	  {
	  ... it->value
	  ... it->key
	  }
	  */
class HashEntryIterator
{
	void findNext();
public:
	const HashTable *ht;
	int hashindex;
	HashEntry *entry;
	int sync;
	HashEntryIterator(const HashTable&t) :ht(&t), hashindex(0), entry(t.tab[0]), sync(ht->sync)
	{
		if (!entry) findNext();
	}
	HashEntryIterator() {}
	void operator++();
	void operator++(int) { operator++(); }
	HashEntry* operator->() { return entry; }
	bool isValid() { return (entry && (sync == ht->sync)) ? 1 : 0; }
};


#endif
