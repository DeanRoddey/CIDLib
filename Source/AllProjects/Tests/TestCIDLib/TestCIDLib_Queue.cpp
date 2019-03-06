//
// FILE NAME: TestCIDLib_Queues.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  Queue collection classes.
//
//  This module brings in a common set of collection tests that are implemented
//  as templates themselves so that they can be invoked for all types of
//  collections without redundancy. But some of the testing is always specific
//  to the particular collection type.
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
//  Typedef some instantiations
// -----------------------------------------------------------------------------
typedef TQueue<TSize>       TQueueOfTSize;
typedef TQueue<TFloat>      TQueueOfTFloat;
typedef TRefQueue<TInteger> TRefQueueOfInteger;


// -----------------------------------------------------------------------------
//  Force full instantiations of at least one specialization of each class so
//  that we can catch any errors
// -----------------------------------------------------------------------------
template class TQueue<TSize>;
template class TRefQueue<TInteger>;



// -----------------------------------------------------------------------------
//  Local static functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestQueueBasics(TTextOutStream& strmOut)
{
    // Create a queue of float objects
    TQueueOfTFloat queTest;

    // Put two objects on it
    queTest.objPut(TFloat(1.0), tCIDLib::EQPrios::P0);
    queTest.objPut(TFloat(8.0), tCIDLib::EQPrios::P0);

    // There should be 2 elements in it
    if (queTest.c4ElemCount() != 2)
        strmOut << CUR_LN
                << L"Queue count was not 2 after putting two objects" << kCIDLib::EndLn;

    // Now get them off and check their values
    TFloat fTmp;
    if (!queTest.bGetNext(fTmp) || (fTmp != 1.0))
        strmOut << CUR_LN << L"Get next from queue was incorrect" << kCIDLib::EndLn;

    if (!queTest.bGetNext(fTmp) || (fTmp != 8.0))
        strmOut << CUR_LN << L"Get next from queue was incorrect" << kCIDLib::EndLn;

    // There should be no elements
    if (queTest.c4ElemCount())
    {
        strmOut << CUR_LN
                << L"Queue was not empty after getting objects out" << kCIDLib::EndLn;
    }

    // Test the other way
    if (!queTest.bIsEmpty())
    {
        strmOut << CUR_LN
                << L"Queue's bIsEmpty is eTrue but elem count was 0" << kCIDLib::EndLn;
    }
}



// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various aspects of
//  the Queue collections.
//
tCIDLib::TVoid TFacTestCIDLib::TestQueues()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic";
        TestQueueBasics(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" queue test" << kCIDLib::EndLn;
        throw;
    }
}
