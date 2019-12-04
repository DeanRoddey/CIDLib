//
// FILE NAME: CIDSock_StreamSocket.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/1998
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
//  This file implements the TStreamSocket class.
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
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TStreamSocket,TSocket)
RTTIDecls(TClientStreamSocket,TStreamSocket)
RTTIDecls(TServerStreamSocket,TStreamSocket)



// ---------------------------------------------------------------------------
//   CLASS: TStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStreamSocket: Destructor
// ---------------------------------------------------------------------------
TStreamSocket::~TStreamSocket()
{
    // Get a quick ref to the implementation object
    TKrnlSocket& ksockThis = ksockImpl();

    // Make sure its not already closed in some way. If so, nada to do
    tCIDLib::TBoolean bOpen;
    if (!ksockThis.bIsOpen(bOpen))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Destruct)
            , kerrLast
            , tCIDLib::ESeverities::Warn
            , eXlatKrnlErrClass(kerrLast)
        );
    }

    if (!bOpen)
        return;

    // If already shutdown, then nothing to do here
    if (m_bIsShutdown)
        return;

    //
    //  Lets do the standard clean shutdown. We shutdown sends first, which
    //  will tell the other side we ain't sending any more.
    //
    if (!ksockThis.bShutdown(tCIDSock::ESockShutdown::Send))
    {
        #if CID_DEBUG_ON
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Shutdown
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , eXlatKrnlErrClass(kerrLast)
        );
        #endif
    }

    //
    //  Read any remaining data into a local tmp buffer until we get an error,
    //  or no more data comes back, or we timeout.
    //
    const tCIDLib::TCard4 c4BufSize = 256;
    tCIDLib::TCard4 c4Read;
    tCIDLib::TCard1 ac1Junk[c4BufSize];
    tCIDLib::TCard4 c4Count;
    tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusSecs(4);
    while (ksockThis.bDataReady(c4Count))
    {
        if (!ksockThis.bReceive(ac1Junk, c4BufSize, c4Read))
        {
            // If debugging, log anything other than a reset
            #if CID_DEBUG_ON
            TKrnlError kerrLast = TKrnlError::kerrLast();
            if (kerrLast.errcId() != kKrnlErrs::errcNet_ConnReset)
            {
                facCIDSock().LogKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Destruct)
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Warn
                    , eXlatKrnlErrClass(kerrLast)
                );
            }
            #endif
            break;
        }

        // When we've eaten it all, break out
        if (!c4Read)
            break;

        // If we timeout, then give up
        if (TTime::enctNow() > enctEnd)
            break;
    }

    // And finally we can shut down the socket
    if (!ksockThis.bShutdown(tCIDSock::ESockShutdown::Both))
    {
        #if CID_DEBUG_ON
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Shutdown)
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , eXlatKrnlErrClass(kerrLast)
        );
        #endif
    }
    m_bIsShutdown = kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TStreamSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStreamSocket::bIsConnected() const
{
    return m_bIsConnected;
}


tCIDLib::TBoolean TStreamSocket::bIsShutdown() const
{
    return m_bIsShutdown;
}


tCIDLib::TBoolean TStreamSocket::bNagleOn() const
{
    tCIDLib::TBoolean bRet;
    if (!ksockImpl().bGetSockOpt(TKrnlSocket::EBSockOpts::Nagle, bRet))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_QuerySockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , eXlatKrnlErrClass(kerrLast)
            , facCIDSock().strMsg(kSockMsgs::midSock_Nagle)
        );
    }
    return bRet;
}

tCIDLib::TBoolean TStreamSocket::bNagleOn(const tCIDLib::TBoolean bNewState)
{
    if (!ksockImpl().bSetSockOpt(TKrnlSocket::EBSockOpts::Nagle, bNewState))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_SetSockOpt
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , eXlatKrnlErrClass(kerrLast)
            , facCIDSock().strMsg(kSockMsgs::midSock_Nagle)
        );
    }
    return bNewState;
}



