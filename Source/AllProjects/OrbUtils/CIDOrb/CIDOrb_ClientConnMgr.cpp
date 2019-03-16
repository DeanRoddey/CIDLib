//
// FILE NAME: CIDOrb_ClientConnMgr.cpp
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
//  This is implementation file for the client manager class.
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
RTTIDecls(TOrbClientConnMgr,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDOrb_ClientConnMgr
{
    // -----------------------------------------------------------------------
    //  The maximum number of worker threads we can have.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4MaxThreads  = 128;


    // -----------------------------------------------------------------------
    //  The maximum number of clients connections we'll accept.
    //
    //  Since there is some lag between the time that we see that a client
    //  has dropped and we get it out of the list, we make this a little
    //  higher than the actual live connections we want to support, to handle
    //  a pathological scenario where a number of clients drop at once, and
    //  then reconnect before we get them cleaned up.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4MaxClients  = 256;
}



// ---------------------------------------------------------------------------
//   CLASS: TOrbClientConnMgr
//  PREFIX: occm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbClientConnMgr: Constructor and Destructor
// ---------------------------------------------------------------------------
TOrbClientConnMgr::TOrbClientConnMgr(const  tCIDLib::TIPPortNum ippnListen
                                    , const tCIDLib::TCard4     c4MaxClients) :
    m_bLimitAccess(kCIDLib::False)
    , m_c4LoopCnt(1)
    , m_c4MaxClients(CIDOrb_ClientConnMgr::c4MaxClients)
    , m_c8NextConnId(0)
    , m_colClientList
      (
        tCIDLib::EAdoptOpts::Adopt
        , CIDOrb_ClientConnMgr::c4MaxClients
        , tCIDLib::EMTStates::Unsafe
      )
    , m_colThreads
      (
        tCIDLib::EAdoptOpts::Adopt
        , CIDOrb_ClientConnMgr::c4MaxThreads
        , tCIDLib::EMTStates::Unsafe
      )
    , m_colWorkQ(tCIDLib::EMTStates::Safe)
    , m_ipaOnlyAcceptFrom()
    , m_ippnListenOn(ippnListen)
    , m_psocklServer(nullptr)
    , m_thrListener
      (
        L"CIDOrbServerListener"
        , TMemberFunc<TOrbClientConnMgr>(this, &TOrbClientConnMgr::eListenerThread)
      )
{
    //
    //  If the max clients is zero, then just leave it at the default. If
    //  they set it and it is more than the max we allow, clip it back to
    //  that.
    //
    if (c4MaxClients)
    {
        m_c4MaxClients = c4MaxClients;
        if (m_c4MaxClients > CIDOrb_ClientConnMgr::c4MaxClients)
            m_c4MaxClients = CIDOrb_ClientConnMgr::c4MaxClients;
    }

    //
    //  We'll initially spin up 4 threads, but we can go up to four
    //  per possible client connection. That might still not be able
    //  to avoid backlog in a worse case scenario, but its unlikely
    //  that anyone will have that large and active a system.
    //
    //  For debugging purposes we can force a single threaded ORB setup.
    //
//    #define UNITHREADTESTMODE
    #if CID_DEBUG_ON && defined(UNITHREADTESTMODE)
    const tCIDLib::TCard4 c4InitSpinup = 1;
    #else
    const tCIDLib::TCard4 c4InitSpinup = 4;
    #endif


    //
    //  Go ahead and pre-register our stats cache items, so that below we
    //  can access them efficiently. The max clients is just a value type
    //  that never changes so we default register it by just setting a
    //  value with that key.
    //
    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_ActiveCmds
        , tCIDLib::EStatItemTypes::Counter
        , m_sciActiveCmds
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_ClientHWMark
        , tCIDLib::EStatItemTypes::Counter
        , m_sciClientHWMark
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_CurClients
        , tCIDLib::EStatItemTypes::Counter
        , m_sciCurClients
    );
    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_DroppedRetPacks
        , tCIDLib::EStatItemTypes::Counter
        , m_sciDroppedRetPacks
    );

    TStatsCache::SetValue
    (
        kCIDOrb::pszStat_Srv_MaxClients
        , m_sciMaxClients
        , m_c4MaxClients
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_QueuedCmds
        , tCIDLib::EStatItemTypes::Counter
        , m_sciQueuedCmds
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_WorkerThreads
        , tCIDLib::EStatItemTypes::Counter
        , m_sciWorkerThreads
    );
    TStatsCache::SetValue(m_sciWorkerThreads, c4InitSpinup);

    //
    //  Spin up our initial worker threads. We use a by reference vector to
    //  them. Its not made thread safe. We already have to have track a mutex
    //  for other synchronization, so we use that for this as well.
    //
    tCIDLib::TCard4 c4Index;
    TString strThrName;
    for (c4Index = 0; c4Index < c4InitSpinup; c4Index++)
    {
        strThrName = L"CIDOrbSrvWorkThread_";
        strThrName.AppendFormatted(c4Index + 1);
        m_colThreads.Add
        (
            new TThread
            (
                strThrName
                , TMemberFunc<TOrbClientConnMgr>(this, &TOrbClientConnMgr::eWorkerThread)
            )
        );
    }
    for (c4Index = 0; c4Index < c4InitSpinup; c4Index++)
        m_colThreads[c4Index]->Start();

    //
    //  And spin up the listener thread. Once this is up, we can start getting
    //  client connections.
    //
    m_thrListener.Start();
}


