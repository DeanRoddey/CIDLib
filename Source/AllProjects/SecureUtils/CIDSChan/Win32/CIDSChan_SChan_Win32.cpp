//
// FILE NAME: CIDCrypto_SChan.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the Win32 version of the secure channel class. We handle some
//  of the methods of the class here, the ones that deal with platform specific issues.
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
#include    "CIDSChan_.hpp"
#define     SECURITY_WIN32
#include    <Windows.h>
#include    <Sspi.h>
#include    <Schnlsp.h>

#pragma     comment(lib, "crypt32")
#pragma     comment(lib, "Secur32")


// ---------------------------------------------------------------------------
//  Each per-platform implementation creates their own version of this structure
//  to hold there internal info in.
//
//  The certificate context is only used on the server side.
//
//  Some of them we need a flag to remember if we got them allocated so that
//  we know if we need to clean them up.
//
//  We also need to remember if we have already seen the close from the other
//  side. If so, we don't do the shutdown exchange on cleanup because we already
//  did it in response to the expired indicator.
// ---------------------------------------------------------------------------
struct TSChanPlatData
{
    CredHandle          CredH;
    CtxtHandle          CtxH;
    SECURITY_INTEGER    CertTimeout;
    PCCERT_CONTEXT      pCertContext;

    tCIDLib::TBoolean   bGotCredHandle;
    tCIDLib::TBoolean   bGotSecContext;
    tCIDLib::TBoolean   bOtherSideClosed;

    tCIDLib::TCard4     c4MaxHeader;
    tCIDLib::TCard4     c4MaxMsg;
    tCIDLib::TCard4     c4MaxTrailer;

    tCIDLib::TCard4     c4PacketSize;
    tCIDLib::TCard1*    pc1Packet;
};


// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------

// Called to load up an ALPN buffer
static tCIDLib::TCard4
c4LoadALPNBuf(          tCIDLib::TCard1*    pc1Out
                , const tCIDLib::TCard4     c4BufSz
                , const tCIDLib::TStrList&  colNames)
{
    //
    //  Set up an output stream to build up the info temporarily into. Default little endian
    //  is correct.
    //
    tCIDLib::TCard4 c4Ret = 0;
    THeapBuf mbufOutput(c4BufSz, c4BufSz);
    {
        TBinMBufOutStream strmTar(&mbufOutput);

        // First put out a zero value that we'll fill in later with the buffer size
        strmTar << tCIDLib::TCard4(0);

        // Then we indicate that it has ALPN data
        strmTar << tCIDLib::TCard4(SecApplicationProtocolNegotiationExt_ALPN);

        //
        //  And another one we'll come back to which contains the bytes for the actual
        //  protocol names, only 16 bits this time.
        //
        strmTar << tCIDLib::TCard2(0);

        //
        //  Then for each entry, we have to transcode to UTF8, then put out the number of
        //  bytes and then the bytes.
        //
        tCIDLib::TCard4 c4CurBytes;
        TUTF8Converter tcvtUTF8;
        THeapBuf mbufUTF8(64, c4BufSz - 20);
        tCIDLib::TStrList::TCursor cursNames(&colNames);
        for (; cursNames; ++cursNames)
        {
            // Put out the converted name bytes, followed by the bytes themselves
            tcvtUTF8.c4ConvertTo(*cursNames, mbufUTF8, c4CurBytes);
            strmTar << tCIDLib::TCard1(c4CurBytes);
            strmTar.c4WriteBuffer(mbufUTF8, c4CurBytes);
        }
        strmTar.Flush();
        c4Ret = strmTar.c4CurSize();
    }
    CIDAssert(c4Ret <= c4BufSz, L"The ALPN data is larger than the available buffer");

    // Put in the overall buffer size we skipped, minus the size bytes themselves
    mbufOutput.PutCard4(c4Ret - 4, 0);

    //
    //  Put the bytes for the actual protocols info, which is the full size minus
    //  the two 4 byte and single 2 byte housekeeping values.
    //
    mbufOutput.PutCard2(tCIDLib::TCard2(c4Ret - 10), 8);

    // And copy this back to the caller's buffer
    mbufOutput.CopyOut(pc1Out, c4Ret);
    return c4Ret;
}


//
//  Maps some of the common errors to errors of our own, wo that we can provide more
//  useful info for in the field diagnosis.
//
//  NOTE YET used for whatever reason. We need to look into implementing this.
//
static tCIDLib::TErrCode errcMapSChanStatusToErr(const tCIDLib::TCard4 Status)
{
    tCIDLib::TErrCode errcRet = 0;
    switch(Status)
    {
        case SEC_E_BAD_BINDINGS :
            errcRet = kSChanErrs::errcSChan_BadBindings;
            break;

        case SEC_E_INTERNAL_ERROR :
            errcRet = kSChanErrs::errcSChan_Internal;
            break;

        case SEC_E_INVALID_HANDLE :
            errcRet = kSChanErrs::errcSChan_Handle;
            break;

        case SEC_E_INVALID_TOKEN :
            errcRet = kSChanErrs::errcSChan_Token;
            break;

        case SEC_E_NO_AUTHENTICATING_AUTHORITY :
            errcRet = kSChanErrs::errcSChan_NoAuth;
            break;

        case SEC_E_NO_CREDENTIALS :
            errcRet = kSChanErrs::errcSChan_NoCreds;
            break;

        case SEC_E_UNSUPPORTED_FUNCTION :
            errcRet = kSChanErrs::errcSChan_FuncUnsupp;
            break;


        default :
            // Just return the generic connection failed error
            errcRet = kSChanErrs::errcSChan_ConnFailed;
            break;
    };

    return errcRet;
}




