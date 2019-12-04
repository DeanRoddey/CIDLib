//
// FILE NAME: CIDOrb_ClientConn.hpp
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
//  This is the header for the CIDOrb_ClientConn.cpp file, which implements
//  the TOrbClientConnImpl class. This class represents a single client
//  connection, i.e. the stream socket and some related state info. The client
//  connection manager class maintains a list of these. We have a spooling
//  thread that blocks on two events, one that will be triggered when
//  our socket state changes and another that will be triggered when a worker
//  thread comes back with a reply that needs to back and queues it up on
//  our reply queue.
//
//  So the thread logic is:
//
//  1.  If the socket read state changes, then we either have data to read,
//      or the client has dropped the connection.
//
//  2.  If the client dropped, we close ourself down and mark ourselves as
//      offline. The connection manager will remove us from the list when he
//      sees this has happened.
//
//      If we have data, then we call into the conn manager to have him
//      read in the data and queue it up on his work queue, which one of his
//      worker threads will pick up and start working on.
//
//  3.  When the worker thread has the reply ready, he will call us to queue
//      it up. We put this on our reply queue, and trigger the event that
//      tells our spooling thread that data is ready to return. He will
//      spool that reply back to the client.
//
//  This of course implies that we have a thread per client architecture,
//  which is just the only sane way to do it for an ORB. It is more overhead,
//  but because of the fact that multiple client threads are talking over
//  the same socket make it really difficult to do it any other way (and I've
//  tried them all by now.)
//
//  We cannot have a single spooling thread for the server because a client
//  with a flakey connection will hose the whole thing.
//
//  Unless the ORB is being shut down, we are the only one who ever shuts
//  us down. The last thing we do is set our offline flag. The manager will
//  check each connection's flag periodically and if they are offline, he
//  will delete us. We are already stopped by that point.
//
//  Any outstanding work items will just be dropped on the floor because the
//  worker thread won't be able to find us to give us the reply back. We
//  just need to clean up any queued up outgoing stuff that hasn't been sent
//  yet when we close down. Any worker thread that comes in to queue up a
//  replace after we shut down but before the manager removes us will see
//  the offline flag set and will just drop the reply on the floor and not
//  try to queue it up.
//
//  The connection manager and the worker threads will synchronize their
//  access to us, so effectively we are never being called by more than one
//  thread at a time. But we do have a little synchronization required between
//  our own spooler thread and the manager/worker threads.
//
//  The manager gives each connection object a unique numerical id. This is
//  used to find the connection when a reply is ready. The connection id is
//  put into the work item that gets queued up in the manager for processing
//  by the worker threads. And the worker thread looks us back up by our id
//  and if we are still around, he sends us the reply.
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
//   CLASS: TOrbClientConnImpl
//  PREFIX: occi
// ---------------------------------------------------------------------------
class TOrbClientConnImpl : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TOrbClientConnImpl() = delete;

        TOrbClientConnImpl
        (
                    TServerStreamSocket* const  psockThis
            , const TIPEndPoint&                ipepClient
            ,       TOrbClientConnMgr* const    poccmOwner
            , const tCIDLib::TCard8             c8ConnId
        );

        TOrbClientConnImpl(const TOrbClientConnImpl&) = delete;
        TOrbClientConnImpl(TOrbClientConnImpl&&) = delete;

        ~TOrbClientConnImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbClientConnImpl& operator=(const TOrbClientConnImpl&) = delete;
        TOrbClientConnImpl& operator=(TOrbClientConnImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bOffline() const;

        tCIDLib::TCard8 c8ConnId() const;

        const TIPEndPoint& ipepClient() const;

        tCIDLib::TVoid SendReply
        (
                    TWorkQItemPtr&          wqipToSend
        );

        tCIDLib::TVoid Shutdown();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eSpoolThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid ReadCmds();

        tCIDLib::TVoid SendReplies();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bOffline
        //      The other end has dropped the connection. The server side
        //      ORB can then drop us the next time he checks us.
        //
        //  m_c8ConnId
        //      We are given a unique connection id that is used to find us
        //      when a reply needs to be returned.
        //
        //  m_colReplyQ
        //      This is the queue that we store reples on until we can
        //      send them to the client. This one is thread safe and use it
        //      to provide the little synchronization we require.
        //
        //  m_enctLastMsg
        //      We enforce a maximum idle tiem after which we will drop the
        //      client. This is set each time we get a reply from the client.
        //
        //  m_evSocket
        //      We associate this event with our socket, so that the system
        //      will trigger it when data is available. It's one of the
        //      events we block on in the spooler thread.
        //
        //  m_evWorkAvail
        //      When worker threads call in to queue up work, the trigger
        //      this event. This is one of the events we block on.
        //
        //  m_ipepClient
        //      The end point of the client. Mostly for error reporting.
        //
        //  m_mbufIO
        //  m_strmOut
        //  m_strmIn
        //      We allocate a small memory stream that we pass in for packet
        //      reading, and use locally for packet writing. The vast majority
        //      of packets are very small and this allows us to avoid
        //      allocating a buffer probably 99% of the time (without the
        //      risk of runaway memory usage if we required this buffer to
        //      hold the biggest data size every sent in either direction on
        //      each connection.) The order of declaration here is important.
        //
        //  m_poccmOwner
        //      We get a pointer to our owning connection manager. We need
        //      this to queue up work on it for processing.
        //
        //  m_psockThis
        //      A pointer to the stream socket for this connection.
        //
        //  m_thrSpooler
        //      Our spooler thread, which is start up on the eSpoolThread
        //      method.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bOffline;
        tCIDLib::TCard8         m_c8ConnId;
        tCIDOrb_::TWorkQ        m_colReplyQ;
        tCIDLib::TEncodedTime   m_enctLastMsg;
        TEvent                  m_evSocket;
        TEvent                  m_evWorkAvail;
        TIPEndPoint             m_ipepClient;
        THeapBuf                m_mbufIO;
        TBinMBufOutStream       m_strmOut;
        TBinMBufInStream        m_strmIn;
        TOrbClientConnMgr*      m_poccmOwner;
        TServerStreamSocket*    m_psockThis;
        TThread                 m_thrSpooler;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbClientConnImpl, TObject)
};

#pragma CIDLIB_POPPACK