TOrbClientConnMgr::~TOrbClientConnMgr()
{
    // The ORB framework will have called our Terminate() method already
}


// ---------------------------------------------------------------------------
//  TOrbClientConnMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Clear the flag that limits us to the m_ipaOnlyAcceptFrom address
tCIDLib::TVoid TOrbClientConnMgr::ClearOnlyAcceptFrom()
{
    m_bLimitAccess = kCIDLib::False;
}


//
//  This is called by client connection objects, to have us queue up any
//  work that has shown up on its incoming socket (which it has already
//  checked and knows data is available for.
//
//  The caller passes us an input stream and the buffer it is set up over,
//  so we can use them as temporaries, which is a big win. We pass them
//  on to the packet reader, who uses them to do the work.
//
//  We don't have to worry about the calling connection object getting
//  dropped while we are in here. Only it closes itself down and since it
//  is calling us now, it cannot close itself down while it is in here.
//
tCIDOrb::EReadRes
TOrbClientConnMgr::eQueueUpWork(TThread&                thrSpooler
                                , TServerStreamSocket&  sockThis
                                , TOrbClientConnImpl&   occiReplyTo
                                , TMemBuf&              mbufTmp
                                , TBinMBufInStream&     strmTmp)
{
    tCIDLib::TCard4 c4SeqId;
    TWorkQItemPtr wqipNew;
    tCIDOrb::EReadRes eRes = facCIDOrb().eReadPacket
    (
        thrSpooler, sockThis, c4SeqId, 0, mbufTmp, strmTmp, wqipNew
    );

    //
    //  If the return indicates we got a packet, and he gen'd up a work
    //  queue item, then queue it up.
    //
    if (eRes == tCIDOrb::EReadRes::Packet)
    {
        // We should have gotten a work item!
        if (!wqipNew)
        {
            // Should log something here in debug mode probably
            #if CID_DEBUG_ON
                // <TBD>
            #endif

            // Just return a lost connection status and punt
            return tCIDOrb::EReadRes::Lost;
        }

        try
        {
            // Set up the connection info on the work item
            const tCIDLib::TCard8 c8RepId = occiReplyTo.c8ConnId();
            wqipNew->SetConnInfo(c8RepId, occiReplyTo.ipepClient());

            //
            //  If in debug mode, then make sure the calling connection is in
            //  our list. If not this has to be really bad since we shouldn't
            //  be able to be callable by a connection not in the list since
            //  we only remove them when they themselves shut themselves down
            //  (or when we are closing everything down at the end.)
            //
            #if CID_DEBUG_ON
            {
                TMtxLocker mtxlSync(&m_mtxSync);
                if (!poccFindConn(c8RepId))
                {
                    facCIDOrb().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcServ_ConmImplNotFound
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , occiReplyTo.ipepClient()
                    );
                }
            }
            #endif

            //
            //  Ok, we can put the pointer into our work queue. The queue is separately
            //  thread safe, and doesn't use the main mutex. But, because of that, we
            //  can't turn around and get the count and use that to set the stats cache
            //  value directly. We have to trust that we are inc/dec'ing it correctly.
            //
            TStatsCache::c8IncCounter(m_sciQueuedCmds);
            m_colWorkQ.objAdd(wqipNew);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }
    return eRes;
}


