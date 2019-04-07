//
// FILE NAME: CIDKernel_PerThreadData.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This file implements platform independent implementation parts of the
//  per-thread data abstraction for the platform. It uses a single thread
//  local storage slot from the host services and then maintains an array of
//  second level slots for each thread that are used to store each thread's
//  per-thread values. The array is increased as required.
//
//  Most of it can be platform independent, since the only parts that are
//  platform dependent are those that manage the TLS slots from the OS. The
//  platform dependent stuff is in the CIDKernel_PerThreadData.Cpp file in
//  each per-platform subdirectory.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  There are two slot thingies involved here, so don't get them confused.
//      We allocate *one* TLS slot from the OS. But, for each thread, that
//      one TLS slot points to a structure that holds an expandable array of
//      data slots that holds the per-thread data values for each per-thread
//      object that gets created.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TKrnlPerThreadData
//  PREFIX: kpthr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlPerThreadData: Static data members
// ---------------------------------------------------------------------------
TKrnlPerThreadData**    TKrnlPerThreadData::s_apkptdList;
tCIDLib::TCard4         TKrnlPerThreadData::s_c4CurDataSlotInd;
tCIDLib::TCard4         TKrnlPerThreadData::s_c4CurListSize;
tCIDLib::TCard4         TKrnlPerThreadData::s_c4SlotId;
TKrnlCritSec*           TKrnlPerThreadData::s_pkcrsLock;



// ---------------------------------------------------------------------------
//  TKrnlPerThreadData: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlPerThreadData::CleanUpThreadData()
{
    //
    //  Lock the critical section and ask each object is the per-thread
    //  object list to destroy its data for the calling thread.
    //
    TKrnlCritSecLocker lockInit(s_pkcrsLock);

    //
    //  Get the current thread's thread info structure. Tell the getter
    //  not to fault it in if it does not already exist. If it comes
    //  back zero, then no per-thread data was ever set on this thread
    //  and we can just return now.
    //
    TThreadInfo* pInfo;
    if (!bGetThreadInfo(pInfo, kCIDLib::False))
    {
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
        );
        return;
    }

    if (!pInfo)
        return;

    //
    //  Oh well, we've got some data. So loop through the list of per
    //  thread objects and ask them to delete their data for this thread.
    //  We can optimize this by first checking to see if any data is
    //  actually stored in the data slot first.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < s_c4CurListSize; c4Index++)
    {
        //
        //  If we are still within the number of slots on this thread's
        //  data slots array, then see if its zero. If it is, then there
        //  is nothing to delete for this one.
        //
        if (c4Index < pInfo->c4CurSlots)
        {
            if (!pInfo->apDataSlots[c4Index])
                continue;
        }

        // There is something to delete so delete it
        if (s_apkptdList[c4Index])
            s_apkptdList[c4Index]->CleanUpUserData();
    }

    //
    //  And now delete the thread's thread info structure itself and
    //  the data slot array it holds.
    //
    delete [] pInfo->apDataSlots;
    delete pInfo;
}


