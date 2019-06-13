//
// FILE NAME: TestCIDLib2_Pools.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2009
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
//  This file contains tests related to the simple pool class
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
RTTIDecls(TTest_FixedSizePool,TTestFWTest)
RTTIDecls(TTest_SimplePool,TTestFWTest)
RTTIDecls(TTest_SimplePoolPtr,TTestFWTest)


// ---------------------------------------------------------------------------
//  We need a little derivative of the fixed size pool
// ---------------------------------------------------------------------------
class TTestFSPool : public TFixedSizePool<TString>
{
    public :
        TTestFSPool() :

            TFixedSizePool(8, L"Test Fixed Size Pool", tCIDLib::EMTStates::Unsafe)
        {
        }

        ~TTestFSPool() {}


    protected :
        TString* pelemMakeNew() override
        {
            return new TString(64UL);
        }

         tCIDLib::TVoid PrepElement(TString& strTar) override
         {
             strTar.Clear();
         }
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_FixedSizePool
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_FixedSizePool1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_FixedSizePool::TTest_FixedSizePool() :

    TTestFWTest
    (
        L"Fixed Size Pool", L"Basic tests of the fixed size pool class", 4
    )
{
}

TTest_FixedSizePool::~TTest_FixedSizePool()
{
}


// ---------------------------------------------------------------------------
//  TTest_FixedSizePool: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_FixedSizePool::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    const tCIDLib::TCard4 c4Max(8);
    tCIDLib::TBoolean bGotIt;

    // Create an instance of our little test pool above
    TTestFSPool splTest;

