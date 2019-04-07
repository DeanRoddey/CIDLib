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
// CAVEATS/GOTCHAS:
//
//  1)  Because the stuff done here is so off the beaten path, we don't use
//      any of our own socket functionality. It's all just done low level.
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

        ~TKrnlSockPinger();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCancelReply
        (
            const   tCIDLib::TBoolean       bResetStamp
        );

        tCIDLib::TBoolean bInitialize
        (
            const   TKrnlIPAddr&            kipaTarget
        );

        tCIDLib::TBoolean bSendRequest();

        tCIDLib::TBoolean bTerminate();

        tCIDLib::TCard4 c4TTL() const;

        tCIDLib::TCard4 c4TTL
        (
            const   tCIDLib::TCard4         c4ToSet
        );

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
        //  Unimplimented constructors and operators
        // -------------------------------------------------------------------
        TKrnlSockPinger(const TKrnlSockPinger&);
        tCIDLib::TVoid operator=(const TKrnlSockPinger&);


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
        //  m_c4TTL
        //      The TTL that we set up the socket for.
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
        //  m_kipaTar
        //      We resolve the caller's target IP address to here for later use.
        //
        //  m_pc1Buf
        //      A buffer we allocate to read the responses back into.
        //
        //  m_pContext
        //      Any context data that the platform implementation requies to
        //      support this operation and maintain context. It's opaque here
        //      at this public level.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2Id;
        tCIDLib::TCard2         m_c2SeqNum;
        tCIDLib::TCard4         m_c4PacketLen;
        tCIDLib::TCard4         m_c4TTL;
        tCIDLib::TEncodedTime   m_enctLastTime;
        tCIDSock::EPingStates   m_eState;
        TKrnlIPAddr             m_kipaTar;
        tCIDLib::TCard1*        m_pc1Buf;
        tCIDLib::TVoid*         m_pContext;


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