// ---------------------------------------------------------------------------
//  TKrnlPerThreadData: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlPerThreadData::TKrnlPerThreadData() :

    m_c4Id(kCIDLib::c4MaxCard)
{
    {
        //
        //  Lock the global lock and get the next available data slot
        //  from the global, bumping it up to the next one.
        //
        TKrnlCritSecLocker lockInit(s_pkcrsLock);

        //
        //  See if we need to expand the list of per thread objects first.
        //  If so, then allocate a new one and copy over all the existing
        //  stuff.
        //
        if (s_c4CurDataSlotInd >= s_c4CurListSize)
        {
            // Save the current list
            TKrnlPerThreadData** apkpthrTmp = s_apkptdList;

            // Allocate the new list
            s_apkptdList =
                    new TKrnlPerThreadData*[s_c4CurListSize + c4ArrayIncrement];

            //
            //  Copy over the old elements and then zero out the new ones
            //  in the list.
            //
            tCIDLib::TCard4 c4Index = 0;
            for (; c4Index < s_c4CurListSize; c4Index++)
                s_apkptdList[c4Index] = apkpthrTmp[c4Index];

            // Update the current list size
            s_c4CurListSize += c4ArrayIncrement;

            for (; c4Index < s_c4CurListSize; c4Index++)
                s_apkptdList[c4Index] = 0;

            // And clean up the old array
            delete [] apkpthrTmp;
        }

        // And now take the next data slot and bump up for the next one
        m_c4Id = s_c4CurDataSlotInd++;
    }

    // Store ourself on the per-thread dtata list now.
    s_apkptdList[m_c4Id] = this;

    // Get our thread info, faulting it in if necessary
    TThreadInfo* pInfo;
    if (!bGetThreadInfo(pInfo))
    {
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    //
    //  Make sure our data slot is zero. It should be because the array
    //  was initialized to zero and only grows, but just in case...
    //
    pInfo->apDataSlots[m_c4Id] = 0;
}

TKrnlPerThreadData::~TKrnlPerThreadData()
{
    //
    //  This should not happen until the end of the process. But, just in
    //  case, check for the existence of our slot and delete the user
    //  data if it still exists.
    //
    //  <TBD>
    //
}


// ---------------------------------------------------------------------------
//  TKrnlPerThreadData: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid* TKrnlPerThreadData::pUserData()
{
    // Get our thread info, faulting it in if necessary
    TThreadInfo* pInfo;
    if (!bGetThreadInfo(pInfo))
    {
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    //
    //  See if our data slot id is within the data slot array of this
    //  thread. If not, then we can just return 0.
    //
    if (pInfo->c4CurSlots < m_c4Id)
        return 0;

    //
    //  Otherwise, we do have data for that data slot id, so get it and
    //  return it.
    //
    return pInfo->apDataSlots[m_c4Id];
}


const tCIDLib::TVoid* TKrnlPerThreadData::pUserData() const
{
    // Get our thread info, faulting it in if necessary
    TThreadInfo* pInfo;
    if (!bGetThreadInfo(pInfo))
    {
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    //
    //  See if our data slot id is within the data slot array of this
    //  thread. If not, then we can just return 0.
    //
    if (pInfo->c4CurSlots < m_c4Id)
        return 0;

    //
    //  Otherwise, we do have data for that data slot id, so get it and
    //  return it.
    //
    return pInfo->apDataSlots[m_c4Id];
}


tCIDLib::TVoid* TKrnlPerThreadData::pUserData(tCIDLib::TVoid* const pNewData)
{
    // Get our thread info, faulting it in if necessary
    TThreadInfo* pInfo;
    if (!bGetThreadInfo(pInfo))
    {
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
        );
        TKrnlProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    //
    //  See if our data slot id is larger than the available number of
    //  slots on this thread. If so, then we need to reallocate our
    //  array.
    //
    //  We have to synchronize this in order to get the global current
    //  data slot index. This lets us expand such that we know that its
    //  big enough to hold any current data slots.
    //
    if (pInfo->c4CurSlots < m_c4Id)
    {
        // Synchronize the operation
        TKrnlCritSecLocker lockInit(s_pkcrsLock);

        tCIDLib::TVoid** apTmp = pInfo->apDataSlots;
        pInfo->apDataSlots =
                new tCIDLib::TVoid*[s_c4CurDataSlotInd + c4ArrayIncrement];

        //
        //  Copy over the old information from the old array, into the same
        //  indices in the new array.
        //
        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < pInfo->c4CurSlots; c4Index++)
            pInfo->apDataSlots[c4Index] = apTmp[c4Index];

        // Delete the old array
        delete [] apTmp;

        //  And complete the new array, zeroing out the new elements
        pInfo->c4CurSlots = s_c4CurDataSlotInd + c4ArrayIncrement;
        for (; c4Index < pInfo->c4CurSlots; c4Index++)
            pInfo->apDataSlots[c4Index] = 0;
    }

    // Get the old data out for return
    tCIDLib::TVoid* pRet = pInfo->apDataSlots[m_c4Id];

    // Put the new data into the data slot for this object
    pInfo->apDataSlots[m_c4Id] = pNewData;

    return pRet;
}

