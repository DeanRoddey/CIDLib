//
// FILE NAME: CIDOrb_ClientBaseHelpers_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2000
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
//  The client base class needs a number of helper classes, which we get
//  out of the main file due to size concerns.
//
//  TSrvTarget is a class that represents a connection to a particular server.
//  Any one process has only one socket to a particular server. So we have one
//  server object for each server we are connected to. Each server target has
//  a spooler thread which reads and writes the connection to that server,
//  so all send/receive ops are async. Client threads queue up their requests
//  to send, and the spooler threads do them in order. Each client proxy is
//  therefore associated with a server target, and will reference count it.
//  This insures that the target server is kept around as long as a client
//  proxy is around referencing it.
//
//  Note that, we don't really drop a server target when the last client
//  proxy drops it. We will actually put it onto a cache list. Since many
//  client threads follow a pattern of getting a client proxy, doing an
//  exchange or two, and then destroying the proxy, doing this can greatly
//  increase efficiency, since a target server is semi-heavy weight, because
//  it has two threads to spin up and tear back down. The target server
//  objects will be dropped out of the cache after a timeout period.
//
//  To queue the incoming client requestions, we use the base linked list
//  to keep things fairly low level here. So we have to create a derivative
//  of it's node class to store our data. We use it like a queue, putting
//  new items in at the tail, and the spool thread reading out at the head
//  to create a FIFO pattern.
//
//  There are two lists. The incoming client requests are put int a request
//  queue. Once it's message is spooled to the other side, it's moved over
//  to another list where it's waiting for the reply to come back. The
//  second list is just standard ref vector.
//
//  TOrbCConnWait is a small class that is used in the psrvtAddSrvRef() method
//  of the client based class. Multiple threads can be trying to connect to a
//  server at once, but only one can be allowed to do it and the others must
//  wait and be awakened when it either worked or failed (and told which
//  happened.) The threads will put one of these onto an internal waiting
//  queue (if someone else is already trying to connect to that server), and
//  the thread that is already trying the connection will, upon success or
//  failure, search the list and wake up anyone waiting. We define it here
//  since it's private to this facility and the client base header is public.
//
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
//   CLASS: TOrbCConnWait
//  PREFIX: occw
// ---------------------------------------------------------------------------
class TOrbCConnWait
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        enum class EWaitStates
        {
            // Used by the thread that does the connecting
            Connecting
            , Success
            , Failure

            // Used by the thread(s) that are waiting for the connection
            , Waiting
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbCConnWait(  const   TIPEndPoint&    ipepTarget
                        , const EWaitStates     eInitState) :

            m_eState(eInitState)
            , m_evWait(tCIDLib::EEventStates::Reset)
            , m_ipepTarget(ipepTarget)
        {
        }

        TOrbCConnWait(const TOrbCConnWait&) = delete;
        TOrbCConnWait(TOrbCConnWait&&) = delete;

        ~TOrbCConnWait() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbCConnWait& operator=(const TOrbCConnWait&) = delete;
        TOrbCConnWait& operator=(TOrbCConnWait&&) = delete;


        // -------------------------------------------------------------------
        //  Public data
        //
        //  m_eState
        //      Indicates the state. For the thread that is assigned the task
        //      of connecting, it is set to connecting and then to either
        //      success or failure when it works or fails. For the other
        //      threads they set the Waiting state.
        //
        //  m_evWait
        //      The event that the thread blocks itself on until it is awakened
        //      by the thread doing the actual connection.
        //
        //  m_ipepTarget
        //      The target address that this thread is waiting to connect to.
        // -------------------------------------------------------------------
        EWaitStates m_eState;
        TEvent      m_evWait;
        TIPEndPoint m_ipepTarget;
};
using TConnWaitList = TRefVector<TOrbCConnWait>;




// ---------------------------------------------------------------------------
//   CLASS: TOrbQNode
//  PREFIX: node
// ---------------------------------------------------------------------------
class TOrbQNode : public TDLstNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbQNode() = delete;

        TOrbQNode
        (
                    TCmdQItem* const        pcqiData
        );

        TOrbQNode(const TOrbQNode&) = delete;
        TOrbQNode(TOrbQNode&&) = delete;

        ~TOrbQNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbQNode& operator=(const TOrbQNode&) = delete;
        TOrbQNode& operator=(TOrbQNode&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TCmdQItem* pobjData() const;

        TCmdQItem* pcqiData();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pobjData
        //      The queue item we are storing. Note that we do not own these
        //      objects. These are owned by a cache of command items that just
        //      get reused over and over. They are just given to us for the
        //      time we need to process commands, then they go back into the
        //      cache for reuse.
        // -------------------------------------------------------------------
        TCmdQItem*  m_pcqiData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbQNode,TDLstNode)
};


