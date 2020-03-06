// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "frams/model/similarity/simil_match.h"

/** Runs unit tests of classes used in computation of similarity.
 */
#include <stdio.h>
#include <assert.h>

/** Prints out (standard output) the ERROR message.
 */
void PrintErrorMessage()
{
    printf("There were ERRORS during tests!\n");
}

/** Prints out (standard output) the OK message.
 */
void PrintPassedMessage()
{
    printf("Tests passed OK!\n");
}

/** Test construction of the SimilMatching objects and whether they are empty at
    the beginning.
        @return 0 if OK. Errors break execution (assert())
 */
int TestConstruction()
{
    const int nSize1 = 10;
    const int nSize2 = 200;

    // check symmetric matching
    SimilMatching match1(nSize1, nSize1);

    // test if all elements are unmatched yet
    int i, j;
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < nSize1; i++)
        {
            assert(match1.IsMatched(j, i) == false);
        }
    }

    // test if all are unmatched - new method
    assert(match1.IsEmpty() == true);

    // check assymetric matching
    SimilMatching match2(nSize1, nSize2);

    // check if all elements are unmatched yet in object 0
    for (i = 0; i < nSize1; i++)
    {
        assert(match2.IsMatched(0, i) == false);
    }
    // check if all elements are unmatched yet in object 1
    for (i = 0; i < nSize2; i++)
    {
        assert(match2.IsMatched(1, i) == false);
    }

    // test if all are unmatched - new method
    assert(match2.IsEmpty() == true);

    return 0;
}

/** Tests if sizes of matching are appropriate.
        @return 0 if OK. Errors break execution (assert()).
 */
int TestSizes()
{
    const int nSizeMax = 100;
    SimilMatching *pMatching = NULL;

    // construct objects of different size and check their sizes
    int i, j;
    for (i = 1; i < nSizeMax; i++)
    {
        for (j = 1; j < nSizeMax; j++)
        {

            // create a matching of size (i,j)
            pMatching = new SimilMatching(i, j);
            assert(pMatching != NULL);

            // check size of both objects
            assert(pMatching->GetObjectSize(0) == i);
            assert(pMatching->GetObjectSize(1) == j);

            // delete disused object
            delete pMatching;
        }
    }
    return 0;
}

/** Tests the copying constructor of 4 different, deterministic matchings:
        empty, straight, reverse, sparse.
        Also tests the method Empty().
        @return 0 if OK. Errors break execution (assert())
 */
