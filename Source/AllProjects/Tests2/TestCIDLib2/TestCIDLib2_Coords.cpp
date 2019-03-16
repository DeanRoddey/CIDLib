//
// FILE NAME: TestCIDLib2_Coords.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/30/2007
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
//  This file contains tests related to the coordinate classes, TPoint and
//  TArea.
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
RTTIDecls(TTest_CoordCtor,TTestFWTest)
RTTIDecls(TTest_CoordAcc,TTestFWTest)
RTTIDecls(TTest_CoordOps,TTestFWTest)
RTTIDecls(TTest_Area1,TTestFWTest)
RTTIDecls(TTest_Area2,TTestFWTest)
RTTIDecls(TTest_Area3,TTestFWTest)



// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean bCheckArea(   const   TArea&          areaToCheck
                                , const tCIDLib::TInt4  i4X
                                , const tCIDLib::TInt4  i4Y
                                , const tCIDLib::TCard4 c4CX
                                , const tCIDLib::TCard4 c4CY)
{
    if ((areaToCheck.i4X() != i4X)
    ||  (areaToCheck.i4Y() != i4Y)
    ||  (areaToCheck.c4Width() != c4CX)
    ||  (areaToCheck.c4Height() != c4CY))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean bCheckPoint(  const   TPoint&         pntToCheck
                                , const tCIDLib::TInt4  i4X
                                , const tCIDLib::TInt4  i4Y)
{
    if ((pntToCheck.i4X() != i4X) || (pntToCheck.i4Y() != i4Y))
        return kCIDLib::False;
    return kCIDLib::True;
}

tCIDLib::TBoolean bCheckSize(   const   TSize&          szToCheck
                                , const tCIDLib::TCard4 c4X
                                , const tCIDLib::TCard4 c4Y)
{
    if ((szToCheck.c4Width() != c4X) || (szToCheck.c4Height() != c4Y))
        return kCIDLib::False;
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordCtor
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CoordCtor: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CoordCtor::TTest_CoordCtor() :

    TTestFWTest
    (
        L"Coord Ctors", L"TArea/TPoint constructor tests", 2
    )
{
}

TTest_CoordCtor::~TTest_CoordCtor()
{
}


// ---------------------------------------------------------------------------
//  TTest_CoordCtor: Public, inherited methods
// ---------------------------------------------------------------------------

tTestFWLib::ETestRes
TTest_CoordCtor::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // First check out the default constructors
    {
        TArea   areaDefault;
        if (!bCheckArea(areaDefault, 0, 0, 0, 0))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area default ctor failed\n\n";
        }
    }

    {
        TPoint  pntDefault;
        if (!bCheckPoint(pntDefault, 0, 0))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point default ctor failed\n\n";
        }
    }

    {
        TSize   szDefault;
        if (!bCheckSize(szDefault, 0, 0))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size default ctor failed\n\n";
        }
    }

    //
    //  Now test all of TPoint's non-default constructors
    //
    {
        TPoint pntTest(15, 16);
        if (!bCheckPoint(pntTest, 15, 16))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point ctor failed\n\n";
        }
    }

    {
        TPoint pntTest(75);
        if (!bCheckPoint(pntTest, 75, 75))
        {
            strmOut << TFWCurLn << L"Point ctor failed\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    {
        TPoint pntTest(TPoint(80, 90));
        if (!bCheckPoint(pntTest, 80, 90))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point ctor failed\n\n";
        }
    }

    {
        const tCIDLib::THostPoint HostPnt = { -1, 95 };
        TPoint pntTest(HostPnt);
        if (!bCheckPoint(pntTest, -1, 95))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point ctor failed\n\n";
        }
    }

    //
    //  Now test all of TSize's non-default constructors
    //
    {
        TSize szTest(101, 202);
        if (!bCheckSize(szTest, 101, 202))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size ctor failed\n\n";
        }
    }

    {
        TSize szTest(44);
        if (!bCheckSize(szTest, 44, 44))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size ctor failed\n\n";
        }
    }

    {
        TSize szTest(TSize(999, 606));
        if (!bCheckSize(szTest, 999, 606))
            strmOut << TFWCurLn << L"Size ctor failed\n\n";
    }

    {
        const tCIDLib::THostSize HostSize = { 1024, 2048 };
        TSize szTest(HostSize);
        if (!bCheckSize(szTest, 1024, 2048))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size ctor failed\n\n";
        }
    }

    //
    //  Now test all of TArea's non-default constructors
    //
    {
        TArea areaTest(10, 20, 30, 40);
        if (!bCheckArea(areaTest, 10, 20, 30, 40))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    {
        TArea areaTest(TPoint(50, 60), 30, 40);
        if (!bCheckArea(areaTest, 50, 60, 30, 40))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    {
        TArea areaTest(TPoint(10, 20), TSize(30, 40));
        if (!bCheckArea(areaTest, 10, 20, 30, 40))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    {
        tCIDLib::THostPoint HostPnt1 = { 100, 100 };
        tCIDLib::THostPoint HostPnt2 = { 500, 200 };

        TArea areaTest1(HostPnt1, HostPnt2);
        if (!bCheckArea(areaTest1, 100, 100, 401, 101))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    {
        TArea areaTest(TPoint(10, 20), TPoint(30, 40));
        if (!bCheckArea(areaTest, 10, 20, 20, 20))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    {
        TArea areaTest(TArea(99, 100, 101, 102));
        if (!bCheckArea(areaTest, 99, 100, 101, 102))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area ctor failed\n\n";
        }
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordAcc
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CoordAcc: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CoordAcc::TTest_CoordAcc() :

    TTestFWTest
    (
        L"Coord Access", L"Tests getters/setters of TArea/TPoint classes", 2
    )
{
}

TTest_CoordAcc::~TTest_CoordAcc()
{
}


// ---------------------------------------------------------------------------
//  TTest_CoordAcc: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CoordAcc::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Test some synonym methods to make sure that they agree.
    //
    {
        const TArea areaTest(11, 12, 13, 14);
        const TSize szTest(50, 60);

        //
        //  Test some of the synonym methods to make sure that they return the
        //  same thing.
        //
        if (areaTest.i4Top() != areaTest.i4Y())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4Bottom != i4Y\n\n";
        }

        if (areaTest.i4Left() != areaTest.i4X())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4Left != i4X\n\n";
        }

        if (szTest.c4X() != szTest.c4Width())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4X != c4Width\n\n";
        }

        if (szTest.c4Y() != szTest.c4Height())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4Y != c4Height\n\n";
        }
    }

    // Test all of the data access methods of TPoint
    {
        const TPoint pntTest(1111, 2222);

        if (!pntTest.bInArea(TArea(1110, 2220, 8, 8)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point should have beenin area\n\n";
        }

        if (pntTest.c4Packed() != TRawBits::c4From16(1111, 2222))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Packed point was not correct\n\n";
        }

        if (pntTest.i4X() != 1111)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point X value was incorrect\n\n";
        }

        if (pntTest.i4Y() != 2222)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point Y value was incorrect\n\n";
        }
    }

    // Test all of the data access methods of TSize
    {
        const TSize szTest(64, 32);

        if (szTest.c4SquareUnits() != 64 * 32)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size square units calc failed\n\n";
        }

        if (szTest.c4Packed() != TRawBits::c4From16(64, 32))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Packed size was incorrect\n\n";
        }

        if ((szTest.c4X() != 64) || (szTest.c4Width() != 64))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size cx was incorrect\n\n";
        }

        if ((szTest.c4Y() != 32) || (szTest.c4Height() != 32))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size cy was incorrect\n\n";
        }
    }

    // Test out all of the access methods of TArea
    {
        const TArea areaTest(128, 32, 64, 256);

        if (!bCheckArea(areaTest, 128, 32, 64, 256))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Initial area contents incorrect\n\n";
        }

        if (!areaTest.bContainsPoint(TPoint(132, 68)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area should have contained point\n\n";
        }

        if (!areaTest.bContainsPoint(132, 68))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area should have contained point\n\n";
        }

        if (areaTest.bIsEmpty())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area should not have been empty\n\n";
        }

        if (areaTest.bIntersects(TArea(0, 0, 32, 32)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Areas should not have intersected\n\n";
        }

        if (areaTest.bIntersects(TArea(0, 0, 33, 33)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Areas should have intersected\n\n";
        }

        tCIDLib::TFloat8 f8PercentX;
        tCIDLib::TFloat8 f8PercentY;
        areaTest.bPercentFromOrg
        (
            TPoint(128 + 32, 32 + 128)
            , f8PercentX, f8PercentY
        );
        if ((f8PercentX != 0.5) || (f8PercentY != 0.5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area percent from org calc failed\n\n";
        }

        if (!areaTest.bSameSize(TArea(0, 0, 64, 256)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area same size test failed\n\n";
        }

        // Check the calculation of the bottom and right sides
        if (areaTest.i4Right() != 191)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4Right() returned incorrect value\n\n";
        }

        if (areaTest.i4Bottom() != 287)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4Bottom() returned incorrect value\n\n";
        }

        if (areaTest.pntCenter() != TPoint(128 + 32, 32 + 128))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area center point was incorrect\n\n";
        }

        if (areaTest.pntOrg() != TPoint(128, 32))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area origin was incorrect\n\n";
        }

        if (areaTest.szArea() != TSize(64, 256))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area size was incorrect\n\n";
        }
    }

    // Test the point data modification methods
    {
        TPoint pntTest(64, 96);

        pntTest.Adjust(1, 1);
        if (!bCheckPoint(pntTest, 65, 97))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point adjust failed\n\n";
        }

        pntTest.Adjust(-2, -2);
        if (!bCheckPoint(pntTest, 63, 95))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point adjust failed\n\n";
        }

        pntTest.AdjustX(6);
        pntTest.AdjustY(-6);
        if (!bCheckPoint(pntTest, 69, 89))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point adjust failed\n\n";
        }

        pntTest.Exchange();
        if (!bCheckPoint(pntTest, 89, 69))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point adjust failed\n\n";
        }

        pntTest.Negate();
        if (!bCheckPoint(pntTest, -89, -69))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point negation failed\n\n";
        }

        pntTest.NegateX();
        pntTest.NegateY();
        if (!bCheckPoint(pntTest, 89, 69))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point negation failed\n\n";
        }

        pntTest.Adjust(1, 1);
        pntTest.Scale(0.5, 0.5);
        if (!bCheckPoint(pntTest, 45, 35))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point scale failed\n\n";
        }

        pntTest.Set(55, 100);
        if (!bCheckPoint(pntTest, 55, 100))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point set failed\n\n";
        }

        // Now do the indivdual sets
        pntTest.i4X(9);
        pntTest.i4Y(10);
        if (pntTest != TPoint(9, 10))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point member sets failed\n\n";
        }

        pntTest.Zero();
        if (!pntTest.bAtOrg())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point zero out failed\n\n";
        }
    }

    // Test the size data modification methods
    {
        TSize szTest(50, 60);

        szTest.Adjust(1, 1);
        if (!bCheckSize(szTest, 51, 61))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size adjust failed\n\n";
        }

        szTest.Adjust(-1, -1);
        if (!bCheckSize(szTest, 50, 60))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size adjust failed\n\n";
        }

        szTest.Adjust(1);
        if (!bCheckSize(szTest, 51, 61))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size adjust failed\n\n";
        }

        szTest.Adjust(-1);
        if (!bCheckSize(szTest, 50, 60))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size adjust failed\n\n";
        }

        szTest.AdjustCX(-2);
        szTest.AdjustCY(2);
        if (!bCheckSize(szTest, 48, 62))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size field adjust failed\n\n";
        }

        szTest.Exchange();
        if (!bCheckSize(szTest, 62, 48))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size exchange failed\n\n";
        }

        szTest.Set(90, 180);
        if (!bCheckSize(szTest, 90, 180))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size set failed\n\n";
        }

        szTest.c4Width(180);
        szTest.c4Height(90);
        if (!bCheckSize(szTest, 180, 90))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size member set failed\n\n";
        }

        szTest.Zero();
        if (!szTest.bAllZero())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Size zero out failed\n\n";
        }
    }

    // Test the area data modification methods
    {
        TArea areaTest(64, 128, 256, 512);

        // Adjust the origin via different methods
        areaTest.AdjustOrg(-32, -64);
        if (!bCheckArea(areaTest, 32, 64, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area origin adjust failed\n\n";
        }

        areaTest.AdjustOrg(TPoint(32, 64));
        if (!bCheckArea(areaTest, 64, 128, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area origin adjust failed\n\n";
        }

        // Adjust the size via different methods
        areaTest.AdjustSize(256, 512);
        if (!bCheckArea(areaTest, 64, 128, 512, 1024))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area size adjust failed\n\n";
        }

        areaTest.AdjustSize(TPoint(-256, -512));
        if (!bCheckArea(areaTest, 64, 128, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area size adjust failed\n\n";
        }

        // Adjust the sides
        areaTest.AdjustSides(TPoint(1, 1));
        if (!bCheckArea(areaTest, 63, 127, 258, 514))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area side adjust failed\n\n";
        }

        // Force it within another area
        areaTest.ForceWithin(TArea(64, 128, 256, 512), kCIDLib::True);
        if (!bCheckArea(areaTest, 64, 128, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area force within failed\n\n";
        }

        // Inflate and deflate and test
        areaTest.Inflate(1, 1);
        if (!bCheckArea(areaTest, 63, 127, 258, 514))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area inflate failed\n\n";
        }

        areaTest.Deflate(1, 1);
        if (!bCheckArea(areaTest, 64, 128, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area deflate failed\n\n";
        }

        areaTest.Inflate(1);
        if (!bCheckArea(areaTest, 63, 127, 258, 514))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area inflate failed\n\n";
        }

        areaTest.Deflate(1);
        if (!bCheckArea(areaTest, 64, 128, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area deflate failed\n\n";
        }

        // Justify the area within a larger area, using a couple justifications
        areaTest.JustifyIn
        (
            TArea(32, 32, 512, 1024)
            , tCIDLib::EHJustify::Left
            , tCIDLib::EVJustify::Top
        );
        if (!bCheckArea(areaTest, 32, 32, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area justify in failed\n\n";
        }

        areaTest.JustifyIn
        (
            TArea(32, 32, 512, 1024)
            , tCIDLib::EHJustify::Right
            , tCIDLib::EVJustify::Bottom
        );
        if (!bCheckArea(areaTest, 288, 544, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area justify in failed\n\n";
        }

        // Negate the origin
        areaTest.NegateOrg();
        if (!bCheckArea(areaTest, -288, -544, 256, 512))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area justify in failed\n\n";
        }

        // Scale the size
        areaTest.ScaleSize(0.5, 0.5);
        if (!bCheckArea(areaTest, -288, -544, 128, 256))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area justify in failed\n\n";
        }

        // Set the area via the Set method
        areaTest.Set(1, 1, 5, 5);
        if (!bCheckArea(areaTest, 1, 1, 5, 5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area set failed\n\n";
        }

        // Set just the size
        areaTest.SetSize(10, 10);
        if (!bCheckArea(areaTest, 1, 1, 10, 10))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area set size failed\n\n";
        }

        areaTest.SetSize(TSize(11, 11));
        if (!bCheckArea(areaTest, 1, 1, 11, 11))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area set size failed\n\n";
        }

        // Do the various zero methods
        areaTest.ZeroOrg();
        if (!bCheckArea(areaTest, 0, 0, 11, 11))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area zero org failed\n\n";
        }

        areaTest.ZeroSizes();
        if (!bCheckArea(areaTest, 0, 0, 0, 0))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area zero size failed\n\n";
        }

        areaTest = TArea(1, 2, 3, 4);
        areaTest.ZeroAll();
        if (!bCheckArea(areaTest, 0, 0, 0, 0))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Area zero all failed\n\n";
        }

    }

    // Test all of the conversions to and from host structures
    {
        // Convert a point to a host point
        TPoint pntTest(1111, 2222);
        tCIDLib::THostPoint HostPoint;
        pntTest.ToHostPt(HostPoint);
        if ((HostPoint.i4X != 1111) || (HostPoint.i4Y != 2222))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion to host point failed\n\n";
        }

        // Create another point from the host point and compare to original
        TPoint pntTest2(HostPoint);
        if (pntTest != pntTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion back from host point failed\n\n";
        }

        TSize szTest(64, 32);
        tCIDLib::THostSize HostSize;
        szTest.ToHostSize(HostSize);
        if ((HostSize.i4CX != 64) || (HostSize.i4CY != 32))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion to host size failed\n\n";
        }

        // Create another size and convert back and compare to original
        TSize szTest2(HostSize);
        if (szTest != szTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion back from host size failed\n\n";
        }

        // Convert an area to a host rect
        TArea areaTest(10, 10, 20, 30);
        tCIDLib::THostRectl HostRectl;

        areaTest.ToRectl(HostRectl, tCIDLib::ERectlTypes::Inclusive);
        if ((HostRectl.i4Left != 10)
        ||  (HostRectl.i4Top != 10)
        ||  (HostRectl.i4Right != 29)
        ||  (HostRectl.i4Bottom != 39))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Convert to host rectl failed\n\n";
        }

        //
        //  These return inclusive points, which should be the same as the rectl
        //  since we did it inclusively.
        //
        if ((HostRectl.i4Right != areaTest.i4Right())
        ||  (HostRectl.i4Bottom != areaTest.i4Bottom()))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Rectl right/bottom is wrong\n\n";
        }

        // Construct another area from it (other mode) and compare to original
        TArea areaTest2(HostRectl, tCIDLib::ERectlTypes::Inclusive);
        if (areaTest2 != areaTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion back from rectl failed\n\n";
        }

        areaTest.ToRectl(HostRectl, tCIDLib::ERectlTypes::NonInclusive);
        if ((HostRectl.i4Left != 10)
        ||  (HostRectl.i4Top != 10)
        ||  (HostRectl.i4Right != 30)
        ||  (HostRectl.i4Bottom != 40))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Convert to host rectl failed\n\n";
        }

        // Convert back from the rectangle
        areaTest2.FromRectl(HostRectl, tCIDLib::ERectlTypes::NonInclusive);
        if (areaTest2 != areaTest)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Conversion back from rectl failed\n\n";
        }
    }
    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordOps
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_CoordOps: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_CoordOps::TTest_CoordOps() :

    TTestFWTest
    (
        L"Operators", L"Tests TArea/TPoint operators", 2
    )
{
}