    // Should have 0 current and 8 max and max avail
    if (splTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Initial element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (splTest.c4MaxElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Maximum element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Initial avail count was not max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    TString* apstrElems[c4Max];

    // Reserve four strings
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem();
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // Still should have four elements
    if (splTest.c4ElemCount() != 4)
    {
        strmOut << TFWCurLn << L"Element count should not have changed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have four still available
    if (splTest.c4ElemsAvail() != 4)
    {
        strmOut << TFWCurLn << L"Avail count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Still should have four elements
    if (splTest.c4ElemCount() != 4)
    {
        strmOut << TFWCurLn << L"Element count should not have changed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be same as max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Now reserve five strings. Some of these will be reused, so make sure that
    //  they come back empty, else it's not resetting them.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 5; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem();
        if (!apstrElems[c4Index]->bIsEmpty())
        {
            strmOut << TFWCurLn << L"Pool element was not reset on reserve\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // It should have expanded
    if (splTest.c4ElemCount() == 4)
    {
        strmOut << TFWCurLn << L"Pool should have expanded\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have three available
    if (splTest.c4ElemsAvail() != 3)
    {
        strmOut << TFWCurLn << L"Available count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reserve the rest
    for (tCIDLib::TCard4 c4Index = 5; c4Index < c4Max; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem();
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // We should have the max now
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Pool should have expanded\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have zero available
    if (splTest.c4ElemsAvail() != 0)
    {
        strmOut << TFWCurLn << L"Available count should be zero\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Try to allocate too many
    bGotIt = kCIDLib::False;
    try
    {
        // Avoid discard warning by just assiging the result
        TString* pstrRes = splTest.pobjReserveElem();
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch over-allocation\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Should still have the max
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Pool size should still be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count was not max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Try to release one that is not in the pool.
    //
    //  NOTE: We have to allocate a string here because it assumes it should clean up
    //  the passed object since it was being given back but wasn't found in the pool.
    //
    bGotIt = kCIDLib::False;
    try
    {
        TString* pstrTest = new TString(L"Bogus Value");
        splTest.ReleaseElem(pstrTest);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch bogus release\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reserve them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem();
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // Call release all
    splTest.ReleaseAll();

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And we should be able to reserve them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        apstrElems[c4Index] = splTest.pobjReserveElem();

    // Should have zero available
    if (splTest.c4ElemsAvail() != 0)
    {
        strmOut << TFWCurLn << L"Available count should be zero\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max allocated
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Allocated element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_SimplePool
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SimplePool1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SimplePool::TTest_SimplePool() :

    TTestFWTest
    (
        L"Simple Pool", L"Basic tests of the simple pool class", 4
    )
{
}

TTest_SimplePool::~TTest_SimplePool()
{
}


// ---------------------------------------------------------------------------
//  TTest_SimplePool: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SimplePool::eRunTest( TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    const tCIDLib::TCard4 c4Max(8);
    tCIDLib::TBoolean bGotIt;

    //
    //  We test with the existing pre-fab variation that works with strings. We use
    //  a 'prefer new string' threshold of 64, and expansion increment of 32.
    //
    TStringPool splTest(L"Test String Pool", c4Max, 32);

    // Should have 0 current and 8 max and max avail
    if (splTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Initial element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (splTest.c4MaxElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Maximum element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Initial avail count was not max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    TString* apstrElems[c4Max];

    // Reserve four strings
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem(16);
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // Still should have four elements
    if (splTest.c4ElemCount() != 4)
    {
        strmOut << TFWCurLn << L"Element count should not have changed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have four still available
    if (splTest.c4ElemsAvail() != 4)
    {
        strmOut << TFWCurLn << L"Avail count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Still should have four elements
    if (splTest.c4ElemCount() != 4)
    {
        strmOut << TFWCurLn << L"Element count should not have changed\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be same as max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Now reserve five strings
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 5; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem(18);
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // It should have expanded
    if (splTest.c4ElemCount() == 4)
    {
        strmOut << TFWCurLn << L"Pool should have expanded\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have three available
    if (splTest.c4ElemsAvail() != 3)
    {
        strmOut << TFWCurLn << L"Available count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reserve the rest
    for (tCIDLib::TCard4 c4Index = 5; c4Index < c4Max; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem(20);
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // We should have the max now
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Pool should have expanded\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have zero available
    if (splTest.c4ElemsAvail() != 0)
    {
        strmOut << TFWCurLn << L"Available count should be zero\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Try to allocate too many
    bGotIt = kCIDLib::False;
    try
    {
        // Avoid discard warning by just assiging the result
        TString* pstrRes = splTest.pobjReserveElem(20);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch over-allocation\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Should still have the max
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Pool size should still be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count was not max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    //
    //  Try to release one that is already released
    //
    //  NOTE: We have to allocate a string here because it assumes it should clean up
    //  the passed object since it was being given back but wasn't found in the pool.
    //
    bGotIt = kCIDLib::False;
    try
    {
        TString* pstrTest = new TString(L"Bogus Value");
        splTest.ReleaseElem(pstrTest);
    }

    catch(...)
    {
        bGotIt = kCIDLib::True;
    }

    if (!bGotIt)
    {
        strmOut << TFWCurLn << L"Failed to catch bogus release\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Reserve them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
    {
        apstrElems[c4Index] = splTest.pobjReserveElem(22);
        *apstrElems[c4Index] = L"Test value ";
        apstrElems[c4Index]->AppendFormatted(c4Index);
    }

    // Call release all
    splTest.ReleaseAll();

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // And we should be able to reserve them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        apstrElems[c4Index] = splTest.pobjReserveElem(22);

    // Should have zero available
    if (splTest.c4ElemsAvail() != 0)
    {
        strmOut << TFWCurLn << L"Available count should be zero\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Release them all again
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Max; c4Index++)
        splTest.ReleaseElem(apstrElems[c4Index]);

    // Should have max available
    if (splTest.c4ElemsAvail() != c4Max)
    {
        strmOut << TFWCurLn << L"Available count should be max\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Should have max allocated
    if (splTest.c4ElemCount() != c4Max)
    {
        strmOut << TFWCurLn << L"Allocated element count was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_SimplePoolPtr
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SimplePoolPtr1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SimplePoolPtr::TTest_SimplePoolPtr() :

    TTestFWTest
    (
        L"Simple Pool Pointer", L"Basic tests of the simple pool smart pointer", 4
    )
{
}

TTest_SimplePoolPtr::~TTest_SimplePoolPtr()
{
}


// ---------------------------------------------------------------------------
//  TTest_SimplePoolPtr: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We are run after the basic tests, so we should be able to trust the pool class
//  itself to be working right.
//
tTestFWLib::ETestRes
TTest_SimplePoolPtr::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    // A string pool with four max strings
    TStringPool splTest(L"Test String Pool", 4, 32);
    {
        TStringPool::TElemPtr ptrFirst = splTest.spptrReserveElem(64);

        // We should have one fewer available
        if (splTest.c4ElemsAvail() != 3)
        {
            strmOut << TFWCurLn << L"Pool pointer didn't reduce avail count\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // The pointer should be set now. Remember the address for checks below
        TString* const pstrElem = ptrFirst.pobjData();
        if (!pstrElem)
        {
            strmOut << TFWCurLn << L"Pool pointer data pointer is not set\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Copy the pointer
        {
            TStringPool::TElemPtr ptrSec(ptrFirst);

            // Both should point to the same element now
            if ((ptrFirst.pobjData() != pstrElem) || (ptrSec.pobjData() != pstrElem))
            {
                strmOut << TFWCurLn
                        << L"Both ptrs should point to the same element after copy ctor\n\n";
                return tTestFWLib::ETestRes::Failed;
            }

            //
            //  Assign back to the first, which should do nothing since they point to the
            //  same underlying object.
            //
            ptrFirst = ptrSec;
            if ((ptrFirst.pobjData() != pstrElem) || (ptrSec.pobjData() != pstrElem))
            {
                strmOut << TFWCurLn
                        << L"Both pts should point to same element after assign\n\n";
                return tTestFWLib::ETestRes::Failed;
            }

            // Set the value of the string and make sure both see it
            *ptrFirst = L"A test of the assign";

            if (*ptrSec != L"A test of the assign")
            {
                strmOut << TFWCurLn
                        << L"Value set via one ptr didn't show up via second\n\n";
                return tTestFWLib::ETestRes::Failed;
            }
        }

        // The second is out of scope now, make sure still reserved
        if (splTest.c4ElemsAvail() != 3)
        {
            strmOut << TFWCurLn << L"Element released with a pointer still alive\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // We should be back to 4 available elements
    if (splTest.c4ElemsAvail() != 4)
    {
        strmOut << TFWCurLn << L"Pool pointer didn't release element\n\n";
        return tTestFWLib::ETestRes::Failed;
    }


    {
        // Allocate two separate pointers
        TStringPool::TElemPtr ptrFirst = splTest.spptrReserveElem(64);
        TStringPool::TElemPtr ptrSec = splTest.spptrReserveElem(64);

        // Now we should have two available
        if (splTest.c4ElemsAvail() != 2)
        {
            strmOut << TFWCurLn << L"Should have 2 elements available her\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  They should both be empty strings now since one was reset on release and
        //  the other is a new one.
        //
        if ((*ptrFirst != TString::strEmpty()) || (*ptrSec != TString::strEmpty()))
        {
            strmOut << TFWCurLn << L"Both strings should be empty on reserve\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        // Assign different string values and make sure they both get them
        *ptrFirst = L"This is first";
        *ptrSec = L"This is second";
        if ((*ptrFirst != L"This is first") || (*ptrSec != L"This is second"))
        {
            strmOut << TFWCurLn << L"Assignment through pointer didn't work\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        //
        //  If we assign one to the other, then one should be released and both
        //  should have the assigned one's text.
        //
        ptrFirst = ptrSec;
        if (splTest.c4ElemsAvail() != 3)
        {
            strmOut << TFWCurLn << L"Assign should have released an element\n\n";
            return tTestFWLib::ETestRes::Failed;
        }

        if ((*ptrFirst != L"This is second") || (*ptrSec != L"This is second"))
        {
            strmOut << TFWCurLn << L"Assignment should make both strings equal\n\n";
            return tTestFWLib::ETestRes::Failed;
        }
    }

    // We should be back to 4 available elements
    if (splTest.c4ElemsAvail() != 4)
    {
        strmOut << TFWCurLn << L"Pool pointer didn't release element\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}


