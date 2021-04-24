//
// FILE NAME: CIDSock_SockStreamImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2000
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
//  This file implements the classes that allow binary streams to operate
//  via a socket.
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
RTTIDecls(TSockInStreamImpl,TInStreamImpl)
RTTIDecls(TSockOutStreamImpl,TOutStreamImpl)



// ---------------------------------------------------------------------------
//   CLASS: TSockInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSockInStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TSockInStreamImpl::TSockInStreamImpl(const  tCIDLib::TEncodedTime enctMaxWait
                                    ,       TStreamSocket* const  psockToAdopt
                                    , const tCIDLib::EAdoptOpts   eAdopt) :
    m_c8Cur(0)
    , m_enctMaxWait(enctMaxWait)
    , m_psockIn(psockToAdopt)
    , m_eAdopt(eAdopt)
{
}

//
//  In this case, we can't adopt. If the stream is adopted, then it will be
//  adopted by the output impl who will clean it up.
//
TSockInStreamImpl::TSockInStreamImpl(const  tCIDLib::TEncodedTime enctMaxWait
                                    , const TSockOutStreamImpl&   strmiToSyncTo) :
    m_c8Cur(0)
    , m_enctMaxWait(enctMaxWait)
    , m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
    , m_psockIn(strmiToSyncTo.m_psockOut)
{
}

TSockInStreamImpl::~TSockInStreamImpl()
{
    // If we adopted it, then clean it up
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
    {
        try
        {
            delete m_psockIn;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TSockInStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSockInStreamImpl::bIsOpen() const
{
    return m_psockIn->bIsOpen();
}


tCIDLib::TCard4
TSockInStreamImpl::c4ReadBytes(         tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToRead)
{
    tCIDLib::TCard4 c4Got = 0;
    try
    {
        // Wait for some data and read it if we get it
        if (m_psockIn->bWaitForDataReady(m_enctMaxWait))
            c4Got = m_psockIn->c4ReceiveRaw(pBuffer, c4BytesToRead - c4Got);
    }

    catch(TError& errToCatch)
    {
        //
        //  If its an indication that we lost the connection, then just
        //  return with whatever we got, else rethrow it
        //
        if (errToCatch.eClass() != tCIDLib::EErrClasses::LostConnection)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }
    return c4Got;
}


tCIDLib::TCard8 TSockInStreamImpl::c8CurPos() const
{
    return m_c8Cur;
}


tCIDLib::TVoid TSockInStreamImpl::Reset()
{
    //
    //  We cannot support this operation
    //
    //  This is a CIDLib error!!
    //
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStrm_ResetNotSupported
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
        , clsThis()
    );
}


tCIDLib::TVoid
TSockInStreamImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    //
    //  We just read the indicated number of bytes from the stream and throw
    //  them away. We use a local buffer and read into it as many times as
    //  required to skip the indicated number of bytes.
    //
    const tCIDLib::TCard4 c4BufSize = 1024;
    CIDLib_Suppress(26494) // Don't need to init, we are reading into it below
    tCIDLib::TCard1 ac1Buf[c4BufSize];
    tCIDLib::TCard4 c4CountDown = c4SkipBy;

    // Handle as many full buffers as we can first
    while (c4CountDown > c4BufSize)
    {
        if (c4ReadBytes(ac1Buf, c4BufSize) != c4BufSize)
        {
            // Not we are throwing a CIDLib stream error here!!
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStrm_InvalidRelSeek
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(c4SkipBy)
            );
        }
        c4CountDown -= c4BufSize;
    }

    // And then do the last partial buffer
    if (c4ReadBytes(ac1Buf, c4CountDown) != c4CountDown)
    {
        // Not we are throwing a CIDLib stream error here!!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_InvalidRelSeek
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4SkipBy)
        );
    }
}



// ---------------------------------------------------------------------------
//  TSockInStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSockInStreamImpl::bIsDataAvailable() const
{
    tCIDLib::TCard4 c4Count;
    return m_psockIn->bDataReady(c4Count);
}


tCIDLib::TBoolean
TSockInStreamImpl::bWaitForDataReady(const tCIDLib::TEncodedTime enctWait) const
{
    return m_psockIn->bWaitForDataReady(enctWait);
}

tCIDLib::TBoolean
TSockInStreamImpl::bWaitForDataReadyMS(const tCIDLib::TCard4 c4WaitMillis) const
{
    return m_psockIn->bWaitForDataReadyMS(c4WaitMillis);
}


tCIDLib::TEncodedTime TSockInStreamImpl::enctMaxWait() const
{
    return m_enctMaxWait;
}

tCIDLib::TEncodedTime
TSockInStreamImpl::enctMaxWait(const tCIDLib::TEncodedTime enctToSet)
{
    m_enctMaxWait = enctToSet;
    return m_enctMaxWait;
}


TStreamSocket& TSockInStreamImpl::sockThis()
{
    return *m_psockIn;
}

const TStreamSocket& TSockInStreamImpl::sockThis() const
{
    return *m_psockIn;
}




// ---------------------------------------------------------------------------
//   CLASS: TSockOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSockOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TSockOutStreamImpl::TSockOutStreamImpl(         TStreamSocket* const psockToAdopt
                                        , const tCIDLib::EAdoptOpts  eAdopt) :
    m_c8Cur(0)
    , m_eAdopt(eAdopt)
    , m_psockOut(psockToAdopt)
{
}

TSockOutStreamImpl::~TSockOutStreamImpl()
{
    // If we adopted the socket, then clean it up
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
        delete m_psockOut;
}


// ---------------------------------------------------------------------------
//  TSockOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSockOutStreamImpl::bIsOpen() const
{
    return m_psockOut->bIsOpen();
}


tCIDLib::TCard4
TSockOutStreamImpl::c4WriteBytes(const  tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToWrite)
{
    //
    //  Send the data, but we have to be careful of overrunning the target.
    //  So we do a throttled send in chunks.
    //
    const tCIDLib::TCard4 c4BufSz = 8192;
    const tCIDLib::TCard1* pc1ToSend = (const tCIDLib::TCard1*)pBuffer;
    const tCIDLib::TCard1* pc1End = pc1ToSend + c4BytesToWrite;
    while (pc1ToSend < pc1End)
    {
        //
        //  Figure out how much to read this time. It's either a
        //  buffer's worth, or enough to finish the send.
        //
        tCIDLib::TCard4 c4ToSend = pc1End - pc1ToSend;
        if (c4ToSend > c4BufSz)
            c4ToSend = c4BufSz;

        // Do a throttled send of this chunk
        m_psockOut->SendThrottled
        (
            pc1ToSend, kCIDLib::enctFiveSeconds, c4ToSend
        );

        // Move forward by the bytes sent
        pc1ToSend += c4ToSend;
    }
    return c4BytesToWrite;
}


tCIDLib::TCard8 TSockOutStreamImpl::c8CurPos() const
{
    return m_c8Cur;
}


TStreamSocket& TSockOutStreamImpl::sockThis()
{
    return *m_psockOut;
}

const TStreamSocket& TSockOutStreamImpl::sockThis() const
{
    return *m_psockOut;
}


tCIDLib::TVoid TSockOutStreamImpl::Reset()
{
    //
    //  We cannot support this operation
    //
    //  This is a CIDLib error, not one of ours!
    //
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStrm_ResetNotSupported
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
        , clsThis()
    );
}


tCIDLib::TVoid TSockOutStreamImpl::SeekToEnd()
{
    // This is a no-op for this type of stream
}