// Just returns the port we are listening on
tCIDLib::TIPPortNum TOrbClientConnMgr::ippnListen() const
{
    return m_ippnListenOn;
}


//
//  The client connection objects call this if they drop any packets, i.e.
//  if the client goes away before all replies are returned to him. We
//  just update our stats cache counter for dropped return packets.
//
tCIDLib::TVoid TOrbClientConnMgr::IncDroppedPackets()
{
    TStatsCache::c8IncCounter(m_sciDroppedRetPacks);
}


//
//  Sets an address that the listener thread will check. If the connecting
//  client isn't from this address, it won't accept the connection.
//
tCIDLib::TVoid TOrbClientConnMgr::OnlyAcceptFrom(const TIPAddress& ipaSource)
{
    TMtxLocker mtxlSync(&m_mtxSync);
    m_ipaOnlyAcceptFrom = ipaSource;
    m_bLimitAccess = kCIDLib::True;
}


//
//  This is called by the facility class to close us down when the client
//  code shuts down the server side ORB services.
//
tCIDLib::TVoid TOrbClientConnMgr::Terminate()
{
    // Stop listening for new connections
    try
    {
        m_thrListener.ReqShutdownSync(15000);
        m_thrListener.eWaitForDeath(5000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // If its not just a timeout, then log it
        if ((errToCatch.eClass() != tCIDLib::EErrClasses::Timeout)
        &&  facCIDOrb().bShouldLog(errToCatch))
        {
            TModule::LogEventObj(errToCatch);
        }

        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ListenShutdownErr
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Term
            );
        }
    }

    catch(...)
    {
        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ListenShutdownErr
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Term
            );
        }
    }

    // Try to clean up the listener socket
    try
    {
        delete m_psocklServer;
        m_psocklServer = nullptr;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        m_psocklServer = nullptr;
        if (facCIDOrb().bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ListenDelete
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Term
            );
        }
    }

    catch(...)
    {
        m_psocklServer = nullptr;

        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ListenDelete
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Term
            );
        }
    }

    //
    //  Now, we can start asking the worker threads to shutdown. This one
    //  can be problematic, because if the called code is ill behaved and
    //  doesn't check for shutdown requests, it won't be killable. If that's
    //  the case, we can't do much.
    //
    //  Generally though, this will allow the worker threads to finish their
    //  work and get the results sent back out.
    //
    //  To be efficient we first do a no sync shutdown request on all of them
    //  so they start shutting down in parallel, then go back and wait for them
    //  all to die.
    //
    const tCIDLib::TCard4 c4Count = m_colThreads.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colThreads[c4Index]->ReqShutdownNoSync();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TThread* pthrCur = m_colThreads[c4Index];
        try
        {
            pthrCur->eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            // <TBD> We need to able either block it, or kill it

            // If its not just a timeout, then log it
            if ((errToCatch.eClass() != tCIDLib::EErrClasses::Timeout)
            &&  facCIDOrb().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_WorkerShutdownErr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Shutdown
                    , pthrCur->strName()
                );
            }
        }

        catch(...)
        {
            // <TBD> We need to able either block it, or kill it
            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_WorkerShutdownErr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Shutdown
                    , pthrCur->strName()
                );
            }
        }
    }

    // And flush the threads out of the list now and clear the stats cache
    m_colThreads.RemoveAll();
    TStatsCache::SetValue(m_sciWorkerThreads, 0);

    //
    //  Now we can clean out our client connection list. First we ask
    //  them all to shut down, then we can trash the list. There shouldn't
    //  be any more threads trying to queue up replies on it since we
    //  stopped them above.
    //
    TIPEndPoint ipepCur;
    const tCIDLib::TCard4 c4ClientCnt = m_colClientList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ClientCnt; c4Index++)
    {
        try
        {
            TOrbClientConnImpl* poccCur = m_colClientList[c4Index];
            ipepCur = poccCur->ipepClient();
            poccCur->Shutdown();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);

            if (facCIDOrb().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);

            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbErrs::errcServ_ShutdownClient
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Term
                    , ipepCur
                );
            }
        }

        catch(...)
        {
            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbErrs::errcServ_ShutdownClient
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Term
                    , ipepCur
                );
            }
        }
    }
    m_colClientList.RemoveAll();
    TStatsCache::SetValue(m_sciCurClients, 0);

    //
    //  And flush the work queue of any last items that might have gotten
    //  in but not yet picked up. We have to give them back to the pool.
    //
    while (!m_colWorkQ.bIsEmpty())
    {
        try
        {
            TWorkQItemPtr wqipCur;
            m_colWorkQ.bGetNext(wqipCur, kCIDLib::False);
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrb().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
            // Probably should log something in debug mode?
        }
    }
    TStatsCache::SetValue(m_sciQueuedCmds, 0);
}


