//
// FILE NAME: CIDOrb_ClientBaseHelpers.cpp
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
#include    "CIDOrb_ClientBaseHelpers_.hpp"


// Turns on some debugging popups below
#if CID_DEBUG_ON
#define CIDORB_POPUPS 1
#endif



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbQNode,TDLstNode)


// ---------------------------------------------------------------------------
//   CLASS: TOrbQNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbQNode: Constructors and Destructor
// ---------------------------------------------------------------------------
TOrbQNode::TOrbQNode(TCmdQItem* const pcqiData) :

    m_pcqiData(pcqiData)
{
}

TOrbQNode::~TOrbQNode()
{
}


// ---------------------------------------------------------------------------
//  TOrbQNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TCmdQItem* TOrbQNode::pobjData() const
{
    return m_pcqiData;
}

TCmdQItem* TOrbQNode::pcqiData()
{
    return m_pcqiData;
}



// ---------------------------------------------------------------------------
//   CLASS: TSrvTarget
//  PREFIX: srvt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSrvTarget: Constructors and Destructor
// ---------------------------------------------------------------------------
TSrvTarget::TSrvTarget(const TIPEndPoint& ipepSrv) :

    m_bReconnMode(kCIDLib::True)
    , m_c4RefCount(0)
    , m_c4SequenceId(1)
    , m_colRepList(tCIDLib::EAdoptOpts::NoAdopt, 64)
    , m_enctCacheStamp(0)
    , m_errUnknown
      (
        facCIDOrb().strName()
        , CID_FILE
        , CID_LINE
        , facCIDOrb().strMsg(kOrbErrs::errcServ_UnknownExcept)
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Unknown
      )
    , m_evQEvent(tCIDLib::EEventStates::Reset)
    , m_evREvent(tCIDLib::EEventStates::Reset)
    , m_ipepServer(ipepSrv)
    , m_psockSrv(nullptr)
    , m_thrSpool
      (
        facCIDOrb().strNextSpoolThreadName(kCIDLib::False)
        , TMemberFunc<TSrvTarget>(this, &TSrvTarget::eSpoolThread)
      )
{
}


TSrvTarget::~TSrvTarget()
{
    Cleanup();
}


// ---------------------------------------------------------------------------
//  TSrvTarget: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Checks to see if we are still connected. The spooler thread is always
//  watching the socket, and if the connection is lost, it will remove the
//  socket. So we just check to see if its zero or not.
//
tCIDLib::TBoolean TSrvTarget::bIsConnected()
{
    return !m_bReconnMode;
}


