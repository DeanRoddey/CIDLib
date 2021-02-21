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
RTTIDecls(TTest_MemberPtr1,TTestFWTest)
RTTIDecls(TTest_UniquePtr,TTestFWTest)
RTTIDecls(TTest_WeakPtr1,TTestFWTest)
RTTIDecls(TTest_WeakPtr2,TTestFWTest)
RTTIDecls(TTest_WeakPtr3,TTestFWTest)


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
struct TTest
{
    static tCIDLib::TCard4 c4Count;
    TTest()
    {
        c4Count++;
    }

    ~TTest()
    {
        c4Count--;
    }
};

tCIDLib::TCard4 TTest::c4Count = 0;

tTestFWLib::ETestRes
TTest_CntPtr1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    TCntPtr<TString> cptrTest1(new TString(L"This is a test"));
    if (!cptrTest1)
    {
        // Have to give up since tests below will cause an exception
        strmOut << TFWCurLn << L"Object passed to ctor not set\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cptrTest1.c4StrongCount() != 1)
    {
        // Also not likely to be worth continuing
        strmOut << TFWCurLn << L"Initial reference count != 1";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!cptrTest1->bCompare(L"This is a test"))
    {
        strmOut << TFWCurLn << L"Object != to original one passed to ctor\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (cptrTest1.c4StrongCount() != 1)
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
    try { if (cptrTest1->bIsEmpty()) {}; } catch(...) { c4ErrCnt++; }
    TestConstDef(cptrTest1, c4ErrCnt);

    if (c4ErrCnt != 4)
    {
        strmOut << TFWCurLn << L"One or more null references did not throw";
        return tTestFWLib::ETestRes::Failed;
    }

    // Test a custom deleter for an array scenario
    {
        TCntPtr<TTest> cptrList
        (
            new TTest[5]
            , [](TTest* pT) -> tCIDLib::TVoid { delete [] pT; }
        );

		cptrList.DropRef();

        // The test class' count should be back to zero
        if (TTest::c4Count != 0)
        {
            strmOut << TFWCurLn << L"Custom array deleter did not work";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // Do a move test, which should leave the original null
    cptrTest1.SetPointer(new TString(L"Testing"));
    TCntPtr<TString> cptrTest2 = tCIDLib::ForceMove(cptrTest1);

    if (cptrTest1)
    {
        strmOut << TFWCurLn << L"Move did not null the original";
        return tTestFWLib::ETestRes::Failed;
    }

    if ((cptrTest1.c4StrongCount() != 0) || (cptrTest1.c4WeakCount() != 0))
    {
        strmOut << TFWCurLn << L"Move did not leave src with zero counts";
        return tTestFWLib::ETestRes::Failed;
    }

    // And the new one should have the string
    if (*cptrTest2 != L"Testing")
    {
        strmOut << TFWCurLn << L"Moved to object did not have the original value";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


// ---------------------------------------------------------------------------
//  TTest_CntPtr1: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTest_CntPtr1::TestConstDef(const TCntPtr<TString>& cptrTest, tCIDLib::TCard4& c4ErrCnt)
{
    try { *cptrTest; } catch(...) { c4ErrCnt++; }
    try { if (cptrTest->bIsEmpty()) {}; } catch(...) { c4ErrCnt++; }
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
                TString(L"TestCntPtr2Thread%(1)", TCardinal(c4Index + 1))
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
    if (m_cptrTest.c4StrongCount() != 1)
    {
        strmOut << TFWCurLn << L"The reference count ended up at " << m_cptrTest.c4StrongCount()
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
//  CLASS: TTest_MemberPtr1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MemberPtr11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MemberPtr1::TTest_MemberPtr1() :

    TTestFWTest
    (
        L"Member Pointer", L"Tests of the TMemberPtr class", 2
    )
{
}

TTest_MemberPtr1::~TTest_MemberPtr1()
{
}


// ---------------------------------------------------------------------------
//  TTest_MemberPtr1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MemberPtr1::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Create one of our test wrappers and copy and assign it a few ways, getting
    //  back to the single one at the end. Each one creates two objects, so the count
    //  at the end should be two, and the zero once back outside the scope.
    //
    {
        TTestData::s_c4Count = 0;
        TTestWrapper wrapTest1;
        {
            TTestWrapper wrapTest2(wrapTest1);
            TTestWrapper wrapTest3(wrapTest2);

            TTestWrapper wrapTest4  = wrapTest1;
            TTestWrapper wrapTest5  = wrapTest3;
        }

        if (TTestData::s_c4Count != 2)
        {
            strmOut << TFWCurLn << L"Member pointer test count was " << TTestData::s_c4Count
                    << L" but expected 2\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }
    if (TTestData::s_c4Count != 0)
    {
        strmOut << TFWCurLn << L"Member pointer test count was " << TTestData::s_c4Count
                << L" but expected 0\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Do the same thing but with custom delete/copy handlers
    {
        TTestData::s_c4Count = 0;
        TTestWrapperCD wrapTest1;
        {
            TTestWrapperCD wrapTest2(wrapTest1);
            TTestWrapperCD wrapTest3(wrapTest2);

            TTestWrapperCD wrapTest4  = wrapTest1;
            TTestWrapperCD wrapTest5  = wrapTest3;
        }

        if (TTestData::s_c4Count != 2)
        {
            strmOut << TFWCurLn << L"Member pointer test count was " << TTestData::s_c4Count
                    << L" but expected 2\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }
    if (TTestData::s_c4Count != 0)
    {
        strmOut << TFWCurLn << L"Member pointer test count was " << TTestData::s_c4Count
                << L" but expected 0\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}

tCIDLib::TCard4 TTest_MemberPtr1::TTestData::s_c4Count;


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


// ---------------------------------------------------------------------------
//  CLASS: TTest_WeakPtr1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_WeakPtr11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_WeakPtr1::TTest_WeakPtr1() :

    TTestFWTest
    (
        L"Basic Weak Pointer", L"Non-threaded tests of the TWeakPtr class", 2
    )
{
}

TTest_WeakPtr1::~TTest_WeakPtr1()
{
}


// ---------------------------------------------------------------------------
//  TTest_WeakPtr1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_WeakPtr1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    // Create a counted pointer to act as the anchor for the test
    TCntPtr<TString> cptrTest1(new TString(L"This is a test"));
    if (!cptrTest1)
    {
        // Have to give up since tests below will cause an exception
        strmOut << TFWCurLn << L"Object passed to ctor not set\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // It should have a single strong ref count and zero weak count
    if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 0))
    {
        strmOut << TFWCurLn << L"Cnt ptr initial ref counts were wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    {
		//
		//  Create a weak pointer from the counted pointer and now we should have counts
		//  of one and one.
		//
        TWeakPtr<TString> wptrTest(cptrTest1);
        if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 1))
        {
            strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong after weak obtained\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

		// And do a second weak and check teh counts again
		{
			TWeakPtr<TString> wptrTest2(cptrTest1);
			if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 2))
			{
				strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong after two weaks obtained\n\n";
				return tTestFWLib::ETestRes::Failed;
			}
		}

		// It should be back to one and one now
		if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 1))
		{
			strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong after 2nd weak dropped\n\n";
			return tTestFWLib::ETestRes::Failed;
		}
    }

	// And we should be back to 1 and zero
	if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 0))
	{
		strmOut << TFWCurLn << L"Cnt ptr initial ref counts were wrong after weaks dropped\n\n";
		return tTestFWLib::ETestRes::Failed;
	}

	// Get a weak and convert to a counted
	{
		TWeakPtr<TString> wptrTest(cptrTest1);
		if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 1))
		{
			strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong after weak obtained\n\n";
			return tTestFWLib::ETestRes::Failed;
		}

		TCntPtr<TString> cptrTest2 = wptrTest.cptrGet();
		if ((cptrTest1.c4StrongCount() != 2) || (cptrTest1.c4WeakCount() != 1))
		{
			strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong after weak converted\n\n";
			return tTestFWLib::ETestRes::Failed;
		}

		// We should see the same via the new one
		if ((cptrTest2.c4StrongCount() != 2) || (cptrTest2.c4WeakCount() != 1))
		{
			strmOut << TFWCurLn << L"Cnt ptr ref counts were wrong via converted strong\n\n";
			return tTestFWLib::ETestRes::Failed;
		}

		// Make sure we got the value
		if (*cptrTest2 != L"This is a test")
		{
			strmOut << TFWCurLn << L"Converted cnt ptr does not have original value\n\n";
			return tTestFWLib::ETestRes::Failed;
		}

		// Compare against original as well
		if (*cptrTest1 != *cptrTest2)
		{
			strmOut << TFWCurLn << L"Converted cnt ptr value != to original pointer value\n\n";
			return tTestFWLib::ETestRes::Failed;
		}
	}

	// And we should be back to 1 and zero
	if ((cptrTest1.c4StrongCount() != 1) || (cptrTest1.c4WeakCount() != 0))
	{
		strmOut << TFWCurLn << L"Cnt ptr initial ref counts were wrong after other refs dropped\n\n";
		return tTestFWLib::ETestRes::Failed;
	}

    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_WeakPtr2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_WeakPtr21: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_WeakPtr2::TTest_WeakPtr2() :

    TTestFWTest
    (
        L"More Weak Pointer", L"More non-threaded tests of the TWeakPtr class", 2
    )
{
}

TTest_WeakPtr2::~TTest_WeakPtr2()
{
}


// ---------------------------------------------------------------------------
//  TTest_WeakPtr2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_WeakPtr2::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
     //
    //  Create a counted pointer and a weak, then reset the counted pointer, which
    //  should then leave the weak pointer such that we get a null if we convert it.
    //
    {
        TCntPtr<TString> cptrTest(new TString(L"This is a test"));
        if (!cptrTest || (*cptrTest != L"This is a test"))
        {
            strmOut << TFWCurLn << L"Object passed to ctor not set\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // It should have a single strong ref count and zero weak count
        if ((cptrTest.c4StrongCount() != 1) || (cptrTest.c4WeakCount() != 0))
        {
            strmOut << TFWCurLn << L"Cnt ptr initial ref counts were wrong\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        TWeakPtr<TString> wptrTest(cptrTest);
        if ((cptrTest.c4StrongCount() != 1) || (cptrTest.c4WeakCount() != 1))
        {
            strmOut << TFWCurLn << L"Ref counts were wrong after getting weak ref\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // After the drop we should have 1 and 0 and a null pointer
        cptrTest.DropRef();
        if ((cptrTest.c4StrongCount() != 1)
        ||  (cptrTest.c4WeakCount() != 0)
        ||  (cptrTest.pobjData() != nullptr))
        {
            strmOut << TFWCurLn << L"Dropped counted pointer had bad state\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // And now try to get another counted from the abandoned weak
        TCntPtr<TString> cptrTest2 = wptrTest.cptrGet();
        if (cptrTest2.pobjData() != nullptr)
        {
            strmOut << TFWCurLn << L"Converted strong pointer should have been null\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Test it the other way just for funzies
        if (cptrTest2)
        {
            strmOut << TFWCurLn << L"Converted strong pointer should have been null\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if ((cptrTest.c4StrongCount() != 1) || (cptrTest.c4WeakCount() != 0))
        {
            strmOut << TFWCurLn << L"Failed lock pointer had bad counts\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  Copy the counted pointer to make sure a null doesn't cause issues.
        //
        TCntPtr<TString> cptrTest3(cptrTest2);
        if ((cptrTest3.c4StrongCount() != 2) || (cptrTest3.c4WeakCount() != 0))
        {
            strmOut << TFWCurLn << L"Ref counts were wrong after 2nd null strong\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    {
        TCntPtr<TString> cptrTest(new TString(L"This is a test"));

        // Get a weak pointer and copy and asign it to others and check the count
        {
            TWeakPtr<TString> wptrTest(cptrTest);
            TWeakPtr<TString> wptrTest2 = wptrTest;
            TWeakPtr<TString> wptrTest3(wptrTest);

            if (cptrTest.c4WeakCount() != 3)
            {
                strmOut << TFWCurLn << L"Copy and assign of weak pointer didn't adjust count\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
        }

        if (cptrTest.c4WeakCount() != 0)
        {
            strmOut << TFWCurLn << L"Dtors of weak pointer didn't adjust count back to 0\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_WeakPtr3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_WeakPtr3 : Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_WeakPtr3::TTest_WeakPtr3() :

    TTestFWTest
    (
        L"Threaded weak Pointer", L"Threaded tests of the TWeakPtr class", 5
    )
    , m_colThreads(tCIDLib::EAdoptOpts::Adopt)
    , m_wptrTest()
{
    MarkAsLong();
}

TTest_WeakPtr3::~TTest_WeakPtr3()
{
}


// ---------------------------------------------------------------------------
//  TTest_WeakPtr3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_WeakPtr3::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    // Set up a counted pointer and then set our weak pointer member from it
    TCntPtr<TSafeCard4Counter> cptrVal(new TSafeCard4Counter(0));
    m_wptrTest = cptrVal;

    // Gen up our threads
    const tCIDLib::TCard4 c4ThreadCnt = 32;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        m_colThreads.Add
        (
            new TThread
            (
                TString(L"TestWeakPtr3Thread%(1)", TCardinal(c4Index + 1))
                , TMemberFunc<TTest_WeakPtr3>(this, &TTest_WeakPtr3::eTestThread)
            )
        );
    }

    // Start them, giving each their own index
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        m_colThreads[c4Index]->Start(&c4Index);

    // Now wait for them to stop
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        m_colThreads[c4Index]->eWaitForDeath(kCIDLib::c4OneSecond * 30);

    // The cumulative effect should be for the counter to be back to 1 and 1
    if ((cptrVal.c4StrongCount() != 1)
    ||  (cptrVal.c4WeakCount() != 1))
    {
        strmOut << TFWCurLn << L"The ref counts ended up at " << cptrVal.c4StrongCount()
                << L"/" << cptrVal.c4WeakCount()
                << L" but should have been 1/1\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And the value should be back to 0
    if (*cptrVal != 0UL)
    {
        strmOut << TFWCurLn << L"The ref counts ended up at " << cptrVal.c4StrongCount()
                << L"/" << cptrVal.c4WeakCount()
                << L" but should have been 1/1\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


// ---------------------------------------------------------------------------
//  TTest_WeakPtr3: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TTest_WeakPtr3::eTestThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    tCIDLib::TCard4 c4Sleep = *reinterpret_cast<tCIDLib::TCard4*>(pData);
    thrThis.Sync();

    c4Sleep = (c4Sleep % 3) + 1;

    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(15);
    while (TTime::enctNow() < enctEnd)
    {
        {
            TCntPtr<TSafeCard4Counter> cptrTest = m_wptrTest.cptrGet();

            // Just for extra testing, create various other refs that will go away again
            TWeakPtr<TSafeCard4Counter> wptr2(m_wptrTest);
            TWeakPtr<TSafeCard4Counter> wptr3 = m_wptrTest;
            TCntPtr<TSafeCard4Counter> cptrTest2 = wptr3.cptrGet();
            TCntPtr<TSafeCard4Counter> cptrTest3 = cptrTest;

            (*cptrTest)++;
            const tCIDLib::TBoolean bStop = !thrThis.bSleep(c4Sleep);
            (*cptrTest)--;

            if (bStop)
                break;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}
