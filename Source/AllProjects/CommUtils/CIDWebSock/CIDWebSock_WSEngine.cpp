//
// FILE NAME: CIDWebSock_WSEngine.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2017
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
//  This file implements the actual server side web sockets engine framework.
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
#include    "CIDWebSock_.hpp"


//#if CID_DEBUG_ON
#define WS_LOGSTATEINFO 1
//#endif
#if defined(WS_LOGSTATEINFO)
#define LogStateInfo(msg) facCIDWebSock().LogMsg \
( \
    CID_FILE \
    , CID_LINE \
    , TString(L"Websock Trace: ") + msg \
    , tCIDLib::ESeverities::Status \
    , tCIDLib::EErrClasses::AppStatus \
)
#define LogStateInfo2(msg,p1) facCIDWebSock().LogMsg \
( \
    CID_FILE \
    , CID_LINE \
    , TString(L"Websock Trace: ") + msg \
    , tCIDLib::ESeverities::Status \
    , tCIDLib::EErrClasses::AppStatus \
    , p1 \
)
#else
#define LogStateInfo(msg)
#define LogStateInfo2(msg,p1)
#endif



// ---------------------------------------------------------------------------
//   CLASS: TCIDWebSockThread
//  PREFIX: thr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Public, static methods
// ---------------------------------------------------------------------------

// Indicate whether the msg type is a control type message
tCIDLib::TBoolean TCIDWebSockThread::bIsControlType(const tCIDLib::TCard1 c1Type)
{
    return
    (
        (c1Type == kCIDWebSock::c1WSockMsg_Close)
        || (c1Type == kCIDWebSock::c1WSockMsg_Ping)
        || (c1Type == kCIDWebSock::c1WSockMsg_Pong)
    );
}


// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Destructor
// ---------------------------------------------------------------------------
TCIDWebSockThread::~TCIDWebSockThread()
{
    //
    //  If by some chance we didn't manage to get the data source cleaned up, and
    //  we were told to adopt it, then clean it up. Perhaps the thread never got
    //  started up or something.
    //
    if (m_pcdsServer && m_bAdoptSrc)
    {
        try
        {
            if (m_pcdsServer->bIsInitialized())
                m_pcdsServer->Terminate(0, kCIDLib::True);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        try
        {
            delete m_pcdsServer;
            m_pcdsServer = nullptr;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The derived class can tell us to log messages that go in and out.
//
tCIDLib::TVoid
TCIDWebSockThread::EnableMsgLogging(const tCIDLib::TBoolean bState, const TString& strPath)
{
    // Whether false or true, close any existing output file
    if (m_pstrmLog)
    {
        try
        {
            delete m_pstrmLog;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        m_pstrmLog = 0;
    }

    // If true, then open up the file again, truncating any existing content
    if (bState)
    {
        try
        {
            m_pstrmLog = new TTextFileOutStream
            (
                strPath
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Write
                , new TUTF8Converter()
            );

            m_tmLog.SetToNow();
            *m_pstrmLog << L"Log opened at " << m_tmLog << kCIDLib::NewEndLn;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


//
//  A helper to send a text msg. We transcode it to a buffer in UTF-8 format, and then
//  pass it on to the base sender.
//
tCIDLib::TVoid TCIDWebSockThread::SendTextMsg(const TString& strText)
{
    // If have been told to log msgs, then do that
    if (m_pstrmLog)
    {
        m_tmLog.SetToNow();
        *m_pstrmLog << L"----- Sending (" << m_tmLog << L") -----\n"
                    << strText << kCIDLib::NewEndLn;
    }

    // Convert it into our write buffer
    tCIDLib::TCard4 c4Bytes;
    m_tcvtText.c4ConvertTo(strText, m_mbufWriteMsg, c4Bytes);

    // And now call the basic binary sender to send it
    SendMsg(kCIDWebSock::c1WSockMsg_Text, m_mbufWriteMsg, c4Bytes);
}



//
//  The derived class can call this if he wants to start the shutdown process, initiated
//  from our side. If we are not a state that indicate the client already started the
//  close, then we send the close and then
//
tCIDLib::TVoid TCIDWebSockThread::StartShutdown(const tCIDLib::TCard2 c2Code)
{
    SendClose(c2Code);
    m_eState = tCIDWebSock::EStates::WaitClientEnd;

    // Set our timer for how long we will wait for the reply
    m_enctEndTimer = TTime::enctNowPlusSecs(10);
}



// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Hidden constructors
// ---------------------------------------------------------------------------
TCIDWebSockThread::TCIDWebSockThread(const  TString&            strName
                                    ,       TCIDDataSrc*        pcdsToUse
                                    , const tCIDLib::TBoolean   bAdoptSrc) :

    TThread(strName)
    , m_bAdoptSrc(bAdoptSrc)
    , m_bSimMode(kCIDLib::True)
    , m_bWaitPong(kCIDLib::False)
    , m_c4PingVal(TTime::c4Millis())
    , m_eState(tCIDWebSock::EStates::WaitStart)
    , m_enctEndTimer(0)
    , m_enctLastInMsg(0)
    , m_enctLastOutMsg(0)
    , m_mbufRead(kCIDWebSock::c4MaxWebsockFragSz, kCIDWebSock::c4MaxWebsockFragSz)
    , m_mbufReadFrag(kCIDWebSock::c4MaxWebsockFragSz, kCIDWebSock::c4MaxWebsockFragSz)
    , m_mbufReadMsg(kCIDWebSock::c4MaxWebsockFragSz, kCIDWebSock::c4MaxWebsockMsgSz, 0x100000)
    , m_mbufWriteMsg(kCIDWebSock::c4MaxWebsockFragSz, kCIDWebSock::c4MaxWebsockMsgSz, 0x100000)
    , m_pcdsServer(pcdsToUse)
    , m_pstrmLog(nullptr)
{
    m_tmLog.strDefaultFormat(TTime::strMMDD_HHMMSS());
}



// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  Thread processing starts here.
//
tCIDLib::EExitCodes TCIDWebSockThread::eProcess()
{
    // We can't be in sim mode since we are being started in a separate thread
    m_bSimMode = kCIDLib::False;

    // Let the calling thread go
    Sync();

    //
    //  Call the service method. This is separated out, because it may need to be called
    //  directly if a containing application wants to do some sort of simulator mode.
    //
    const tCIDLib::EExitCodes eRes = eServiceClient();

    // If a logging stream was opened, then close it if it wasn't already
    if (m_pstrmLog)
    {
        try
        {
            delete m_pstrmLog;
        }

        catch(...)
        {
        }
        m_pstrmLog = nullptr;
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  If the derived class doesn't override, we just do nothing.
//
tCIDLib::TVoid TCIDWebSockThread::WSTerminate()
{
}


// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method implements the server side support for a websockets session. This is
//  called from eProcess() which is where the thread starts up. But we separate this
//  out so that it could be called from something else for testing purposes.
//
tCIDLib::EExitCodes TCIDWebSockThread::eServiceClient()
{
    //
    //  Make sure we have a data source and we are in WaitStart mode. Otherwise we have
    //  either gotten a null data source or are already running or finished.
    //
    if (!m_pcdsServer || (m_eState != tCIDWebSock::EStates::WaitStart))
    {
        facCIDWebSock().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kWSockErrs::errcWSock_CantStart
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
        return tCIDLib::EExitCodes::InitFailed;
    }

    // We are running now, so move to connecting state
    m_eState = tCIDWebSock::EStates::Connecting;

    try
    {
        // Let the derived class initialize
        if (!bWSInitialize())
            return tCIDLib::EExitCodes::InitFailed;

        //
        //  We accumulate msgs in frames. So we need to track how much we gotten so
        //  far and the original msg type.
        //
        tCIDLib::TBoolean   bFinal;
        tCIDLib::TCard1     c1MsgType = 0xFF;
        tCIDLib::TCard4     c4MsgLen = 0;

        //
        //  Initialize some timers we use to decide it's time to do things. Do the
        //  initial fld poll time a little longer than we subsequently do. To give
        //  things time to settle down before we start checking.
        //
        m_enctLastInMsg = TTime::enctNow();
        m_enctLastOutMsg = m_enctLastInMsg;

        //
        //  Now we can loop until it's time to close down.
        //
        m_eState = tCIDWebSock::EStates::Connecting;
        m_enctEndTimer = 0;
        while(m_eState != tCIDWebSock::EStates::End)
        {
            //
            //  If not in sim mode check for a shutdown. We can't do it in sim mode because
            //  we aren't running as a separate thread and so we were never started up as a
            //  thread, and this would cause an error. So in that scenario, if not in idle
            //  mode, we just call the idle callback.
            //
            //  In sim mode we call the CheckShutdownReq() method, which lets the derived
            //  class watch for shutdown requests. If so, they will thrown a debug exit
            //  exception.
            //
            if (!m_bSimMode)
            {
                if (bCheckShutdownRequest())
                {
                    //
                    //  Send a close message indicating going away. Don't wait for reply.
                    //  We need to stop and get shutdown quickly.
                    //
                    try
                    {
                        LogStateInfo(L"Asked to stop, shutting down session");
                        SendClose(kCIDWebSock::c2WSockErr_Exiting);
                    }

                    catch(...)
                    {
                    }
                    break;
                }
            }
             else
            {
                WSCheckShutdownReq();
            }

            //
            //  See if we need to do some time driven stuff.
            //
            {
                tCIDLib::TEncodedTime enctCur = TTime::enctNow();

                //
                //  If we haven't gotten a message from the client in the last two
                //  minutes, then give up.
                //
                if ((enctCur - m_enctLastInMsg) > (kCIDLib::enctOneMinute * 2))
                {
                    LogStateInfo(L"Connection timeout, closing hard");

                    //
                    //  We don't do the normal thing since we assume he's not going
                    //  respond and it would just waste time to wait.
                    //
                    SendClose(kCIDWebSock::c2WSockErr_Timeout);
                    break;
                }

                //
                //  If it's been over a minute since the last incoming message, and we
                //  aren't currently waiting on a pong reply, then send a ping.
                //
                if (((enctCur - m_enctLastInMsg) > (kCIDLib::enctOneSecond * 25)) && !m_bWaitPong)
                {
                    SendPing();
                    enctCur = TTime::enctNow();
                }
            }

            switch(m_eState)
            {
                case tCIDWebSock::EStates::Connecting :
                    LogStateInfo(L"Connecting");
                    WSConnected();

                    m_eState = tCIDWebSock::EStates::Idle;
                    break;

                case tCIDWebSock::EStates::Idle :
                {
                    // Check for an incoming msg
                    tCIDLib::TCard1 c1FragType;
                    tCIDLib::TCard4 c4FragLen;

                    if (bGetFragment(c1FragType, bFinal, m_mbufReadFrag, c4FragLen))
                    {
                        if (c1FragType == kCIDWebSock::c1WSockMsg_Cont)
                        {
                            // This can't be valid
                            LogStateInfo(L"Unstarted continuation");
                            StartShutdown(kCIDWebSock::c2WSockErr_UnstartedCont);
                        }
                         else if (bFinal)
                        {
                            // It's a single fragment msg
                            HandleMsg(c1FragType, m_mbufReadFrag, c4FragLen);
                        }
                         else
                        {
                            // It has to be a binary or text msg
                            if ((c1FragType == kCIDWebSock::c1WSockMsg_Text)
                            ||  (c1FragType == kCIDWebSock::c1WSockMsg_Bin))
                            {
                                LogStateInfo(L"Got first fragment");

                                m_mbufReadMsg.CopyIn(m_mbufReadFrag, c4FragLen, 0);
                                c1MsgType = c1FragType;
                                c4MsgLen = c4FragLen;

                                // Move to in message state to wait for remainder
                                m_eState = tCIDWebSock::EStates::InMsg;
                            }
                             else
                            {
                                LogStateInfo(L"Bad fragment, close");
                                StartShutdown(kCIDWebSock::c2WSockErr_BadFragMsg);
                            }
                        }
                    }
                     else
                    {
                        // Call the idle callback
                        WSIdle();
                    }
                    break;
                }

                case tCIDWebSock::EStates::InMsg :
                {
                    //
                    //  We are accumulating a msg, get another frame. THough it may
                    //  be a control message arriving inside a fragmented data msg.
                    //
                    tCIDLib::TCard1 c1FragType;
                    tCIDLib::TCard4 c4FragLen;
                    if (bGetFragment(c1FragType, bFinal, m_mbufReadFrag, c4FragLen))
                    {
                        if (c1FragType == kCIDWebSock::c1WSockMsg_Cont)
                        {
                            if (bFinal)
                                LogStateInfo(L"Got final fragment");
                            else
                                LogStateInfo(L"Got cont fragment");

                            // Add it to our accumulating msg
                            m_mbufReadMsg.CopyIn(m_mbufReadFrag, c4FragLen, c4MsgLen);
                            c4MsgLen += c4FragLen;

                            // If this is the end, then
                            if (bFinal)
                            {
                                // Go back to Idle state
                                m_eState = tCIDWebSock::EStates::Idle;
                                HandleMsg(c1FragType, m_mbufReadMsg, c4MsgLen);
                            }
                        }
                         else
                        {
                            // It has to be a nested control frame
                            if (bFinal)
                            {
                                if (!bIsControlType(c1FragType))
                                {
                                    LogStateInfo(L"New message within previous msg");
                                    StartShutdown(kCIDWebSock::c2WSockErr_Nesting);
                                }
                                 else
                                {
                                    HandleMsg(c1FragType, m_mbufReadFrag, c4FragLen);
                                }
                            }
                             else
                            {
                                LogStateInfo(L"Non-final ctrl msg");
                                StartShutdown(kCIDWebSock::c2WSockErr_NonFinalCtrl);
                            }
                        }
                    }
                    break;
                }

                case tCIDWebSock::EStates::WaitClientEnd :
                {
                    //
                    //  We are waiting for the client to send the return close after we
                    //  sent one first.
                    //
                    tCIDLib::TCard1 c1FragType;
                    tCIDLib::TCard4 c4FragLen;

                    if (bGetFragment(c1FragType, bFinal, m_mbufReadFrag, c4FragLen))
                    {
                        // Break out when we get the close. Ignore everything else
                        if (bFinal && (c1FragType == kCIDWebSock::c1WSockMsg_Close))
                        {
                            LogStateInfo(L"Got client close response, exiting");
                            HandleMsg(c1FragType, m_mbufReadFrag, c4FragLen);
                            m_eState = tCIDWebSock::EStates::End;
                        }
                    }
                     else if (TTime::enctNow() > m_enctEndTimer)
                    {
                        LogStateInfo(L"No response to Close, giving up");
                        m_eState = tCIDWebSock::EStates::End;
                    }
                    break;
                }

                case tCIDWebSock::EStates::End :
                    LogStateInfo(L"Ending session");
                    break;

                default :
                    break;
            };
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        facCIDWebSock().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kWSockMsgs::midStatus_WebSockExcept
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        Cleanup();
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        facCIDWebSock().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kWSockMsgs::midStatus_WebSockExcept
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        Cleanup();
        return tCIDLib::EExitCodes::RuntimeError;
    }

    // Make sure we get cleaned up
    Cleanup();

    return tCIDLib::EExitCodes::Normal;
}



//
//  A helper for sending 16 bit payloads. We just adjust the payload for byte order as
//  required and call the base sender.
//
tCIDLib::TVoid
TCIDWebSockThread::SendBinMsg(const tCIDLib::TCard1 c1Type, const tCIDLib::TCard2 c2Payload)
{
    tCIDLib::TCard2 c2Send;
    #if defined(CID_LITTLEENDIAN)
    c2Send = tCIDLib::c2SwapBytes(c2Payload);
    #else
    c2Send = c2Payload;
    #endif

    // Call the low level sender
    SendMsg(c1Type, &c2Send, 2);
}


//
//  A simple helper that takes a memory buffer. We just turn around can call the base
//  message sender, passing the raw buffer pointer from the memory buffer.
//
tCIDLib::TVoid
TCIDWebSockThread::SendMsg(const   tCIDLib::TCard1 c1Type
                        , const THeapBuf&       mbufData
                        , const tCIDLib::TCard4 c4DataCnt)
{
    // Pass it to the lower level method
    SendMsg(c1Type, mbufData.pc1Data(), c4DataCnt);
}


//
//  This is the base message sender. It handles message fragmentation if needed. The
//  other senders call this guy.
//
tCIDLib::TVoid
TCIDWebSockThread::SendMsg(const   tCIDLib::TCard1         c1Type
                        , const tCIDLib::TVoid* const   pData
                        , const tCIDLib::TCard4         c4DataCnt)
{
    tCIDLib::TCard1 ac1Hdr[16];

    //
    //  We never send a message longer than our max packet size, fragmenting it if so.
    //  It's not too likely any will be that long anyway.
    //
    const tCIDLib::TCard1* pac1Data = static_cast<const tCIDLib::TCard1*>(pData);
    tCIDLib::TCard4 c4SoFar = 0;
    while (c4SoFar < c4DataCnt)
    {
        const tCIDLib::TCard4 c4Left = c4DataCnt - c4SoFar;

        //
        //  See how many bytes this time around, and set up a flags byte based on the
        //  size info, for use below.
        //
        tCIDLib::TCard4 c4ThisTime;
        tCIDLib::TCard1 c1Flags = 0;
        if (c4Left < kCIDWebSock::c4MaxWebsockFragSz)
        {
            c4ThisTime = c4Left;
            c1Flags = 0x80;
        }
         else
        {
            c4ThisTime = kCIDWebSock::c4MaxWebsockFragSz;
        }

        //
        //  First fragment is the actual type, else a continuation packet, and add in
        //  the flags byte we set up above.
        //
        tCIDLib::TCard4 c4Cnt = 0;
        if (c4SoFar)
            ac1Hdr[c4Cnt++] = kCIDWebSock::c1WSockMsg_Cont | c1Flags;
        else
            ac1Hdr[c4Cnt++] = c1Type | c1Flags;

        //
        //  And the second one based on the length. We never do the four byte form,
        //  since we have a maximum fragment size that won't go over 64K.
        //
        if (c4ThisTime < 126)
        {
            ac1Hdr[c4Cnt++] = tCIDLib::TCard1(c4ThisTime);
        }
         else
        {
            ac1Hdr[c4Cnt++] = 126;
            ac1Hdr[c4Cnt++] = tCIDLib::TCard1(c4ThisTime >> 8);
            ac1Hdr[c4Cnt++] = tCIDLib::TCard1(c4ThisTime);
        }

        // Send the header stuff first
        m_pcdsServer->WriteRawBytes(ac1Hdr, c4Cnt);

        //
        //  And then the payload data, so that we don't have to take the overhead hit
        //  of copying the data the data just to send it.
        //
        if (c4DataCnt)
            m_pcdsServer->WriteRawBytes(&pac1Data[c4SoFar], c4ThisTime);

        // Flush the data source output now
        m_pcdsServer->FlushOut(TTime::enctNowPlusSecs(8));

        // Move the so far up by the bytes we did this time
        c4SoFar += c4ThisTime;
    }

    // Update the last out message timestamp
    m_enctLastOutMsg = TTime::enctNow();
}


// ---------------------------------------------------------------------------
//  TCIDWebSockThread: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to get a web socket fragment. If not, we just return and the main
//  loop can do other things. The main loop handles putting fragments back together.
//
//  BUT THE WAIT time we do initially, to check for data read, provides the main loop's
//  throttling, so keep it some short but reasonable length.
//
tCIDLib::TBoolean
TCIDWebSockThread::bGetFragment(tCIDLib::TCard1&        c1Type
                                , tCIDLib::TBoolean&    bFinal
                                , TMemBuf&              mbufToFill
                                , tCIDLib::TCard4&      c4DataLen)
{
    const tCIDLib::TEncodedTime enctEnd(TTime::enctNowPlusSecs(4));
    c4DataLen = 0;
    try
    {
        //
        //  It has to be at least two bytes. If we get nothing, then return. We
        //  do a tolerant read of one byte first. If we get nothing, then we can just
        //  return and know we didn't get anytying out of sync. We do a very short
        //  wait time, but we do need to do a little, since we are providing the
        //  throttling of the main loop.
        //
        tCIDLib::TCard1 ac1Hdr[2];
        if (!m_pcdsServer->c4ReadBytes(ac1Hdr, 1, TTime::enctNowPlusMSs(100), kCIDLib::False))
            return kCIDLib::False;;

        //
        //  We must get a fragment now, so read the 2nd header byte. From here on
        //  out, we can indicate we must read all requested bytes or an error.
        //
        m_pcdsServer->c4ReadBytes(&ac1Hdr[1], 1, enctEnd, kCIDLib::True);

        //
        //  We check that none of the reserved bits are on and that the payload length
        //  first byte is not greater than 127.
        //
        if (ac1Hdr[0] & 0x70)
        {
            StartShutdown(kCIDWebSock::c2WSockErr_ResrvBits);
            return kCIDLib::False;
        }

        // Get the first length value, masking off the masking bit
        const tCIDLib::TCard1 c1LenByte1(ac1Hdr[1] & 0x7F);

        // Get the type and final flag out
        c1Type = ac1Hdr[0] & 0xF;
        bFinal = (ac1Hdr[0] & 0x80) != 0;

        // Figure out the actual length
        if (c1LenByte1 < 126)
        {
            c4DataLen = c1LenByte1;
        }
         else if (c1LenByte1 == 126)
        {
            // Read two more bytes and combine them
            tCIDLib::TCard1 ac1Len[2];
            m_pcdsServer->c4ReadBytes(ac1Len, 2, enctEnd, kCIDLib::True);

            c4DataLen = ac1Len[0];
            c4DataLen <<= 8;
            c4DataLen |= ac1Len[1];
        }
         else if (c1LenByte1 == 127)
        {
            // Read 8 more bytes and combine them
            tCIDLib::TCard1 ac1Len[8];
            m_pcdsServer->c4ReadBytes(ac1Len, 8, enctEnd, kCIDLib::True);

            for (tCIDLib::TCard4 c4BInd = 0; c4BInd < 8; c4BInd++)
            {
                if (c4BInd)
                    c4DataLen <<= 8;
                c4DataLen |= ac1Len[c4BInd];
            }
        }

        // If larger than our max fragment, then we close the connection.
        if (c4DataLen > kCIDWebSock::c4MaxWebsockFragSz)
        {
            StartShutdown(kCIDWebSock::c2WSockErr_FragTooBig);
            return kCIDLib::False;
        }

        // If the masking bit is on, then read out the mask
        const tCIDLib::TBoolean bMasked = (ac1Hdr[1] & 0x80) != 0;
        tCIDLib::TCard1 ac1Mask[4];
        if (bMasked)
            m_pcdsServer->c4ReadBytes(ac1Mask, 4, enctEnd, kCIDLib::True);

        //
        //  OK, let's read the indicated number of bytes to our fragment buffer,
        //  if there are any.
        //
        if (c4DataLen)
        {
            //
            //  If it's masked, then we have to unmask it, copying it to the caller's
            //  buffer as we go. Else we can read directly into the caller's buffer.
            //
            if (bMasked)
            {
                m_pcdsServer->c4ReadBytes(m_mbufRead, c4DataLen, enctEnd, kCIDLib::True);

                // We run the return length value up as well, as we covert
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DataLen; c4Index++)
                    mbufToFill[c4Index] = m_mbufRead[c4Index] ^ ac1Mask[c4Index % 4];
            }
             else
            {
                mbufToFill.CopyIn(mbufToFill, c4DataLen, 0);
            }
        }

        //
        //  Update the last in message time stamp. It may only be a fragment, but
        //  basically it's just activity from the client.
        //
        m_enctLastInMsg = TTime::enctNow();
    }

    catch(TError& errToCatch)
    {
        // If the source is not connected, we have lost him
        if (!m_pcdsServer->bConnected())
        {
            LogStateInfo(L"Socket connection lost, terminating...");

            if (facCIDWebSock().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            m_eState = tCIDWebSock::EStates::End;
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This is called when it's time to clean up the connection. We call WSTerminate() to
//  let the derived class do whatever it needs, then we do our cleanup.
//
//  NOTE that the data source will get cleaned up by the janitor we put on it in the
//  client servicing method. But it has its own copy of the pointer so we can clean up
//  the pointer here to indicate we are not connected anymore.
//
tCIDLib::TVoid TCIDWebSockThread::Cleanup()
{
    // Let the derived class clean up
    try
    {
        WSTerminate();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    //
    //  Let's shut down the data source and the connection level, give a little time
    //  for the clean shutdown handshaking.
    //
    try
    {
        m_pcdsServer->Terminate(TTime::enctNowPlusMSs(1000), kCIDLib::True);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // If adopting the data source, then destroy it
    if (m_bAdoptSrc)
    {
        try
        {
            delete m_pcdsServer;
        }

        catch(TError& errToCatch)
        {
            if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::DataSrc))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }

    // And now reset our members
    m_bAdoptSrc         = kCIDLib::False;
    m_bSimMode          = kCIDLib::True;
    m_bWaitPong         = kCIDLib::False;
    m_enctEndTimer      = 0;
    m_enctLastInMsg     = 0;
    m_enctLastOutMsg    = 0;
    m_pcdsServer        = nullptr;

    // Make sure this gets set to end, to indicate this one is dead
    m_eState            = tCIDWebSock::EStates::End;
}


//
//  The main loop calls us here when it has a full msg buffered up. We process it.
//  Some we respond to ourself. If it's a text data msg, we dipatch it to the derived
//  clcass.
//
tCIDLib::TVoid
TCIDWebSockThread::HandleMsg(const  tCIDLib::TCard1 c1Type
                            , const THeapBuf&       mbufData
                            , const tCIDLib::TCard4 c4DataCnt)
{
    // Depending on the type
    switch(c1Type)
    {
        case kCIDWebSock::c1WSockMsg_Text :
        {
            //
            //  Transcode it to our string format. Catch any errors since we are
            //  required to fail the connection if so.
            //
            LogStateInfo(L"Dispatching text msg");
            try
            {
                m_tcvtText.c4ConvertFrom(mbufData.pc1Data(), c4DataCnt, m_strTextDispatch);
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);

                LogStateInfo(L"Bad UTF-8 data, failing connection");
                SendClose(kCIDWebSock::c2WSockErr_BadData);
                return;
            }

            try
            {
                // If logging, then spit it out
                if (m_pstrmLog)
                {
                    m_tmLog.SetToNow();
                    *m_pstrmLog << L"----- Receiving (" << m_tmLog << L") -----\n"
                                << m_strTextDispatch << kCIDLib::NewEndLn;
                }

                // Pass it on to the derived class for processing
                WSProcessMsg(m_strTextDispatch);
            }

            catch(TError& errToCatch)
            {
                if (facCIDWebSock().bShouldLog(errToCatch))
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);

                    // Log the info and close the connection
                    facCIDWebSock().LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kWSockErrs::errcWSock_ExceptCB
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , TString(L"ProcesMsg")
                    );
                }
                SendClose(kCIDWebSock::c2WSockErr_HandlerErr);
            }
            break;
        }

        case kCIDWebSock::c1WSockMsg_Bin :
            // We don't process these, so just eat it
            LogStateInfo(L"Ignoring binary msg");
            break;

        case kCIDWebSock::c1WSockMsg_Close :
        {
            //
            //  The client sent a close. If we are not in waitclientend, then it's
            //  the client who started it, so we need to send a response. Otherwise,
            //  it's him responding to our close. Either way we are done.
            //
            tCIDLib::TCard2 c2Code = 0xFFFF;
            if (c4DataCnt >= 2)
            {
                c2Code = mbufData.c2At(0);
                #if defined(CID_LITTLEENDIAN)
                c2Code = tCIDLib::c2SwapBytes(c2Code);
                #endif
            }

            if (m_eState != tCIDWebSock::EStates::WaitClientEnd)
            {
                LogStateInfo2(L"Client closed, sending response. Code=%(1)", TCardinal(c2Code));
                SendClose(0);
            }
             else
            {
                LogStateInfo2(L"Client returned close. Code=%(1)", TCardinal(c2Code));
            }
            m_eState = tCIDWebSock::EStates::End;
            break;
        }

        case kCIDWebSock::c1WSockMsg_Ping :
            // Send back a pong
            LogStateInfo(L"Got ping, sending pong");

            // Send back the exact same data but change the opcode.
            SendMsg(kCIDWebSock::c1WSockMsg_Pong, mbufData, c4DataCnt);
            break;

        case kCIDWebSock::c1WSockMsg_Pong :
            //
            //  It will update the last incomng message time stamp, which is all we
            //  need it to do. If we aren't getting them back, then eventually we will
            //  time out and close the connection.
            //
            //  We also clear our waiting flag if set
            //
            if (m_bWaitPong)
            {
                m_bWaitPong = kCIDLib::False;
                LogStateInfo(L"Got expected pong");

                // See if the data is the same as what we would have sent
                if ((c4DataCnt != 4) || (mbufData.c4At(0) != m_c4PingVal))
                    LogStateInfo(L"Did not get expected ping data back");
            }
             else
            {
                LogStateInfo(L"Got unexpected pong");
            }
            break;

        default :
            break;
    };
}



// We send out a close message with the passed code.
tCIDLib::TVoid TCIDWebSockThread::SendClose(const tCIDLib::TCard2 c2Code)
{
    LogStateInfo2(L"Sending close message. Code=%(1)", TCardinal(c2Code));
    SendMsg(kCIDWebSock::c1WSockMsg_Close, &c2Code, 2);
}


//
//  If it's been over a period of tiem since any incoming message, we send a ping to
//  the client. We bump the ping value and send that as the data. We set the flag to
//  remember we are waiting.
//
tCIDLib::TVoid TCIDWebSockThread::SendPing()
{
    LogStateInfo(L"Sending ping");

    m_c4PingVal++;
    SendMsg(kCIDWebSock::c1WSockMsg_Ping, &m_c4PingVal, 4);
    m_bWaitPong = kCIDLib::True;
}
