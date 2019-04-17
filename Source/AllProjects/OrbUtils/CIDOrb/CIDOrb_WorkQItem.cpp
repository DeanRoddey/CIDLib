//
// FILE NAME: CIDOrb_WorkQItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/2002
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
//  This is implementation file for the work queue item class.
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
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TWorkQItem,TObject)



// ---------------------------------------------------------------------------
//  A pool for work items, which are used and dropped really quickly, so we
//  don't want to pay the price.
// ---------------------------------------------------------------------------
class TWorkQItemPool : public TSimplePool<TWorkQItem>
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWorkQItemPool() :

            TSimplePool
            (
                1024, L"ORB Work Item Pool", tCIDLib::EMTStates::Safe
            )
        {
        }

        TWorkQItemPool(const TWorkQItemPool&) = delete;

        ~TWorkQItemPool() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWorkQItemPool& operator=(const TWorkQItemPool&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ElemSize(const TWorkQItem& wqiSrc) override
        {
            return wqiSrc.c4BufSize();
        }

        // Create a new string with the requested size
        TWorkQItem* pelemMakeNew(const tCIDLib::TCard4 c4Size) override
        {
            return new TWorkQItem(c4Size);
        }

        tCIDLib::TVoid PrepElement(TWorkQItem& wqiTar, const tCIDLib::TCard4 c4Size) override
        {
            wqiTar.Reset(c4Size);
        }
};


//
//  Create a static work item pool for our internal use. All the work items are
//  gotten via the work queue item pointer class, so we can keep this all internal
//  here.
//
namespace CIDOrb_WorkQItem
{
    TWorkQItemPool  splWorkQPool;
}



// ---------------------------------------------------------------------------
//   CLASS: TWorkQItem
//  PREFIX: rwqi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWorkQItem: Constructor and Destructor
// ---------------------------------------------------------------------------
TWorkQItem::TWorkQItem(const tCIDLib::TCard4 c4InitSz) :

    m_c8ConnId(0)
    , m_enctStart(TTime::enctNow())
    , m_ocmdThis(c4InitSz)
{
}

TWorkQItem::~TWorkQItem()
{
}


// ---------------------------------------------------------------------------
//  TWorkQItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Get the connection id, which identifies the connection that we have
//  to return the reply to.
//
tCIDLib::TCard8 TWorkQItem::c8ConnId() const
{
    return m_c8ConnId;
}


// Return the difference between now and time this item was queued
tCIDLib::TEncodedTime TWorkQItem::enctElapsed() const
{
    return TTime::enctNow() - m_enctStart;
}


// Provides access to the client side end point, for reporting purposes
const TIPEndPoint& TWorkQItem::ipepClient() const
{
    return m_ipepClient;
}


// Provides access to the ORB command object we are processing
TOrbCmd& TWorkQItem::ocmdThis()
{
    return m_ocmdThis;
}

const TOrbCmd& TWorkQItem::ocmdThis() const
{
    return m_ocmdThis;
}


// Mostly to support the pool
tCIDLib::TVoid TWorkQItem::Reset(const tCIDLib::TCard4  c4Size)
{
    MRefCounted::ResetMRefCount();
    m_ocmdThis.Reset(c4Size);
}


// Set up info about the connection we are queued up for
tCIDLib::TVoid
TWorkQItem::SetConnInfo(const   tCIDLib::TCard8 c8ConnId
                        , const TIPEndPoint&    ipepClient)
{
    m_c8ConnId = c8ConnId;
    m_ipepClient = ipepClient;
}



// ---------------------------------------------------------------------------
//   CLASS: TWorkQItemPtr
//  PREFIX: wqip
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWorkQItemPtr: Constructor and Destructor
// ---------------------------------------------------------------------------
TWorkQItemPtr::TWorkQItemPtr(const tCIDLib::TCard4 c4InitSz) :

    TSimplePoolPtr(&CIDOrb_WorkQItem::splWorkQPool, c4InitSz)
{
}
