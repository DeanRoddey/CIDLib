//
// FILE NAME: CIDOrb_ClientBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/12/2012
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
//  This file defines the IDL client base class. The IDL compiler makes all
//  the client proxy classes derivatives of this class. It doesn't provide a
//  lot of external interface, but internally it is the client side ORB
//  and manages all connections to the servers, and all queued threads waiting
//  for replies.
//
//  TCmdQInfo is a small class that represents a command as it flows through
//  the client ORB. The generated client proxy classes will call into the base
//  class to get one of these from a cache. It will put it's command data into
//  the object, then dispatch it, then blocks on an event in the object. The
//  thread that fields replies will post that event when the reply to this
//  command comes in.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSrvTarget;


// ---------------------------------------------------------------------------
//   CLASS: TCmdQItem
//  PREFIX: cqi
// ---------------------------------------------------------------------------
class CIDORBEXP TCmdQItem
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCmdQItem();

        ~TCmdQItem();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SequenceId() const;

        tCIDLib::TCard4 c4SequenceId
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDOrb::ECmdStages eStage() const;

        tCIDOrb::ECmdStages eStage
        (
            const   tCIDOrb::ECmdStages     eToSet
        );

        tCIDLib::TEncodedTime enctElapsed() const;

        TMutex* pmtxLock();

        const TOrbCmd& ocmdData() const;

        TOrbCmd& ocmdData();

        tCIDLib::TVoid Reset
        (
            const   TOrbId&                 oidToSet
        );

        tCIDLib::TVoid SetStartTime();

        tCIDLib::TVoid WakeUpWaiter();

        tCIDLib::TVoid WaitFor
        (
            const   tCIDLib::TCard4         c4Millis
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_enctStart
        //      This stamp can be reset via the SetStartTime() method, so
        //      that we can time various stages of the ORB during development.
        //      The enctElapsed() returns now minus this value.
        //
        //  m_eStage
        //      The current processing stage of this command.
        //
        //  m_evWait
        //      The client thread waits on this event for whatever his timeout
        //      time is. The reply thread will post it if the reply to this
        //      command comes in.
        //
        //  m_mtxSync
        //      Multiple threads are message with this object, so we need to
        //      synchronize access to it.
        //
        //  m_ocmdData
        //      This holds the data to send on the way in, and the reply
        //      data on the way out. It also will be set with the sequence
        //      id of the command when it's queued to be sent.
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctStart;
        tCIDOrb::ECmdStages     m_eStage;
        TEvent                  m_evWait;
        TMutex                  m_mtxSync;
        TOrbCmd                 m_ocmdData;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TCmdQItem(const TCmdQItem&);
        tCIDLib::TVoid operator=(const TCmdQItem&);
};


// ---------------------------------------------------------------------------
//  TCmdQItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TCmdQItem::c4SequenceId() const
{
    return m_ocmdData.c4SequenceId();
}

inline tCIDLib::TCard4 TCmdQItem::c4SequenceId(const tCIDLib::TCard4 c4ToSet)
{
    m_ocmdData.c4SequenceId(c4ToSet);
    return c4ToSet;
}

inline tCIDOrb::ECmdStages TCmdQItem::eStage() const
{
    return m_eStage;
}

inline tCIDOrb::ECmdStages TCmdQItem::eStage(const tCIDOrb::ECmdStages eToSet)
{
    m_eStage = eToSet;
    return m_eStage;
}

inline TMutex* TCmdQItem::pmtxLock()
{
    return &m_mtxSync;
}

// Just sets our performance debugging timer to now
inline tCIDLib::TVoid TCmdQItem::SetStartTime()
{
    m_enctStart = TTime::enctNow();
}




// ---------------------------------------------------------------------------
//   CLASS: TOrbClientBase
//  PREFIX: orbc
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbClientBase : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TOrbClientBase();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid SetObjId
        (
            const   TOrbObjId&              ooidSrc
            , const TString&                strNSBinding
            , const tCIDLib::TBoolean       bCheck = kCIDLib::True
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckForLostConnection();

        tCIDLib::TBoolean bCheckForLostConnection
        (
            const   TError&                 errToCheck
        );

        const TIPEndPoint& ipepServer() const;

        const TOrbObjId& ooidThis() const;

        tCIDLib::TVoid ResetProxy();

        tCIDLib::TVoid SendORBPing();


    protected :
        // --------------------------------------------------------------------
        //  Declare our friends
        // --------------------------------------------------------------------
        friend class TFacCIDOrb;


        // --------------------------------------------------------------------
        //  Protected, static methods
        // --------------------------------------------------------------------
        static tCIDLib::TVoid GiveBackCmdItem
        (
                    TCmdQItem* const        pciGiveBack
        );

        static TCmdQItem* pcqiGetCmdItem
        (
            const   TOrbId&                 oidToSet
        );


        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TOrbClientBase();

        TOrbClientBase
        (
            const   TOrbObjId&              ooidSrc
            , const TString&                strNSBinding
        );


        // --------------------------------------------------------------------
        // Protected, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid Dispatch
        (
            const   tCIDLib::TCard4         c4WaitFor
            ,       TCmdQItem* const        pcqiToUse
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TOrbClientBase(const TOrbClientBase&);
        tCIDLib::TVoid operator=(const TOrbClientBase&);


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bOnSrvWaitList
        (
            const   TIPEndPoint&            ipepServer
        );

        static tCIDLib::TVoid InitializeOrbClient();

        static TSrvTarget* psrvtFindServer
        (
            const   TIPEndPoint&            ipepServer
            , const tCIDLib::TBoolean       bReclaimScavenged
        );

        static TSrvTarget* psrvtFindServer
        (
            const   TIPEndPoint&            ipepServer
            ,       tCIDLib::TCard4&        c4Index
            , const tCIDLib::TBoolean       bReclaimScavenged
        );

        static tCIDLib::TVoid RemoveFromWaitList
        (
            const   TIPEndPoint&            ipepServer
        );

        static tCIDLib::TVoid TerminateOrbClient();


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsProxyConnected() const;

        TSrvTarget* psrvtAddSrvRef();

        tCIDLib::TVoid RemoveSrvRef();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSrvRefd
        //      We've already referenced a server object, so if our object id
        //      is set, we have to let one go before we get another.
        //
        //  m_enctNextOIDFresh
        //      We don't want to try to update the OID cache on every call,
        //      see m_strNSBinding below, since that would be wasteful. But we
        //      want to do it often enough that the object won't stale out of
        //      the cache if this proxy is being used pretty regularly. So
        //      we use this to throttle updates.
        //
        //  m_ipepSrv
        //      An IP end point object that we set up from the info in the
        //      object id. This gives us what we need to create a socket and
        //      connect to the server.
        //
        //  m_ooidThis
        //      This is the object id of this object instance.
        //
        //  m_strNSBinding
        //      If we were created from an object id gotten from the name
        //      server, then the binding can be stored on us. If so, then we
        //      will automatically refresh the binding periodically after a
        //      successful interaction with the server. We have the object id
        //      so all we need to do the refresh is the binding it would be
        //      stored under in the cache.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSrvRefd;
        tCIDLib::TEncodedTime   m_enctNextOIDFresh;
        TIPEndPoint             m_ipepSrv;
        TOrbObjId               m_ooidThis;
        TString                 m_strNSBinding;
};

#pragma CIDLIB_POPPACK