//
//  These two methods will do an unconditional read. If the socket is
//  non-blocking, and our sockets always are, then it will return zero
//  bytes if nothing is immediately available. Generally it is assumed
//  that the caller already checked that data is available.
//
//  If we get a disconnected error and zero bytes back, we'll return
//  zero and mark ourself disconnected, so any further reads will throw.
//
tCIDLib::TCard4
TStreamSocket::c4ReceiveRaw(        tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4MaxToRecv)
{
    CheckConnected(CID_FILE, CID_LINE);

    tCIDLib::TCard4 c4Got = 0;
    if (!ksockImpl().bReceive(pToFill, c4MaxToRecv, c4Got, kCIDLib::False))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();

        m_bIsConnected = kCIDLib::False;
        if (kerrLast.errcId() == kKrnlErrs::errcNet_ConnReset)
            return 0;

        // Otherwise, we just throw and assume a loss of connection
        DumpConnection();
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Receive)
            , kerrLast
            , tCIDLib::ESeverities::Failed
            , eXlatKrnlErrClass(kerrLast)
        );
    }

    return c4Got;
}

tCIDLib::TCard4
TStreamSocket::c4ReceiveMBuf(       TMemBuf&            mbufToFill
                            , const tCIDLib::TCard4     c4MaxToRecv)
{
    // Make sure the byte to receive is not larger than the max buffer size
    if (c4MaxToRecv > mbufToFill.c4MaxSize())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Figure out the actual size. If c4BytesToRecv is zero, then we use
    //  the max size of the buffer. Else, make sure the buffer is allocated
    //  out large enough.
    //
    tCIDLib::TCard4 c4ActualSz = c4MaxToRecv;

    if (c4ActualSz)
    {
        if (mbufToFill.c4Size() < c4ActualSz)
            mbufToFill.Reallocate(c4ActualSz);
    }
     else
    {
        c4ActualSz = mbufToFill.c4MaxSize();
    }

    // And all the other version
    return c4ReceiveRaw(mbufToFill.pc1Data(), c4ActualSz);
}