// ---------------------------------------------------------------------------
//   CLASS: TSChannel
//  PREFIX: schan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSChannel: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to terminate the connection if it is not already terminated and
//  both our side and the other side have not closed. and then to clean up any
//  per-platform resources.
//
tCIDLib::TVoid
TSChannel::Terminate(TCIDDataSrc& cdsTar, const tCIDLib::TEncodedTime enctEnd)
{
    // If we don't have data, do nothing
    if (!m_pInfo)
        return;

    //
    //  If we didn't already do the disconnect due to the other side closing, and the
    //  data source still appears connected, then do the disconnect from our side.
    //
    if (!m_pInfo->bOtherSideClosed && cdsTar.bConnected())
    {
        try
        {
            DoDisconnect(cdsTar);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    // Clean up the resources stored in our internal data
    Cleanup();
}




// ---------------------------------------------------------------------------
//  TSChannel: Private, non-virtal methods
// ---------------------------------------------------------------------------

//
//  We can read into a raw buffer or memory buffer, using either a milliseconds wait
//  time or an end time.
//
tCIDLib::TCard4
TSChannel::c4ReceiveData(       TCIDDataSrc&            cdsTar
                        ,       tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4ReqBytes
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::EAllData       eAllData)
{
    // If we don't have data, then this is not legal
    if (!m_pInfo)
    {
        facCIDSChan().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSChanErrs::errcSChan_NotInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // For less verbiage get the all data into a flag
    const tCIDLib::TBoolean bAllData(eAllData == tCIDLib::EAllData::FailIfNotAll);

    //
    //  See if any data is in our decrypted buffer. If so, and it's enough to handle
    //  the whole request, then do that now.
    //
    if (m_c4DecBufSz && (!bAllData || (m_c4DecBufSz >= c4ReqBytes)))
    {
        //
        //  See how many bytes we can do. It's the smaller of the requested bytes or
        //  the amount available.
        //
        const tCIDLib::TCard4 c4ToCopy = tCIDLib::MinVal(c4ReqBytes, m_c4DecBufSz);

        m_mbufDecBuf.CopyOut(pc1ToFill, c4ToCopy);
        m_c4DecBufSz -= c4ToCopy;

        //
        //  If there's any data left in the buffer, move it down
        //
        if (m_c4DecBufSz)
            m_mbufDecBuf.MoveToStart(c4ToCopy, m_c4DecBufSz);

        // And return since we know in this block that we are satisfying the requirement
        return c4ToCopy;
    }

    SecBuffer InBuffers[4];
    SecBufferDesc InBufList;
    InBufList.cBuffers = 4;
    InBufList.pBuffers = InBuffers;
    InBufList.ulVersion = SECBUFFER_VERSION;

    //
    //  We use an extension mechanism to handle timing out right in the middle of a
    //  packet that we haven't fully received yet. It's one shot deal.
    //
    tCIDLib::TBoolean       bExtended = kCIDLib::False;
    tCIDLib::TEncodedTime   enctRealEnd = enctEnd;

    //
    //  We have to loop until we get either all of the data the caller asked for,
    //  or what's current available. We also have to deal with not having enough
    //  data for a whole block to decrypt as well.
    //
    tCIDLib::TBoolean   bDone = kCIDLib::False;
    tCIDLib::TCard4     c4CarryOver = 0;
    tCIDLib::TCard4     c4Cnt = 0;
    TThread*            pthrCaller = nullptr;
    SECURITY_STATUS     Status = SEC_E_OK;
    while (!bDone)
    {
        //
        //  Read some data. We say not all has to be read, because the data we are
        //  reading isn't the actual data. We are just reading chunks at a time and
        //  decrypting real data out of it.
        //
        //  If we had data left over from a previous round, we don't try to read
        //  more, we just process that, to avoid blocking for data that may not be
        //  available. If it turns out this isn't enough for a full block, then we
        //  will get an incomplete message and will come back around with cnt zero
        //  and the data in the carryover so we just read more onto the end.
        //
        if (!c4Cnt)
        {
            c4Cnt = cdsTar.c4ReadBytes
            (
                &m_pInfo->pc1Packet[c4CarryOver]
                , m_pInfo->c4PacketSize - c4CarryOver
                , enctRealEnd
                , kCIDLib::False
            );
        }

        //
        //  If we got zero bytes, we waited until the end time, and nothing showed up
        //  this time. This means one of two things
        //
        //  1.  We are waiting for a packet and haven't gotten a whole one yet. If
        //      so this means we've gotten a partial packet which is bad. So, if we
        //      have more than zero bytes for this round, that's the deal. We'll check
        //      below for partial msg state and throw, so we just break out, assuming
        //      we don't break out just for a failure to read enough data.
        //
        //      Since these are encrypted packets, we can't just read a partial one. We
        //      have to get a whole one or something is badly wrong.
        //
        //  2.  We haven't gotten any bytes this time around. So, either we break out
        //      with what we have. Or, if they indicated we had to read all requested
        //      bytes and we haven't, then we throw an exception.
        //
        //  In order to deal with the possibility of a timeout mid-packet of a packet
        //  that is being delivered in good faith, we use an extension mechanism. If we
        //  haven't done so already, and we are in incomplete msg state, and we have
        //  some bytes so far, we'll extend the end time a bit and keep going. This is
        //  a one time thing.
        //
        if (!c4Cnt)
        {
            if ((Status == SEC_E_INCOMPLETE_MESSAGE) && (c4CarryOver != 0) && !bExtended)
            {
                bExtended = kCIDLib::True;
                enctRealEnd += kCIDLib::enctOneMilliSec * 250;
                continue;
            }

            // No extension available, so face the music
            if (bAllData && (m_c4DecBufSz < c4ReqBytes))
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_NotAllData
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotAllRead
                    , TCardinal(c4ReqBytes)
                    , TCardinal(m_c4DecBufSz)
                    , m_strName
                );
            }
            break;
        }

        // Add in the carryover now and clear the carryover flag
        c4Cnt += c4CarryOver;
        c4CarryOver = 0;

        // Set up our input buffers
        InBuffers[0].pvBuffer   = m_pInfo->pc1Packet;
        InBuffers[0].cbBuffer   = c4Cnt;
        InBuffers[0].BufferType = SECBUFFER_DATA;

        InBuffers[1].BufferType = SECBUFFER_EMPTY;
        InBuffers[1].pvBuffer   = NULL;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[2].BufferType = SECBUFFER_EMPTY;
        InBuffers[2].pvBuffer   = NULL;
        InBuffers[2].cbBuffer   = 0;
        InBuffers[3].BufferType = SECBUFFER_EMPTY;
        InBuffers[3].pvBuffer   = NULL;
        InBuffers[3].cbBuffer   = 0;

        // And let's try a decrypt
        Status = ::DecryptMessage(&m_pInfo->CtxH, &InBufList, 0, NULL);

        //
        //  If an incomplete message, then set it as carryover and go back to the top.
        //  Zero the count since we are making it now carryover.
        //
        if (Status == SEC_E_INCOMPLETE_MESSAGE)
        {
            c4CarryOver = c4Cnt;
            c4Cnt = 0;
            continue;
        }

        //
        //  Check for errors that we won't recover from but that aren't static types,
        //  i.e. driven by communications errors. Others are handled below after
        //  processing any data that might still have been returned.
        //
        if (Status == SEC_E_OUT_OF_SEQUENCE)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_OutOfSeq
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Protocol
                , m_strName
            );
        }
         else if (Status == SEC_E_MESSAGE_ALTERED)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_MsgAltered
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Protocol
                , m_strName
            );
        }

        // Set up convenience pointers to the two buffers of interest
        SecBuffer* pDataBuf = nullptr;
        SecBuffer* pExtraBuf = nullptr;
        if ((InBuffers[1].BufferType == SECBUFFER_DATA) && InBuffers[1].cbBuffer)
            pDataBuf = &InBuffers[1];
        if ((InBuffers[3].BufferType == SECBUFFER_EXTRA) && InBuffers[3].cbBuffer)
            pExtraBuf = &InBuffers[3];

        //
        //  If we got a any output data, copy it to our decoded buffer for storage
        //  until we have enough.
        //
        if (pDataBuf)
        {
            m_mbufDecBuf.CopyIn(pDataBuf->pvBuffer, pDataBuf->cbBuffer, m_c4DecBufSz);
            m_c4DecBufSz += pDataBuf->cbBuffer;
        }

        //
        //  Remember if we are done or not. We break out on two basic scenarios:
        //
        //  1. We have all the bytes requested
        //  2. We don't have to read all bytes, and we have some data though not all.
        //  3. There's no extra buffer left to process
        //
        //  If not breaking out, then any extra data will be copyed down and processed
        //  again.
        //
        if (((m_c4DecBufSz >= c4ReqBytes) || (m_c4DecBufSz && !bAllData)) && !pExtraBuf)
        {
            bDone = kCIDLib::True;
        }

        //
        //  If any extra data, we have to copy it down to the start of the data buffer
        //  and go back around. Even if we have enough data already we do this so that
        //  we don't lose any data.
        //
        if (pExtraBuf)
        {
            CIDAssert(pExtraBuf->cbBuffer <= m_pInfo->c4PacketSize, L"Extra buffer too larger for packet buf");

            //
            //  Copy these bytes to the input buffer and go again. We keep it as
            //  byte count, so that we don't try to read again at the top. This
            //  could be enough to fulfill our requirements. If it's not, then
            //  we will just keep going and will try to read more eventually.
            //
            TRawMem::CopyMemBuf(m_pInfo->pc1Packet, pExtraBuf->pvBuffer, pExtraBuf->cbBuffer);
            c4Cnt = pExtraBuf->cbBuffer;

        }
         else
        {
            // No data to carry around to the next time
            c4Cnt = 0;
        }

        //
        //  If the other side closed, then send our side of it. Then break out with
        //  what we have. If we return something this time, next time we won't get
        //  anything and will return zero which will end the connection.
        //
        if (Status == SEC_I_CONTEXT_EXPIRED)
        {
            DoDisconnect(cdsTar);

            // Remember we already did the disconnect
            m_pInfo->bOtherSideClosed = kCIDLib::True;
            break;
        }

        //
        //  If asked to, then renegotiate and then continue. We have to save this to
        //  the end becase we have to deal with any existing returned data first. If
        //  this fails, we will just break out and return zero to end the connection.
        //
        if (Status == SEC_I_RENEGOTIATE)
        {
            try
            {
                if (m_bClient)
                    ClNegotiate(cdsTar, enctRealEnd);
                else
                    SrvNegotiate(cdsTar, enctRealEnd);
            }

            catch(TError& errToCatch)
            {
                if (facCIDSChan().bShouldLog(errToCatch))
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }

                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_Renegotiate
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppStatus
                    , m_strName
                );

                // Break out with zero bytes to drop this connection
                m_c4DecBufSz = 0;
                break;
            }
        }

        if (!bDone)
        {
            if (!pthrCaller)
                pthrCaller = TThread::pthrCaller();

            if (pthrCaller->bCheckShutdownRequest())
            {
                // Break out with zero bytes if asked to shut down
                m_c4DecBufSz = 0;
                break;
            }
        }
    }

    // If we ended up with a partial packet, then give up since we are doomed
    if (Status == SEC_E_INCOMPLETE_MESSAGE)
    {
        facCIDSChan().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSChanErrs::errcSChan_PartialMsg
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutOfSync
            , m_strName
        );
    }

    //
    //  Copy over the data. We copy the smaller of the available data in the decode
    //  buffer or the amount requested.
    //
    const tCIDLib::TCard4 c4RetVal = tCIDLib::MinVal(m_c4DecBufSz, c4ReqBytes);
    if (c4RetVal)
    {
        m_mbufDecBuf.CopyOut(pc1ToFill, c4RetVal);
        m_c4DecBufSz -= c4RetVal;

        // Move down the remainder of the decoded buffer if any is left
        if (m_c4DecBufSz)
            m_mbufDecBuf.MoveToStart(c4RetVal, m_c4DecBufSz);
    }
    return c4RetVal;
}


