//
// FILE NAME: CIDOrbUC_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 2/18/2001
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacCIDOrbUC, TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDOrbUC
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDOrbUC: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDOrbUC::TFacCIDOrbUC() :

    TFacility
    (
        L"CIDOrbUC"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_c8LastNSCookie(0)
    , m_enctLastNSCheck(0)
    , m_ippnNS(0)
    , m_thrRebinder
      (
        L"OrbUCAutoRebinder"
        , TMemberFunc<TFacCIDOrbUC>(this, &TFacCIDOrbUC::eBinderThread)
      )
{
    // Set up any stats cache stuff we support
    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Cl_RebindFails
        , tCIDLib::EStatItemTypes::Counter
        , m_sciRebindFailures
    );
}

TFacCIDOrbUC::~TFacCIDOrbUC()
{
    //
    //  Shut down the rebinder thread if it got started.
    //
    //  DO NOT lock or we could deadlock with the thread we are trying to stop
    //
    try
    {
        if (m_thrRebinder.bIsRunning())
        {
            m_thrRebinder.ReqShutdownSync(10000);
            m_thrRebinder.eWaitForDeath(5000);
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDOrbUC().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TFacCIDOrbUC: Public, non-virtual methods
// ---------------------------------------------------------------------------


//
//  This is a helper and optimizer, that everyone should use where possible,
//  to get objects from the name server. It does some work that allows us to
//  both keep the name server ooid fresh in the cache, and also to quickly
//  see if the name server has cycled and, if so, to flush the cache so that
//  old ids don't hang around. That call will also update a timer that's used
//  to periodically force a trip to the name server just in case, if it's
//  not happened naturally for some period of time.
//
//  We have one for where they already have a name server proxy, and another
//  where we'll create one for local use.
//
//  And we have versions of those for millisecond waits as well, since that
//  is often convenient. They all call the on emain version.
//
//
//  The thing about waitiing is that we need to have some reasonable wait time
//  in case the system is a bit slow. But, if we connect to the NS and it just
//  says, no, that binding is not there, we would often just want to give up
//  then. If we get an exception, then we can retry until the timeout completes,
//  but if the name server is there but the binding isn't set now, just give up
//  right then. So we have a 'quick test' flag on these guys. It defaults to
//  no, so you only get it if you ask for it specifically. It's used mostly for
//  things like design tools that don't want to hang up the UI if the thing is
//  not set.
//
tCIDLib::TBoolean
TFacCIDOrbUC::bGetNSObjectMS(const  TString&            strBinding
                            ,       TOrbObjId&          ooidToFill
                            , const tCIDLib::TCard4     c4Millis
                            , const tCIDLib::TBoolean   bQuickTest)
{
    return bGetNSObject
    (
        strBinding, ooidToFill, TTime::enctNowPlusMSs(c4Millis), bQuickTest
    );
}

tCIDLib::TBoolean
TFacCIDOrbUC::bGetNSObjectMS(       tCIDOrbUC::TNSrvProxy&  orbcNS
                            , const TString&                strBinding
                            ,       TOrbObjId&              ooidToFill
                            , const tCIDLib::TCard4         c4Millis
                            , const tCIDLib::TBoolean       bQuickTest)
{
    return bGetNSObject
    (
        orbcNS, strBinding, ooidToFill, TTime::enctNowPlusMSs(c4Millis), bQuickTest
    );
}

tCIDLib::TBoolean
TFacCIDOrbUC::bGetNSObject( const   TString&                strBinding
                            ,       TOrbObjId&              ooidToFill
                            , const tCIDLib::TEncodedTime   enctEndTime
                            , const tCIDLib::TBoolean       bQuickTest)
{
    //
    //  Get a name server proxy and call the other version. Conver the end
    //  time into a milliseconds so we can pass that to the name server
    //  lookup.
    //
    const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
    tCIDLib::TCard4 c4NSWait = 0;
    if (enctEndTime > enctNow)
        c4NSWait = tCIDLib::TCard4((enctEndTime - enctNow) / kCIDLib::enctOneMilliSec);
    tCIDOrbUC::TNSrvProxy orbcNS = orbcNameSrvProxy(c4NSWait);

    //
    //  Now call the other version. We'll wait however long is left over after
    //  the the name server lookup.
    //
    return bGetNSObject(orbcNS, strBinding, ooidToFill, enctEndTime, bQuickTest);
}

tCIDLib::TBoolean
TFacCIDOrbUC::bGetNSObject(         tCIDOrbUC::TNSrvProxy&  orbcNS
                            , const TString&                strBinding
                            ,       TOrbObjId&              ooidToFill
                            , const tCIDLib::TEncodedTime   enctEndTime
                            , const tCIDLib::TBoolean       bQuickTest)
{
    // If the end time is zero, then provide a default
    tCIDLib::TEncodedTime enctRealEnd = enctEndTime;
    if (!enctRealEnd)
        enctRealEnd = TTime::enctNowPlusSecs(5);

    tCIDLib::TBoolean   bOK = kCIDLib::False;
    TThread*            pthrCaller = nullptr;
    tCIDLib::TCard8     c8NSCookie = 0;
    while (!bOK)
    {
        try
        {
            //
            //  Break out if we get it or we are in quick test mode. If we get
            //  a response back, then the name server is there, so it's just
            //  that the binding is not set.
            //
            bOK = orbcNS->bGetObject(strBinding, ooidToFill, c8NSCookie);
            if (bOK || bQuickTest)
                break;
        }

        catch(...)
        {
            //
            //  If we've lost the connection, flush the object id cache
            //  and force us to start again.
            //
            if (orbcNS->bCheckForLostConnection())
                facCIDOrb().FlushOIDCache();

            throw;
        }

        // See if we have any millis left to wait
        const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
        tCIDLib::TCard4 c4MillisLeft = 0;
        if (enctNow < enctRealEnd)
        {
            c4MillisLeft = tCIDLib::TCard4
            (
                (enctRealEnd - enctNow) / kCIDLib::enctOneMilliSec
            );
        }

        // If fewer than 50MSs left, don't bother, break out with failure
        if (c4MillisLeft < 50)
            break;

        // We need to pause a bit, so get the calling thread if not already
        if (!pthrCaller)
            pthrCaller = TThread::pthrCaller();

        // Sleep up 500 milliseconds, or whatever we have left
        if (c4MillisLeft > 500)
            c4MillisLeft = 500;

        // Use the interuptable sleep and break out with failure if so
        if (!pthrCaller->bSleep(c4MillisLeft))
            break;
    }

    //
    //  Check the name server cookie to see if our cache is still good. As
    //  long as there was no exception, it returns the cookie whether we
    //  find the object id or not. This will also update the stamp that is
    //  used to force a trip to the name server if it's not happened naturally
    //  in some period of time.
    //
    //  Make sure it did get set though. It should be changed from the zero
    //  initial value.
    //
    if (c8NSCookie)
        facCIDOrb().CheckNSCookie(c8NSCookie);

    return bOK;
}


//
//  A wrapper for checking to see if a name server binding exists. Everyone
//  calls this instead of going straight to the name server, since this is
//  a reasonably common call and this lets us do OOID caching stuff.
//
//  We have one whre we already hve a name server and another that creates a
//  local one.
//
//  As with bGetNSObject() above, we get back the latest name server cookie
//  and pass that down to the core Orb facility to see if the name server
//  has cycled, and flush the OOID cache if so.
//
tCIDLib::TBoolean TFacCIDOrbUC::bNSBindingExists(const TString& strToCheck)
{
    tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy(2000);
    return bNSBindingExists(orbcNS, strToCheck);
}

tCIDLib::TBoolean
TFacCIDOrbUC::bNSBindingExists(         tCIDOrbUC::TNSrvProxy&  orbcNS
                                , const TString&                strToCheck)
{
    tCIDLib::TCard8 c8NSCookie = 0;
    tCIDLib::TBoolean bRet = kCIDLib::False;

    try
    {
        bRet = orbcNS->bBindingExists(strToCheck, c8NSCookie);
    }

    catch(...)
    {
        //
        //  If we've lost the connection, flush the object id cache
        //  and force us to start again.
        //
        if (orbcNS->bCheckForLostConnection())
            facCIDOrb().FlushOIDCache();
        throw;
    }

    if (c8NSCookie)
        facCIDOrb().CheckNSCookie(c8NSCookie);

    return bRet;
}


// Like the binding check above, but checks for a scope
tCIDLib::TBoolean
TFacCIDOrbUC::bNSScopeExists(tCIDOrbUC::TNSrvProxy& orbcNS, const TString& strToCheck)
{
    tCIDLib::TCard8 c8NSCookie = 0;
    tCIDLib::TBoolean bRet = kCIDLib::False;

    try
    {
        bRet = orbcNS->bScopeExists(strToCheck, c8NSCookie);
    }

    catch(...)
    {
        // If we've lost connection, flush the object id cache and force us to start again.
        if (orbcNS->bCheckForLostConnection())
            facCIDOrb().FlushOIDCache();
        throw;
    }

    if (c8NSCookie)
        facCIDOrb().CheckNSCookie(c8NSCookie);

    return bRet;
}


tCIDLib::TIPPortNum TFacCIDOrbUC::ippnNS() const
{
    return m_ippnNS;
}


// A helper to let the auto-rebinder increment the rebinding failures stats
tCIDLib::TVoid TFacCIDOrbUC::IncFailedRebinds()
{
    TStatsCache::c8IncCounter(m_sciRebindFailures);
}


//
//  This will initialize the client side support that is required to
//  access the name server, which almost any ORB based client will need
//  to do. If this isn't called explicitly, then it will be called the
//  first time the client tries to create a name server client proxy, so we
//  have to be atomic.
//
tCIDLib::TVoid TFacCIDOrbUC::Initialize()
{
    //
    //  They will only call if init isn't set, so lock and check it again.
    //  If still not initialized, then we are the guy, so do it.
    //
    TBaseLock lockInit;
    if (!m_atomInitDone)
    {
        //
        //  The name server end point address can be set via the
        //  CID_NSADDR environment var or /NSAddr= parameter. This
        //  info is handled for us automatically by the TSysInfo class,
        //  which checks for these and removes the /NSAddr parameter
        //  after processing it.
        //
        TString strNSAddr = TSysInfo::strNSAddr();
        m_strNSHost.Clear();
        m_ippnNS = kCIDOrbUC::ippnNameSrvDefPort;

        try
        {
            //
            //  If it's not been set, assume the local host and default
            //  port. If it has, then parse that info out and store it.
            //
            if (strNSAddr.bIsEmpty())
            {
                m_strNSHost = facCIDSock().strIPHostName();
            }
            else
            {
                // Break it into host/port if both were provided
                tCIDLib::TCard4 c4Ofs;
                if (!strNSAddr.bLastOccurrence(kCIDLib::chColon, c4Ofs))
                {
                    //
                    //  No colon, so no port, use the default port and
                    //  take the host as is.
                    //
                    m_strNSHost = strNSAddr;
                }
                else if (c4Ofs == strNSAddr.c4Length() - 1)
                {
                    //
                    //  There's a colon, but it's the last character. Use
                    //  the default port, and remove the colon.
                    //
                    m_strNSHost = strNSAddr;
                    m_strNSHost.DeleteLast();
                }
                else
                {
                    // There's a port so break them out
                    m_strNSHost = strNSAddr;
                    m_strNSHost.bSplit(strNSAddr, kCIDLib::chColon);
                    m_ippnNS = strNSAddr.c4Val();
                }
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            LogEventObj(errToCatch);

            if (strNSAddr.bIsEmpty())
            {
                facCIDOrbUC().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbUCErrs::errcNS_CantSetLocalNS
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            else
            {
                facCIDOrbUC().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbUCErrs::errcNS_BadNSEndPoint
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , strNSAddr
                );
            }
        }

        // Ok we can mark us initialized now
        m_atomInitDone.Set();
    }
}



//
//  A convenience to get a core server admin proxy, packaged in a counted
//  pointer so that it's easy to manage. We have one that provides a name
//  server proxy and one that creates one, and just calls the first
//
tCIDOrbUC::TCoreAdminProxy TFacCIDOrbUC::
orbcCoreSrvAdminProxy(          tCIDOrbUC::TNSrvProxy&  orbcNS
                        , const TString&                strBindingName
                        , const tCIDLib::TCard4         c4WaitUpTo)
{
    // Try to create the client proxy, with all the caching support and such
    TCIDCoreAdminClientProxy* porbcProxy = porbcMakeClient<TCIDCoreAdminClientProxy>
    (
        orbcNS, strBindingName, c4WaitUpTo
    );

    if (!porbcProxy)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcSrv_CAdminNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strBindingName
        );
    }

    return tCIDOrbUC::TCoreAdminProxy(porbcProxy);
}


tCIDOrbUC::TCoreAdminProxy
TFacCIDOrbUC::orbcCoreSrvAdminProxy(const   TString&        strBindingName
                                    , const tCIDLib::TCard4 c4WaitUpTo)
{
    //
    //  Try to get the name server proxy. We pass on the wait time to it
    //  since we have the same issues with it as we do with the server we
    //  are trying to get.
    //
    tCIDOrbUC::TNSrvProxy orbcNS = orbcNameSrvProxy(c4WaitUpTo);
    return orbcCoreSrvAdminProxy(orbcNS, strBindingName, c4WaitUpTo);
}


//
//  A convenience to get a log server proxy, packaged in a counted pointer
//  so that it's easy to manage.
//
tCIDOrbUC::TLSrvProxy TFacCIDOrbUC::orbcLogSrvProxy(const tCIDLib::TCard4 c4WaitUpTo)
{
    // Try to create the client proxy, with all the caching support and such
    TCIDLogSrvClientProxy* porbcProxy = porbcMakeClient<TCIDLogSrvClientProxy>
    (
        TCIDLogSrvClientProxy::strBinding, c4WaitUpTo
    );

    if (!porbcProxy)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcSrv_LSNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }
    return tCIDOrbUC::TLSrvProxy(porbcProxy);
}


//
//  A convenience to get a remote file browser proxy, packaged in a counted
//  pointer so that it's easy to manage.
//
//  The caller must provide us with a name server binding name, which we'll
//  look up and use to create a client proxy for them.
//
tCIDOrbUC::TRemBrowserProxy
TFacCIDOrbUC::orbcRemBrowserProxy(  const   TString&        strBindingName
                                    , const tCIDLib::TCard4 c4WaitUpTo)
{
    TRemBrowserClientProxy* porbcProxy = porbcMakeClient<TRemBrowserClientProxy>
    (
        strBindingName, c4WaitUpTo
    );

    if (!porbcProxy)
    {
        // We'll use a name server error here, since it says what we want
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_BindingNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strBindingName
        );
    }
    return tCIDOrbUC::TRemBrowserProxy(porbcProxy);
}


