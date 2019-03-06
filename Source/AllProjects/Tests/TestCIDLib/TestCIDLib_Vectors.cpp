//
// FILE NAME: TestCIDLib_Vectors.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/31/2002
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
//  This module is part of the TestCIDLib.Exe program. This module is called
//  from the program's main function. The functions in this module test the
//  vector collection class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Facility specific includes
// ----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

// Some functors used in one of the binary search tests below
static tCIDLib::ESortComps
eRedComp(const TRGBClr& rgb1, const TRGBClr& rgb2)
{
    if (rgb1.c1Red() < rgb2.c1Red())
        return tCIDLib::ESortComps::FirstLess;
    if (rgb1.c1Red() > rgb2.c1Red())
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}

static tCIDLib::ESortComps
eRedSort(const tCIDLib::TCard1& c1Comp, const TRGBClr& rgb1)
{
    if (rgb1.c1Red() > c1Comp)
        return tCIDLib::ESortComps::FirstLess;
    if (rgb1.c1Red() < c1Comp)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


static tCIDLib::TVoid TestBasic(TTextOutStream& strmOut)
{
    // Create a string list (a vector of strings)
    tCIDLib::TStrList colTest1(8);

    // Fill it up
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(c4Index);
        colTest1.objAdd(strTmp);
    }


    // Copy the collection and compare the equality operators
    tCIDLib::TStrList colTest2(colTest1);

    if (!tCIDLib::bCompareElems(colTest1, colTest2, TString::bComp))
        strmOut << CUR_LN << L"Vectors != after copy ctor" << kCIDLib::EndLn;

    // Change the copy and test that
    colTest2.RemoveAt(0);
    if (colTest1 == colTest2)
        strmOut << CUR_LN << L"Vectors equal after a change" << kCIDLib::EndLn;
}


// Test binary searches of ref/value vectors
static tCIDLib::TVoid TestSearching(TTextOutStream& strmOut)
{
    tCIDLib::TCard4 c4At;

    // Add some strings to a ref vector, sorted, and binary search it
    {
        TRefVector<TString> colFind(tCIDLib::EAdoptOpts::Adopt, 8);
        tCIDLib::TCard4 c4At;
        colFind.InsertSorted(new TString(L"Value 10"), TString::eComp, c4At);
        colFind.InsertSorted(new TString(L"Value 05"), TString::eComp, c4At);
        colFind.InsertSorted(new TString(L"Value 02"), TString::eComp, c4At);
        colFind.InsertSorted(new TString(L"Value 12"), TString::eComp, c4At);

        const TString strFind(L"Value 10");
        TString* pstrFind = colFind.pobjBinarySearch(strFind, TString::eComp, c4At);
        if (!pstrFind || (*pstrFind != L"Value 10") || (c4At != 2))
            strmOut << CUR_LN << L"Did not find item" << kCIDLib::EndLn;
    }

    // Similar but add some colors and search for a particular red component
    {
        TRefVector<TRGBClr> colFind(tCIDLib::EAdoptOpts::Adopt, 8);
        colFind.InsertSorted(new TRGBClr(3, 2, 3), &eRedComp, c4At);
        colFind.InsertSorted(new TRGBClr(2, 2, 3), &eRedComp, c4At);
        colFind.InsertSorted(new TRGBClr(1, 2, 3), &eRedComp, c4At);

        TRGBClr* prgbFind = colFind.pobjKeyedBinarySearch
        (
            tCIDLib::TCard1(2), &eRedSort, c4At
        );

        const TRGBClr rgbFind(2, 2, 3);
        if (!prgbFind || (*prgbFind != rgbFind) || (c4At != 1))
            strmOut << CUR_LN << L"Did not find item" << kCIDLib::EndLn;
    }

    // Add some strings to a vector, sorted, and binary search it
    {
        TVector<TString> colFind(4UL);
        colFind.InsertSorted(TString(L"Value 10"), &TString::eComp, c4At);
        colFind.InsertSorted(TString(L"Value 05"), &TString::eComp, c4At);
        colFind.InsertSorted(TString(L"Value 02"), &TString::eComp, c4At);
        colFind.InsertSorted(TString(L"Value 12"), &TString::eComp, c4At);

        tCIDLib::TCard4 c4At;
        const TString strFind(L"Value 10");
        TString* pstrFind = colFind.pobjBinarySearch(strFind, &TString::eComp, c4At);
        if (!pstrFind || (*pstrFind != strFind) || (c4At != 2))
            strmOut << CUR_LN << L"Did not find item" << kCIDLib::EndLn;
    }

    // Similar but add some colors and search for a particular red component
    {
        TVector<TRGBClr> colFind(4UL);
        colFind.InsertSorted(TRGBClr(3, 2, 3), &eRedComp, c4At);
        colFind.InsertSorted(TRGBClr(2, 2, 3), &eRedComp, c4At);
        colFind.InsertSorted(TRGBClr(1, 2, 3), &eRedComp, c4At);

        const TRGBClr* prgbFind = colFind.pobjKeyedBinarySearch
        (
            tCIDLib::TCard1(2), &eRedSort, c4At
        );

        const TRGBClr rgbFind(2, 2, 3);
        if (!prgbFind || (*prgbFind != rgbFind) || (c4At != 1))
            strmOut << CUR_LN << L"Did not find item" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestSorting(TTextOutStream& strmOut)
{
    TVector<TString> colTest;

    const tCIDLib::TCard4 ac4Tmp[] =
    {
        4, 8, 1, 7, 2, 4, 3, 3, 1
    };
    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(ac4Tmp);

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(c4Index);
        colTest.objAdd(strTmp);
    }

    // Use the default comparator, which will use the < operator
    colTest.Sort(tCIDLib::eComp<TString>);

    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(c4Index);

        if (colTest[c4Index] != strTmp)
        {
            strmOut << CUR_LN << L"Vector did not sort correctly" << kCIDLib::EndLn;
            break;
        }
    }
}


