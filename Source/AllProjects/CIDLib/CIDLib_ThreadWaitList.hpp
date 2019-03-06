//
// FILE NAME: CIDLib_ThreadWaitList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/04/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_ThreadWaitList.cpp file. This file
//  implements the TThreadWaitList class. Thisis a general purpose mechanism
//  that allows threads to wait on something and be woken up as required.
//
//  Of course there are other mechanisms to do this, i.e. Events, Mutexes, and
//  Semaphores. However, this class provides a special function that is often
//  required. It is like an event except that it allows one thread to be awakened
//  at a time, whereas an Event allows all waiting threads to go.
//
//  In many situations, allowing all threads to wake up is very inefficient
//  since most of them might discover that another thread beat them to whatever
//  they are competing for, and thus they must all loop back around and go back
//  to sleep on the event. The thread wait list supports this functionality
//  much more efficiently.
//
//  Each thread that blocks can provide a reason code, which is an arbitrary
//  value in general (though some common ones are provided in the constants
//  header, the c4TWLReason_xxx values.) It allows threads to block for
//  different reasons and for other threads to call in and wake up only those
//  that are waiting for a specific reason. This is common in a thread farm
//  scenario where some threads are waiting for data to arrive in a queue
//  to work on and others are waiting for space to free up to put new data
//  in.
//
//  The only special reason is c4TWLReason_All, which can be passed to the
//  release methods to release all threads no matter what the reason.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class is completely thread safe of course, since it wouldn't be
//      worth anything otherwise.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TThreadWaitList
//  PREFIX: twl
// ---------------------------------------------------------------------------
class CIDLIBEXP TThreadWaitList : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TThreadWaitList
        (
            const   tCIDLib::TCard4         c4InitAlloc = 32
        );

        TThreadWaitList(const TThreadWaitList&) = delete;

        ~TThreadWaitList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TThreadWaitList& operator=(const TThreadWaitList&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReleaseAll
        (
            const   tCIDLib::TCard4         c4Reason
        );

        tCIDLib::TBoolean bReleaseOne
        (
            const   tCIDLib::TCard4         c4Reason
        );

        tCIDLib::TBoolean bWaitOnList
        (
            const   tCIDLib::TCard4         c4Reason
            , const tCIDLib::TCard4         c4Millis
        );

        tCIDLib::TBoolean bWaitOnList
        (
                    TMtxLocker&             lockSync
            , const tCIDLib::TCard4         c4Reason
            , const tCIDLib::TCard4         c4Millis
        );

        tCIDLib::TCard4 c4ThreadsWaiting() const;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  TListItem
        //
        //  This structure represents a single waiting thread.
        //
        //  m_c4Reason
        //      The user of this class might have different reasons why a
        //      thread wants to block and wants only wake up one or all of
        //      the threads taht are waiting for a particular reason to
        //      wait up. So the threads calling bWait() can provide a reason
        //      and the release methods have variants that will only wake
        //      up threads waiting for that reason. c4MaxCard is not a valid
        //      reason and is used to indicate no particualr reason.
        //
        //      So if the release methods are called with the default max
        //      card, they will wake up any threads not waiting for any
        //      pariticular reason.
        //
        //  m_eState
        //      Indicates a couple of states, such as whether this item is
        //      in use and whether it's active or waiting for it's thread to
        //      wake up and release it.
        //
        //  m_evWait
        //      An event that we use to block this thread. It allows him to
        //      do a timed wait and then give up, and for the thread list to
        //      wake him up at any point.
        // -------------------------------------------------------------------
        enum class EStates
        {
            Free
            , Active
            , Dying
        };

        struct TListItem
        {
            TListItem();
            ~TListItem();

            tCIDLib::TCard4 m_c4Reason;
            EStates         m_eState;
            TEvent          m_evWait;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AddToList
        (
            const   tCIDLib::TCard4         c4Reason
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ActiveCount
        //      The number of items currently active, i.e. the number of
        //      threads actively blocked or in the process of exiting. When
        //      this reaches m_c4CurAlloc, we have to expand.
        //
        //  m_c4CurAlloc
        //      The current number of items we've allocated in the list.
        //
        //  m_crsSync
        //      This is the crit sec used to sync access to the thread list.
        //
        //  m_apItems
        //      This is a dynamic array of pointers to TListItem structures.
        //      This is used to track the threads currently waiting on the
        //      list. It will grow as needed.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ActiveCount;
        tCIDLib::TCard4     m_c4CurAlloc;
        TCriticalSection    m_crsSync;
        TListItem**         m_apItems;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TThreadWaitList,TObject)
};

#pragma CIDLIB_POPPACK


