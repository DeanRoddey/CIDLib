//
// FILE NAME: TestCIDLib2_Numeric.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This file contains tests the basic TMathLib namespace, which provides low level
//  match functions. And also some of the numeric classes, which aren't used for math
//  but for those places where we need to treat numbers as objects, such as for
//  formatting purposes via the MFormattable base class.
//
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
RTTIDecls(TTest_Numeric1,TTestFWTest)
RTTIDecls(TTest_Numeric2,TTestFWTest)


// -----------------------------------------------------------------------------
//  Some templatized testing code that can be applied to all of the numeric
//  classes without change.
// -----------------------------------------------------------------------------
template <class T, class FT>
tCIDLib::TVoid CommonTests(TTextOutStream& strmOut, tTestFWLib::ETestRes& eRes)
{
    // Default construct an object
    T   objTest1;

    // Make sure it has a value of 0
    if (objTest1 != FT(0))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Default construction is != 0\n\n";
    }

    // Create another one with a value of 1
    T   objTest2(FT(1));

    // Make sure that they have the correct relationship
    if (objTest1 > objTest2)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Operator> failed\n\n";
    }

    if (objTest1 >= objTest2)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Operator>= failed\n\n";
    }

    if (objTest2 < objTest1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Operator< failed\n\n";
    }

    if (objTest2 <= objTest1)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Operator<= failed\n\n";
    }

    // Copy construct from the second object and compare
    T   objTest3(objTest2);

    if (objTest3 != objTest2)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Copy constructed object != to original\n\n";
    }

    if (!(objTest3 == objTest3))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Copy constructed object != to original\n\n";
    }

    // Assign its related fundamental type
    objTest1 = FT(16);
    if (objTest1 != FT(16))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Assignment of fundamental failed\n\n";
    }

    if (!(objTest1 == FT(16)))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Assignment of fundamental failed\n\n";
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Numeric1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Numeric1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Numeric1::TTest_Numeric1() :

    TTestFWTest
    (
        L"TMathLib Tests", L"Tests the low level math functions namespace", 2
    )
{
}

TTest_Numeric1::~TTest_Numeric1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Numeric1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Numeric1::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    //
    //  Do some rounding tests.
    //
    {
        // Rounding zero in any mode should return zero
        tCIDLib::TFloat4 f4Val = 0.0;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Down);
        if (f4Val != 0.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round down of zero failed\n\n";
        }

        f4Val = 0.0;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Up);
        if (f4Val != 0.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round up of zero failed\n\n";
        }

        f4Val = 0.0;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Closest);
        if (f4Val != 0.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round closest of zero failed\n\n";
        }


        //
        //  Test them on the edges. This wlla ctually come out 3.0 because it
        //  is too long in the fractions to resolve to 9s.
        //
        f4Val = 2.9999999999F;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Down);
        if (f4Val != 3.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round down of 2.9999... failed\n\n";
        }

        // This one should properly go down to 2.0
        f4Val = 2.99F;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Down);
        if (f4Val != 2.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round down of 2.99 failed\n\n";
        }


        // This one should rown down
        f4Val = 5.499F;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Closest);
        if (f4Val != 5.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round closest of 5.499... failed\n\n";
        }

        // This one should round up
        f4Val = 5.5;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Closest);
        if (f4Val != 6.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round closest of 5.5... failed\n\n";
        }

        // This one should round down because the fraction is too small
        f4Val = 5.0000000000001F;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Up);
        if (f4Val != 5.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round closest of 5.0... failed\n\n";
        }


        // This one should round up since there's just enough fraction to get there
        f4Val = kCIDLib::f4MinFloat;
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes::Up);
        if (f4Val != 1.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Round up of min float failed\n\n";
        }
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Numeric2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Numeric2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Numeric2::TTest_Numeric2() :

    TTestFWTest
    (
        L"Numeric Tests", L"Tests the simple 'numbers as objects' classes", 4
    )
{
}

TTest_Numeric2::~TTest_Numeric2()
{
}


// ---------------------------------------------------------------------------
//  TTest_Numeric2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Numeric2::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Construct each type via each constructor and insure that the values
    //  are set up correctly. We don't test copy construction because that's
    //  done in the basic tests above.
    //
    {
        TCardinal   cDefault;
        if (cDefault.c4Val() != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default cardinal ctor != 0\n\n";
        }

        if (cDefault.eRadix() != tCIDLib::ERadices::Dec)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default cardinal ctor has wrong radix\n\n";
        }

        TCardinal   cExplicit(91, tCIDLib::ERadices::Bin);
        if (cExplicit.c4Val() != 91)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Explicit cardinal ctor failed\n\n";
        }

        if (cExplicit.eRadix() != tCIDLib::ERadices::Bin)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Explicit cardinal ctor failed\n\n";
        }


        TInteger    iDefault;
        if (iDefault.i4Val() != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default integer ctor != 0\n\n";
        }

        if (iDefault.eRadix() != tCIDLib::ERadices::Dec)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default integer ctor has wrong radix\n\n";
        }

        TInteger   iExplicit(-91, tCIDLib::ERadices::Bin);
        if (iExplicit.i4Val() != -91)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Explicit integer ctor failed\n\n";
        }

        if (iExplicit.eRadix() != tCIDLib::ERadices::Bin)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Explicit ingeger ctor failed\n\n";
        }


        TFloat  fDefault;
        if (fDefault.f8Val() != 0.0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default float ctor != 0.0\n\n";
        }

        if (fDefault.c1Precision() != 2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default float ctor has bad precision\n\n";
        }

        if (fDefault.eTrailingStyle() != tCIDLib::ETrailFmts::Zeroes)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Default float ctor has bad trail style\n\n";
        }
    }

    // Do common tests on each basic type
    {
        CommonTests<TCardinal, tCIDLib::TCard4>(strmOut, eRes);
        CommonTests<TFloat, tCIDLib::TFloat8>(strmOut, eRes);
        CommonTests<TInteger, tCIDLib::TInt4>(strmOut, eRes);
        CommonTests<TMoney, tCIDLib::TFloat8>(strmOut, eRes);
    }

    return eRes;
}