//
//  Receive with a timeout. We have four scenarios. The first two are the
//  main ones and receive either into a raw buffer or TMemBuf. We have two
//  others that take a millisecond timeout instead of an end time, which
//  just calc the end time and call the other versions.
//
tCIDLib::TCard4
TStreamSocket::c4ReceiveRawTO(          tCIDLib::TVoid* const   pToFill
                                , const tCIDLib::TEncodedTime   enctWait
                                , const tCIDLib::TCard4         c4MaxToRecv
                                , const tCIDLib::EAllData       eAllData)
{
    // If already disconnected, we fail
    CheckConnected(CID_FILE, CID_LINE);

    //
    //  We'll have to do our own timeout processing here. And, since a
    //  packet can start arriving at any time, we will want to provide a
    //  little grace period if we have some data by the time the original
    //  timeout occurs, so we use a flag that lets us do a one time extension
    //  of the end time if needed.
    //
    tCIDLib::TBoolean bExtended = kCIDLib::False;

    //
    //  We will use a local temp buffer to read into and dump them one at
    //  at time into the caller's buffer, until we get no more data or
    //  we fill the caller's buffer. If we didn't do this, we'd have to
    //  always allocate the caller's buffer to the max to receive size,
    //  which would be inefficient.
    //
    tCIDLib::TBoolean   bMoreData;
    tCIDLib::TCard4     c4SoFar = 0;
    tCIDLib::TCard4     c4ThisTime;

    //
    //  We need a pointer to the output buffer that we can index, so get
    //  a pointer to it as a TCard1 buffer.
    //
    tCIDLib::TCard1* pc1ToFill = static_cast<tCIDLib::TCard1*>(pToFill);

    // Loop until we run out of time or we get enough bytes
    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    tCIDLib::TEncodedTime enctCurWait;
    tCIDLib::TEncodedTime enctEnd;
    if (enctWait == kCIDLib::enctMaxWait)
        enctEnd = kCIDLib::enctMaxWait;
    else
        enctEnd = enctCur + enctWait;

    //
    //  We want the check at the end, so that, if we get a zero wait time,
    //  we'll still do the loop once.
    //
    TThread* pthrThis = nullptr;
    do
    {
        // Wait for up to half a second this time around
        if (enctCur + kCIDLib::enctHalfSecond > enctEnd)
            enctCurWait = (enctEnd - enctCur);
        else
            enctCurWait = kCIDLib::enctHalfSecond;

        //
        //  We assume any error here is fatal and just dump the connection
        //  and fail, even if data has been read, since obviously the
        //  socket is in a bad state now. If the other side disconnects,
        //  we'll still be successful here until we kill the socket. THe
        //  read below will catch the final end of input and mark us as
        //  disconnected.
        //
        if (!ksockImpl().bWaitForDataReady(bMoreData, enctCurWait))
        {
            TKrnlError kerrLast = TKrnlError::kerrLast();
            DumpConnection();
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Wait)
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , eXlatKrnlErrClass(kerrLast)
            );
        }

        // If we got some data, then get it and store it
        if (bMoreData)
        {
            // Receive what has arrived so far
            if (!ksockImpl().bReceive(  &pc1ToFill[c4SoFar]
                                        , c4MaxToRecv - c4SoFar
                                        , c4ThisTime))
            {
                //
                //  If the it's that the other side disconnected normally,
                //  and we don't ahve to read all requested bytes and we got
                //  something, then we can just return with whatever we have.
                //
                TKrnlError kerrLast = TKrnlError::kerrLast();

                //
                //  If it's not a connection reset error, just go ahead nd
                //  throw a receive exception, since we are in some bad
                //  way.
                //
                if (kerrLast.errcId() != kKrnlErrs::errcNet_ConnReset)
                {
                    DumpConnection();
                    facCIDSock().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Receive)
                        , kerrLast
                        , tCIDLib::ESeverities::Failed
                        , eXlatKrnlErrClass(kerrLast)
                    );
                }

                //
                //  It has to have been a connection reset. If we got
                //  zero data, then we are closed and hosed, so mark us
                //  closed so that they will get an error next time if
                //  they try to read again.
                //
                if (!c4SoFar)
                    m_bIsConnected = kCIDLib::False;

                //
                //  It's a connection reset and we do have some data to
                //  return. But, if they said fail if not all data was
                //  read, we need to throw an excetion to indicate that
                //  we timed out. We don't mark it disconnected yet, since
                //  it may not be.
                //
                if (eAllData == tCIDLib::EAllData::FailIfNotAll)
                {
                    facCIDSock().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kSockErrs::errcSock_NotAllData
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::NotAllRead
                    );
                }
                break;
            }

            //
            //  Bump the current count of bytes read so far. If this meets our
            //  requirements, then get out now before wasting time doing
            //  another countdown calculation.
            //
            c4SoFar += c4ThisTime;
            if (c4SoFar == c4MaxToRecv)
                break;
        }

        // Shouldn't be possible, but check
        #if CID_DEBUG_ON
        if (c4SoFar > c4MaxToRecv)
        {
            facCIDSock().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_TargetBufOverflow
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
            );
        }
        #endif

        // Check for a shutdown request
        if (!pthrThis)
            pthrThis = TThread::pthrCaller();
        if (pthrThis->bCheckShutdownRequest())
            break;

        // Update the current time and see if we are out of time or extend
        enctCur = TTime::enctNow();
        if ((enctCur >= enctEnd) && c4SoFar && !bExtended
        &&  (eAllData == tCIDLib::EAllData::FailIfNotAll))
        {
            bExtended = kCIDLib::True;
            enctEnd += (250 * kCIDLib::enctOneMilliSec);
        }
    }   while (enctCur < enctEnd);

    //
    //  If we didn't get all the bytes requested then look at the all data
    //  flag and see if that's an error.
    //
    if ((c4SoFar < c4MaxToRecv) && (eAllData == tCIDLib::EAllData::FailIfNotAll))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_NotAllData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
        );
    }

    // Return the bytes read
    return c4SoFar;
}

tCIDLib::TCard4
TStreamSocket::c4ReceiveRawTOMS(        tCIDLib::TVoid* const   pToFill
                                , const tCIDLib::TCard4         c4Millis
                                , const tCIDLib::TCard4         c4MaxToRecv
                                , const tCIDLib::EAllData       eAllData)
{
    const tCIDLib::TEncodedTime enctWait(c4Millis * kCIDLib::enctOneMilliSec);
    return c4ReceiveRawTO(pToFill, enctWait, c4MaxToRecv, eAllData);
}


