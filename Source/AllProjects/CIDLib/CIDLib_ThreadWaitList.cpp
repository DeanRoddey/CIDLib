//
// FILE NAME: CIDLib_ThreadWaitList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/04/2000
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
//  This file implements the TThreadWaitList class.
//
// CAVEATS/GOTCHAS:
//
//  1)  The private methods assume that the public callers have already locked
//      the list critical section.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TThreadWaitList,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TThreadWaitList::TListItem
//  PREFIX: twi
// ---------------------------------------------------------------------------
TThreadWaitList::TListItem::TListItem() :

    m_c4Reason(kCIDLib::c4TWLReason_None)
    , m_eState(TThreadWaitList::EStates::Free)
    , m_evWait()
{
}

TThreadWaitList::TListItem::~TListItem()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TThreadWaitList
//  PREFIX: twl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TThreadWaitList: Constuctors and Destructor
// ---------------------------------------------------------------------------
TThreadWaitList::TThreadWaitList(const tCIDLib::TCard4 c4InitAlloc) :

    m_c4ActiveCount(0)
    , m_c4CurAlloc(c4InitAlloc)
    , m_apItems(nullptr)
{
    // Allocate the initial list item
    m_apItems = new TListItem*[m_c4CurAlloc];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
        m_apItems[c4Index] = new TListItem;
}

