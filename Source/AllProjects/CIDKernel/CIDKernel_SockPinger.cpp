//
// FILE NAME: CIDKernel_SockPinger.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/13/2019
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
//  This file implements the TKrnlSockPinger class. This was moved up to the
//  platform independent layer to reduce the amount of per-platform code. With
//  just a specialize init method on the socket class provided per-platform,
//  we can handle the rest here.
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
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  We need some ICMP and PING oriented structures and constants. Make sure it's
//  byte packed.
// ---------------------------------------------------------------------------
#pragma CIDLIB_PACK(1)

// IPv4 header
struct ip_hdr
{
    unsigned char  ip_verlen;
    unsigned char  ip_tos;
    unsigned short ip_totallength;
    unsigned short ip_id;
    unsigned short ip_offset;
    unsigned char  ip_ttl;
    unsigned char  ip_protocol;
    unsigned short ip_checksum;
    unsigned int   ip_srcaddr;
    unsigned int   ip_destaddr;
};


// IPv4 option header
struct ipv4_option_hdr
{
    unsigned char   opt_code;
    unsigned char   opt_len;
    unsigned char   opt_ptr;
    unsigned long   opt_addr[9];
};


// IPv4 ICMP header
struct icmp_hdr
{
    unsigned char   icmp_type;
    unsigned char   icmp_code;
    unsigned short  icmp_checksum;
};


// IPv4 Echo request header (ICMP header plus echo header stuff)
struct icmp_echohdr
{
    unsigned char   icmp_type;
    unsigned char   icmp_code;
    unsigned short  icmp_checksum;
    unsigned short  icmp_id;
    unsigned short  icmp_sequence;
};


// IPv6 protocol header
struct ipv6_hdr
{
    unsigned long   ipv6_vertcflow;
    unsigned short  ipv6_payloadlen;
    unsigned char   ipv6_nexthdr;
    unsigned char   ipv6_hoplimit;
    in6_addr        ipv6_srcaddr;
    in6_addr        ipv6_destaddr;
};


// IPv6 fragment header
struct ipv6_fragment_hdr
{
    unsigned char   ipv6_frag_nexthdr;
    unsigned char   ipv6_frag_reserved;
    unsigned short  ipv6_frag_offset;
    unsigned long   ipv6_frag_id;
};


// ICMPv6 header
struct icmpv6_hdr
{
    unsigned char   icmp6_type;
    unsigned char   icmp6_code;
    unsigned short  icmp6_checksum;
};


// ICMPv6 echo header (ICMP header plus ECHO header stuff)
struct icmpv6_echohdr
{
    unsigned char   icmp6_type;
    unsigned char   icmp6_code;
    unsigned short  icmp6_checksum;
    unsigned short  icmp6_id;
    unsigned short  icmp6_sequence;
};


// IPv4 option for record route
#define IP_RECORD_ROUTE             0x7


// ICMP types and codes
#define ICMPV4_ECHO_REQUEST_TYPE    8
#define ICMPV4_ECHO_REQUEST_CODE    0
#define ICMPV4_ECHO_REPLY_TYPE      0
#define ICMPV4_ECHO_REPLY_CODE      0
#define ICMPV4_MINIMUM_HEADER       8


// ICPM6 types and codes
#define ICMPV6_ECHO_REQUEST_TYPE    128
#define ICMPV6_ECHO_REQUEST_CODE    0
#define ICMPV6_ECHO_REPLY_TYPE      129
#define ICMPV6_ECHO_REPLY_CODE      0


// Error msg types
#define ICMPV4_ERROR_REPLY_TYPE     3
#define ICMPV6_ERROR_REPLY_TYPE     1

#pragma CIDLIB_POPPACK




// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_SockPinger_Win32
{
    //
    //  The bytes we subtract from the received data to get the count of actual
    //  echo data bytes we received. There's no ip header included in the response
    //  under V6.
    //
    constexpr tCIDLib::TCard4 c4V4NonData(sizeof(ip_hdr) + sizeof(icmp_echohdr));
    constexpr tCIDLib::TCard4 c4V6NonData(sizeof(icmpv6_echohdr));

    // The number of payload bytes we send in the ping
    constexpr tCIDLib::TCard4 c4PLBytes(32);

    // The fill byte we set the echo payload bytes to
    constexpr tCIDLib::TCard1 c1PLData(0x45);
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSockPinger
//  PREFIX: ksping
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSockPinger::TKrnlSockPinger() :

    m_c2Id(0)
    , m_c2SeqNum(1)
    , m_c4LocalAddrSz(0)
    , m_c4PacketLen(0)
    , m_c4RemAddrSz(0)
    , m_enctLastTime(0)
    , m_eState(tCIDSock::EPingStates::WaitInit)
    , m_pc1Buf(nullptr)
    , m_ippnLocal(0)
    , m_pc1LocalAddr(nullptr)
    , m_pc1RemAddr(nullptr)
{
    //
    //  Get a unique id for this pinger object. Wrap it if it gets close to
    //  64K. Wrap it early enough that there's no danger of overrun. There's some
    //  very small chance of a run condition where two threads would separately
    //  reset it and two subequent threads would be get 1, but it's so unlikely
    //  it's not worth worrying about.
    //
    //  And of course our safe pointer is signed as well, and keeping the values
    //  small means we never have to worry about it wrapping.
    //
    const tCIDLib::TCard4 c4Id = s_scntIds.c4Inc();
    if (c4Id >= 0xFFA0)
        s_scntIds.c4SetValue(1);
    m_c2Id = tCIDLib::TCard2(c4Id);
}

TKrnlSockPinger::~TKrnlSockPinger()
{
    // Call our own termination method to make sure all is cleaned up
    bTerminate();
}


// ---------------------------------------------------------------------------
//  TKrnlSockPinger: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we are waiting for a reply, this cancels it by putting the state back
//  to idle. They can ask us to not reset the time stamp. They might not because
//  if they timed out, they may just want to continue counting elapsed time up
//  to another ping cycle time, and not include the possibly many seconds they
//  spent waiting for the one they are cancelling.
//
tCIDLib::TBoolean
TKrnlSockPinger::bCancelReply(const tCIDLib::TBoolean bResetStamp)
{
    // Make sure we are initialized
    if (m_eState == tCIDSock::EPingStates::WaitInit)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSPing_NotInitialized);
        return kCIDLib::False;
    }

    if (m_eState == tCIDSock::EPingStates::WaitReply)
    {
        m_eState = tCIDSock::EPingStates::Idle;

        // Reset the stamp if they asked us to
        if (bResetStamp)
            m_enctLastTime = TKrnlTimeStamp::enctNow();
    }
    return kCIDLib::True;
}


//
//  Set us up to ping a specific target address. They can optinally provide a
//  source address to bind to. if not, we do a default bind.
//
tCIDLib::TBoolean
TKrnlSockPinger::bInitialize(const  TKrnlIPAddr&        kipaRemote
                            , const tCIDLib::TCard4     c4TTL)
{
    // If we don't get out of here successfully, then call terminate
    TLambdaJan janCleanup([this]() { this->bTerminate(); });

    // Make sure we are not already initialized
    if (m_eState != tCIDSock::EPingStates::WaitInit)
    {
        // We are already initialized
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSPing_AlreadyInit);
        return kCIDLib::False;
    }

    // Store the incoming remote address info
    m_kipaRemote = kipaRemote;

    // Remember if it's a V6 type
    m_bV6Target = m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV6;

    //
    //  Set up our socket as a raw type. The protocol is 4/6 mode specific,
    //  so that's driven by the target address.
    //
    const tCIDLib::TBoolean bRes = m_ksockPing.bCreate
    (
        tCIDSock::ESocketTypes::Raw
        , m_bV6Target ? tCIDSock::ESockProtos::ICMP6 : tCIDSock::ESockProtos::ICMP
        , m_kipaRemote.eType()
    );

    //
    //  Get a local binding appropriate for the target address. Set remote port to
    //  zero.
    //
    if (!m_ksockPing.bBindForRemote(m_kipaRemote, 0))
        return kCIDLib::False;

    // Get the local address we bound to
    TKrnlIPAddr kipaLocal;
    if (!m_ksockPing.bLocalEndPoint(m_ippnLocal, m_kipaLocal))
        return kCIDLib::False;

    // Set the TTL we got
    if (!m_ksockPing.bSetSockOpt(TKrnlSocket::EISockOpts::TTL, c4TTL))
        return kCIDLib::False;

    // Calculate our packet length and allocate our data buffer
    m_c4PacketLen = 0;
    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
        m_c4PacketLen = sizeof(icmp_echohdr);
    else
        m_c4PacketLen = sizeof(icmpv6_echohdr);
    m_c4PacketLen += CIDKernel_SockPinger_Win32::c4PLBytes;
    m_pc1Buf = new tCIDLib::TCard1[m_c4PacketLen];

    // Set up the headers for the settings we have now
    InitHeader();

    //
    //  Set up some data based on src/target addresses and such, so that we don't
    //  have to repeatedly set them up. Ports don't matter here.
    //
    m_pc1LocalAddr = m_kipaLocal.pc1ToSockAddr(m_c4LocalAddrSz, 0);
    m_pc1RemAddr = m_kipaRemote.pc1ToSockAddr(m_c4RemAddrSz, 0);

    //
    //  And now put our state to idle, and set the last request time to zero,
    //  since it's been forever since our last successful response to a request.
    //
    m_eState = tCIDSock::EPingStates::Idle;
    m_enctLastTime = 0;

    // We survived, so release the cleanup janitor
    janCleanup.Orphan();

    return kCIDLib::True;
}