TTest_CoordOps::~TTest_CoordOps()
{
}


// ---------------------------------------------------------------------------
//  TTest_CoordOps: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_CoordOps::eRunTest(TTextStringOutStream&  strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test TSize operators
    {
        TSize szTest1(100, 200);
        TSize szTest2;

        // Do an assignment and test for equality
        szTest2 = szTest1;
        if (szTest1 != szTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Assignment of TSize object != to original\n\n";
        }

        // Some some addition and subtraction and test results
        if (TSize(1, 1) + TSize(10, 10) != TSize(11, 11))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Addition of two size objects failed\n\n";
        }

        if (TSize(10, 10) - TSize(5, 5) != TSize(5, 5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Subtraction of two size objects failed\n\n";
        }

        szTest2 -= TSize(50, 50);
        if (szTest2 == szTest1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Modified size object still == to original\n\n";
        }

        if (szTest2 != TSize(50, 150))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Subtraction of size failed\n\n";
        }

        szTest1 += TSize(60, 40);
        if (szTest1 != TSize(160, 240))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Addition of size failed\n\n";
        }
    }

    // Test TPoint operators
    {
        TPoint pntTest1(10, 11);
        TPoint pntTest2;

        // Do an assignment and test for equality
        pntTest2 = pntTest1;
        if (pntTest1 != pntTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Assignment of TPoint object != to original\n\n";
        }

        // Add and subtract them
        pntTest1 += pntTest2;
        if (pntTest1 != TPoint(20, 22))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point addition failed\n\n";
        }

        pntTest1 -= pntTest2;
        if (pntTest1 != TPoint(10, 11))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point subtraction failed\n\n";
        }

        if ((pntTest1 + pntTest2) != TPoint(20, 22))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point addition failed\n\n";
        }

        if ((pntTest1 - pntTest2) != TPoint::pntOrigin)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Point addition failed\n\n";
        }
    }

    // Test TArea operators
    {
        TArea areaTest1(1, 2, 10, 11);
        TArea areaTest2;

        // Do an assignment and test for equality
        areaTest2 = areaTest1;
        if (areaTest1 != areaTest2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Assignment of TArea object != to original\n\n";
        }

        //
        //  Do all of the logical operations and insure that they create the
        //  correct results.
        //
        if ((TArea(10, 10, 20, 20) | TArea(15, 15, 25, 25)) != TArea(10, 10, 30, 30))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Logical OR of two areas failed\n\n";
        }

        if ((TArea(10, 10, 20, 20) & TArea(15, 15, 25, 25)) != TArea(15, 15, 15, 15))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Logical AND of two areas failed\n\n";
        }

        areaTest1.Set(10, 10, 20, 20);
        areaTest1 |= TArea(15, 15, 25, 25);
        if (areaTest1 != TArea(10, 10, 30, 30))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Logical OR of area failed\n\n";
        }

        areaTest1.Set(10, 10, 20, 20);
        areaTest1 &= TArea(15, 15, 25, 25);
        if (areaTest1 != TArea(15, 15, 15, 15))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Logical AND of area failed\n\n";
        }


        //
        //  Do the mathmatical operators, only subtract for now, which effectively does
        //  an intersection operation. If one is inside the other, then the reult should
        //  be empty.
        //
        if ((TArea(10, 10, 20, 20) - TArea(15, 10, 20, 20)) != TArea(10, 10, 5, 20))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Subtract of two areas failed\n\n";
        }

        if ((TArea(10, 10, 20, 20) - TArea(15, 15, 5, 5)) != TArea(10, 10, 5, 5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Subtract of nested areas failed\n\n";
        }
    }

    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Area1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Area11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Area1::TTest_Area1() :

    TTestFWTest
    (
        L"TArea 1", L"Basic tests of the TArea class", 3
    )
{
}

