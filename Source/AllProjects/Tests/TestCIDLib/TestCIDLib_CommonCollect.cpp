//
// FILE NAME: TestCIDLib_CommonCollect.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This module is part of the TestCIDLib.Exe program. Our header provides some
//  templatized collection tests that are used by all the other collection
//  testing files. This guy provides some tests that we don't need to do for
//  every type of collection (since its just written in terms of a collection
//  and will work the same for all of them.)
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
//  Local data
// -----------------------------------------------------------------------------





// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestCallBack(TTextOutStream& strmOut)
{
    const tCIDLib::TCard4 c4MaxElems = 8;

    // Create a deque of cardinals
    TDeque<TCardinal> colTest;

    // Fill it in, numbered by index
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4MaxElems; c4Index++)
        colTest.objPushTop(TCardinal(c4Index+1));

    // Do the callback, which should bump each one up
    colTest.bForEachNC
    (
        [](TCardinal& cTest) { cTest.c4Val(cTest.c4Val() + 1); return kCIDLib::True; }
    );

    // Now test that they were all bumped up
    c4Index = 2;
    TDeque<TCardinal>::TCursor cursTest(&colTest);
    do
    {
        if (*cursTest != c4Index)
        {
            strmOut << CUR_LN << L"NC callback failed to bump up object"
                    << kCIDLib::EndLn;
            break;
        }
        c4Index++;
    }   while (cursTest.bNext());

    // Now test the const version
     tCIDLib::TCard4  c4Counter = 0;
    c4Counter = 0;
    colTest.bForEach
    (
        [&c4Counter](const TCardinal& cTest)
        { c4Counter += cTest.c4Val(); return kCIDLib::True; }
    );

    const tCIDLib::TCard4 c4ExpSum = 2+3+4+5+6+7+8+9;
    if (c4Counter != c4ExpSum)
    {
        strmOut << CUR_LN << L"Callback failed to create correct sum"
                << kCIDLib::EndLn;
    }

    // Do it again with a lambda
    tCIDLib::TCard4 c4LocalCnt = 0;
    colTest.bForEach
    (
        [&c4LocalCnt](const TCardinal& cVal) -> tCIDLib::TBoolean
        {
            c4LocalCnt += cVal.c4Val();
            return kCIDLib::True;
        }
    );

    if (c4Counter != c4ExpSum)
    {
        strmOut << CUR_LN << L"Lambda callback failed to create correct sum"
                << kCIDLib::EndLn;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------
tCIDLib::TVoid TFacTestCIDLib::TestColCommon()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        //
        //  Do common tests for all collection types that we can get away with
        //  it for. We can't do it for those that have compound element types,
        //  i.e. map types, or that have special ctor parameters, e.g. hash
        //  oriented collections.
        //
        {
            TBag<TCardinal> colBag;
            DoCommonColTests<TBag<TCardinal>>(strmOut(), colBag, 64);
        }
        {
            TQueue<TCardinal> colQ;
            DoCommonColTests<TQueue<TCardinal>>(strmOut(), colQ, 43);
        }
        {
            TStack<TCardinal> colStack;
            DoCommonColTests<TStack<TCardinal>>(strmOut(), colStack, 14);
        }
        {
            TVector<TCardinal> colVec(59);
            DoCommonColTests<TVector<TCardinal>>(strmOut(), colVec, 59);
        }

        // Test the collection callout method
        pszCurTest = L"collection callback";
        TestCallBack(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" collection callback test" << kCIDLib::EndLn;
        throw;
    }
}