//
//  Set up a ping request and send it out. Set up our state to idnicate we
//  now waiting for a reply.
//
//  If we are already in waiting for reply mode, we'll still do it, on the
//  assumption that the last one timed out and the caller is trying again.
//
tCIDLib::TBoolean TKrnlSockPinger::bSendRequest()
{
    if ((m_eState != tCIDSock::EPingStates::Idle)
    &&  (m_eState != tCIDSock::EPingStates::WaitReply))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSPing_NotReqReady);
        return kCIDLib::False;
    }

    // Set the next sequence number and compute the check sum
    SetSequence();
    ComputeChecksum();

    // And now send the buffer, using a zero port
    tCIDLib::TCard4 c4Sent;
    if (!m_ksockPing.bSendTo(m_pc1Buf, m_c4PacketLen, c4Sent, m_kipaRemote, 0))
        return kCIDLib::False;

    //
    //  Put our state to waiting for a reply and set the last request time
    //  stamp to now. When we are in wait reply mode, this stamp indicates
    //  the time at which we made the request we are waiting for.
    //
    m_eState = tCIDSock::EPingStates::WaitReply;
    m_enctLastTime = TKrnlTimeStamp::enctNow();

    return kCIDLib::True;
}


//
//  Clean up and put us back to default state. We have ot be initialized again
//  before we can be reused.
//
tCIDLib::TBoolean TKrnlSockPinger::bTerminate()
{
    // IF we got the socket open, then close it first
    tCIDLib::TBoolean bOpen;
    if (m_ksockPing.bIsOpen(bOpen) && bOpen)
        m_ksockPing.bClose();

    // Clean up the ping data buffer
    delete [] m_pc1Buf;
    m_pc1Buf = nullptr;

    // Delete our address buffers
    delete [] m_pc1LocalAddr;
    m_pc1LocalAddr = nullptr;
    delete [] m_pc1RemAddr;
    m_pc1RemAddr = nullptr;

    // Make sure our state and other per-run stuff gets reset
    m_eState = tCIDSock::EPingStates::WaitInit;
    m_enctLastTime = 0;
    m_c4PacketLen = 0;

    return kCIDLib::True;
}


// Provide read access to our last timestamp
tCIDLib::TEncodedTime TKrnlSockPinger::enctLastTime() const
{
    return m_enctLastTime;
}


// Provide read access to our current state
tCIDSock::EPingStates TKrnlSockPinger::eState() const
{
    return m_eState;
}


