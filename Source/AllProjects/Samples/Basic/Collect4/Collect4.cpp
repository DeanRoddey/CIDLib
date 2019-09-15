//
// FILE NAME: Collect4.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
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
//  This is the main module of the fourth of the collection oriented demo
//  programs. This program demonstrates more of the type safety of the
//  collections, plus it demonstrates the TQueue collection which has not
//  been used yet in the collection oriented samples.
//
//  This program simulates what a queue would be used for, a situation where
//  there are multiple 'producer' threads that are putting objects into a
//  queue which is read from one element at a time by a 'consumer' thread
//  that handles each element one at a time. Or of course it could be the
//  other way around, or multiple threads on both sides.
//
//  In this program, we just simulate this kind of situation. The worker
//  threads sleep for a given period of time, then they put their own
//  thread name on the queue. This is a useless program but it just illustrates
//  the issue. In reality, the worker threads might be waiting on a remote
//  machine to send a request which they would put onto the queue.
//
//  Note that the sleep time is generated as a random number by the main
//  thread and passed to the worker thread. Since the the thread that starts
//  a new thread is in sync mode, it won't return until the started thread calls
//  its Sync() method. So the main thread can just pass a pointer to the
//  counter that is using. The worker thread will get its copy before calling
//  Sync(), so there is no need to dynamically allocate a value or provide
//  another synchronization mechanism just to pass data to each new thread.
//
//  The queue consumer thread just waits for information to arrive and, when
//  it does, it pulls the string out and writes it out. It will wait for X
//  number of elements to be gotten from the worker threads, then it asks them
//  to shut down and then it exits. It prints out the elements as it gets
//  them out of the queue.
//
//  Note that no external synchronization is used here because threads are
//  doing single, atomic operations. The previous demo required that each
//  thread do multiple operations on the collection, which had to be atomic,
//  so it locked the collection itself and did all of the operations while
//  the collection was locked. This one can just depend on the natural
//  safety of a thread safe collection.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
tCIDLib::EExitCodes eWorkerThreadFunc(TThread&, tCIDLib::TVoid*);


// ---------------------------------------------------------------------------
//  Local data
//
//  colTest
//      A pointer to the queue that will be used for this test. We make it
//      a thread safe collection.
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class. We don't support
//      redirection in this program, so we can just use a console directly.
//
//  evWait
//      We hold the threads back until they are all created.
// ---------------------------------------------------------------------------
static TQueue<TString>  colTest(tCIDLib::EMTStates::Safe);
static TOutConsole      conOut;
static TEvent           evWait(tCIDLib::EEventStates::Reset);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Collect4MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. This is where the
//  program starts. It is indicated as the main thread via the use of the
//  CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  We create a vector of threads here to provides a little 'thread farm'
    //  and we use a by reference vector because threads are not copyable
    //  objects. So we have to store a pointer to them if we want to have a
    //  collection of them. We make it adopting so that it owns the threads
    //  that we put into it.
    //
    using TThreadList = TRefVector<TThread>;
    const tCIDLib::TCard4 c4ThreadCount = 5;
    TThreadList colOfThreads(tCIDLib::EAdoptOpts::Adopt, c4ThreadCount);

    // Default return value
    tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
    try
    {
        TRandomNum randTest;
        randTest.Seed(TTime::c4Millis());

        //
        //  Create our list of worker threads. We use a helper in the CIDLib faclity
        //  to give each one the required unique name.
        //
        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            colOfThreads.Add
            (
                new TThread(facCIDLib().strNextThreadName(L"Collect4Worker"), eWorkerThreadFunc)
            );
        }

        //
        //  Now start them all up. We could have started them above. But, in
        //  general, this is safer. If something happened in the loop above
        //  that caused an exception, some would have just been started for
        //  nothing. At this point we know for sure that all of them are there
        //  and ready to rock and roll.
        //
        //  Note that, since we are using a pointer wrapper, the [] operator
        //  just gets us the wrapper object. objData() gets us a reference
        //  to the object its wrapping.
        //
        TThreadList::TNCCursor cursThreads(&colOfThreads);
        for (; cursThreads; ++cursThreads)
        {
            //
            //  Get a random number between 1000 and 2000. This will be the
            //  sleep time for this thread.
            //
            tCIDLib::TCard4 c4SleepTime = 0;
            while ((c4SleepTime < 1000) || (c4SleepTime > 2000))
                c4SleepTime = randTest.c4GetNextNum();

            //
            //  Start the thread up now. We don't come back until it's up
            //  and got a copy of its passed start time.
            //
            cursThreads->Start(&c4SleepTime);
        }

        // Let them go now
        evWait.Trigger();

        //
        //  Now enter our main loop. We wait for 64 elements to arrive then
        //  we exit.
        //
        tCIDLib::TCard4 c4Count = 0;
        TString         strNextElem;
        while (c4Count < 64)
        {
            //
            //  Wait on something to arrive in the queue. We tell it we are
            //  willing to wait forever. bGetNext() will block until another
            //  element arrives. It has other parameters, but for our simple
            //  uses, all we need to do is to give it the element object we
            //  want it to fill in. We don't check the return because we are
            //  calling it in such a way that it wouldn't return unless it
            //  got one. If we provided a timeout, we'd have to check the
            //  return.
            //
            colTest.bGetNextMv(strNextElem);
            conOut  << L"    Got element from thread: " << strNextElem
                    << kCIDLib::EndLn;
            c4Count++;
        }

        //
        //  Now lets go through the list and ask each thread to shutdown. We
        //  do a non-synchronized request, so it just flags them to stop and
        //  returns. This way, we can start them all shutting down at once.
        //
        conOut << L"Asking threads to stop..." << kCIDLib::EndLn;
        cursThreads.bReset();
        for (; cursThreads; ++cursThreads)
            cursThreads->ReqShutdownNoSync();

        //
        //  Now wait for them to all be dead. We don't care about the exit
        //  status, so we don't check it.
        //
        cursThreads.bReset();
        for (; cursThreads; ++cursThreads)
            cursThreads->eWaitForDeath();
    }

    // Catch any CIDLib runtime errors for debuggin purposes
    catch(const TError& errToCatch)
    {
        conOut  <<  L"A CIDLib runtime error occured during processing.\n"
                <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        eRet = tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        eRet = tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        eRet = tCIDLib::EExitCodes::SystemException;
    }

    return eRet;
}


//
//  This is the the thread function for the worker threads. Each instance
//  just sleeps some random amount of time. When it wakes up, it puts its
//  name on the global test queue collection.
//
tCIDLib::EExitCodes
eWorkerThreadFunc(TThread& thrThis, tCIDLib::TVoid* pData)
{
    //
    //  Get our sleep time out that was passed to us. We need to do this
    //  before we sync and let the caller go, since he is going to reuse
    //  that data potentially.
    //
    const tCIDLib::TCard4 c4SleepTime = *reinterpret_cast<tCIDLib::TCard4*>(pData);
    thrThis.Sync();

    // Wait until we are told to go
    evWait.WaitFor();

    //
    //  Enter our loop. We just sleep for our sleep time, then we wake up
    //  and put our thread name onto the test queue. We check for shutdown
    //  requests each time. If we are asked to shutdown, the sleep call
    //  will return false.
    //
    while (thrThis.bSleep(c4SleepTime))
        colTest.objAdd(thrThis.strName());

    return tCIDLib::EExitCodes::Normal;
}