//
//  This is called to spin us up. It may fail if the socket cannot be created.
//  If it works, we spin up the spooler thread.
//
tCIDLib::TBoolean TSrvTarget::bInitialize()
{
    // Spin up the threads
    try
    {
        // Try to create the socket. If this fails, we bail out immediately
        if (!bMakeSocket())
            return kCIDLib::False;

        m_bReconnMode = kCIDLib::False;
        m_thrSpool.Start();
    }

    catch(...)
    {
        //
        //  Don't try to log anything here. It's normal for this to fail since
        //  servers come and go. And an ORB based logging client is commonly
        //  used and this would just cause a lot of problems.
        //
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Queues up the passed command to be processed on this server connection
tCIDLib::TCard4 TSrvTarget::c4QueueCmd(TCmdQItem* const pcqiToQ)
{
    // Lock the sync mutex
    TLocker lockrSrv(&m_mtxSync);

    // Get the next sequence number available for this target
    const tCIDLib::TCard4 c4Ret = m_c4SequenceId++;

    // Lock the command and update it
    {
        TLocker lockrCmd(pcqiToQ->pmtxLock());
        pcqiToQ->eStage(tCIDOrb::ECmdStages::CmdQ);
        pcqiToQ->c4SequenceId(c4Ret);

        // Reset our performance debugging timer so we can time this next stage
        pcqiToQ->SetStartTime();
    }

    //
    //  Add it to our queue and trigger the queue event to let the spooler
    //  thread know we have added new items.
    //
    m_colCmdQ.AppendNode(new TOrbQNode(pcqiToQ));
    m_evQEvent.Trigger();

    //
    //  Return the sequence number this guy got. We'll need this later to
    //  retract it possibly.
    //
    return c4Ret;
}


//
//  Stops our spooler thread and cleans up our socket if it is current
//  allocated.
//
tCIDLib::TVoid TSrvTarget::Cleanup()
{
    if (m_thrSpool.bIsRunning())
    {
        try
        {
            m_thrSpool.ReqShutdownSync(20000);
            m_thrSpool.eWaitForDeath(10000);
        }

        catch(const TError&)
        {
            // <TBD> Can't really log this
        }
    }

    // And clean up the socket
    if (m_psockSrv)
    {
        try
        {
            m_psockSrv->c4Shutdown();
        }

        catch(...)
        {
            // <TBD> Can't really log this
        }

        // And delete the object, which cleans up the actual socket handle
        try
        {
            delete m_psockSrv;
        }

        catch(...)
        {
            // <TBD> Can't really log this
        }

        m_psockSrv = nullptr;

        //
        //  This will cause any straggling clients that might come in to error
        //  out because we now show disconnected.
        //
        m_bReconnMode = kCIDLib::True;
    }

    // Flush the remaining queued objects
    try
    {
        ErrorOutWaiting();
    }

    catch(...)
    {
        // <TBD> Can't really log this
    }
}


// ---------------------------------------------------------------------------
//  TSrvTarget: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSrvTarget::bMakeSocket()
{
    // We shouldn't have a socket already
    if (m_psockSrv)
    {
        if (facCIDOrb().bLogWarnings())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , L"The socket already exists"
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Already
            );
        }
        return kCIDLib::False;
    }

    // Create a new socket and bind to the other side
    TClientStreamSocket* psockNew = nullptr;
    try
    {
        psockNew = new TClientStreamSocket
        (
            tCIDSock::ESockProtos::TCP, m_ipepServer.eAddrType()
        );

        psockNew->Connect
        (
            m_ipepServer, kCIDLib::enctThreeSeconds + facCIDOrb().enctTimeoutAdjust()
        );
    }

    catch(TError& errToCatch)
    {
        if (facCIDOrb().bLogWarnings())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // Just eat it and fall through with null socket pointer
        if (psockNew)
        {
            try
            {
                delete psockNew;
            }

            catch(TError& errInner)
            {
                if (facCIDOrb().bShouldLog(errInner))
                {
                    errInner.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errInner);
                }

                if (facCIDOrb().bLogFailures())
                {
                    facCIDOrb().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcClient_SockCleanupErr
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
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
                        , kOrbErrs::errcClient_SockCleanupErr
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }
            }
            psockNew = nullptr;
        }
    }

    catch(...)
    {
        // Just eat it and fall through with null socket pointer
        if (psockNew)
        {
            try
            {
                delete psockNew;
            }

            catch(TError& errToCatch)
            {
                if (facCIDOrb().bShouldLog(errToCatch))
                {
                    TModule::LogEventObj(errToCatch);
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                }

                if (facCIDOrb().bLogFailures())
                {
                    facCIDOrb().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcClient_SockCleanupErr
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::AppStatus
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
                        , kOrbErrs::errcClient_SockCleanupErr
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::AppStatus
                    );
                }
            }
            psockNew = nullptr;
        }
    }

    // If we didn't get it, then return a failure
    if (!psockNew)
        return kCIDLib::False;

    TJanitor<TClientStreamSocket> janSocket(psockNew);

    // Turn off the Nagle stuff
    psockNew->bNagleOn(kCIDLib::False);

    // And wait for an acceptance code from the server
    tCIDLib::TCard4 c4Status = 0;
    const tCIDLib::TCard4 c4ToRead = sizeof(c4Status);
    const tCIDLib::TEncodedTime enctWait(kCIDLib::enctFourSeconds + facCIDOrb().enctTimeoutAdjust());
    if (psockNew->c4ReceiveRawTO(&c4Status, enctWait, c4ToRead) != c4ToRead)
    {
        if (facCIDOrb().bLogWarnings())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcComm_NoAcceptReply
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Protocol
                , m_ipepServer
            );
        }
        return kCIDLib::False;
    }

    if (c4Status != kCIDOrb::c4Accepted)
    {
        // It wasn't accepted by the server, so translate to an error
        tCIDLib::EErrClasses eClass = tCIDLib::EErrClasses::Unknown;
        tCIDLib::TMsgId midReason = kOrbMsgs::midRej_Unknown;
        switch(c4Status)
        {
            case kCIDOrb::c4RejectTooMany :
                eClass = tCIDLib::EErrClasses::OutResource;
                midReason = kOrbMsgs::midRej_TooManyClients;
                break;

            default :
                break;
        }

        if (facCIDOrb().bLogWarnings())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , midReason
                , tCIDLib::ESeverities::Warn
                , eClass
            );
        }
        return kCIDLib::False;
    }

    // Associate the socket with our read event
    psockNew->AssociateReadEvent(m_evREvent);

    //
    //  Orphan the socket out the janitor into our member. The write thread
    //  won't do anything until we set the reconnect state back to false upon
    //  return.
    //
    m_psockSrv = janSocket.pobjOrphan();
    return kCIDLib::True;
}