TTest_Area1::~TTest_Area1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Area1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Area1::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //
    //  Do some constructor tests, where we construct some areas and make
    //  sure that the values end up being what they are supposed to be.
    //
    {
        // A simple object to hold the data for a single test
        struct TTestData
        {
            public :
                TTestData(  const   TArea&              areaToTest
                            , const tCIDLib::TInt4      i4X
                            , const tCIDLib::TInt4      i4Y
                            , const tCIDLib::TCard4     c4CX
                            , const tCIDLib::TCard4     c4CY) :

                    m_areaToTest(areaToTest)
                    , m_i4X(i4X)
                    , m_i4Y(i4Y)
                    , m_c4CX(c4CX)
                    , m_c4CY(c4CY)
                {
                }

                TArea               m_areaToTest;
                tCIDLib::TCard4     m_c4CX;
                tCIDLib::TCard4     m_c4CY;
                tCIDLib::TInt4      m_i4X;
                tCIDLib::TInt4      m_i4Y;
        };


        // Some host rectl structures for some tests below
        const tCIDLib::THostRectl rectlSrc1 = {1, 1, 2, 2};
        const tCIDLib::THostRectl rectlSrc2 = {1, 1, 1, 1};

        // And create an array of test objects to test
        const TTestData aTestData[] =
        {
            TTestData(TArea(), 0, 0, 0, 0)
          , TTestData(TArea(1,2,3,4), 1, 2, 3, 4)
          , TTestData(TArea(-1, -1, 1, 1), -1, -1, 1, 1)
          , TTestData(TArea(TPoint(50, 60), TSize(70, 80)), 50, 60, 70, 80)
          , TTestData(TArea(TPoint(51, 61), 71, 81), 51, 61, 71, 81)
          , TTestData(TArea(TPoint(1, 1), TPoint(100, 100)), 1, 1, 99, 99)

          // Inclusive vs. exclusive tests
          , TTestData(TArea(rectlSrc1, tCIDLib::ERectlTypes::Inclusive), 1, 1, 2, 2)
          , TTestData(TArea(rectlSrc1, tCIDLib::ERectlTypes::NonInclusive), 1, 1, 1, 1)
          , TTestData(TArea(rectlSrc2, tCIDLib::ERectlTypes::Inclusive), 1, 1, 1, 1)
          , TTestData(TArea(rectlSrc2, tCIDLib::ERectlTypes::NonInclusive), 1, 1, 0, 0)
        };
        const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(aTestData);

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TTestData& Data = aTestData[c4Index];

            if ((Data.m_areaToTest.i4X() != Data.m_i4X)
            ||  (Data.m_areaToTest.i4Y() != Data.m_i4Y)
            ||  (Data.m_areaToTest.c4Width() != Data.m_c4CX)
            ||  (Data.m_areaToTest.c4Height() != Data.m_c4CY))
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Failed ctor tests #" << (c4Index + 1)
                        << kCIDLib::DNewLn;
            }
        }
    }

    // Do some basic sanity check tests
    TArea areaTest1(0, 0, 10, 10);

    // Make sure that the right/bottom methods are correct. They return inclusive points!
    if (areaTest1.i4Right() != 9)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Right side (" << areaTest1.i4Right()
                << L" not at expected coordinate\n\n";
    }

    if (areaTest1.i4Bottom() != 9)
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Bottom (" << areaTest1.i4Bottom()
                << L" not at expected coordinate\n\n";
    }

    // Test equal size test (origin can be different)
    if (!areaTest1.bSameSize(TArea(1, 1, 10, 10)))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Same size test failed\n\n";
    }

    // Check some synonym methods
    if ((areaTest1.i4X() != areaTest1.i4Left())
    ||  (areaTest1.i4Y() != areaTest1.i4Top()))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Synonyms failed\n\n";
    }

    // Deflate and test coordinates
    areaTest1.Deflate(1);
    if ((areaTest1.i4X() != 1)
    ||  (areaTest1.i4Y() != 1)
    ||  (areaTest1.c4Width() != 8)
    ||  (areaTest1.c4Height() != 8))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Deflate gave wrong result\n\n";
    }

    areaTest1.Deflate(2, 1);
    if ((areaTest1.i4X() != 3)
    ||  (areaTest1.i4Y() != 2)
    ||  (areaTest1.c4Width() != 4)
    ||  (areaTest1.c4Height() != 6))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Deflate gave wrong result\n\n";
    }

    // Test the set variations
    areaTest1.Set(1, 2, 3, 4);
    if (areaTest1 != TArea(1, 2, 3, 4))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Set method failed \n\n";
    }

    areaTest1.Set(TPoint::pntOrigin, TSize(14, 15));
    if (areaTest1 != TArea(0, 0, 14, 15))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Set method failed \n\n";
    }

    areaTest1.SetOrg(-24, -26);
    if (areaTest1 != TArea(-24, -26, 14, 15))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"SetOrg method failed \n\n";
    }

    areaTest1.SetOrg(TPoint(435, 433));
    if (areaTest1 != TArea(435, 433, 14, 15))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"SetOrg method failed \n\n";
    }

    areaTest1.SetSize(70, 83);
    if (areaTest1 != TArea(435, 433, 70, 83))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"SetSize method failed \n\n";
    }

    areaTest1.SetSize(TSize(94, 5));
    if (areaTest1 != TArea(435, 433, 94, 5))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"SetSize method failed \n\n";
    }


    //
    //  Test setting from points. They are assumed to be non-inclusive.
    //
    areaTest1.FromPoints(TPoint(0, 0), TPoint(1, 1));
    if (areaTest1 != TArea(0, 0, 1, 1))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"SetFromPoints method failed \n\n";
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_Area2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Area21: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Area2::TTest_Area2() :

    TTestFWTest
    (
        L"TArea 2", L"Tests various offset/adjust scenarios of TArea", 3
    )
{
}

