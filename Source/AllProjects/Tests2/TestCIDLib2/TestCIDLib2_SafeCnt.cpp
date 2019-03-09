//
// FILE NAME: TestCIDLib2_SafeCounters.cpp
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
        L"Threaded Safe Counter", L"Threaded tests of the safe counter classes", 5
    )
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt)
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

    // Gen up our threads
    const tCIDLib::TCard4 c4ThreadCnt = 32;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        m_colThreads.Add
        (
            new TThread
            (
                TString(L"CntPtrTestThread%(1)", TCardinal(c4Index + 1))
                , TMemberFunc<TTest_SafeCnt1>(this, &TTest_SafeCnt1::eTestThread)
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
//  TTest_SafeCnt1: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TTest_SafeCnt1::eTestThread(TThread& thrThis, tCIDLib::TVoid* pData)
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