//
//  The name server proxy is a special case. Its the one object that you cannot
//  get via the name server (obviously.) So we have to provide a special API to
//  allow clients to get a name server proxy object, which it can then use to
//  register its services and/or to find others.
//
//  We will fault in an IP end point for the name server using the info
//  provided by the TSysInfo class, or if that's not been set we assume a
//  single machien configuration and use the local IP address.
//
TCIDNameSrvClientProxy*
TFacCIDOrbUC::porbcMakeNSProxy(const tCIDLib::TCard4 c4WaitUpTo)
{
    //
    //  If the NS end point hasn't been set explicitly by the client app
    //  (not usually done), or faulted in yet, we need to fault it in.
    //
    if (!m_atomInitDone)
        Initialize();

    //
    //  See if the object id is in the cache (under a faux binding we
    //  use so that we have some way to track it. If so, then let's
    //  try to create a proxy and see if it works.
    //
    TCIDNameSrvClientProxy* porbcRet = nullptr;
    TOrbObjId ooidServer;
    if (facCIDOrb().bCheckOOIDCache(TFacCIDOrb::strFauxNSBinding, ooidServer))
    {
        // It's there so try to create the proxy
        try
        {
            porbcRet = new TCIDNameSrvClientProxy(ooidServer, TFacCIDOrb::strFauxNSBinding);
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrbUC().bLogInfo() && !errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
            }

            // It failed, so flush this guy from the cache
            porbcRet = nullptr;
            facCIDOrb().RemoveFromOIDCache(TFacCIDOrb::strFauxNSBinding);
        }
    }

    // If we created one, then return it and we are done
    if (porbcRet)
        return porbcRet;

    //
    //  Not available in the cache or bad, so let's create the special
    //  object id for the name server.
    //
    //  NOTE: Have to create a temp and then assign it, or the compiler
    //  will insist on using a move operator even though there's an
    //  assignment operator available!
    //
    {
        TOrbObjId ooidTmp
        (
            TCIDNameSrvClientProxy::strInterfaceId
            , kCIDOrbUC::pszNSInstanceId
            , m_strNSHost
            , m_ippnNS
            , L"TCIDNameSrvClientProxy"
        );
        ooidServer = ooidTmp;
    }

    //
    //  Calc the end time where we'll give up trying to connect. If they
    //  don't give a specific value, we use a short one, otherwise we'll
    //  make programs very unresponsive if the master server cycles.
    //
    const tCIDLib::TCard8 enctEnd
    (
        TTime::enctNow() + (c4WaitUpTo ? (c4WaitUpTo * kCIDLib::enctOneMilliSec)
                                       : kCIDLib::enctTwoSeconds)
    );

    //
    //  Create a proxy for us to try to set up. Put a janitor on it until
    //  we know it's good and give it back.
    //
    porbcRet = new TCIDNameSrvClientProxy;
    TJanitor<TCIDNameSrvClientProxy> janProxy(porbcRet);

    // And let's try to connect ot the server until we run out of time
    tCIDLib::TBoolean   bConnected = kCIDLib::False;
    TError              errCaught;
    TThread*            pthrCaller = nullptr;
    while (!bConnected)
    {
        try
        {
            porbcRet->SetObjId(ooidServer, TFacCIDOrb::strFauxNSBinding);

            //
            //  Get the current name server cookie and store it. This will
            //  prove it's working, and get the name server cache flushed
            //  if it's out of date, and get the current cookie stored away
            //  for subsequent checks.
            //
            tCIDLib::TCard8 c8NSCookie = porbcRet->c8QueryCookie();
            facCIDOrb().CheckNSCookie(c8NSCookie);

            bConnected = kCIDLib::True;
            break;
        }

        catch(const TError& errToCatch)
        {
            // Didn't work, so save the error
            errCaught = errToCatch;
        }

        // See if we have any millis left to wait
        const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
        tCIDLib::TCard4 c4MillisLeft = 0;
        if (enctNow < enctEnd)
        {
            c4MillisLeft = tCIDLib::TCard4
            (
                (enctEnd - enctNow) / kCIDLib::enctOneMilliSec
            );
        }

        // If fewer than 50MSs left, don't bother, break out with failure
        if (c4MillisLeft < 50)
            break;

        // We need to pause a bit, so get the calling thread if not already
        if (!pthrCaller)
            pthrCaller = TThread::pthrCaller();

        // Sleep up 500 milliseconds, or whatever we have left
        if (c4MillisLeft > 500)
            c4MillisLeft = 500;

        // Use the interuptable sleep and break out with failure if so
        if (!pthrCaller->bSleep(c4MillisLeft))
            break;
    }

    //
    //  If we never got it, then throw. If its just that the server wasn't
    //  found, then translate that into a more nameserver specific error.
    //  Else, just rethrow.
    //
    if (!bConnected)
    {
        //
        //  Note that, if this happens, we flush the object id cache. This
        //  insures that no stale object ids will be in it when the name
        //  server comes back up and we'll get all new stuff.
        //
        facCIDOrb().FlushOIDCache();

        if (errCaught.bCheckEvent(  facCIDOrb().strName()
                                    , kOrbErrs::errcClient_SrvNotAvailable))
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcSrv_NSNotFound
                , errCaught.strErrText()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , m_strNSHost
                , TCardinal(m_ippnNS)
            );
        }
         else
        {
            errCaught.AddStackLevel(CID_FILE, CID_LINE);
            throw errCaught;
        }
    }

    //  We made it, so orphan the proxy and return it.
    return janProxy.pobjOrphan();
}


