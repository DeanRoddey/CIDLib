//
// FILE NAME: CIDKernel_Signals.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/25/1999
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
//  This file implements the TKrnlSignals namespace.
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
//  Local types
//
//  TListNode
//      This structure defines the nodes in the reverse linked list of
//      handlers.
// ---------------------------------------------------------------------------
struct TListNode
{
    MSignalHandler*     pmshToCall;
    tCIDLib::TCard4     c4HandlerId;
    TListNode*          pnodePrev;
};



namespace CIDKernel_Signals
{
    // -----------------------------------------------------------------------
    //  Local data
    //
    //  pnodeTail
    //      This is the tail of the reversed linked list of signal handlers.
    //      If its zero, then the list is empty.
    //
    //  c4CurId
    //      A counter used to issue handlers ids when new handlers are
    //      installed.
    // -----------------------------------------------------------------------
    tCIDLib::TCard4  c4CurId = 1;
    TKrnlCritSec*    pkcrsLock;
    TListNode*       pnodeTail = 0;
}



// ---------------------------------------------------------------------------
//  Init handler, which is a member of TCIDKrnlModule
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermSigHandler(const tCIDLib::EInitTerm eState)
{
    // We only have pre-constructor init
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        //
        //  Set up our local data. We have to create a kernel critical
        //  section to be used to serialize access to other local data.
        //
        CIDKernel_Signals::pkcrsLock = new TKrnlCritSec;

        // Call the platform init method
        TKrnlSignals::PlatformInit();
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//   CLASS: MSignalHandler
//  PREFIX: msh
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MSignalHandler: Destructor
// ---------------------------------------------------------------------------
MSignalHandler::~MSignalHandler()
{
}


// ---------------------------------------------------------------------------
//  MSignalHandler: Hidden constructors and operators
// ---------------------------------------------------------------------------
MSignalHandler::MSignalHandler()
{
}

MSignalHandler::MSignalHandler(const MSignalHandler&)
{
}

tCIDLib::TVoid MSignalHandler::operator=(const MSignalHandler&)
{
}




// ---------------------------------------------------------------------------
//  TKrnlSignals namespace methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSignals::bCallHandlers(const tCIDLib::ESignals eSignal)
{
    // Serialize access to local handler list data
    TKrnlCritSecLocker kcrslSync(CIDKernel_Signals::pkcrsLock);

    // If the list is empty, then obviously just return false now
    if (!CIDKernel_Signals::pnodeTail)
        return kCIDLib::False;

    //
    //  Search back through the list for a handler that handles this signal.
    //  If we find one call it and see what it returns. If it returns true,
    //  then we are done, else keep searching.
    //
    tCIDLib::TBoolean bRes = kCIDLib::False;
    TListNode* pnodeCur = CIDKernel_Signals::pnodeTail;
    while (pnodeCur)
    {
        // Call the virtual method of the handler object in this entry
        bRes = pnodeCur->pmshToCall->bHandleSignal(eSignal);

        // If it returned true, then don't call any more
        if (bRes)
            break;

        // Move to the previous element
        pnodeCur = pnodeCur->pnodePrev;
    }
    return bRes;
}


tCIDLib::TBoolean
TKrnlSignals::bRemoveHandler(       MSignalHandler* const   pmshToRemove
                            , const tCIDLib::TCard4         c4HandlerId)
{
    // Serialize access to local handler list data
    TKrnlCritSecLocker kcrslSync(CIDKernel_Signals::pkcrsLock);

    //
    //  Search through the list for a node that holds this handler and
    //  this handler id.
    //
    TListNode* pnodeCur = CIDKernel_Signals::pnodeTail;
    TListNode* pnodeOld = 0;
    while (pnodeCur)
    {
        if ((pnodeCur->c4HandlerId == c4HandlerId)
        &&  (pnodeCur->pmshToCall == pmshToRemove))
        {
            //
            //  This is our guy, so we need to make our previous node pointer
            //  be the previous node of the old node. If the old node is zero,
            //  then this is the tail, so we just make the previous node
            //  the tail.
            //
            if (pnodeOld)
                pnodeOld->pnodePrev = pnodeCur->pnodePrev;
             else
                CIDKernel_Signals::pnodeTail = pnodeCur->pnodePrev;

            // And now delete the current node and return success
            delete pnodeCur;
            return kCIDLib::True;
        }

        //
        //  Set the current node as the old node and then move the current
        //  node backwards.
        //
        pnodeOld = pnodeCur;
        pnodeCur = pnodeCur->pnodePrev;
    }

    // We never found this entry, so an error
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcSig_UnknownSigHandler);
    return kCIDLib::False;
}


tCIDLib::TCard4 TKrnlSignals::c4AddHandler(MSignalHandler* const pmshToAdd)
{
    // Serialize access to local handler list data
    TKrnlCritSecLocker kcrslSync(CIDKernel_Signals::pkcrsLock);

    //
    //  Create a new element and fill it in. Then add it at the end of the
    //  list. We have to deal with the two scenarios where the list is
    //  empty and where its not empty.
    //
    TListNode* pNewElem = new TListNode;
    pNewElem->pmshToCall = pmshToAdd;

    // Give this one the next available id
    pNewElem->c4HandlerId = CIDKernel_Signals::c4CurId++;

    if (!CIDKernel_Signals::pnodeTail)
    {
        // Just zero out the previous node and store it as the tail
        pNewElem->pnodePrev = 0;
        CIDKernel_Signals::pnodeTail = pNewElem;
    }
     else
    {
        //
        //  We need to store the current tail as the new element's previous
        //  node, and the set the new node as the tail node.
        //
        pNewElem->pnodePrev = CIDKernel_Signals::pnodeTail;
        CIDKernel_Signals::pnodeTail = pNewElem;
    }

    // Return the handler id of this element
    return pNewElem->c4HandlerId;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSignalJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSignalJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSignalJanitor::TKrnlSignalJanitor(MSignalHandler* const pmshToAdd) :

    m_c4HandlerId(0)
    , m_pmshToAdd(pmshToAdd)
{
    // Add it and store the id
    m_c4HandlerId = TKrnlSignals::c4AddHandler(pmshToAdd);
}


TKrnlSignalJanitor::~TKrnlSignalJanitor()
{
    // And remove the handler we added
    TKrnlSignals::bRemoveHandler(m_pmshToAdd, m_c4HandlerId);
}


