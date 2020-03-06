// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef SIMILMATCHING_H
#define SIMILMATCHING_H

#include <vector>

/** This class describes a mutually single-valued function between two sets of elements
	(these sets are called objects). These sets may have different sizes, so this function
	is mutually single-valued only for some subset of the bigger set.
	This class is used while building a matching function between Parts of two Framsticks'
	organisms (similarity measure computation).
 */
class SimilMatching
{
protected:
	/** Array of pointers to two vectors. Each one stores indices of matched elements of
		the other object. Value <0 means that an element is not matched yet. Sizes of these
		vectors are sizes of objects themselves.
	 */
	std::vector<int> *m_apvMatched[2];
public:
	SimilMatching(int Obj0Size, int Obj1Size);
	SimilMatching(const SimilMatching &Source);
	~SimilMatching();
	int GetObjectSize(int Obj);
	void Match(int Obj0, int Index0, int Obj1, int Index1);
	bool IsMatched(int Obj, int Index);
	int GetMatchedIndex(int Obj, int index);
	bool IsFull();
	bool IsEmpty();
	void Empty();
	void PrintMatching();
};

#endif