//
//  Handles the basic cleanup of our session data. We don't do the shutdown handshake
//  stuff here, just the cleanup.
//
tCIDLib::TVoid TSChannel::Cleanup()
{
    if (m_pInfo)
    {
        // Free the context info
        if (m_pInfo->bGotSecContext)
            ::DeleteSecurityContext(&m_pInfo->CtxH);

        // Release the security handle if set
        if (m_pInfo->bGotCredHandle)
            ::FreeCredentialsHandle(&m_pInfo->CredH);

        // If we created a certificate context, free that
        if (m_pInfo->pCertContext)
            ::CertFreeCertificateContext(m_pInfo->pCertContext);

        delete m_pInfo->pc1Packet;
        m_pInfo->pc1Packet = nullptr;

        // Delete the info and clear the pointer
        delete m_pInfo;
        m_pInfo = nullptr;
    }

    // Make sure any of our connection oriented stuff is cleared
    m_c4DecBufSz = 0;
    m_strPrincipal.Clear();
    m_strName.Clear();
}


//
//  This method handles the client side of the connection negotiation. It's pretty
//  similar to the server side, but there's no certificate involved (we don't currently
//  support client side validation.)
//
//  We assume the caller will catch exceptions and clean up appropriate for his needs.
//
tCIDLib::TVoid
TSChannel::ClNegotiate(         TCIDDataSrc&            cdsTar
                        , const tCIDLib::TEncodedTime   enctEnd)
{
    tCIDLib::TBoolean   bDone = kCIDLib::False;
    SECURITY_STATUS     Status;

    // Get a pointer to our internal data structure
    CIDAssert(m_pInfo != nullptr, L"The secure channel internal data is not allocated");

    // Get the token size info for schanne
    tCIDLib::TCard4 c4TokenSize = 0;
    {
        PSecPkgInfoW pkgInfo;
        Status = ::QuerySecurityPackageInfoW(L"schannel", &pkgInfo);
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_GetPkgInfo
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TCardinal(Status, tCIDLib::ERadices::Hex)
            );
        }

        c4TokenSize = pkgInfo->cbMaxToken;
        ::FreeContextBuffer(pkgInfo);
    }

    // Allocate temp buffers of the max token size
    tCIDLib::TCard1* pc1In = new tCIDLib::TCard1[c4TokenSize];
    TArrayJanitor<tCIDLib::TCard1> janIn(pc1In);

    tCIDLib::TCard1* pc1Out = new tCIDLib::TCard1[c4TokenSize];
    TArrayJanitor<tCIDLib::TCard1> janOut(pc1Out);

    // Set up our I/O buffer lists
    SecBuffer InBuffers[2];
    SecBufferDesc InBufList;
    InBufList.cBuffers = 2;
    InBufList.pBuffers = InBuffers;
    InBufList.ulVersion = SECBUFFER_VERSION;

    SecBuffer OutBuffers[1];
    SecBufferDesc OutBufList;
    OutBufList.cBuffers = 1;
    OutBufList.pBuffers = OutBuffers;
    OutBufList.ulVersion = SECBUFFER_VERSION;

    // The flags we want to use
    const tCIDLib::TCard4 c4InFlags
    (
        ISC_REQ_STREAM
        | ISC_REQ_CONFIDENTIALITY
        | ISC_REQ_REPLAY_DETECT
    );

    // And now loop until we are done or get an error
    tCIDLib::TBoolean   bFirstRound = kCIDLib::True;
    tCIDLib::TCard4     c4CarryOver = 0;
    tCIDLib::TCard4     c4OutFlags;
    tCIDLib::TCard4     c4Cnt = 0;
    TThread*            pthrCaller = TThread::pthrCaller();
    TimeStamp           ExpTime;
    do
    {
        // Read some data more data if not the first round
        if (!bFirstRound)
        {
            // We add this to any carryover from a previous round
            c4Cnt = cdsTar.c4ReadBytes
            (
                &pc1In[c4CarryOver], c4TokenSize - c4CarryOver, enctEnd, kCIDLib::False
            ) + c4CarryOver;

            CIDAssert(c4Cnt <= c4TokenSize, L"Token was bigger than indicated size");

            //
            //  If zero bytes, then something is wrong. It would have waited until
            //  the end of the available time to get some data.
            //
            if (!c4Cnt)
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_NoData
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Timeout
                    , m_strName
                );
            }
        }

        // We've consumed any carryover from the previous round now
        c4CarryOver = 0;

        // Set up our buffers for this round
        InBuffers[0].pvBuffer   = pc1In;
        InBuffers[0].cbBuffer   = c4Cnt;
        InBuffers[0].BufferType = SECBUFFER_EMPTY;
        if (bFirstRound && !m_colALPNList.bIsEmpty())
        {
            // It's the first around and we have ALPN info so start with that
            c4Cnt = c4LoadALPNBuf(pc1In, c4TokenSize, m_colALPNList);
            if (c4Cnt)
            {
                InBuffers[0].cbBuffer = c4Cnt;
                InBuffers[0].BufferType = SECBUFFER_APPLICATION_PROTOCOLS;
            }
        }
         else if (c4Cnt)
        {
            // We just have token info to send, so it's a token buffer
            InBuffers[0].BufferType = SECBUFFER_TOKEN;
        }


        InBuffers[1].pvBuffer   = nullptr;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        OutBuffers[0].pvBuffer  = pc1Out;
        OutBuffers[0].cbBuffer  = c4TokenSize;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;

        // And do a round
        Status = ::InitializeSecurityContextW
        (
            &m_pInfo->CredH
            , m_pInfo->bGotSecContext ? &m_pInfo->CtxH : 0
            , (SEC_WCHAR*)m_strPrincipal.pszBuffer()
            , c4InFlags
            , 0
            , 0
            , &InBufList
            , 0
            , &m_pInfo->CtxH
            , &OutBufList
            , &c4OutFlags
            , &ExpTime
        );

        // Clear the first round flag now that we've made the first call
        bFirstRound = kCIDLib::False;

        // And remember that we now have the context handle for subsequent rounds
        m_pInfo->bGotSecContext = kCIDLib::True;

        // If an incomplete message, we have to read more data
        if (Status == SEC_E_INCOMPLETE_MESSAGE)
        {
            //
            //  Remember the current count as the carry over, and go back to
            //  the top. We'll try to read in more data past what we have already
            //  in the buffer.
            //
            c4CarryOver = c4Cnt;
            continue;
        }

        //
        //  If we need to complete a token, then do that. This updates the data in
        //  our output buffer, so we have do this before we do any sending of data
        //
        if ((Status == SEC_I_COMPLETE_AND_CONTINUE)
        ||  (Status == SEC_I_COMPLETE_NEEDED))
        {
            Status = ::CompleteAuthToken(&m_pInfo->CtxH, &OutBufList);
            if (Status < 0)
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_CompAuthToken
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , m_strName
                    , TCardinal(Status, tCIDLib::ERadices::Hex)
                );
            }

            //
            //  If complete and continue, set the status to continue for below.
            //  Else, set it to OK, to indicate we are done.
            //
            if (Status == SEC_I_COMPLETE_AND_CONTINUE)
                Status = SEC_I_CONTINUE_NEEDED;
            else
                Status = SEC_E_OK;
        }

        // If there's data to send, let's send it
        if (((Status == SEC_E_OK) || (Status == SEC_I_CONTINUE_NEEDED))
        &&  (OutBuffers[0].cbBuffer && OutBuffers[0].pvBuffer))
        {
            cdsTar.WriteRawBytes
            (
                reinterpret_cast<tCIDLib::TCard1*>(OutBuffers[0].pvBuffer)
                , OutBuffers[0].cbBuffer
            );
            cdsTar.FlushOut(enctEnd);
        }

        // If less than zero at this point and we failed
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_AcceptSecCtx
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TCardinal(Status, tCIDLib::ERadices::Hex)
            );
        }

        // The server asked for a certificate, which we don't have
        if (Status == SEC_I_INCOMPLETE_CREDENTIALS)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_NoClientCert
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }

        //
        //  If the result is OK, then we are done. If there's an extra buffer, we have
        //  to push it back into the source data source because it's data that needs
        //  to be decrypted.
        //
        if (Status == SEC_E_OK)
        {
            if ((InBuffers[1].BufferType == SECBUFFER_EXTRA) && InBuffers[1].cbBuffer)
            {
                cdsTar.PushBack
                (
                    pc1In + (c4Cnt - InBuffers[1].cbBuffer)
                    , InBuffers[1].cbBuffer
                );
            }
            break;
        }

        // Watch for shutdown requests
        if (pthrCaller->bCheckShutdownRequest())
        {
            // Get us out with an exception
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_ShutdownReq
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , m_strName
            );
        }

        //
        //  If there's extra data at this point, we have to move it down to the
        //  start of the buffer and read some more after it.
        //
        if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
        {
            c4CarryOver = InBuffers[1].cbBuffer;
            tCIDLib::TCard1* pc1Src = pc1In + (c4Cnt - c4CarryOver);

            tCIDLib::TCard1* pc1Tar = pc1In;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CarryOver; c4Index++)
            {
                *pc1Tar = *pc1Src;
                pc1Tar++;
                pc1Src++;
            }
        }

    }   while (!bDone);

    // Allocate our packet buffer, deleting any previous first.
    delete [] m_pInfo->pc1Packet;
    m_pInfo->pc1Packet = nullptr;

    {
        SecPkgContext_StreamSizes StrmSzs;
        Status = ::QueryContextAttributes(&m_pInfo->CtxH, SECPKG_ATTR_STREAM_SIZES, &StrmSzs);

        m_pInfo->c4MaxHeader = StrmSzs.cbHeader;
        m_pInfo->c4MaxMsg = StrmSzs.cbMaximumMessage;
        m_pInfo->c4MaxTrailer = StrmSzs.cbTrailer;

        m_pInfo->c4PacketSize = m_pInfo->c4MaxHeader + m_pInfo->c4MaxMsg + m_pInfo->c4MaxTrailer;
    }

    m_pInfo->pc1Packet = new tCIDLib::TCard1[m_pInfo->c4PacketSize];
}


