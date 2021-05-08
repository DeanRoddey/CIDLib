//
// FILE NAME: TestKernel_Mutex.cpp
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
//  This module tests the TKrnlMutex class.
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
// ----------------------------------------------------------------------------
struct  TThreadInfo
{
    tCIDLib::TCard4     c4Id;
    tCIDLib::TCard4     c4SleepTime;
    tCIDLib::TBoolean   bShutdown;
    const tCIDLib::TCh* pszError;
    TKrnlMutex*         pkmtxTest;
};

class TThread
{
    tCIDLib::TCard4     c4Dummy;
};



// ----------------------------------------------------------------------------
//  Local data
//
//  bGone
//      The main thread sets this when it thinks all the test threads are
//      shut down. If any of the threads see this set, they know that they
//      are still alive even though the main thread thinks they are dead.
//
//  c4CurrentId
//      This is set to the id of the thread that has the lock. If another
//      thread sees this set when it gets control, then it knows something
//      bad happened. Each thread zeros it when letting go of the lock.
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean    bGone = kCIDLib::False;
static tCIDLib::TCard4      c4CurrentId;


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::EExitCodes
eThreadFunc(TThread* const, tCIDLib::TVoid* const pData)
{
    // Get a pointer to our data, which is a TThreadInfo structure
    TThreadInfo& Info = *(TThreadInfo*)pData;

    //
    //  Loop until our shutdown flag gets set. We sleep for our sleep
    //  time on each round.
    //
    while (!Info.bShutdown)
    {
        // See if the gone flag is set
        if (bGone)
        {
            strmOut << CUR_LN
                    << L"Main thread exited while test thread is running\n";
            return tCIDLib::EExitCodes::FatalError;
        }

        // Sleep for our time
        TKrnlThread::Sleep(Info.c4SleepTime);

        if (!Info.pkmtxTest->bLock())
        {
            strmOut << CUR_LN << L"Could not lock text mutex. "
                    << TKrnlError::kerrLast() << EndLn;
        }

        // Check the global thread. It should be zero
        if (c4CurrentId)
        {
            Info.pszError = L"Thread id was non-zero upon getting control";
            return tCIDLib::EExitCodes::FatalError;
        }

        // Set it to our id
        c4CurrentId = Info.c4Id;

        // We should be able to lock it again in the same thread
        if (!Info.pkmtxTest->bLock())
        {
            strmOut << CUR_LN << L"Could not lock text mutex again in same thread. "
                    << TKrnlError::kerrLast() << EndLn;
        }
         else
        {
            if (!Info.pkmtxTest->bUnlock())
            {
                strmOut << CUR_LN << L"Could not not unlock nested mutex lock. "
                        << TKrnlError::kerrLast() << EndLn;
            }
        }

        // And sleep for half of our time
        TKrnlThread::Sleep(Info.c4SleepTime / 2);

        // Set the thread id back to zero
        c4CurrentId = 0;

        if (!Info.pkmtxTest->bUnlock())
        {
            strmOut << CUR_LN << L"Could not not unlock test mutex. "
                    << TKrnlError::kerrLast() << EndLn;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


static tCIDLib::TVoid BasicMutexTests()
{
    tCIDLib::TCh szTmpName[1024];

    // Test the constructors and make sure they set the right values
    TKrnlMutex  kmtxTest1;

    if (kmtxTest1.bNamed())
        strmOut << CUR_LN << L"Default constructor did not create unnamed\n";

    if (kmtxTest1.bIsValid())
        strmOut << CUR_LN << L"Handle was valid before creation\n";

    if (!kmtxTest1.bCreate(tCIDLib::ELockStates::Unlocked))
    {
        strmOut << CUR_LN << L"Error creating test mutex 1. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (!kmtxTest1.bIsValid())
        strmOut << CUR_LN << L"Created mutex was not valid\n";

    // Create a name object for the mutex
    TKrnlRscName krsnTest(L"TestKernel", L"Test2", L"Mutex");

    // Build the real mutex name
    if (!krsnTest.bBuildFullName(szTmpName, tCIDLib::c4MaxBufChars(szTmpName), tCIDLib::ENamedRscTypes::Mutex))
            throw TKrnlError::kerrLast();

    // Create a named semaphore now and test it out
    TKrnlMutex   kmtxTest2(szTmpName);

    if (!kmtxTest2.bNamed())
        strmOut << CUR_LN << L"Constructor did not create named\n";

    if (!TRawStr::bCompareStr(kmtxTest2.pszName(), szTmpName))
        strmOut << CUR_LN << L"Mutex name was not same as original\n";

    if (kmtxTest2.bIsValid())
        strmOut << CUR_LN << L"Handle was valid before creation\n";

    if (!kmtxTest2.bCreate(tCIDLib::ELockStates::Unlocked))
    {
        strmOut << CUR_LN << L"Error creating test mutex 2. "
                << TKrnlError::kerrLast() << EndLn;
    }

    if (!kmtxTest2.bIsValid())
        strmOut << CUR_LN << L"Created mutex was not valid\n";

    //
    //  Now try to create it again. This should fail because it already
    //  exists.
    //
    TKrnlMutex kmtxDuplicate(szTmpName);
    if (kmtxDuplicate.bCreate(tCIDLib::ELockStates::Unlocked))
    {
        strmOut << CUR_LN << L"Creation of duplicate mutex name succeeded\n";
    }
     else
    {
        if (TKrnlError::kerrLast().errcId() != kKrnlErrs::errcGen_AlreadyExists)
            strmOut << CUR_LN << L"Duplicate mutex name caused wrong error\n";
    }
}


static tCIDLib::TVoid ThreadedMutexTests()
{
    // Create an array of thread objects and thread info structures
    const tCIDLib::TCard4 c4ThreadCnt = 8;
    TKrnlThread* apkthrTest[c4ThreadCnt];
    TThreadInfo aThreadInfo[c4ThreadCnt];

    // Create the mutex that we want to test with
    TKrnlMutex  kmtxTest;
    if (!kmtxTest.bCreate(tCIDLib::ELockStates::Unlocked))
        throw TKrnlError::kerrLast();

    // Reset the global flags
    bGone = kCIDLib::False;
    c4CurrentId = 0;

    // Set up al the thread info structures
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        apkthrTest[c4Index] = new TKrnlThread;

        aThreadInfo[c4Index].c4Id = c4Index + 1;
        aThreadInfo[c4Index].c4SleepTime = c4Index * 250;
        aThreadInfo[c4Index].bShutdown = kCIDLib::False;
        aThreadInfo[c4Index].pszError = 0;
        aThreadInfo[c4Index].pkmtxTest = &kmtxTest;
    }

    // Kick off the threads
    TThread thrDummy;
    for (c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        if (!apkthrTest[c4Index]->bBeginThread
        (
            eThreadFunc
            , 0
            , &aThreadInfo[c4Index]
            , tCIDLib::EThreadFlags::None
            , &thrDummy
            , L"ThreadedMutexTestThread"))
        {
            throw TKrnlError::kerrLast();
        }
    }

    //
    //  Sleep for a while while they work. Then wake up and ask them all
    //  to shut down.
    //
    TKrnlThread::Sleep(10000);

    for (c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        aThreadInfo[c4Index].bShutdown = kCIDLib::True;

    for (c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
    {
        //
        //  Does not really need to be initialized, since this is an output
        //  parameter, but this suppresses warnings on some compilers.
        //
        tCIDLib::EExitCodes eCode = tCIDLib::EExitCodes::Normal;

        tCIDLib::TBoolean bState;
        if (!apkthrTest[c4Index]->bWaitForDeath(bState, eCode, 5000))
            throw TKrnlError::kerrLast();

        if (eCode != tCIDLib::EExitCodes::Normal)
        {
            strmOut << L"Thread #" << c4Index
                    << L" had an error: Text: \n    "
                    << aThreadInfo[c4Index].pszError << EndLn;
        }
         else if (!bState)
        {
            strmOut << L"Thread #" << c4Index << L" did not stop" << EndLn;
        }
    }

    // Indicate that we think everyone is down
    bGone = kCIDLib::True;

    // Delete the thread objects
    for (c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
        delete apkthrTest[c4Index];
}



// ----------------------------------------------------------------------------
//  Intrafacility functions
// ----------------------------------------------------------------------------
tCIDLib::TVoid TestMutexes(const tCIDLib::TCard4)
{
    BasicMutexTests();
    ThreadedMutexTests();
}

