//
// FILE NAME: TestKernel_Events.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/1996
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
//  This module tests the TKrnlEvent class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "TestKernel.hpp"


// ----------------------------------------------------------------------------
//  In order to make the kernel thread functions work, we have to fake a type
//  for TThread. He never tries to access it, but does pass through a pointer
//  to one. So we make a simple dummy structure for it.
// ----------------------------------------------------------------------------
class TThread
{
    tCIDLib::TCard4 c4Dummy;
};


// ----------------------------------------------------------------------------
//  Local static data
//
//  c4ThreadSet
//      A test flag that the test thread sets to indicate that it did what
//      it was supposed to do.
//
//  kscntTest1
//      A safe counter used for one of the tests, in which the thread's
//      access to the counter is not naturally serialized.
//
//  pkevTest1
//  pkevTest2
//      Two events that are created and destroyed as required to do testing.
//      They are global to make it easy for the spun off test threads to
//      see.
// ----------------------------------------------------------------------------
static  tCIDLib::TCard4         c4ThreadSet = 0;
static  TKrnlSafeCard4Counter   kscntTest;
static  TKrnlEvent*             pkevTest1 = 0;
static  TKrnlEvent*             pkevTest2 = 0;


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes eThreadFunc(TThread* const, tCIDLib::TVoid* const)
{
    // Sleep for a little while
    TKrnlThread::Sleep(2000);

    // Bump up the global flag
    c4ThreadSet++;

    // And signal the first test event
    if (!pkevTest1->bTrigger())
        throw TKrnlError::kerrLast();

    return tCIDLib::EExitCodes::Normal;
}

static tCIDLib::EExitCodes eThreadFunc2(TThread* const, tCIDLib::TVoid* const)
{
    // Bump up the safe counter
    kscntTest.bInc();

    // Indicate we're up on the first event
    if (!pkevTest1->bTrigger())
        throw TKrnlError::kerrLast();

    // Wait on the second event
    if (!pkevTest2->bWaitFor())
        throw TKrnlError::kerrLast();

    // Bump down the count
    kscntTest.bDec();

    return tCIDLib::EExitCodes::Normal;
}