// ---------------------------------------------------------------------------
//  TOrbClientConnMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  To keep the main listener thread method from getting too large, we break out
//  this, which it calls when it's trying to create the listener thread.
//
//  The main thread tells us if this is the first time this is called. If so, we
//  will log stuff that we won't subsequently if we have to keep trying.
//
tCIDLib::TBoolean TOrbClientConnMgr::bMakeListener(tCIDLib::TBoolean& bFirstTime)
{
    // Remember the first time flag and then clear it
    tCIDLib::TBoolean bLogMsgs = bFirstTime;
    bFirstTime = kCIDLib::False;

    try
    {
        //
        //  If we haven't created it yet, do that. If we have, clean it up
        //  to try to initilize it again.
        //
        if (!m_psocklServer)
            m_psocklServer = new TSocketListener(m_ippnListenOn);
        else
            m_psocklServer->Cleanup();

        // And try to initilize it.
        m_psocklServer->Initialize(tCIDSock::ESockProtos::TCP);

        // In case the port was zero, get the actual port used out
        m_ippnListenOn = m_psocklServer->ippnListenOn();
    }

    catch(TError& errToCatch)
    {
        if (bLogMsgs)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcServ_CantCreateListener
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::Init
                , TCardinal(m_ippnListenOn)
            );
        }
        return kCIDLib::False;
    }

    catch(...)
    {
        if (bLogMsgs)
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcServ_CantCreateListener
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::Init
                , TCardinal(m_ippnListenOn)
            );
        }
        return kCIDLib::False;
    }

    return kCIDLib::True;
}



