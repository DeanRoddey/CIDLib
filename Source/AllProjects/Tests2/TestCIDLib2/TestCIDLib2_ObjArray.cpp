//
// FILE NAME: TestCIDLib2_ObjArray.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains tests related to the object array class
//  test.
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
RTTIDecls(TTest_ObjArrayLambda, TTestFWTest)
// RTTIDecls(TTest_ObjArrayMoveSem, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjArrayLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjArrayLambda: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjArrayLambda::TTest_ObjArrayLambda() :

    TTestFWTest
    (
        L"Object Array Lambda", L"Object Array lambda callback tests", 3
    )
{
}

TTest_ObjArrayLambda::~TTest_ObjArrayLambda()
{
}


// ---------------------------------------------------------------------------
//  TTest_ObjArrayLambda: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjArrayLambda::eRunTest( TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up an object array with some values
    TObjArray<TSize> objaTest(16);
    TSize szVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        szVal.Set(c4Index, c4Index);
        objaTest[c4Index] = szVal;
    }

    // Iterate the original and update another one with the same values
    TObjArray<TSize> objaComp(16);
    tCIDLib::TCard4 c4LoopVal = 0;
    objaTest.ForEach
    (
        [&objaComp, &c4LoopVal](const TSize& szCur)
        {
            objaComp[c4LoopVal++] = szCur;
            return kCIDLib::True;
        }
    );

    // Compare to the original
    if (objaComp != objaTest)
    {
        strmOut << L"Iteration based copy != to original\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Do a find test
    szVal.Set(4, 4);
    if (objaTest.tFind([&szVal](const TSize& szCur) { return szVal == szCur; }) != 4)
    {
        strmOut << L"tFind did not find the test value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}



/*
// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjArrayMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjArrayMoveSem: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjArrayMoveSem::TTest_ObjArrayMoveSem() :

    TTestFWTest
    (
        L"Object Array Move Semantics", L"Object Array move ctor and assignment", 3
    )
{
}

TTest_ObjArrayMoveSem::~TTest_ObjArrayMoveSem()
{
}


// ---------------------------------------------------------------------------
//  TTest_ObjArrayMoveSem: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjArrayMoveSem::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up an object array with some values
    TObjArray<TSize> objaTest(16);
    TSize szVal;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        szVal.Set(c4Index, c4Index);
        objaTest[c4Index] = szVal;
    }

    //
    //  Force a move ctor. The new guy should have the elements and the original
    //  should have 1 element now.
    //
    TObjArray<TSize> objaMoveCtor(tCIDLib::ForceMove(objaTest));
    if (objaMoveCtor.tElemCount() != 16)
    {
        strmOut << TFWCurLn << L"Move ctor did not move items to new array\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (objaTest.tElemCount() != 1)
    {
        strmOut << TFWCurLn << L"Move ctor did not reset source item count\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure that we got the correct values over
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        szVal.Set(c4Index, c4Index);
        if (objaMoveCtor[c4Index] != szVal)
        {
            strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    return eRes;
}

*/