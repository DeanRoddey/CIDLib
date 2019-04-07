//
// FILE NAME: TestCIDLib2_LogLimiter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/2018
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
//  This file contains tests related to the log limiter class.
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
RTTIDecls(TTest_LogLimiter,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_LogLimiter
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_LogLimiter: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_LogLimiter::TTest_LogLimiter() :

    TTestFWTest
    (
        L"Log Limiter", L"Tests of the TLogLimiter class", 3
    )
{
}

TTest_LogLimiter::~TTest_LogLimiter()
{
}


// ---------------------------------------------------------------------------
//  TTest_LogLimiter: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_LogLimiter::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Get an object and set the threshold to 1 seconds so that we can do this quickly
    TLogLimiter loglimTest(1);

    // Create some error objects to test with
    TLogEvent errTest1
    (
        L"MyFacility"
        , L"SomeFile"
        , 1
        , 1000
        , L"Some error"
        , L"Some aux text"
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Duplicate
    );

    // Try to log this guy, which should do it
    if (!loglimTest.bLogErr(errTest1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Initially constructed error was not logged\n\n";
    }

    // Do it again a few times and it should not be
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 3; c4Index++)
    {
        if (loglimTest.bLogErr(errTest1))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Immediate relog still was logged\n\n";
        }
    }

    // Pause for a bit over a second and it should work again
    TThread::Sleep(1100);
    if (!loglimTest.bLogErr(errTest1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Error not logged after threshold wait\n\n";
    }


    //
    //  Now do a loop of 128, where we just increment the error code to generate new
    //  unique errors. They should all be logged since they are all new ones.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 128; c4Index++)
    {
        errTest1.errcId(errTest1.errcId() + 1);
        if (!loglimTest.bLogErr(errTest1))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"New unique error not logged\n\n";
            break;
        }
    }

    return eRes;
}

