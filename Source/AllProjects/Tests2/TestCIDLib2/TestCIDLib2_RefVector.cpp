//
// FILE NAME: TestCIDLib2_RefVector.cpp
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
//  This file contains tests related to the by reference vector collection
//  test.
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
RTTIDecls(TTest_RefVectorLambda, TTestFWTest)
RTTIDecls(TTest_RefVectorMoveItem, TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_RefVectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_RefVectorLambda: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_RefVectorLambda::TTest_RefVectorLambda() :

    TTestFWTest
    (
        L"Ref Vector Lambda", L"Ref vector lambda callback tests", 3
    )
{
}

TTest_RefVectorLambda::~TTest_RefVectorLambda()
{
}


// ---------------------------------------------------------------------------
//  TTest_RefVectorLambda: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_RefVectorLambda::eRunTest(TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a ref fector with some objects
    TRefVector<TString> colTest(tCIDLib::EAdoptOpts::Adopt, 16);
    TString strVal(32UL);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        strVal = L"Value ";
        strVal.AppendFormatted(c4Index);
        colTest.Add(new TString(strVal));
    }

    // Iterate the values and check them
    tCIDLib::TCard4 c4LoopVal = 0;
    colTest.ForEach
    (
        [&c4LoopVal, &strVal, &eRes, &strmOut](const TString& strCur)
        {
            strVal = L"Value ";
            strVal.AppendFormatted(c4LoopVal++);
            if (strVal != strCur)
            {
                strmOut << L"ForEach callback got out of sequence value\n\n";
                eRes = tTestFWLib::ETestRes::Failed;
            }
            return kCIDLib::True;
        }
    );

    // Do a find test
    strVal = L"Value ";
    strVal.AppendFormatted(5UL);
    if (colTest.tFind([&strVal](const TString& strCur) { return strVal == strCur; }) != 5)
    {
        strmOut << L"tFind did not find the test value\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_RefVectorMoveItem
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_RefVectorMoveItem: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_RefVectorMoveItem::TTest_RefVectorMoveItem() :

    TTestFWTest
    (
        L"Ref Vector Move Items", L"Ref vector item movement", 3
    )
{
}

TTest_RefVectorMoveItem::~TTest_RefVectorMoveItem()
{
}


// ---------------------------------------------------------------------------
//  TTest_RefVectorMoveItem: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_RefVectorMoveItem::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the moving of items in the vector
    {
        TRefVector<TString> colTest(tCIDLib::EAdoptOpts::Adopt, 16);

        // Load it up with values consequetive numeric values
        TString strVal;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
        {
            strVal.SetFormatted(c4Index);
            colTest.Add(new TString(strVal));
        }

        if (*colTest[0] != L"0")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First element value was wrong\n\n";
        }

        if (*colTest[15] != L"15")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Last element value was wrong\n\n";
        }


        //
        //  OK, move the 0th item to the end. The first element should now
        //  be 1 and the last element should be 0.
        //
        //  NOTE we give the count in order to to get it to the end. The to position
        //  is where we want to put it in the context of the current positions,
        //  not the position after it is removed from the from position, so that
        //  we don't have to adjust for that manually all the time.
        //
        colTest.MoveItem(0, 16);

        if (*colTest[0] != L"1")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First element value was wrong after move to end\n\n";
        }

        if (*colTest[15] != L"0")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Last element value was wrong after move to end\n\n";
        }

        //
        //  Move the first element to the end. Now the first element should still
        //  be 1 and the last should be 2.
        //
        colTest.MoveItem(1, 16);

        if (*colTest[0] != L"1")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First element value was wrong after move to end\n\n";
        }

        if (*colTest[15] != L"2")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Last element value was wrong after move to end\n\n";
        }


        //
        //  Move the second to last to the last, so now 0 should be back at
        //  the end and 2 should be the second to the last.
        //
        colTest.MoveItem(14, 15);

        if (*colTest[14] != L"2")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Element 14 was wrong after swap\n\n";
        }

        if (*colTest[15] != L"0")
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Last element was wrong after swap\n\n";
        }
    }

    return eRes;
}



