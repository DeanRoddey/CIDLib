//
// FILE NAME: CIDKernel_SockPinger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2012
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
//  This is the header for the CIDKernel_SockPinger.cpp file, which implements
//  the class TKrnlSockPinger. This class implements the standard Ping protocol
//  in such a way that it's convenient for the higher level TSockPinger in
//  CIDSock to use it.
//
//  You first initialize, which sets up a socket with all of the fiddly
//  stuff required, appropriately for the IP address type that the provided
//  IP address resolves to. You then start a ping, which sends the request
//  and then returns immediately. You can then call eWaitRep() to wait for
//  up to some period of time for a reply. You can do multiple calls to the
//  wait, with short waits, if you don't want to block.
//
//  The SockPinger class in CIDSock needs this sort of async scheme because it
//  has to watch for thread shutdown requests, so we can't just block for a
//  long time down here. So he typically calls the wait with fairly short
//  timeouts.
//
//  When done, you can destruct the object or call bTerminate() to close it
//  all down. At that pint the initialize call has to be made again to use it
//
//  We keep a time stamp. If the state is idle, then the stamp is the time
//  that we received that last reply (or cancelled waiting for one.) If the
//  state is waiting for reply, the stamp is the time at which we sent the
//  request, so we can calculate how long we've waited. In wait init mode
//  the stamp is 0 and has no meaning.
//
//  Most of this is implemented here at the platform independent level. The
//  TKrnlSocket class provides a special method that only we can see, which
//  will set it up for our purposes. The reset we can do via existing public
//  functionality of the socket class.
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
//   CLASS: TKrnlSockPinger
//  PREFIX: ksping
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSockPinger
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSockPinger();

        TKrnlSockPinger(const TKrnlSockPinger&) = delete;

        ~TKrnlSockPinger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSockPinger& operator=(const TKrnlSockPinger&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCancelReply
        (
            const   tCIDLib::TBoolean       bResetStamp
        );

        tCIDLib::TBoolean bInitialize
        (
            const   TKrnlIPAddr&            kipaRemote
            , const tCIDLib::TCard4         c4TTL = 128
        );

        tCIDLib::TBoolean bSendRequest();

        tCIDLib::TBoolean bTerminate();

        tCIDLib::TEncodedTime enctLastTime() const;

        tCIDSock::EPingStates eState() const;

        tCIDSock::EPingRes eWaitRep
        (
            const   tCIDLib::TCard4         c4WaitFor
            ,       tCIDLib::TCard4&        c4RepMSs
            ,       tCIDLib::TCard4&        c4RepBytes
            ,       TKrnlIPAddr&            ipaFrom
        );

        const TKrnlIPAddr& ipaTar() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2CheckSum
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4Len
        )   const;

        tCIDLib::TVoid ComputeChecksum();

        tCIDSock::EPingRes eReadReply
        (
                    tCIDLib::TCard4&        c4RepBytes
            ,       TKrnlIPAddr&            kipaFrom
        );

        tCIDLib::TVoid InitHeader();

        tCIDLib::TVoid SetSequence();

        tCIDLib::TVoid SetTTL();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bV6Target
        //      During init we remember if our target is V6 vs V4, which affects how
        //      we set up the buffer we send and such.
        //
        //  m_c2Id
        //      We get a unique id for this pinger object (from the static
        //      s_scntEqId member.) This plus our own sequence number tht we
        //      run up is good enough to insure we can id a response to our
        //      own requests.
        //
        //  m_c2SeqNum
        //      A sequence number we run up for each ping in a given session.
        //
        //  m_c4PacketLen
        //      The packet length we calculate during init.
        //
        //  m_enctLastTime
        //      If our current state is waiting for a reply, then this is the
        //      time at which we made the original request. If our curent state
        //      is idle, then it's the time that the last successful reply
        //      came in. If in wait init state, it'll be zeroed out.
        //
        //  m_eState
        //      We remember our current state here.
        //
        //  m_kipaRemote
        //      We resolve the caller's target IP address to here for later use.
        //
        //  m_pc1Buf
        //      A buffer we allocate to read the responses back into.
        //
        //  m_ksockPing
        //      The socket object we use to do the pinging.
        //
        //  m_ippnLocal
        //      The local port we ended up bound to.
        //
        //  m_pc1LocalAddr
        //  m_pc1RemAddr
        //      We have to include the local and remote address info in the ping
        //      buffer, so we get them out into raw form during init.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bV6Target;
        tCIDLib::TCard2         m_c2Id;
        tCIDLib::TCard2         m_c2SeqNum;
        tCIDLib::TCard4         m_c4LocalAddrSz;
        tCIDLib::TCard4         m_c4PacketLen;
        tCIDLib::TCard4         m_c4RemAddrSz;
        tCIDLib::TEncodedTime   m_enctLastTime;
        tCIDSock::EPingStates   m_eState;
        TKrnlIPAddr             m_kipaLocal;
        TKrnlIPAddr             m_kipaRemote;
        TKrnlSocket             m_ksockPing;
        tCIDLib::TIPPortNum     m_ippnLocal;
        tCIDLib::TCard1*        m_pc1Buf;
        tCIDLib::TCard1*        m_pc1LocalAddr;
        tCIDLib::TCard1*        m_pc1RemAddr;


        // -------------------------------------------------------------------
        //  Private, static members
        //
        //  s_scntIds
        //      A safe counter we use to provide a unique id for each pinger
        //      object. Within each object it runs up a sequence number. As long
        //      as the two are unique, that's enough and this insures they are.
        // -------------------------------------------------------------------
        static TKrnlSafeCard4Counter    s_scntIds;
};

#pragma CIDLIB_POPPACK

