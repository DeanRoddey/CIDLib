//
// FILE NAME: TestCIDLib2_Pointers.cpp
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
RTTIDecls(TTest_CntPtr1,TTestFWTest)
RTTIDecls(TTest_CntPtr2,TTestFWTest)
RTTIDecls(TTest_UniquePtr,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_CntPtr1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CntPtr11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CntPtr1::TTest_CntPtr1() :

    TTestFWTest
    (
        L"Basic Counted Pointer", L"Non-threaded tests of the TCntPtr class", 2
    )
{
}

TTest_CntPtr1::~TTest_CntPtr1()
{
}


// ---------------------------------------------------------------------------
//  TTest_CntPtr1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CntPtr1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TCntPtr<TString> cptrTest1(new TString(L"This is a test"));
    if (!cptrTest1)
    {
        // Have to give up since tests below will cause an exception
        strmOut << TFWCurLn << L"Object passed to ctor not set\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cptrTest1.c4RefCount() != 1)
    {
        // Also not likely to be worth continuing
        strmOut << TFWCurLn << L"Initial reference count != 1";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!cptrTest1->bCompare(L"This is a test"))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Object != to original one passed to ctor\n\n";
    }

    if (cptrTest1.c4RefCount() != 1)
    {
        strmOut << TFWCurLn << L"Initial reference count != 1";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Drop the reference and make sure it's now null. And it should be valid
    //  call pobjData() on it even if null.
    //
    cptrTest1.DropRef();
    if (cptrTest1.pobjData())
    {
        strmOut << TFWCurLn << L"Pointer not null after drop of reference";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  But any of the access mechanism should throw. WE need to test the
    //  const variations also.
    //
    tCIDLib::TCard4 c4ErrCnt = 0;
    try { *cptrTest1; } catch(...) { c4ErrCnt++; }
    try { cptrTest1->bIsEmpty(); } catch(...) { c4ErrCnt++; }
    TestConstDef(cptrTest1, c4ErrCnt);

    if (c4ErrCnt != 4)
    {
        strmOut << TFWCurLn << L"One or more null references did not throw";
        return tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_CntPtr1: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTest_CntPtr1::TestConstDef(const TCntPtr<TString>& cptrTest, tCIDLib::TCard4& c4ErrCnt)
{
    try { *cptrTest; } catch(...) { c4ErrCnt++; }
    try { cptrTest->bIsEmpty(); } catch(...) { c4ErrCnt++; }
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_CntPtr2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CntPtr2 : Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CntPtr2::TTest_CntPtr2() :

    TTestFWTest
    (
        L"Threaded Counted Pointer", L"Threaded tests of the TCntPtr class", 5
    )
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt)
    , m_cptrTest(new TCardinal)
{
    MarkAsLong();
}

TTest_CntPtr2::~TTest_CntPtr2()
{
}


// ---------------------------------------------------------------------------
//  TTest_CntPtr2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CntPtr2::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
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
                TString(L"Thread%(1)", TCardinal(c4Index + 1))
                , TMemberFunc<TTest_CntPtr2>(this, &TTest_CntPtr2::eTestThread)
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

    // The cumulative effect should be for the counter to be back to 1
    if (m_cptrTest.c4RefCount() != 1)
    {
        strmOut << TFWCurLn << L"The reference count ended up at " << m_cptrTest.c4RefCount()
                << L" but should have been one\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_CntPtr2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TTest_CntPtr2::eTestThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    tCIDLib::TCard4 c4Sleep = *reinterpret_cast<tCIDLib::TCard4*>(pData);
    thrThis.Sync();

    c4Sleep = (c4Sleep % 3) + 1;

    //
    //  This one gets assigned to each time around and ultimately goes out of scope
    //  and releases the last one it was pointed at.
    //
    TCntPtr<TCardinal> cptrAssign;

    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(15);
    while (TTime::enctNow() < enctEnd)
    {
        {
            TCntPtr<TCardinal> cptrCopy(m_cptrTest);
            // cptrAssign = cptrCopy;
            if (!thrThis.bSleep(c4Sleep))
                break;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}




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
    if (!uptrTest1)
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
    if (uptrTest1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Move ctor did not clear original pointer\n\n";
    }

    if (!uptrTest2)
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