//
//  We have two different public connect methods, one for the server and one for the
//  client. They take different info. They will store their info away, along with the
//  m_bClient flag, and then call us to to the actual work.
//
tCIDLib::TVoid
TSChannel::DoConnect(       TCIDDataSrc&            cdsTar
                    , const TString&                strCertInfo
                    , const tCIDLib::TEncodedTime   enctEnd)
{
    //
    //  Create our internal info structure. Store the pointer so that clean up
    //  can be done if we throw. The info structure will be released and the
    //  pointer zeroed if that happens.
    //
    m_pInfo = new TSChanPlatData{0};

    tCIDLib::TBoolean bRet = kCIDLib::False;
    HCERTSTORE hMyCertStore = 0;
    try
    {
        if (strCertInfo.bIsEmpty())
        {
            // If server side, then that's an error
            if (!m_bClient)
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_NoServerCert
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , m_strName
                );
            }
        }
         else
        {
            PCCERT_CONTEXT pCertContext = nullptr;

            //
            //  Parse the certificate info. The caller an indicate different ways for
            //  us to load it. The facility class provides a helper to do the parsing.
            //
            TString  strCertType;
            TString  strCertStore;
            TString  strCertName;
            if (!facCIDSChan().bParseCertInfo(strCertInfo, strCertType, strCertStore, strCertName))
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_BadCertInfo
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , m_strName
                    , strCertInfo
                );
            }

            // Try to create a certificate based on the info provided
            if ((strCertType == L"mstore") || (strCertType == L"ustore"))
            {
                // Set up the right physical store
                DWORD dwPStore;
                if (strCertType == L"mstore")
                    dwPStore = CERT_SYSTEM_STORE_LOCAL_MACHINE;
                else
                    dwPStore = CERT_SYSTEM_STORE_CURRENT_USER;

                hMyCertStore = ::CertOpenStore
                (
                    CERT_STORE_PROV_SYSTEM_W
                    , 0
                    , 0
                    , dwPStore
                    , strCertStore.pszBuffer()
                );

                if (hMyCertStore == NULL)
                {
                    // Can't go any further
                    facCIDSChan().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSChanErrs::errcSChan_OpenStore
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                        , m_strName
                    );
                }

                // That worked so let's look for the caller's certificate
                pCertContext = ::CertFindCertificateInStore
                (
                    hMyCertStore
                    , X509_ASN_ENCODING | PKCS_7_ASN_ENCODING
                    , 0
                    , CERT_FIND_SUBJECT_STR_W
                    , strCertName.pszBuffer()
                    , NULL
                );
            }
             else if (strCertType == L"file")
            {
                //
                //  Try to open the file and create a certificate from that. The
                //  'store' in this case is the file path.
                //
                if (TFileSys::bExists(strCertStore))
                {
                    TBinaryFile bflCert(strCertStore);
                    bflCert.Open
                    (
                        tCIDLib::EAccessModes::Read
                        , tCIDLib::ECreateActs::OpenIfExists
                        , tCIDLib::EFilePerms::Default
                        , tCIDLib::EFileFlags::SequentialScan
                    );

                    THeapBuf mbufCert(8192UL);
                    tCIDLib::TCard4 c4Bytes = bflCert.c4ReadBuffer
                    (
                        mbufCert
                        , tCIDLib::TCard4(bflCert.c8CurSize())
                        , tCIDLib::EAllData::FailIfNotAll
                    );

                    pCertContext = ::CertCreateCertificateContext
                    (
                        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, mbufCert.pc1Data(), c4Bytes
                    );
                }
            }
             else
            {
                facCIDSChan().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_CertType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , strCertType
                    , m_strName
                );
            }

            // If we never got one open
            if (pCertContext == NULL)
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_OpenCert
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , m_strName
                    , strCertInfo
                );
            }

            // It worked, so store the context
            m_pInfo->pCertContext = pCertContext;
        }

        // Reset some stuff used to track data
        m_c4DecBufSz = 0;

        // Set up the credentials stuff we want
        SCHANNEL_CRED SCCred = { 0 };
        SCCred.dwVersion = SCHANNEL_CRED_VERSION;
        if (m_bClient)
        {
            SCCred.grbitEnabledProtocols =  SP_PROT_TLS1_1_CLIENT
                                            | SP_PROT_TLS1_2_CLIENT;
            SCCred.dwFlags = SCH_CRED_NO_DEFAULT_CREDS;
        }
         else
        {
            SCCred.cCreds = 1;
            SCCred.paCred = &m_pInfo->pCertContext;
            SCCred.grbitEnabledProtocols =  SP_PROT_TLS1_1_SERVER
                                            | SP_PROT_TLS1_2_SERVER;
        }

        // And try to aquire a credentials handle
        SECURITY_STATUS Status = ::AcquireCredentialsHandle
        (
            NULL
            , UNISP_NAME
            , m_bClient ? SECPKG_CRED_OUTBOUND : SECPKG_CRED_INBOUND
            , NULL
            , &SCCred
            , NULL
            , NULL
            , &m_pInfo->CredH
            , &m_pInfo->CertTimeout
        );

        // If we failed,then we are done
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_AqcCreds
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TInteger(Status)
            );
        }

        // Remember we got the credentials handle
        m_pInfo->bGotCredHandle = kCIDLib::True;

        //
        //  Call a helper that does the negotiation. It needs to be separate because
        //  we have to call this also for renegotiation.
        //
        if (m_bClient)
        {
            ClNegotiate(cdsTar, enctEnd);
        }
         else
        {
            SrvNegotiate(cdsTar, enctEnd);
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        // Clean up any local stuff
        if (hMyCertStore)
            ::CertCloseStore(hMyCertStore, 0);

        // And any of the info stuff that got created and rethrow
        Cleanup();
        throw;
    }
}