//
//  Wait up to the indicated time for a reply to be ready. If it is we return
//  read it and verify it and return the reply time and bytes received.
//
tCIDSock::EPingRes
TKrnlSockPinger::eWaitRep(  const   tCIDLib::TCard4     c4WaitFor
                            ,       tCIDLib::TCard4&    c4RepMSs
                            ,       tCIDLib::TCard4&    c4RepBytes
                            ,       TKrnlIPAddr&        kipaFrom)
{
    // Make sure we are waiting for a reply
    if (m_eState != tCIDSock::EPingStates::WaitReply)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSPing_NotWaiting);
        return tCIDSock::EPingRes::Error;
    }

    //
    //  We need to wait for data to become available, up to the indicated amount
    //  of time. The higher level code will actually call us for short periods of
    //  time in a loop so that he can watch for shutdown requests.
    //
    tCIDLib::TBoolean bGotData;
    if (!m_ksockPing.bWaitForDataReady(bGotData, c4RepMSs * kCIDLib::enctOneMilliSec))
        return tCIDSock::EPingRes::Error;

    // If no data, we timed out, so return that status
    if (!bGotData)
        return tCIDSock::EPingRes::Timeout;

    // Let's read the reply
    tCIDSock::EPingRes eRes = eReadReply(c4RepBytes, kipaFrom);
    if (eRes == tCIDSock::EPingRes::Success)
    {
        // We got something, so reset our state
        m_eState = tCIDSock::EPingStates::Idle;

        // Return the millis between request and reception
        c4RepMSs = tCIDLib::TCard4
        (
            (TKrnlTimeStamp::enctNow() - m_enctLastTime) / kCIDLib::enctOneMilliSec
        );
    }

    m_enctLastTime = TKrnlTimeStamp::enctNow();
    return eRes;
}


// ---------------------------------------------------------------------------
//  TKrnlSockPinger: Private, static data
// ---------------------------------------------------------------------------
TKrnlSafeCard4Counter    TKrnlSockPinger::s_scntIds(1);


// ---------------------------------------------------------------------------
//  TKrnlSockPinger: Private, non-virtual methods
// ---------------------------------------------------------------------------

// The basic checksum calculation
tCIDLib::TCard2
TKrnlSockPinger::c2CheckSum(const   tCIDLib::TCard1* const  pc1Src
                            , const tCIDLib::TCard4         c4Len) const
{
    tCIDLib::TCard4         c4Sum = 0;
    tCIDLib::TInt4          i4Cnt = tCIDLib::TInt4(c4Len);
    const tCIDLib::TCard2*  pc2Ptr = reinterpret_cast<const tCIDLib::TCard2*>(pc1Src);

    // Add all of the full buffer words to the sum
    while (i4Cnt > 1)
    {
        c4Sum += *pc2Ptr++;
        i4Cnt -= sizeof(tCIDLib::TCard2);
    }

    // If there's a partial at the end, then add it
    if (i4Cnt)
        c4Sum += *pc1Src;

    c4Sum = (c4Sum >> 16) + (c4Sum & 0xFFFF);
    c4Sum += (c4Sum >> 16);

    // Flip the bits and take the low half
    return (tCIDLib::TCard2)(~c4Sum);
}


// Sets up the checksum for our next outgoing packet
tCIDLib::TVoid TKrnlSockPinger::ComputeChecksum()
{
    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        icmp_hdr* pHdr = (icmp_hdr*)m_pc1Buf;

        //
        //  Zero the checksum before we calculate it. SETTING IT TO ZERO FIRST
        //  is not redundant. It must be done.
        //
        pHdr->icmp_checksum  = 0;
        pHdr->icmp_checksum = c2CheckSum(m_pc1Buf, m_c4PacketLen);
    }
     else
    {
        tCIDLib::TCard1     ac1Buf[4096];
        tCIDLib::TCard1*    pc1BufPtr = ac1Buf;
        tCIDLib::TCard4     c4Total = 0;

        // Copy source address
        TRawMem::CopyMemBuf(pc1BufPtr, &m_pc1LocalAddr, m_c4LocalAddrSz);
        pc1BufPtr += m_c4LocalAddrSz;
        c4Total += m_c4LocalAddrSz;

        // Copy destination address
        TRawMem::CopyMemBuf(pc1BufPtr, &m_pc1RemAddr, m_c4RemAddrSz);
        pc1BufPtr += m_c4RemAddrSz;
        c4Total += m_c4RemAddrSz;

        // Copy ICMP packet length
        tCIDLib::TCard4 c4NetLen = m_c4PacketLen;
        #if CID_LITTLE_ENDIAN
        c4NetLen = TRawBits::c4SwapBytes(c4NetLen);
        #endif
        TRawMem::CopyMemBuf(pc1BufPtr, &c4NetLen, sizeof(c4NetLen));
        pc1BufPtr += sizeof(c4NetLen);
        c4Total += sizeof(c4NetLen);

        // Zero the next 3 bytes
        TRawMem::SetMemBuf(pc1BufPtr, tCIDLib::TCard1(0), 3);
        pc1BufPtr += 3;
        c4Total += 3;

        // Copy next hop header (IPPROTO_ICMP)
        *pc1BufPtr = tCIDLib::TCard1(58);
        pc1BufPtr++;
        c4Total++;

        // Copy the ICMP header and payload
        TRawMem::CopyMemBuf(pc1BufPtr, m_pc1Buf, m_c4PacketLen);
        pc1BufPtr += m_c4PacketLen;
        c4Total += m_c4PacketLen;

        // And zero pad to nearest 64 bit boundary
        for(tCIDLib::TCard4 c4Index = 0; c4Index < m_c4PacketLen % 2 ; c4Index++)
        {
            *pc1BufPtr++ = 0;
            c4Total++;
        }

        icmpv6_hdr* pHdr = (icmpv6_hdr*)m_pc1Buf;

        //
        //  Zero the checksum before we calculate it. SETTING IT TO ZERO FIRST
        //  is not redundant. It must be done.
        //
        pHdr->icmp6_checksum = 0;
        pHdr->icmp6_checksum = c2CheckSum(ac1Buf, c4Total);
    }
}