int TestCopyConstructor()
{
    // define the size used for creating matchings
    const int iSize = 1000;
    int iObj; // a counter of objects in matchings

    {
        // 1st test: create an empty matching, copy it, and check the copy
        SimilMatching Source(iSize, iSize);
        // check if it is empty
        assert(Source.IsEmpty() == true);
        // check sizes of objects
        assert(Source.GetObjectSize(0) == iSize);
        assert(Source.GetObjectSize(1) == iSize);
        // create a copy of this matching
        SimilMatching Dest(Source);
        // check the copy:
        // - sizes of matched object
        assert(Dest.GetObjectSize(0) == Source.GetObjectSize(0));
        assert(Dest.GetObjectSize(1) == Source.GetObjectSize(1));
        // - the copy should be empty, too
        assert(Dest.IsEmpty() == true);
        // make it empty once again
        Dest.Empty();
        // and check once more
        assert(Dest.IsEmpty() == true);
    }

    {
        // 2nd test: create a straight matching (i, i), copy it, and check the copy
        SimilMatching Source(iSize, iSize);
        // check if it is empty
        assert(Source.IsEmpty() == true);
        // check sizes of objects
        assert(Source.GetObjectSize(0) == iSize);
        assert(Source.GetObjectSize(1) == iSize);
        // match objects (iObj, iObj)
        for (iObj = 0; iObj < iSize; iObj++)
        {
            Source.Match(0, iObj, 1, iObj);
        }
        // check if the matching is full
        assert(Source.IsFull() == true);
        // now create a copy of the matching
        SimilMatching Dest(Source);
        // check the copy
        // - sizes of matched object
        assert(Dest.GetObjectSize(0) == Source.GetObjectSize(0));
        assert(Dest.GetObjectSize(1) == Source.GetObjectSize(1));
        // - the copy should be full, too
        assert(Dest.IsFull() == true);
        // - the copy should have exactly the same assignments in matching
        for (iObj = 0; iObj < iSize; iObj++)
        {
            // all object should be matched!
            // check both directions: 0 -> 1
            assert(Dest.IsMatched(0, iObj) == true);
            assert(iObj == Dest.GetMatchedIndex(0, iObj));
            // and: 1 -> 0
            assert(Dest.IsMatched(1, iObj) == true);
            assert(iObj == Dest.GetMatchedIndex(1, iObj));
        }
        // make it empty
        Dest.Empty();
        // and check once more
        assert(Dest.IsEmpty() == true);
    }

    {
        // 3rd test: create a reverse matching (i, N - i - 1), copy it, and check the copy
        SimilMatching Source(iSize, iSize);
        // check if it is empty
        assert(Source.IsEmpty() == true);
        // check sizes of objects
        assert(Source.GetObjectSize(0) == iSize);
        assert(Source.GetObjectSize(1) == iSize);
        // match objects (iObj, N - iObj - 1)
        for (iObj = 0; iObj < iSize; iObj++)
        {
            Source.Match(0, iObj, 1, iSize - iObj - 1);
        }
        // check if the matching is full
        assert(Source.IsFull() == true);
        // now create a copy of the matching
        SimilMatching Dest(Source);
        // check the copy
        // - sizes of matched object
        assert(Dest.GetObjectSize(0) == Source.GetObjectSize(0));
        assert(Dest.GetObjectSize(1) == Source.GetObjectSize(1));
        // - the copy should be full, too
        assert(Dest.IsFull() == true);
        // - the copy should have exactly the same assignments in matching
        for (iObj = 0; iObj < iSize; iObj++)
        {
            // all object should be matched!
            // check both directions: 0 -> 1
            assert(Dest.IsMatched(0, iObj) == true);
            assert((iSize - iObj - 1) == Dest.GetMatchedIndex(0, iObj));
            // and: 1 -> 0
            assert(Dest.IsMatched(1, iObj) == true);
            assert((iSize - iObj - 1) == Dest.GetMatchedIndex(1, iObj));
        }
        // make it empty
        Dest.Empty();
        // and check once more
        assert(Dest.IsEmpty() == true);
    }

    {
        // 4th test: create a sparse matching (i, 2*i), copy it and check the copy
        SimilMatching Source(iSize, 2 * iSize);
        // check if it is empty
        assert(Source.IsEmpty() == true);
        // check sizes of objects
        assert(Source.GetObjectSize(0) == iSize);
        assert(Source.GetObjectSize(1) == 2 * iSize);
        // match objects (iObj, 2 * iObj)
        for (iObj = 0; iObj < iSize; iObj++)
        {
            Source.Match(0, iObj, 1, 2 * iObj);
        }
        // check if the matching is full (should be, as the smaller set is completely matched
        assert(Source.IsFull() == true);
        // now create a copy of the matching
        SimilMatching Dest(Source);
        // check the copy
        // - sizes of matched object
        assert(Dest.GetObjectSize(0) == Source.GetObjectSize(0));
        assert(Dest.GetObjectSize(1) == Source.GetObjectSize(1));
        // - the copy should be full, too
        assert(Dest.IsFull() == true);
        // - the copy should have exactly the same assignments in matching
        for (iObj = 0; iObj < iSize; iObj++)
        {
            // check both directions: 0 -> 1
            // (all matched, (iObj, 2 * iObj))
            assert(Dest.IsMatched(0, iObj) == true);
            assert((2 * iObj) == Dest.GetMatchedIndex(0, iObj));
            // and direction; 1 -> 0
            // (only even are matched, ( 2 * iObj, iObj))
            // for 2 * iObj (which are even): matched
            assert(Dest.IsMatched(1, 2 * iObj) == true);
            assert(iObj == Dest.GetMatchedIndex(1, 2 * iObj));
            // for 2 * iObj + 1 (which are odd): unmatched
            assert(Dest.IsMatched(1, 2 * iObj + 1) == false);
        }
        // make it empty
        Dest.Empty();
        // and check once more
        assert(Dest.IsEmpty() == true);
    }

    return 0;
}

