//
// FILE NAME: TestCIDLib2_SafeCounters.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2019
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
//  This file contains tests related to the safe counter tests
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
RTTIDecls(TTest_SafeCnt1,TTestFWTest)
RTTIDecls(TTest_SafeCnt2,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_SafeCnt1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SafeCnt1 : Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SafeCnt1::TTest_SafeCnt1() :

    TTestFWTest
    (
        L"Safe Counter", L"Basic tests of the safe counter classes", 3
    )
{
}

TTest_SafeCnt1::~TTest_SafeCnt1()
{
}


// ---------------------------------------------------------------------------
//  TTest_SafeCnt1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SafeCnt1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create a signed and unsigned counter to play with, with initial values of 1
    TSafeCard4Counter scntCard(1);
    TSafeInt4Counter  scntInt(1);

    if ((scntCard.c4Value() != 1) || (scntInt.i4Value() != 1))
    {
        strmOut << TFWCurLn << L"Initial counter value was not set\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Bump both and check again, this is post-inc so they should return the original value
    if ((scntCard++ != 1) || (scntInt++ != 1))
    {
        strmOut << TFWCurLn << L"Post increment didn't return original value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4Value() != 2) || (scntInt.i4Value() != 2))
    {
        strmOut << TFWCurLn << L"Post increment did not bump value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Pre-increment should return the new value
    if ((++scntCard != 3) || (++scntInt != 3))
    {
        strmOut << TFWCurLn << L"Pre increment didn't return new value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4Value() != 3) || (scntInt.i4Value() != 3))
    {
        strmOut << TFWCurLn << L"Pre  increment did not bump value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Do the same tests for decrement
    if ((scntCard-- != 3) || (scntInt-- != 3))
    {
        strmOut << TFWCurLn << L"Post decrement didn't return original value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4Value() != 2) || (scntInt.i4Value() != 2))
    {
        strmOut << TFWCurLn << L"Post decrement did not bump value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Pre-decrement should return the new value
    if ((--scntCard != 1) || (--scntInt != 1))
    {
        strmOut << TFWCurLn << L"Pre decrement didn't return new value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4Value() != 1) || (scntInt.i4Value() != 1))
    {
        strmOut << TFWCurLn << L"Pre decrement did not bump value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Add test add/subtract
    if ((scntCard.c4AddTo(5) != 6) || (scntInt.i4AddTo(2) != 3))
    {
        strmOut << TFWCurLn << L"Addition not change value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4SubFrom(3) != 3) || (scntInt.i4SubFrom(5) != -2))
    {
        strmOut << TFWCurLn << L"Subtraction not change value correctly\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // And test setting the value, which should return the previous value
    if ((scntCard.c4SetValue(4) != 3) || (scntInt.i4SetValue(-10) != -2))
    {
        strmOut << TFWCurLn << L"Direct value set did not return the old value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if ((scntCard.c4Value() != 4) || (scntInt.i4Value() != -10))
    {
        strmOut << TFWCurLn << L"Newly set value did not come back\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_SafeCnt2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SafeCnt2 : Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SafeCnt2::TTest_SafeCnt2() :

    TTestFWTest
    (
        L"Threaded Safe Counter", L"Threaded tests of the safe counter classes", 5
    )
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt)
{
}

TTest_SafeCnt2::~TTest_SafeCnt2()
{
}


// ---------------------------------------------------------------------------
//  TTest_SafeCnt2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SafeCnt2::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Gen up our threads
    const tCIDLib::TCard4 c4ThreadCnt = 32;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        m_colThreads.Add
        (
            new TThread
            (
                TString(L"CntPtrTestThread%(1)", TCardinal(c4Index + 1))
                , TMemberFunc<TTest_SafeCnt2>(this, &TTest_SafeCnt2::eTestThread)
            )
        );
    }

    // Start them, giving each their own index
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        m_colThreads[c4Index]->Start(&c4Index);

    // Now wait for them to stop
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        m_colThreads[c4Index]->eWaitForDeath(kCIDLib::c4OneSecond * 30);
    }

    // The cumulative effect should be for the counter to be back to zero
    if (m_scntCard.c4Value() != 0)
    {
        strmOut << TFWCurLn << L"The unsigned safe counter did not return to zero\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (m_scntInt.i4Value() != 0)
    {
        strmOut << TFWCurLn << L"The signed safe counter did not return to zero\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_SafeCnt2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TTest_SafeCnt2::eTestThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    tCIDLib::TCard4 c4Sleep = *reinterpret_cast<tCIDLib::TCard4*>(pData) + 5;
    thrThis.Sync();

    c4Sleep = (c4Sleep % 3) + 1;

    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(15);
    while (TTime::enctNow() < enctEnd)
    {
        m_scntCard.c4Inc();
        m_scntInt.i4Inc();
        if (!thrThis.bSleep(c4Sleep))
            break;
        m_scntCard.c4Dec();
        m_scntInt.i4Dec();
    }
    return tCIDLib::EExitCodes::Normal;
}

