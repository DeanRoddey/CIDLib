//
// FILE NAME: CIDRTP_Transmitter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
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
//  This class implements an RTP transmitter. So it handles the transmission of a
//  single stream of data to another end point (or potentially to a multi-cast address
//  it doesn't know or care.)
//
//  For a transmitter we spin off these threads:
//
//  1. Services the input source, reading ahead, converting if needed, and queuing
//     up data to stay a certain amount of ahead of consumption, so that the data is
//     always there (though that may not be possible for a real time source.)
//  2. Packetizes the queued up data for RTP transmission, and queues it up again
//     for the third thread to process. Again, it stays a certain amount of ahead
//     of the next thread if it can.
//  3. Transmits the RTP packets at the appropriate times. Also watches for RTPC
//     receiver reports to arrive and adjusts itself based on performance reports.
//     Sends out the periodic minimal RTCP stuff that senders have to provide.
//
//
//  Though in some specialized cases client code may create trans objects themselves,
//  where they are talking to a known server that uses a fixed media format, mostly
//  these will be created within the context of some session setup object, e.g.
//  RTSP or SIP, in order for both sides to agree on what format to use and to
//  exchange ongoing session management info.
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
//  CLASS: TCIDRTPTrans
// PREFIX: rtpt
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPTrans : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTPTrans();

        TCIDRTPTrans(const TCIDRTPTrans&) = delete;
        TCIDRTPTrans(TCIDRTPTrans&&) = delete;

        ~TCIDRTPTrans();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTPTrans& operator=(const TCIDRTPTrans&) = delete;
        TCIDRTPTrans& operator=(TCIDRTPTrans&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoCmd
        (
            const   tCIDRTP::ERTSPCmds      eCmd
            ,       tCIDLib::TCard4&        c4SequNum
            ,       tCIDLib::TCard4&        c4RTPTime
        );

        tCIDLib::TVoid EndSession();

        tCIDLib::TVoid StartSession
        (
            const   TIPEndPoint&            ipepClRTP
            , const TIPEndPoint&            ipepClRTCP
            , const tCIDLib::TIPPortNum     ippnRTP
            , const tCIDLib::TIPPortNum     ippnRTCP
            , const TString&                strCName
            ,       TCIDRTPIOSrc* const     prtpioToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TMediaQ   = TRefQueue<TCIDRTPMediaBuf>;
        using TProcQ    = TRefQueue<TCIDRTPMediaBuf>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BuildRTCPHeader
        (
            const   tCIDLib::TCard1         c1Type
            ,       tCIDLib::TCard1*&       pc1Buf
            , const tCIDLib::TCard1         c1Count
        );

        tCIDLib::TCard4 c4BuildRTCPMsg
        (
            const   tCIDLib::TCard4         c4PackCnt
            , const tCIDLib::TCard4         c4ByteCnt
            , const tCIDLib::TBoolean       bEndOfStream
            ,       TMemBuf&                mbufTar
        );

        tCIDLib::TVoid Cleanup();

        tCIDLib::EExitCodes eReadThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eWriteThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bPlaying
        //      While the session is active, we can either be in play or pause
        //      mode. If paused, the reader thread just kills time waiting for a
        //      play command.
        //
        //  m_c1PayloadType
        //      When we initialize the I/O object, we get back the AVP static
        //      payload type.
        //
        //  m_c2CurSeqId
        //      The incrementing sequence id we send out in msgs.
        //
        //  m_c4PayloadMSs
        //  m_c4PayloadBytes
        //  m_c4PayloadSamples
        //  m_c4SampleBytes
        //  m_c4TotalMSs
        //      When the I/O source is initialized we get back this info, which
        //      we can use as necessary.
        //
        //  m_c4SyncSrc
        //      We generate this randomly during init, so for each session.
        //
        //  m_colFreeMQ
        //  m_colProcQ
        //      We use a simple free/used pool scheme for managing media buffers.
        //
        //  m_ipepTarRTP
        //  m_ipepTarRTCP
        //      The end points that the client told us to contact him on for the
        //      RTP session.
        //
        //  m_prtpioIn
        //      A pointer to a data source derivative that we use to read in
        //      source data to be sent.
        //
        //  m_sockRTP
        //  m_sockRTCP
        //      The two datagram sockets we create for our side of the session.
        //      If it's two way the other side is not our business.
        //
        //  m_thrXXX
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bPlaying;
        tCIDLib::TCard1         m_c1PayloadType;
        tCIDLib::TCard2         m_c2CurSeqId;
        tCIDLib::TCard4         m_c4PayloadBytes;
        tCIDLib::TCard4         m_c4PayloadMSs;
        tCIDLib::TCard4         m_c4PayloadSamples;
        tCIDLib::TCard4         m_c4SampleBytes;
        tCIDLib::TCard4         m_c4SentBytes;
        tCIDLib::TCard4         m_c4SentPackets;
        tCIDLib::TCard4         m_c4SyncSrc;
        tCIDLib::TCard4         m_c4TotalBytes;
        tCIDLib::TCard4         m_c4TotalMSs;
        TMediaQ                 m_colFreeMQ;
        TProcQ                  m_colProcQ;
        tCIDLib::TEncodedTime   m_enctStartTime;
        TIPEndPoint             m_ipepTarRTP;
        TIPEndPoint             m_ipepTarRTCP;
        TCIDRTPIOSrc*           m_prtpioIn;
        TServerDatagramSocket   m_sockRTP;
        TServerDatagramSocket   m_sockRTCP;
        TThread                 m_thrReader;
        TThread                 m_thrWriter;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTPTrans,TObject)
};

#pragma CIDLIB_POPPACK