tCIDLib::TVoid
TSrvTarget::CancelCmdWithErr(TCmdQItem& cqiCan, const TError& errLostConn)
{
    // Lock the command while we update it
    TLocker lockrCmd(cqiCan.pmtxLock());

    //
    //  If it's already orphaned, then we can just free it and leave it
    //  at that. Else, we have to return the error.
    //
    if (cqiCan.eStage() == tCIDOrb::ECmdStages::Orphaned)
    {
        cqiCan.eStage(tCIDOrb::ECmdStages::Free);
    }
     else
    {
        cqiCan.eStage(tCIDOrb::ECmdStages::Ready);
        cqiCan.ocmdData().SetReplyMode();
        cqiCan.ocmdData().strmOut().Reset();
        cqiCan.ocmdData().strmOut() << errLostConn << kCIDLib::FlushIt;
        cqiCan.ocmdData().bRetStatus(kCIDLib::False);
        cqiCan.WakeUpWaiter();
    }
}


tCIDLib::EExitCodes
TSrvTarget::eSpoolThread(TThread& thrThis, tCIDLib::TVoid*)
{
    thrThis.Sync();

    // Set up read and write buffers
    THeapBuf                mbufRead(16 * 1024, kCIDOrb::c4MaxPayload, 0x100000);
    TBinMBufInStream        strmRead(&mbufRead);
    TBinMBufOutStream       strmWrite(16 * 1024, kCIDOrb::c4MaxPayload);

    //
    //  If a connection isn't used for a given time (MaxIdle), the server
    //  side ORB will assume it's dead and drop it. So each time we send
    //  any outgoing, we set this to that time plus half the MaxIdle
    //  time. If we get to that time, we'll send a ping to keep the
    //  connection alive.
    //
    //  This is done by way of a special header being sent. The server
    //  side ORB sees it and just eats it and doesn't respond, but this
    //  updates his time stamp for the last activity from us. So we go
    //  ahead and set up that header for use below. We set the hash
    //  to 0xFFFFFF and the sequence id to 12345678 as a signal that it's
    //  a ping header.
    //
    tCIDLib::TEncodedTime   enctNextPing = 0;;
    tCIDOrb::TPacketHdr     hdrPing;
    hdrPing.c4MagicVal      = 0xFEADBEAF;
    hdrPing.c4MagicVal2     = 0xBEAFDEAD;
    hdrPing.c4DataBytes     = 0;
    hdrPing.hshData         = 0xFFFFFFFF;
    hdrPing.c4SequenceId    = 0x12345678;

    // And now loop till asked to shut down
    while (kCIDLib::True)
    {
        try
        {
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  On each loop through, we are either in reconnect mode or not.
            //  If in reconnect mode, we've destroyed the previous socket and
            //  are trying to create a new one.
            //
            //  If we aren't in reconnect mode, then we think we have a good
            //  connection so we can try to process incoming or outgoing.
            //  If we lose it while processing, we return any queued events
            //  with errors, and go back to reconn mode.
            //
            if (m_bReconnMode)
            {
                TryReconnect();

                // Init the next ping time now, since we just reconnected.
                enctNextPing =  TTime::enctNow() + kCIDOrb::enctHalfMaxIdle;
            }
             else
            {
                // Do a multi-wait on our two events
                const tCIDLib::TCard4 c4Which = TEvent::c4WaitMultiple
                (
                    m_evREvent, m_evQEvent, 250
                );

                //
                //  According to which one was triggered, call the local
                //  helper that does that one. If both are available, that's
                //  ok since we'll just trigger immediately next time
                //  around.
                //
                if (c4Which == 0)
                {
                    //
                    //  Note that we reset here before doing anything else.
                    //  This event is associated with the socket and won't
                    //  retrigger (once we've reset it and done the reads
                    //  that we are about to do, which tells it we've reacted
                    //  to this triggering of the event.)
                    //
                    //  If we've lost the connection, this will throw.
                    //
                    m_evREvent.Reset();
                    PollReplies(thrThis, mbufRead, strmRead);
                }
                 else if (c4Which == 1)
                {
                    // Update the next ping time since we sent something
                    enctNextPing = TTime::enctNow() + kCIDOrb::enctHalfMaxIdle;

                    // Reset the event before we start sending
                    m_evQEvent.Reset();
                    SendQueued(thrThis, strmWrite);
                }
                 else
                {
                    //
                    //  If it's been more than half the MaxIdle time since
                    //  we last sent an outgoing, then ping the server on
                    //  behalf of the proxies that use us, so that the
                    //  server won't drop us.
                    //
                    if (TTime::enctNow() > enctNextPing)
                    {
                        //
                        //  Just send a ping header. There's no body to
                        //  it and we don't expect a reply. If we've not
                        //  sent anything in this long, it should not be
                        //  remotely busy so we don't bother with a throttled
                        //  send here. If we get a busy exception here, then
                        //  it is a sign of problems.
                        //
                        //  Reset the time stamp first just in case, so that
                        //  we can't get stuck constantly trying to send
                        //  pings.
                        //
                        enctNextPing =  TTime::enctNow()
                                        + kCIDOrb::enctHalfMaxIdle;
                        m_psockSrv->Send(&hdrPing, sizeof(tCIDOrb::TPacketHdr));
                    }
                }
            }
        }

        catch(...)
        {
            //
            //  If we have a socket, then we will just panic and shut it
            //  down, error our any clients, and start trying to reconnect
            //  again.
            //
            //  !!We can't do any logging here since the thing calling us
            //  could be an ORB based logger.
            //
            if (m_psockSrv)
            {
                try
                {
                    try
                    {
                        // If not shutdown yet, then do the polite shutdown
                        if (!m_psockSrv->bIsShutdown())
                            m_psockSrv->c4Shutdown();
                        m_psockSrv->Close();
                    }

                    catch(...)
                    {
                        //
                        //  Not much we can do, we'll go ahead and do the
                        //  non-polite shutdown by deleting it.
                        //
                    }

                    delete m_psockSrv;
                }

                catch(...)
                {
                    //
                    //  Can't do much here. Logging would be bad because our
                    //  process might be using an ORB based logger.
                    //
                }

                m_psockSrv = nullptr;
                m_bReconnMode = kCIDLib::True;

                try
                {
                    // Error out any waiting stuff
                    ErrorOutWaiting();
                }

                catch(...)
                {
                    //
                    //  Can't do much here. Logging would be bad because our
                    //  process might be using an ORB based logger.
                    //
                }
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid TSrvTarget::ErrorOutWaiting()
{
    //
    //  Lock while we do this beause clients call in to queue up events
    //  on the command queue, so we have to make sure they don't try that
    //  while we are doing this.
    //
    //  In theory, we don't have to lock for the first list, since only
    //  the spool thread, who is calling this, ever reads/writes that list.
    //  But it's not worth even having the paranoia, since this is only
    //  done when things go to hell anyway, so performance isn't a biggie
    //  here.
    //
    //  The reconnect flag should ahve already been set, so incoming
    //  client queing calls should be rejecting themselves when they see
    //  the flag anyway, and not even trying to queue.
    //
    TLocker lockrSync(&m_mtxSync);

    // If any replies in the list, then error them out
    tCIDLib::TCard4 c4Count = m_colRepList.c4ElemCount();
    if (c4Count)
    {
        TError errLostConn
        (
            facCIDOrb().strName()
            , CID_FILE
            , CID_LINE
            , facCIDOrb().strMsg(kOrbErrs::errcClient_ServerLost, m_ipepServer)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
        );

        while (c4Count)
        {
            TCmdQItem* pcqiCur = m_colRepList.pobjOrphanAt(0);
            CancelCmdWithErr(*pcqiCur, errLostConn);
            c4Count--;
        }
    }

    // If any in the command queue, then error them out
    TDLstNode* pnodeCur = m_colCmdQ.pnodeHead();
    if (pnodeCur)
    {
        TError errLostConn
        (
            facCIDOrb().strName()
            , CID_FILE
            , CID_LINE
            , facCIDOrb().strMsg(kOrbErrs::errcClient_ServerLost, m_ipepServer)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
        );

        while (pnodeCur)
        {
            // Get the data out and cancel it
            TCmdQItem* pcqiTmp = static_cast<TOrbQNode*>(pnodeCur)->pcqiData();
            CancelCmdWithErr(*pcqiTmp, errLostConn);

            // Move forward to the next node
            pnodeCur = pnodeCur->pnodeNext();
        }

        // Clean out the queue now
        m_colCmdQ.RemoveAll();
    }
}


//
//  Note that this does not have to lock, because it's only called by the
//  spooling thread, and the spooling thread is the only one who accesses
//  the reply list.
//
//  Note that we also remove any orphaned items from the list.
//
TCmdQItem* TSrvTarget::pcqiExtractReply(const tCIDLib::TCard4 c4SeqId)
{
    TCmdQItem* pcqiRet = nullptr;
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < m_colRepList.c4ElemCount())
    {
        TCmdQItem* pcqiCur = m_colRepList[c4Index];

        // If it's our guy, remember it and remove from list
        if (pcqiCur->c4SequenceId() == c4SeqId)
        {
            //
            //  Suppress no-discard warning by assigning return from the orphan
            //  call instead of using the current pointer we already have.
            //
            pcqiRet = m_colRepList.pobjOrphanAt(c4Index);
        }
         else if (pcqiCur->eStage() == tCIDOrb::ECmdStages::Orphaned)
        {
            //
            //  It's orphaned, so remove from the list and free it. Since it
            //  is orphaned, we don't have to sync here, because the client
            //  has already gone away and will not change the stage after we
            //  set it free.
            //
            m_colRepList.OrphanElemAt(c4Index);
            pcqiCur->eStage(tCIDOrb::ECmdStages::Free);
        }
         else
        {
            // Not ours and in use, so just move past it
            c4Index++;
        }
    }

    // Return the command if we got one
    return pcqiRet;
}


//
//  Note that we are called here when we know that data is waiting on the
//  socket, and we read all the packets that are available. We don't want
//  to block waiting on data, so we check for more data available at the
//  top of the loop with a short timeout.
//
tCIDLib::TVoid
TSrvTarget::PollReplies(TThread& thrCaller
                        , TMemBuf& mbufRead
                        , TBinMBufInStream& strmRead)
{
    if (!m_psockSrv)
    {
        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , L"The server target socket was not set"
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotReady
            );
        }
        return;
    }

    //
    //  Handle packets till we get all that are waiting. We pass zero for
    //  the timeout, so we'll only get any that are already waiting.
    //
    //  IMPORTANTLY, this will have reset the event that we use to wait
    //  for data. Until we do a read that would block, it won't retrigger.
    //  NOTE that we mean the FD_READ event, not the event that we block on.
    //  We reset the event object before even calling into here.
    //
    tCIDLib::TCard4     c4DataBytes = 0;
    tCIDLib::TCard4     c4SeqId = 0;
    while (kCIDLib::True)
    {
        strmRead.Reset();
        const tCIDOrb::EReadRes eRes = facCIDOrb().eReadPacket
        (
            thrCaller, *m_psockSrv, c4SeqId, 0, c4DataBytes, mbufRead
        );

        // If we got nothing, then not bad, just nothing available
        if (eRes == tCIDOrb::EReadRes::NoPacket)
            break;

        if (eRes != tCIDOrb::EReadRes::Packet)
        {
            // We just need to throw something with a lost conn error class
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_LostConn
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
                , m_ipepServer
            );
        }

        //
        //  We got a packet, so find it's sequence id in the queue. If it is
        //  found, this removes it from the collection at the same time.
        //
        //  Note that this guy doesn't lock and doesn't need to, since only
        //  we (the spooling thread) access the reply list anyway.
        //
        //  Once we get the queued item, we lock it and then we can update
        //  it as required without worrying about overall locks.
        //
        //  This guy will also take any already orphaned commands out of the
        //  reply queue since no one is waiting for them anymore. We orphan
        //  below as well, but if we never get a reply for an item, and the
        //  user gives up, then they will get stranded.
        //
        TCmdQItem* pcqiCur = pcqiExtractReply(c4SeqId);
        if (pcqiCur)
        {
            TLocker lockrCmd(pcqiCur->pmtxLock());

            //
            //  If it has not already been orphaned by the waiting thread,
            //  then process the reply. Else, just drop it on the floor and
            //  free the command item.
            //
            if (pcqiCur->eStage() == tCIDOrb::ECmdStages::Orphaned)
            {
                pcqiCur->eStage(tCIDOrb::ECmdStages::Free);
            }
             else
            {
                try
                {
                    //
                    //  Force the stream to see the data that we just snuck
                    //  into its buffer behind its back, and stream the data
                    //  into the orb command object.
                    //
                    strmRead.SetEndIndex(c4DataBytes);
                    pcqiCur->ocmdData().SetReplyMode();
                    strmRead >> pcqiCur->ocmdData();
                }

                catch(TError& errToCatch)
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);

                    // Oops, put the exception we got into the caller's buffer
                    pcqiCur->ocmdData().SetReplyMode();
                    pcqiCur->ocmdData().strmOut().Reset();
                    pcqiCur->ocmdData().strmOut()   << errToCatch
                                                    << kCIDLib::FlushIt;
                    pcqiCur->ocmdData().bRetStatus(kCIDLib::False);

                    // Fall through
                }

                catch(...)
                {
                    TError errStreamFailed
                    (
                        facCIDOrb().strName()
                        , CID_FILE
                        , CID_LINE
                        , facCIDOrb().strMsg(kOrbErrs::errcClient_StreamFailed)
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Unknown
                    );

                    pcqiCur->ocmdData().SetReplyMode();
                    pcqiCur->ocmdData().strmOut().Reset();
                    pcqiCur->ocmdData().strmOut()   << errStreamFailed
                                                    << kCIDLib::FlushIt;
                    pcqiCur->ocmdData().bRetStatus(kCIDLib::False);

                    // Fall through
                }

                // Wake up the thread waiting on this command
                pcqiCur->eStage(tCIDOrb::ECmdStages::Ready);
                pcqiCur->WakeUpWaiter();
            }
        }
         else
        {
            if (facCIDOrb().bLogWarnings())
            {
                facCIDOrb().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , L"The sequence id was not found in the reply queue"
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::NotFound
                );
            }
        }
    }
}


