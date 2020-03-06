// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "hashtable.h"

int HashTable::hash(const SString &s)
{
	return s.hash() & 0x7fffffff;
}

void HashTable::init(int initsize, float lo)
{
	size = initsize;
	load = lo;
	threshold = (int)(load*(float)size);
	tab = (HashEntry**)calloc(size, sizeof(HashEntry*));
	count = 0;
	sync = 0;
}

void HashTable::rehash(int newsize)
{
	if (newsize == size) return;
	HashEntry **newtab = (HashEntry**)calloc(newsize, sizeof(HashEntry*));
	HashEntry **te = tab, *e, *ne;
	int i;
	for (int s = size; s > 0; s--, te++)
		for (e = *te; e;)
		{
		ne = e; e = e->next;
		i = ne->hash%newsize;
		ne->next = newtab[i];
		newtab[i] = ne;
		}
	free(tab);
	tab = newtab;
	size = newsize;
	threshold = int(load*(float)size);
	sync++;
}

void HashTable::clear()
{
	HashEntry *e, **te, *next;
	int n;
	for (n = size, te = tab; n > 0; n--, te++)
		for (e = *te; e; e = next)
		{
		next = e->next;
		delete e;
		}
	if (tab) free(tab);
	tab = 0; size = 0;
	sync++;
}

HashTable::~HashTable()
{
	clear();
}

void* HashTable::put(const SString& key, void *value)
{
	int h = hash(key);
	int i = h%size;
	for (HashEntry *e = tab[i]; e; e = e->next)
	{
		if (e->key == key)
		{
			void *v = e->value;
			e->value = value;
			return v;
		}
	}
	if (count >= threshold) { rehash(2 * size + 1); i = h%size; }
	HashEntry *e = new HashEntry(h, key, value);
	e->next = tab[i];
	tab[i] = e;
	count++;
	sync++;
	return 0;
}

void* HashTable::remove(const SString& key)
{
	int i = hash(key) % size;
	HashEntry **ptr = tab + i, *e;
	for (; e = *ptr; ptr = &e->next)
	{
		if (e->key == key)
		{
			*ptr = e->next;
			void *v = e->value;
			delete e;
			count--;
			sync++;
			return v;
		}
	}
	return 0;
}

void* HashTable::remove(HashEntryIterator& it)
{
	if (!it.entry) return 0;
	HashEntry **ptr = tab + it.hashindex, *e;
	for (; e = *ptr; ptr = &e->next)
	{
		if (e == it.entry)
		{
			it++;
			*ptr = e->next;
			void *v = e->value;
			delete e;
			count--;
			sync++;
			it.sync++;
			return v;
		}
	}
	return NULL;
}

void* HashTable::get(const SString& key, int *reallygot)
{
	int i = hash(key) % size;
	for (HashEntry *e = tab[i]; e; e = e->next)
		if (e->key == key) { if (reallygot) *reallygot = 1; return e->value; }
	return 0;
}

/////////

void HashEntryIterator::findNext()
{
	while (hashindex < (ht->size - 1))
	{
		hashindex++;
		if (entry = ht->tab[hashindex]) return;
	}
}


void HashEntryIterator::operator++()
{
	if (entry) entry = entry->next;
	if (!entry) findNext();
}

//////////////////////////

void HashTable::debugprint()
{
	printf("HashTable: %d/%d (max %d)\n", count, size, threshold);
	HashEntry *e, **te;
	int n;
	for (n = 0, te = tab; n < size; n++, te++)
		if (e = *te)
		{
		printf(" %d:", n);
		for (; e; e = e->next)
			printf(" (%x)%s=%p", e->hash, e->key.c_str(), e->value);
		printf("\n");
		}
}

/**
stats[0]=buckets used
stats[1]=min keys in bucket
stats[2]=avg keys in bucket
stats[3]=max keys in bucket
*/
void HashTable::getstats(float *stats)
{
	HashEntry *e, **te;
	int used = 0, ma = 0, mi = count;
	int c, n;
	for (n = size, te = tab; n > 0; n--, te++)
	{
		c = 0;
		for (e = *te; e; e = e->next) c++;
		if (c > ma) ma = c;
		if ((c < mi) && (c>0)) mi = c;
		if (c > 0) used++;
	}
	stats[0] = (float)used;
	stats[1] = (float)((mi == count) ? 0 : mi);
	stats[2] = (float)count / (float)used;
	stats[3] = (float)ma;
}
