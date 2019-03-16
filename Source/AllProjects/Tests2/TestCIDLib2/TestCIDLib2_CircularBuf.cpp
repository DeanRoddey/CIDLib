//
// FILE NAME: TestCIDLib2_CircularBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2007
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains tests related to the by ciruclar buffer template class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_CircularBuf,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_CircularBuf
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CircularBuf: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CircularBuf::TTest_CircularBuf() :

    TTestFWTest
    (
        L"Circular Buffer", L"Tests of the Circular Buffer class", 2
    )
{
}

TTest_CircularBuf::~TTest_CircularBuf()
{
}


// ---------------------------------------------------------------------------
//  TTest_CircularBuf: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CircularBuf::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TCircularBuf<TString> cbufTest(6);

    // Should be empty after ctor
    if (!cbufTest.bIsEmpty() || (cbufTest.c4ElemCount() != 0))
    {
        strmOut << TFWCurLn << L"Circular buffer not empty after ctor\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cbufTest.c4MaxElemCount() != 6)
    {
        strmOut << TFWCurLn << L"Max elements not correct after ctor\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Add some elements and test the count
    if (cbufTest.bAdd(L"Value 1")
    ||  cbufTest.bAdd(L"Value 2")
    ||  cbufTest.bAdd(L"Value 3"))
    {
        strmOut << TFWCurLn << L"Circular buffer reported full when it is not\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cbufTest.c4ElemCount() != 3)
    {
        strmOut << TFWCurLn << L"Circular buffer count should be 3\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if ((cbufTest.tAt(0) != L"Value 1") || (cbufTest[0] != L"Value 1"))
    {
        strmOut << TFWCurLn << L"0th element != to expected value\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if ((cbufTest.tAt(1) != L"Value 2") || (cbufTest[1] != L"Value 2"))
    {
        strmOut << TFWCurLn << L"1st element != to expected value\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if ((cbufTest.tAt(2) != L"Value 3") || (cbufTest[2] != L"Value 3"))
    {
        strmOut << TFWCurLn << L"2nd element != to expected value\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Now take it up to full
    if (cbufTest.bAdd(L"Value 4")
    ||  cbufTest.bAdd(L"Value 5")
    ||  cbufTest.bAdd(L"Value 6"))
    {
        strmOut << TFWCurLn << L"Circular buffer reported full when it is not\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cbufTest.c4ElemCount() != 6)
    {
        strmOut << TFWCurLn << L"Circular buffer count should be 6\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if ((cbufTest.tAt(5) != L"Value 6") || (cbufTest[5] != L"Value 6"))
    {
        strmOut << TFWCurLn << L"Last element != to expected value\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Push one more, which should make it wrap
    if (!cbufTest.bAdd(L"Value 7"))
    {
        strmOut << TFWCurLn << L"Circular buffer reported not full when it is\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // The 0th elements now should be value 2
    if ((cbufTest.tAt(0) != L"Value 2") || (cbufTest[0] != L"Value 2"))
    {
        strmOut << TFWCurLn << L"0th element != to expected value 2\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Count still should be 6
    if (cbufTest.c4ElemCount() != 6)
    {
        strmOut << TFWCurLn << L"Circular buffer count should be 6\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Now run up through an even multiple of 6, so that we test the 'throw out the
    //  old one' at all of the possible coordinate positions.
    //
    TString strVal;
    for (tCIDLib::TCard4 c4Index = 7; c4Index < 18; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index + 1UL);

        if (!cbufTest.bAdd(strVal))
        {
            strmOut << TFWCurLn << L"Circular buffer reported not full when it is\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // The 0th element now should be 12
    if (cbufTest.tAt(0) != L"Value 13")
    {
        strmOut << TFWCurLn << L"0th element should now be value 13, but got: "
                << cbufTest.tAt(0) << L"\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Count still should be 6
    if (cbufTest.c4ElemCount() != 6)
    {
        strmOut << TFWCurLn << L"Circular buffer count should still be 6\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Make sure remove all cleans us out
    cbufTest.RemoveAll();
    if (!cbufTest.bIsEmpty())
    {
        strmOut << TFWCurLn << L"Not empty after RemoveAll\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




