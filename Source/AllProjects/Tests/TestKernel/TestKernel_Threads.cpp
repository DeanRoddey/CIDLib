//
// FILE NAME: TestKernel_Threads.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//  This module tests the threading classes
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
//  Local data types
//
//  We have to create a dummy TThread type because the kernel thread has a
//  parameter which is a pointer to one. But the real TThread is defined in a
//  layer above us. This simple structure is just a dummy place holder for
//  type. Its never looked at so that's fine.
// ----------------------------------------------------------------------------
struct  TThreadInfo
{
    tCIDLib::TCard4     c4Frequency;
    tCIDLib::TCard4     c4Duration;
    tCIDLib::TCard4     c4SleepTime;
    tCIDLib::TCard4     c4Count;
};

class TThread
{
    tCIDLib::TCard4     c4Dummy;
};


// ----------------------------------------------------------------------------
//  Local static data
//
//  kevWakeup
//      This is an event semaphore that the last thread to die will trigger,
//      to allow the main thread to continue.
//
//  kscntCounter1
//  kscntCounter2
//  kscntCounter3
//      These are global counters used by the thread instances.
//
//      Counter1 is bumped up when threads start and down when they end. The
//      last thread will then release the main thread again, which is blocked
//      on kevWakeup.
//
//      Counters 2 and 3 are manipulated in various ways, the results of which
//      should end up having no net effect on the initial value. So, they
//      should remain the same after all of the threads end up messing with
//      them.
// ----------------------------------------------------------------------------
static  TKrnlEvent              kevWakeup;
static  TKrnlSafeCard4Counter   kscntCounter1;
static  TKrnlSafeInt4Counter    kscntCounter2;
static  TKrnlSafeCard4Counter   kscntCounter3;


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  This is the thread function for the test thread. Multiple instances of
//  it are created and use semaphores to coordinate their activities (which
//  consists of manipulating counters and beeping.)
//
static tCIDLib::EExitCodes
eThreadFunc(TThread* const, tCIDLib::TVoid* const pData)
{
    // Bump up the main counter
    kscntCounter1.c4Inc();

    TThreadInfo& Info = *(TThreadInfo*)pData;
    for  (tCIDLib::TCard4 c4Counter = 0; c4Counter < Info.c4Count; c4Counter++)
    {
        TKrnlAudio::Beep(Info.c4Frequency, Info.c4Duration);
        TKrnlThread::Sleep(Info.c4SleepTime);

        kscntCounter3.c4AddTo(4);
        kscntCounter2.i4SubFrom(100);
        kscntCounter3.c4AddTo(10);
        kscntCounter2.i4Inc();
        kscntCounter2.i4AddTo(97);
        kscntCounter3.c4SubFrom(13);
        kscntCounter3.c4Dec();
        kscntCounter2.i4Inc();
        kscntCounter2.i4Inc();
    }

    // Dec the main counter and release main thread if 0
    if (!kscntCounter1.c4Dec())
        kevWakeup.bTrigger();
    return tCIDLib::EExitCodes::Normal;
}


// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------

tCIDLib::TVoid TestThreads(const tCIDLib::TCard4 c4RunIndex)
{
    //
    //  Create the event semaphore in a reset state so we will block.
    //
    if (!kevWakeup.bCreate(tCIDLib::EEventStates::Reset, kCIDLib::True))
    {
        strmOut << CUR_LN << L"Error creating wakeup event. "
                << TKrnlError::kerrLast() << EndLn;
        return;
    }

    //
    //  Set up a thread info structure for each of the threads and kick
    //  them off. They are all started blocked, then let go at the same
    //  time.
    //
    TThreadInfo Info1 = { 440,  75,  500, 10 };
    TThreadInfo Info2 = { 660, 100,  700, 10 };
    TThreadInfo Info3 = { 880, 200, 1500,  6 };

    TKrnlThread kthr1;
    TKrnlThread kthr2;
    TKrnlThread kthr3;

    tCIDLib::TCard4         c4StartCount = 0;
    tCIDLib::EThreadFlags   eFlags = tCIDLib::EThreadFlags::None;

    // Set up the counts that have non-zero initial values
    kscntCounter2.i4SetValue(-10);
    kscntCounter3.c4SetValue(100);

    //
    //  Flush standard out before we start so that preceeding messages
    //  will be visible before we potentially freak out here.
    //
    strmOut.flush();

    // Now kick off the threads
    TThread thrDummy;
    try
    {
        if (!kthr1.bBeginThread
        (
            eThreadFunc, 80 * 1024, &Info1, eFlags, &thrDummy, L"Thread1"))
        {
            throw TKrnlError::kerrLast();
        }
        c4StartCount++;

        if (!kthr2.bBeginThread
        (
            eThreadFunc, 80 * 1024, &Info2, eFlags, &thrDummy, L"Thread1"))
        {
            throw TKrnlError::kerrLast();
        }
        c4StartCount++;

        if (!kthr3.bBeginThread
        (
            eThreadFunc, 80 * 1024, &Info3, eFlags, &thrDummy, L"Thread1"))
        {
            throw TKrnlError::kerrLast();
        }
        c4StartCount++;
    }

    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << CUR_LN << L"Error starting test thread #"
                << c4StartCount << L" " << kerrToCatch << EndLn;
        return;
    }

    if (!c4StartCount)
    {
        strmOut << CUR_LN << L"No threads started up, bailing out\n";
        return;
    }

    //
    //  Now we just block on the wakeup event. When the last thread dies
    //  we should be awakened.
    //
    if (!kevWakeup.bWaitFor(30000))
    {
        if (TKrnlError::kerrLast().errcId() == kKrnlErrs::errcGen_Timeout)
        {
            strmOut << CUR_LN
                    << L"Test threads did not awaken main thread\n";
        }
         else
        {
            strmOut << CUR_LN << L"Error "
                    << TKrnlError::kerrLast().errcId()
                    << L" waiting on test threads to wake us up\n";
        }
    }

    //
    //  Check the test counters to see if they have their initial values
    //  still.
    //
    if (kscntCounter2.i4Value() != -10)
        strmOut << CUR_LN << L"Counter value is != original\n";

    if (kscntCounter3.c4Value() != 100)
        strmOut << CUR_LN << L"Counter value is != original\n";

    // Clean up the event
    if (!kevWakeup.bClose())
        throw TKrnlError::kerrLast();
}
