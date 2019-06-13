//
// FILE NAME: TestCIDLib2_Deque.cpp
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
//  This file contains tests related to the by by value Deque collection, which
//  indirectly tests the basic doubly linked list class that it uses for its
//  implementation, which doubly indirectly tests the guts of a number of other
//  collections that also use it.
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
RTTIDecls(TTest_DequeMoveSem, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_DequeMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_DequeMoveSem: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_DequeMoveSem::TTest_DequeMoveSem() :

    TTestFWTest
    (
        L"Deque Move Semantics", L"Deque move ctor and assignment", 3
    )
{
}

TTest_DequeMoveSem::~TTest_DequeMoveSem()
{
}


// ---------------------------------------------------------------------------
//  TTest_DequeMoveSem: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_DequeMoveSem::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a deque with some objects
    TDeque<TString> colTest;
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.objAddAtTop(strVal);
    }

    //
    //  Force a move ctor. The new guy should have the elements and the original
    //  should have none.
    //
    TDeque<TString> colMoveCtor(tCIDLib::ForceMove(colTest));
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
    {
        tCIDLib::TCard4 c4Index = 0;
        TDeque<TString>::TCursor cursTest(&colMoveCtor);
        for (; cursTest; ++cursTest)
        {
            strVal = L"Value ";
            strVal.AppendFormatted(c4Index);
            if (*cursTest != strVal)
            {
                strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
            c4Index++;
        }
    }

    //
    //  Create a thread safe one and do a forced assign. We should still be
    //  thread safe afterwards since only element content is moved.
    //
    TDeque<TString> colSafe(tCIDLib::EMTStates::Safe);
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
    {
        tCIDLib::TCard4 c4Index = 0;
        TDeque<TString>::TCursor cursSafe(&colSafe);
        for (; cursSafe; ++cursSafe)
        {
            strVal = L"Value ";
            strVal.AppendFormatted(c4Index);
            if (*cursSafe != strVal)
            {
                strmOut << TFWCurLn << L"Move ctor copy is out of sequence\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
            c4Index++;
        }
    }

    // Move construct one from a thread safe src, which should also be thread safe
    TDeque<TString> colNewSafe(tCIDLib::ForceMove(colSafe));
    if (colNewSafe.eMTState() != tCIDLib::EMTStates::Safe)
    {
        strmOut << TFWCurLn<< L"Move ctor did not copy over thread safety\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}