tCIDLib::TCard4
TStreamSocket::c4ReceiveMBufTO(         TMemBuf&                mbufToFill
                                , const tCIDLib::TEncodedTime   enctWait
                                , const tCIDLib::TCard4         c4MaxToRecv
                                , const tCIDLib::EAllData       eAllData)
{
    // Make sure that the receive size isn't zero if debugging
    #if CID_DEBUG_ON
    if (!c4MaxToRecv)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_ZeroBytesRequested
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    #endif

    // Make sure the requested isn't larger than the max buffer size
    if (c4MaxToRecv > mbufToFill.c4MaxSize())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(mbufToFill.c4MaxSize())
            , TCardinal(c4MaxToRecv)
        );
    }

    // Make sure the buffer is allocated out large enough
    if (mbufToFill.c4Size() < c4MaxToRecv)
        mbufToFill.Reallocate(c4MaxToRecv);

    // Now call the other version
    return c4ReceiveRawTO(mbufToFill.pc1Data(), enctWait, c4MaxToRecv, eAllData);
}

tCIDLib::TCard4
TStreamSocket::c4ReceiveMBufTOMS(       TMemBuf&            mbufToFill
                                , const tCIDLib::TCard4     c4Millis
                                , const tCIDLib::TCard4     c4MaxToRecv
                                , const tCIDLib::EAllData   eAllData)
{
    const tCIDLib::TEncodedTime enctWait(c4Millis * kCIDLib::enctOneMilliSec);
    return c4ReceiveMBufTO(mbufToFill, enctWait, c4MaxToRecv, eAllData);
}


// Do a polite shutdown of this buffer, returning any remaining data
tCIDLib::TCard4 TStreamSocket::c4Shutdown(TMemBuf& mbufToFill)
{
    //
    //  If we are already shutdown, then log a warning error if in debug
    //  mode. Otherwise just return zero.
    //
    if (m_bIsShutdown)
    {
        if (facCIDSock().bLogWarnings())
        {
            facCIDSock().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_AlreadyShutdown
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Already
            );
        }
        return 0;
    }

    // To track how much data we read, which is the return value
    tCIDLib::TCard4 c4BytesRead = 0;

    // See if we got a buffer to copy the remained read data to
    const tCIDLib::TBoolean bGotBuffer = TMemBuf::bIsNullObject(mbufToFill);

    // Get a quick ref to the implementation object
    TKrnlSocket& ksockThis = ksockImpl();

    //
    //  Lets do the standard clean shutdown. We shutdown sends
    //  first, which will tell the other side we ain't sending
    //  any more.
    //
    if (!ksockThis.bShutdown(tCIDSock::ESockShutdown::Send))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Shutdown
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Mark this socket as shutdown and not connected
    m_bIsShutdown = kCIDLib::True;
    m_bIsConnected = kCIDLib::False;

    //
    //  Read any remaining data into the passed memory buffer. We only copy as
    //  many bytes as will fit into the buffer and throw the rest away. If we
    //  have no buffer, we read it in and throw it away anyway.
    //
    //  To make sure we don't get hung up, we'll only read for up to a couple
    //  seconds.
    //
    const tCIDLib::TCard4   c4BufSize = mbufToFill.c4MaxSize();
    const tCIDLib::TCard4   c4TmpBufSize = 2048;
    tCIDLib::TCard1         ac1Tmp[c4TmpBufSize];
    tCIDLib::TCard4         c4Bytes = 0;
    tCIDLib::TCard4         c4BytesAvail;
    tCIDLib::TEncodedTime   enctEnd = TTime::enctNowPlusSecs(2);

    if (ksockThis.bDataReady(c4BytesAvail))
    {
        while (c4BytesAvail && (enctEnd > TTime::enctNow()))
        {
            // Read a buffer's worth, but just break out if an error
            if (!ksockThis.bReceive(ac1Tmp, c4TmpBufSize, c4Bytes))
                break;

            if (c4Bytes && bGotBuffer)
            {
                //
                //  Quit taking bytes when the buffer is full, and handle
                //  the last potentially partial copy into the buffer to
                //  totally fill it up.
                //
                if (c4BytesRead + c4Bytes <= c4BufSize)
                    mbufToFill.CopyIn(ac1Tmp, c4Bytes, c4BytesRead);
                else if (c4BytesRead < c4BufSize)
                    mbufToFill.CopyIn(ac1Tmp, c4BufSize - c4BytesRead, c4BytesRead);
            }

            // Track the bytes read
            c4BytesRead += c4Bytes;

            if (!ksockThis.bDataReady(c4BytesAvail))
                break;
        }
    }

    // Now fully shut it down before its closed
    if (!ksockThis.bShutdown(tCIDSock::ESockShutdown::Both))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Shutdown
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Return the bytes read. If this is larger than the max size of the
    //  passed buffer, the caller knows that some bytes were lost.
    //
    return c4BytesRead;
}