TTest_Area2::~TTest_Area2()
{
}


// ---------------------------------------------------------------------------
//  TTest_Area2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Area2::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    //  Test that the methods that lock the bottom/right work
    {
        TArea areaTest(1, 1, 10, 10);
        const tCIDLib::TInt4 i4OldR = areaTest.i4Right();
        const tCIDLib::TInt4 i4OldB = areaTest.i4Bottom();

        areaTest.i4X(2, kCIDLib::True);
        if (i4OldR != areaTest.i4Right())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4X() didn't lock right side correctly\n\n";
        }

        areaTest.i4Y(2, kCIDLib::True);
        if (i4OldB != areaTest.i4Bottom())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"i4Y() didn't lock bottom side correctly\n\n";
        }
    }

    // Test the justification methods
    {
        TArea areaOuter(10, 10, 100, 100);
        TArea areaInner(0, 0, 10, 10);

        // Do top and left
        areaInner.JustifyIn
        (
           areaOuter, tCIDLib::EHJustify::Left, tCIDLib::EVJustify::Top
        );
        if ((areaInner.i4X() != 10) || (areaInner.i4Y() != 10))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Justify left or top failed. Got "
                    << areaInner << L"\n\n";
        }

        // Do bottom and right
        areaInner.JustifyIn
        (
           areaOuter, tCIDLib::EHJustify::Right, tCIDLib::EVJustify::Bottom
        );
        if ((areaInner.i4X() != 100) || (areaInner.i4Y() != 100))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Justify bottom or left failed. Got "
                    << areaInner << L"\n\n";
        }


        // And center it
        areaInner.JustifyIn
        (
           areaOuter, tCIDLib::EHJustify::Center, tCIDLib::EVJustify::Center
        );
        if ((areaInner.i4X() != 55) || (areaInner.i4Y() != 55))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Center justification failed. Got "
                    << areaInner << L"\n\n";
        }
    }

    return eRes;
}