TThreadWaitList::~TThreadWaitList()
{
    // Release all of the waiting threads
    try
    {
        //
        //  Call release all and pass the special all reasons code so that
        //  we wake them all up.
        //
        bReleaseAll(kCIDLib::c4TWLReason_All);

        //
        //  Spin quickly until the active count goes zero. Otherwise, we can
        //  delete ourself while the threads are still getting back to their
        //  items and clearing out. This will generally happen very quickly.
        //
        tCIDLib::TCard4 c4Count = 0;
        while (m_c4ActiveCount)
        {
            TThread::Sleep(5);
            c4Count++;
            if (c4Count > 200)
                break;
        }

        // Clean up the items and the listarray itself
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
            delete m_apItems[c4Index];
        delete [] m_apItems;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TThreadWaitList: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TThreadWaitList::bReleaseAll(const tCIDLib::TCard4 c4Reason)
{
    //
    //  Lock the sync mutex while we do this. Not only does this provide the
    //  obvious synchronization, it also prevents any other threads from
    //  getting back into Wait(). Otherwise, we might never complete.
    //
    TCritSecLocker crslSync(&m_crsSync);

    // If there aren't any waiting, return false now
    if (!m_c4ActiveCount)
        return kCIDLib::False;

    //
    //  Iterate the list and unblock the active items that match the indicated
    //  reason, or all of them if the passed reason is the standard 'All'
    //  reasons value.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
    {
        if ((m_apItems[c4Index]->m_eState == TThreadWaitList::EStates::Active)
        &&  ((m_apItems[c4Index]->m_c4Reason == c4Reason)
        ||   (c4Reason == kCIDLib::c4TWLReason_All)))
        {
            m_apItems[c4Index]->m_eState = EStates::Dying;
            m_apItems[c4Index]->m_evWait.Trigger();
        }
    }

    //
    //  Note that we don't bump down the active count since the thread is
    //  still in the list until it wakes up and removes itself.
    //
    return kCIDLib::True;
}


tCIDLib::TBoolean TThreadWaitList::bReleaseOne(const tCIDLib::TCard4 c4Reason)
{
    // Lock the sync mutex while we do this
    TCritSecLocker crslSync(&m_crsSync);

    //
    //  Wake up the first thread on the list that is waiting for the
    //  indicated reason, or just the first active one if the passed code
    //  indicates all reasons.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
    {
        //
        //  Break out on the first one. Note that we don't bump down the
        //  active count since the thread is still in the list until it wakes
        //  up and removes itself.
        //
        if ((m_apItems[c4Index]->m_eState == TThreadWaitList::EStates::Active)
        &&  ((m_apItems[c4Index]->m_c4Reason == c4Reason)
        ||   (c4Reason == kCIDLib::c4TWLReason_All)))
        {
            m_apItems[c4Index]->m_eState = TThreadWaitList::EStates::Dying;
            m_apItems[c4Index]->m_evWait.Trigger();
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TCard4 TThreadWaitList::c4ThreadsWaiting() const
{
    // Lock the sync mutex while we do this
    TCritSecLocker crslSync(&m_crsSync);
    return m_c4ActiveCount;
}


tCIDLib::TBoolean
TThreadWaitList::bWaitOnList(const  tCIDLib::TCard4 c4Reason
                            , const tCIDLib::TCard4 c4Millis)
{
    //
    //  Lock and add ourself to the list. We get back our index into the
    //  list, setup and ready to wait on.
    //
    tCIDLib::TCard4 c4MyIndex = kCIDLib::c4MaxCard;
    {
        TCritSecLocker crslSync(&m_crsSync);
        c4MyIndex = c4AddToList(c4Reason);
    }

    TListItem* ptwiMine = m_apItems[c4MyIndex];
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        bRet = ptwiMine->m_evWait.bWaitFor(c4Millis);
    }

    catch(TError& errToCatch)
    {
        // Lock and clean up again then rethrow
        {
            TCritSecLocker crslSync(&m_crsSync);
            ptwiMine->m_eState = EStates::Free;
            m_c4ActiveCount--;
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Lock the list again while we clean ourselves up
    {
        TCritSecLocker crslSync(&m_crsSync);
        ptwiMine->m_eState = EStates::Free;
        m_c4ActiveCount--;
    }
    return bRet;
}


tCIDLib::TBoolean
TThreadWaitList::bWaitOnList(       TLocker&        lockSync
                            , const tCIDLib::TCard4 c4Reason
                            , const tCIDLib::TCard4 c4Millis)
{
    //
    //  Lock and add ourself to the list. We get back our index into the
    //  list, setup and ready to wait on.
    //
    tCIDLib::TCard4 c4MyIndex = kCIDLib::c4MaxCard;
    {
        TCritSecLocker crslSync(&m_crsSync);
        c4MyIndex = c4AddToList(c4Reason);
    }

    TListItem* ptwiMine = m_apItems[c4MyIndex];
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        // Release the passed mutex and then block
        lockSync.Release();
        bRet = ptwiMine->m_evWait.bWaitFor(c4Millis);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Lock the list and remove ourselves from the list
        {
            TCritSecLocker crslSync(&m_crsSync);
            ptwiMine->m_eState = EStates::Free;
            m_c4ActiveCount--;
        }

        // Get the lock back and throw
        lockSync.Lock();
        throw;
    }

    // Lock the list again while we clean ourselves up
    {
        TCritSecLocker crslSync(&m_crsSync);
        ptwiMine->m_eState = EStates::Free;
        m_c4ActiveCount--;
    }

    // Reaquire the mutex
    lockSync.Lock();

    return bRet;
}



// ---------------------------------------------------------------------------
//  TThreadWaitList: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TThreadWaitList::c4AddToList(const tCIDLib::TCard4 c4Reason)
{
    tCIDLib::TCard4 c4Index;

    // If we need to expand the list, then do it now
    if (m_c4ActiveCount == m_c4CurAlloc)
    {
        const tCIDLib::TCard4 c4NewSz = m_c4CurAlloc
                                        + tCIDLib::TCard4(m_c4CurAlloc * 0.5);
        TListItem** apTmp = new TListItem*[c4NewSz];

        // Copy over the existing stuff
        for (c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
        {
            #pragma warning(suppress : 6386) // We know the index is OK
            apTmp[c4Index] = m_apItems[c4Index];
        }

        // And allocate the new ones
        for (; c4Index < c4NewSz; c4Index++)
            apTmp[c4Index] = new TListItem;

        // And free the old array and store the new one
        delete [] m_apItems;
        m_apItems = apTmp;

        //
        //  And clearly the new index will be the old current alloc, so set
        //  it and then update the current alloc.
        //
        c4Index = m_c4CurAlloc;
        m_c4CurAlloc = c4NewSz;
    }
     else
    {
        // Find the first free slot
        for (c4Index = 0; c4Index < m_c4CurAlloc; c4Index++)
        {
            if (m_apItems[c4Index]->m_eState == EStates::Free)
                break;
        }
    }

    //
    //  We found our slot, so mark it taken, and bump the active count. Be
    //  sure to reset the mutex before we set it active.
    //
    #pragma warning(suppress : 6385) // We made sure the index is good above
    m_apItems[c4Index]->m_evWait.Reset();
    m_apItems[c4Index]->m_eState = EStates::Active;
    m_apItems[c4Index]->m_c4Reason = c4Reason;
    m_c4ActiveCount++;

    // Return the index we added it at
    return c4Index;
}


