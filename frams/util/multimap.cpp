// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "multimap.h"
#include <stdio.h>

int MultiMap::getBegin() const
{
	if (isEmpty()) return 0;
	return getBegin(0);
}

int MultiMap::getEnd() const
{
	if (isEmpty()) return -1;
	return getBegin(mappingCount() - 1) - 1;
}

int MultiMap::findData(int x) const
{
	if (isEmpty()) return -1;
	if (getBegin(0) > x) return -1;
	int a, b, c;
	int n = mappingCount() - 1;
	a = 0; b = n; // find c = last range with begin<=x
	while (1){
		c = (a + b + 1) / 2;
		if (getBegin(c) <= x)
		{
			if ((c == n) || (getBegin(c + 1) > x)) return c;
			a = c;
		}
		else
		{
			b = c - 1;
		}
	}
}

int MultiMap::addRange(int &r, const MultiRange& mr)
{
	// precondition: 0 <= r < (mappingCount()-1)
	// 1.maybe change mapping in this range
	int ret = 0;
	SingleMapping *sm = getMapping(r);
	if (sm->to.contains(mr)) return 0; // do nothing
	sm->to.add(mr);
	// after adding mr to this mapping it could be the same as the previous/next one
	if (r > 0) // prev exists
	{
		SingleMapping *prev = getMapping(r - 1);
		if (prev->to == sm->to)
		{ // splice with prev
			removeData(r);
			delete sm;
			sm = prev;
			r--;
			ret--;
		}
	}
	if (r < (mappingCount() - 2)) // next exists
	{
		SingleMapping *next = getMapping(r + 1);
		if (next->to == sm->to)
		{ // splice with next
			removeData(r + 1);
			delete next;
			ret--;
			r--;
		}
	}
	return ret;
}

int MultiMap::addRangeBeginEnd(int &r, int begin, int end, const MultiRange& mr)
{
	// precondition: -1 <= r < mappingCount()
	//               begin <= end
	//               begin >= mapping.begin
	//               end < nextmapping.begin
	SingleMapping *m, *m2, *m1;
	if (r < 0)
	{
		if (mappingCount())
		{
			m = getMapping(0);
			if (end == (m->begin - 1))
			{ // adjacent to m
				if (m->to == mr)
				{ // shift only
					m->begin = begin;
					return 0;
				}
				// single add
				SingleMapping *newmap = new SingleMapping(begin, mr);
				addData(0, newmap);
				r = 0;
				return 1;
			}
		}
		// double add
		SingleMapping *newmap = new SingleMapping(begin, mr);
		SingleMapping *newmap2 = new SingleMapping(end + 1);
		addData(0, newmap);
		addData(1, newmap2);
		r = 1;
		return 2;
	}

	if (r == (mappingCount() - 1))
	{
		m = getMapping(r);
		m1 = getMapping(r - 1);
		if (begin == m->begin)
		{ // adjacent
			if (m1->to == mr)
			{ // shift only
				m->begin = end + 1;
				return 0;
			}
			// single add
			m->begin = end + 1;
			SingleMapping *newmap = new SingleMapping(begin, mr);
			addData(r, newmap);
			return 1;
		}
		// double add
		SingleMapping *newmap = new SingleMapping(begin, mr);
		SingleMapping *newmap2 = new SingleMapping(end + 1);
		addData(r + 1, newmap);
		addData(r + 2, newmap2);
		r += 2;
		return 2;
	}

	m = getMapping(r);
	if (m->to.contains(mr)) return 0;
	if (begin == m->begin) // begin at range boundary
	{
		m2 = getMapping(r + 1);
		if (end == (m2->begin - 1)) return addRange(r, mr);

		SingleMapping *newmap = new SingleMapping(begin, m->to);
		newmap->to.add(mr);
		if (r > 0)
		{ // join prev possible...
			m1 = getMapping(r - 1);
			if (m1->to == newmap->to)
			{ // joint prev = shift
				delete newmap;
				m->begin = end + 1;
				return 0;
			}
		}
		// single add:
		m->begin = end + 1;
		addData(r, newmap);
		return 1;
	}

	m2 = getMapping(r + 1);
	if (end == (m2->begin - 1)) // end at range boundary
	{
		SingleMapping *newmap = new SingleMapping(begin, m->to);
		newmap->to.add(mr);
		if (r < (mappingCount() - 2))
		{ // join next possible...
			if (m2->to == newmap->to)
			{ // joint next = shift
				m2->begin = begin;
				delete newmap;
				return 0;
			}
		}
		// single add
		r++;
		addData(r, newmap);
		return 1;
	}
	// double add:
	SingleMapping *newmap = new SingleMapping(begin, m->to);
	newmap->to.add(mr);
	SingleMapping *newmap2 = new SingleMapping(end + 1, m->to);
	addData(r + 1, newmap);
	addData(r + 2, newmap2);
	r += 2;
	return 2;
}