// ---------------------------------------------------------------------------
//  CLASS: TTest_Area3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Area31: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Area3::TTest_Area3() :

    TTestFWTest
    (
        L"TArea 3", L"Tests parsing and formatting of the TArea class", 3
    )
{
}

TTest_Area3::~TTest_Area3()
{
}


// ---------------------------------------------------------------------------
//  TTest_Area3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Area3::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Do some parsing tests
    {
        struct TTestData
        {
            public :
                TTestData(  const   tCIDLib::TCh* const pszData
                            , const tCIDLib::TCh        chSepChar
                            , const tCIDLib::TBoolean   bShoudFail
                            , const tCIDLib::ERadices   eRadix
                            , const TArea&              areaResult) :

                    m_areaResult(areaResult)
                    , m_bShouldFail(bShoudFail)
                    , m_chSepChar(chSepChar)
                    , m_eRadix(eRadix)
                    , m_pszData(pszData)
                {
                }

                TArea               m_areaResult;
                tCIDLib::TBoolean   m_bShouldFail;
                tCIDLib::TCh        m_chSepChar;
                tCIDLib::ERadices   m_eRadix;
                const tCIDLib::TCh* m_pszData;
        };

        const TTestData aTestData[] =
        {
            TTestData(L"1,1,1,1", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TArea(1,1,1,1))
          , TTestData(L"-1,-1,1,1", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TArea(-1,-1,1,1))
          , TTestData(L"-10,-20,1100,40", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TArea(-10,-20,1100,40))
          , TTestData(L"-0x10,-0x20,0x1100,0x40", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TArea(-0x10,-0x20,0x1100,0x40))
          , TTestData(L"-10,-20,1100,40", L',', kCIDLib::False, tCIDLib::ERadices::Hex, TArea(-0x10,-0x20,0x1100,0x40))
          , TTestData(L"-10 -20 1100 40", L' ', kCIDLib::False, tCIDLib::ERadices::Hex, TArea(-0x10,-0x20,0x1100,0x40))

            // These should fail
          , TTestData(L"-1,0", L',', kCIDLib::True, tCIDLib::ERadices::Auto, TArea())
          , TTestData(L"-1,X,1", L',', kCIDLib::True, tCIDLib::ERadices::Auto, TArea())
          , TTestData(L"-1,0,-1,0", L',', kCIDLib::True, tCIDLib::ERadices::Auto, TArea())
        };
        const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(aTestData);

        TArea areaTmp;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TTestData& Data = aTestData[c4Index];
            if (!areaTmp.bParseFromText(Data.m_pszData, Data.m_eRadix, Data.m_chSepChar))
            {
                if (!Data.m_bShouldFail)
                {
                    eRes = tTestFWLib::ETestRes::Failed;
                    strmOut << TFWCurLn << L"Failed to parse test area "
                            << (c4Index + 1) << kCIDLib::DNewLn;
                }
                continue;
            }

            if (Data.m_bShouldFail)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Test area " << (c4Index + 1)
                        << L" should have failed to parse\n\n";
                continue;
            }

            if (areaTmp != Data.m_areaResult)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Test area " << (c4Index + 1)
                        << L" was not equal to expected value\n\n";
            }
        }
    }


    // Do some formatting tests
    {
        TArea areaTest1(1, 2, 3, 4);
        TTextStringOutStream strmOut(1024UL);

        strmOut << areaTest1 << kCIDLib::FlushIt;
        if (strmOut.strData() != L"1,2,3,4")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Simple format test failed\n\n";
        }

        areaTest1.Set(-1, -2, 34, 45);
        strmOut.Reset();
        strmOut << areaTest1 << kCIDLib::FlushIt;
        if (strmOut.strData() != L"-1,-2,34,45")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Simple format test 2 failed\n\n";
        }
    }

    return eRes;
}