//
//  Do the usual connection operation to the indicated end point. If not already
//  bound, this will implicitly bind the socket. We will wait up 2 seconds for
//  it to connect, watching for shutdown requests in the meantime.
//
//  We are non-blocking so the kernel connect call, if it times out, doesn't
//  stop the connect process. So we can call it repeatedly and on subsequent
//  calls it will just do another check and wait. We half a second at a time.
//
tCIDLib::TVoid
TStreamSocket::Connect( const   TIPEndPoint&            ipepConnectTo
                        , const tCIDLib::TEncodedTime   enctWait)
{
    if (m_bIsConnected)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_AlreadyConnected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    const tCIDLib::TEncodedTime enctEnd  = enctCur + enctWait;

    //
    //  We want to check for the timeout at the end so that if we get a
    //  zero wait time, we'll still do at least one check.
    //
    TThread* pthrCaller = nullptr;
    do
    {
        // Wait for up to half a second this time, or what's left
        tCIDLib::TEncodedTime enctThisTime = (enctEnd - enctCur);
        if (enctThisTime > kCIDLib::enctHalfSecond)
            enctThisTime = kCIDLib::enctHalfSecond;

        // If we connected, we can break out
        if (ksockImpl().bConnect(ipepConnectTo.kipaThis()
                                , ipepConnectTo.ippnThis()
                                , enctThisTime))
        {
            // Mark it connected now and not shutdown
            m_bIsConnected = kCIDLib::True;
            m_bIsShutdown = kCIDLib::False;

            // Also mark it bound since, if it wasn't before, it is now
            bIsBound(kCIDLib::True);
            break;
        }

        //  If it's not just a timeout error, then throw
        TKrnlError kerrLast = TKrnlError::kerrLast();
        if (kerrLast.errcId() != kKrnlErrs::errcGen_Timeout)
        {
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kSockErrs::errcSock_Connect
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , eXlatKrnlErrClass(kerrLast)
                , ipepConnectTo
            );
        }

        // Check for a shutdown request. If so, break out
        if (!pthrCaller)
            pthrCaller = TThread::pthrCaller();
        if (pthrCaller->bCheckShutdownRequest())
            break;

        enctCur = TTime::enctNow();
    }   while (enctCur < enctEnd);

    // If we timed out, then throw
    if (!m_bIsConnected)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_Connect
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , ipepConnectTo
        );
    }
}


//
//  If connected, returns the end point connected to. Else throws a not
//  connected error.
//
TIPEndPoint TStreamSocket::ipepRemoteEndPoint() const
{
    CheckConnected(CID_FILE, CID_LINE);

    TKrnlIPAddr         kipaRemote;
    tCIDLib::TIPPortNum ippnRemote;

    if (!ksockImpl().bRemoteEndPoint(ippnRemote, kipaRemote))
    {
        TKrnlError kerrLast = TKrnlError::kerrLast();
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_QueryRemotePoint)
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , eXlatKrnlErrClass(kerrLast)
        );
    }
    return TIPEndPoint(kipaRemote, ippnRemote);
}


