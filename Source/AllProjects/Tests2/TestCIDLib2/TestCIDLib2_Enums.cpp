//
// FILE NAME: TestCIDLib2_Enums.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/19/2019
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
//  This file contains tests related to enumerations
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
RTTIDecls(TTest_Enum1, TTestFWTest)
RTTIDecls(TTest_EnumForEach, TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_Enum1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Enum1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Enum1::TTest_Enum1() :

    TTestFWTest
    (
        L"Enum Basics", L"Basic enumeration tests", 1
    )
{
}

TTest_Enum1::~TTest_Enum1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Enum1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Enum1::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_EnumForEach
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_EnumForEach: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_EnumForEach::TTest_EnumForEach() :

    TTestFWTest
    (
        L"Enum For Each", L"Tests enum auto-iteration with callback", 2
    )
{
}

TTest_EnumForEach::~TTest_EnumForEach()
{
}


// ---------------------------------------------------------------------------
//  TTest_EnumForEach: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_EnumForEach::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tCIDLib::TCard4 c4Count;

    // Let's do a full iteration of the test enum
    c4Count = 0;
    tCIDLib::TBoolean bIterOK = kCIDLib::True;
    tTestCIDLib2::ETest1 eCompVal = tTestCIDLib2::ETest1::Min;
    tCIDLib::ForEachE<tTestCIDLib2::ETest1>
    (
        [&eCompVal, &bIterOK, &c4Count](const auto eVal)
        {
            if (eVal != eCompVal)
                bIterOK = kCIDLib::False;
            eCompVal++;
            c4Count++;
        }
    );

    if (!bIterOK || (c4Count != 5))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ForEach enum iteration failed" << kCIDLib::DNewLn;
    }

    // Do a range limited one
    c4Count = 0;
    bIterOK = kCIDLib::True;
    eCompVal = tTestCIDLib2::ETest1::Value2;
    tCIDLib::ForEachE
    <tTestCIDLib2::ETest1, tTestCIDLib2::ETest1::Value2, tTestCIDLib2::ETest1::Value4>
    (
        [&eCompVal, &bIterOK, &c4Count](const auto eVal)
        {
            if (eVal != eCompVal)
                bIterOK = kCIDLib::False;
            eCompVal++;
            c4Count++;
        }
    );

    if (!bIterOK || (c4Count != 3))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ForEach enum range iteration failed" << kCIDLib::DNewLn;
    }


    // Do a range limited one that uses a synonymm value
    c4Count = 0;
    bIterOK = kCIDLib::True;
    eCompVal = tTestCIDLib2::ETest1::Value2;
    tCIDLib::ForEachE
    <tTestCIDLib2::ETest1, tTestCIDLib2::ETest1::StartVal, tTestCIDLib2::ETest1::EndVal>
    (
        [&eCompVal, &bIterOK, &c4Count](const auto eVal)
        {
            if (eVal != eCompVal)
                bIterOK = kCIDLib::False;
            eCompVal++;
            c4Count++;
        }
    );

    if (!bIterOK || (c4Count != 3))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"ForEach enum syn range iteration failed" << kCIDLib::DNewLn;
    }

    // Test the variation that allows for break out
    tTestCIDLib2::ETest1 eLastVal = tCIDLib::eForEachE<tTestCIDLib2::ETest1>
    (
        [](const auto eCurVal) { return (eCurVal != tTestCIDLib2::ETest1::Value3); }
    );

    if (eLastVal != tTestCIDLib2::ETest1::Value3)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Conditional ForEach enum iteration to match failed"
                << kCIDLib::DNewLn;
    }

    // And again but make it go to the end this time
    eLastVal = tCIDLib::eForEachE<tTestCIDLib2::ETest1>
    (
        [](const auto eCurVal) { return kCIDLib::True; }
    );

    if (eLastVal != tTestCIDLib2::ETest1::Count)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Conditional ForEach enum iteration to end failed"
                << kCIDLib::DNewLn;
    }

    return eRes;
}
