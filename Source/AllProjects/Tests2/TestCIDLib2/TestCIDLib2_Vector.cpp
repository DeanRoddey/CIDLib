//
// FILE NAME: TestCIDLib2_Vector.cpp
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
//  This file contains tests related to the by value vector collection.
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
RTTIDecls(TTest_VectorLambda, TTestFWTest)
RTTIDecls(TTest_VectorMoveSem, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorLambda: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorLambda::TTest_VectorLambda() :

    TTestFWTest
    (
        L"Vector Lambda", L"Vector lambda callback tests", 3
    )
{
}

TTest_VectorLambda::~TTest_VectorLambda()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorLambda: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorLambda::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a ref fector with some objects
    TVector<TString> colTest(16);
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.objAdd(strVal);
    }

    // Iterate the values and check them
    tCIDLib::TCard4 c4LoopVal = 0;
    colTest.ForEach
    (
        [&c4LoopVal, &strVal, &eRes, &strmOut](const TString& strCur)
        {
            strVal = L"Value ";
            strVal.AppendFormatted(c4LoopVal++);
            if (strVal != strCur)
            {
                strmOut << TFWCurLn << L"ForEach callback got out of sequence value\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
            return kCIDLib::True;
        }
    );

    // Do a find test
    strVal = L"Value ";
    strVal.AppendFormatted(2UL);
    if (colTest.tFind([&strVal](const TString& strCur) { return strVal == strCur; }) != 2)
    {
        strmOut << TFWCurLn << L"tFind did not find the test value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_VectorMoveSem: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_VectorMoveSem::TTest_VectorMoveSem() :

    TTestFWTest
    (
        L"Vector Move Semantics", L"Vector move ctor and assignment", 3
    )
{
}

TTest_VectorMoveSem::~TTest_VectorMoveSem()
{
}


// ---------------------------------------------------------------------------
//  TTest_VectorMoveSem: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_VectorMoveSem::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a vector with some objects
    tCIDLib::TStrList colTest(16);
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.objAdd(strVal);
    }

    //
    //  Force a move ctor. The new guy should have the elements and the original
    //  should have none.
    //
    tCIDLib::TStrList colMoveCtor(tCIDLib::ForceMove(colTest));
    if (colMoveCtor.c4ElemCount() != 16)
    {
        strmOut << TFWCurLn << L"Move ctor did not move elements to new collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And the original should be empty now
    if (colTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move ctor did not empty source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure that we got the correct values over
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        if (colMoveCtor[c4Index] != strVal)
        {
            strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    //
    //  Create a thread safe vector and do a forced assign. We should still be
    //  thread safe afterwards since only element content is moved.
    //
    tCIDLib::TStrList colSafe(tCIDLib::EMTStates::Safe);
    colSafe = tCIDLib::ForceMove(colMoveCtor);
    if (!colSafe.bIsMTSafe())
    {
        strmOut << TFWCurLn << L"Move operator overwrote thread safety\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // The move one should now be empty
    if (colMoveCtor.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Move operator did not empty source collection\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And make sure that we got the correct values over
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        if (colSafe[c4Index] != strVal)
        {
            strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    // Move construct one from a thread safe src, which should also be thread safe
    tCIDLib::TStrList colNewSafe(tCIDLib::ForceMove(colSafe));
    if (colNewSafe.eMTState() != tCIDLib::EMTStates::Safe)
    {
        strmOut << TFWCurLn << L"Move ctor did not copy over thread safety\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}

