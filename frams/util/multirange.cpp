// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "multirange.h"
#include <common/nonstd_stl.h>

#include <stdio.h>

int IRange::operator==(const IRange& r)
{
	if (isEmpty() && r.isEmpty()) return 1;
	return (begin == r.begin) && (end == r.end);
}

void IRange::intersect(const IRange& r)
{
	if (r.begin > begin) begin = r.begin;
	if (r.end < end) end = r.end;
}
void IRange::add(const IRange& r)
{
	if (r.begin < begin) begin = r.begin;
	if (r.end > end) end = r.end;
}

void IRange::print() const
{
	if (begin == end) printf("[%d]", begin);
	else printf("[%d-%d]", begin, end);
}

////////////////////////////////////////

void MultiRange::print() const
{
	printf("[");
	for (int i = 0; i < data.size(); i += 2)
	{
		int b = getData(i);
		int e = getData(i + 1);
		if (i) printf(",");
		if (b == e) printf("%d", b);
		else printf("%d-%d", b, e);
	}
	printf("]");
}

void MultiRange::print2() const
{
	const IRange& r = singleRange();
	for (int i = 0; i <= r.end; i++)
		printf(contains(i) ? "X" : ".");
}

void MultiRange::shift(int delta)
{
	for (int i = 0; i<data.size(); i++)
		data.get(i) += delta;
}

int MultiRange::findRange(int x) const
{
	if (isEmpty()) return -1;
	if (getData(0)>x) return -1;
	int a, b, c;
	int n = rangeCount() - 1;
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

void MultiRange::addRange(int i, int b, int e)
{
	data.insert(2 * i, b);
	data.insert(2 * i + 1, e);
}

void MultiRange::removeRanges(int r1, int r2)
{
	//data.remove(2*r1,(r2-r1)*2+2);
	for (; r2 >= r1; r2--)
		removeRange(r2);
}

void MultiRange::removeRange(int i)
{
	data.remove(2 * i, 2);
}

void MultiRange::clear()
{
	data.clear();
}

int MultiRange::isEmpty() const
{
	return rangeCount() == 0;
}

IRange MultiRange::singleRange() const
{
	if (isEmpty()) return IRange();
	int b = getData(0);
	int e = getData(data.size() - 1);
	return IRange(b, e);
}

int MultiRange::rangeCount() const
{
	return data.size() / 2;
}

IRange MultiRange::getRange(int i) const
{
	if ((i < 0) || (i >= rangeCount())) return IRange();
	return IRange(getData(2 * i), getData(2 * i + 1));
}

void MultiRange::remove(int begin, int end)
{
	if (end < begin) return; // NOP
	int count = rangeCount();
	if (!count) return;
	int r1 = findRange(begin);
	int r2 = findRange(end);
	if (r2<0) return; // NOP
	if (r1 == r2)
	{
		int e = getEnd(r1);
		int b = getBegin(r1);
		if (begin <= e)
		{
			if (end >= e)
			{
				if (begin>b)
					setEnd(r1, begin - 1);
				else
					removeRange(r1);
			}
			else
			{
				if (begin > b)
				{ //split
					addRange(r1 + 1, end + 1, e);
					setEnd(r1, begin - 1);
				}
				else
				{
					setBegin(r1, end + 1);
				}
			}
		}
	}
	else
	{
		if (r1 >= 0)
		{
			int e1 = getEnd(r1);
			int b1 = getBegin(r1);
			if (begin <= e1)
			{
				if (begin == b1)
				{
					removeRange(r1);
					r2--;
					r1--;
				}
				else
				{
					setEnd(r1, begin - 1);
				}
			}
		}
		int e2 = getEnd(r2);
		if (end < e2)
		{
			setBegin(r2, end + 1);
			removeRanges(r1 + 1, r2 - 1);
		}
		else
		{
			removeRanges(r1 + 1, r2);
		}
	}
}

void MultiRange::add(int begin, int end)
{
	if (end < begin) return; // NOP
	int count = rangeCount();
	int last = count - 1;
	int r1 = findRange(begin);
	int r2 = findRange(end);
	if (r2 < 0) // special case: before first range
	{
		if (count && (getData(0) == (end + 1)))
			setData(0, begin);
		else
			addRange(0, begin, end);
		return;
	}
	if (r1 < 0) // special case: begin is before first range
	{
		setData(0, begin);
		r1 = 0;
	}
	// now r1>=0 and r2>=0
	int joinbegin = (begin <= (getEnd(r1) + 1));
	int joinend = (r2 < last) && (end >= (getBegin(r2 + 1) - 1));
	const int SAME = 1;
	const int JOINBEGIN = 2;
	const int JOINEND = 4;
	int action = ((r1 == r2) ? SAME : 0) + (joinbegin ? JOINBEGIN : 0) + (joinend ? JOINEND : 0);
	switch (action)
	{
	case SAME + JOINBEGIN + JOINEND: // remove 1 range
		setEnd(r1, getEnd(r1 + 1));
		removeRange(r1 + 1);
		break;
	case SAME + JOINBEGIN: // extend 1 range
		setEnd(r1, max(getEnd(r2), end));
		break;
	case SAME + JOINEND: // extend 1 range
		setBegin(r1 + 1, begin);
		break;
	case SAME: // add 1 range
		addRange(r1 + 1, begin, end);
		break;

	case JOINBEGIN + JOINEND: // extend r2+1
		setBegin(r2 + 1, getBegin(r1));
		removeRanges(r1, r2);
		break;
	case JOINBEGIN: // extend r1
		setEnd(r1, max(end, getEnd(r2)));
		removeRanges(r1 + 1, r2);
		break;
	case JOINEND: // extend r2+1
		setBegin(r2 + 1, begin);
		removeRanges(r1 + 1, r2);
		break;
	case 0: // extend r2
		setBegin(r2, begin);
		setEnd(r2, max(end, getEnd(r2)));
		removeRanges(r1 + 1, r2 - 1);
		break;
	}
}

void MultiRange::remove(const MultiRange &mr)
{
	for (int i = 0; i < mr.rangeCount(); i++)
	{
		IRange r = mr.getRange(i);
		remove(r);
	}
}

void MultiRange::add(const MultiRange &mr)
{
	for (int i = 0; i < mr.rangeCount(); i++)
	{
		IRange r = mr.getRange(i);
		add(r);
	}
}

void MultiRange::intersect(const MultiRange &mr)
{
	// = this - (1-mr)
	MultiRange full(singleRange());
	full.remove(mr);
	remove(full);
}

int MultiRange::contains(const MultiRange &mr) const
{
	for (int i = 0; i < mr.rangeCount(); i++)
	{
		IRange r = mr.getRange(i);
		if (!contains(r)) return 0;
	}
	return 1;
}

int MultiRange::contains(int x) const
{
	int r = findRange(x);
	if (r < 0) return 0;
	return getEnd(r) >= x;
}

int MultiRange::contains(const IRange& r) const
{
	if (r.isEmpty()) return 0;
	int r1 = findRange(r.begin);
	if (r1 < 0) return 0;
	return getRange(r1).contains(r);
}

void MultiRange::intersect(int begin, int end)
{
	if (isEmpty()) return;
	if (end < begin) { clear(); return; }
	IRange sr = singleRange();
	remove(sr.begin, begin - 1);
	remove(end + 1, sr.end);
}
