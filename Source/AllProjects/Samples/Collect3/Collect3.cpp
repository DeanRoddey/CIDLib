//
// FILE NAME: Collect3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
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
//  This is the main module of the third of the collection oriented demo
//  programs. This program demonstrates the ability of all collections to
//  be thread safe. Any collection can optionally be thread safe if that is
//  desired. Thread safe collections make all of their methods atomic that
//  need to be (i.e. the ones that are not naturally thread safe.) They can
//  also be locked from the outside for multiple non-atomic operations.
//
//  This program simulates a situation where there are threads that are adding
//  elements to a collection and other threads are removing them. It creates
//  a set of threads, some of which are 'adders' and some of which are 'removers'
//  and lets them add and remove elements from a sorted bag of TInteger objects.
//
//  Each thread will sleep for some random small amount of time. Then it will
//  generate a random number. If its an adder it will see if this value is
//  already in the collection. If so, it does nothing, else it adds it. If its
//  a remover, it sees if this value is in the bag and removes it if so.
//
//  The main thread just sleeps for a while to let them do their thing, then it
//  wakes up and asks them to shutdown, waiting until they are all dead. It
//  then displays any values that survived, which should occur in sorted order
//  since the collection is a sorted bag.
//
//  This program also demonstrates how to do a TThread derivative. Threads can
//  either be pointed at a function or a class method, or they can be standalone
//  threads that override the eProcess() method. This one does the latter just
//  to demonstrate that.
//
//  As with most of these basic samples, this one does not create a facility object,
//  it just starts a main thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "Collect3.hpp"


// ---------------------------------------------------------------------------
//  Local data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class. We don't support
//      redirection in this program, so we can just use a console directly.
// ---------------------------------------------------------------------------
static TOutConsole  conOut;


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Collect3MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread, which is where
//  this demo program starts executing. The CIDLib_MainModule() macro
//  above points to this method as the starting thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  For these tests programs we just catch anything that might come out, for
    //  diagnostic purposes if you play around with them.
    //
    try
    {
        //
        //  Create a sorted bag of TCardinal objects. We have to provide
        //  it with an object comparator. We just use a standard one that
        //  works with anything that implements <, >, and ==.
        //
        //  We make it a thread safe collection by passing EMTStates::Safe.
        //
        TSortedBag<TCardinal> colTest
        (
            tCIDLib::eComp<TCardinal>
            , tCIDLib::ESortDirs::Ascending
            , tCIDLib::EMTStates::Safe
        );

        //
        //  Create a list of worker threads. We just create 16 of them and
        //  let them run for a while. Threads are not copyable objects, so
        //  we use an adopting by reference vector.
        //
        using TThreadList = TRefVector<TColThread>;
        const tCIDLib::TCard4 c4ThreadCount = 16;
        TThreadList colOfThreads(tCIDLib::EAdoptOpts::Adopt, c4ThreadCount);

        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4ThreadCount; c4Index++)
        {
            // Make most of them removers
            TColThread::EThreadTypes eType;
            if (!(c4Index % 6))
                eType = TColThread::EThreadTypes::Adder;
            else
                eType = TColThread::EThreadTypes::Remover;
            colOfThreads.Add(new TColThread(&colTest, eType, &conOut));
        }

        //
        //  Now start them all up. We could have started them above. But,
        //  in general, this is safer. If something happened in the loop
        //  above that caused an exception, some would have just been
        //  started for nothing. At this point we know for sure that all
        //  of them are there and ready to rock and roll.
        //
        //  Note that, since we are using a pointer wrapper, the [] operator
        //  just gets us the wrapper object. objData() gets us a reference
        //  to the object its wrapping.
        //
        //  Note we do a NC (non-const) cursor, since we have to modify the
        //  values by starting
        //
        TThreadList::TNCCursor cursThreads(&colOfThreads);
        for (; cursThreads; ++cursThreads)
            cursThreads->Start();

        //
        //  Now lets sleep for 10 seconds and let the threads work their
        //  magic.
        //
        TThread::Sleep(10000);

        //
        //  Now lets go through the list and ask each thread to shutdown
        //  and wait for them to actually die.
        //
        //  In a real program you could most likely go through first and do a non
        //  synchronized shutdown request, to start them all shutting down,
        //  then go back and do another loop to wait for them to die. But
        //  in a simple program like this where the threads should die quickly
        //  it's fine.
        //
        //  Note we need to reset the cursor here since we are reusing it
        //
        conOut << L"Asking threads to stop..." << kCIDLib::EndLn;
        cursThreads.bReset();
        for (; cursThreads; ++cursThreads)
        {
            cursThreads->ReqShutdownSync(5000);

            //
            //  And wait for it to fully die. We don't care about the
            //  return value from the thread, and we say we will wait
            //  for up to 2 seconds. This should be plenty of time since
            //  the thead is already on the way down.
            //
            cursThreads->eWaitForDeath(2000);
        }

        //
        //  Now lets get a cursor for the collection and print out the
        //  list of elements that survived.
        //
        conOut << L"The elements left in the collection are:" << kCIDLib::NewLn;

        TSortedBag<TCardinal>::TCursor cursDisplay(&colTest);
        if (cursDisplay.bIsValid())
        {
            for (; cursDisplay; ++cursDisplay)
                conOut << L"   Value: " << *cursDisplay << kCIDLib::NewLn;
            conOut << kCIDLib::EndLn;
        }
         else
        {
            conOut << L"   No elements found" << kCIDLib::NewEndLn;
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  <<  L"A CIDLib runtime error occured during processing.\n"
                <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
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
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
