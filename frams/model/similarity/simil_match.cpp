// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <assert.h>
#include <stdio.h>
#include "simil_match.h"

/** Creates an empty matching for two objects of specified size.
		@param Obj0Size Size of the first object. Must be positive.
		@param Obj1Size Size of the second object. Must be positive.
 */
SimilMatching::SimilMatching(int Obj0Size, int Obj1Size)
{
	// assure that sizes of objects are positive
	assert(Obj0Size > 0);
	assert(Obj1Size > 0);

	// create necessary vectors
	m_apvMatched[0] = new std::vector<int>(Obj0Size);
	m_apvMatched[1] = new std::vector<int>(Obj1Size);

	// assure that vectors are created
	assert(m_apvMatched[0] != NULL);
	assert(m_apvMatched[1] != NULL);

	// fill vectors with "unmatched" indicator
	for (unsigned int i = 0; i < m_apvMatched[0]->size(); i++)
	{
		m_apvMatched[0]->operator[](i) = -1;
	}
	for (unsigned int i = 0; i < m_apvMatched[1]->size(); i++)
	{
		m_apvMatched[1]->operator[](i) = -1;
	}
}

/** A copying constructor.
		@param Source The object to be copied.
 */
SimilMatching::SimilMatching(const SimilMatching &Source)
{
	// copy the vectors of the actual matching
	m_apvMatched[0] = new std::vector<int>(*(Source.m_apvMatched[0]));
	m_apvMatched[1] = new std::vector<int>(*(Source.m_apvMatched[1]));

	// assure that vectors are created
	assert(m_apvMatched[0] != NULL);
	assert(m_apvMatched[1] != NULL);
}

/** Destroys a matching object.
 */
SimilMatching::~SimilMatching()
{
	// delete vectors of matching
	delete m_apvMatched[0];
	delete m_apvMatched[1];
}

/** Gets size of the specified object.
		@param Index of an object (must be 0 or 1).
		@return Size of the object (in elements).
 */
int SimilMatching::GetObjectSize(int Obj)
{
	// check parameter
	assert((Obj == 0) || (Obj == 1));

	// return the result
	return m_apvMatched[Obj]->size();
}

/** Matches elements given by indices in the given objects.
		@param Obj0 Index of the first object. Must be 0 or 1.
		@param index0 Index of element in the first object. Must be a valid index
		( >= 0 and < size of the object).
		@param Obj1 Index of the second object. Must be 0 or 1 and different from Obj0.
		@param Index1 index of element in the second object. Must be a valid index
		( >= 0 and < size of the object).

 */
void SimilMatching::Match(int Obj0, int Index0, int Obj1, int Index1)
{
	// check parameters of object 0
	assert((Obj0 == 0) || (Obj0 == 1));
	assert((Index0 >= 0) && (Index0 < (int)m_apvMatched[Obj0]->size()));
	// check parameters of object 1
	assert(((Obj1 == 0) || (Obj1 == 1)) && (Obj0 != Obj1));
	assert((Index1 >= 0) && (Index1 < (int)m_apvMatched[Obj1]->size()));

	// match given elements
	// matching_Obj0(Index0) = Index1
	m_apvMatched[Obj0]->operator[](Index0) = Index1;
	// matching_Obj1(Index1) = Index0
	m_apvMatched[Obj1]->operator[](Index1) = Index0;
}

/** Checks if the given element in the given object is already matched.
		@param Obj Index of an object (must be 0 or 1).
		@param Index Index of an element in the given object. Must be a valid index
		( >=0 and < size of the object).
		@return true if the given element is matched, false otherwise.
 */