// Opens the socket using the indicated protocol, if not already opened
tCIDLib::TVoid TStreamSocket::Open( const   tCIDSock::ESockProtos eProtocol
                                    , const tCIDSock::EAddrTypes    eAddrType)
{
    // Call our parent to create the socket, providing the correct type
    Create(tCIDSock::ESocketTypes::Stream, eProtocol, eAddrType);
}


tCIDLib::TVoid
TStreamSocket::Send(const   tCIDLib::TVoid* const   pToSend
                    , const tCIDLib::TCard4         c4BytesToSend)
{
    // If not connected, then obviously this is an error
    CheckConnected(CID_FILE, CID_LINE);

    if (!ksockImpl().bSend(pToSend, c4BytesToSend))
    {
        // Assume any failure here means loss of connection
        TKrnlError kerrLast = TKrnlError::kerrLast();

        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Send)
            , kerrLast
            , tCIDLib::ESeverities::Failed
            , eXlatKrnlErrClass(kerrLast)
        );
    }
}


tCIDLib::TVoid
TStreamSocket::Send(  const TMemBuf&        mbufToSend
                    , const tCIDLib::TCard4 c4BytesToSend)
{
    // Make sure the bytes to send is not larger than the allocation size
    if (c4BytesToSend > mbufToSend.c4Size())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  First get the actual bytes. If the c4BytesToSend is zero, then we
    //  use the allocation size of the buffer.
    //
    const tCIDLib::TCard4 c4ActualSz
    (
        c4BytesToSend ? c4BytesToSend : mbufToSend.c4Size()
    );

    // And call the other version
    Send(mbufToSend.pc1Data(), c4ActualSz);
}


//
//  Sends the the indicated number of bytes. Assumes this is a non-blocking
//  socket and that if the other side is busy that we'll get a 'would block'
//  error. If so, we do a wait for up to the indicated max wait milllis. If
//  the socket becomes available in that time, we try again.
//
tCIDLib::TVoid
TStreamSocket::SendThrottled(const  tCIDLib::TVoid*         pToSend
                            , const tCIDLib::TEncodedTime   enctWait
                            , const tCIDLib::TCard4         c4BytesToSend)
{
    try
    {
        Send(pToSend, c4BytesToSend);

        // We didn't fail, so just return now
        return;
    }

    catch(TError& errToCatch)
    {
        if (errToCatch.errcKrnlId() != kKrnlErrs::errcGen_WouldBlock)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }

    // We threw and it was a would block, so wait for it to become available
    if (!bWaitForSendReady(enctWait))
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_OtherSideBusy
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , ipepRemoteEndPoint()
        );
    }

    // And try again. If it fails again, just propopgate
    Send(pToSend, c4BytesToSend);
}

tCIDLib::TVoid
TStreamSocket::SendThrottled(const  TMemBuf&                mbufToSend
                            , const tCIDLib::TEncodedTime   enctWait
                            , const tCIDLib::TCard4         c4BytesToSend)
{
    // Make sure the bytes to send is not larger than the allocation size
    if (c4BytesToSend > mbufToSend.c4Size())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcSock_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(mbufToSend.c4Size())
            , TCardinal(c4BytesToSend)
        );
    }

    //
    //  First get the actual bytes. If the c4BytesToSend is zero, then we
    //  use the allocation size of the buffer.
    //
    const tCIDLib::TCard4 c4ActualSz = c4BytesToSend ? c4BytesToSend :
                                        mbufToSend.c4Size();

    // And call the other version
    SendThrottled(mbufToSend.pc1Data(), enctWait, c4ActualSz);
}


// ---------------------------------------------------------------------------
//  TStreamSocket: Hidden Constructors and operators
// ---------------------------------------------------------------------------
TStreamSocket::TStreamSocket() :

    m_bIsConnected(kCIDLib::False)
    , m_bIsShutdown(kCIDLib::True)
{
}

TStreamSocket::TStreamSocket(const  tCIDSock::ESockProtos eProtocol
                            , const tCIDSock::EAddrTypes    eAddrType) :

    TSocket(tCIDSock::ESocketTypes::Stream, eProtocol, eAddrType)
    , m_bIsConnected(kCIDLib::False)
    , m_bIsShutdown(kCIDLib::True)
{
}

