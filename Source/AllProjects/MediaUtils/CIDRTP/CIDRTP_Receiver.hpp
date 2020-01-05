//
// FILE NAME: CIDRTP_Receiver.hpp
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
//  This class impleemnts an RTP receiver. So it handles receiving a single stream
//  of data from a sender.
//
//  For a receiver these threads are created to perform these jobs:
//
//  1. Watches the RTP socket for data to be available and reads those packets. Queues
//     up their data for processing. Also uses stats stored by thread 2 to periodically
//     send out receiver reports.
//  2. Depacketizes the RTP data, deals with missing data, stores stats about quality
//     that thread 1 will use to send out RTCP reports.
//  3. Spools out depacketized data to the data target, which will in turn feed it
//     to some appropriate consumer of the data to play it back.
//
//
//  Though in some specialized cases client code may create receiver objects themselves,
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

class TCIDRTPMediaBuf;

// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPRec
// PREFIX: rtptr
// ---------------------------------------------------------------------------
class CIDRTPEXP TCIDRTPRec : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDRTPRec();

        TCIDRTPRec(const TCIDRTPRec&) = delete;

        ~TCIDRTPRec();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDRTPRec& operator=(const TCIDRTPRec&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid EndSession();

        tCIDLib::TVoid StartSession
        (
            const   TIPEndPoint&            ipepRTP
            , const TIPEndPoint&            ipepRTCP
            , const TString&                strCName
            ,       TCIDRTPIOTar* const     prtpioToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TBufferQ = TSafeRefQueue<TCIDRTPMediaBuf>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Cleanup();

        tCIDLib::EExitCodes eProcThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

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

        tCIDLib::TVoid ProcessMediaBuf
        (
            const   TCIDRTPMediaBuf&        rtpbCheck
            ,       tCIDLib::TBoolean&      bFirstPacket
        );

        tCIDLib::TVoid ReadCtrlPacket
        (
                    TDatagramSocket&        sockSrc
            ,       TIPEndPoint&            ipepFrom
        );

        tCIDLib::TVoid ReadMediaPacket
        (
                    TDatagramSocket&        sockSrc
            ,       TIPEndPoint&            ipepFrom
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2LastSeqId
        //      The last sequence id we got. We use this to know when we have missed
        //      packets. We don't know what the first id is, but the reader thread
        //      will watch for the first inocming packet and only start checking
        //      after that.
        //
        //  m_c4LastTime
        //      The last time stamp we got, which lets us know how to deal with new
        //      packets and their new time stamps. Each packet isn't necessarily a
        //      new time stamp. We don't know what the first time will be, since it
        //      should be randomly selected by the sender. But the reader threa dill
        //      wait for the first msg and then start checking it.
        //
        //  m_c4Stat_XXX
        //      Some stats we maintain about incoming RTP packets. The processing
        //      thread keeps these up to date, and based on them will queue up RTCP
        //      msgs to be sent out by the reader thread.
        //
        //      'bad packet' means it's malformed in some way. Total packets counts all
        //      packets received, good or bad. The others are for valid packets, but we
        //      just can't use them for some reason.
        //
        //  m_c4SyncSrc
        //      Our synchronization source id. We just generate a random 32 bit
        //      number, and go ahead and flip it to big endian if we are currently
        //      little endian, so it's already in the form required to transmission.
        //
        //  m_colProcQ
        //      The processing queue that the reader thread dumps packets into when
        //      they are received. This queue is thread safe and the processing thread
        //      pulls the buffers off and processes them, releasing them back to the
        //      pool once it's done with them. It is non-adopting because the pool
        //      owns the packet buffers.
        //
        //  m_enctLastMsg
        //      The last time we got a message. This is used for inactivity timeout
        //      where we drop the session after a period of inactivity. Since RTCP
        //      msgs are required at some reasonable rate, even if the other side
        //      is not transmitting media we should get regular msgs.
        //
        //  m_ipepRTP
        //  m_ipepRTCP
        //      The end points that we selected (and told the server to use) for
        //      the RTP session.
        //
        //  m_prtpioOut
        //      A pointer to a RTP I/O derivative that we use to spool out the
        //      media data to be played (or stored or whatever.) We adopt this and
        //      clean it up when the session is terminated (or when we are destroyed
        //      if the client code doesn't do the right thing.)
        //
        //  m_psockRTP
        //  m_psockRTCP
        //      These are the sockets used by this stream, one for RTP and one for
        //      RTCP. They are bound locally to the requested ports since we the
        //      receiving side. They are created by StartStream() and destroyed by
        //      EndStream(). Otherwise they are only accessed by the reader thread.
        //
        //  m_strCName
        //      The canonical name we are supposed to use for this session. This is
        //      what we will send out in our receiver reports as our name in the
        //      session.
        //
        //  m_thrXXX
        //      Our three threads that read, process, and write out the received
        //      data.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2LastSeqId;
        tCIDLib::TCard4         m_c4LastTime;
        tCIDLib::TCard4         m_c4SyncSrc;
        tCIDLib::TCard4         m_c4Stat_BadPackets;
        tCIDLib::TCard4         m_c4Stat_MissingPackets;
        tCIDLib::TCard4         m_c4Stat_MediaFmt;
        tCIDLib::TCard4         m_c4Stat_OutSeqPackets;
        tCIDLib::TCard4         m_c4Stat_TotalPackets;
        tCIDLib::TCard4         m_c4Stat_WrongSrc;
        TBufferQ                m_colProcQ;
        tCIDLib::TEncodedTime   m_enctLastMsg;
        TIPEndPoint             m_ipepRTP;
        TIPEndPoint             m_ipepRTCP;
        TCIDRTPIOTar*           m_prtpioOut;
        TServerDatagramSocket*  m_psockRTP;
        TServerDatagramSocket*  m_psockRTCP;
        TString                 m_strCName;
        TThread                 m_thrProcess;
        TThread                 m_thrReader;
        TThread                 m_thrWriter;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDRTPRec,TObject)
};

#pragma CIDLIB_POPPACK


