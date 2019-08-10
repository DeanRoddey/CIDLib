//
// FILE NAME: TestCIDLib2_ObjLocker.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/09/2019
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
//  This file contains tests related to the TObjLock and TObjLocker classes.
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
RTTIDecls(TTest_ObjLocker1,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjLocker1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ObjLocker1 : Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ObjLocker1::TTest_ObjLocker1() :

    TTestFWTest(L"Object Locker", L"Basic tests of the TObjLocker class", 2)
{
}

TTest_ObjLocker1::~TTest_ObjLocker1()
{
}


// ---------------------------------------------------------------------------
//  TTest_ObjLocker1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ObjLocker1::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TObjLocker<TArea>   olockrTest(10, 12, 14, 16);
    {
        TObjLock<TArea> olockTest = olockrTest.olockGet(50);
        if (!olockTest)
        {
            strmOut << TFWCurLn << L"Failed to get a lock\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if (*olockTest != TArea(10, 12, 14, 16))
        {
            strmOut << TFWCurLn << L"The locked object has the wrong value\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        *olockTest = TArea(20, 22, 24, 26);
        if (*olockTest != TArea(20, 22, 24, 26))
        {
            strmOut << TFWCurLn << L"Setting the object via the lock didn't work\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        //
        //  Since we are on the same thread, this will be a recursive lock, so
        //  it should throw.
        //
        try
        {
            TObjLock<TArea> olockTest2 = olockrTest.olockTryGet(50);
            strmOut << TFWCurLn << L"Should not have gotten a recursive lock\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        catch(...)
        {
        }

        try
        {
            TObjLock<TArea> olockTest2 = olockrTest.olockGet(50);
            strmOut << TFWCurLn << L"Should not have gotten a recursive lock\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        catch(...)
        {
        }
    }

    // Now we should be able to get a lock again
    {
        TObjLock<TArea> olockTest = olockrTest.olockTryGet(50);
        if (!olockTest)
        {
            strmOut << TFWCurLn << L"Failed to get a lock\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        // We can copy the lock, but it's a unique type pointer
        TObjLock<TArea> olockTest2(tCIDLib::ForceMove(olockTest));

        if (olockTest)
        {
            strmOut << TFWCurLn << L"Source lock should be null now\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        try
        {
            //
            //  A deref on the original should throw now. Do a read operation
            //  here so we are sure we caught the right error, and not a null
            //  ref error.
            //
            if (*olockTest != TArea())
            {
            }
            strmOut << TFWCurLn << L"Source lock should be null now\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
        catch(...)
        {
        }

        if (!olockTest2)
        {
            strmOut << TFWCurLn << L"Target lock is still null\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if (*olockTest2 != TArea(20, 22, 24, 26))
        {
            strmOut << TFWCurLn << L"Getting value through copy of lock failed\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}

