//
// FILE NAME: TestCIDLib_HashMap.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/28/2002
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  hash map collection class.
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
#include    "TestCIDLib_CommonCollect.hpp"


// -----------------------------------------------------------------------------
//  Typedef our test maps
// -----------------------------------------------------------------------------
typedef THashMap<TArea,TString,TStringKeyOps>   THashMapOfTArea;
typedef THashMapOfTArea::TPair                  TTestPair;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class THashMap<TArea, TString, TStringKeyOps>;



// -----------------------------------------------------------------------------
//  Local constants
// -----------------------------------------------------------------------------
static const tCIDLib::TCard4 c4HashModulus = 17;
static const tCIDLib::TCard4 c4MaxElems = 6;


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

static tCIDLib::TVoid TestSetBasics(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean bCaughtIt;

    // Create a hash ste of TString objects
    THashMapOfTArea colTest(c4HashModulus, new TStringKeyOps);

    // Add in one element, then try to add it again. It should be rejected
    colTest.objAdd(TTestPair(L"Test1", TArea(1, 2, 3, 4)));

    bCaughtIt = kCIDLib::False;
    try
    {
        colTest.objAdd(TTestPair(L"Test1", TArea(1, 2, 3, 4)));
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DuplicateElem))
        {
            TModule::LogEventObj(errToCatch);
            strmOut <<CUR_LN << L"Wrong exception thrown for duplicate element"
                    << kCIDLib::EndLn;
            return;
        }
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Failed to catch duplicate element addition"
                << kCIDLib::EndLn;
    }
}

static tCIDLib::TVoid TestSetCommon(TTextOutStream& strmOut)
{
    // Create a hash ste of TString objects
    THashMapOfTArea colTest(c4HashModulus, new TStringKeyOps);

    //
    //  Add in the elements, creating a unique value for each one, which
    //  is required for a map.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MaxElems; c4Index++)
    {
        colTest.objAdd
        (
            TTestPair
            (
                TString(L"Testing") + TString(TCardinal(c4Index))
                , TArea(0, 0, c4Index, c4Index)
            )
        );
    }

    //
    //  Do the copy and duplication tests. We provide a standard object
    //  equality function for the element comparison.
    //
    TestColCopy<THashMapOfTArea, TTestPair>(strmOut, colTest);

    //
    //  Invoke the basic collection testing template function. It will do
    //  some generic things that all collections should do the same.
    //
    TestColBasics<THashMapOfTArea, TTestPair>(strmOut, colTest, c4MaxElems);
}


static tCIDLib::TVoid TestSetStreaming(TTextOutStream& strmOut)
{
    THashMapOfTArea colTest(17, new TStringKeyOps);

    // Put some values into it
    colTest.objAdd(TTestPair(L"Test1", TArea(1, 2, 3, 4)));
    colTest.objAdd(TTestPair(L"Test2", TArea(2, 3, 4, 5)));
    colTest.objAdd(TTestPair(L"Test3", TArea(3, 4, 5, 6)));

    //
    //  Create a memory based output stream, and an input stream linked
    //  to it.
    //
    TBinMBufOutStream strmTestOut(4096UL);
    TBinMBufInStream strmTestIn(strmTestOut);

    // Stream the bag out, flush it, then stream back into it
    strmTestOut << colTest << kCIDLib::FlushIt;
    colTest.RemoveAll();
    strmTestIn >> colTest;

    // Make sure we got the right stuff back
    if (colTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Streamed in map had bad element count" << kCIDLib::EndLn;
        return;
    }

    if (!colTest.bKeyExists(L"Test1")
    ||  !colTest.bKeyExists(L"Test2")
    ||  !colTest.bKeyExists(L"Test3"))
    {
        strmOut << CUR_LN << L"Streamed in map had missing elements" << kCIDLib::EndLn;
        return;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the hash map class.
//
tCIDLib::TVoid TFacTestCIDLib::TestHashMap()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"common";
        TestSetCommon(strmOut());

        pszCurTest = L"basic";
        TestSetBasics(strmOut());

        pszCurTest = L"streaming";
        TestSetStreaming(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" hash map test" << kCIDLib::EndLn;
        throw;
    }
}