/** Tests different matchings.
        @return 0 if OK. Errors break execution (assert())
 */
int TestMatching()
{
    // define size used by method
    const int nSize1 = 10;
    // some loop counters
    int i;

    // first check some symmetric matching
    SimilMatching match1(nSize1, nSize1);

    // matching is empty
    assert(match1.IsEmpty() == true);

    // create matching - (i,i)
    for (i = 0; i < nSize1; i++)
    {

        // matching is not full
        assert(match1.IsFull() == false);

        // these are not matched yet
        assert(match1.IsMatched(0, i) == false);
        assert(match1.IsMatched(1, i) == false);

        // now - match!
        match1.Match(0, i, 1, i);

        // matching is not empty
        assert(match1.IsEmpty() == false);

        // now they are matched
        assert(match1.IsMatched(0, i) == true);
        assert(match1.IsMatched(1, i) == true);

        // check the matched index for object 0 and 1
        assert(match1.GetMatchedIndex(0, i) == i);
        assert(match1.GetMatchedIndex(1, i) == i);
    }

    // now matching have to be full
    assert(match1.IsFull() == true);

    // check some symmetric matching
    SimilMatching match2(nSize1, nSize1);

    // matching is empty
    assert(match2.IsEmpty() == true);

    // create matching - (i, nSize1 - 1 - i)
    for (i = 0; i < nSize1; i++)
    {

        // matching is not full
        assert(match2.IsFull() == false);

        // these are not matched yet
        assert(match2.IsMatched(0, i) == false);
        assert(match2.IsMatched(1, nSize1 - 1 - i) == false);

        // now - match (but use the opposite syntax)!
        match2.Match(1, nSize1 - 1 - i, 0, i);

        // matching is not empty
        assert(match2.IsEmpty() == false);

        // now they are matched
        assert(match2.IsMatched(0, i) == true);
        assert(match2.IsMatched(1, nSize1 - 1 - i) == true);

        // check the matched index for object 0 and 1
        assert(match2.GetMatchedIndex(0, i) == (nSize1 - 1 - i));
        assert(match2.GetMatchedIndex(1, nSize1 - 1 - i) == i);
    }

    // now matching have to be full
    assert(match2.IsFull() == true);

    // check some asymmnetic matching, too
    SimilMatching match3(nSize1, 2 * nSize1);

    // matching is empty
    assert(match3.IsEmpty() == true);

    // create matching - (i, 2 * i)
    for (i = 0; i < nSize1; i++)
    {

        // matching is not full
        assert(match3.IsFull() == false);

        // these are not matched yet
        assert(match3.IsMatched(0, i) == false);
        assert(match3.IsMatched(1, 2 * i) == false);

        // now - match (but use the opposite syntax)!
        match3.Match(1, 2 * i, 0, i);

        // matching is not empty
        assert(match3.IsEmpty() == false);

        // now they are matched
        assert(match3.IsMatched(0, i) == true);
        assert(match3.IsMatched(1, 2 * i) == true);

        // but the odd elements of object 1 are not matched
        assert(match3.IsMatched(1, 2 * i + 1) == false);

        // check the matched index for object 0 and 1
        assert(match3.GetMatchedIndex(0, i) == 2 * i);
        assert(match3.GetMatchedIndex(1, 2 * i) == i);
    }

    // now matching have to be full (because the smallest object has all elements matched).
    assert(match3.IsFull() == true);

    return 0;
}

/** Defines all tests of SimilMatching class.
        @return 0 if tests passed, (-1) if there were errors.
 */
int main(int argc, char *argv[])
{
    // assume that the result of tests is OK
    bool bResultOK = true;

    // run construction test
    if (TestConstruction() != 0)
    {
        bResultOK = false;
    }

    // run sizes test
    if (TestSizes() != 0)
    {
        bResultOK = false;
    }

    // run matching test
    if (TestMatching() != 0)
    {
        bResultOK = false;
    }

    // run a copy constructor test
    if (TestCopyConstructor() != 0)
    {
        bResultOK = false;
    }

    // print proper message about tests status and return
    if (bResultOK)
    {
        PrintPassedMessage();
        return 0;
    }
    else
    {
        PrintErrorMessage();
        return (-1);
    }
}