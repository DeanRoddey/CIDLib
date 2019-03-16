//
// FILE NAME: CIDRTP_RTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  This file implements an RTP stream receiver.
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
#include    "CIDRTP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDRTPRec,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPRec
// PREFIX: rtptr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPRec: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPRec::TCIDRTPRec() :

    m_c2LastSeqId(0)
    , m_c4LastTime(0)
    , m_c4Stat_BadPackets(0)
    , m_c4Stat_MissingPackets(0)
    , m_c4Stat_OutSeqPackets(0)
    , m_c4Stat_TotalPackets(0)
    , m_c4Stat_WrongSrc(0)
    , m_c4SyncSrc(0)
    , m_colProcQ(tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::EMTStates::Safe)
    , m_enctLastMsg(0)
    , m_prtpioOut(nullptr)
    , m_psockRTP(nullptr)
    , m_psockRTCP(nullptr)
    , m_thrProcess
      (
          facCIDLib().strNextThreadName(TString(L"RTPRecProc"))
          , TMemberFunc<TCIDRTPRec>(this, &TCIDRTPRec::eProcThread)
      )
    , m_thrReader
      (
          facCIDLib().strNextThreadName(TString(L"RTPRecReader"))
          , TMemberFunc<TCIDRTPRec>(this, &TCIDRTPRec::eReadThread)
      )
    , m_thrWriter
      (
          facCIDLib().strNextThreadName(TString(L"RTPRecWriter"))
          , TMemberFunc<TCIDRTPRec>(this, &TCIDRTPRec::eWriteThread)
      )
{
}

TCIDRTPRec::~TCIDRTPRec()
{
    //
    //  Call the cleanup method to clean up anything that wasn't appropriately
    //  taken care of by ending the session appropriately.
    //
    Cleanup();
}


// ---------------------------------------------------------------------------
//  TCIDRTPRec: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If there is an active session, we shut it down. We stop the threads, close the
//  sockets, and terminate the I/O target.
//
tCIDLib::TVoid TCIDRTPRec::EndSession()
{
    // Just call the cleanup method
    Cleanup();
}


//
//  This method starts us up listening for a session. We can't already be started, so
//  we throw if so.
//
tCIDLib::TVoid
TCIDRTPRec::StartSession(const  TIPEndPoint&            ipepRTP
                        , const TIPEndPoint&            ipepRTCP
                        , const TString&                strCName
                        ,       TCIDRTPIOTar* const     prtpioToAdopt)
{
    // Can't get a null I/O object
    if (!prtpioToAdopt)
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcSess_NullIO
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strCName
        );
    }

    //
    //  Until we get fully initialized, make sure that the I/O target gets cleaned up
    //  if we bail.
    //
    TCIDRTPIOJan janTar(prtpioToAdopt);

    // If already running, we have to throw. Use the already set session info here!
    if (m_psockRTP)
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcSess_AlreadyRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , TString(L"RTP Receiver")
            , m_strCName
        );
    }

    // The I/O object cannot have been initialized yet
    if (prtpioToAdopt->bInitialized())
    {
        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcSess_AlreadyInitIO
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
            , strCName
        );
    }

    // Initialize it
    try
    {
        prtpioToAdopt->Initialize();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcSess_InitIO
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
            , strCName
        );
    }

    //
    //  Create the two datagram sockets. Once created they will only be used by the
    //  reader thread (which also sends RTCP msgs, so it's not strictly only a reader
    //  I guess.) Otherwise, only at the end when the session is ended will they
    //  be accessed (to be closed) by anything other than the reader.
    //
    //  We create them here so that we can just throw back to the caller if one or
    //  both cannot be created.
    //
    //  Note that we haven't created the other threads yet, so it doesn't matter if
    //  we haven't stored the I/O target yet or done other setup.
    //
    try
    {
        m_psockRTP = new TServerDatagramSocket(tCIDSock::ESockProtos::UDP, m_ipepRTP);
        m_psockRTCP = new TServerDatagramSocket(tCIDSock::ESockProtos::UDP, m_ipepRTCP);
    }

    catch(TError& errToCatch)
    {
        // Clean up the socket that got opened
        if (m_psockRTP)
        {
            try{delete m_psockRTP;} catch(...){}
            m_psockRTP = 0;
        }

        if (m_psockRTCP)
        {
            try{delete m_psockRTCP;} catch(...){}
            m_psockRTCP = 0;
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        facCIDRTP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kRTPErrs::errcSess_OpenSocks
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Init
            , TString(L"receiver")
            , strCName
        );
    }

    //
    //  It looks like we are ok, so store the other info and then start up the
    //  threads to start processing. We need to generate a sync source id and
    //  store it as we..
    //
    m_strCName = strCName;
    m_c4SyncSrc = 0;

    // Orpan the I/O out of the janitor and store the pointer
    m_prtpioOut = prtpioToAdopt;
    janTar.Orphan();

    //
    //  Start our last msg timer to now to avoid spurious timeout before we get the
    //  first msg.
    //
    m_enctLastMsg = TTime::enctNow();

    // Reset out statistics and per-session memebers
    m_c2LastSeqId = 0;
    m_c4LastTime = 0;
    m_c4Stat_BadPackets = 0;
    m_c4Stat_MissingPackets = 0;
    m_c4Stat_OutSeqPackets = 0;
    m_c4Stat_TotalPackets = 0;
    m_c4Stat_WrongSrc = 0;

    //
    //  Start up the threads in reverse order of the flow, so that each one is ready
    //  for the up-stream data when it starts arriving.
    //
    m_thrProcess.Start();
    m_thrReader.Start();
}



