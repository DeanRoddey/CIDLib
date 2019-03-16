//
// FILE NAME: CIDRTP_Bufs_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/21/2014
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
//  This is the header for the I/O buffer related classes. We have one type of buffer
//  for incoming raw msgs (or msgs to be sent), so these are in the RTP packet
//  format with header in place, followed by payload.
//
//  The other is for the processed data, which the receiver class queues up to be
//  output to the I/O target and which in the transmitter is queued up by the reader
//  thread that reads in from the I/O source. This one is our own format.
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
//  CLASS: TCIDRTPMediaBuf
// PREFIX: rtpb
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPMediaBuf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTPMediaBuf
        (
            const   tCIDLib::TCard4         c4PayloadSz = 0
        );

        TCIDRTPMediaBuf(const TCIDRTPMediaBuf&) = delete;

        ~TCIDRTPMediaBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTPMediaBuf& operator==(const TCIDRTPMediaBuf&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuildMsg
        (
                    TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4AtMilli
            ,       tCIDLib::TCard4&        c4AtSample
            ,       tCIDLib::TCard4&        c4HdrBytes
            ,       tCIDLib::TCard4&        c4DataBytes
            ,       tCIDLib::TCard2&        c2SeqNum
            ,       tCIDLib::TCard4&        c4MsgSz
        )   const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TBoolean       bMarker
            , const tCIDLib::TCard2         c2SeqNum
            , const tCIDLib::TCard4         c4DataBytes
            , const tCIDLib::TCard4         c4PLType
            , const tCIDLib::TCard4         c4SyncSrc
            , const tCIDLib::TCard4         c4AtMilli
            , const tCIDLib::TCard4         c4AtSample
            , const TMemBuf&                mbufData
        );


        // -------------------------------------------------------------------
        //  Public data members
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bEndOfStream;
        tCIDLib::TBoolean   m_bMarker;
        tCIDLib::TCard2     m_c2SeqNum;
        tCIDLib::TCard4     m_c4AtMilli;
        tCIDLib::TCard4     m_c4AtSample;
        tCIDLib::TCard4     m_c4BufSz;
        tCIDLib::TCard4     m_c4CSrcCnt;
        tCIDLib::TCard4     m_c4DataBytes;
        tCIDLib::TCard4     m_c4PLType;
        tCIDLib::TCard4     m_c4SyncSrc;
        THeapBuf            m_mbufData;
};

#pragma CIDLIB_POPPACK

