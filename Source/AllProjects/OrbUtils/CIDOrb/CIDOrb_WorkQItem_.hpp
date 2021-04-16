//
// FILE NAME: CIDOrb_WorkQItem_.hpp
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
//  This is the header for the CIDOrb_WorkQItem.cpp file, which implements the
//  class that represents an item in the server side read/write queues. When
//  a command comes in from a client, the connection object puts it on the
//  work queue in the manager object where it can be picked up by a worker
//  thread and processed, then given back to the connection object for
//  spooling back to the client. Once done (or failed) the connection object
//  will destroy it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TWorkQItem
//  PREFIX: wqi
// ---------------------------------------------------------------------------
class TWorkQItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4UsedQItems();


        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TWorkQItem() = delete;

        TWorkQItem
        (
            const   tCIDLib::TCard4         c4InitSz
        );

        TWorkQItem(const TWorkQItem&) = delete;
        TWorkQItem(TWorkQItem&&) = delete;

        ~TWorkQItem() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWorkQItem& operator=(const TWorkQItem&) = delete;
        TWorkQItem& operator=(TWorkQItem&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TCard4 c4BufSize() const
        {
            return m_ocmdThis.c4BufSize();
        }

        [[nodiscard]] tCIDLib::TCard8 c8ConnId() const;

        [[nodiscard]] tCIDLib::TEncodedTime enctElapsed() const;

        [[nodiscard]] const TIPEndPoint& ipepClient() const;

        TOrbCmd& ocmdThis();

        const TOrbCmd& ocmdThis() const;

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4Size
        );

        tCIDLib::TVoid SetConnInfo
        (
            const   tCIDLib::TCard8         c8ConnId
            , const TIPEndPoint&            ipepClient
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8ConnId
        //      Each client connection object is given a new sequential value.
        //      When a work item is queued up by a connection object, its
        //      id is stored here so that the worker thread in the conn mgr
        //      that processes it can figure out where to send it back. Zero
        //      is never a valid id, so if its zero, it's never been set.
        //
        //  m_enctStart
        //      This is set to the current time when it is queued up for
        //      processing. It can be used to time the flow of an item through
        //      the ORB. The enctElapsed() will return the difference between
        //      the current time and this stored start time.
        //
        //  m_ipepClient
        //      We are given this for later reporting purposes, since the
        //      actual connection object might already be gone.
        //
        //  m_ocmdThis
        //      The ORB command object that holds the incoming command data
        //      and the outgoing reply data.
        // -------------------------------------------------------------------
        tCIDLib::TCard8         m_c8ConnId;
        tCIDLib::TEncodedTime   m_enctStart;
        TIPEndPoint             m_ipepClient;
        TOrbCmd                 m_ocmdThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWorkQItem,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TWorkQItemPtr
//  PREFIX: wqip
//
//  For convenience, we create a derivative of the simple pool pointer class which
//  can gen up instances of itself since the public ctor has access to the pool.
// ---------------------------------------------------------------------------
class TWorkQItemPtr : public TSimplePoolPtr<TWorkQItem>
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TWorkQItemPtr() = default;

        TWorkQItemPtr
        (
            const   tCIDLib::TCard4         c4InitSz
        );

        TWorkQItemPtr(const TWorkQItemPtr&) = default;
        TWorkQItemPtr(TWorkQItemPtr&&) = default;

        ~TWorkQItemPtr() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWorkQItemPtr& operator=(const TWorkQItemPtr&) = default;
        TWorkQItemPtr& operator=( TWorkQItemPtr&&) = default;
};

#pragma CIDLIB_POPPACK
