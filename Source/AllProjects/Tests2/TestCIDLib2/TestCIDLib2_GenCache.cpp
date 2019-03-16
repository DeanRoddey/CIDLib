//
// FILE NAME: TestCIDLib2_GenCache.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/27/2014
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
//  This file contains tests related to the generic cache item class.
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
RTTIDecls(TTest_GenCache1,TTestFWTest)
RTTIDecls(TTest_GenCache2,TTestFWTest)
RTTIDecls(TTest_GenCache3,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_GenCache11: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_GenCache1::TTest_GenCache1() :

    TTestFWTest
    (
        L"Generic Cache Item", L"Basic tests of the TCIDGenCacheItem class", 4
    )
{
}

TTest_GenCache1::~TTest_GenCache1()
{
}


// ---------------------------------------------------------------------------
//  TTest_GenCache1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_GenCache1::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    // Just test out the generic cache item class itself
    TCIDGenCacheItem cciTest1(L"/Path/Test1", 1);

    if (cciTest1.strName() != L"/Path/Test1")
    {
        strmOut << TFWCurLn << L"Cache item path was not correct\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Initial bytes should be zero
    if (cciTest1.c4Bytes() != 0)
    {
        strmOut << TFWCurLn << L"Cache item bytes was not correct\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Serial number should be the 1 we set it to
    if (cciTest1.c4SerialNum() != 1)
    {
        strmOut << TFWCurLn << L"Cache item serial number was not correct\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Test serial number increment
    if ((cciTest1.c4IncSerialNum() != 2) || (cciTest1.c4SerialNum() != 2))
    {
        strmOut << TFWCurLn << L"Cache item serial number increment failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And directly set it
    cciTest1.c4SerialNum(4);
    if (cciTest1.c4SerialNum() != 4)
    {
        strmOut << TFWCurLn << L"Cache item serial number set failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_GenCache21: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_GenCache2::TTest_GenCache2() :

    TTestFWTest
    (
        L"Generic Cache 2", L"Tests of the generic cache", 5
    )
{
}

TTest_GenCache2::~TTest_GenCache2()
{
}


// ---------------------------------------------------------------------------
//  TTest_GenCache2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_GenCache2::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Create the standard cache. Make it small so we can test LRU removal
    //  of items easily.
    //
    tCIDLib::TGenCache colCache
    (
        109, new TStringKeyOps, &TCIDGenCacheItem::strGenKey
    );

    // Should be empty
    if (!colCache.bIsEmpty())
    {
        strmOut << TFWCurLn << L"Cache was not initially empty\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Add enough items to it to fill it up. Pausing a bit between each one so that
    //  we get a different time stamp.
    //
    TString strKey;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        strKey = L"/Path/Test";
        strKey.AppendFormatted(c4Index + 1);

        TCIDGenCacheItem& cciNew = colCache.objAdd(TCIDGenCacheItem(strKey, c4Index + 1));

        // Set the time stamp to now
        cciNew.UpdateTimeStamp(0);

        TThread::Sleep(100);
    }

    // Now do an LRU cleanup of up to 2 items. This should remove the first two
    if (TCIDGenCacheItem::c4LRUDiscard(colCache, 2) != 2)
    {
        strmOut << TFWCurLn << L"LRU did not remove 2 items\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Make sure it was the first two
    if (colCache.bKeyExists(L"/Path/Test1") || colCache.bKeyExists(L"/Path/Test2"))
    {
        strmOut << TFWCurLn << L"LRU did not remove correct items\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Be abusive and let it remove the remaining ones
    if (TCIDGenCacheItem::c4LRUDiscard(colCache, 6) != 6)
    {
        strmOut << TFWCurLn << L"LRU did not remove 6 items\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should be empty now
    if (!colCache.bIsEmpty())
    {
        strmOut << TFWCurLn << L"LRU did not remove all items\n\n";
        return tTestFWLib::ETestRes::Failed;
    }
    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_GenCache31: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_GenCache3::TTest_GenCache3() :

    TTestFWTest
    (
        L"Generic Cache 2", L"Tests of the generic cache (no case)", 5
    )
{
}

TTest_GenCache3::~TTest_GenCache3()
{
}


// ---------------------------------------------------------------------------
//  TTest_GenCache3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_GenCache3::eRunTest(  TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    //
    //  Create the standard cache. We tell the key ops not to be case sensitive in
    //  it's key operations. We test here that key sof different case will all be
    //  treated the same.
    //
    tCIDLib::TGenCache colCache
    (
        109, new TStringKeyOps(kCIDLib::False), &TCIDGenCacheItem::strGenKey
    );


    // Add an item
    colCache.objAdd(TCIDGenCacheItem(L"/TEST1/TEST2", 1));

    // Test it in some different case forms
    if (!colCache.bKeyExists(L"/test1/test2"))
    {
        strmOut << TFWCurLn << L"Case insensitive key exists failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Test it in some different case forms
    if (!colCache.bKeyExists(L"/Test1/Test2"))
    {
        strmOut << TFWCurLn << L"Case insensitive key exists failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!colCache.pobjFindByKey(L"/Test1/Test2"))
    {
        strmOut << TFWCurLn << L"Case insensitive lookup failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Try to add it again under a different case, which should fail
    tCIDLib::TBoolean bGotIt = kCIDLib::False;
    try
    {
        colCache.objAdd(TCIDGenCacheItem(L"/test1/test2", 1));
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch dup key\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (!colCache.bRemoveKeyIfExists(L"/test1/TEST2"))
    {
        strmOut << TFWCurLn << L"Case insensitive remove failed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