void MultiMap::add(int begin, int end, const MultiRange& to)
{
	if (to.isEmpty()) return;
	if (end < begin) return;
	int r1 = findData(begin);
	int r2 = findData(end);
	if (r1 == r2)
	{
		addRangeBeginEnd(r1, begin, end, to);
	}
	else
	{
		r2 += addRangeBeginEnd(r1, begin, getBegin(r1 + 1) - 1, to);
		r1++;
		for (; r1 < r2; r1++)
			r2 += addRange(r1, to);
		addRangeBeginEnd(r1, getBegin(r1), end, to);
	}
}

const MultiRange& MultiMap::map(int x) const
{
	static MultiRange empty;
	int m = findData(x);
	if (m < 0) return empty;
	SingleMapping *sm = getMapping(m);
	return sm->to;
}

MultiRange MultiMap::map(int begin, int end) const
{
	MultiRange mr;
	int m = findData(begin);
	for (; m < rangeCount(); m++)
		if (m >= 0)
		{
		SingleMapping *sm = getMapping(m);
		if (sm->begin > end) break;
		mr.add(sm->to);
		}
	return mr;
}

MultiRange MultiMap::map(const MultiRange& ranges) const
{
	MultiRange mr;
	for (int i = 0; i < ranges.rangeCount(); i++)
		mr.add(map(ranges.getRange(i)));
	return mr;
}

IRange MultiMap::getRange(int i) const
{
	if ((i<0) || (i>(data.size() - 1))) return IRange();
	return IRange(getBegin(i), getBegin(i + 1) - 1);
}

void MultiMap::operator=(const MultiMap& mm)
{
	clear();
	for (int i = 0; i < mm.mappingCount(); i++)
		addData(i, new SingleMapping(*mm.getMapping(i)));
}

void MultiMap::addCombined(const MultiMap& m1, const MultiMap& m2)
{
	for (int i = 0; i < m1.rangeCount(); i++)
	{
		IRange r = m1.getRange(i);
		add(r, m2.map(m1.getMapping(i)->to));
	}
}

void MultiMap::add(const MultiRange& from, const MultiRange& to)
{
	for (int i = 0; i < from.rangeCount(); i++)
		add(from.getRange(i), to);
}

void MultiMap::addReversed(const MultiMap& m)
{
	for (int i = 0; i < m.rangeCount(); i++)
	{
		IRange r = m.getRange(i);
		const MultiRange& mr = m.getMapping(i)->to;
		for (int j = 0; j < mr.rangeCount(); j++)
			add(mr.getRange(j), r);
	}
}

MultiMap::~MultiMap()
{
	clear();
}

void MultiMap::clear()
{
	for (int i = 0; i < data.size(); i++)
		delete getData(i);
	data.clear();
}

///////////////////

void MultiMap::print() const
{
	printf("{ ");
	for (int i = 0; i < (mappingCount() - 1); i++)
	{
		if (i) printf("  ");
		SingleMapping *sm = getMapping(i);
		SingleMapping *sm2 = getMapping(i + 1);
		if (sm2->begin == sm->begin + 1)
			printf("[%d] -> ", sm->begin);
		else
			printf("[%d-%d] -> ", sm->begin, sm2->begin - 1);
		sm->to.print();
		printf("\n");
	}
	printf("}\n");
}

void MultiMap::print2() const
{
	int y;
	int id = 0, id2;
	if (isEmpty())
	{
		printf("{ empty }\n");
		return;
	}
	printf("{\n");
	for (y = getBegin(); y <= getEnd(); y++)
	{
		id2 = findMappingId(y + 1);
		printf("%2d %c", y, (id2 != id) ? '_' : ' ');
		id = id2;
		map(y).print2();
		printf("\n");
	}
	printf("}\n");
}