bool SimilMatching::IsMatched(int Obj, int Index)
{
	// check parameters
	assert((Obj == 0) || (Obj == 1));
	assert((Index >= 0) && (Index < (int)m_apvMatched[Obj]->size()));

	// check if the element is matched
	if (m_apvMatched[Obj]->operator[](Index) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/** Gets index of the element thet is matched in the other object withe the element given
		by parameters.
		@param Obj Index of an object (must be 0 or 1).
		@param Index Index of checked element in the given object.
		@return Index of the element (in the other organism) that is matched with the given
		element. WARNING! If the given element is not matched, the result may be smaller than 0
		(check IsMatched() before using GetMatchedIndex()).
 */
int SimilMatching::GetMatchedIndex(int Obj, int Index)
{
	// check parameters
	assert((Obj == 0) || (Obj == 1));
	assert((Index >= 0) && (Index < (int)m_apvMatched[Obj]->size()));

	// return the index of the matched element
	return m_apvMatched[Obj]->operator[](Index);
}

/** Checks if the matching is already full, i.e. if the smaller object already has all its
		elements matched.
		@return true if matching is full, false otherwise.
 */
bool SimilMatching::IsFull()
{
	// assume that the matching is full
	bool bResult = true;
	// index of the smallest object
	int nObj;

	// find the smallest object (its index)
	if (m_apvMatched[0]->size() < m_apvMatched[1]->size())
	{
		nObj = 0;
	}
	else
	{
		nObj = 1;
	}

	// check if all elements of the smallest object are matched
	for (unsigned int nElem = 0; nElem < m_apvMatched[nObj]->size(); nElem++)
	{
		if (m_apvMatched[nObj]->operator[](nElem) < 0)
		{
			// if any element is not matched, the result is false
			bResult = false;
			break;
		}
	}

	// return the result
	return bResult;
}

/** Checks if the matching is empty (i.e. none of elements is matched).
	@return true if matching is empty, otherwise - false.
 */
bool SimilMatching::IsEmpty()
{
	// result - assume that matching is empty
	bool bResult = true;

	// matching is empty if either of objects has only unmatched elements
	// so it may be first object 
	int nObj = 0;
	for (unsigned int nElem = 0; nElem < m_apvMatched[nObj]->size(); nElem++)
	{
		if (m_apvMatched[nObj]->operator[](nElem) >= 0)
		{
			// if any element of the object is matched (unmatched objects have (-1))
			bResult = false;
			break;
		}
	}

	// return the result from the loop
	return bResult;
}

/** Makes the matching completely empty. After a call to this method IsEmpty() should return true.
 */
void SimilMatching::Empty()
{
	for (int iObj = 0; iObj < 2; iObj++) // a counter of objects
	{
		// for each object in the matching
		for (unsigned int iElem = 0; iElem < m_apvMatched[iObj]->size(); iElem++) // a counter of objects' elements
		{
			// for each element iElem for the object iObj
			// set it as unmatched (marker: -1)
			m_apvMatched[iObj]->operator[](iElem) = -1;
		}
	}

	// the exit condition
	assert(IsEmpty() == true);
}

/** Prints the current state of the matching
 */
void SimilMatching::PrintMatching()
{
	int nBigger;

	// check which object is bigger
	if (m_apvMatched[0]->size() >= m_apvMatched[1]->size())
	{
		nBigger = 0;
	}
	else
	{
		nBigger = 1;
	}

	// print first line - indices of objects
	printf("[ ");
	for (unsigned int i = 0; i < m_apvMatched[nBigger]->size(); i++)
	{
		printf("%2d ", i);
	}
	printf("]\n");

	// print second line and third - indices of elements matched with elements of the objects
	for (int nObj = 0; nObj < 2; nObj++)
	{
		// for both objects - print out lines of matched elements
		printf("[ ");
		for (unsigned int i = 0; i < m_apvMatched[nObj]->size(); i++)
		{
			if (IsMatched(nObj, i))
			{
				// if the element is matched - print the index
				printf("%2d ", GetMatchedIndex(nObj, i));
			}
			else
			{
				// if the element is not matched - print "X"
				printf(" X ");
			}
		}
		printf("]\n");
	}
}