// ---------------------------------------------------------------------------
//   CLASS: TSrvTarget
//  PREFIX: srvt
// ---------------------------------------------------------------------------
class TSrvTarget
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TCmdQueue = TDLinkedList;
        using TRepList  = TRefVector<TCmdQItem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSrvTarget() = delete;

        TSrvTarget
        (
            const   TIPEndPoint&            ipepSrv
        );

        TSrvTarget(const TSrvTarget&) = delete;
        TSrvTarget(TSrvTarget&&) = delete;

        ~TSrvTarget();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSrvTarget& operator=(const TSrvTarget&) = delete;
        TSrvTarget& operator=(TSrvTarget&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConnected();

        tCIDLib::TBoolean bInitialize();

        tCIDLib::TCard4 c4QueueCmd
        (
                    TCmdQItem* const        pcqiToQ
        );

        tCIDLib::TVoid Cleanup();


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_bReconnMode
        //      When we lose the connection, this flag is set. The spool
        //      thread uses it to decide whether to try to reconnect or
        //      poll/write on each round. And its used by the client base to
        //      avoid putting events on the send queue when its obviously
        //      already offline.
        //
        //      It is defaulted to true in the ctor, so that we immediately
        //      start trying to reconnect.
        //
        //  m_c4RefCount
        //      We only keep server connections alive as long as some clients
        //      are using it. So we reference count it.
        //
        //  m_c4SequenceId
        //      Used to send sequential packet ids to the server side so
        //      that he can know if one got dropped, and so that we can match
        //      returned replies with the cmd that caused them.
        //
        //  m_colCmdQ
        //      The queue of commands waiting to be sent to this server. It's
        //      a low level linked list, so that we can add to the start and
        //      pull them off at the end, for FIFO processing. Once they are
        //      processed, their msg sent to the server, they are moved over
        //      to the m_colRepList to wait for a reply.
        //
        //      Access to this guy must be synchronized, since it's accessed
        //      by incoming calls to drop off commands and by our internal
        //      threads.
        //
        //      Note that we don't own the command items that are placed on
        //      our list. We just reference them as we are processing them.
        //
        //  m_colRepList
        //      The list of cmd items that have gone through the command
        //      queue, i.e. they've been sent, and we are waiting for the
        //      reply to come back.
        //
        //      This list does not require synchronization, since it is only
        //      accessed internally and only by the spool thread.
        //
        //      Note that we don't own the command items that are placed on
        //      our list. We just reference them as we are processing them.
        //
        //  m_enctCacheStamp
        //      When this target is dropped by the last client proxy, it will
        //      be put on a cache list for a short period of time, since it
        //      will often be needed again soon. This stamp is set when its
        //      put on the cache list, so that it can be staled out of the
        //      list. It represents the time at which it should be removed.
        //
        //  m_errUnknown
        //      If we get an unknown exception, we have to give it back to
        //      the waiting thread. No need to make it over and over again,
        //      so just create one and re-use it.
        //
        //  m_evQEvent
        //  m_evREvent
        //      Our spool thread spends most of it's time blocked on a
        //      multi-event wait, for these two events. The Q event is
        //      triggered when a client puts an item in the queue. The R
        //      event is triggered by the socket when data is available or
        //      the socket is closed (we associate the event with the
        //      socket.) So if we come back from the multi-wait without a
        //      timeout, we have something to do.
        //
        //  m_ipepServer
        //      The end point for this server. This is how we match outgoing
        //      commands to a particular server. The host name is't included
        //      in the comparison, just the IP address info.
        //
        //  m_mtxSync
        //      We need a mutex to sync access to the command queue list and
        //      some of the other fields.
        //
        //  m_psockSrv
        //      The stream socket that we use for this server. It will be
        //      destroyed and created as needed to keep us connected.
        //
        //      The socket is purely accessed internally and only by the
        //      spooling thread, so it does not require any synchronization.
        //
        //  m_strSrvId
        //      We need to link each client proxy to the server it is working
        //      through, so we generate a unique id into each server target.
        //      We cannot let them have a pointer, because we might have to
        //      drop the server and get it again and so we want all the clients
        //      of it to still be able to find the new target object.
        //
        //  m_thrSpool
        //      Each connection has a spooler thread which handles sending
        //      queued up messages, and pulling out incoming messages and
        //      waking up the threads waiting for them.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bReconnMode;
        tCIDLib::TCard4         m_c4RefCount;
        tCIDLib::TCard4         m_c4SequenceId;
        TCmdQueue               m_colCmdQ;
        TRepList                m_colRepList;
        tCIDLib::TEncodedTime   m_enctCacheStamp;
        TError                  m_errUnknown;
        TEvent                  m_evQEvent;
        TEvent                  m_evREvent;
        TIPEndPoint             m_ipepServer;
        TMutex                  m_mtxSync;
        TStreamSocket*          m_psockSrv;
        TString                 m_strSrvId;
        TThread                 m_thrSpool;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bMakeSocket();

        tCIDLib::TVoid CancelCmdWithErr
        (
                    TCmdQItem&              cqiCan
            , const TError&                 errLostConn
        );

        tCIDLib::EExitCodes eSpoolThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid ErrorOutWaiting();

        TCmdQItem* pcqiExtractReply
        (
            const   tCIDLib::TCard4         c4SeqId
        );

        tCIDLib::TVoid PollReplies
        (
                    TThread&                thrCaller
            ,       TMemBuf&                mbufRead
            ,       TBinMBufInStream&       strmRead
        );

        tCIDLib::TVoid SendQueued
        (
                    TThread&                thrCaller
            ,       TBinMBufOutStream&      strmWrite
        );

        tCIDLib::TVoid TryReconnect();
};

#pragma CIDLIB_POPPACK