//
//  Does a formal disconnect process. We won't wait for long, because we don't want to
//  hold up shutdown. This is called during the termination process.
//
tCIDLib::TVoid TSChannel::DoDisconnect(TCIDDataSrc& cdsTar)
{
    // Shouldn't get called if not, but just in case...
    if (!m_pInfo)
        return;

    // If we never got the security context, nothing to do here.
    if (!m_pInfo->bGotSecContext)
        return;

    DWORD dwShutdown = SCHANNEL_SHUTDOWN;
    SECURITY_STATUS Status;

    // Set up our buffers
    SecBuffer  OutBufs[1];
    OutBufs[0].BufferType = SECBUFFER_TOKEN;
    OutBufs[0].cbBuffer = sizeof(dwShutdown);
    OutBufs[0].pvBuffer = &dwShutdown;
    SecBufferDesc   OutBuffer;
    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBufs;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    // Update the context to start the shutdown process.
    Status = ::ApplyControlToken(&m_pInfo->CtxH, &OutBuffer);
    if (Status < 0)
        return;

    // Now update the buffers for the the exchange
    OutBufs[0].BufferType = SECBUFFER_TOKEN;
    OutBufs[0].cbBuffer = 0;
    OutBufs[0].pvBuffer = nullptr;

    TimeStamp Expire;
    DWORD dwOutFlags;
    Status = ::AcceptSecurityContext
    (
        &m_pInfo->CredH
        , &m_pInfo->CtxH
        , NULL
        , ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM
        , 0
        , NULL
        , &OutBuffer
        , &dwOutFlags
        , &Expire
    );

    if ((Status == SEC_E_OK) && OutBufs[0].pvBuffer && OutBufs[0].cbBuffer)
    {
        try
        {
            cdsTar.WriteRawBytes
            (
                reinterpret_cast<tCIDLib::TCard1*>(OutBufs[0].pvBuffer), OutBufs[0].cbBuffer
            );
            cdsTar.FlushOut(TTime::enctNowPlusSecs(3));
        }

        catch(...)
        {
            // It's not unusual to get an error here if the other side already dropped
        }

        ::FreeContextBuffer(OutBufs[0].pvBuffer);
    }
}