//
//  Just to keep the listener thread method from getting out of hand, we
//  break out this method which is called to check for new connections and
//  do other checks that we need to do periodically.
//
//
tCIDLib::TVoid TOrbClientConnMgr::DoChecks()
{
    TIPEndPoint             ipepClient;
    TServerStreamSocket*    psockClient = nullptr;

    // Bump our loop count up andmake sure it's nto ever zero
    m_c4LoopCnt++;
    if (!m_c4LoopCnt)
        m_c4LoopCnt++;

    //
    //  Check the client list every time through and prune out any dead
    //  connections. Doing this up front will help below to insure that
    //  we have minimized the client list before potentially accepting
    //  another one, so that we are less likely to reject a client once we
    //  get close to saturation.
    //
    {
        TMtxLocker mtxlSync(&m_mtxSync);

        tCIDLib::TBoolean bChanges = kCIDLib::False;
        tCIDLib::TCard4 c4Index = 0;
        tCIDLib::TCard4 c4Count = m_colClientList.c4ElemCount();
        while (c4Index < c4Count)
        {
            TOrbClientConnImpl* poccCur = m_colClientList[c4Index];
            if (poccCur->bOffline())
            {
                m_colClientList.RemoveAt(c4Index);
                c4Count--;
                bChanges = kCIDLib::True;
            }
             else
            {
                c4Index++;
            }
        }

        // If any changes, update the current clients in the stats cache
        if (bChanges)
            TStatsCache::SetValue(m_sciCurClients, c4Count);
    }

    //
    //  Wait a bit for a connection from a client. This provides our
    //  throttling.
    //
    psockClient = m_psocklServer->psockListenFor
    (
        kCIDLib::enctOneMilliSec * 250, ipepClient
    );

    // We have a new client, so let's see if we can accept him
    if (psockClient)
    {
        // Put a janitor on it until we get it stored away successfully
        TJanitor<TServerStreamSocket> janSock(psockClient);

        // See if we are maxed out on clients
        tCIDLib::TCard4 c4Count = m_colClientList.c4ElemCount();
        if (c4Count >= m_c4MaxClients)
        {
            //
            //  We've maxed out, so send it a rejection and close it. It's
            //  still in the janitor so that will clean it up when we exit
            //  the parent scope.
            //
            psockClient->Send
            (
                &kCIDOrb::c4RejectTooMany, sizeof(kCIDOrb::c4RejectTooMany)
            );
            psockClient->c4Shutdown();

            //
            //  We've done the clean shutdown, and this is server side, so don't
            //  don't linger and let up a socket. This prevents DOS attack,
            //  purposeful or accidental, by large numbers of rejected connections.
            //
            psockClient->bLinger(kCIDLib::False, 0);

            // Log this if in verbose mode
            if (facCIDOrb().bLogWarnings())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbErrs::errcServ_RejectedClient
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::RejectedConn
                    , TString(L"Server has too many connections")
                );
            }

            // And we are done
            return;
        }

        //
        //  If the limit access flag is on, then see if he is from the
        //  source that we can accept.
        //
        if (m_bLimitAccess)
        {
            //
            //  Note that we are only comparing the address part of the
            //  socket's end point, to the access from address. The port
            //  doesn't matter here.
            //
            if (psockClient->ipepRemoteEndPoint() != m_ipaOnlyAcceptFrom)
            {
                // He's not, so send a rejection and close it
                psockClient->Send
                (
                    &kCIDOrb::c4NotBlessed, sizeof(kCIDOrb::c4NotBlessed)
                );
                psockClient->c4Shutdown();

                //
                //  We've done the clean shutdown, and this is server side, so don't
                //  don't linger and let up a socket. This prevents DOS attack,
                //  purposeful or accidental, by large numbers of rejected connections.
                //
                psockClient->bLinger(kCIDLib::False, 0);

                // Log this if in verbose mode
                if (facCIDOrb().bLogWarnings())
                {
                    facCIDOrb().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcServ_RejectedClient
                        , tCIDLib::ESeverities::Warn
                        , tCIDLib::EErrClasses::AppStatus
                        , TString(L"Not from blessed end point")
                    );
                }

                // And we are done
                return;
            }
        }

        //
        //  Looks like we are going to accept it. So set up the socket.
        //  Turn off the Nagle stuff.
        //
        psockClient->bNagleOn(kCIDLib::False);

        // Send an 'accepted' message back to the client.
        psockClient->Send(&kCIDOrb::c4Accepted, sizeof(kCIDOrb::c4Accepted));

        //
        //  And orphan the socket out into the client list. We lock the
        //  list and give the socket to it, then get the current count (it
        //  could have been changed by the read thread since we checked
        //  above.) We give this guy the next available connection id.
        //
        {
            TMtxLocker mtxlSync(&m_mtxSync);

            // Get the next connection id. Don't let it be zero
            m_c8NextConnId++;
            if (!m_c8NextConnId)
                m_c8NextConnId++;

            m_colClientList.Add
            (
                new TOrbClientConnImpl
                (
                    janSock.pobjOrphan(), ipepClient, this, m_c8NextConnId
                )
            );
            c4Count = m_colClientList.c4ElemCount();

            // Update the stats cache count of current clients
            TStatsCache::SetValue(m_sciCurClients, c4Count);
        }

        // If a new high water mark, then save it and log
        if (TStatsCache::bSetIfHigher(m_sciClientHWMark, c4Count))
        {
            // Always log since this is critical for post-mortem
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ClientHWMInc
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , TCardinal(c4Count)
            );
        }

        //
        //  Log the new connection if in verbose mode. Try to do a reverse
        //  DNS if we can, so that we can log the actual host name.
        //
        if (facCIDOrb().bLogInfo())
        {
            TString strAddr;
            ipepClient.FormatPlain(strAddr);
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ClientConnFrom
                , tCIDLib::ESeverities::Info
                , tCIDLib::EErrClasses::AppStatus
                , strAddr
            );
        }
    }

    //
    //  Check to see if the work queue is getting too big. If so, and we can
    //  spin up more threads, then do so. We do this one every 8 times
    //  through when we didn't get any new client.
    //
    if (!(m_c4LoopCnt % 8))
    {
        TMtxLocker mtxlSync(&m_mtxSync);

        const tCIDLib::TCard4 c4Cmds = m_colWorkQ.c4ElemCount();
        const tCIDLib::TCard4 c4Threads = m_colThreads.c4ElemCount();

        if (c4Cmds >= (c4Threads << 1))
        {
            if (c4Threads < CIDOrb_ClientConnMgr::c4MaxThreads)
            {
                TString strThrName = L"CIDOrbSrvWorkThread_";
                strThrName.AppendFormatted(c4Threads + 1);
                TThread* pthrNew = new TThread
                (
                    strThrName
                    , TMemberFunc<TOrbClientConnMgr>(this, &TOrbClientConnMgr::eWorkerThread)
                );
                TJanitor<TThread> janThread(pthrNew);
                pthrNew->Start();

                // Set the worker threads stat and add the thread to the list
                TStatsCache::SetValue(m_sciWorkerThreads, c4Threads + 1);
                m_colThreads.Add(janThread.pobjOrphan());

                // Log a message about this
                if (facCIDOrb().bLogInfo())
                {
                    facCIDOrb().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbMsgs::midStatus_NewWorkerThread
                        , tCIDLib::ESeverities::Info
                        , tCIDLib::EErrClasses::AppStatus
                        , TCardinal(c4Threads + 1)
                    );
                }
            }
        }
    }
}


