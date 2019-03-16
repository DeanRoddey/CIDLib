//
// FILE NAME: TestCIDLib_PerThreadData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/29/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  per-thread data support.
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


// -----------------------------------------------------------------------------
//  Local static data
//
//  ptdArea
//  ptdPoint
//  ptdString
//      Some per-thread data objects for areas, points, and strings. These are
//      used in the testing below.
// -----------------------------------------------------------------------------
#if 0
static TPerThreadDataFor<TArea>     ptdArea;
static TPerThreadDataFor<TPoint>    ptdPoint;
static TPerThreadDataFor<TString>   ptdString;


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Local, static methods
// -----------------------------------------------------------------------------

static tCIDLib::TVoid CommonTests()
{
    // Get a short cut to the output stream
    TTextOutStream& strmOut = TFacTestCIDLib::strmOut();

    //
    //  Check each of the objects and make sure that the initial values
    //  in them are initialized correct to zero.
    //
    if (ptdArea.pobjThis())
        strmOut << CUR_LN << L"Area per-thread not initialized to zero" << kCIDLib::EndLn;

    if (ptdPoint.pobjThis())
        strmOut << CUR_LN << L"Point per-thread not initialized to zero" << kCIDLib::EndLn;

    if (ptdString.pobjThis())
        strmOut << CUR_LN << L"String per-thread not initialized to zero" << kCIDLib::EndLn;

    //
    //  Set each one with an allocated object, then read the object back out
    //  and test that its the original object.
    //
    TArea* pareaSet = new TArea(1,2,3,4);
    ptdArea.pobjThis(pareaSet);

    TPoint* ppntSet = new TPoint(1,2);
    ptdPoint.pobjThis(ppntSet);

    TString* pstrSet = new TString(L"This is a test");
    ptdString.pobjThis(pstrSet);

    if (pareaSet != ptdArea.pobjThis())
        strmOut << CUR_LN << L"Area object != to object set" << kCIDLib::EndLn;

    if (ppntSet != ptdPoint.pobjThis())
        strmOut << CUR_LN << L"Point object != to object set" << kCIDLib::EndLn;

    if (pstrSet != ptdString.pobjThis())
        strmOut << CUR_LN << L"String object != to object set" << kCIDLib::EndLn;

    if (ptdArea.objThis() != TArea(1,2,3,4))
        strmOut << CUR_LN << L"Area object value != to object set" << kCIDLib::EndLn;

    if (ptdPoint.objThis() != TPoint(1,2))
        strmOut << CUR_LN << L"Point object value != to object set" << kCIDLib::EndLn;

    if (ptdString.objThis() != TString(L"This is a test"))
        strmOut << CUR_LN << L"String object value != to object set" << kCIDLib::EndLn;

    //
    //  Test out the dereference operator, which gets out the object pointer
    //  such that its convenient to access the methods of the object.
    //
    if (ptdArea->i4Left() != 1)
        strmOut << CUR_LN << L"Deref access to area object failed" << kCIDLib::EndLn;

    if (ptdPoint->i4Y() != 2)
        strmOut << CUR_LN << L"Deref access to point object failed" << kCIDLib::EndLn;

    if (ptdString->chAt(0) != L'T')
        strmOut << CUR_LN << L"Deref access to string object failed" << kCIDLib::EndLn;
}


static tCIDLib::EExitCodes eTestFunc(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Get a short cut to the output stream
    TTextOutStream& strmOut = TFacTestCIDLib::strmOut();

    // Get started by doing the common tests
    CommonTests();

    //
    //  Loop around for a while and manipulate our copy of the data. Check
    //  each change to insure that it hasn't been stepped on by another
    //  thread.
    //
    for (tCIDLib::TCard4 c4LoopCnt = 0; c4LoopCnt < 256; c4LoopCnt++)
    {
        //
        //  Set the point to the loop count. Set the area size to the
        //  loop count. And set the string to the formatted loop count.
        //
        ptdArea->SetSize(c4LoopCnt, c4LoopCnt);
        ptdPoint->i4X(tCIDLib::TInt4(c4LoopCnt));
        ptdString.objThis() = TCardinal(c4LoopCnt);

        TThread::Sleep(25);

        // Check that the values are correct before doing the next loop
        if (ptdArea->szSize() != TSize(c4LoopCnt))
            strmOut << CUR_LN << L"Thread's area value change not correct" << kCIDLib::EndLn;

        if (ptdPoint->i4X() != tCIDLib::TInt4(c4LoopCnt))
            strmOut << CUR_LN << L"Thread's point value change not correct" << kCIDLib::EndLn;

        if (ptdString.objThis() != TString(TCardinal(c4LoopCnt)))
            strmOut << CUR_LN << L"Thread's string value change not correct" << kCIDLib::EndLn;
    }

    //
    //  We don't delete the objects for each thread. They should be deleted
    //  when this thread ends!
    //

    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::TVoid SimpleTests()
{
    // Get a short cut to the output stream
    TTextOutStream& strmOut = TFacTestCIDLib::strmOut();

    // Call the common test function
    CommonTests();

    //
    //  Delete the objects and set the pointers back to zero so that it will
    //  work on the next pass if a memory leak pass is done.
    //
    ptdArea.CleanUpUserData();
    ptdPoint.CleanUpUserData();
    ptdString.CleanUpUserData();
}

static tCIDLib::TVoid ThreadedTests()
{
    // Get a short cut to the output stream
    TTextOutStream& strmOut = TFacTestCIDLib::strmOut();

    // Kick off a set of threads and wait for all of them to die
    const tCIDLib::TCard4 c4ThreadCount = 16;
    TThread* apthrList[c4ThreadCount];

    // Use a unique name object to create the thread names
    TUniqueName unamThreads(L"PerThread%(1)");

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        apthrList[c4Index] = new TThread
        (
            unamThreads.strQueryNewName()
            , eTestFunc
        );
    }

    // Kick off the threads
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        apthrList[c4Index]->Start();

    // And wait for all of them to die
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        apthrList[c4Index]->eWaitForDeath();
        delete apthrList[c4Index];
    }
}
#endif


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  instantiations of the per-thread data classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestPerThreadData()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"Simple Test";
//        SimpleTests();

        pszCurTest = L"Threaded Test";
//        ThreadedTests();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}

