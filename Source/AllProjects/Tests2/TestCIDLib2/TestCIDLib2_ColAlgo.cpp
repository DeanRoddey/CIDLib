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
    using TPairList = TVector<TKeyValuePair>;
    TPntList colPoints(16);
    TCardList colCards;
    tCIDLib::TStrList colStrs;
    TPairList colPairs;
    TString strTmp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
    {
        colPoints.objPlace(c4Index, c4Index);
        colCards.objAdd(TCardinal(c4Index));

        strTmp.SetFormatted(c4Index);
        colStrs.objAdd(strTmp);

        colPairs.objPlace(strTmp, strTmp);
    }

    // Set up a reverse direction one as well
    TCardList colRevCards;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 16; c4Index++)
        colRevCards.objAdd(TCardinal(15 - c4Index));

    // Test the element finder algorithms, first the one that finds whole objects
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

    // Test the 'contains' algorithms
    {
        if (!tCIDColAlgo::bContains(colPoints, TPoint(2, 2)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"bContains failed to find contained element\n\n";
        }

        if (tCIDColAlgo::bContains(colPoints, TPoint(9999, 1111)))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"bContains found a non-contained element\n\n";
        }

        // Set up a list with some points in the point list and check all are present
        TPntList colTest;
        colTest.objPlace(5, 5);
        colTest.objPlace(8, 8);
        colTest.objPlace(11, 11);
        if (!tCIDColAlgo::bContainsAll(colPoints, colTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"bContainsAll reported present elements not found\n\n";
        }

        // Add one that's not in the list and test that
        colTest.objPlace(9995, 103);
        if (tCIDColAlgo::bContainsAll(colPoints, colTest))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"bContainsAll round non-present-elements\n\n";
        }
    }


    // And now test the finder variation that looks for a key
    {
        // Search for one we know is there
        TString strKey(L"8");
        TPairList::TCursor cursFind = tCIDColAlgo::cursFindByKey
        (
            colPairs, strKey, [](const TKeyValuePair& kvalCur, const TString& strKey)
            {
                return kvalCur.strKey() == strKey;
            }
        );
        if (!cursFind.bIsValid() || (cursFind->strValue() != strKey))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find element by key algorithm failed\n\n";
        }

        // And do the not exists test
        cursFind = tCIDColAlgo::cursFindNotByKey
        (
            colPairs, L"0", [](const TKeyValuePair& kvalCur, const TString& strKey)
            {
                return kvalCur.strKey() == strKey;
            }
        );
        if (!cursFind.bIsValid() || (cursFind->strValue() != L"1"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Find not element by key algorithm failed \n\n";
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

    // Test copying, to copy between two different types of collections
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

        if (tCIDColAlgo::tAccumulate(colCards, TCardinal(0)) != TCardinal(c4Sum2))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"tAccumulate on empty collection gave bad sum\n\n";
        }

        // Do it again with a non-zero starting value
        if (tCIDColAlgo::tAccumulate(colCards, TCardinal(5)) != TCardinal(c4Sum2 + 5))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"tAccumulate on empty collection gave bad sum\n\n";
        }
    }

    // Test the removal of consequetive dups
    {
        tCIDLib::TStrList colDups(8UL);
        colDups.objAdd(L"First");
        colDups.objAdd(L"Second");
        colDups.objAdd(L"Third");
        colDups.objAdd(L"Third");
        colDups.objAdd(L"Fourth");
        colDups.objAdd(L"Fifth");
        colDups.objAdd(L"Fifth");

        if (colDups.c4ElemCount() != 7)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Initial dups list count is wrong\n\n";
        }

        // It should remove two of them
        if (tCIDColAlgo::c4RemoveSeqDups(colDups) != 2)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove dropped wrong number of elements\n\n";
        }

        if (colDups.c4ElemCount() != 5)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove left wrong number of elements\n\n";
        }

        if ((colDups[3] != L"Fourth") || (colDups[4] != L"Fifth"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove bad remaining order\n\n";
        }


        // Do a more extreme example
        colDups.RemoveAll();
        colDups.objAdd(L"First");
        colDups.objAdd(L"First");
        colDups.objAdd(L"First");
        colDups.objAdd(L"First");
        if (tCIDColAlgo::c4RemoveSeqDups(colDups) != 3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove dropped wrong number of elements\n\n";
        }

        // We should have a single one left
        if ((colDups.c4ElemCount() != 1) || (colDups[0] != L"First"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove bad remaining elements\n\n";
        }

        // And a corner case
        colDups.RemoveAll();
        if (tCIDColAlgo::c4RemoveSeqDups(colDups) != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Seq dups remove on empty list returned non-zero\n\n";
        }
    }

    //
    //  Test element add/remove. Not something always needed, since most will have their
    //  own removal method. But it's there for any that don't, or for generic removal.
    //
    {
        tCIDLib::TStrList colRemove(8UL);
        colRemove.objAdd(L"Third");
        colRemove.objAdd(L"First");
        colRemove.objAdd(L"Second");
        colRemove.objAdd(L"Third");
        colRemove.objAdd(L"Fourth");
        colRemove.objAdd(L"Fifth");
        colRemove.objAdd(L"Third");

        //
        //  Do a single element removal first. It should remove the 0th element,
        //  and leave First as the 0th.
        //
        if (!tCIDColAlgo::bRemoveMatches(colRemove, L"Third", kCIDLib::False)
        ||  (colRemove[0] != L"First"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Single element removal failed\n\n";
        }

        // Then remove all remaining
        if (!tCIDColAlgo::bRemoveMatches(colRemove, L"Third", kCIDLib::True)
        ||  (colRemove.c4ElemCount() != 4))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Multiple element removal failed\n\n";
        }

        // Do add of somethng that's already there, which should be rejected
        if (tCIDColAlgo::bAddIfNew(colRemove, L"Second"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Duplicate element was not rejected\n\n";
        }

        // Now one that should be taken
        if (!tCIDColAlgo::bAddIfNew(colRemove, L"Sixth"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"New unique element was rejected\n\n";
        }

        // Do a remove if which should remove the one we just added
        if (!tCIDColAlgo::bRemoveIf
        (
            colRemove, [](const TString& strCheck) { return strCheck.bCompare(L"Sixth"); }))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Failed to remove new element via bRemoveIf\n\n";
        }

        //
        //  Use a case insensitive comparison, which should reject even though
        //  it's a different case.
        //
        if (tCIDColAlgo::bAddIfNew(colRemove, L"second", TString::bCompI))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Case insentive compare was not rejected\n\n";
        }

        // Add with a different case and case snsitive compare, which should be taken
        if (!tCIDColAlgo::bAddIfNew(colRemove, L"second"))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Different case element was rejected\n\n";
        }
    }

    // Test some miscellaneous bits
    {
        tCIDLib::TStrList colTest(8UL);
        colTest.objAdd(L"Third");
        colTest.objAdd(L"First");
        colTest.objAdd(L"Second");
        colTest.objAdd(L"Third");
        colTest.objAdd(L"Fourth");
        colTest.objAdd(L"Fifth");
        colTest.objAdd(L"Third");

        if (tCIDColAlgo::c4Count(colTest, TString(L"Third")) != 3)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4Count algorithm got incorrect count for Third\n\n";
        }

        if (tCIDColAlgo::c4Count(colTest, TString(L"Fifth")) != 1)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4Count algorithm got incorrect count for Fifth\n\n";
        }

        if (tCIDColAlgo::c4Count(colTest, TString(L"Sixth")) != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4Count algorithm found value not in list\n\n";
        }

        colTest.RemoveAll();
        if (tCIDColAlgo::c4Count(colTest, TString(L"Third")) != 0)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"c4Count algorithm found values in empty collection\n\n";
        }
    }

    // Test the map and map/reduce stuff
    {
        TVector<TCardinal> colSrcVals(1000UL);
        for (tCIDLib::TCard4 c4Index = 1; c4Index < 1001; c4Index++)
            colSrcVals.objAdd(TCardinal(c4Index));

        //
        //  Now reduce to even values. It returns references to elements in the source colelction
        //  in a non-adopting ref vector. We know there will be 500 results, so give it that hint
        //  as to the needed initial allocation of the return vector.
        //
        TRefVector<TCardinal> colMapped = tCIDColAlgo::colMapNC
        (
            colSrcVals
            , [](TCardinal& cVal) -> tCIDLib::TBoolean { return (cVal.c4Val() & 0x1U) == 0; }
            , 500
        );

        if (colMapped.c4ElemCount() != 500)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Expected 500 mapped values\n\n";
        }

        // Make sure they are all even
        for (tCIDLib::TCard4 c4Index = 0; c4Index < colMapped.c4ElemCount(); c4Index++)
        {
            if ((colMapped[c4Index]->c4Val() & 0x1) != 0)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Expected only event values to be mapped\n\n";
                break;
            }
        }

        // Test the const version
        if (!bTestConstMap(colSrcVals))
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Const map algorithm failed\n\n";
        }


        // Test the map/reduce. We accumulate the odd values starting with 10
        const tCIDLib::TCard4 c4Reduced = tCIDColAlgo::tMapReduce
        (
            colSrcVals
            , [](const TCardinal& cVal) -> tCIDLib::TBoolean { return (cVal.c4Val() & 0x1U) == 0; }
            , [](TCardinal& cAccum, const TCardinal& cVal) -> tCIDLib::TVoid { cAccum += cVal; }
            , TCardinal(10UL)
            , 500
        );

        if (c4Reduced != 250510)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Map/Reduce failed\n\n";
        }
    }

    return eRes;
}


// A helper so we can get a const collection to test the mapping algorithm on
tCIDLib::TBoolean TTest_ColAlgo1::bTestConstMap(const TVector<TCardinal>& colSrcVals)
{
    // We have to take const elements in this case and our lambda has to take a const param
    TRefVector<const TCardinal> colMapped = tCIDColAlgo::colMap
    (
        colSrcVals
        , [](const TCardinal& cVal) { return (cVal.c4Val() & 1) == 0; }
        , 500
    );

    if (colMapped.c4ElemCount() != 500)
        return kCIDLib::False;

    // Make sure they are all even
    for (tCIDLib::TCard4 c4Index = 0; c4Index < colMapped.c4ElemCount(); c4Index++)
    {
        if ((colMapped[c4Index]->c4Val() & 0x1) != 0)
            return kCIDLib::False;
    }
    return kCIDLib::True;
}
