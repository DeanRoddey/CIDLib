//
// FILE NAME: CIDOrb_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2000
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
//  This is the header for the CIDOrb_ThisFacility.cpp file, which implements
//  the standard CIDLib style facility class for CIDOrb. It provides some helper
//  methods that various players within this facility need.
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
class TOrbClientConnMgr;
class TWorkQItemPtr;


// ---------------------------------------------------------------------------
//  CLASS: TFacCIDOrb
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDORBEXP TFacCIDOrb : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TObjIdCache = THashMap<TOrbObjId, TString, TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Public, static members
        // -------------------------------------------------------------------
        static const TString strCmd_Ping;
        static const TString strFauxNSBinding;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDOrb();

        TFacCIDOrb(const TFacCIDOrb&) = delete;

        ~TFacCIDOrb();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDOrb& operator=(const TFacCIDOrb&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckOOIDCache
        (
            const   TString&                strBindingName
            ,       TOrbObjId&              ooidToFill
        );

        tCIDLib::TBoolean bHasInterface
        (
            const   TOrbObjId&              ooidToCheck
        )   const;

        tCIDLib::TCard4 c4CmdOverhead() const
        {
            return m_c4CmdOverhead;
        }

        tCIDLib::TCard4 c4ReplyOverhead() const
        {
            return m_c4ReplyOverhead;
        }

        tCIDLib::TCard4 c4TimeoutAdjust() const;

        tCIDLib::TCard8 c8LastNSCookie() const;

        tCIDLib::TVoid CheckNSCookie
        (
            const   tCIDLib::TCard8         c8ToCheck
        );

        tCIDLib::TVoid ClearOnlyAcceptFrom();

        tCIDLib::TVoid DeregisterObject
        (
                    TOrbServerBase* const   porbsToDereg
        );

        tCIDLib::TVoid DispatchCmd
        (
            const   TString&                strCmd
            ,       TOrbCmd&                orbcToDispatch
        );

        tCIDLib::TEncodedTime enctTimeoutAdjust() const;

        tCIDOrb::EReadRes eReadPacket
        (
                    TThread&                thrCaller
            ,       TStreamSocket&          sockSrc
            ,       tCIDLib::TCard4&        c4Id
            , const tCIDLib::TCard4         c4WaitFor
            ,       tCIDLib::TCard4&        c4BytesRead
            ,       TMemBuf&                mbufToFill
        );

        tCIDOrb::EReadRes eReadPacket
        (
                    TThread&                thrCaller
            ,       TStreamSocket&          sockSrc
            ,       tCIDLib::TCard4&        c4Id
            , const tCIDLib::TCard4         c4WaitFor
            ,       TMemBuf&                mbufTmp
            ,       TBinMBufInStream&       strmTmp
            ,       TWorkQItemPtr&          wqipNew
        );

        tCIDLib::TVoid FlushOIDCache();

        tCIDLib::TIPPortNum ippnORB() const;

        tCIDLib::TVoid InitClient();

        tCIDLib::TVoid InitServer
        (
            const   tCIDLib::TIPPortNum     ippnListen
            , const tCIDLib::TCard4         c4MaxClients = 0
        );

        tCIDLib::TVoid OnlyAcceptFrom
        (
            const   TIPAddress&             ipaSource
        );

        TOrbClientBase* porbcDynamic
        (
            const   TOrbObjId&              ooidToUse
        );

        tCIDLib::TVoid RefreshObjIDCache
        (
            const   TString&                strBindingName
            , const TOrbObjId&              ooidToCheck
        );

        tCIDLib::TVoid RegisterObject
        (
                    TOrbServerBase* const   porbsToReg
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        tCIDLib::TVoid RemoveFromOIDCache
        (
            const   TString&                strBindingName
        );

        tCIDLib::TVoid RemoveFromOIDCache
        (
            const   TOrbObjId&              ooidToRemove
        );

        TString strNextSpoolThreadName
        (
            const   tCIDLib::TBoolean       bServer
        );

        tCIDLib::TVoid SendMsg
        (
                    TStreamSocket&          sockTar
            , const tCIDOrb::TPacketHdr&    hdrToSend
            , const TMemBuf&                mbufData
        );

        tCIDLib::TVoid SetEncrypter
        (
                    TBlockEncrypter* const  pcrypToAdopt
        );

        tCIDLib::TVoid StoreObjIDCache
        (
            const   TString&                strBindingName
            , const TOrbObjId&              ooidToStore
        );

        tCIDLib::TVoid Terminate();

        tCIDLib::TVoid UpdateWorkQItemStat
        (
            const   tCIDLib::TCard4         c4Count
        );


    protected :
        // -------------------------------------------------------------------
        //  The work item queue ptr is our friend, so that he can reserve and
        //  release work items.
        // -------------------------------------------------------------------
        friend class TWorkQItemPtr;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDOrb::EReadRes eReadPacketData
        (
                    TThread&                thrCaller
            ,       TStreamSocket&          sockSrc
            , const tCIDOrb::TPacketHdr&    hdrRead
            ,       tCIDLib::TCard4&        c4DataBytes
            ,       TMemBuf&                mbufToFill
        );

        tCIDOrb::EReadRes eReadPacketHdr
        (
                    TThread&                thrCaller
            ,       TStreamSocket&          sockSrc
            , const tCIDLib::TCard4         c4WaitFor
            ,       tCIDOrb::TPacketHdr&    hdrToFill
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_atomClientInit
        //  m_atomServerInit
        //      These flags indicate whether the client or server support has
        //      been initialized. It tells us what to shut down when we clean
        //      up.
        //
        //  m_c4CmdOverhead
        //  m_c4ReplyOverhead
        //      The reply and command classes contain a payload but they
        //      also have their own streamed overhead. This has to be taken
        //      into account, not for the actual streamed in/out objects,
        //      which are what they are, but for buffer management purposes,
        //      where we use a temp buffer for things larger than the std
        //      buffers, to avoid ongoing buffer creap. So these don't need
        //      to be 100% accurate, but reflect at least the amount of bytes
        //      that are required in addition to the payload in command and
        //      reply modes.
        //
        //      To keep this adaptive, we just figure it out emperically by
        //      streaming some objects during our ctor.
        //
        //  m_c4TimeoutAdjust
        //      The remote adjust time (see m_enctTimeoutAdjust below) in ms
        //      so that we won't have to keep dividing every time.
        //
        //  m_c8LastNSCookie
        //      This is the last name server cookie reported to us, by threads
        //      that have to go to the name server to get an object id. If they
        //      report a different one from what we've got, we flush the object
        //      id cache.
        //
        //  m_colObjList
        //      This is our list of server objects. Its a specialized collection class
        //      which is optimized for our needs.
        //
        //  m_colNSCache
        //      A cache of object ids that we maintain for often accessed servers.
        //
        //  m_crsAddrInfo
        //  m_crsObjList
        //      Critical sections to sync access to our object list and to
        //      the local default IP address info.
        //
        //  m_enctNextForcedNS
        //      If we have not made a trip to the name server in some length
        //      of time, then we want to force one. The name server object
        //      lookup methods in the CIDOrbUC facility will call in here to
        //      see if the name server cookie has changed. We also will update
        //      this stamp at the same time, since they will have jut done a
        //      successful trip to the name server.
        //
        //  m_enctTimeoutAdjust
        //      If the CID_ORBTIMEOUTADJUST environment variable is set, then
        //      this is set to a non-zero value. This is added to timeouts on
        //      the client and server side reads, to help keep the timeouts
        //      low normally, but to be able to adjust for remote clients or
        //      slower network connections.
        //
        //  m_poccmSrv
        //      This is the client connection manager for the server side
        //      of the ORB. It keeps up with the clients that are currently
        //      connected and provides the logic for the worker threads to
        //      find work and return results. It is a pointer because we don't
        //      need to create it unless the server side is initialized.
        //
        //  m_pcrypSecure
        //      A block encrypter can be set on us, and we'll use that to
        //      encrypt/decrypt packet data being sent back and forth. We
        //      use a pointer so that we don't force the crypto facility
        //      headers on our clients.
        //
        //  m_sciRegisteredObjs
        //      A stats cache object that we need up to date with the number
        //      of registered ORB objects on the server side.
        //
        //  m_sciWorkQItems
        //      The number of items (free and reserved) in our work queue item
        //      pool (which is in the WorkQItem file. We call a method on it
        //      periodically to update the stat.
        // -------------------------------------------------------------------
        TAtomicFlag             m_atomClientInit;
        TAtomicFlag             m_atomServerInit;
        tCIDLib::TCard4         m_c4CmdOverhead;
        tCIDLib::TCard4         m_c4ReplyOverhead;
        tCIDLib::TCard4         m_c4TimeoutAdjust;
        tCIDLib::TCard8         m_c8LastNSCookie;
        TOrbSObjList            m_colObjList;
        TObjIdCache             m_colNSCache;
        TCriticalSection        m_crsAddrInfo;
        TCriticalSection        m_crsObjList;
        tCIDLib::TEncodedTime   m_enctNextForcedNS;
        tCIDLib::TEncodedTime   m_enctTimeoutAdjust;
        TOrbClientConnMgr*      m_poccmSrv;
        TBlockEncrypter*        m_pcrypSecure;
        TStatsCacheItem         m_sciRegisteredObjs;
        TStatsCacheItem         m_sciWorkQItems;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDOrb,TFacility)
};

#pragma CIDLIB_POPPACK

