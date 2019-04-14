//
// FILE NAME: TestCIDLib2_FundVector.cpp
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
//  This file contains tests related to the fundamental vector classes.
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
RTTIDecls(TTest_FundVectorLambda, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_FundVectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_FundVectorLambda: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_FundVectorLambda::TTest_FundVectorLambda() :

    TTestFWTest
    (
        L"FundVector Lambda", L"FundVector lambda callback tests", 3
    )
{
}

TTest_FundVectorLambda::~TTest_FundVectorLambda()
{
}


// ---------------------------------------------------------------------------
//  TTest_FundVectorLambda: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_FundVectorLambda::eRunTest(TTextStringOutStream&  strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a ref fector with some values
    TFundVector<tCIDLib::TCard4> fcolTest(8);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
        fcolTest.c4AddElement(c4Index);


    // Loop through them and compare the values
    TFundVector<tCIDLib::TCard4> fcolComp(8);
    tCIDLib::TCard4 c4TestVal = 0;
    fcolTest.ForEach
    (
        [&fcolComp, &c4TestVal, &eRes, &strmOut](const tCIDLib::TCard4 c4Cur)
        {
            if (c4Cur != c4TestVal)
            {
                strmOut << TFWCurLn << L"ForEach callback value mismatch\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
                return kCIDLib::False;
            }
            c4TestVal++;
            return kCIDLib::True;
        }
    );


    // Do a find
    tCIDLib::TCard4 c4ToFind = 4;
    if (fcolTest.tFind([c4ToFind](const tCIDLib::TCard4 c4Cur) { return c4ToFind == c4Cur; }) != 4)
    {
        strmOut << TFWCurLn << L"Could not find search value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}

