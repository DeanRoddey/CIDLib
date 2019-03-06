//
// FILE NAME: CIDOrbUC_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/18/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This facility class provides a lot of functionality that is required for
//  any practical ORB based program. All client programs will need to create
//  client side proxies, and in order to do that they'll need to look up the
//  object id of the server side interface which requires access to the
//  name server. Servers need to insure that their registered objects remain
//  registered with the name server, which requires regularly refreshing their
//  'binding leases' with the server.
//
//  We provide helper methods that make these both fairly easy but also insure
//  that common housekeeping can be done as well. Mostly this housekeeping has
//  to do with keeping the object id cache in the underlying CIDOrb facility
//  as correct as possible, and keeping server side objects registered with
//  the name server.
//
//  We keep a thread going that regularly pings the name server, to see if
//  it is still there and to see if it has cycled (even if it cycles too fast
//  for us to see it went away and came back.) If it sees that the server
//  has cycled it will flush the object id cache to force all object ids to
//  be loaded up again. And it will re-register any object ids that have been
//  registered with us for automatic binding.
//
//  Since it's possible to have programs that don't use the name server the
//  application must start the thread up themselves.
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
//   CLASS: TFacCIDOrbUC
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDORBUCEXP TFacCIDOrbUC : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDOrbUC();

        TFacCIDOrbUC(const TFacCIDOrbUC&) = delete;

        ~TFacCIDOrbUC();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDOrbUC& operator=(const TFacCIDOrbUC&) = delete;


        // -------------------------------------------------------------------
        //  A templatized helper to do the very common operation of trying to
        //  create a client side proxy (of the normal sort that takes no
        //  extra parameters but the object id and name server binding.) This
        //  deals with checking the cache first, trying to create a proxy and
        //  removing the cached id if that fails. If not cached or failed,
        //  it tries the name server. If it gets the proxy created it sets
        //  the name server binding on it so that the object will be cached
        //  and automatically refreshed by the ORB when successfully used.
        //
        //  We have one that takes an existing name server proxy, and another
        //  that doesn't, for efficiency.
        //
        //  These don't return any object id, which the vast majority of such
        //  uses don't require. If so, the caller can do these steps himself.
        //
        //  We almost always want to use a cached object if available, but we
        //  allow it to be disabled for those programs that want to specifically
        //  insure they are going to the name server and getting the freshest
        //  info.
        // -------------------------------------------------------------------
        template <class T> T*
        porbcMakeClient(const   TString&            strNSBinding
                        , const tCIDLib::TCard4     c4WaitMillis = 0
                        , const tCIDLib::TBoolean   bNoCache = kCIDLib::False
                        , const tCIDLib::TBoolean   bQuickTest = kCIDLib::False)
        {
            // See if it's cached first if we are allowed
            TOrbObjId ooidSrv;
            tCIDLib::TBoolean bCached = kCIDLib::False;
            if (!bNoCache)
                bCached = facCIDOrb().bCheckOOIDCache(strNSBinding, ooidSrv);

            // If cached, try to create it
            T* porbcRet = 0;
            if (bCached)
            {
                try
                {
                    porbcRet = new T(ooidSrv, strNSBinding);
                }

                catch(TError& errToCatch)
                {
                    if (facCIDOrbUC().bLogWarnings() && !errToCatch.bLogged())
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }

                    // Not found so remove from the cache and fall through
                    facCIDOrb().RemoveFromOIDCache(strNSBinding);
                }
            }

            if (!porbcRet)
            {
                const tCIDLib::TCard8 enctEnd
                (
                    TTime::enctNow()
                    + (c4WaitMillis ? (c4WaitMillis * kCIDLib::enctOneMilliSec)
                                    : kCIDLib::enctOneSecond)
                );

                //
                //  Either not cached or the cached one wouldn't work, so do a
                //  lookup. If we get it, try to create the proxy.
                //
                if (facCIDOrbUC().bGetNSObject( strNSBinding
                                                , ooidSrv
                                                , enctEnd
                                                , bQuickTest))
                {
                    try
                    {
                        porbcRet = new T(ooidSrv, strNSBinding);
                    }

                    catch(TError& errToCatch)
                    {
                        if (facCIDOrbUC().bLogWarnings() && !errToCatch.bLogged())
                        {
                            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                            TModule::LogEventObj(errToCatch);
                        }
                    }
                }
            }

            // And return the new guy or null if we failed
            return porbcRet;
        }

        template <class T> T*
        porbcMakeClient(        tCIDOrbUC::TNSrvProxy&  orbcNS
                        , const TString&                strNSBinding
                        , const tCIDLib::TCard4         c4WaitMillis = 0
                        , const tCIDLib::TBoolean       bNoCache = kCIDLib::False
                        , const tCIDLib::TBoolean       bQuickTest = kCIDLib::False)
        {
            // See if it's cached first, if we are a allowed
            TOrbObjId ooidSrv;
            tCIDLib::TBoolean bCached = kCIDLib::False;
            if (!bNoCache)
                bCached = facCIDOrb().bCheckOOIDCache(strNSBinding, ooidSrv);

            // If so, try to create it
            T* porbcRet = 0;
            if (bCached)
            {
                try
                {
                    porbcRet = new T(ooidSrv, strNSBinding);
                }

                catch(TError& errToCatch)
                {
                    if (facCIDOrbUC().bLogWarnings() && !errToCatch.bLogged())
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }

                    // Not found so remove from the cache and fall through
                    facCIDOrb().RemoveFromOIDCache(strNSBinding);
                }
            }

            if (!porbcRet)
            {
                const tCIDLib::TCard8 enctEnd
                (
                    TTime::enctNow()
                    + (c4WaitMillis ? (c4WaitMillis * kCIDLib::enctOneMilliSec)
                                    : kCIDLib::enctOneSecond)
                );

                //
                //   Either not cached or the cached one wouldn't work, so do a
                //  lookup. If we get it, try to create the proxy. If this fails
                //  we let it throw.
                //
                if (facCIDOrbUC().bGetNSObject( orbcNS
                                                , strNSBinding
                                                , ooidSrv
                                                , enctEnd
                                                , bQuickTest))
                {
                    try
                    {
                        porbcRet = new T(ooidSrv, strNSBinding);
                    }

                    catch(TError& errToCatch)
                    {
                        if (facCIDOrbUC().bLogWarnings() && !errToCatch.bLogged())
                        {
                            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                            TModule::LogEventObj(errToCatch);
                        }
                    }
                }
            }

            // And return the new guy, or null if we failed
            return porbcRet;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDeregRebindObj
        (
            const   TString&                strNodePath
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TBoolean bGetNSObject
        (
            const   TString&                strBinding
            ,       TOrbObjId&              ooidToFill
            , const tCIDLib::TEncodedTime   enctEndTime = 0
            , const tCIDLib::TBoolean       bQuickTest = kCIDLib::False
        );

        tCIDLib::TBoolean bGetNSObject
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            , const TString&                strBinding
            ,       TOrbObjId&              ooidToFill
            , const tCIDLib::TEncodedTime   enctEndTime = 0
            , const tCIDLib::TBoolean       bQuickTest = kCIDLib::False
        );

        tCIDLib::TBoolean bGetNSObjectMS
        (
            const   TString&                strBinding
            ,       TOrbObjId&              ooidToFill
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::TBoolean       bQuickTest = kCIDLib::False
        );

        tCIDLib::TBoolean bGetNSObjectMS
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            , const TString&                strBinding
            ,       TOrbObjId&              ooidToFill
            , const tCIDLib::TCard4         c4Millis
            , const tCIDLib::TBoolean       bQuickTest = kCIDLib::False
        );

        tCIDLib::TBoolean bNSBindingExists
        (
            const   TString&                strToCheck
        );

        tCIDLib::TBoolean bNSBindingExists
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            , const TString&                strToCheck
        );

        tCIDLib::TBoolean bNSScopeExists
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            , const TString&                strToCheck
        );

        tCIDLib::TVoid DoRebindCycle();

        tCIDLib::TIPPortNum ippnNS() const;

        tCIDLib::TVoid IncFailedRebinds();

        tCIDLib::TVoid Initialize();

        tCIDOrbUC::TCoreAdminProxy orbcCoreSrvAdminProxy
        (
            const   TString&                strBindingName
            , const tCIDLib::TCard4         c4WaitUpTo = 0
        );

        tCIDOrbUC::TCoreAdminProxy orbcCoreSrvAdminProxy
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            , const TString&                strBindingName
            , const tCIDLib::TCard4         c4WaitUpTo = 0
        );

        tCIDOrbUC::TLSrvProxy orbcLogSrvProxy
        (
            const   tCIDLib::TCard4         c4WaitUpTo = 0
        );

        tCIDOrbUC::TRemBrowserProxy orbcRemBrowserProxy
        (
            const   TString&                strBindingName
            , const tCIDLib::TCard4         c4WaitUpTo = 0
        );

        tCIDOrbUC::TNSrvProxy orbcNameSrvProxy
        (
            const   tCIDLib::TCard4         c4WaitUpTo = 0
        );

        TCIDNameSrvClientProxy* porbcMakeNSProxy
        (
            const   tCIDLib::TCard4         c4WaitUpTo = 0
        );

        tCIDLib::TVoid ParseNSBindingPath
        (
            const   TString&                strToVal
            ,       TString&                strScope
            ,       TString&                strName
        );

        tCIDLib::TVoid RegRebindObj
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strPath
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
        );

        tCIDLib::TVoid RegRebindObj
        (
            const   TOrbObjId&              ooidSrvObject
            , const TString&                strPath
            , const TString&                strDescription
        );

        const TString& strNSHost() const;

        tCIDLib::TVoid SetNSEndPoint
        (
            const   TString&                strHost
            , const tCIDLib::TIPPortNum     ippnPort
        );

        tCIDLib::TVoid StartRebinder();

        tCIDLib::TVoid StopRebinder() noexcept;

        tCIDLib::TVoid UpdateExtraNSVal
        (
            const   TString&                strNodePath
            , const tCIDLib::TCard4         c4Index
            , const TString&                strNewVal
        );

        tCIDLib::TVoid ValidateNSScopePath
        (
            const   TString&                strToVal
        );


    private :
        // -------------------------------------------------------------------
        //  Private class specific types
        // -------------------------------------------------------------------
        using TBindList = TVector<TNSRebindInfo>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBindAll
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
        );

        tCIDLib::TBoolean bDoBindingPass
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            ,       tCIDLib::TBoolean&      bChanges
        );

        tCIDLib::TBoolean bDoLeaseRenewal
        (
                    tCIDOrbUC::TNSrvProxy&  orbcNS
            ,       tCIDLib::TBoolean&      bChanges
        );

        tCIDLib::EExitCodes eBinderThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid LogRemoved
        (
            const   TString&                strPath
            , const tCIDLib::TErrCode       errcToLog
        );

        TNSRebindInfo* pnsrbiFind
        (
            const   TString&                strToFind
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bInitalized
        //      We have to fault in a few things.
        //
        //  m_c8LastNSCookie
        //      This is the last cookie we got from the name server. If it
        //      changes, we know the name server cycled and we rebind.
        //
        //  m_colList
        //      This is our collection of TNSRebindInfo objects. It holds the
        //      info that we need to do the bindings, and to keep up with when
        //      it's time to do them. We sort this guy by the next renewal
        //      time remaining. This way, we can just look at the top one and
        //      if it is ready, then at least one is ready to rebind.
        //
        //  m_enctLastNSCheck
        //      This is set any time we have contacted the NS and checked the
        //      cookie, directly or indirectly. If it's been a certain amount
        //      of time since we've done this, the thread will do a ping
        //      of the NS to see if the cookie has changed (i.e. the NS has
        //      cycled.)
        //
        //  m_ippnNS
        //      See m_strNSHost below. We store away the provided end point
        //      info for the name server. This is used any time we need to
        //      resolve the name server's address.
        //
        //  m_logevLast
        //      We need to log errors in the rebinder thread, but we have
        //      to be careful because of it's period nature, or we'll fill
        //      the logs. So we keep up with the last error we logged and
        //      don't relog the same error over and over.
        //
        //  m_mtxSync
        //      We need to sync access to the list of rebind objects between
        //      the rebinder and incoming register/deregister calls.
        //
        //  m_sciRebindFailures
        //      A stats cache object that is bumped any time the auto-rebinder
        //      class has a failure to rebind. WE provide a method for him to
        //      bump it.
        //
        //  m_strNSHost
        //      We fault this in with the host name for the name server that
        //      was either set explicitly by the application, gotten from the
        //      environment, or we fault one in assuming the local host and
        //      default port. This is then used thereafter as the name server
        //      end point for lookups.
        //
        //      See m_ippnNS above as well.
        //
        //  m_thrRebinder
        //      This is the rebinding thread that does all of the work.
        // -------------------------------------------------------------------
        volatile tCIDLib::TBoolean  m_bInitialized;
        tCIDLib::TCard8             m_c8LastNSCookie;
        TBindList                   m_colList;
        tCIDLib::TEncodedTime       m_enctLastNSCheck;
        tCIDLib::TIPPortNum         m_ippnNS;
        TLogEvent                   m_logevLast;
        TMutex                      m_mtxSync;
        TStatsCacheItem             m_sciRebindFailures;
        TString                     m_strNSHost;
        TThread                     m_thrRebinder;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDOrbUC,TFacility)
};

#pragma CIDLIB_POPPACK


