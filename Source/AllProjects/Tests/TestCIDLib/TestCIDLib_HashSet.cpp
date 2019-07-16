//
// FILE NAME: TestCIDLib_HashSet.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/1997
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
//  hash set collection class.
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
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class THashSet<TString, TStringKeyOps>;



// -----------------------------------------------------------------------------
//  Local constants
// -----------------------------------------------------------------------------
static const tCIDLib::TCard4 c4HashModulus = 17;
static const tCIDLib::TCard4 c4MaxElems = 6;


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------

static tCIDLib::TVoid TestSetBasics(TTextOutStream& strmOut)
{
    tCIDLib::TBoolean bCaughtIt;

    // Create a hash set of TString objects
    tCIDLib::TStrHashSet colTest(c4HashModulus, TStringKeyOps());

    // Add in one element, then try to add it again. It should be rejected
    colTest.objAdd(TString(L"Test String Value"));

    bCaughtIt = kCIDLib::False;
    try
    {
        colTest.objAdd(TString(L"Test String Value"));
    }

    catch(const TError& errToCatch)
    {
        if (!errToCatch.bCheckEvent(facCIDLib().strName(), kCIDErrs::errcCol_DuplicateElem))
        {
            TModule::LogEventObj(errToCatch);
            strmOut <<CUR_LN << L"Wrong exception thrown for duplicate element"
                    << kCIDLib::EndLn;
            return;
        }
        bCaughtIt = kCIDLib::True;
    }

    if (!bCaughtIt)
    {
        strmOut << CUR_LN << L"Failed to catch duplicate element addition"
                << kCIDLib::EndLn;
    }

    // Test the foreach callback stuff
    tCIDLib::TCard4 c4CBCount = 0;
    colTest.bForEach
    (
        [&c4CBCount](const TString& strCur) -> tCIDLib::TBoolean
        {
            c4CBCount++;
            return kCIDLib::True;
        }
    );

    if (c4CBCount != colTest.c4ElemCount())
    {
        strmOut << CUR_LN << L"ForEach lambda callback didn't count correctly"
                << kCIDLib::EndLn;
    }
}

static tCIDLib::TVoid TestSetStreaming(TTextOutStream& strmOut)
{
    // Create a hash set of string objects
    tCIDLib::TStrHashSet colTest(17, TStringKeyOps());

    // Put some values into it
    colTest.objAdd(L"Key1");
    colTest.objAdd(L"Key2");

    //
    //  Create a memory based output stream, and an input stream linked
    //  to it.
    //
    TBinMBufOutStream strmTestOut(4096UL);
    TBinMBufInStream strmTestIn(strmTestOut);

    // Stream the bag out, flush it, then stream back into it
    strmTestOut << colTest << kCIDLib::FlushIt;
    colTest.RemoveAll();
    strmTestIn >> colTest;

    // Make sure we got the right stuff back
    if (colTest.c4ElemCount() != 2)
    {
        strmOut << CUR_LN << L"Streamed in set had bad element count" << kCIDLib::EndLn;
        return;
    }

    if (!colTest.bHasElement(L"Key1")
    ||  !colTest.bHasElement(L"Key2"))
    {
        strmOut << CUR_LN << L"Streamed in set had missing elements" << kCIDLib::EndLn;
        return;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the hash set class.
//
tCIDLib::TVoid TFacTestCIDLib::TestHashSet()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestSetBasics(strmOut());

        pszCurTest = L"streaming";
        TestSetStreaming(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" hash set test" << kCIDLib::EndLn;
        throw;
    }
}