//
//  Does the server side of the connection negotiation. It's fairly similar to the
//  client side, but we are receiving info initially and reacting to it, so we accept
//  token info.
//
//  We also have to handle loading a certificate to pass to the client, but that will
//  have already been done, because this is also called for a renegotiate, and the
//  certificate doesn't need to be reloaded.
//
//  We assume the caller will catch exceptions and clean up appropriate for his needs.
//
tCIDLib::TVoid
TSChannel::SrvNegotiate(TCIDDataSrc& cdsTar, const tCIDLib::TEncodedTime enctEnd)
{
    SECURITY_STATUS Status;

    // Get a pointer to our internal data structure
    CIDAssert(m_pInfo != nullptr, L"The secure channel internal data is not allocated");

    // Get the token size info for schanne
    tCIDLib::TCard4 c4TokenSize = 0;
    {
        PSecPkgInfoW pkgInfo;
        Status = ::QuerySecurityPackageInfoW(L"schannel", &pkgInfo);
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_GetPkgInfo
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TCardinal(Status, tCIDLib::ERadices::Hex)
            );
        }

        c4TokenSize = pkgInfo->cbMaxToken;
        ::FreeContextBuffer(pkgInfo);
    }

    // Allocate temp buffers of the max token size
    tCIDLib::TCard1* pc1In = new tCIDLib::TCard1[c4TokenSize];
    TArrayJanitor<tCIDLib::TCard1> janIn(pc1In);

    tCIDLib::TCard1* pc1Out = new tCIDLib::TCard1[c4TokenSize];
    TArrayJanitor<tCIDLib::TCard1> janOut(pc1Out);

    //
    //  Else, let's attempt to connect. So set up the buffers we need to do
    //  the work. The main input buffer we point to our memory buffer member,
    //  since that's where we accumulate it.
    //
    SecBuffer InBuffers[2];
    SecBufferDesc InBufList;
    InBufList.cBuffers = 2;
    InBufList.pBuffers = InBuffers;
    InBufList.ulVersion = SECBUFFER_VERSION;

    SecBuffer OutBuffers[2];
    SecBufferDesc OutBufList;
    OutBufList.cBuffers = 2;
    OutBufList.pBuffers = OutBuffers;
    OutBufList.ulVersion = SECBUFFER_VERSION;

    //
    //  We will loop here until we get done or fail. We set the status initially to
    //  one that would cause us to continue.
    //
    tCIDLib::TBoolean   bDone = kCIDLib::False;
    tCIDLib::TCard4     c4CarryOver = 0;
    ULONG               ClAttrs = 0;
    TThread*            pthrCaller = TThread::pthrCaller();
    do
    {
        // Get the number of bytes in the next msg
        tCIDLib::TCard4 c4Cnt = cdsTar.c4ReadBytes
        (
            &pc1In[c4CarryOver], c4TokenSize - c4CarryOver, enctEnd, kCIDLib::False
        );

        //
        //  If zero bytes, then something is wrong. It would have waited until
        //  the end of the available time to get some data.
        //
        if (!c4Cnt)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_NoData
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
                , m_strName
            );
        }

        //
        //  Add the previous carry over to the count, and clear the carry over
        //  again for the next round.
        //
        c4Cnt += c4CarryOver;
        c4CarryOver = 0;

        // Make sure we didn't get more than the token size we are trying to get
        CIDAssert(c4Cnt <= c4TokenSize, L"Token was bigger than indicated size");

        // Set up the buffers for this round
        InBuffers[0].pvBuffer   = pc1In;
        InBuffers[0].cbBuffer   = c4Cnt;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;
        InBuffers[1].pvBuffer   = nullptr;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        OutBuffers[0].pvBuffer  = pc1Out;
        OutBuffers[0].cbBuffer  = c4TokenSize;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;
        OutBuffers[1].pvBuffer   = nullptr;
        OutBuffers[1].cbBuffer   = 0;
        OutBuffers[1].BufferType = SECBUFFER_ALERT;

        SECURITY_STATUS Status = ::AcceptSecurityContext
        (
            &m_pInfo->CredH
            , m_pInfo->bGotSecContext ? &m_pInfo->CtxH : 0
            , &InBufList
            , ASC_REQ_STREAM
            , 0
            , &m_pInfo->CtxH
            , &OutBufList
            , &ClAttrs
            , NULL
        );

        if (Status == SEC_E_INCOMPLETE_MESSAGE)
        {
            //
            //  Remember the current count as the carry over, and go back to
            //  the top. We'll try to read in more data past what we have already
            //  in the buffer.
            //
            c4CarryOver = c4Cnt;
            continue;
        }

        // Else, if any other error, we failed
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_AcceptSecCtx
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TCardinal(Status, tCIDLib::ERadices::Hex)
            );
        }

        if ((Status == SEC_I_COMPLETE_AND_CONTINUE)
        ||  (Status == SEC_I_COMPLETE_NEEDED))
        {
            Status = ::CompleteAuthToken(&m_pInfo->CtxH, &OutBufList);
            if (Status < 0)
            {
                facCIDSChan().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kSChanErrs::errcSChan_CompAuthToken
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , m_strName
                    , TCardinal(Status, tCIDLib::ERadices::Hex)
                );
            }

            //
            //  If complete and continue, set the status to continue for below.
            //  Else, set it to OK, to indicate we are done.
            //
            if (Status == SEC_I_COMPLETE_AND_CONTINUE)
                Status = SEC_I_CONTINUE_NEEDED;
            else
                Status = SEC_E_OK;
        }

        //
        //  If we aren't at continue needed there, then we are done at the end
        //  of this round.
        //
        bDone = (Status != SEC_I_CONTINUE_NEEDED);

        //
        //  Send back the data. We write it to the data source and then
        //  flush it out.
        //
        if (OutBuffers[0].cbBuffer)
        {
            cdsTar.WriteRawBytes
            (
                reinterpret_cast<tCIDLib::TCard1*>(OutBuffers[0].pvBuffer)
                , OutBuffers[0].cbBuffer
            );
            cdsTar.FlushOut(enctEnd);
        }

        // Indicate that we now have the context handle for subsequent rounds
        m_pInfo->bGotSecContext = kCIDLib::True;

        //
        //  See if we had extra data in the input buffer. If we are done, then
        //  it needs to be pushed back into the provided data source, so that it
        //  will be read as msg data when the caller starts to read.
        //
        //  Otherwise, we move it down and make it carryover data for the next
        //  round.
        //
        SecBuffer* pExtraBuf = nullptr;
        if ((InBuffers[1].BufferType == SECBUFFER_EXTRA) && (InBuffers[1].cbBuffer))
        {
            pExtraBuf = &InBuffers[1];
            c4CarryOver = pExtraBuf->cbBuffer;
            tCIDLib::TCard1* pc1Src = pc1In + (c4Cnt - c4CarryOver);

            if (bDone)
            {
                cdsTar.PushBack(pc1Src, c4CarryOver);
            }
             else
            {
                // Copy down the extra data
                tCIDLib::TCard1* pc1Tar = pc1In;
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CarryOver; c4Index++)
                {
                    *pc1Tar = *pc1Src;
                    pc1Tar++;
                    pc1Src++;
                }
            }
        }

        if (pthrCaller->bCheckShutdownRequest())
        {
            // Get us out with an exception
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_ShutdownReq
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , m_strName
            );
        }

    }   while (!bDone);

    // Allocate our packet buffer, deleting any previous first.
    delete [] m_pInfo->pc1Packet;
    m_pInfo->pc1Packet = nullptr;

    {
        SecPkgContext_StreamSizes StrmSzs;
        Status = ::QueryContextAttributes(&m_pInfo->CtxH, SECPKG_ATTR_STREAM_SIZES, &StrmSzs);

        m_pInfo->c4MaxHeader = StrmSzs.cbHeader;
        m_pInfo->c4MaxMsg = StrmSzs.cbMaximumMessage;
        m_pInfo->c4MaxTrailer = StrmSzs.cbTrailer;

        m_pInfo->c4PacketSize = m_pInfo->c4MaxHeader + m_pInfo->c4MaxMsg + m_pInfo->c4MaxTrailer;
    }

    m_pInfo->pc1Packet = new tCIDLib::TCard1[m_pInfo->c4PacketSize];
}


