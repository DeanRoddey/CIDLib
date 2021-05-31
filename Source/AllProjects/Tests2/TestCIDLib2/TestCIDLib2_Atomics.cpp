//
// FILE NAME: TestCIDLib2_Atomics.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file tests various bits related to atomic operations
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
RTTIDecls(TTest_Atomics,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Atomics
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Atomics: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Atomics::TTest_Atomics() :

    TTestFWTest
    (
        L"Atomic Ops Tests", L"Test various atomic operations", 1
    )
{
}

TTest_Atomics::~TTest_Atomics()
{
}


// ---------------------------------------------------------------------------
//  TTest_Atomics: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Atomics::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Do some simple tests of the fenced get/set helpers
    {
        tCIDLib::TCard4 c4Val1 = 16;
        tCIDLib::TCard4 c4Val2 = 45;

        alignas(kCIDLib::c4CacheAlign) tCIDLib::TCard4* pc4Val1 = &c4Val1;
        alignas(kCIDLib::c4CacheAlign) tCIDLib::TCard4* pc4Val2 = &c4Val2;

        // Make sure the alignment worked
        if ((tCIDLib::TCard8(pc4Val1) % kCIDLib::c4CacheAlign) != 0)
        {
            strmOut << TFWCurLn << L"Alignment keyword did not work\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if (TAtomic::pFencedGet<tCIDLib::TCard4>(&pc4Val1) != pc4Val1)
        {
            strmOut << TFWCurLn << L"Fenced get returned invalid value\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // This should store the new and return the old
        const tCIDLib::TCard4* pc4OldPtr = pc4Val1;
        if (TAtomic::pFencedSet<tCIDLib::TCard4>(&pc4Val1, pc4Val2) != pc4OldPtr)
        {
            strmOut << TFWCurLn << L"Fenced set did not return the old pointer value\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if (*pc4Val1 != c4Val2)
        {
            strmOut << TFWCurLn << L"Fenced set did not set the correct value\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}
