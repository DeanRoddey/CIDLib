//
// FILE NAME: TestCIDLib2_Pointers.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2019
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains tests related to the various smart pointer type classes.
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
RTTIDecls(TTest_UniquePtr,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_UniquePtr
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_UniquePtr1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_UniquePtr::TTest_UniquePtr() :

    TTestFWTest
    (
        L"Unique Pointer", L"Tests of the TUniquePtr class", 2
    )
{
}

TTest_UniquePtr::~TTest_UniquePtr()
{
}


// ---------------------------------------------------------------------------
//  TTest_UniquePtr: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_UniquePtr::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create a unique pointer with a string and test that it got set
    TUniquePtr<TString> uptrTest1(new TString(L"This is a test"));
    if (!uptrTest1.pData())
    {
        // Have to give up since tests below will cause an exception
        strmOut << TFWCurLn << L"Object passed to ctor not set\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!uptrTest1->bCompare(L"This is a test"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Object != to original one passed to ctor\n\n";
    }

    // We can only copy by forced move
    TUniquePtr<TString> uptrTest2(tCIDLib::ForceMove(uptrTest1));

    // And the original should be empty now and the new one should have the value
    if (uptrTest1.pData())
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Move ctor did not clear original pointer\n\n";
    }

    if (!uptrTest2.pData())
    {
        // Have to give up since tests below will cause an exception
        strmOut << TFWCurLn << L"Move ctor did not get pointer to 2nd pointer\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!uptrTest2->bCompare(L"This is a test"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Moved object not equal to original value\n\n";
    }

    return eRes;
}