// And for most folks, return a NS proxy in its special counted pointer type
tCIDOrbUC::TNSrvProxy
TFacCIDOrbUC::orbcNameSrvProxy(const tCIDLib::TCard4 c4WaitUpTo)
{
    return tCIDOrbUC::TNSrvProxy(porbcMakeNSProxy(c4WaitUpTo));
}


tCIDLib::TVoid
TFacCIDOrbUC::ParseNSBindingPath(const  TString&    strToVal
                                ,       TString&    strScope
                                ,       TString&    strName)
{
    tCIDLib::TCard4 c4Ofs= kCIDLib::c4MaxCard;
    if (strToVal.bIsEmpty()
    || (strToVal.chLast() == kCIDLib::chTreeSepChar)
    || !strToVal.bLastOccurrence(kCIDLib::chTreeSepChar, c4Ofs)
    || (c4Ofs < 2))
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNS_BadBindingPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToVal
        );
    }

    // Ok, lets break it apart there
    strToVal.CopyOutSubStr(strScope, 0, c4Ofs);
    strToVal.CopyOutSubStr(strName, c4Ofs + 1);
}


const TString& TFacCIDOrbUC::strNSHost() const
{
    return m_strNSHost;
}


//
//  Stores the end point for the name server, which is a magic one that
//  we keep around and use instead of looking it up in the name server (for
//  obvious self-referential reasons.)
//
tCIDLib::TVoid
TFacCIDOrbUC::SetNSEndPoint(const   TString&            strHost
                            , const tCIDLib::TIPPortNum ippnPort)
{
    //
    //  If its not been set yet, then lock and update it and mark it as
    //  initialized. This will prevent any subsequent faulting in above
    //  in the name server proxy creation method.
    //
    if (!m_atomInitDone)
    {
        TBaseLock lockInit;
        if (!m_atomInitDone)
        {
            m_strNSHost = strHost;
            m_ippnNS = ippnPort;
            m_atomInitDone.Set();
        }
        return;
    }

    // Just store the passed info for later use
    m_strNSHost = strHost;
    m_ippnNS = ippnPort;
}


// Checks that the passed name server scope is basically well formed
tCIDLib::TVoid TFacCIDOrbUC::ValidateNSScopePath(const TString& strToVal)
{
    tCIDLib::TCard4 c4Ofs;
    if (strToVal.bIsEmpty()
    || (strToVal.chLast() == kCIDLib::chForwardSlash)
    || !strToVal.bLastOccurrence(kCIDLib::chForwardSlash, c4Ofs)
    || (c4Ofs < 2))
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNS_BadScopePath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToVal
        );
    }
}


