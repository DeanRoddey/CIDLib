//
// FILE NAME: CIDKernel_PerThreadData_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/09/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation of the per-thread
//  data support class.
//
//
// CAVEATS/GOTCHAS:
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
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermPerThread(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        // Allocate our per-thread slot id
        tCIDLib::TOSErrCode errc
            = ::pthread_key_create(reinterpret_cast<pthread_key_t*>(&TKrnlPerThreadData::__c4SlotId), 0);
        if (errc)
        {
            TKrnlError::SetLastHostError(errc);
            return kCIDLib::False;
        }

        // Allocate the critical section
        TKrnlPerThreadData::__pkcrsLock = new TKrnlCritSec;
    }
    else
    {
        tCIDLib::TOSErrCode errc = ::pthread_key_delete(TKrnlPerThreadData::__c4SlotId);
        if (errc)
        {
            TKrnlError::SetLastHostError(errc);
            return kCIDLib::False;
        }

        delete TKrnlPerThreadData::__pkcrsLock;
        TKrnlPerThreadData::__pkcrsLock = 0;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlPerThreadData: Private, methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlPerThreadData::__bGetThreadInfo(TKrnlPerThreadData::TThreadInfo*&  pToFill
                                    , const tCIDLib::TBoolean           bFaultIn)
{
    //
    //  First, see if we have any thread info for the calling thread. If not,
    //  then we need to fault one in if requested.
    //
    pToFill = reinterpret_cast<TThreadInfo*>(::pthread_getspecific(__c4SlotId));
    if (!pToFill && bFaultIn)
    {
        // Synchronize the operation
        TKrnlCritSecLocker lockInit(__pkcrsLock);

        //
        //  Allocate a new slot info and initialize it. Allocate an
        //  initial thread array that is the array increment plus the
        //  current data slot index.
        //
        pToFill = new TThreadInfo;
        pToFill->c4CurSlots = __c4ArrayIncrement + __c4CurDataSlotInd;
        pToFill->apDataSlots = new tCIDLib::TVoid*[pToFill->c4CurSlots];

        for (tCIDLib::TCard4 c4Index = 0; c4Index < pToFill->c4CurSlots; c4Index++)
            pToFill->apDataSlots[c4Index] = 0;

        // And store away our slot info in our thread's copy of the OS slot
        if (::pthread_setspecific(__c4SlotId, pToFill))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcPTD_SetSlot);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}
