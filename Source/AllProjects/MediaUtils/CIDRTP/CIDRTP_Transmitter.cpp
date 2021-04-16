//
// FILE NAME: CIDRTP_Transmitter.cpp
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
//  This file implements an RTP stream transmitter.
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
RTTIDecls(TCIDRTPTrans,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDRTP_Transmitter
{
    namespace
    {
        const tCIDLib::TCard4 c4MaxFreeMQSz = 256;
    }
}


// ---------------------------------------------------------------------------
//  CLASS: TCIDRTPTrans
// PREFIX: rtptr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDRTPTrans: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDRTPTrans::TCIDRTPTrans() :

    m_bPlaying(kCIDLib::False)
    , m_c1PayloadType(0xFF)
    , m_c2CurSeqId(1)
    , m_c4PayloadBytes(0)
    , m_c4PayloadMSs(0)
    , m_c4PayloadSamples(0)
    , m_c4SampleBytes(0)
    , m_c4SentBytes(0)
    , m_c4SentPackets(0)
    , m_c4SyncSrc(0)
    , m_c4TotalBytes(0)
    , m_c4TotalMSs(0)
    , m_colFreeMQ(tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::EMTStates::Safe)
    , m_colProcQ(tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::EMTStates::Safe)
    , m_enctStartTime(0)
    , m_prtpioIn(nullptr)
    , m_thrReader
      (
          facCIDLib().strNextThreadName(TString(L"RTPTransReader"))
          , TMemberFunc<TCIDRTPTrans>(this, &TCIDRTPTrans::eReadThread)
      )
    , m_thrWriter
      (
          facCIDLib().strNextThreadName(TString(L"RTPTransWriter"))
          , TMemberFunc<TCIDRTPTrans>(this, &TCIDRTPTrans::eWriteThread)
      )
{
}

TCIDRTPTrans::~TCIDRTPTrans()
{
    // Just in case
    Cleanup();
}


// ---------------------------------------------------------------------------
//  TCIDRTPTrans: Public, non-virtual methods
// ---------------------------------------------------------------------------


tCIDLib::TVoid
TCIDRTPTrans::DoCmd(const   tCIDRTP::ERTSPCmds  eCmd
                    ,       tCIDLib::TCard4&    c4SequNum
                    ,       tCIDLib::TCard4&    c4RTPTime)
{
    // If we are not running, then do nothing
    if (!m_thrReader.bIsRunning())
        return;

    if (eCmd == tCIDRTP::ERTSPCmds::Play)
    {
        if (!m_bPlaying)
        {
            m_enctStartTime = TTime::enctNow();
            m_bPlaying = kCIDLib::True;

            c4SequNum = 1;
            c4RTPTime = 0;
        }
    }
     else
    {
        m_bPlaying = kCIDLib::False;
    }
}


tCIDLib::TVoid TCIDRTPTrans::EndSession()
{
    // Just call the cleanup method
    Cleanup();
}


tCIDLib::TVoid
TCIDRTPTrans::StartSession( const   TIPEndPoint&        ipepTarRTP
                            , const TIPEndPoint&        ipepTarRTCP
                            , const tCIDLib::TIPPortNum ippnOurRTP
                            , const tCIDLib::TIPPortNum ippnOurRTCP
                            , const TString&            strCName
                            ,       TCIDRTPIOSrc* const prtpioToAdopt)
{
    // Put a janitor on the I/O object until we get it stored away
    TJanitor<TCIDRTPIOSrc> janIO(prtpioToAdopt);

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

    //
    //  Initialize it and get the full length and other info about the source
    //  media.
    //
    tCIDLib::TCard1 c1PayloadType;
    tCIDLib::TCard4 c4PayloadMSs;
    tCIDLib::TCard4 c4PayloadBytes;
    tCIDLib::TCard4 c4PayloadSamples;
    tCIDLib::TCard4 c4SampleBytes;
    tCIDLib::TCard4 c4TotalMSs;
    tCIDLib::TCard4 c4TotalBytes;
    try
    {
        prtpioToAdopt->Initialize
        (
            c1PayloadType
            , c4PayloadMSs
            , c4PayloadBytes
            , c4PayloadSamples
            , c4SampleBytes
            , c4TotalMSs
            , c4TotalBytes
        );
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
    //  Set up our queues. We need to fill the free queues up to get them
    //  ready. We pre-allocate the buffers of the media buffers to the max
    //  payload size reported by the I/O object.
    //
    //
    m_colFreeMQ.RemoveAll();
    m_colProcQ.RemoveAll();

    tCIDLib::TCard4 c4Count = CIDRTP_Transmitter::c4MaxFreeMQSz;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colFreeMQ.Add(new TCIDRTPMediaBuf(c4PayloadBytes));


    // Save the info and then start up the threads
    m_bPlaying          = kCIDLib::False;
    m_c1PayloadType     = c1PayloadType;
    m_c2CurSeqId        = 1;
    m_c4PayloadBytes    = c4PayloadBytes;
    m_c4PayloadMSs      = c4PayloadMSs;
    m_c4PayloadSamples  = c4PayloadSamples;
    m_c4SampleBytes     = c4SampleBytes;
    m_c4SentBytes       = 0;
    m_c4SentPackets     = 0;
    m_c4TotalBytes      = c4TotalBytes;
    m_c4TotalMSs        = c4TotalMSs;
    m_enctStartTime     = TTime::enctNow();
    m_ipepTarRTP        = ipepTarRTP;
    m_ipepTarRTCP       = ipepTarRTCP;
    m_prtpioIn          = janIO.pobjOrphan();


    // Create a simple random sync source
    tCIDLib::TCard8 c8Tmp = TTime::enctNow();
    m_c4SyncSrc = tCIDLib::TCard4(c8Tmp) | tCIDLib::TCard4(c8Tmp >> 32);

    //
    //  OK, create our UDP sockets. We want to bind them to our selected local
    //  port, but be sure that they get bound on an interface that is good for
    //  the target address. So we use the BindForRemote call, passing in the target
    //  address.
    //
    try
    {
        m_sockRTP.Open(tCIDSock::ESockProtos::UDP, m_ipepTarRTP.eAddrType());
        m_sockRTP.BindForRemote(m_ipepTarRTP, ippnOurRTP);

        m_sockRTCP.Open(tCIDSock::ESockProtos::UDP, m_ipepTarRTCP.eAddrType());
        m_sockRTCP.BindForRemote(m_ipepTarRTCP, ippnOurRTCP);
    }

    catch(TError& errToCatch)
    {
        // Close any sockets we opened
        if (m_sockRTP.bIsOpen())
            m_sockRTP.Close();

        if (m_sockRTCP.bIsOpen())
            m_sockRTCP.Close();

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
    //  Now we can start up our threads. Doesn't really matter what order, since
    //  they only connect via the processing queue, but start with the reader
    //  and let him start queuing up data first.
    //
    m_thrReader.Start();
    m_thrWriter.Start();
}



// ---------------------------------------------------------------------------
//  TCIDRTPTrans: Protected, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TCIDRTPTrans::c4BuildRTCPHeader(const   tCIDLib::TCard1     c1Type
                                ,       tCIDLib::TCard1*&   pc1Buf
                                , const tCIDLib::TCard1     c1Count)
{
    // Remember the original pointer
    const tCIDLib::TCard1* const pc1Org = pc1Buf;

    // If a bye, put in our 1 sync source count
    *pc1Buf = tCIDLib::TCard1(0x2) << 6;
    *pc1Buf |= c1Count;
    pc1Buf++;

    // The type is the only thing in the second byte
    *pc1Buf = c1Type;
    pc1Buf++;

    //
    //  Calc the length, which is number of 32 bit chunks minus 1.
    //
    tCIDLib::TCard2 c2Len = 0;
    switch(c1Type)
    {
        case 200 :
            c2Len = (7 + (c1Count * 6)) - 1;
            break;

        case 203 :
            c2Len = (2 + c1Count) - 1;
            break;

        default :
        {
            CIDAssert2(L"Unknown RTCP msg type");
            break;
        }
    };

    // Put in the two length bytes
    *pc1Buf = tCIDLib::TCard1(c2Len >> 8);
    pc1Buf++;
    *pc1Buf = tCIDLib::TCard1(c2Len);
    pc1Buf++;

    // Return the bytes we added
    return tCIDLib::TCard4(pc1Buf - pc1Org);
}


tCIDLib::TCard4
TCIDRTPTrans::c4BuildRTCPMsg(const  tCIDLib::TCard4     c4PackCnt
                            , const tCIDLib::TCard4     c4ByteCnt
                            , const tCIDLib::TBoolean   bEndOfStream
                            ,       TMemBuf&            mbufTar)
{
    tCIDLib::TCard1* pc1Tar = mbufTar.pc1Data();
    const tCIDLib::TCard1* const pc1Org = pc1Tar;

    // Set up the first header for a sender report
    c4BuildRTCPHeader(200, pc1Tar, 0);

    // Get the current time as an NTP time stamp, in network order
    tCIDLib::TCard4 c4NTPSecs, c4NTPFract;
    TTime::CurNTPTime(c4NTPSecs, c4NTPFract, kCIDLib::True);

    // And insert the data
    tCIDLib::TCard4* pc4Tmp = reinterpret_cast<tCIDLib::TCard4*>(pc1Tar);
    #if CID_LITTLE_ENDIAN
    *pc4Tmp++ = TRawBits::c4SwapBytes(m_c4SyncSrc);
    *pc4Tmp++ = c4NTPSecs;
    *pc4Tmp++ = c4NTPFract;
    *pc4Tmp++ = 0;
    *pc4Tmp++ = TRawBits::c4SwapBytes(m_c4SentPackets);
    *pc4Tmp++ = TRawBits::c4SwapBytes(m_c4SentBytes);
    #else
    *pc4Tmp++ = m_c4SyncSrc;
    *pc4Tmp++ = c4NTPSecs;
    *pc4Tmp++ = c4NTPFract;
    *pc4Tmp++ = 0;
    *pc4Tmp++ = m_c4SentPackets;
    *pc4Tmp++ = m_c4SentBytes;
    #endif

    // Get the byte pointer back again
    pc1Tar = reinterpret_cast<tCIDLib::TCard1*>(pc4Tmp);

    // If end of stream, then add a bye message
    if (bEndOfStream)
    {
        c4BuildRTCPHeader(203, pc1Tar, 0);

        pc4Tmp = reinterpret_cast<tCIDLib::TCard4*>(pc1Tar);
        #if CID_LITTLE_ENDIAN
        *pc4Tmp++ = TRawBits::c4SwapBytes(m_c4SyncSrc);
        #else
        *pc4Tmp++ = m_c4SyncSrc;
        #endif

        // Get the byte pointer back again
        pc1Tar = reinterpret_cast<tCIDLib::TCard1*>(pc4Tmp);
    }

    // Return the bytes we output
    return tCIDLib::TCard4(pc1Tar - pc1Org);
}


//
//  Do any required cleanup when we are shut down
//
tCIDLib::TVoid TCIDRTPTrans::Cleanup()
{
    //
    //  Start any that are still running shutting down, in parallel. Remember
    //  which ones to wait for below on the next step.
    //
    const tCIDLib::TBoolean bWaitReader = m_thrReader.bIsRunning();
    if (bWaitReader)
        m_thrReader.ReqShutdownNoSync();

    const tCIDLib::TBoolean bWaitWriter = m_thrWriter.bIsRunning();
    if (bWaitWriter)
        m_thrWriter.ReqShutdownNoSync();

    // And wait for all of them to have died
    if (bWaitReader)
    {
        try
        {
            m_thrReader.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    if (bWaitWriter)
    {
        try
        {
            m_thrWriter.eWaitForDeath(5000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    // Clean up the source. Terminate it in case it hasn't already been
    if (m_prtpioIn)
    {
        try
        {
            m_prtpioIn->Terminate();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        try
        {
            delete m_prtpioIn;
        }

        catch(...)
        {
        }

        m_prtpioIn = 0;
    }

    // Dump any items in the qeueues
    m_colFreeMQ.RemoveAll();
    m_colProcQ.RemoveAll();
}


//
//  We use the I/O object to read buffers worth of data. If there are buffers
//  available in the free media buffers list, we get one and load it up and put
//  it into the processing buffer.
//
tCIDLib::EExitCodes TCIDRTPTrans::eReadThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the caller go
    thrThis.Sync();

    #if CID_DEBUG_ON
    #define RTPREADLOG  1
    #endif

    #if defined(RTPREADLOG)
    TTextFileOutStream strmLog;
    strmLog.Open
    (
        L"RTPReadLog.Txt"
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Write
    );
    #endif

    //
    //  A heap buffer to read buffers into. We want to have the data ready before
    //  we get a buffer out of the free list.
    //
    //  And we need an index to run up to the max milliseconds, reading up to
    //  a full payload of millis each time. And we need a timestamp to send in
    //  each msg, which we run up by samples read each time.
    //
    THeapBuf        mbufRead(m_c4PayloadBytes, m_c4PayloadBytes);
    tCIDLib::TCard4 c4CurMSs = 0;
    tCIDLib::TCard4 c4CurSamples = 0;

    //
    //  Move forward until we consume all the available input. If it's open ended
    //  then the I/O object will have given back max card for the total, so we'll
    //  just go on indefinitely.
    //
    tCIDLib::TBoolean bEndOfStream = kCIDLib::False;
    while(!bEndOfStream)
    {
        try
        {
            // Watch for thread shutdown requests
            if (thrThis.bCheckShutdownRequest())
                break;

            // If we are not in play mode, then do nothing
            if (!m_bPlaying)
            {
                // If asked to shutdown while sleeping, break out
                if (!thrThis.bSleep(100))
                    break;
                continue;
            }

            //
            //  Wait for a media buffer to be available. This provides our
            //  throttling, since we will get ahead of the processing thread,
            //  and will start waiting for recyled buffers to get put back into
            //  the free queue.
            //
            TCIDRTPMediaBuf* prtpbNew = m_colFreeMQ.pobjGetNext(250, kCIDLib::False);
            if (!prtpbNew)
                continue;

            try
            {
                //
                //  If we get zero bytes back, it's end of stream, and this
                //  buffer will be marked as such. We'll fall out below.
                //
                const tCIDLib::TCard4 c4Bytes = m_prtpioIn->c4ReadSamples
                (
                    mbufRead, c4CurSamples, c4CurMSs
                );

                // Set up the buffer
                prtpbNew->Set
                (
                    kCIDLib::False
                    , m_c2CurSeqId++
                    , c4Bytes
                    , m_c1PayloadType
                    , m_c4SyncSrc
                    , c4CurMSs
                    , c4CurSamples
                    , mbufRead
                );

                #if defined(RTPREADLOG)
                strmLog << L"SeqId: " << m_c2CurSeqId
                        << L", CurSamples: " << c4CurSamples
                        << L", CurMillis: " << c4CurMSs
                        << L", PLType: " << m_c1PayloadType
                        << L", Bytes: " << c4Bytes
                        << kCIDLib::NewLn;
                #endif

                // Add this one to the processing queue
                m_colProcQ.Add(prtpbNew);

                // If end of stream, then we are done
                if (!c4Bytes)
                {
                    #if defined(RTPREADLOG)
                    strmLog << L"-----End of stream----\n";
                    #endif

                    bEndOfStream = kCIDLib::True;
                }
            }

            catch(...)
            {
                // Give the buffer back to the free list and rethrow
                m_colFreeMQ.Add(prtpbNew);
                throw;
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

    #if defined(RTPREADLOG)
    strmLog.Flush();
    #endif

    return tCIDLib::EExitCodes::Normal;
}


//
//  We dequeue data from the processing queue and transmit it, being sure not to
//  get too far ahead. We don't want to try to be too tight on the timing since
//  we can't assume our system clock is better than maybe 30'ish millis. So we
//  try to stay about 80ms ahead. So, based on the payload seconds, we spit out
//  the next available packet once now plus the payload seconds is beyond 80ms.
//
tCIDLib::EExitCodes TCIDRTPTrans::eWriteThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the caller go
    thrThis.Sync();

    #if CID_DEBUG_ON
    #define RTPWRITELOG  1
    #endif

    #if defined(RTPWRITELOG)
    TTextFileOutStream strmLog;
    strmLog.Open
    (
        L"RTPWriteLog.Txt"
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Write
    );

    strmLog << L"TarRTP: " << m_ipepTarRTP << kCIDLib::NewLn
            << L"TarRTCP: " << m_ipepTarRTCP << kCIDLib::NewLn
            << L"Payload MS: " << m_c4PayloadMSs << kCIDLib::NewLn
            << L"Payload Samples: " << m_c4PayloadSamples << kCIDLib::NewLn
            << L"Sync Src: " << m_c4SyncSrc << kCIDLib::NewLn
            << kCIDLib::NewLn;
    #endif


    //
    //  The buffer time and our packet millis, in time stamp format
    //
    const tCIDLib::TEncodedTime enctBuf(80 * kCIDLib::enctOneMilliSec);
    const tCIDLib::TEncodedTime enctPayload(20 * kCIDLib::enctOneMilliSec);

    //
    //  We need a time stamp for sending out RTCP messages. We remember the next
    //  time at which we need to do so.
    //
    tCIDLib::TEncodedTime enctNextRTCP = TTime::enctNowPlusSecs(1);

    THeapBuf mbufOut(8192, 8192);

    tCIDLib::TBoolean       bEndOfStream = kCIDLib::False;
    tCIDLib::TEncodedTime   enctCur = 0;
    while (!bEndOfStream)
    {
        TCIDRTPMediaBuf* prtpbCur = 0;
        try
        {
            // Watch for thread shutdown requests
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  Wait a while for a packet to show up in the processing queue. This
            //  provides our throttling.
            //
            prtpbCur = m_colProcQ.pobjGetNext(250, kCIDLib::False);
            if (!prtpbCur)
                continue;

            //
            //  Copy out the data first, then we can give back the buffer before
            //  we do anything that might cause an exception and leak the
            //  buffer. Zero the pointer just in case we try to ref it below.
            //
            tCIDLib::TCard2 c2SeqNum;
            tCIDLib::TCard4 c4SrcMilli;
            tCIDLib::TCard4 c4SrcSample;
            tCIDLib::TCard4 c4HdrBytes;
            tCIDLib::TCard4 c4SrcBytes;
            tCIDLib::TCard4 c4MsgSz;
            bEndOfStream = !prtpbCur->bBuildMsg
            (
                mbufOut
                , c4SrcMilli
                , c4SrcSample
                , c4HdrBytes
                , c4SrcBytes
                , c2SeqNum
                , c4MsgSz
            );
            m_colFreeMQ.Add(prtpbCur);
            prtpbCur = 0;

            //
            //  If not end of stream, then send the info
            //
            if (bEndOfStream)
            {
                #if defined(RTPWRITELOG)
                strmLog << L"\n----- End Of Stream ------\n"
                        << kCIDLib::NewLn;
                #endif

                // Fall through so that we send an RTCP Bye
            }
             else
            {
                //
                //  Figure out the difference between now and the last start time
                //
                enctCur = TTime::enctNow();
                const tCIDLib::TCard4 c4TarMS = tCIDLib::TCard4
                (
                    (enctCur - m_enctStartTime) / kCIDLib::enctOneMilliSec
                );

                #if defined(RTPWRITELOG)
                strmLog << L"Media -> Tar MSs: " << c4TarMS
                        << L", Src MSs: " << c4SrcMilli
                        << L", Src Samples: " << c4SrcSample
                        << L", Src Bytes: " << c4SrcBytes
                        << L", Msg Bytes: " << c4MsgSz
                        << L", SeqNum: " << c2SeqNum
                        << kCIDLib::NewLn;
                #endif

                if ((c4SrcMilli > c4TarMS) && (c4TarMS + 5 < c4SrcMilli))
                {
                    const tCIDLib::TCard4 c4SleepMSs((c4SrcMilli - c4TarMS));
                    if (c4SleepMSs)
                    {
                        #if defined(RTPWRITELOG)
                        strmLog   << L"---Sleeping for: " << c4SleepMSs << kCIDLib::NewLn;
                        #endif

                        if (!thrThis.bSleep(c4SleepMSs))
                            break;
                    }
                }

                m_c4SentBytes += c4SrcBytes;
                m_c4SentPackets++;

                // Send the buffer
                if (m_sockRTP.c4SendTo(m_ipepTarRTP, mbufOut, c4MsgSz) != c4MsgSz)
                {
                    #if defined(RTPWRITELOG)
                    strmLog << L"Failed to send all media bytes" << kCIDLib::NewLn;
                    #endif
                }
            }

            //
            //  If it's been five seconds since the last RTCP msg, or we have
            //  reached end of stream, then build one and send it.
            //
            enctCur = TTime::enctNow();
            if ((enctCur > enctNextRTCP) || bEndOfStream)
            {
                #if defined(RTPWRITELOG)
                strmLog << L"Ctrl -> Bytes: " << m_c4SentBytes
                        << L", Packets: " << m_c4SentPackets
                        << L", EndOfStream: " << (bEndOfStream ? L"Yes" : L"No")
                        << kCIDLib::NewLn;
                #endif

                enctNextRTCP = enctCur + (kCIDLib::enctOneSecond * 5);
                c4MsgSz = c4BuildRTCPMsg
                (
                    m_c4SentPackets, m_c4SentBytes, bEndOfStream, mbufOut
                );

                if (m_sockRTCP.c4SendTo(m_ipepTarRTCP, mbufOut, c4MsgSz) != c4MsgSz)
                {
                    #if defined(RTPWRITELOG)
                    strmLog << L"Failed to send all ctrl bytes" << kCIDLib::NewLn;
                    #endif
                }
            }
        }

        catch(TError& errToCatch)
        {
            // Just in case... give it back to the free list
            if (prtpbCur)
            {
                m_colFreeMQ.Add(prtpbCur);
                prtpbCur = 0;
            }

            if (facCIDRTP().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
            // Just in case... give it back to the free list
            if (prtpbCur)
            {
                m_colFreeMQ.Add(prtpbCur);
                prtpbCur = 0;
            }
        }
    }

    #if defined(RTPWRITELOG)
    strmLog.Flush();
    #endif

    return tCIDLib::EExitCodes::Normal;
}