//
//  We assume the caller has determined that data is available. And it should be
//  a ping packet unless something is really wrong. We read the available data
//  into m_pc1Buf and return the bytes and the source address
//
tCIDSock::EPingRes
TKrnlSockPinger::eReadReply(tCIDLib::TCard4& c4RepBytes, TKrnlIPAddr& kipaFrom)
{
    // A buffer way bigger enough
    constexpr tCIDLib::TCard4 c4MaxBufSize(4096);
    tCIDLib::TCard1 ac1InBuf[c4MaxBufSize];

    tCIDLib::TIPPortNum ippnFrom;
    const tCIDLib::TBoolean bRes = m_ksockPing.bReceiveFrom
    (
        ac1InBuf, c4MaxBufSize, c4RepBytes, kipaFrom, ippnFrom
    );

    if (!bRes)
        return tCIDSock::EPingRes::Error;


    // Figure out the msg type
    tCIDLib::TCard1     c1MsgType;
    tCIDLib::TCard2     c2MsgId;
    tCIDLib::TCard2     c2MsgSeq;
    tCIDLib::TCard1*    pc1Data = nullptr;

    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        icmp_echohdr* pHdr = reinterpret_cast<icmp_echohdr*>(ac1InBuf + sizeof(ip_hdr));
        c1MsgType = pHdr->icmp_type;

        //
        //  If not a V4 echo response, then we are done. If it's an error message
        //  then return error, else timeout.
        //
        if (c1MsgType != ICMPV4_ECHO_REPLY_TYPE)
        {
            if (c1MsgType == ICMPV4_ERROR_REPLY_TYPE)
                return tCIDSock::EPingRes::Error;
            return tCIDSock::EPingRes::Timeout;
        }

        c2MsgId = pHdr->icmp_id;
        c2MsgSeq = pHdr->icmp_sequence;
        pc1Data = ac1InBuf + (sizeof(ip_hdr) + sizeof(icmp_echohdr));
    }
     else
    {
        // There's no IP header in this case, just the echo response
        icmpv6_echohdr* pHdr = reinterpret_cast<icmpv6_echohdr*>(ac1InBuf);
        c1MsgType = pHdr->icmp6_type;

        //
        //  If not a V6 echo response, then we are done. If it's an error message
        //  then return error, else timeout.
        //
        if (c1MsgType != ICMPV6_ECHO_REPLY_TYPE)
        {
            if (c1MsgType == ICMPV4_ERROR_REPLY_TYPE)
                return tCIDSock::EPingRes::Error;
            return tCIDSock::EPingRes::Timeout;
        }

        c2MsgId = pHdr->icmp6_id;
        c2MsgSeq = pHdr->icmp6_sequence;
        pc1Data = ac1InBuf + sizeof(icmpv6_echohdr);
    }

    //
    //  Make sure it's for our current ping. If not, return timeout so that we
    //  keep looking until time is up.
    //
    if ((c2MsgId != m_c2Id) || (c2MsgSeq != m_c2SeqNum))
        return tCIDSock::EPingRes::Timeout;

    //
    //  It seems to be an echo response, so check the number of reply bytes we
    //  got back and return that count as well, if we end up returning success.
    //
    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        if (c4RepBytes > CIDKernel_SockPinger_Win32::c4V4NonData)
            c4RepBytes = c4RepBytes - CIDKernel_SockPinger_Win32::c4V4NonData;
    }
     else
    {
        if (c4RepBytes > CIDKernel_SockPinger_Win32::c4V6NonData)
            c4RepBytes = c4RepBytes - CIDKernel_SockPinger_Win32::c4V6NonData;
    }

    if (c4RepBytes != CIDKernel_SockPinger_Win32::c4PLBytes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSPing_BadCntRecv);
        return tCIDSock::EPingRes::Error;
    }

    // Check that the returned bytes have the values we sent out
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RepBytes; c4Index++)
    {
        if (*pc1Data != CIDKernel_SockPinger_Win32::c1PLData)
        {
            //
            //  We almost got a correct reply, so assume something just got
            //  munged and return timeout to try again if there's time left.
            //
            return tCIDSock::EPingRes::Timeout;
        }
        pc1Data++;
    }

    return tCIDSock::EPingRes::Success;
}


