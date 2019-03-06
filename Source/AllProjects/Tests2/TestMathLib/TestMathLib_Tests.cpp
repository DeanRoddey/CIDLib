//
// FILE NAME: TestMathLib_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a good bit of the tests for the MathLib facility.
//  Others are split out into their own files, but a lot of them are fairly
//  straightforward and done here in this one file.
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
#include    "TestMathLib.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Basic,TTestFWTest)
RTTIDecls(TTest_Ctor,TTestFWTest)
RTTIDecls(TTest_Geo,TTestFWTest)
RTTIDecls(TTest_Range,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Basic: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Basic::TTest_Basic() :

    TTestFWTest(L"Basic", L"Basic tests of MathLib operations", 2)
{
}

TTest_Basic::~TTest_Basic()
{
}


// ---------------------------------------------------------------------------
//  TTest_Basic: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Basic::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    if (TMathLib::f4Ceil(5.5) != 6.0)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"f4Ceiling failed";
    }

    if (TMathLib::f8Ceil(10.9) != 11.0)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"f8Ceiling failed";
    }


    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Ctor
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Ctor: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Ctor::TTest_Ctor() :

    TTestFWTest(L"Constructors", L"Tests math library class constructors", 2)
{
}

TTest_Ctor::~TTest_Ctor()
{
}


// ---------------------------------------------------------------------------
//  TTest_Ctor: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Ctor::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Geo
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Geo: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Geo::TTest_Geo() :

    TTestFWTest(L"Geo", L"Tests the geography math functions", 3)
{
}

TTest_Geo::~TTest_Geo()
{
}


// ---------------------------------------------------------------------------
//  TTest_Geo: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Geo::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Atlanta to Boulder
    tCIDLib::TCard4 c4Dist = TMathLib::c4GeoDistance
    (
        33.45, -84.23, 37.27, -105.52, kCIDLib::False
    );
    if (c4Dist != 1973)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Geo distance 1 calc failed";
    }

    c4Dist = TMathLib::c4GeoDistance
    (
        36.12, -86.67, 33.94, -118.4, kCIDLib::False
    );
    if (c4Dist != 2887)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Geo distance 2 calc failed";
    }

    c4Dist = TMathLib::c4GeoDistance
    (
        59.17, 18.3, 20.41, -103.2, kCIDLib::True
    );
    if (c4Dist != 6027)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Geo distance 3 calc failed";
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Range
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Range: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Range::TTest_Range() :

    TTestFWTest(L"Range", L"Tests the TRange class", 3)
{
}

TTest_Range::~TTest_Range()
{
}


// ---------------------------------------------------------------------------
//  TTest_Range: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Range::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TRange  rngTest(5, 0, 0, 99);

    if (rngTest.c4FullRange() != 100)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Full range of range object was not 100";
    }

    if (rngTest.c4CurPercent() != 0)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Current percent of range object is not 0";
    }

    rngTest.i4CurValue(50);
    if (rngTest.c4CurPercent() != 50)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Current percent of range object is not 50";
    }

    rngTest.i4CurValue(95);
    if (rngTest.c4CurPercent() != 95)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Current percent of range object is not 95";
    }

    if (rngTest.i4MaxLegalValue() != 95)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Max legal value of range is not 95";
    }

    if (rngTest.c4CalcPercent(75) != 75)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Calc'd percent of range is not 75";
    }

    // Set it all differently now
    rngTest.SetAll(995, 5, 0, 999);

    if (rngTest.c4FullRange() != 1000)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Full range of range object was not 1000";
    }

    if (rngTest.i4MaxLegalValue() != 5)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Max legal value of range is not 5";
    }

    // Bump up the max value
    rngTest.i4MaxValue(9999);

    if (rngTest.i4MaxLegalValue() != 9005)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Max legal value of range is not 9005";
    }

    // Create another with the known current values and compare
    TRange  rngTest2(995, 5, 0, 9999);

    if (rngTest != rngTest2)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Range objects did not equate properly";
    }

    // Copy construct another from the first and compare
    TRange  rngTest3(rngTest);

    if (rngTest != rngTest3)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Range objects did not equate properly";
    }
    return eRes;
}

