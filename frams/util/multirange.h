// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MULTIRANGE_H_
#define _MULTIRANGE_H_

#include <frams/util/list.h>

/**
   range in integer domain.
   - (a,a) contains single integer: a
   - (a,b) contains [b-a+1] integers
   */
class IRange
{
public:
	int begin, end;
	IRange() :begin(0), end(0) {}
	IRange(int b, int e) :begin(b), end(e) {}
	IRange(const IRange& r) :begin(r.begin), end(r.end) {}
	int size() const { return end - begin + 1; }
	int isEmpty() const { return end < begin; }
	void makeEmpty() { end = begin; }
	void set(int b, int e) { begin = b; end = e; }
	void intersect(const IRange& r);
	void add(const IRange& r);
	/** all empty ranges are equal */
	int operator==(const IRange& r);
	void operator=(const IRange& r) { begin = r.begin; end = r.end; }
	int contains(int x) { return (x >= begin) && (x <= end); }
	int contains(const IRange& r) { return !r.isEmpty() && contains(r.begin) && contains(r.end); }
	void print() const;
};

/** set of ranges = multiple selection. used in conversion mapping. see @ref convmap */
class MultiRange
{
	/** subsequent ranges in array, stored as: {begin1,end1,begin2,end2,...}
		there are data.size()/2 ranges.
		all ranges are sorted by 'begin' value.
		ranges cannot intersect.
		*/
	SListTempl<int> data;
	int getData(int i) const { return (int)data(i); }
	void setData(int i, int x) { data.set(i, x); }
	int getBegin(int i) const { return getData(2 * i); }
	int getEnd(int i) const { return getData(2 * i + 1); }
	void setBegin(int i, int x) { setData(2 * i, x); }
	void setEnd(int i, int x) { setData(2 * i + 1, x); }
	/** find the last range with begin<=x
	 @return -1 if not found */
	int findRange(int x) const;
	void addRange(int i, int b, int e);
	void removeRange(int i);
	void removeRanges(int r1, int r2);

public:
	MultiRange() {}
	MultiRange(const MultiRange &mr) { data = mr.data; }
	void operator=(const MultiRange &mr) { data = mr.data; }
	MultiRange(const IRange &r) { add(r); }
	MultiRange(int begin, int end) { add(begin, end); }
	MultiRange(int x) { add(x); }

	int operator==(const MultiRange &mr) const { return data == mr.data; }

	void clear();
	int isEmpty() const;

	IRange singleRange() const;
	int rangeCount() const;
	IRange getRange(int i) const;

	void add(int x) { add(x, x); }
	void add(const IRange& r) { add(r.begin, r.end); }
	void add(int begin, int end);
	void add(const MultiRange &mr);

	void remove(int x) { remove(x, x); }
	void remove(const IRange& r) { remove(r.begin, r.end); }
	void remove(int begin, int end);
	void remove(const MultiRange &mr);

	int contains(int x) const;
	int contains(const IRange& r) const;
	int contains(const MultiRange &mr) const;

	void intersect(const IRange& r) { intersect(r.begin, r.end); }
	void intersect(int begin, int end);
	void intersect(const MultiRange &mr);

	void shift(int delta);

	void print() const;
	void print2() const;
};

#endif