//
//  Called from the init to set up the headers appropriately for the protocol,
//  data size, etc... The sequence numbers we leave zero since they will be set
//  on each new ping, via a running counter, and the checksum which has to be
//  done at the end after it's all set up and ready to send.
//
tCIDLib::TVoid TKrnlSockPinger::InitHeader()
{
    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        icmp_echohdr* pHdr = (icmp_echohdr*)m_pc1Buf;

        pHdr->icmp_type      = ICMPV4_ECHO_REQUEST_TYPE;
        pHdr->icmp_code      = ICMPV4_ECHO_REQUEST_CODE;
        pHdr->icmp_id        = m_c2Id;
        pHdr->icmp_checksum  = 0;
        pHdr->icmp_sequence  = 0;

        tCIDLib::TCard1* pc1Data = m_pc1Buf + sizeof(icmp_echohdr);
        TRawMem::SetMemBuf
        (
            pc1Data
            , CIDKernel_SockPinger_Win32::c1PLData
            , CIDKernel_SockPinger_Win32::c4PLBytes
        );
    }
     else
    {
        icmpv6_echohdr* pHdr = (icmpv6_echohdr*)m_pc1Buf;

        // Initialize the ICMP6 header fields
        pHdr->icmp6_type     = ICMPV6_ECHO_REQUEST_TYPE;
        pHdr->icmp6_code     = ICMPV6_ECHO_REQUEST_CODE;
        pHdr->icmp6_checksum = 0;

        // Initialize the echo request fields
        pHdr->icmp6_id       = m_c2Id;
        pHdr->icmp6_sequence = 0;

        tCIDLib::TCard1* pc1Data = m_pc1Buf + sizeof(icmpv6_echohdr);
        TRawMem::SetMemBuf
        (
            pc1Data
            , CIDKernel_SockPinger_Win32::c1PLData
            , CIDKernel_SockPinger_Win32::c4PLBytes
        );
    }
}

//
//  Called to set the next packet sequence
//
//  BE SURE TO USE prefix increment, so that we store the same value that's in
//  m_c2SeqNum, else we will never get match when the message comes back.
//
tCIDLib::TVoid TKrnlSockPinger::SetSequence()
{
    // If the sequence number hits 0xFFFF, bump it to 0 so we move to 1 next
    if (m_c2SeqNum == 0xFFFF)
        m_c2SeqNum++;

    if (m_kipaRemote.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        icmp_echohdr* pHdr = (icmp_echohdr*)m_pc1Buf;
        pHdr->icmp_sequence = ++m_c2SeqNum;
    }
     else
    {
        icmpv6_echohdr* phdr = (icmpv6_echohdr*)(m_pc1Buf);
        phdr->icmp6_sequence = ++m_c2SeqNum;
    }
}