//
//  We send the passed data in block sized chunks (with a possible partial final one)
//  through the encryption process and then send the encrypted blocks, applying a
//  sequence number to each one.
//
tCIDLib::TVoid
TSChannel::TransmitData(        TCIDDataSrc&            cdsTar
                        , const tCIDLib::TCard1* const  pc1Data
                        , const tCIDLib::TCard4         c4SendBytes)
{
    // Get a pointer to our internal data structure
    CIDAssert(m_pInfo != nullptr, L"The secure channel internal data is not allocated");

    SecBuffer InBuffers[4];
    SecBufferDesc InBufList;
    InBufList.cBuffers = 4;
    InBufList.pBuffers = InBuffers;
    InBufList.ulVersion = SECBUFFER_VERSION;

    tCIDLib::TCard4         c4Left = c4SendBytes;
    const tCIDLib::TCard1*  pc1Cur = pc1Data;
    TThread*                pthrCaller = nullptr;
    while (c4Left)
    {
        tCIDLib::TCard4 c4ThisTime = c4Left;
        if (c4ThisTime > m_pInfo->c4MaxMsg)
            c4ThisTime = m_pInfo->c4MaxMsg;

        // Set up the pointers into the packet buffer
        tCIDLib::TCard1* pc1Header = m_pInfo->pc1Packet;
        tCIDLib::TCard1* pc1Msg = m_pInfo->pc1Packet + m_pInfo->c4MaxHeader;
        tCIDLib::TCard1* pc1Trailer = m_pInfo->pc1Packet + (m_pInfo->c4MaxHeader + m_pInfo->c4MaxMsg);

        InBuffers[0].pvBuffer   = pc1Header;
        InBuffers[0].cbBuffer   = m_pInfo->c4MaxHeader;
        InBuffers[0].BufferType = SECBUFFER_STREAM_HEADER;

        InBuffers[1].pvBuffer   = pc1Msg;
        InBuffers[1].cbBuffer   = c4ThisTime;
        InBuffers[1].BufferType = SECBUFFER_DATA;

        InBuffers[2].pvBuffer   = pc1Trailer;
        InBuffers[2].cbBuffer   = m_pInfo->c4MaxTrailer;
        InBuffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

        InBuffers[3].pvBuffer   = nullptr;
        InBuffers[3].cbBuffer   = 0;;
        InBuffers[3].BufferType = SECBUFFER_EMPTY;

        //
        //  Copy over this many bytes to the msg buffer. The data is encrypted
        //  in place.
        //
        TRawMem::CopyMemBuf(pc1Msg, pc1Cur, c4ThisTime);

        // And let's do the encryption
        SECURITY_STATUS Status = ::EncryptMessage(&m_pInfo->CtxH, 0, &InBufList, 0);
        if (Status < 0)
        {
            facCIDSChan().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSChanErrs::errcSChan_Encrypt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
                , TCardinal(Status, tCIDLib::ERadices::Hex)
            );
        }

        // Move our src pointer and count up
        pc1Cur += c4ThisTime;
        c4Left -= c4ThisTime;

        //
        //  And write it all out. Supposedly this will get us back a contiguous
        //  buffer we can just write out, and various examples show this. But I
        //  cannot see how that is possible since padding may be required for the
        //  data if not a full packet.
        //
        //  Since we are just buffering up date in the data source until we send it,
        //  there's no down side to just being very safe and writing out each buffer
        //  separately.
        //
        cdsTar.WriteRawBytes(InBuffers[0].pvBuffer, InBuffers[0].cbBuffer);
        cdsTar.WriteRawBytes(InBuffers[1].pvBuffer, InBuffers[1].cbBuffer);
        cdsTar.WriteRawBytes(InBuffers[2].pvBuffer, InBuffers[2].cbBuffer);

        if (c4Left)
        {
            if (!pthrCaller)
                pthrCaller = TThread::pthrCaller();

            //
            //  Return if asked to shutdown. We don't want to try to send any
            //  more data.
            //
            if (pthrCaller->bCheckShutdownRequest())
                return;
        }
    }

    // It's all buffered up now by the data source, so flush it
    cdsTar.FlushOut(TTime::enctNowPlusSecs(5));
}

