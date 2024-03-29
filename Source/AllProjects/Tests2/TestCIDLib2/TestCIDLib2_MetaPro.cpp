//
// FILE NAME: TestCIDLib2_MetaPro.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/11/2019
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
//  This file contains tests related to CIDLib's metaprogramming support.
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
RTTIDecls(TTest_MetaProg1,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_MetaProg1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MetaProg1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MetaProg1::TTest_MetaProg1() :

    TTestFWTest
    (
        L"Meta Programming 1", L"Basic meta-programming tests", 1
    )
{
}

TTest_MetaProg1::~TTest_MetaProg1()
{
}


// ---------------------------------------------------------------------------
//  TTest_MetaProg1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MetaProg1::eRunTest(TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    if (!tCIDLib::IsTCardX<tCIDLib::TCard1>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCard1 test failed\n\n";
    }

    // Abreviated version
    if (!tCIDLib::bIsTCardX<tCIDLib::TCard2>)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCard2 test failed\n\n";
    }

    if (!tCIDLib::IsTCardX<tCIDLib::TCard4>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TCard4 test failed\n\n";
    }

    if (tCIDLib::bIsTCardX<TPoint>)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Point object reported being Card\n\n";
    }



    if (!tCIDLib::IsTFloatX<tCIDLib::TFloat4>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TFloat4 test failed\n\n";
    }

    if (!tCIDLib::bIsTFloatX<tCIDLib::TFloat8>)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TFloat8 test failed\n\n";
    }

    if (tCIDLib::IsTFloatX<TPoint>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Point object reported being Float\n\n";
    }



    if (!tCIDLib::IsTIntX<tCIDLib::TInt1>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TInt1 test failed\n\n";
    }

    if (!tCIDLib::bIsTIntX<tCIDLib::TInt2>)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TInt2 test failed\n\n";
    }

    if (!tCIDLib::IsTIntX<tCIDLib::TInt4>::bState)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"TInt4 test failed\n\n";
    }

    if (tCIDLib::bIsTIntX<TString>)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"String  object reported being Int\n\n";
    }


    return eRes;
}

