//
// FILE NAME: TestCIDLib2_ColAlgo.cpp
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
//  This file contains tests related to the 'collection algorithms'. Though we
//  don't got remotely as crazy as the STL with generic collection algorithms,
//  we do have some simple ones for very common scenarios where performance is
//  not an issue.
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
RTTIDecls(TTest_ColAlgo1, TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_ColAlgo1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ColAlgo1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ColAlgo1::TTest_ColAlgo1() :

    TTestFWTest
    (
        L"Collection Algorithms 1", L"Basic collection algorithm tests", 4
    )
{
}

TTest_ColAlgo1::~TTest_ColAlgo1()
{
}


// ---------------------------------------------------------------------------
//  TTest_ColAlgo1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ColAlgo1::eRunTest(TTextStringOutStream&  strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Load up a couple types of collections for testing
    using TPntList = TVector<TPoint>;
    using TCardList = TBag<TCardinal>;
    TPntList colPoints(16);
    TCardList colCards;
    tCIDLib::TStrList colStrs;
    TString strTmp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        colPoints.objAdd(TPoint(c4Index, c4Index));
        colCards.objAdd(TCardinal(c4Index));

        strTmp.SetFormatted(c4Index);
        colStrs.objAdd(strTmp);
    }

    // Set up a reverse direction one as well
    TCardList colRevCards;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
        colRevCards.objAdd(TCardinal(15 - c4Index));

    // Test the element finder algorithm
    {
        // Search for one we know is there
        TPntList::TCursor cursFind = tCIDColAlgo::cursFind(colPoints, TPoint(1, 1));
        if (!cursFind.bIsValid() || (*cursFind != TPoint(1, 1)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find element algorithm failed\n\n";
        }

        // Search for one we know is not there
        cursFind = tCIDColAlgo::cursFind(colPoints, TPoint(999, 888));
        if (cursFind.bIsValid())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find element algorithm reported a hit for a bad value\n\n";
        }

        // Test first not
        cursFind = tCIDColAlgo::cursFindNot(colPoints, TPoint(0, 0));
        if (!cursFind.bIsValid() || (*cursFind != TPoint(1, 1)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find not element algorithm failed \n\n";
        }

        // Do an exhaustive search
        for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
        {
            if (!tCIDColAlgo::cursFind(colPoints, TPoint(c4Index, c4Index)))
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Failed to find one or more elements\n\n";
                break;
            }
        }
    }

    //
    //  Make sure function pointers work, since plenty of classes provide efficient
    //  static function pointer comparisons.
    //
    {
        tCIDLib::TStrList::TCursor cursFind;

        // Test with a function pointer comparator
        cursFind = tCIDColAlgo::cursFind(colStrs, TString(L"4"), TString::bComp);
        if (!cursFind || (*cursFind != L"4"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find failed with function pointer comp\n\n";
        }

        cursFind = tCIDColAlgo::cursFirstGreater(colStrs, TString(L"5"), TString::eComp);
        if (!cursFind.bIsValid() || (*cursFind != L"6"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First greater failed with function pointer comp\n\n";
        }

    }

    // Test some relative magnitude stuff
    {
        TCardList::TCursor cursFind;
        cursFind = tCIDColAlgo::cursFirstGreater(colCards, TCardinal(2));
        if (!cursFind.bIsValid() || (*cursFind != TCardinal(3)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First greater algorithm failed\n\n";
        }

        cursFind = tCIDColAlgo::cursFirstLess(colRevCards, TCardinal(8));
        if (!cursFind.bIsValid() || (*cursFind != TCardinal(7)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First less algorithm failed\n\n";
        }

        // Test failure scenarios for these two
        cursFind = tCIDColAlgo::cursFirstGreater(colCards, TCardinal(999));
        if (cursFind)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First greater reported bogus success\n\n";
        }

        cursFind = tCIDColAlgo::cursFirstLess(colCards, TCardinal(0));
        if (cursFind)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"First less reported bogus success\n\n";
        }
    }

    // Test the copying one, to copy between two different types of collections
    {
        TBag<TString> colTar;
        tCIDColAlgo::CopyElems(colTar, colStrs);

        // They should have the same element count
        if (colTar.c4ElemCount() != colStrs.c4ElemCount())
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Collections not equal size after CopyElems\n\n";
        }

        //
        //  The bag is ordered, so they should still iterate in the same order. So
        //  we should be able to use the generic bCompareElems() method to compare
        //  them.
        //
        if (!tCIDLib::bCompareElems<TString>(colTar, colStrs))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Elements not equal size after CopyElems\n\n";
        }
    }

    // Test the accumulate method
    {
        const TCardinal cSum = tCIDColAlgo::tAccumulate(colCards, TCardinal(0));

        // Do the sum manually and compare them
        TCardList::TCursor cursSum(&colCards);
        tCIDLib::TCard4 c4Sum2 = 0;
        for (; cursSum; ++cursSum)
            c4Sum2 += *cursSum;

        if (cSum != c4Sum2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"tAccumulate generated bad sum\n\n";
        }
    }

    return eRes;
}