//
//  This is the listener thread. It waits for connections from the client ORB
//  support of a client host. When it gets one, it adds an item to the client
//  list collection. It also handles pruning dead connections from the client
//  list. In the case where we cannot open our listener socket, it just tries
//  to do so periodically. Once the socket is open, then it starts listenting
//  for connections.
//
tCIDLib::EExitCodes
TOrbClientConnMgr::eListenerThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    if (facCIDOrb().bLogInfo())
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbMsgs::midStatus_StartListener
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
            , TCardinal(m_ippnListenOn)
            , TCardinal(m_c4MaxClients)
        );
    }

    //
    //  We have basically two states. In one, we are trying to get our
    //  listener thread open. In the other, we are processing incoming
    //  connections. We start off in the first state, but could get back
    //  to it if there's a network error and we decided to cycle the
    //  listener thread.
    //
    enum class EStates
    {
        MakeSocket
        , Normal
    };
    EStates eCurState = EStates::MakeSocket;

    // And now loop till told to stop
    tCIDLib::TBoolean bFirstFail = kCIDLib::True;
    while (kCIDLib::True)
    {
        try
        {
            if (thrThis.bCheckShutdownRequest())
                break;

            if (eCurState == EStates::MakeSocket)
            {
                // Try to create our lisener socket
                if (bMakeListener(bFirstFail))
                {
                    // It worked, so set our state to normal
                    eCurState = EStates::Normal;

                    if (facCIDOrb().bLogInfo())
                    {
                        facCIDOrb().LogMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , kOrbMsgs::midStatus_CreatedListener
                            , tCIDLib::ESeverities::Info
                            , tCIDLib::EErrClasses::AppStatus
                            , TCardinal(m_ippnListenOn)
                        );
                    }
                }
                 else
                {
                    //
                    //  We failed to create our listener socket. So sleep
                    //  for 5 seconds, breaking out if asked to end.
                    //
                    if (!thrThis.bSleep(5000))
                        break;
                }
            }

            else
            {
                //
                //  Do the usual checks that we do on a periodic basis once
                //  our listener thread is created.
                //
                DoChecks();
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrb().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_ListenTError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Unknown
                );
            }
        }

        catch(...)
        {
            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_ListenError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Unknown
                );
            }
        }
    }

    if (facCIDOrb().bLogInfo())
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbMsgs::midStatus_StopListener
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }
    return tCIDLib::EExitCodes::Normal;
}



