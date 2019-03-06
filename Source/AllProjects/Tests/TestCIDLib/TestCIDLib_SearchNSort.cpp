//
// FILE NAME: TestCIDlib_SortSearch.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/03/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module tests the
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"



// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

// A function pointer comparator to test with
static tCIDLib::ESortComps
eCompTest(const tCIDLib::TCard4& c41, const tCIDLib::TCard4& c42)
{
    if (c41 < c42)
        return tCIDLib::ESortComps::FirstLess;
    else if (c41 > c42)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


static tCIDLib::TVoid TestSort(TTextOutStream& strmOut)
{
    tCIDLib::TCard4 c4Index;

    //
    //  Do a very simple sort of a small array. This one is for debugging
    //  purposes since its easy to trace through, unlike the ones below.
    //
    {
        tCIDLib::TInt4 ai4ToSort[6];
        for (tCIDLib::TInt4 i4Number = 0; i4Number < 6; i4Number++)
            ai4ToSort[i4Number] = 3 - i4Number;

        TArrayOps::TSort<tCIDLib::TInt4>
        (
            ai4ToSort, tCIDLib::c4ArrayElems(ai4ToSort), tCIDLib::eComp<tCIDLib::TInt4>
        );

        // Make sure it got sorted
        for (c4Index = 0; c4Index < tCIDLib::c4ArrayElems(ai4ToSort) - 1; c4Index++)
        {
            if (ai4ToSort[c4Index] > ai4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

        // Do a reversed order sort on it now.
        TArrayOps::TSort<tCIDLib::TInt4>
        (
            ai4ToSort
            , tCIDLib::c4ArrayElems(ai4ToSort)
            , tCIDLib::eRevComp<tCIDLib::TInt4>
        );

        // And check it again
        for (c4Index = 0; c4Index < tCIDLib::c4ArrayElems(ai4ToSort) - 1; c4Index++)
        {
            if (ai4ToSort[c4Index] < ai4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was < than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

    }

    //  Create an array of cardinal values and then sort it.
    {
        const tCIDLib::TCard4 c4ArraySize = 512;
        tCIDLib::TCard4 ac4ToSort[c4ArraySize];

        for (c4Index = 0; c4Index < c4ArraySize; c4Index++)
            ac4ToSort[c4Index] = c4Index * (c4Index % 3);

        TArrayOps::TSort<tCIDLib::TCard4>
        (
            ac4ToSort, c4ArraySize, tCIDLib::eComp<tCIDLib::TCard4>
        );

        // Make sure that it got sorted
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (ac4ToSort[c4Index] > ac4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

        // Now sort it descending
        TArrayOps::TSort<tCIDLib::TCard4>
        (
            ac4ToSort, c4ArraySize, tCIDLib::eRevComp<tCIDLib::TCard4>
        );

        // And verify that order
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (ac4ToSort[c4Index] < ac4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << (c4Index + 1) << L" was > than element "
                        << (c4Index) << kCIDLib::EndLn;
                break;
            }
        }
    }

    //
    //  Create a large, already sorted array and sort it. This can cause
    //  problems if the sort algorithm isn't correct.
    //
    //  And then do the same with it completely reverse sorted and sort
    //  it to try to catch other problems.
    //
    {
        const tCIDLib::TCard4 c4ArraySize = 100000;
        tCIDLib::TCard4* pc4ToSort = new tCIDLib::TCard4[c4ArraySize];
        TArrayJanitor<tCIDLib::TCard4> janArray(pc4ToSort);

        for (c4Index = 0; c4Index < c4ArraySize; c4Index++)
            pc4ToSort[c4Index] = c4Index;

        TArrayOps::TSort<tCIDLib::TCard4>
        (
            pc4ToSort, c4ArraySize, tCIDLib::eComp<tCIDLib::TCard4>
        );

        // Make sure that it got sorted
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (pc4ToSort[c4Index] > pc4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

        for (c4Index = 0; c4Index < c4ArraySize; c4Index++)
            pc4ToSort[c4Index] = c4ArraySize - c4Index;

        TArrayOps::TSort<tCIDLib::TCard4>
        (
            pc4ToSort, c4ArraySize, tCIDLib::eComp<tCIDLib::TCard4>
        );

        // Make sure that it got sorted
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (pc4ToSort[c4Index] > pc4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

    }


    // Test sorting an array using a pointer based comparitor
    {
        const tCIDLib::TCard4 c4ArraySize = 512;
        tCIDLib::TCard4* apc4ToSort[c4ArraySize];
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ArraySize; c4Index++)
            apc4ToSort[c4Index] = new tCIDLib::TCard4(c4Index * (c4Index % 3));

        TArrayOps::TSort<tCIDLib::TCard4*>
        (
            apc4ToSort, c4ArraySize, tCIDLib::eCompPtr<tCIDLib::TCard4>
        );

        // Make sure it got sorted
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (*apc4ToSort[c4Index] > *apc4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

        // Clean up the pointers
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ArraySize; c4Index++)
            delete apc4ToSort[c4Index];
    }


    // Test a functoion pointer comparator and reverse functor wrapper
    {
        const tCIDLib::TCard4 c4ArraySize = 512;
        tCIDLib::TCard4 ac4ToSort[c4ArraySize];

        for (c4Index = 0; c4Index < c4ArraySize; c4Index++)
            ac4ToSort[c4Index] = c4Index * (c4Index % 3);

        TArrayOps::TSort<tCIDLib::TCard4>(ac4ToSort, c4ArraySize, eCompTest);

        // Make sure that it got sorted
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (ac4ToSort[c4Index] > ac4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << c4Index << L" was > than element "
                        << (c4Index + 1) << kCIDLib::EndLn;
                break;
            }
        }

        // Now sort it descending via the function pointer wrapper
        TArrayOps::TSort<tCIDLib::TCard4>
        (
            ac4ToSort, c4ArraySize, tCIDLib::TRevCompWrap(eCompTest)
        );

        // And verify that order
        for (c4Index = 0; c4Index < c4ArraySize - 1; c4Index++)
        {
            if (ac4ToSort[c4Index] < ac4ToSort[c4Index + 1])
            {
                strmOut << CUR_LN << L"Array was not sorted. Element "
                        << (c4Index + 1) << L" was > than element "
                        << (c4Index) << kCIDLib::EndLn;
                break;
            }
        }
    }
}


static tCIDLib::TVoid TestBinarySearch(TTextOutStream& strmOut)
{
    const tCIDLib::TCard4   ac4Test[] =
    {
        1, 5, 9, 10, 14, 18, 29, 50, 190, 200, 410
    };
    const tCIDLib::TCard4 c4ElemCount = tCIDLib::c4ArrayElems(ac4Test);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ElemCount; c4Index++)
    {
        tCIDLib::TCard4 c4FoundAt;
        if (!TArrayOps::bBinarySearch
        (
            ac4Test
            , ac4Test[c4Index]
            , c4FoundAt
            , c4ElemCount
            , tCIDLib::eComp<tCIDLib::TCard4>))
        {
            strmOut << CUR_LN << L"Failed to find element with value "
                    << ac4Test[c4Index] << kCIDLib::EndLn;
        }
         else if (c4FoundAt != c4Index)
        {
            strmOut << CUR_LN << L"Element found at:" << c4FoundAt
                    << L" instead of "
                    << c4Index << L" as expected\n";
        }
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------
tCIDLib::TVoid TFacTestCIDLib::TestSearchSort()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"Sort";
        TestSort(strmOut());

        pszCurTest = L"Binary Search";
        TestBinarySearch(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn << kCIDLib::FlushIt;
        throw;
    }
}

