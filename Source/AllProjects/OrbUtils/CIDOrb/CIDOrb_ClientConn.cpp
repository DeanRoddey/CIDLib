//
// FILE NAME: CIDOrb_ClientConn.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/09/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is implementation file for the client connection class.
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
RTTIDecls(TOrbClientConnImpl,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TOrbClientConnImpl
//  PREFIX: occ
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbClientConnImpl: Constructor and Destructor
// ---------------------------------------------------------------------------
TOrbClientConnImpl::
TOrbClientConnImpl(         TServerStreamSocket* const  psockThis
                    , const TIPEndPoint&                ipepClient
                    ,       TOrbClientConnMgr* const    poccmOwner
                    , const tCIDLib::TCard8             c8ConnId) :

    m_bOffline(kCIDLib::False)
    , m_c8ConnId(c8ConnId)
    , m_colReplyQ(tCIDLib::EMTStates::Safe)
    , m_enctLastMsg(TTime::enctNow())
    , m_evSocket(tCIDLib::EEventStates::Reset)
    , m_evWorkAvail(tCIDLib::EEventStates::Reset)
    , m_ipepClient(ipepClient)
    , m_mbufIO(kCIDOrb_::c4SmallIOBufSz, kCIDOrb_::c4SmallIOBufSz)
    , m_strmOut(&m_mbufIO)
    , m_strmIn(m_strmOut)
    , m_poccmOwner(poccmOwner)
    , m_psockThis(psockThis)
    , m_thrSpooler
      (
        facCIDOrb().strNextSpoolThreadName(kCIDLib::True)
        , TMemberFunc<TOrbClientConnImpl>(this, &TOrbClientConnImpl::eSpoolThread)
      )
{
    // Associate our socket event with the socket
    m_psockThis->AssociateReadEvent(m_evSocket);

    // Start up the spooling thread
    m_thrSpooler.Start();
}

TOrbClientConnImpl::~TOrbClientConnImpl()
{
    //
    //  Take one last swipe at the reply queue in case something snuck
    //  in at the last minute.
    //
    m_colReplyQ.RemoveAll();
}


// ---------------------------------------------------------------------------
//  TOrbClientConnImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get our offline status
tCIDLib::TBoolean TOrbClientConnImpl::bOffline() const
{
    return m_bOffline;
}


// Get our connection id
tCIDLib::TCard8 TOrbClientConnImpl::c8ConnId() const
{
    return m_c8ConnId;
}


// Get the end point of the client
const TIPEndPoint& TOrbClientConnImpl::ipepClient() const
{
    return m_ipepClient;
}


//
//  The worker threads call into here to have us queue up replies to send
//  back. We just add it to our send queue, which is thread safe, but we need
//  to explicitly lock it because we need to synhronize the triggering of the
//  new work available event.
//
//  If we are already offline, but he connection manager hasn't removed us
//  from the list yet, then we just delete the work item instead of queuing
//  it.
//
tCIDLib::TVoid TOrbClientConnImpl::SendReply(TWorkQItemPtr& wqipToSend)
{
    TMtxLocker mtxlSync(m_colReplyQ.pmtxLock());

    if (m_bOffline)
    {
        // Just let the counted pointer release it
        m_poccmOwner->IncDroppedPackets();
    }
     else
    {
        // We take ownership of it to send back
        m_colReplyQ.objAdd(wqipToSend);
        m_evWorkAvail.Trigger();
    }
}


// Do a polite shutdown of our socket
tCIDLib::TVoid TOrbClientConnImpl::Shutdown()
{
    // If it's already offline, then nothing to do
    if (m_bOffline)
        return;

    //
    //  First shut down the spooling thread, but this can be called by
    //  the spooler thread to self-shutdown, in which case we don't want
    //  to try to do this. Worker threads can still be queuing up replies
    //  on us while this is happening, but that's a good thing since we
    //  cannot lock them up. We'll destroy the unprocessed replies at
    //  the end.
    //
    if ((TThread::pthrCaller() != &m_thrSpooler) && m_thrSpooler.bIsRunning())
    {
        try
        {
            m_thrSpooler.ReqShutdownSync(10000);
            m_thrSpooler.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            // If its not just a timeout, then log it
            if (facCIDOrb().bShouldLog(errToCatch)
            &&  (errToCatch.eClass() != tCIDLib::EErrClasses::Timeout))
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
                    , kOrbMsgs::midStatus_SpoolShutdownErr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Term
                    , m_ipepClient
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
                    , kOrbMsgs::midStatus_SpoolShutdownErr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Term
                    , m_ipepClient
                );
            }
        }
    }

    //
    //  At this point, we have to deal with us and the connection manager
    //  both calling us at the same time. So lock on the reply queue, so
    //  that only one will actually do this cleanup. It also deals with
    //  worker threads calling in to queue up replies. They can't slip one
    //  in while we are shutting down. When we unblock, they'll wake up
    //  and see that we are down and just drop the reply on the floor.
    //
    TMtxLocker mtxlSync(m_colReplyQ.pmtxLock());

    // Shut down the socket if not already
    if (m_psockThis && !m_psockThis->bIsShutdown())
    {
        try
        {
            m_psockThis->c4Shutdown();
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                facCIDOrb().LogEventObj(errToCatch);
            }
        }
    }

    try
    {
        delete m_psockThis;
        m_psockThis = nullptr;
    }

    catch(TError& errToCatch)
    {
        m_psockThis = nullptr;
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            facCIDOrb().LogEventObj(errToCatch);
        }
    }

    // Release any work items in our reply queue
    m_colReplyQ.RemoveAll();

    //
    //  And finally mark us offline. We have to do this last, because once
    //  we set this flag, the connection manager's thread will remove us
    //  from the list, which could destroy us.
    //
    m_bOffline = kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TOrbClientConnImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We start our spooler thread on this method. We watch for incoming data