//
//  A number of instances of this thread are kicked off, and more are started
//  if the load grows. These pull items of the work queue, find the target
//  server side object, and dispatch the command to the target. Any reply
//  data is returned to the connection object that the command came from,
//  it is still is alive. If not, we just delete the work item ourself.
//
tCIDLib::EExitCodes
TOrbClientConnMgr::eWorkerThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Now let the calling thread go since we have the passed data
    thrThis.Sync();

    //
    //  Ok, we just loop until told to shut down.
    //
    tCIDLib::TBoolean       bDone = kCIDLib::False;
    TIPEndPoint             ipepClient;
    TString                 strMethodName;

    while (!bDone)
    {
        try
        {
            if (thrThis.bCheckShutdownRequest())
            {
                bDone = kCIDLib::True;
                break;
            }

            //
            //  Wait for a bit for some work to show up in the queue. This
            //  one is thread safe and doesn't use the main mutex, since it
            //  wouldn't work otherwise.
            //
            //  Kind of annoying that we have to create a work queue item
            //  pointer every time around even if never used. But it has to be
            //  in scope to do what it is supposed to do. They are very light
            //  weight and don't allocate anything unless we get something.
            //
            TWorkQItemPtr wqipCur;
            if (!m_colWorkQ.bGetNext(wqipCur, 1500, kCIDLib::False))
                continue;

            // Update our stats cache of queued cmds
            TStatsCache::c8DecCounter(m_sciQueuedCmds);

            try
            {
                // Store the ip end point for the exception handlers below
                ipepClient = wqipCur->ipepClient();

                // Bump the active commands count
                TStatsCache::c8IncCounter(m_sciActiveCmds);

                //
                //  Get the method name out, and then dispatch it. The stream
                //  will be on the appropriate spot for the handler to start
                //  streaming in parms, now that we've read the name out.
                //
                wqipCur->ocmdThis().strmIn() >> strMethodName;
                facCIDOrb().DispatchCmd(strMethodName, wqipCur->ocmdThis());
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);

                // And decrement the active cmds count again then rethrow
                TStatsCache::c8DecCounter(m_sciActiveCmds);
                throw;
            }

            //
            //  Ok, it worked. So we want to queue up the reply on the client
            //  connection object that is stored in the work queue. At this point
            //  we are no longer responsible for the work item.
            //
            //  The connection might have gone away, in which case the counted
            //  pointer will release the work item back to the pool.
            //
            //  We need to decrement the active commands count as well.
            //
            TStatsCache::c8DecCounter(m_sciActiveCmds);
            TOrbClientConnImpl* poccReply = nullptr;
            {
                TMtxLocker mtxlSync(&m_mtxSync);

                // Now let's see if we can find it
                const tCIDLib::TCard8 c8RepId = wqipCur->c8ConnId();
                TOrbClientConnImpl* poccReply = poccFindConn(c8RepId);
                if (poccReply)
                {
                    //
                    //  Note that this doesn't actually send the reply,
                    //  else we couldn't keep the mutex locked while we
                    //  are in here. It just queues up the item to be
                    //  sent by the connection object's spooler thread.
                    //
                    poccReply->SendReply(wqipCur);
                }
                 else
                {
                    TStatsCache::c8IncCounter(m_sciDroppedRetPacks);
                }
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrb().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_WorkerTError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppError
                    , ipepClient
                );
            }
        }

        catch(...)
        {
            if (facCIDOrb().bLogFailures())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_WorkerError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppError
                    , ipepClient
                );
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  A convenience method to find a connection object by its connection id
//  We assume that the caller locked first.
//
TOrbClientConnImpl*
TOrbClientConnMgr::poccFindConn(const tCIDLib::TCard8 c8ConnId)
{
    const tCIDLib::TCard4 c4ClientCnt = m_colClientList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ClientCnt; c4Index++)
    {
        TOrbClientConnImpl* poccCur = m_colClientList[c4Index];
        if (poccCur->c8ConnId() == c8ConnId)
            return poccCur;
    }
    return nullptr;
}