// In this one, we test adding them sorted, instead of sorting after the fact
static tCIDLib::TVoid TestSorting2(TTextOutStream& strmOut)
{
    TVector<TString> colTest;

    const tCIDLib::TCard4 ac4Unsorted[] =
    {
        4, 8, 1, 7, 2, 4, 3, 3, 1
    };
    const tCIDLib::TCard4 ac4Sorted[] =
    {
        1, 1, 2, 3, 3, 4, 4, 7, 8
    };

    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(ac4Sorted);

    tCIDLib::TCard4 c4Index;
    tCIDLib::TCard4 c4At;
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(ac4Unsorted[c4Index]);
        colTest.InsertSorted(strTmp, tCIDLib::eComp<TString>, c4At);
    }

    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(ac4Sorted[c4Index]);

        if (colTest[c4Index] != strTmp)
        {
            strmOut << CUR_LN << L"Vector did not sort correctly" << kCIDLib::EndLn;
            break;
        }
    }

    // Now do it descending
    colTest.RemoveAll();
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(ac4Unsorted[c4Index]);
        colTest.InsertSorted(strTmp, tCIDLib::eRevComp<TString>, c4At);
    }

    tCIDLib::TInt4 i4Index = tCIDLib::TInt4(c4Count - 1);
    c4Index = 0;
    for (; i4Index > 0; i4Index--, c4Index++)
    {
        TString strTmp(L"Test ");
        strTmp.AppendFormatted(ac4Sorted[i4Index]);

        if (colTest[c4Index] != strTmp)
        {
            strmOut << CUR_LN << L"Vector did not sort correctly" << kCIDLib::EndLn;
            break;
        }
    }
}


// ----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method is one of the testing methods of the facility class. It tests
//  out all of the various capabilities of the TVector class.
//
tCIDLib::TVoid TFacTestCIDLib::TestVectors()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestBasic(strmOut());

        pszCurTest = L"searching";
        TestSearching(strmOut());

        pszCurTest = L"sorting";
        TestSorting(strmOut());

        pszCurTest = L"sorting 2";
        TestSorting2(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"An exception occured during the "
                    << pszCurTest
                    << L" vector test" << kCIDLib::EndLn;
        throw;
    }
}