tCIDLib::TVoid TSrvTarget::SendQueued(TThread&, TBinMBufOutStream& strmWrite)
{
    // The socket should be set when we get here
    if (!m_psockSrv)
    {
        if (facCIDOrb().bLogFailures())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , L"The server target socket is not set yet"
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotReady
            );
        }

        // Don't let the queue continue to grow, just flush it
        TLocker lockrSync(&m_mtxSync);
        m_colCmdQ.RemoveAll();
        return;
    }

    // Set up the standard parts of the packet header
    tCIDOrb::TPacketHdr hdrCur;
    hdrCur.c4MagicVal = kCIDOrb_::c4MagicVal;
    hdrCur.c4MagicVal2 = kCIDOrb_::c4MagicVal2;

    while (kCIDLib::True)
    {
        //
        //  Get the next element from the head of the list, which will be the
        //  oldest one added since we add at the tail. We only need to lock
        //  while getting the item out, but once we have it, we can let the
        //  list go again so that clients can queue up more items while we are
        //  in here.
        //
        TCmdQItem* pcqiCur = nullptr;
        {
            TLocker lockrSync(&m_mtxSync);

            // If we don't have any more, then break out
            TDLstNode* pnodeNext = m_colCmdQ.pnodeHead();
            if (!pnodeNext)
                break;

            // Get the item out and remove the node from the queue
            pcqiCur = static_cast<TOrbQNode*>(pnodeNext)->pcqiData();
            m_colCmdQ.RemoveNode(pnodeNext);
        }

        // Lock the item while we work with it
        {
            TLocker lockrCmd(pcqiCur->pmtxLock());

            //
            //  If the command has  been orphaned (which means that the thread
            //  that posted it has given up waiting), then let's mark it free
            //  and drop it.
            //
            if (pcqiCur->eStage() == tCIDOrb::ECmdStages::Orphaned)
            {
                pcqiCur->eStage(tCIDOrb::ECmdStages::Free);
                return;
            }

            try
            {
                // We've got to flatten the command object
                strmWrite.Reset();
                strmWrite << pcqiCur->ocmdData();
                strmWrite.Flush();

                // Build up the header
                hdrCur.c4DataBytes = strmWrite.c4CurPos();
                hdrCur.hshData = strmWrite.mbufData().hshCalcHash
                (
                    kCIDOrb_::c4PacketHashMod, 0, hdrCur.c4DataBytes
                );
                hdrCur.c4SequenceId = pcqiCur->c4SequenceId();
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                CancelCmdWithErr(*pcqiCur, errToCatch);
                throw;
            }

            catch(...)
            {
                CancelCmdWithErr(*pcqiCur, m_errUnknown);
                throw;
            }
        }

        try
        {
            //
            //  We use a facilty method to send, so that encryption can be
            //  centrally managed.
            //
            facCIDOrb().SendMsg(*m_psockSrv, hdrCur, strmWrite.mbufData());
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            CancelCmdWithErr(*pcqiCur, errToCatch);
            throw;
        }

        catch(...)
        {
            CancelCmdWithErr(*pcqiCur, m_errUnknown);
            throw;
        }

        // It all worked, so add it and mark it as being in the reply list
        {
            TLocker lockrCmd(pcqiCur->pmtxLock());
            pcqiCur->eStage(tCIDOrb::ECmdStages::ReplyList);
            m_colRepList.Add(pcqiCur);

            // Reset our perf debugging timer so that we can time this stage
            pcqiCur->SetStartTime();
        }
    }
}


tCIDLib::TVoid TSrvTarget::TryReconnect()
{
    // Try to create a new socket for our end point
    if (bMakeSocket())
    {
        // Remove us from reconn mode now
        m_bReconnMode = kCIDLib::False;
    }
     else
    {
        //
        //  If we are still offline, just in case a command got into the
        //  queue, and there is a small window for this to happen, give it
        //  an immediate error back now.
        //
        ErrorOutWaiting();
    }
}


