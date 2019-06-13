//
// FILE NAME: TestCIDLib2_Cursors.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/1/2019
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
//  This file contains tests related to collection cursors
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
RTTIDecls(TTest_ColCursors, TTestFWTest)


// ---------------------------------------------------------------------------
//  Templatized helpers to avoid a lot of redundancy
// ---------------------------------------------------------------------------

//
//  This one tests the bulk of the functionality. But it can't do bi-directional
//  stuff since not all of the collections we pass here will support them. So there
//  is a second one that does the b
//
template <typename TCol, typename TElem>
static tCIDLib::TBoolean bTestType(         TTextStringOutStream&   strmOut
                                    , const TString&                strType
                                    , const TTFWCurLn               clnAt
                                    ,       TCol&                   colTest
                                    , const TElem&                  objTestVal)
{
    TCol::TCursor cursEmpty;
    TCol::TNCCursor cursNCEmpty;
    if (cursEmpty.bIsValid() || cursNCEmpty.bIsValid())
    {
        strmOut << clnAt << L"Empty "
                << strType << L" cursors were not invalid\n\n";
        return kCIDLib::False;
    }

    // Remember if bi-directional for later use
    const tCIDLib::TBoolean bIsBiDir
    (
        cursEmpty.bIsDescendantOf(TBiColCursor<TElem>::clsThis())
    );

    tCIDLib::TCard4 c4ExceptCnt = 0;

    try { cursEmpty.bNext(); } catch(...) { c4ExceptCnt++; }
    try { cursEmpty.bSeekToEnd(); } catch(...) { c4ExceptCnt++; }
    try { cursEmpty.bReset(); } catch(...) { c4ExceptCnt++; }
    try { cursEmpty.objRCur(); } catch(...) { c4ExceptCnt++; }
    if (c4ExceptCnt != 4)
    {
        strmOut << clnAt << L"Some methods of "
                << strType << L" cursors did not throw when not initialized\n\n";
        return kCIDLib::False;
    }


    TCol::TCursor cursReal(&colTest);
    TCol::TNCCursor cursNCReal(&colTest);
    if (!cursReal.bIsValid() || !cursNCReal.bIsValid())
    {
        strmOut << clnAt << L"Correctly ctor'd "
                << strType << L" cursors were invalid\n\n";
        return kCIDLib::False;
    }

    // Real and empty shouldn't be equal
    if ((cursReal == cursEmpty) || (cursNCReal == cursNCEmpty))
    {
        strmOut << clnAt << L"Empty and valid "
                << strType << L" cursors were equal\n\n";
        return kCIDLib::False;
    }

    // Assign real to the empty, and they should be equal now
    cursEmpty = cursReal;
    cursNCEmpty = cursNCReal;
    if ((cursReal != cursEmpty) || (cursNCReal != cursNCEmpty))
    {
        strmOut << clnAt << L"Empty and valid "
                << strType << L" cursors were not equal after assignment\n\n";
        return kCIDLib::False;
    }

    // We should be able to assign via the non-const cursor
    *cursNCReal = objTestVal;
    if (*cursReal != objTestVal)
    {
        strmOut << clnAt << L"Assign through NC "
                << strType << L" cursor did not appear to set value\n\n";
        return kCIDLib::False;
    }

    //
    //  Iterate through them all and they should all give equal elements and
    //  should remain equal as we go.
    //
    while (cursEmpty.bIsValid())
    {
        // Test the elements pointed to
        const TElem& elemCur = *cursEmpty;
        if ((*cursNCEmpty != elemCur)
        ||  (*cursReal != elemCur)
        ||  (*cursNCReal != elemCur))
        {
            strmOut << clnAt << L"Some "
                    << strType << L" cursors iterated different results\n\n";
            return kCIDLib::False;
        }

        // Test the cursors themselves
        if ((cursEmpty != cursReal) || (cursNCEmpty != cursNCReal))
        {
            strmOut << clnAt << L"Some "
                    << strType << L" cursors became unequal during iteration\n\n";
            return kCIDLib::False;
        }

        ++cursEmpty;
        ++cursNCEmpty;
        ++cursReal;
        ++cursNCReal;
    }

    // They should all be invalid now
    if (cursReal.bIsValid()
    ||  cursEmpty.bIsValid()
    ||  cursNCReal.bIsValid()
    ||  cursNCEmpty.bIsValid())
    {
        strmOut << clnAt << L"Some "
                << strType << L" cursors were still valid after iteration to end\n\n";
        return kCIDLib::False;
    }

    // They are now invalid, but should be equal
    if ((cursReal != cursEmpty) || (cursNCReal != cursNCEmpty))
    {
        strmOut << clnAt << L"Empty and valid "
                << strType << L" cursors not equal after iteration to end\n\n";
        return kCIDLib::False;
    }

    //
    //  Reset them and they should all be pointing at the same first value, and
    //  should return true from reset, to indicate there are elements available.
    //
    if (!cursEmpty.bReset()
    ||  !cursNCEmpty.bReset()
    ||  !cursReal.bReset()
    ||  !cursNCReal.bReset())
    {
        strmOut << clnAt << L"Some "
                << strType << L" cursors returned false from reset when not empty\n\n";
        return kCIDLib::False;
    }

    if ((*cursEmpty != *cursReal) || (*cursNCEmpty != *cursNCReal))
    {
        strmOut << clnAt << strType
                << L" cursors on different elements after reset\n\n";
        return kCIDLib::False;
    }

    // Remove all from the collection, which should invalidate all of them
    colTest.RemoveAll();
    if (cursReal.bIsValid()
    ||  cursEmpty.bIsValid()
    ||  cursNCReal.bIsValid()
    ||  cursNCEmpty.bIsValid())
    {
        strmOut << clnAt << L"Some "
                << strType << L" cursors were still valid after col change\n\n";
        return kCIDLib::False;
    }

    // Reset them all again and they should return false, no elements available
    if (cursEmpty.bReset()
    ||  cursNCEmpty.bReset()
    ||  cursReal.bReset()
    ||  cursNCReal.bReset())
    {
        strmOut << clnAt << L"Some "
                << strType << L" cursors returned true from empty reset\n\n";
        return kCIDLib::False;
    }

    // Once again they are invalid, but should be equal
    if ((cursReal != cursEmpty) || (cursNCReal != cursNCEmpty))
    {
        strmOut << clnAt << L"Empty and valid "
                << strType << L" cursors not equal after empty reset\n\n";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


template <typename TCol, typename TElem>
static tCIDLib::TBoolean bTestBiType(       TTextStringOutStream&   strmOut
                                    , const TString&                strType
                                    , const TTFWCurLn               clnAt
                                    ,       TCol&                   colTest)
{
    TCol::TCursor cursTest(&colTest);

    // Count how many times we move forward before we are invalid
    tCIDLib::TCard4 c4Count = 0;
    for (; cursTest; ++cursTest)
        c4Count++;

    // Should not be valid now
    if (cursTest.bIsValid())
    {
        strmOut << clnAt << strType << L" cursor still valid after forward iter\n\n";
        return kCIDLib::False;
    }

    //
    //  Now go backwards and we should get the same count. Note can't just
    //  enter the loop without a seek because it would immediately show up as
    //  invalid, and would never enter the loop.
    //
    if (!cursTest.bSeekToEnd())
    {
        strmOut << clnAt << L"SeekToEnd should have worked for non-empty "
                << strType << L" cursor\n\n";
        return kCIDLib::False;
    }

    // Should be valid now
    if (!cursTest.bIsValid())
    {
        strmOut << clnAt << strType << L" cursor not valid after seek to end\n\n";
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4BCount = 0;
    for (; cursTest; --cursTest)
        c4BCount++;

    if (c4BCount != c4Count)
    {
        strmOut << clnAt << L"Got " << c4Count << L" forward iterations, but only "
                << c4BCount << L" reverse ones, for " << strType << L" cursor\n\n";
        return kCIDLib::False;
    }

    // SHould not be valid now
    if (cursTest.bIsValid())
    {
        strmOut << clnAt << strType << L" cursor still valid after reverse iter\n\n";
        return kCIDLib::False;
    }


    // Remove all the elements and do more checks
    colTest.RemoveAll();

    if (cursTest.bIsValid())
    {
        strmOut << clnAt << strType << L" cursor still valid after remove all\n\n";
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  CLASS: TTest_ColCursors
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_ColCursors: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_ColCursors::TTest_ColCursors() :

    TTestFWTest
    (
        L"Collect Cursors", L"Basic Collection Cursor Tests", 3
    )
{
}

TTest_ColCursors::~TTest_ColCursors()
{
}


// ---------------------------------------------------------------------------
//  TTest_ColCursors: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_ColCursors::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tCIDLib::TBoolean bRes;

    //
    //  In order to get the double linked lists, which are abstract, we need
    //  to do at least one derivative of them, so we do the bags. The deque
    //  and stack collections are also based on it, so we don't need to do
    //  those. They all use the cursors provided by the linked list base class,
    //  just aliasing them as their own cursor types.
    //
    {
        TBag<TString> colTest;
        colTest.objAdd(TString(L"Value 1"));
        colTest.objAdd(TString(L"Value 2"));
        colTest.objAdd(TString(L"Value 3"));
        colTest.objAdd(TString(L"Value 4"));
        colTest.objAdd(TString(L"Value 5"));

        bRes = bTestType<TBag<TString>, TString>
        (
            strmOut, L"bag", TFWCurLn, colTest, TString(L"Value 6")
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    {
        TRefBag<TString> colTest(tCIDLib::EAdoptOpts::Adopt);
        colTest.Add(new TString(L"Value 1"));
        colTest.Add(new TString(L"Value 2"));
        colTest.Add(new TString(L"Value 3"));
        colTest.Add(new TString(L"Value 4"));
        bRes = bTestType<TRefBag<TString>, TString>
        (
            strmOut, L"ref bag", TFWCurLn, colTest, TString(L"Value 5")
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }


    // Do hash sets
    {
        tCIDLib::TStrHashSet colTest(29, TStringKeyOps());
        colTest.objAdd(TString(L"Value 1"));
        colTest.objAdd(TString(L"Value 2"));
        colTest.objAdd(TString(L"Value 3"));

        // Just for syntax testing, which we can't do generically
        tCIDLib::TStrHashSet::TCursor cursC(&colTest);
        tCIDLib::TStrHashSet::TNCCursor cursNC(&colTest);
        if (cursC->c4Length() != cursNC->c4Length())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        // These have cursor based lookups, so test them before we call the helper
        const TString strToFind(L"Value 1");
        tCIDLib::TStrHashSet::TCursor cursFind = colTest.cursFind(strToFind);
        if (!cursFind)
        {
            strmOut << TFWCurLn << L"Failed to find string via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }
        else if (*cursFind != strToFind)
        {
            strmOut << TFWCurLn << L"Found wrong element via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }

        cursFind = colTest.cursFind(L"Not Found");
        if (cursFind)
        {
            strmOut << TFWCurLn << L"Cursor is valid after bad lookup\n\n";
            bRes = kCIDLib::False;
        }

        // And now do the general testing
        bRes = bTestType<THashSet<TString, TStringKeyOps>, TString>
        (
            strmOut, L"hash set", TFWCurLn, colTest, TString(L"Value 4")
        );
        if (bRes)
        {
            colTest.objAdd(TString(L"Value 1"));
            colTest.objAdd(TString(L"Value 2"));
            colTest.objAdd(TString(L"Value 3"));
            bRes = bTestBiType<THashSet<TString, TStringKeyOps>, TString>
            (
                strmOut, L"hash set", TFWCurLn, colTest
            );
        }

        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do keyed hash sets
    {
        tCIDLib::TKVHashSet colTest
        (
            29, TStringKeyOps(), TKeyValuePair::strExtractKey
        );
        colTest.objAdd(TKeyValuePair(L"Key 1", L"Value 1"));
        colTest.objAdd(TKeyValuePair(L"Key 2", L"Value 2"));
        colTest.objAdd(TKeyValuePair(L"Key 3", L"Value 3"));

        // Just for syntax testing, which we can't do generically
        tCIDLib::TKVHashSet::TCursor cursC(&colTest);
        tCIDLib::TKVHashSet::TNCCursor cursNC(&colTest);
        if (cursC->strKey() != cursNC->strKey())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        // These have cursor based lookups, so test them before we call the helper
        const TString strToFind(L"Key 1");
        tCIDLib::TKVHashSet::TCursor cursFind = colTest.cursFindByKey(strToFind);
        if (!cursFind)
        {
            strmOut << TFWCurLn << L"Failed to find string via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }
        else if (cursFind->strKey() != strToFind)
        {
            strmOut << TFWCurLn << L"Found wrong element via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }

        cursFind = colTest.cursFindByKey(L"Not Found");
        if (cursFind)
        {
            strmOut << TFWCurLn << L"Cursor is valid after bad lookup\n\n";
            bRes = kCIDLib::False;
        }

        // And now do the general testing
        bRes = bTestType<tCIDLib::TKVHashSet, TKeyValuePair>
        (
            strmOut
            , L"keyed hash set"
            , TFWCurLn
            , colTest
            , TKeyValuePair(L"Key 4", L"Value 4")
        );
        if (bRes)
        {
            colTest.objAdd(TKeyValuePair(L"Key 1", L"Value 1"));
            colTest.objAdd(TKeyValuePair(L"Key 2", L"Value 2"));
            colTest.objAdd(TKeyValuePair(L"Key 3", L"Value 3"));
            bRes = bTestBiType<tCIDLib::TKVHashSet, TKeyValuePair>
            (
                strmOut, L"keyed hash set", TFWCurLn, colTest
            );
        }
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do queues
    {
        TQueue<TArea> colTest;
        colTest.objAdd(TArea(1, 2, 3, 4));
        colTest.objAdd(TArea(2, 3, 4, 5));
        colTest.objAdd(TArea(3, 4, 5, 6));

        // Just for syntax testing, which we can't do generically
        TQueue<TArea>::TCursor cursC(&colTest);
        TQueue<TArea>::TNCCursor cursNC(&colTest);
        if (cursC->i4X() != cursNC->i4X())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        bRes = bTestType<TQueue<TArea>, TArea>
        (
            strmOut, L"queue", TFWCurLn, colTest, TArea(4, 5, 6, 7)
        );
        if (bRes)
        {
            colTest.objAdd(TArea(1, 2, 3, 4));
            colTest.objAdd(TArea(2, 3, 4, 5));
            colTest.objAdd(TArea(3, 4, 5, 6));
            bRes = bTestBiType<TQueue<TArea>, TArea>
            (
                strmOut, L"queue", TFWCurLn, colTest
            );
        }

        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do ref keyed hash sets
    {
        using TTestType = TRefKeyedHashSet<TKeyValuePair, TString, TStringKeyOps>;
        TTestType colTest
        (
            tCIDLib::EAdoptOpts::Adopt
            , 29
            , TStringKeyOps()
            , TKeyValuePair::strExtractKey
        );
        colTest.Add(new TKeyValuePair(L"Key 1", L"Value 1"));
        colTest.Add(new TKeyValuePair(L"Key 2", L"Value 2"));
        colTest.Add(new TKeyValuePair(L"Key 3", L"Value 3"));

        // Just for syntax testing, which we can't do generically
        TTestType::TCursor cursC(&colTest);
        TTestType::TNCCursor cursNC(&colTest);
        if (cursC->strKey() != cursNC->strKey())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        // These have cursor based lookups, so test them before we call the helper
        const TString strToFind(L"Key 1");
        TTestType::TCursor cursFind = colTest.cursFindByKey(strToFind);
        if (!cursFind)
        {
            strmOut << TFWCurLn << L"Failed to find string via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }
        else if (cursFind->strKey() != strToFind)
        {
            strmOut << TFWCurLn << L"Found wrong element via cursor lookup\n\n";
            bRes = kCIDLib::False;
        }

        cursFind = colTest.cursFindByKey(L"Not Found");
        if (cursFind)
        {
            strmOut << TFWCurLn << L"Cursor is valid after bad lookup\n\n";
            bRes = kCIDLib::False;
        }

        // And now do the general tests
        bRes = bTestType<TTestType, TKeyValuePair>
        (
            strmOut
            , L"ref keyed hash set"
            , TFWCurLn
            , colTest
            , TKeyValuePair(L"Key 4", L"Value 4")
        );
        if (bRes)
        {
            colTest.Add(new TKeyValuePair(L"Key 1", L"Value 1"));
            colTest.Add(new TKeyValuePair(L"Key 2", L"Value 2"));
            colTest.Add(new TKeyValuePair(L"Key 3", L"Value 3"));
            bRes = bTestBiType<TTestType, TKeyValuePair>
            (
                strmOut, L"ref keyed hash set", TFWCurLn, colTest
            );
        }
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do ref queues
    {
        TRefQueue<TArea> colTest(tCIDLib::EAdoptOpts::Adopt);
        colTest.Add(new TArea(1, 2, 3, 4));
        colTest.Add(new TArea(2, 3, 4, 5));
        colTest.Add(new TArea(3, 4, 5, 6));

        // Just for syntax testing, which we can't do generically
        TRefQueue<TArea>::TCursor cursC(&colTest);
        TRefQueue<TArea>::TNCCursor cursNC(&colTest);
        if (cursC->i4X() != cursNC->i4X())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        bRes = bTestType<TRefQueue<TArea>, TArea>
        (
            strmOut, L"ref queue", TFWCurLn, colTest, TArea(4, 5, 6, 7)
        );
        if (bRes)
        {
            colTest.Add(new TArea(1, 2, 3, 4));
            colTest.Add(new TArea(2, 3, 4, 5));
            colTest.Add(new TArea(3, 4, 5, 6));
            bRes = bTestBiType<TRefQueue<TArea>, TArea>
            (
                strmOut, L"ref queue", TFWCurLn, colTest
            );
        }
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }


    // Do ref vectors
    {
        TRefVector<TArea> colTest(tCIDLib::EAdoptOpts::Adopt, 3);
        colTest.Add(new TArea(1, 2, 3, 4));
        colTest.Add(new TArea(2, 3, 4, 5));
        colTest.Add(new TArea(3, 4, 5, 6));

        // Just for syntax testing, which we can't do generically
        TRefVector<TArea>::TCursor cursC(&colTest);
        TRefVector<TArea>::TNCCursor cursNC(&colTest);
        if (cursC->i4X() != cursNC->i4X())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        bRes = bTestType<TRefVector<TArea>, TArea>
        (
            strmOut, L"ref vector", TFWCurLn, colTest, TArea(4, 5, 6, 7)
        );
        if (bRes)
        {
            colTest.Add(new TArea(1, 2, 3, 4));
            colTest.Add(new TArea(2, 3, 4, 5));
            colTest.Add(new TArea(3, 4, 5, 6));
            bRes = bTestBiType<TRefVector<TArea>, TArea>
            (
                strmOut, L"ref vector", TFWCurLn, colTest
            );
        }
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Do vectors
    {
        TVector<TArea> colTest(3);
        colTest.objAdd(TArea(1, 2, 3, 4));
        colTest.objAdd(TArea(2, 3, 4, 5));
        colTest.objAdd(TArea(3, 4, 5, 6));

        // Just for syntax testing, which we can't do generically
        TVector<TArea>::TCursor cursC(&colTest);
        TVector<TArea>::TNCCursor cursNC(&colTest);
        if (cursC->i4X() != cursNC->i4X())
        {
            // Just in case
            strmOut << TFWCurLn << L"Const/Non-const arrow operators don't agree\n\n";
            bRes = kCIDLib::False;
        }

        bRes = bTestType<TVector<TArea>, TArea>
        (
            strmOut, L"vector", TFWCurLn, colTest, TArea(4, 5, 6, 7)
        );
        if (bRes)
        {
            colTest.objAdd(TArea(1, 2, 3, 4));
            colTest.objAdd(TArea(2, 3, 4, 5));
            colTest.objAdd(TArea(3, 4, 5, 6));
            bRes = bTestBiType<TVector<TArea>, TArea>
            (
                strmOut, L"vector", TFWCurLn, colTest
            );
        }
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}
