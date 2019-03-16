//
// FILE NAME: TestCIDLib2_FlagJanitors.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2019
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains tests related to the very fundamental flag janitors, which
//  save and restore the value of some fundamental value on a scoped basis.
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
RTTIDecls(TTest_FlagJanitor,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_FlagJanitor1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_FlagJanitor1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_FlagJanitor::TTest_FlagJanitor() :

    TTestFWTest
    (
        L"Flag Janitors", L"Tests of the fundamental flag janitor classes", 0
    )
{
}

TTest_FlagJanitor::~TTest_FlagJanitor()
{
}


// ---------------------------------------------------------------------------
//  TTest_FlagJanitor: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_FlagJanitor::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Do the pointer janitor, which has const and non-const variations
    {
        tCIDLib::TBoolean b1 = kCIDLib::True;
        const tCIDLib::TBoolean b2 = kCIDLib::True;
        tCIDLib::TBoolean b3 = kCIDLib::False;
        const tCIDLib::TBoolean b4 = kCIDLib::False;

        tCIDLib::TBoolean* pB1 = &b1;
        const tCIDLib::TBoolean* pB2 = &b2;
        tCIDLib::TBoolean* pB3 = &b3;
        const tCIDLib::TBoolean* pB4 = &b4;

        // Do the non-const version
        {
            TPtrJanitor<tCIDLib::TBoolean> janTest(pB1, pB3);

            // The value should be false, since we have B3 now
            if (*pB1 != kCIDLib::False)
            {
                strmOut << TFWCurLn
                        << L"Non-const pointer janitor did not update pointer\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (*janTest != kCIDLib::False)
            {
                strmOut << TFWCurLn
                        << L"Non-const pointer janitor did not take new value\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // Now it should be back to true
        if (*pB1 != kCIDLib::True)
        {
            strmOut << TFWCurLn
                    << L"Non-const pointer janitor did not restore old value\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }


        // Do the const version
        {
            TPtrJanitor<const tCIDLib::TBoolean> janTest(pB2, pB4);

            // The value should be false, since we have B4 now
            if (*pB4 != kCIDLib::False)
            {
                strmOut << TFWCurLn
                        << L"Const pointer janitor did not update pointer\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }

            if (*janTest != kCIDLib::False)
            {
                strmOut << TFWCurLn
                        << L"Const pointer janitor did not take new value\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // Now it should be back to true
        if (*pB2 != kCIDLib::True)
        {
            strmOut << TFWCurLn
                    << L"Const pointer janitor did not restore old value\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}