static tCIDLib::TVoid BasicEventTests()
{
    tCIDLib::TCh szTmpName[1024];

    // Test the constructors and make sure they set the right values
    TKrnlEvent  kevTest1;

    if (kevTest1.bNamed())
        strmOut << CUR_LN << L"Default constructor did create unnamed\n";

    if (kevTest1.bIsValid())
        strmOut << CUR_LN << L"Handle was valid before creation\n";

    if (!kevTest1.bCreate(tCIDLib::EEventStates::Triggered, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Error creating test event 1. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (!kevTest1.bIsValid())
        strmOut << CUR_LN << L"Created event was not valid\n";

    // Create a name object for our test event
    TKrnlRscName krsnTest(L"TestKernel", L"Test2", L"Event");

    //
    //  Build the full name to the event and construct an event object
    //
    if (!krsnTest.bBuildFullName
    (
        szTmpName
        , c4MaxBufChars(szTmpName)
        , tCIDLib::ENamedRscTypes::Event))
    {
        throw TKrnlError::kerrLast();
    }
    TKrnlEvent kevTest2(szTmpName);

    if (!kevTest2.bNamed())
        strmOut << CUR_LN << L"Constructor did not create named\n";

    if (!TRawStr::bCompareStr(kevTest2.pszName(), szTmpName))
        strmOut << CUR_LN << L"Event name was not same as original\n";

    if (kevTest2.bIsValid())
        strmOut << CUR_LN << L"Handle was valid before creation\n";

    if (!kevTest2.bCreate(tCIDLib::EEventStates::Triggered, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Error creating test event 2. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (!kevTest2.bIsValid())
        strmOut << CUR_LN << L"Created event was not valid\n";

    //
    //  Now try to create it again. This should fail because it already
    //  exists.
    //
    TKrnlEvent kevDuplicate(szTmpName);
    if (kevDuplicate.bCreate(tCIDLib::EEventStates::Triggered, kCIDLib::True))
    {
        // Make sure its the expected error
        if (TKrnlError::kerrLast().errcId() != kKrnlErrs::errcGen_AlreadyExists)
            strmOut << CUR_LN << L"Failed to catch duplicate event name\n";
    }
}


static tCIDLib::TVoid ThreadedEventTests()
{
    // Create the test event. Create it reset
    pkevTest1 = new TKrnlEvent;
    if (!pkevTest1->bCreate(tCIDLib::EEventStates::Reset, kCIDLib::True))
        throw TKrnlError::kerrLast();

    // Reset the global flag
    c4ThreadSet = 0;

    TKrnlThread kthrTest;
    TThread thrDummy;
    if (!kthrTest.bBeginThread
    (
        eThreadFunc
        , 0
        , 0
        , tCIDLib::EThreadFlags::None
        , &thrDummy
        , L"ThreadedEventTestsThread"))
    {
        throw TKrnlError::kerrLast();
    }

    // Wait on the event, up to 5 seconds
    if (!pkevTest1->bWaitFor(5000))
        throw TKrnlError::kerrLast();

    // Make sure that the global flag is set to 1
    if (c4ThreadSet != 1)
        strmOut << CUR_LN << L"Test thread did not set global flag\n";

    // Delete the test event
    delete pkevTest1;
}


static tCIDLib::TVoid ThreadedEventTests2()
{
    // Create the test events
    pkevTest1 = new TKrnlEvent;
    if (!pkevTest1->bCreate(tCIDLib::EEventStates::Reset, kCIDLib::True))
        throw TKrnlError::kerrLast();

    pkevTest2 = new TKrnlEvent;
    if (!pkevTest2->bCreate(tCIDLib::EEventStates::Reset, kCIDLib::True))
        throw TKrnlError::kerrLast();

    //
    //  Create an array of thread objects.
    //
    const tCIDLib::TCard4 c4ThreadCount = 8;
    TKrnlThread* apkthrTest[c4ThreadCount];

    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        apkthrTest[c4Index] = new TKrnlThread;

    //
    //  Now kick each one off and wait for it to signal that it up, which it
    //  does on the first event. It should bump up the global flag before
    //  it signals us.
    //
    kscntTest.c4SetValue(0);
    TThread thrDummy;
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        // And start the thread
        if (!apkthrTest[c4Index]->bBeginThread
        (
            eThreadFunc2
            , 0
            , 0
            , tCIDLib::EThreadFlags::None
            , &thrDummy
            , L"ThreadedEventTestsThread"))
        {
            throw TKrnlError::kerrLast();
        }

        // Wait for the thread to get started
        if (!pkevTest1->bWaitFor(2000))
            throw TKrnlError::kerrLast();

        // The global flag should be equal to the index+1
        if (kscntTest.c4Value() != c4Index+1)
        {
            strmOut << CUR_LN << L"Thread " << c4Index
                 << L" failed to bump global flag\n";
        }

        // And reset the event for the next round
        if (!pkevTest1->bReset())
            throw TKrnlError::kerrLast();
    }

    // The count should be equal to the thread count
    if (kscntTest.c4Value() != c4ThreadCount)
        strmOut << CUR_LN << L"Counter != thread count\n";

    // All of the threads should still be running, waiting on the 2nd event
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        tCIDLib::TBoolean bRunning;
        if (!apkthrTest[c4Index]->bIsRunning(bRunning))
            throw TKrnlError::kerrLast();

        if (!bRunning)
        {
            strmOut << CUR_LN << L"Thread " << c4Index
                    << L" did not block on event\n";
        }
    }

    // Trigger the second event to cause them to shut down
    if (!pkevTest2->bTrigger())
        throw TKrnlError::kerrLast();

    // Wait for them all to die
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
    {
        tCIDLib::TBoolean   bState;
        tCIDLib::EExitCodes eCode;
        if (!apkthrTest[c4Index]->bWaitForDeath(bState, eCode))
            throw TKrnlError::kerrLast();
    }

    // The count should now be back to zero
    if (kscntTest.c4Value() != 0)
    {
        strmOut << CUR_LN << L"Counter was not back to zero (It was "
                << c4ThreadSet << L"\n";
    }

    // Delete the thread objecst
    for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        delete apkthrTest[c4Index];

    // Delete the test events
    delete pkevTest1;
    delete pkevTest2;
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestEvents(const tCIDLib::TCard4)
{
    BasicEventTests();
    ThreadedEventTests();
    ThreadedEventTests2();
}
