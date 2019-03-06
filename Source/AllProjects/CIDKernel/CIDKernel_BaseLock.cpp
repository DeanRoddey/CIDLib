//
// FILE NAME: CIDKernel_BaseLock.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a very simple class, TBaseLock, that is used for
//  initialization synchronization purposes. It is all platform independent
//  since it just uses other kernel abstractions.
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
#include    "CIDKernel_.hpp"


namespace CIDKernel_BaseLock
{
    // -----------------------------------------------------------------------
    //  Local data
    //
    //  pkcrsLock
    //      This is a pointer to a critical section object. It is not
    //      actually allocated until someone tries to use the lock. An atomic
    //      compare and swap is used to create it and set the pointer.
    // -----------------------------------------------------------------------
    TKrnlCritSec*   pkcrsLock = 0;
}


// ---------------------------------------------------------------------------
//   CLASS: TBaseLock
//  PREFIX: lock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBaseLock: Contructors and Destructor
// ---------------------------------------------------------------------------
TBaseLock::TBaseLock()
{
    //
    //  See if the lock exists. If not, then we need to gen it up. We use
    //  an atomic swap to do it so there is no chance of a leak, but we might
    //  make one redundant object and have to destroy it if someone has
    //  already got it set while we were trying to get ours destructed.
    //
    //  The platform layer should insure that critical section objects are aligned
    //  appropriately for such an operation.
    //
    if (!CIDKernel_BaseLock::pkcrsLock)
    {
        // Not there so create one
        TKrnlCritSec* pkcrsCandidate = new TKrnlCritSec;
        TKrnlCritSec* pkcrsNone = 0;

        //
        //  Now do the compare and exchange. If we lose, then delete our
        //  candidate object. If we win, then ours is in there and we are
        //  done. The original value is returned, so if the return is 0, then
        //  we won. Otherwise, we have to toast our candidate.
        //
        if (TRawMem::pCompareAndExchangePtr(CIDKernel_BaseLock::pkcrsLock
                                            , pkcrsCandidate
                                            , pkcrsNone))
        {
            delete pkcrsCandidate;
        }
    }

    // Ok, we know that the lock is created, so enter the critical section
    CIDKernel_BaseLock::pkcrsLock->Enter();
}

TBaseLock::~TBaseLock()
{
    // And exit the critical section
    CIDKernel_BaseLock::pkcrsLock->Exit();
}