TStreamSocket::TStreamSocket(const TSocketHandle& hsockToAdopt) :

    TSocket(hsockToAdopt)
    , m_bIsConnected(kCIDLib::True)
    , m_bIsShutdown(kCIDLib::False)
{
}


// ---------------------------------------------------------------------------
//  TStreamSocket: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  The base socket class calls them when the socket is closed. So, if they do a hard
//  close instead of a shutdown, we can still know it closed and clear our flags.
//
tCIDLib::TVoid TStreamSocket::SocketClosed()
{
    m_bIsConnected = kCIDLib::False;
    m_bIsShutdown  = kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TStreamSocket: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStreamSocket::CheckConnected(  const   tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    if (!m_bIsConnected)
    {
        facCIDSock().ThrowErr
        (
            pszFile
            , c4Line
            , kSockErrs::errcSock_NotConnected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
        );
    }
}


tCIDLib::TVoid TStreamSocket::DumpConnection()
{
    m_bIsConnected = kCIDLib::False;
    ksockImpl().bShutdown(tCIDSock::ESockShutdown::Both);
}





// ---------------------------------------------------------------------------
//   CLASS: TClientStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TClientStreamSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TClientStreamSocket::
TClientStreamSocket(const   tCIDSock::ESockProtos eProtocol
                    , const tCIDSock::EAddrTypes    eAddrType) :

    TStreamSocket(eProtocol, eAddrType)
{
}

TClientStreamSocket::
TClientStreamSocket(const   tCIDSock::ESockProtos eProtocol
                    , const TIPEndPoint&            ipepTarget) :

    TStreamSocket(eProtocol, ipepTarget.eAddrType())
{
    try
    {
        // First lets try to connect
        Connect(ipepTarget);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Close the socket back down
        if (!ksockImpl().bClose())
        {
            // Log a warning level, which will not throw
            #if CID_DEBUG_ON
            TKrnlError kerrLast = TKrnlError::kerrLast();
            facCIDSock().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , errcXlatKrnlErr(kerrLast, kSockErrs::errcSock_Create)
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , eXlatKrnlErrClass(kerrLast)
            );
            #endif
        }
        throw;
    }
}

TClientStreamSocket::~TClientStreamSocket()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TServerStreamSocket
//  PREFIX: sock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TServerStreamSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TServerStreamSocket::~TServerStreamSocket()
{
}


// ---------------------------------------------------------------------------
//  TServerStreamSocket: Hidden constructors
// ---------------------------------------------------------------------------
TServerStreamSocket::TServerStreamSocket()
{
}

TServerStreamSocket::TServerStreamSocket(const TSocketHandle& hsockToAdopt) :

    TStreamSocket(hsockToAdopt)
{
}



// ---------------------------------------------------------------------------
//   CLASS: TStreamSocketJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStreamSocketJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TStreamSocketJan::TStreamSocketJan(TStreamSocket* const psockToSanitize) :

    m_psockToSanitize(psockToSanitize)
{
}

TStreamSocketJan::~TStreamSocketJan()
{
    if (m_psockToSanitize)
    {
        try
        {
            if (m_psockToSanitize->bIsConnected())
                m_psockToSanitize->c4Shutdown();
        }

        catch(const TError& errToCatch)
        {
            if (facCIDSock().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        try
        {
            delete m_psockToSanitize;
            m_psockToSanitize = nullptr;
        }

        catch(const TError& errToCatch)
        {
            if (facCIDSock().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }
    }
}


// ---------------------------------------------------------------------------
//  TStreamSocketJan: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Provide access to the socket, which may be zero
TStreamSocket* TStreamSocketJan::psockData()
{
    return m_psockToSanitize;
}


// Orphan it out. The caller becomes responsible for it
TStreamSocket* TStreamSocketJan::psockOrphan()
{
    TStreamSocket* psockRet = m_psockToSanitize;
    m_psockToSanitize = nullptr;
    return psockRet;
}


// Orphan it without returning it. THe caller is responsible for it
tCIDLib::TVoid TStreamSocketJan::Orphan()
{
    m_psockToSanitize = nullptr;
}

