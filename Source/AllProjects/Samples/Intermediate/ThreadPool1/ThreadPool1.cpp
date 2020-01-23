//
// FILE NAME: ThreadPool.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2019
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
//  This is the main module for a sample program that demonstrates how to create
//  a simpel thread pool. So you can start up a number of threads that will just
//  wait for something to do, by blocking on a queue. When you put something into
//  the queue the next available thread grabs it and processes it.
//
//  In this case, we create a 'facility class' which is very common in CIDLib,
//  and represents the library/executable. Many of the samples don't create one
//  because they are very simple, but this one warrants it. It provides a very
//  convenient way to coordinate everything. The main program thread is started
//  on this object, and the worker threads are as well. So they all have a common
//  place to share data.
//
//  In this case, the only sync required is the queue, which is shared by the
//  main thread and the pool threads. FOr this type of application the queue
//  must be a thread safe one, since it has to do a lot of work to safely
//  coordinate the adding and removing of items in the queue. It allows both the
//  worker threads to wait for work, and for the threads adding work to the
//  queue (our main one in this case) to block if the queue gets to a size that
//  it wants to throttle to. When a slot becomes available it will be unblocked
//  and can add more items.
//
//  Since there's only one thread adding items, it will always succeed at adding
//  at least one item once it is told there is space in the queue. If there were
//  multiple threads adding items, they may get beaten to the punch.
//
//  The work items are either some single class which are added by value, or
//  a polymorphic family of work items that the threads us via an abstract base
//  class. We do the former here for this simple demonstration, in which we just
//  queue up values that the worker threads add to a shared accumulator. We then
//  wait for the queue to be emptied, then we display the result.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. We just need the the core CIDLib functionality.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TFacThreadPool1
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacThreadPool1 : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Class constants
        // -------------------------------------------------------------------
        static constexpr tCIDLib::TCard4 c4ThreadCnt = 4;


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TFacThreadPool1() :
            TFacility(L"ThreadPool1", tCIDLib::EModTypes::Exe, 1, 0, 0, tCIDLib::EModFlags::None)
            , m_colWorkQ(tCIDLib::EMTStates::Safe)
            , m_colWorkers(tCIDLib::EAdoptOpts::Adopt)
        {
        }

        // Make the code analyzer happy
        TFacThreadPool1(const TFacThreadPool1&) = delete;
        TFacThreadPool1(TFacThreadPool1&&) = delete;

        ~TFacThreadPool1() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------

        // Make the code analyzer happy
        TFacThreadPool1& operator=(const TFacThreadPool1&) = delete;
        TFacThreadPool1& operator=(TFacThreadPool1&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virutal methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread(TThread& thrThis, tCIDLib::TVoid*)
        {
            thrThis.Sync();

            TOutConsole conOut;
            conOut << L"Starting worker threads" << kCIDLib::EndLn;

            // Create and start our worker threads, pointed at our worker method below
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ThreadCnt; c4Index++)
            {
                TThread* pthrNew = new TThread
                (
                    facCIDLib().strNextThreadName(L"ThreadPool1_")
                    , TMemberFunc<TFacThreadPool1>(this, &TFacThreadPool1::eWorkerThread)
                );
                m_colWorkers.Add(pthrNew);
                pthrNew->Start();
            }

            conOut << L"Loading up work" << kCIDLib::EndLn;

            //
            //  Just add 100 work items to the queue, setting the value to the current
            //  loop value.
            //
            TWorkItem wiCur;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < 100; c4Index++)
            {
                wiCur.c4ToAdd = c4Index;
                m_colWorkQ.objAdd(wiCur);
            }

            // Wait for the queue to be emptied. Not what a real app would do of course
            conOut << L"Waiting for completion" << kCIDLib::EndLn;
            while (!m_colWorkQ.bIsEmpty())
                TThread::Sleep(100);

            // Ask the worker threads to stop and wait for them to end
            m_colWorkers.bForEachNC
            (
                [](TThread& thrCur)
                {
                    thrCur.ReqShutdownSync();
                    thrCur.eWaitForDeath(2000);
                    return kCIDLib::True;
                }
            );

            // Display the total
            conOut << L"The total is: " << m_scntTotal.c4Value() << kCIDLib::NewEndLn;

            return tCIDLib::EExitCodes::NotFound;
        }


    private :
        // -------------------------------------------------------------------
        //  We need a little class for our work items. For this simple example
        //  it's just a count to add to the total.
        // -------------------------------------------------------------------
        struct TWorkItem
        {
            tCIDLib::TCard4 c4ToAdd;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eWorkerThread(TThread& thrThis, tCIDLib::TVoid*)
        {
            thrThis.Sync();

            TWorkItem wiCur;
            while (!thrThis.bCheckShutdownRequest())
            {
                //
                //  Wait a bit for a work item to process. If we get one, then we
                //  process it by adding it to the total.
                //
                if (m_colWorkQ.bGetNext(wiCur, 100))
                    m_scntTotal.c4AddTo(wiCur.c4ToAdd);
            }
            return tCIDLib::EExitCodes::NotFound;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colWorkQ
        //      Our work queue, which we create thread safe.
        //
        //  m_colWorkers
        //      Our list of worker threads that we start. It adopts the thread
        //      objects.
        //
        //  m_scntTotal
        //      A total that our worker threads accumulate into.
        // -------------------------------------------------------------------
        TQueue<TWorkItem>       m_colWorkQ;
        TRefVector<TThread>     m_colWorkers;
        TSafeCard4Counter       m_scntTotal;
};


// ----------------------------------------------------------------------------
//  Create the facility object and start the main thread on it
// ----------------------------------------------------------------------------
TFacThreadPool1 facThreadPool1;
CIDLib_MainModule
(
    TThread
    (
        L"MainThread"
        , TMemberFunc<TFacThreadPool1>(&facThreadPool1, &TFacThreadPool1::eMainThread)
    )
)

