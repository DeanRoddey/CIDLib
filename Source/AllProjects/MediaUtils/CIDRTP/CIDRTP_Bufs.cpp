//
// FILE NAME: CIDRTP_Bufs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/21/2014
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
//  This file implements some simple buffer classes, which are used to queue up
//  data as it passes through our receiver and transmitter classes. We also provide
//  pools to alocate these buffers from, and janitor classes that allocate/free them
//  on a scoped basis as required.
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
//  CLASS: TCIDRTPMediaBuf
// PREFIX: rtpb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPMediaBuf: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  We initialize our buffer so that it can hold a worst case header plus the
//  max payload the using code will be using.
//
TCIDRTPMediaBuf::TCIDRTPMediaBuf(const tCIDLib::TCard4 c4MaxPayload) :

    m_bEndOfStream(kCIDLib::False)
    , m_bMarker(kCIDLib::False)
    , m_c2SeqNum(0)
    , m_c4AtMilli(0)
    , m_c4AtSample(0)
    , m_c4BufSz(kCIDRTP::c4MaxHdrSz + c4MaxPayload)
    , m_c4CSrcCnt(0)
    , m_c4DataBytes(0)
    , m_c4PLType(0)
    , m_c4SyncSrc(0)
    , m_mbufData(kCIDRTP::c4MaxHdrSz + c4MaxPayload, kCIDRTP::c4MaxHdrSz + c4MaxPayload)
{
}

TCIDRTPMediaBuf::~TCIDRTPMediaBuf()
{
}


// ---------------------------------------------------------------------------
//  TCIDRTPMediaBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Take our data and build up the real RTP message into the passed buffer. We
//  return a couple of the values that the caller will tend to need for sending
//  out the msg and doing some calcs. He is going to have to immediatley release
//  the buffer back to the free pool, so he can't keep us around to get this info
//  out later.
//
tCIDLib::TBoolean
TCIDRTPMediaBuf::bBuildMsg( TMemBuf&                mbufToFill
                            , COP tCIDLib::TCard4&  c4AtMilli
                            , COP tCIDLib::TCard4&  c4AtSample
                            , COP tCIDLib::TCard4&  c4HdrBytes
                            , COP tCIDLib::TCard4&  c4DataBytes
                            , COP tCIDLib::TCard2&  c2SeqNum
                            , COP tCIDLib::TCard4&  c4MsgSz) const
{
    // If this is the end of stream dummy buffer, then just return false
    if (m_bEndOfStream)
        return kCIDLib::False;

    // Otherwise, set up the buffer
    c4MsgSz = 0;

    //
    //  Build up the first byte. Don't currently support any extensions or padding.
    //  csrc count will be zero for now as well, but go ahead and do it.
    //
    mbufToFill.PutCard1
    (
        tCIDLib::TCard1((0x2UL << 6) | m_c4CSrcCnt), c4MsgSz++
    );

    //
    //  And the second byte, only support the payload type since no video currently,
    //  but go ahead and do the marker anyway.
    //
    mbufToFill.PutCard1
    (
        tCIDLib::TCard1(m_c4PLType | ((m_bMarker ? 0x1UL : 0UL) << 7)), c4MsgSz++
    );

    // The sequence number
    #if CID_LITTLE_ENDIAN
    mbufToFill.PutCard2(m_c2SeqNum, c4MsgSz);
    #else
    mbufToFill.PutCard2(TRawBits::c2SwapBytes(m_c2SeqNum), c4MsgSz);
    #endif
    c4MsgSz += sizeof(m_c2SeqNum);


    // The time stamp and sync source
    #if CID_LITTLE_ENDIAN
    mbufToFill.PutCard4(m_c4AtSample, c4MsgSz);
    #else
    mbufToFill.PutCard4(TRawBits::c4SwapBytes(m_c4AtSample), c4MsgSz);
    #endif
    c4MsgSz += sizeof(m_c4AtSample);

    #if CID_LITTLE_ENDIAN
    mbufToFill.PutCard4(m_c4SyncSrc, c4MsgSz);
    #else
    mbufToFill.PutCard4(TRawBits::c4SwapBytes(m_c4SyncSrc), c4MsgSz);
    #endif
    c4MsgSz += sizeof(m_c4SyncSrc);

    // If any contributing sources (not currently supported)
    if (m_c4CSrcCnt)
    {
        CIDAssert2(L"Contributing sources not supported yet");
    }

    // This is the header size so store that
    c4HdrBytes = c4MsgSz;

    // Copy in the media data if we have any
    if (m_c4DataBytes)
    {
        mbufToFill.CopyIn(m_mbufData, m_c4DataBytes, c4MsgSz);
        c4MsgSz += m_c4DataBytes;
    }

    c2SeqNum = m_c2SeqNum;
    c4DataBytes = m_c4DataBytes;

    c4AtMilli = m_c4AtMilli;
    c4AtSample = m_c4AtSample;

    // This is a buffer to send, not the end of stream marker
    return kCIDLib::True;
}



tCIDLib::TVoid
TCIDRTPMediaBuf::Set(const  tCIDLib::TBoolean   bMarker
                    , const tCIDLib::TCard2     c2SeqNum
                    , const tCIDLib::TCard4     c4DataBytes
                    , const tCIDLib::TCard4     c4PLType
                    , const tCIDLib::TCard4     c4SyncSrc
                    , const tCIDLib::TCard4     c4AtMilli
                    , const tCIDLib::TCard4     c4AtSample
                    , const TMemBuf&            mbufData)
{
    m_bMarker       = bMarker;
    m_c2SeqNum      = c2SeqNum;
    m_c4AtMilli     = c4AtMilli;
    m_c4AtSample    = c4AtSample;
    m_c4DataBytes   = c4DataBytes;
    m_c4PLType      = c4PLType;
    m_c4SyncSrc     = c4SyncSrc;

    // If zero bytes, it's the end of stream, else copy in the data.
    if (c4DataBytes)
    {
        m_bEndOfStream  = kCIDLib::False;
        m_mbufData.CopyIn(mbufData, c4DataBytes);
    }
     else
    {
        m_bEndOfStream  = kCIDLib::True;
    }

    // For now not supported
    m_c4CSrcCnt    = 0;
}

