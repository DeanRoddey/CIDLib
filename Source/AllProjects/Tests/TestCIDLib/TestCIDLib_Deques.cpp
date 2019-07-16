//
// FILE NAME: TestCIDLib_Deques.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
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
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  deque collection classes.
//
//  This module brings in a common set of collection tests that are implemented
//  as templates themselves so that they can be invoked for all types of
//  collections without redundancy. But some of the testing is always specific
//  to the particular collection type.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"
#include    "TestCIDLib_CommonCollect.hpp"


// -----------------------------------------------------------------------------
//  Typedef some instantiations that we want to test
// -----------------------------------------------------------------------------
typedef TDeque<TArea>       TDequeOfTArea;
typedef TDeque<TInteger>    TDequeOfTInteger;
typedef TRefDeque<TString>  TRefDequeOfString;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TDeque<TArea>;



// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestDequeBasics(TTextOutStream& strmOut)
{
    //
    //  Create a deques of integer objects
    //
    TDequeOfTInteger deqTest;

    // Push one on the bottom and one the top
    deqTest.objPushBottom(TInteger(1));
    deqTest.objPushTop(TInteger(16));

    // There should be 2 elements in it
    if (deqTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN
                << L"Deque count was not 2 after pushing two objects" << kCIDLib::EndLn;
    }

    // Peek at the too objects and make sure they are correct
    if (deqTest.objPeekBottom() != 1L)
        strmOut << CUR_LN << L"Peek at bottom of deque was incorrect" << kCIDLib::EndLn;

    if (deqTest.objPeekTop() != 16L)
        strmOut << CUR_LN << L"Peek at top of deque was incorrect" << kCIDLib::EndLn;

    // There should still be 2 elements in it
    if (deqTest.c4ElemCount() != 2)
        strmOut << CUR_LN << L"Deque could was not 2 after peeking" << kCIDLib::EndLn;

    // Now pop them off and check their values
    if (deqTest.objPopBottom() != 1L)
        strmOut << CUR_LN << L"Pop of bottom of deque was incorrect" << kCIDLib::EndLn;

    if (deqTest.objPopTop() != 16L)
        strmOut << CUR_LN << L"Pop at top of deque was incorrect" << kCIDLib::EndLn;

    // There should be no elements
    if (deqTest.c4ElemCount())
        strmOut << CUR_LN << L"Deque was not empty after popping" << kCIDLib::EndLn;

    // Test the other way
    if (!deqTest.bIsEmpty())
        strmOut << CUR_LN
                << L"Deque's bIsEmpty is eTrue but elem count was 0" << kCIDLib::EndLn;

    // Push a bunch on
    for (tCIDLib::TInt4 i4Ind = 0; i4Ind < 16; i4Ind++)
        deqTest.objPushBottom(TInteger(i4Ind+1));

    // Make sure that there are 16 elements
    if (deqTest.c4ElemCount() != 16)
        strmOut << CUR_LN
                << L"Deque count was not 16 after pushing 16 objects" << kCIDLib::EndLn;

    // Pop them the other way and check them
    for (tCIDLib::TInt4 i4Ind = 0; i4Ind < 16; i4Ind++)
    {
        if (deqTest.objPopTop() != i4Ind+1L)
            strmOut << CUR_LN << L"Deque element not equal to test index" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestRefDequeBasics(TTextOutStream& strmOut)
{
    //
    //  Create a deques of integer objects. It can hold up to 16 objects
    //
    TRefDequeOfString deqTest(tCIDLib::EAdoptOpts::Adopt);

    // Push one on the bottom and one the top
    deqTest.PushBottom(new TString(L"Value 1"));
    deqTest.PushTop(new TString(L"Value 2"));

    // There should be 2 elements in it
    if (deqTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN
                << L"Deque count was not 2 after pushing two objects" << kCIDLib::EndLn;
    }

    if (*deqTest.pobjPeekTop() != L"Value 2")
        strmOut << CUR_LN << L"Got wrong element content" << kCIDLib::EndLn;

    if (*deqTest.pobjPeekBottom() != L"Value 1")
        strmOut << CUR_LN << L"Got wrong element content" << kCIDLib::EndLn;

    // Pop one off. Now both top and bottom show be the same element
    deqTest.PopTop();
    if (*deqTest.pobjPeekTop() != L"Value 1")
        strmOut << CUR_LN << L"Got wrong element content" << kCIDLib::EndLn;

    if (*deqTest.pobjPeekBottom() != L"Value 1")
        strmOut << CUR_LN << L"Got wrong element content" << kCIDLib::EndLn;
}

// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the deque collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestDeques()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestDequeBasics(strmOut());

        pszCurTest = L"ref basic";
        TestRefDequeBasics(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" deque test" << kCIDLib::EndLn;
        throw;
    }
}