//  to read and queue or outgoing data to send back to the client.
//
//  NOTE: We cannot be dropped while in here. Only we shut ourself down,
//  and the connection manager won't drop us from his list until we are
//  marked as down.
//
tCIDLib::EExitCodes
TOrbClientConnImpl::eSpoolThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    tCIDLib::TBoolean       bDone = kCIDLib::False;
    tCIDSock::ESockEvs   eEvents;
    while(!bDone)
    {
        try
        {
            //
            //  Block on our two events for a while. We'll either time out
            //  and just go back to the top, or we have something to do.
            //
            const tCIDLib::TCard4 c4Which = TEvent::c4WaitMultiple
            (
                m_evSocket, m_evWorkAvail, 250
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
                //  See which socket event occured. We either read in
                //  data until we run out or the client dropped and we
                //  need to shut ourself down. Note that this also will
                //  reset the event in an appropriately atomic manner.
                //
                eEvents = m_psockThis->eEnumEvents(m_evSocket);
                if (tCIDLib::bAllBitsOn(eEvents, tCIDSock::ESockEvs::Read))
                {
                    ReadCmds();
                }
                 else if (tCIDLib::bAllBitsOn(eEvents, tCIDSock::ESockEvs::Close))
                {
                    // Just a regular old shutdown of the other side
                    if (facCIDOrb().bLogInfo())
                    {
                        facCIDOrb().LogMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , kOrbMsgs::midStatus_ClientDropped
                            , tCIDLib::ESeverities::Info
                            , tCIDLib::EErrClasses::AppStatus
                            , m_ipepClient
                        );
                    }
                    Shutdown();
                    bDone = kCIDLib::True;
                }
            }
             else if (c4Which == 1)
            {
                // Reset the event before we start processing
                m_evWorkAvail.Reset();
                SendReplies();
            }
             else
            {
                //
                //  See how long it's been since we heard from this client.
                //  If it's been more than the max idle time, we need to
                //  give up and close ourself down.
                //
                //  If ont that, then just check for a shutdown request.
                //
                if ((m_enctLastMsg + kCIDOrb::enctMaxIdle < TTime::enctNow())
                ||  thrThis.bCheckShutdownRequest())
                {
                    Shutdown();
                    bDone = kCIDLib::True;
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

            // Assume the worst and close us down
            Shutdown();
            bDone = kCIDLib::True;
        }

        catch(...)
        {
            // Assume the worst and close us down
            Shutdown();
            bDone = kCIDLib::True;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}



//
//  This is called from the spooler thread when data becomes available on our
//  socket. We read in however many commands are available and queue them up
//  on the connection manager for processing.
//
tCIDLib::TVoid TOrbClientConnImpl::ReadCmds()
{
    //
    //  Read in work until we drain the socket.
    //
    //  We call a helper method in the client connection object that will
    //  do the reading and queuing up of the data for us. We have to pass
    //  him a ref to us, so that he can put some info about us into the
    //  work item that will be queued up. We provide him with a buffer
    //  and stream so that he doesn't have to create them or use some
    //  sort of pool.
    //
    while (kCIDLib::True)
    {
        const tCIDOrb::EReadRes eRes = m_poccmOwner->eQueueUpWork
        (
            m_thrSpooler, *m_psockThis, *this, m_mbufIO, m_strmIn
        );

        if (eRes == tCIDOrb::EReadRes::NoPacket)
        {
            // No more data, so we are done reading for now
            break;
        }
         else if ((eRes == tCIDOrb::EReadRes::Packet)
              ||  (eRes == tCIDOrb::EReadRes::KeepAlive))
        {
            //
            //  In either of these cases we want to update our last msg
            //  time since we've seen activity from the client.
            //
            m_enctLastMsg = TTime::enctNow();
        }
         else
        {
            // Oh well, we failed, so give up
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcServ_LostConn
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
                , m_ipepClient
            );
        }
    }
}


//
//  This is called from the spooler thread when there are items on the
//  reply queue to send back. We just process all of the items in the
//  queue and send them back.
//
tCIDLib::TVoid TOrbClientConnImpl::SendReplies()
{
    // Some parts of the header can be pre-done since it stays the same
    tCIDOrb::TPacketHdr hdrCur;
    hdrCur.c4MagicVal   = kCIDOrb_::c4MagicVal;
    hdrCur.c4MagicVal2  = kCIDOrb_::c4MagicVal2;

    while (kCIDLib::True)
    {
        try
        {
            //
            //  Get the next available item, with zero timeout so we only get what's
            //  already there. It is kind of awkward that we have to create a queue
            //  item pointer every round even if if it's not used. But, we only get
            //  called when it's known we have stuff to send. So, worst case we
            //  should only create one unneeded one. They are very light weight.
            //
            TWorkQItemPtr wqipCur;
            if (!m_colReplyQ.bGetNext(wqipCur, 0, kCIDLib::False))
                break;

            // Get the command object which has the reply info
            TOrbCmd& orbcCur = wqipCur->ocmdThis();

            //
            //  Stream the command back out. Put it into reply mode so that
            //  it will stream out in reply format.
            //
            orbcCur.SetReplyMode();

            //
            //  If we can use our local I/O stream/buffer, then we will.
            //  Else, we will allocate a local buffer big enough to hold
            //  the data and use that.
            //
            TMemBuf*            pmbufToUse = &m_mbufIO;
            TBinMBufOutStream*  pstrmToUse = &m_strmOut;
            THeapBuf*           pmbufBig = nullptr;
            TBinMBufOutStream*  pstrmBig = nullptr;

            const tCIDLib::TCard4 c4PayLoad = wqipCur->ocmdThis().c4PayloadBytes();
            const tCIDLib::TCard4 c4ToSend = c4PayLoad + facCIDOrb().c4ReplyOverhead();
            if (c4ToSend > m_mbufIO.c4MaxSize())
            {
                pmbufBig = new THeapBuf(c4ToSend, c4ToSend);
                pstrmBig = new TBinMBufOutStream(pmbufBig, tCIDLib::EAdoptOpts::Adopt);
                pmbufToUse = pmbufBig;
                pstrmToUse = pstrmBig;
            }
            TJanitor<TBinMBufOutStream> janBig(pstrmBig);

            // Ok, stream out ot the stream we are using
            pstrmToUse->Reset();
            *pstrmToUse << wqipCur->ocmdThis() << kCIDLib::FlushIt;

            // Fill in the per-message parts of the header
            hdrCur.c4DataBytes  = pstrmToUse->c4CurPos();
            hdrCur.c4SequenceId = wqipCur->ocmdThis().c4SequenceId();
            hdrCur.hshData      = pmbufToUse->hshCalcHash
            (
                kCIDOrb_::c4PacketHashMod, 0, hdrCur.c4DataBytes
            );

            // And now try to send the response
            facCIDOrb().SendMsg(*m_psockThis, hdrCur, *pmbufToUse);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            m_poccmOwner->IncDroppedPackets();
            throw;
        }
    }
}

