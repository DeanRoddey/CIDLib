//
// FILE NAME: TestCIDLib2_Bag.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 1/25/2019
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
//  This file contains tests related to the by value bags and sorted bags
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
RTTIDecls(TTest_BagMove, TTestFWTest)
RTTIDecls(TTest_BagPlace, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_BagMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BagMove: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BagMove::TTest_BagMove() :

    TTestFWTest
    (
        L"Bag Move", L"Bag collection move support", 3
    )
{
}

TTest_BagMove::~TTest_BagMove()
{
}


// ---------------------------------------------------------------------------
//  TTest_BagMove: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BagMove::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TBag<TArea> colTest;
    colTest.objAdd(TArea(1, 1, 1, 1));
    colTest.objAdd(TArea(2, 2, 2, 2));
    colTest.objAdd(TArea(3, 3, 3, 3));
    colTest.objAdd(TArea(4, 4, 4, 4));

    // Do a move ctor and test that they moved
    TBag<TArea> colTest2(tCIDLib::ForceMove(colTest));
    if (!colTest.bIsEmpty())
    {
        strmOut << L"Move ctor did not leave source empty\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest2.c4ElemCount() != 4)
    {
        strmOut << L"Move ctor did not move elements to target\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // And now do a move assign
    TBag<TArea> colTest3 = tCIDLib::ForceMove(colTest2);
    if (!colTest2.bIsEmpty())
    {
        strmOut << L"Move operator did not leave source empty\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    if (colTest3.c4ElemCount() != 4)
    {
        strmOut << L"Move operator did not move elements to target\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }


    // Test moving elements into a bag
    {
        tCIDLib::TStrBag colTest4;

        TString strSrc(L"Value 1");
        colTest4.objAdd(tCIDLib::ForceMove(strSrc));
        strSrc = L"Value 2";
        colTest4.objAdd(tCIDLib::ForceMove(strSrc));
        strSrc = L"Value 3";
        colTest4.objAdd(tCIDLib::ForceMove(strSrc));
        strSrc = L"Value 4";
        colTest4.objAdd(tCIDLib::ForceMove(strSrc));

        if (colTest4.c4ElemCount() != 4)
        {
            strmOut << L"Move of elements into bag created bad element count\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }

        tCIDLib::TStrBag::TCursor cursTest(&colTest4);
        tCIDLib::TCard4 c4Val = 1;
        for (; cursTest; ++cursTest)
        {
            strSrc = L"Value ";
            strSrc.AppendFormatted(c4Val++);
            if (*cursTest != strSrc)
            {
                strmOut << L"Move of elements into bag created bad values\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
                break;
            }
        }
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  CLASS: TTest_BagPlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_BagPlace: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_BagPlace::TTest_BagPlace() :

    TTestFWTest
    (
        L"Bag Element Placement", L"Bag collection element placement support", 3
    )
{
}

TTest_BagPlace::~TTest_BagPlace()
{
}


// ---------------------------------------------------------------------------
//  TTest_BagPlace: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_BagPlace::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    TBag<TArea> colTest;

    //
    //  Place objects at the top, which should then mean that iteration will be in
    //  this order.
    //
    colTest.objPlaceAtTop(1, 1, 1, 1);
    colTest.objPlaceAtTop(2, 2, 2, 2);
    colTest.objPlaceAtTop(3, 3, 3, 3);
    colTest.objPlaceAtTop(4, 4, 4, 4);

    TBag<TArea>::TCursor cursTest(&colTest);
    tCIDLib::TCard4 c4CurVal = 1;
    for (; cursTest; ++cursTest, c4CurVal++)
    {
        if (*cursTest != TArea(c4CurVal, c4CurVal, c4CurVal, c4CurVal))
        {
            strmOut << L"Top placement did not create correct element order\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }


    // Test the other direction
    colTest.RemoveAll();
    colTest.objPlaceAtBottom(1, 1, 1, 1);
    colTest.objPlaceAtBottom(2, 2, 2, 2);
    colTest.objPlaceAtBottom(3, 3, 3, 3);
    colTest.objPlaceAtBottom(4, 4, 4, 4);

    cursTest.bReset();
    c4CurVal = 4;
    for (; cursTest; ++cursTest, c4CurVal--)
    {
        if (*cursTest != TArea(c4CurVal, c4CurVal, c4CurVal, c4CurVal))
        {
            strmOut << L"Bottom placement did not create correct element order\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
            break;
        }
    }

    return eRes;
}