// ---------------------------------------------------------------------------
//  TCIDRTPRec: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Do any required cleanup when we are shut down
//
tCIDLib::TVoid TCIDRTPRec::Cleanup()
{
    // Start any that are still running shutting down, in parallel
    if (m_thrProcess.bIsRunning())
        m_thrProcess.ReqShutdownNoSync();

    if (m_thrReader.bIsRunning())
        m_thrReader.ReqShutdownNoSync();

    if (m_thrWriter.bIsRunning())
        m_thrWriter.ReqShutdownNoSync();

    // And wait for all of them to have died
    try
    {
        m_thrProcess.eWaitForDeath(10000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    try
    {
        m_thrReader.eWaitForDeath(10000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    try
    {
        m_thrWriter.eWaitForDeath(10000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }


    //
    //  Close any sockets that are open and zero their pointers.
    //
    if (m_psockRTP)
    {
        try{delete m_psockRTP;} catch(...){}
        m_psockRTP = 0;
    }

    if (m_psockRTCP)
    {
        try{delete m_psockRTCP;} catch(...){}
        m_psockRTCP = 0;
    }


    // Clean up and delete any I/O target
    if (m_prtpioOut)
    {
        try
        {
            m_prtpioOut->Terminate();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        try
        {
            delete m_prtpioOut;
        }

        catch(...)
        {
        }

        m_prtpioOut = 0;
    }
}


//
//  Our processing thread. We just wait for the reader thread to queue us up data.
//  We grab that data, process it as required, and queue it up again for the writer
//  thread to deal with. This thread queues up RTCP msgs as required since it is
//  the one who knows the state of the data flow, the reader thread sends them since
//  it owns the sockets.
//
tCIDLib::EExitCodes TCIDRTPRec::eProcThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the caller go
    thrThis.Sync();

    //
    //  We have to watch for the first received packet, because that means we have
    //  set sequence and sources, instead of checking against them which we will do
    //  afterwards.
    //
    tCIDLib::TBoolean bFirstPacket = kCIDLib::True;

    while(kCIDLib::True)
    {
        // Watch for shutdown requests
        if (thrThis.bCheckShutdownRequest())
            break;

        try
        {
#if 0
            // Wait a while for a buffer to show up. This provides our throttling
            TCIDRTPBuf* prtpbNew = m_colProcQ.pobjGetNext(250);

            // If none, go back to the top
            if (!prtpbNew)
                continue;

            //
            //  We got one. Put a janitor on it to insure it gets released back to
            //  to the pool.
            //
            TCIDRTPBufJan janBuf(prtpbNew);

            //
            //  We process it according to its type. We have to cast it to its actual
            //  type.
            //
            if (prtpbNew->m_eType == tCIDRTP::EBufType_Media)
            {
                ProcessMediaBuf
                (
                    static_cast<TCIDRTPMediaBuf&>(*prtpbNew)
                    , bFirstPacket
                );
            }
             else if (prtpbNew->m_eType == tCIDRTP::EBufType_Ctrl)
            {
            }
             else
            {
                // <TBD> Should do something about
            }
#endif
        }

        catch(TError& errToCatch)
        {
            if (facCIDRTP().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


//
//  We monitor the RTP and RTPC datagram sockets for msgs to arrive. We give priority
//  to RTP for performance reasons, but have to check the RTCP socket at least so
//  often even if media data is available. We just store away the sender info.
//
tCIDLib::EExitCodes TCIDRTPRec::eReadThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the caller go
    thrThis.Sync();

    //
    //  Set up a multi-select list for our two sockets, so that we can block on input
    //  from either of them. Doesn't have to be thread safe since only we use it.
    //
    //  The select items don't own the sockets, so we can safely pass in pointers
    //  to our socket members.
    //
    TRefVector<TMSockSelItem> colSelList
    (
        tCIDLib::EAdoptOpts::Adopt, 2, tCIDLib::EMTStates::Unsafe
    );
    colSelList.Add(new TMSockSelItem(m_psockRTP));
    colSelList.Add(new TMSockSelItem(m_psockRTCP));

    TIPEndPoint ipepFrom;
    while(kCIDLib::True)
    {
        try
        {
            // Watch for thread shutdown requests
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  We just block a bit, waiting for data to be available on either
            //  socket. This provides our throttling. We'll wait for up to half a
            //  second at a time.
            //
            //  If nothing this time, go back to the top
            //
            if (!TSocket::bMultiReadSel(colSelList, TTime::enctNowPlusMSs(500)))
                continue;

            //
            //  We got something so check them to see which ones reported ready and
            //  read one msgs from each that's ready. We don't try to read more. We'll
            //  get ready indicators again next time if there's more left.
            //
            if (tCIDLib::bAllBitsOn(colSelList[0]->eFlags(), tCIDSock::EMSelFlags::Read))
            {
                //
                //  We have RTP data available.
                //
                //  Read in a packet. This will queue up the data for the processing
                //  thread if the packet is good and not out of sequence and all that
                //  kind of thing. Any failures will adjust the appropriate stats
                //  members.
                //
                ReadMediaPacket(*m_psockRTP, ipepFrom);
            }

            if (tCIDLib::bAllBitsOn(colSelList[1]->eFlags(), tCIDSock::EMSelFlags::Read))
            {
                // We have RTCP data available
                ReadCtrlPacket(*m_psockRTCP, ipepFrom);
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDRTP().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


//
//  We dequeue data from the processing thread and spool it out to the data target.
//  If the data target cannot accept it in a timely manner, that's not our problem.
//  The data target just has to discard what it cannot accept. We can't stop receiving
//  data.
//
//  To make things more efficient, we attempt to dequeue multiple packets of data into
//  more sizeable chunks for writing out to the target. We assume that the target will
//  usually provide some sort of buffering so that if we get a little ahead it won't
//  be a big problem.
//
tCIDLib::EExitCodes TCIDRTPRec::eWriteThread(TThread& thrThis, tCIDLib::TVoid*)
{
    return tCIDLib::EExitCodes::Normal;
}


//
//  Check the passed media buffer to see if it's good and process it if so. We bump
//  appropriate stats.
//
tCIDLib::TVoid
TCIDRTPRec::ProcessMediaBuf(const   TCIDRTPMediaBuf&    rtpbCheck
                            ,       tCIDLib::TBoolean&  bFirstPacket)
{
#if 0
    const tCIDLib::TCard4 c4Sz = rtpbCheck.m_c4ByteCnt;

    // Bump total packets received even if it ends up being unusuable
    m_c4Stat_TotalPackets++;

    // It has to be at least big enough to hold the fixed header
    if (c4Sz < sizeof(tCIDRTP::TRTPPacket))
    {
        m_c4Stat_BadPackets++;
        return;
    }

    //
    //  If this is the first packet, then we need to initialize our session oriented
    //  counters. Else we test against what we have.
    //
    if (bFirstPacket)
    {
        bFirstPacket = kCIDLib::False;

        m_c2LastSeqId = rtpbCheck.c2SeqNum();
        m_c4LastTime = rtpbCheck.c4TimeStamp();
        m_c4SyncSrc = rtpbCheck.c4SyncSrc();
    }
     else
    {
        // If it's not for the active sync source, then ignore it
        if (rtpbCheck.c4SyncSrc() != m_c4SyncSrc)
        {
            m_c4Stat_WrongSrc++;
            return;
        }

        //
        //  If the sequence is before the last one, we ignore it. We don't bother to
        //  try to figure out if we could actually keep it.
        //
        if (rtpbCheck.c2SeqNum() < m_c2LastSeqId)
        {
            m_c4Stat_OutSeqPackets++;
            return;
        }
    }

    // If it's not a payload type we understand, then reject it
    if (rtpbCheck.c4PayloadType() != 0)
    {
        m_c4Stat_MediaFmt++;
        return;
    }

    //
    //  Figure out the actual header size and therefore where the data starts, and
    //  then the count of data bytes.
    //
    tCIDLib::TCard4 c4DataStart
    (
        sizeof(tCIDRTP::TRTPPacket) + (rtpbCheck.c4CSrcCount() * 32)
    );

    // If theres any extensions, then ignore them by moving the start forward
    if (rtpbCheck.m_mbufData[0] & 0x8)
    {
        // Get the length, and skip forward that many plus 1 4 byte chunks
        tCIDLib::TCard2 c4XCnt = rtpbCheck.m_mbufData[c4DataStart + 2];
        c4XCnt <<= 8;
        c4XCnt |= rtpbCheck.m_mbufData[c4DataStart + 3];

        c4DataStart += (c4XCnt + 1) * 4;
    }

    // If the data start ended up past the buffer size, something is wrong
    if (c4DataStart > c4Sz)
    {
        m_c4Stat_BadPackets++;
        return;
    }

    // The count is the full size minus where we figured out the start index is
    tCIDLib::TCard4 c4DataCnt = c4Sz - c4DataStart;

    // If there's any padding, remove that
    if (rtpbCheck.m_mbufData[0] & 0x4)
    {

    }

    // Looks good, so let's process it
#endif
}



//
//  This is called when we see data ready on our RTP socket. We read in a packet
//  and queue it up for the processing thread to deal with.
//
tCIDLib::TVoid
TCIDRTPRec::ReadMediaPacket(TDatagramSocket& sockSrc, TIPEndPoint& ipepFrom)
{
    try
    {
#if 0
        // Get us a packet to use
        TCIDRTPBufJan janBuf(tCIDRTP::EBufType_Media);
        TCIDRTPBuf& rtpbBuf = janBuf.rtpbOwned();

        const tCIDLib::TCard4 c4Sz = sockSrc.c4ReceiveMBufFrom
        (
            ipepFrom, rtpbBuf.m_mbufData, TTime::enctNowPlusMSs(500)
        );

        //
        //  The read didn't fail, so queue it up. We have to orphan it out of the
        //  janitor so that it won't release it. The processing thread is now
        //  responsible.
        //
        rtpbBuf.m_c4ByteCnt = c4Sz;
        m_colProcQ.Add(janBuf.prtpbOrphan());
#endif
    }

    catch(TError& errToCatch)
    {
        if (facCIDRTP().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}



//
//  This is called when we see data ready on our RTCP socket. We read in a packet
//  and
//
tCIDLib::TVoid
TCIDRTPRec::ReadCtrlPacket(TDatagramSocket& sockSrc, TIPEndPoint& ipepFrom)
{
    try
    {
#if 0
        // Get us a packet to use
        TCIDRTPBufJan janBuf(tCIDRTP::EBufType_Ctrl);
        TCIDRTPBuf& rtpbBuf = janBuf.rtpbOwned();

        const tCIDLib::TCard4 c4Sz = sockSrc.c4ReceiveMBufFrom
        (
            ipepFrom, rtpbBuf.m_mbufData, TTime::enctNowPlusMSs(500)
        );

        //
        //  The read didn't fail, so queue it up. We have to orphan it out of the
        //  janitor so that it won't release it. The processing thread is now
        //  responsible.
        //
        rtpbBuf.m_c4ByteCnt = c4Sz;
        m_colProcQ.Add(janBuf.prtpbOrphan());
#endif
    }

    catch(TError& errToCatch)
    {
        if (facCIDRTP().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}



