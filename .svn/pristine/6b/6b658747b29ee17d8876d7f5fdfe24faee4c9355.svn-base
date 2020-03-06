// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MULTIMAP_H_
#define _MULTIMAP_H_

#include "multirange.h"

class SingleMapping
{
public:
	SingleMapping(int b, const MultiRange& t) :begin(b), to(t) {}
	SingleMapping(int b) :begin(b) {}
	SingleMapping(const SingleMapping& sm) :begin(sm.begin), to(sm.to) {}
	int begin;
	MultiRange to;
};

/** MultiMap - used for conversion mapping.
	Read about how mappings work: http://www.framsticks.com/files/common/GeneticMappingsInArtificialGenomes.pdf
	see @ref convmap
	*/
class MultiMap
{
	/** list of (SingleMapping*) */
	SList data;

	SingleMapping* getData(int i) const { return (SingleMapping*)data(i); }
	void addData(int i, SingleMapping* mapping) { data.insert(i, (void*)mapping); }
	void removeData(int i) { data.remove(i); }
	int findData(int x) const;
	int getBegin(int i) const { return getData(i)->begin; }

	// addRangeXXX return the shift for range numbers > r
	int addRange(int &r, const MultiRange& mr);
	// value of 'r' is adjusted according to its range number change
	int addRangeBeginEnd(int &r, int begin, int end, const MultiRange& mr);

public:
	MultiMap() {}
	MultiMap(const MultiMap& mm) { operator=(mm); }
	~MultiMap();
	void operator=(const MultiMap& mm);

	void clear();
	int isEmpty() const { return data.size() == 0; }
	int mappingCount() const { return data.size(); }
	SingleMapping* getMapping(int i) const { return (SingleMapping*)getData(i); }
	int findMappingId(int x) const { return findData(x); }
	int rangeCount() const { return isEmpty() ? 0 : data.size() - 1; }
	IRange getRange(int i) const;

	int getBegin() const;
	int getEnd() const;

	void add(const IRange& from, const IRange& to) { add(from.begin, from.end, MultiRange(to)); }
	void add(const IRange& from, const MultiRange& to) { add(from.begin, from.end, to); }
	void add(int from1, int from2, int to1, int to2) { add(from1, from2, MultiRange(to1, to2)); }
	void add(int from1, int from2, const MultiRange& to);
	void add(const MultiRange& from, const MultiRange& to);
	void add(const MultiMap& mm);
	void addCombined(const MultiMap& m1, const MultiMap& m2);
	void addReversed(const MultiMap& mm);

	const MultiRange& map(int x) const;
	MultiRange map(int begin, int end) const;
	MultiRange map(const IRange& range) const { return map(range.begin, range.end); }
	MultiRange map(const MultiRange& ranges) const;

	void print() const;
	void print2() const;
};

#endif
