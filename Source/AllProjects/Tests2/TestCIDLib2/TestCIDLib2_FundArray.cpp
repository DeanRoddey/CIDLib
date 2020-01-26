//
// FILE NAME: TestCIDLib2_FundArray.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 1/25/2020
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
//  This file contains tests related to the fundamental array class.
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
RTTIDecls(TTest_FundArrayMove, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_FundArrayMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_FundArrayMove: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_FundArrayMove::TTest_FundArrayMove() :

    TTestFWTest
    (
        L"FundArray move", L"Fundumental array move support tests", 3
    )
{
}

TTest_FundArrayMove::~TTest_FundArrayMove()
{
}


// ---------------------------------------------------------------------------
//  TTest_FundArrayMove: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_FundArrayMove::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create an array and set some values
    TFundArray<tCIDLib::TCard4> fcolTest(8);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
        fcolTest[c4Index] = c4Index;

    // Move construct one from that
    TFundArray<tCIDLib::TCard4> fcolTest2(tCIDLib::ForceMove(fcolTest));

    // Source will now have 1 value and the target has the 8 originasl
    if ((fcolTest.c4ElemCount() != 1) || (fcolTest[0] != 0))
    {
        strmOut << TFWCurLn << L"Source was not updated after move ctor\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (fcolTest2.c4ElemCount() != 8)
    {
        strmOut << TFWCurLn << L"Elements were not moved to target by move ctor\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        if (fcolTest2[c4Index] != c4Index)
        {
            strmOut << TFWCurLn << L"Elements in wrong order after move ctor\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // No point doing this if we failed above
    if (eRes == tTestFWLib::ETestRes::Success)
    {
        // Do an assign move
        TFundArray<tCIDLib::TCard4> fcolTest3(2);
        fcolTest3 = tCIDLib::ForceMove(fcolTest2);

        // Source will now have the target's 2 values and the target has the 8 originasl
        if ((fcolTest2.c4ElemCount() != 2) || (fcolTest2[0] != 0))
        {
            strmOut << TFWCurLn << L"Source was not updated after move operator\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        if (fcolTest3.c4ElemCount() != 8)
        {
            strmOut << TFWCurLn << L"Elements were not moved to target by move operator\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
        {
            if (fcolTest3[c4Index] != c4Index)
            {
                strmOut << TFWCurLn << L"Elements in wrong order after move operator\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
    }

    return eRes;
}

