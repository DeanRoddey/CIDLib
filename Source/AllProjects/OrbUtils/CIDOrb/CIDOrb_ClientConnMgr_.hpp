//
// FILE NAME: CIDOrb_ClientConnMgr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2002
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
//  This is the header for the CIDOrb_ClientConnMgr.cpp file, which implements
//  the main logic of the server side ORB. This guy contains the listener
//  thread that watches for new connections (and drops dead connections) and
//  maintains a list of client connection objects for all the currently
//  live connections.
//
//  It also provides a set of worker threads that do the actual dispatching
//  of commands and the returning of reply data to the connection objects.
//  The connections manager their own socket and do their own reading and
//  writing. We just add new ones to the list and remove dead ones and provide
//  methods for them to queue up incoming commands on us and bump some stats
//  that they need to help maintain.
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
//   CLASS: TOrbClientConnMgr
//  PREFIX: occm
// ---------------------------------------------------------------------------
class TOrbClientConnMgr : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TClientList = TRefVector<TOrbClientConnImpl>;


        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TOrbClientConnMgr
        (
            const   tCIDLib::TIPPortNum     ippnListen
            , const tCIDLib::TCard4         c4MaxClients
        );

        TOrbClientConnMgr(const TOrbClientConnMgr&) = delete;
        TOrbClientConnMgr(TOrbClientConnMgr&&) = delete;

        ~TOrbClientConnMgr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbClientConnMgr& operator=(const TOrbClientConnMgr&) = delete;
        TOrbClientConnMgr& operator=(TOrbClientConnMgr&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ClearOnlyAcceptFrom();

        tCIDOrb::EReadRes eQueueUpWork
        (
                    TThread&                thrSpooler
            ,       TServerStreamSocket&    sockThis
            , const TOrbClientConnImpl&     occiReplyTo
            ,       TMemBuf&                mbufTmp
            ,       TBinMBufInStream&       strmTmp
        );

        tCIDLib::TIPPortNum ippnListen() const;

        tCIDLib::TVoid IncDroppedPackets();

        tCIDLib::TVoid OnlyAcceptFrom
        (
            const   TIPAddress&             ipaSource
        );

        tCIDLib::TVoid Terminate();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bMakeListener
        (
                    tCIDLib::TBoolean&      bFirstTime
        );

        tCIDLib::TVoid DoChecks();

        tCIDLib::EExitCodes eListenerThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eWorkerThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        TOrbClientConnImpl* poccFindConn
        (
            const   tCIDLib::TCard8         c8ConnId
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bLimitAccess
        //      If this is set, then we've been told to limit access to a
        //      particular client address, in which case m_ipaOnlyAcceptFrom
        //      holds that address. Else, we accept from any source.
        //
        //  m_c4LoopCnt
        //      We want to do some things in the listener thread every so many
        //      times through the loop, so we use this counter.
        //
        //  m_c4MaxClients
        //      The max number of clients that we will serve at once. We have
        //      a default which is the max sockets we can monitor at once on
        //      the current platform. The client code can set it to something
        //      lower during server side init if desired, but never more than
        //      that.
        //
        //      This is not the number of simultaneous command requests that
        //      can be fielded, but the number of client processes that can
        //      be connected to this process. Each of those processes can have
        //      multiple threads sending requests at once.
        //
        //      Note that we also dump this into a stats cache item, defined
        //      below, so that it's available to the outside world to see.
        //
        //  m_colClientList
        //      This is the current list of client connections. The listener
        //      thread adds to the list when new connections arrive and
        //      remove them when the connections indicate that they have
        //      shut down.
        //
        //  m_colThreads
        //      This is a vector of thread objects, which hold our worker
        //      threads. We create it big enough to hold the number of
        //      clients we were told to support by the clinet code. But it is
        //      just a general thread pool, and there is no connection between
        //      worker threads and particular clients. These threads just
        //      wait for work to show up.
        //
        //  m_colWorkQ
        //      The client connection objects queue up work here, and our
        //      worker threads pull them off to process. This one has to be
        //      thread safe since we cannot block on the main mutex while we
        //      wait for stuff to arrive, since the other threads wouldn't
        //      be able to then lock and put something into it.
        //
        //  m_ipaOnlyAcceptFrom
        //      We can be told to limit access to just a particular source
        //      address. If the m_bLimitAccess member is set, then this
        //      holds the valid address we can accept from. Else any address
        //      is fine.
        //
        //  m_ippnListenOn
        //      The port we were told to listen on.
        //
        //  m_mtxSync
        //      A mutex to provide any sync that we need.
        //
        //  m_psocklServer
        //      This is our listener. It will created during init, once
        //      we know what our port number is. Its used by the listener
        //      thread. It is a pointer because it is only used if the server
        //      side of the ORB is initialized.
        //
        //  m_sciActiveCmds
        //      Each worker thread bumps this when he's processing a command,
        //      and decrements it when done. So it represents how many threads
        //      are actually processing commands at the current time.
        //
        //  m_sciClientHWMark
        //      The maximum clients that have been connected at once so far.
        //
        //  m_sciCurClients
        //      The current number of connected clients, which is just a
        //      reflection of the size of m_colClientList, for stats purposes.
        //
        //  m_sciDroppedRetPacks
        //      The number of packets queued for return that were dropped on
        //      the floor because the client was gone by that time.
        //
        //  m_sciMaxClients
        //      The max number of clients that we will serve at once, see
        //      m_c4MaxClients above.
        //
        //  m_sciQueuedCmds
        //      The number of commands currently queued up for processing,
        //      which is just a reflection of the size of m_colWorkQ, for
        //      stats purposes.
        //
        //  m_sciWorkerThreads
        //      The number of worker threads we've spun up so far. This is
        //      a reflectino of the elelements in m_colThreads above, for
        //      stats purposes.
        //
        //  m_thrListener
        //      Our listener thread. It just watches for connections from the
        //      client and adds them top the active client list.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bLimitAccess;
        tCIDLib::TCard4         m_c4LoopCnt;
        tCIDLib::TCard4         m_c4MaxClients;
        tCIDLib::TCard8         m_c8NextConnId;
        TClientList             m_colClientList;
        tCIDLib::TThreadList    m_colThreads;
        tCIDOrb_::TWorkQ        m_colWorkQ;
        TIPAddress              m_ipaOnlyAcceptFrom;
        tCIDLib::TIPPortNum     m_ippnListenOn;
        TMutex                  m_mtxSync;
        TSocketListener*        m_psocklServer;
        TStatsCacheItem         m_sciActiveCmds;
        TStatsCacheItem         m_sciClientHWMark;
        TStatsCacheItem         m_sciCurClients;
        TStatsCacheItem         m_sciDroppedRetPacks;
        TStatsCacheItem         m_sciMaxClients;
        TStatsCacheItem         m_sciQueuedCmds;
        TStatsCacheItem         m_sciWorkerThreads;
        TThread                 m_thrListener;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbClientConnMgr,TObject)
};

#pragma CIDLIB_POPPACK


