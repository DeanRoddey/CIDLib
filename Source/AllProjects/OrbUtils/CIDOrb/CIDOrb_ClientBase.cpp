//
// FILE NAME: CIDOrb_ClientBase.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/2000
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
//  This is implementation file for the base IDL client class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDOrb_.hpp"
#include    "CIDOrb_ClientBaseHelpers_.hpp"


// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------
typedef TRefVector<TSrvTarget>  TOrbCSrvList;
typedef TRefVector<TCmdQItem>   TOrbCmdItemCache;


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDOrb_ClientBase
{
    // -----------------------------------------------------------------------
    //  atomInitDone
    //      Used to handle initialization and avoiding multiple init.
    //
    //  c4OIDRefreshSecs
    //      The number of seconds we throttle between automatic refreshes
    //      of the object id cache.
    //
    //  pcolCmdCache
    //      Each queued command requires a 'cmd Q item' object to flow through
    //      the ORB, holding the command and the event that the client thread
    //      sleeps on until the response comes back. These are relatively
    //      heavy objects so we keep a cache of them and give them out to
    //      clients.
    //
    //      It is thread safe, because WE CANNOT use the overall mutex to sync
    //      it due to deadlocking potential!!
    //
    //  enctCacheTimeout
    //  pcolCache
    //      To be much more efficient in the face of client threads which
    //      follow the common pattern of creating a proxy, making a call or
    //      two, and the destroying the proxy, we don't actually drop a target
    //      server immediately upon the last proxy dereferencing it. We will
    //      put it on this list and time stamp it. It will be dropped after a
    //      short period of time (currently hard coded, but eventually
    //      perhaps settable.) But, if it is reused, it will be far more
    //      efficient, since target server objects are a bit heavy to spin
    //      up rapidly.
    //
    //  pcolConnWaitList
    //      This is a list used in psrvtAddSrvRef, to coordinate when
    //      multiple threads are trying to connect to a server at once.
    //
    //  pcolServers
    //      A collection of TSrvTarget objects for the currently active
    //      targets.
    //
    //  pmtxSync
    //      A mutex used to synchronize updates to the file level info
    //      here in this name space.
    //
    //  pthrCacheScavenger
    //      We need a thread internally to periodically scan the connection
    //      cache collection, to see if any of them have timed out and need
    //      to be removed.
    //
    //  sciXXX
    //      These are stats cache items where we echo the sies of the above
    //      collections for stats purposes. It's kind of unfortunate that
    //      we store this info in two places, but necessary. So be careful
    //      and keep them in sync.
    // -----------------------------------------------------------------------
    TAtomicFlag                 atomInitDone;
    tCIDLib::TCard4             c4OIDRefreshSecs = 5;
    const tCIDLib::TEncodedTime enctCachePeriod = 45 * kCIDLib::enctOneSecond;
    TOrbCSrvList*               pcolCache;
    TOrbCmdItemCache*           pcolCmdCache;
    TOrbCSrvList*               pcolServers;
    TConnWaitList*              pcolConnWaitList;
    TMutex*                     pmtxSync;
    TThread*                    pthrCacheScavenger = nullptr;

    TStatsCacheItem             sciCmdCache;
    TStatsCacheItem             sciSrvCache;
    TStatsCacheItem             sciSrvTargets;
    TStatsCacheItem             sciWaitList;
}


// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------

//
//  This thread manages the pcolCache list (above at file scope.) When
//  the last proxy drops a server connection (the ref count goes zero), it
//  is put on the cache list. We monitor it and throw away those that are
//  not reused within a certain time.
//
//  While it's on this list, the server object's spooler thread is still
//  running and watching for input. It won't get any since it's not in use,
//  but it will catch a drop of the connection by the server side. So it
//  could go stale while in there. We don't care if it does or not. If
//  another proxy object comes in and pulls it back out of the list, it will
//  be checked at that time to see if it's still good. If not, then it'll
//  be tossed at that point.
//
static tCIDLib::EExitCodes
eCacheScavengerThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    while (kCIDLib::True)
    {
        try
        {
            // Sleep a second, break out if we got a shutdown request
            if (!thrThis.bSleep(2000))
                break;

            //
            //  Lock the overall mutex and see if there are any items in
            //  the cache list.
            //
            {
                //
                //  Use the special form of the mutex locker that let's us
                //  provisionally lock it. If we can't, then just go back to
                //  the top, else we got it and the lock janitor will let
                //  it go normally when it goes out of scope.
                //
                TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync, kCIDLib::False);
                if (!lockSrv.bLock(250))
                    continue;

                tCIDLib::TCard4 c4Count = CIDOrb_ClientBase::pcolCache->c4ElemCount();

                // If none, then give up now
                if (!c4Count)
                    continue;

                // We have some so check them
                tCIDLib::TCard4 c4Index = 0;
                while (c4Index < c4Count)
                {
                    TSrvTarget* psrvtCur = CIDOrb_ClientBase::pcolCache->pobjAt(c4Index);

                    if (psrvtCur->m_enctCacheStamp < TTime::enctNow())
                    {
                        //
                        //  This one is toast. Do this in a try block, because
                        //  this will destroy it, and that will kick off a lot
                        //  of stuff.
                        //
                        try
                        {
                            c4Count--;
                            CIDOrb_ClientBase::pcolCache->RemoveAt(c4Index);
                            TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciSrvCache);
                        }

                        catch(TError& errToCatch)
                        {
                            if (!errToCatch.bLogged())
                            {
                                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                                TModule::LogEventObj(errToCatch);
                            }
                        }

                        catch(...)
                        {
                        }
                    }
                     else
                    {
                        c4Index++;
                    }
                }
            }
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        catch(...)
        {
        }
    }
    return tCIDLib::EExitCodes::Normal;
}



// ---------------------------------------------------------------------------
//   CLASS: TCmdQItem
//  PREFIX: cqi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCmdQItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TCmdQItem::TCmdQItem() :

    m_enctStart(0)
    , m_eStage(tCIDOrb::ECmdStages::Free)
    , m_ocmdData(1024)
{
}

TCmdQItem::~TCmdQItem()
{
}


// ---------------------------------------------------------------------------
//  TCmdQItem: Public, non-virtual methods
//
//  Some are inlined
// ---------------------------------------------------------------------------

// Returns how long since this guy was queued
tCIDLib::TEncodedTime TCmdQItem::enctElapsed() const
{
    return TTime::enctNow() - m_enctStart;
}


const TOrbCmd& TCmdQItem::ocmdData() const
{
    return m_ocmdData;
}

TOrbCmd& TCmdQItem::ocmdData()
{
    return m_ocmdData;
}


//
//  Get's us ready to be used again. It is assumed that the caller has
//  lock the overall sync mutex before calling us here and that no one
//  else could be poking at us while we do this.
//
tCIDLib::TVoid TCmdQItem::Reset(const TOrbId& oidToSet)
{
    try
    {
        m_ocmdData.SetTarget(oidToSet);
        m_ocmdData.Reset(1024);
        m_ocmdData.SetCmdMode();
        m_evWait.Reset();
        m_eStage = tCIDOrb::ECmdStages::Wait;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // It didn't work. Mark us as free and rethrow
        m_eStage = tCIDOrb::ECmdStages::Free;
        throw;
    }
}


tCIDLib::TVoid TCmdQItem::WakeUpWaiter()
{
    m_evWait.Trigger();
}


tCIDLib::TVoid TCmdQItem::WaitFor(const tCIDLib::TCard4 c4Millis)
{
    m_evWait.WaitFor(c4Millis);
}






// ---------------------------------------------------------------------------
//   CLASS: TOrbClientBase
//  PREFIX: orbc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbClientBase: Public destructor
// ---------------------------------------------------------------------------
TOrbClientBase::~TOrbClientBase()
{
    // Remove our server reference if we have one
    try
    {
        RemoveSrvRef();
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  Before a client proxy can be used, this must be called. It's generally
//  called by the IDL generated drived proxy class ctors, but sometimes the
//  default ctor is calleda nd this is called after the fact.
//
//  We try to either connect to an existing server target, or create a new
//  one, using the incoming object id info.
//
tCIDLib::TVoid TOrbClientBase::SetObjId(const   TOrbObjId&          ooidSrc
                                        , const TString&            strNSBinding
                                        , const tCIDLib::TBoolean   bCheck)
{
    //
    //  The client proxy class name stored in this object id better damn
    //  well be that of our most derived class' name, unless we were told
    //  not to check, which can happen when creating generic proxies.
    //
    if (bCheck)
    {
        if (clsIsA() != ooidSrc.strClientProxyClass())
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcOOId_WrongProxyClass
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , ooidSrc.strClientProxyClass()
                , clsThis()
            );
        }
    }

    // Remember if the incoming address is an already cached one
    const tCIDLib::TBoolean bWasCached = ooidSrc.bHasCachedAddr();

    //
    //  Try to set up the end point for this object id, which will involve
    //  resolving the host name. If the passed object already has a cached IP
    //  address, we'll use that instead of trying to create one.
    //
    //  We can't store it yet, since we need to leave any original end point
    //  in place to use for removing any previous target. But we want
    //  to do this before we lock, since if the server is not there, we don't
    //  want to hold everyone else up while the system thrashes around trying
    //  to resolve the target.
    //
    //  We don't indicate a preference for address type (IPV4 vs 6), so we'll
    //  use whatever is the default.
    //
    TIPEndPoint ipepNew;
    if (bWasCached)
    {
        ipepNew = TIPEndPoint(ooidSrc.ipaCached(), ooidSrc.ippnHost());
    }
     else
    {
        try
        {
            ipepNew = TIPEndPoint
            (
                ooidSrc.strAddrText(), ooidSrc.ippnHost(), tCIDSock::EAddrTypes::Unspec
            );
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrb().bLogWarnings() && !errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_SrvNotAvailable
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , ooidSrc
            );
        }
    }

    //
    //  If we have already referenced a server target, we have to let
    //  it go. If we are the last one, then it will be moved to the scanvenger
    /// list for potential later reuse.
    //
    if (m_bSrvRefd)
        RemoveSrvRef();

    //
    //  Store the object id and end point away now, and the name serer
    //  binding, which may be empty or not.
    //
    m_ooidThis = ooidSrc;
    m_ipepSrv = ipepNew;
    m_strNSBinding = strNSBinding;

    //
    //  If we didn't get a cached address, then store the address on our object
    //  id, for caching purposes.
    //
    if (!bWasCached)
        m_ooidThis.SetCachedAddr(ipepNew);

    //
    //  And reference a new server. It will try to create one if there isn't
    //  already one for this server. If it cannot, then give up now.
    //
    if (!psrvtAddSrvRef())
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_SrvNotAvailable
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_ipepSrv
        );
    }

    //
    //  If we got a name server binding, add this guy to the OOID cache.
    //  If the incoming object id has a cached address, then in theory it
    //  is already there and the cached object id is this one. But don't
    //  assume that's true. And if it wasn't a previously cached address we'd
    //  store it as well. So just go ahead and do it either way.
    //
    //  NOTE: Make sure we use OUR local one, not the one passed in, since
    //  we've updated ours with cache info.
    //
    if (!m_strNSBinding.bIsEmpty())
        facCIDOrb().StoreObjIDCache(m_strNSBinding, m_ooidThis);

    // Set the next object id refresh interval from now
    m_enctNextOIDFresh = TTime::enctNowPlusSecs(CIDOrb_ClientBase::c4OIDRefreshSecs);
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
//
//  These allow client code to test this client proxy to see if it's still
//  connected. The first one checks at the network level. The second one
//  takes an error returned and checks that as well, to see if it's a loss
//  at the ORB level, i.e. our target object is not there.
//
tCIDLib::TBoolean TOrbClientBase::bCheckForLostConnection()
{
    return !bIsProxyConnected();
}


tCIDLib::TBoolean
TOrbClientBase::bCheckForLostConnection(const TError& errToCheck)
{
    //
    //  If the object is no longer there on the server, the we haven't lost
    //  the actual object pipe connection, but the server object has probably
    //  been removed. It's not a loss of network connection, but that
    //  particular client proxy is hosed and should be closed.
    //
    if (errToCheck.bCheckEvent( facCIDOrb().strName()
                                , kOrbErrs::errcServ_ObjNotFound))
    {
        return kCIDLib::True;
    }
    return !bIsProxyConnected();
}


//
//  In some cases we need access to the target end point of our remote server
//  we are talking to. The caller should be sure we are connected, or this
//  could be meaningless.
//
const TIPEndPoint& TOrbClientBase::ipepServer() const
{
    return m_ipepSrv;
}


// Return the object id of our remote end point
const TOrbObjId& TOrbClientBase::ooidThis() const
{
    return m_ooidThis;
}


// Drop our server reference, disconnecting us
tCIDLib::TVoid TOrbClientBase::ResetProxy()
{
    // Remove our server reference if we have one
    RemoveSrvRef();
}


//
//  This method allows any client side proxy to be used to ping the
//  server side ORB to make sure it's still alive and kicking. It doesn't
//  go to the target object, but is handled by the server side ORB.
//
tCIDLib::TVoid TOrbClientBase::SendORBPing()
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(m_ooidThis.oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TFacCIDOrb::strCmd_Ping
                            << tCIDLib::TCard4(0xFAFEFAD);
        Dispatch(10000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        tCIDLib::TCard4 c4Val;
        ocmdToUse.strmIn() >> c4Val;
        if (c4Val != 0xFAFEFAD)
        {
            // Probably should do something here
        }
        GiveBackCmdItem(pcqiToUse);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        GiveBackCmdItem(pcqiToUse);
        throw;
    }
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Protected, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbClientBase::GiveBackCmdItem(TCmdQItem* const pcqiGiveBack)
{
    // Lock the object, so we can be sure of the stage
    TMtxLocker lockCmd(pcqiGiveBack->pmtxLock());

    switch(pcqiGiveBack->eStage())
    {
        case tCIDOrb::ECmdStages::CmdQ :
        case tCIDOrb::ECmdStages::ReplyList :
            //
            //  Mark it as orphaned. When the write thread gets to it, it
            //  will just mark it available again. If it's in the reply list,
            //  then it should have been removed by the client thread when
            //  it gave up, but just in case, we mark it as orphaned also,
            //  and the reply thread will periodically flush them.
            //
            pcqiGiveBack->eStage(tCIDOrb::ECmdStages::Orphaned);
            break;

        case tCIDOrb::ECmdStages::Wait :
        case tCIDOrb::ECmdStages::Ready :
        case tCIDOrb::ECmdStages::Orphaned :
            // Just mark it as free now
            pcqiGiveBack->eStage(tCIDOrb::ECmdStages::Free);
            break;

        default :
        case tCIDOrb::ECmdStages::Free :
            // Do nothing
            break;
    };
}


TCmdQItem* TOrbClientBase::pcqiGetCmdItem(const TOrbId& oidToSet)
{
    // Lock the collection to sync this non-atomic op
    TMtxLocker lockSrv(CIDOrb_ClientBase::pcolCmdCache->pmtxLock());

    //
    //  And search the cache for a free cmd item. If none are found, then
    //  add a new one.
    //
    const tCIDLib::TCard4 c4Count = CIDOrb_ClientBase::pcolCmdCache->c4ElemCount();
    tCIDLib::TCard4 c4Index;
    TCmdQItem* pcqiCur = nullptr;
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        pcqiCur = CIDOrb_ClientBase::pcolCmdCache->pobjAt(c4Index);
        if (pcqiCur->eStage() == tCIDOrb::ECmdStages::Free)
            break;
    }

    //
    //  If no free ones, then add one. Don't allow us to go crazy. If we
    //  get up to 128, return an error that says we cannot handle any more
    //  requests right now.
    //
    if (c4Index == c4Count)
    {
        if (c4Count >= 128)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_TooManyQReqs
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
            );
        }

        pcqiCur = new TCmdQItem;
        CIDOrb_ClientBase::pcolCmdCache->Add(pcqiCur);
        TStatsCache::c8IncCounter(CIDOrb_ClientBase::sciCmdCache);
    }

    // Reset this one for new use
    pcqiCur->Reset(oidToSet);
    return pcqiCur;
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Protected constructors
// ---------------------------------------------------------------------------
TOrbClientBase::TOrbClientBase() :

    m_bSrvRefd(kCIDLib::False)
    , m_enctNextOIDFresh(0)
{
}

TOrbClientBase::TOrbClientBase( const   TOrbObjId&  ooidSrc
                                , const TString&    strNSBinding) :

    m_bSrvRefd(kCIDLib::False)
    , m_enctNextOIDFresh(0)
    , m_strNSBinding(strNSBinding)
{
    // Remember if the incoming address is an already cached one
    const tCIDLib::TBoolean bWasCached = ooidSrc.bHasCachedAddr();

    //
    //  If we got a cached address, then use that. Else we try to resolve
    //  the address and throw if it fails.
    //
    if (bWasCached)
    {
        m_ipepSrv = TIPEndPoint(ooidSrc.ipaCached(), ooidSrc.ippnHost());
    }
     else
    {
        try
        {
            m_ipepSrv = TIPEndPoint
            (
                ooidSrc.strAddrText(), ooidSrc.ippnHost(), tCIDSock::EAddrTypes::Unspec
            );
        }

        catch(TError& errToCatch)
        {
            if (facCIDOrb().bLogFailures() && !errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_SrvNotAvailable
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , ooidSrc
            );
        }

        catch(...)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_SrvNotAvailable
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , ooidSrc
            );
        }
    }

    //
    //  And now store the object id. If the incoming one got wasn't a
    //  cached object with pre-resolved address, then store the new
    //  end point on ours, in case it gets cached.
    //
    m_ooidThis = ooidSrc;
    if (!bWasCached)
        m_ooidThis.SetCachedAddr(m_ipepSrv);

    //
    //  Add a new server reference. It will try to create one if there isn't
    //  already one for this server. If it cannot, then give up now. If there
    //  already is one, we just reference it.
    //
    if (!psrvtAddSrvRef())
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_SrvNotAvailable
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_ipepSrv
        );
    }

    //
    //  If we got a name server binding, add this guy to the OOID cache.
    //  If the incoming object id has a cached address, then in theory it
    //  is already there and the cached object id is this one. But don't
    //  assume that's true. And if it wasn't a previously cached address we'd
    //  store it as well. So just go ahead and do it either way.
    //
    //  NOTE: Be sure to store OURs not the one we got, since ours may have
    //  been updated with cached address info.
    //
    if (!m_strNSBinding.bIsEmpty())
        facCIDOrb().StoreObjIDCache(m_strNSBinding, m_ooidThis);

    // Set the next object id refresh interval from now
    m_enctNextOIDFresh = TTime::enctNowPlusSecs(CIDOrb_ClientBase::c4OIDRefreshSecs);
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The queue item is now marked as 'Wait' stage, meaning that it's been
//  reserved and is waiting to be queued on the command queue. If we throw
//  out of here before we get it queued, then the caller (IDL generated
//  client side proxy code) will release it back.
//
tCIDLib::TVoid
TOrbClientBase::Dispatch(const  tCIDLib::TCard4     c4WaitFor
                        ,       TCmdQItem* const    pcqiToUse)
{
    // Make sure we have initialized the ORB
    #if CID_DEBUG_ON
    if (!CIDOrb_ClientBase::atomInitDone)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
        );
    }
    #endif

    TSrvTarget* psrvtOurs = nullptr;

    // Make sure that we have a server referenced
    if (!m_bSrvRefd)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_NoSrvTarget
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
            , m_ipepSrv
            , m_ooidThis.strClientProxyClass()
        );
    }

    // Lock the overall mutex and look up our server target
    tCIDLib::TCard4 c4SeqNum = 0;
    {
        TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync, 5000UL);

        psrvtOurs = psrvtFindServer(m_ipepSrv, kCIDLib::False);
        if (!psrvtOurs)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_NoSrvTarget
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
                , m_ipepSrv
                , m_ooidThis.strClientProxyClass()
            );

            // This won't happen, but it makes the analyzer happier
            return;
        }

        #if CID_DEBUG_ON
        if (psrvtOurs->m_ipepServer != m_ipepSrv)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_DiffEndPoint
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
            );
        }
        #endif

        // If its current in reconnect mode, then give up now
        if (psrvtOurs->m_bReconnMode)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_ServerClosed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
                , m_ipepSrv
                , m_ooidThis.strClientProxyClass()
            );
        }

        //
        //  No reason not to try queuing it up. There is still a small
        //  window for us to queue one when the server is in the process
        //  of being lost, but it'll still get the same error, just
        //  through a longer route.
        //
        c4SeqNum = psrvtOurs->c4QueueCmd(pcqiToUse);
    }

    //
    //  Wait for a reply for the indicated length of time. If it doesn't
    //  arrive, then will either either remove it from the list or orphan
    //  it so that the reply get's dropped on the floor.
    //
    //  We add the timeout adjust here, which normally is zero but allows
    //  adjustment for slower clients or networks.
    //
    try
    {
        pcqiToUse->WaitFor(c4WaitFor + facCIDOrb().c4TimeoutAdjust());
    }

    catch(TError& errToCatch)
    {
        // If it's not a mutex timeout error, then log it
        if (!errToCatch.bLogged()
        &&  !errToCatch.bCheckEvent(facCIDLib().strName()
                                    , kCIDErrs::errcEv_Timeout))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        //
        //  Oops, it either timed out, or something went really awry in the
        //  mutex wait. So lock the object so we can clean up.
        //
        TMtxLocker lockCmd(pcqiToUse->pmtxLock());

        //
        //  It's possible that it came in just after we timed out, so check
        //  the state.
        //
        if (pcqiToUse->eStage() != tCIDOrb::ECmdStages::Ready)
        {
            //
            //  If not, then mark it as orphaned, so that the thread that's
            //  processing it will just free it when it finally comes back,
            //  and throw our ORB timeout error
            //
            pcqiToUse->eStage(tCIDOrb::ECmdStages::Orphaned);
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_Timeout
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Timeout
                , m_ipepSrv
                , m_ooidThis.strClientProxyClass()
            );
        }
    }

    //
    //  Check and see if this response is an exception. If so, then we
    //  want to throw that exception from here. This way, all derived
    //  code knows that if it gets back from here, then it worked (i.e.
    //  just like an exception would be in a local scenario.)
    //
    //  We specially watch for an object not found error. That means that
    //  the remote interface that we tried to talk to is not on the server.
    //  If we see that, we check to see if need to remove that object id
    //  from the object id cache, so that we won't keep using it (and keep
    //  failing.) The next client to use it will have to look it up again,
    //  and therefore recover if the remove interface was removed and then
    //  came back.
    //
    if (!pcqiToUse->ocmdData().bRetStatus())
    {
        TError errRet;
        pcqiToUse->ocmdData().strmIn() >> errRet;

        if (errRet.bCheckEvent(facCIDOrb().strName(), kOrbErrs::errcServ_ObjNotFound))
        {
            //
            //  This guy shouldn't throw, but just in case, don't allow it.
            //  Note that we remove by object id, even if we have the name
            //  server binding, because it may have already been updated by
            //  now with the correct object id, and we only want to remove
            //  this bad object id if it's there, not a potentially now corrected
            //  one.
            //
            try
            {
                facCIDOrb().RemoveFromOIDCache(m_ooidThis);
            }

            catch(...)
            {
            }
        }

        // And now throw it in our client context
        throw errRet;
    }

    //
    //  If we got here, then it worked. So, if we were given a name server
    //  binding, and it's been over our auto-cache refresh time, refresh the
    //  binding and update our next refresh time.
    //
    if (!m_strNSBinding.bIsEmpty())
    {
        tCIDLib::TEncodedTime enctNow = TTime::enctNow();
        if (enctNow < m_enctNextOIDFresh)
        {
            // Update the throttling stamp first just in case, then refresh
            m_enctNextOIDFresh = enctNow +
                (CIDOrb_ClientBase::c4OIDRefreshSecs * kCIDLib::enctOneSecond);
            facCIDOrb().RefreshObjIDCache(m_strNSBinding, m_ooidThis);
        }
    }
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Private, static methods
// ---------------------------------------------------------------------------

//
//  Look up a target by IP end point. We assume the caller has locked!. They
//  can request that if we don't find it in the active server list that we
//  check the scavenger list as well. If so, then the caller
//
TSrvTarget*
TOrbClientBase::psrvtFindServer(const   TIPEndPoint&        ipepServer
                                , const tCIDLib::TBoolean   bReclaimScavenged)
{
    // Just call the other version with a dummy index parm
    tCIDLib::TCard4 c4Dummy;
    return psrvtFindServer(ipepServer, c4Dummy, bReclaimScavenged);
}


TSrvTarget*
TOrbClientBase::psrvtFindServer(const   TIPEndPoint&        ipepServer
                                ,       tCIDLib::TCard4&    c4Index
                                , const tCIDLib::TBoolean   bReclaimScavenged)
{
    if (!CIDOrb_ClientBase::pcolServers)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );

        // Won't actually happen, but make sthe analyzer happy
        return nullptr;
    }

    // Check the list of active servers
    TSrvTarget* psrvtRet = nullptr;
    tCIDLib::TCard4 c4Count = CIDOrb_ClientBase::pcolServers->c4ElemCount();
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TSrvTarget* psrvtCur = CIDOrb_ClientBase::pcolServers->pobjAt(c4Index);
        if (psrvtCur->m_ipepServer == ipepServer)
        {
            psrvtRet = psrvtCur;
            break;
        }
    }

    //
    //  If we didn't find it in the active list, and they want us to try to
    //  reclaim one from the scavenger list, then try that if the cache hasn't been
    //
    //
    if (!psrvtRet && bReclaimScavenged && CIDOrb_ClientBase::pcolCache)
    {
        c4Count = CIDOrb_ClientBase::pcolCache->c4ElemCount();
        for (c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TSrvTarget* psrvtCur = CIDOrb_ClientBase::pcolCache->pobjAt(c4Index);

            // If this is our guy, process it
            if (psrvtCur->m_ipepServer == ipepServer)
            {
                //
                //  Orphan it out of the cache. Suppress nodiscard error by setting
                //  the current pointer again from the orphan call.
                //
                psrvtCur = CIDOrb_ClientBase::pcolCache->pobjOrphanAt(c4Index);
                TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciSrvCache);

                //
                //  See if it's gone offline while it was in the cache. If
                //  so, then drop it and return null. Else, take it and add
                //  it back to the active server list.
                //
                try
                {
                    if (psrvtCur->bIsConnected())
                    {
                        CIDOrb_ClientBase::pcolServers->Add(psrvtCur);
                        psrvtRet = psrvtCur;
                        TStatsCache::c8IncCounter(CIDOrb_ClientBase::sciSrvTargets);
                    }
                     else
                    {
                        delete psrvtCur;
                    }
                }

                catch(...)
                {
                    delete psrvtCur;
                }

                // Either way, we break out now
                break;
            }
        }
    }
    return psrvtRet;
}


// Called to iniitalize the client side of the ORB interface
tCIDLib::TVoid TOrbClientBase::InitializeOrbClient()
{
   if (!CIDOrb_ClientBase::atomInitDone)
   {
        TBaseLock lockInit;
        if (!CIDOrb_ClientBase::atomInitDone)
        {
            // Create our collections and mutex
            CIDOrb_ClientBase::pcolCmdCache = new TOrbCmdItemCache
            (
                tCIDLib::EAdoptOpts::Adopt, 32, tCIDLib::EMTStates::Safe
            );
            CIDOrb_ClientBase::pcolCache = new TOrbCSrvList(tCIDLib::EAdoptOpts::Adopt);
            CIDOrb_ClientBase::pcolServers = new TOrbCSrvList(tCIDLib::EAdoptOpts::Adopt);
            CIDOrb_ClientBase::pcolConnWaitList = new TConnWaitList(tCIDLib::EAdoptOpts::Adopt, 16);

            //
            //  Force our stats cache items into the cache so that from here on out
            //  we can just use the cache item for fast access. If the client
            //  side is terminated and re-initialized, then we'll reset them, which
            //  is appropriate.
            //
            TStatsCache::RegisterItem
            (
                kCIDOrb::pszStat_Cl_CmdCache
                , tCIDLib::EStatItemTypes::Counter
                , CIDOrb_ClientBase::sciCmdCache
            );

            TStatsCache::RegisterItem
            (
                kCIDOrb::pszStat_Cl_SrvCache
                , tCIDLib::EStatItemTypes::Counter
                , CIDOrb_ClientBase::sciSrvCache
            );

            TStatsCache::RegisterItem
            (
                kCIDOrb::pszStat_Cl_SrvTargets
                , tCIDLib::EStatItemTypes::Counter
                , CIDOrb_ClientBase::sciSrvTargets
            );

            TStatsCache::RegisterItem
            (
                kCIDOrb::pszStat_Cl_WaitList
                , tCIDLib::EStatItemTypes::Counter
                , CIDOrb_ClientBase::sciWaitList
            );

            //
            //  Create our sync mutex late in the process, since calling
            //  threads will use it as an ORB initialized check.
            //
            CIDOrb_ClientBase::pmtxSync = new TMutex;

            // Set the initialized flag now
            CIDOrb_ClientBase::atomInitDone.Set();

            // Create and spin up the cache thread
            CIDOrb_ClientBase::pthrCacheScavenger = new TThread
            (
                L"CIDOrbClientScavengerThread", &eCacheScavengerThread
            );
            CIDOrb_ClientBase::pthrCacheScavenger->Start();
        }
    }
}


//
//  DO NOT try to lock the overall mutex here. It will both cause the
//  scavenger thread to deadlock when we try to close it, and we also clean
//  up the mutex at the end anyway.
//
//  We assume here that this will not be done until the program has stopped
//  using the ORB, so we don't need any sync here.
//
tCIDLib::TVoid TOrbClientBase::TerminateOrbClient()
{
    // If not initialized, then do nothing
    if (!CIDOrb_ClientBase::atomInitDone)
        return;

    // Stop the cache scavenger thread
    try
    {
        CIDOrb_ClientBase::pthrCacheScavenger->ReqShutdownSync(10000);
        CIDOrb_ClientBase::pthrCacheScavenger->eWaitForDeath(5000);
        delete CIDOrb_ClientBase::pthrCacheScavenger;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
    CIDOrb_ClientBase::pthrCacheScavenger = nullptr;

    try
    {
        CIDOrb_ClientBase::pcolCache->RemoveAll();
        delete CIDOrb_ClientBase::pcolCache;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
    CIDOrb_ClientBase::pcolCache = nullptr;

    try
    {
        CIDOrb_ClientBase::pcolServers->RemoveAll();
        delete CIDOrb_ClientBase::pcolServers;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
    CIDOrb_ClientBase::pcolServers = nullptr;


    try
    {
        CIDOrb_ClientBase::pcolCmdCache->RemoveAll();
        delete CIDOrb_ClientBase::pcolCmdCache;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
    CIDOrb_ClientBase::pcolCmdCache = nullptr;

    // Clean up the mutex and clear the initialized flag
    delete CIDOrb_ClientBase::pmtxSync;
    CIDOrb_ClientBase::pmtxSync = nullptr;

    CIDOrb_ClientBase::atomInitDone.SetValue(kCIDLib::False);


    // Zero out our stats cache values
    TStatsCache::SetValue(kCIDOrb::pszStat_Cl_CmdCache, CIDOrb_ClientBase::sciCmdCache, 0);
    TStatsCache::SetValue(kCIDOrb::pszStat_Cl_SrvCache, CIDOrb_ClientBase::sciSrvCache, 0);
    TStatsCache::SetValue(kCIDOrb::pszStat_Cl_SrvTargets, CIDOrb_ClientBase::sciSrvTargets, 0);
    TStatsCache::SetValue(kCIDOrb::pszStat_Cl_WaitList, CIDOrb_ClientBase::sciWaitList, 0);
}


// ---------------------------------------------------------------------------
//  TOrbClientBase: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Checks whether this proxy has a connection to it's server, at the network
//  level.
//
tCIDLib::TBoolean TOrbClientBase::bIsProxyConnected() const
{
    // Make sure we have initialized the ORB
    #if CID_DEBUG_ON
    if (!CIDOrb_ClientBase::atomInitDone)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
        );
    }
    #endif

    //
    //  No need to lock for this quick check. If we don't even have a server
    //  object at this point, then we clearly aren't connected.
    //
    if (!m_bSrvRefd)
        return kCIDLib::False;

    // Lock and find our server reference
    TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);
    TSrvTarget* psrvtUs = psrvtFindServer(m_ipepSrv, kCIDLib::False);

    // If we didn't find one, then obviously not connected
    if (!psrvtUs)
        return kCIDLib::False;

    //
    //  We did, so see if he's alive. We return his reconnect mode flag. If
    //  this is set, then he is trying to reconnect. There is a small window
    //  here where the socket might be dead but he's not seen it yet, but
    //  this method is always called in response to a failure coming out of
    //  a remote call, and if it really failed due to loss of connection, then
    //  the flag will already have been set.
    //
    //  The only other alternative requires synching with the spooler thread,
    //  since that's the only way we could be sure that the socket wouldn't
    //  get deleted while we were checking it's connected status. The risk
    //  and overhead aren't worth the tiny improvement.
    //
    return !psrvtUs->m_bReconnMode;
}


//
//  Finds or adds a new server target object for our end point. If one exists,
//  we get that one and the ref count is bumped. If not, one gets added.
//
//  We have to do a little dance here to deal with multiple threads trying
//  to connect to the same server at once. One will be first and will start
//  trying. Subsequent ones that come in for the same target will be queued
//  up and the first thread will release them and tell them his success or
//  failure status so that they can react correctly.
//
TSrvTarget* TOrbClientBase::psrvtAddSrvRef()
{
    TSrvTarget* psrvtNew = nullptr;

    //
    //  First, lock and see if the server is already available. If so, we
    //  bump it's ref, mark ourself as having a server referenced, and we
    //  can return now.
    //
    TOrbCConnWait* poccwUs = nullptr;
    {
        TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);

        //
        //  Find our server target object. We look up the one with the end point
        //  that our object id is for. Tell him to check the scavenged list
        //  as well.
        //
        psrvtNew = psrvtFindServer(m_ipepSrv, kCIDLib::True);

        // If we got one, then bump the ref count and indicate we have a ref
        if (psrvtNew)
        {
            psrvtNew->m_c4RefCount++;
            m_bSrvRefd = kCIDLib::True;

            // And we are happy
            return psrvtNew;
        }

        //
        //  There isn't one, so see if we need to be responsible for trying
        //  the connection, or should just queue ourself up. So search the
        //  wait queue for our ip address.
        //
        TOrbCConnWait* poccwConn = nullptr;

        const tCIDLib::TCard4 c4Count = CIDOrb_ClientBase::pcolConnWaitList->c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TOrbCConnWait* poccwCur
            (
                CIDOrb_ClientBase::pcolConnWaitList->pobjAt(c4Index)
            );

            // It's for our server, so check it
            if (poccwCur->m_ipepTarget == m_ipepSrv)
            {
                //
                //  If this guy is connecting, then we need to wait. If it is
                //  a waiting one, we don't pay attention to those.
                //
                if (poccwCur->m_eState == TOrbCConnWait::EWaitStates::Connecting)
                {
                    poccwConn = poccwCur;
                    break;
                }
            }
        }

        //
        //  If no one else is connecting, then we need to do so. If someone
        //  is, then we need to wait.
        //
        poccwUs = new TOrbCConnWait
        (
            m_ipepSrv
            , (poccwConn == nullptr) ? TOrbCConnWait::EWaitStates::Connecting
                                     : TOrbCConnWait::EWaitStates::Waiting
        );

        //
        //  And add us to the wait list. We have to add ourself, even if we
        //  are the one connecting, so that later threads have a way to know
        //  that we are already here and connecting.
        //
        CIDOrb_ClientBase::pcolConnWaitList->Add(poccwUs);
        TStatsCache::c8IncCounter(CIDOrb_ClientBase::sciWaitList);
    }

    // If we weren't first in line on this server, then let's block
    if (poccwUs->m_eState == TOrbCConnWait::EWaitStates::Waiting)
    {
        //
        //  We use a non-throwing wait, so that if we time out we just
        //  come back and clean up.
        //
        poccwUs->m_evWait.bWaitFor(5000);

        //
        //  Get the status out, then we can remove our entry nneed to lock
        //  the main list again while we do this. It is adopting so this will
        //  delete the object.
        //
        TOrbCConnWait::EWaitStates eRes = poccwUs->m_eState;
        {
            TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);
            CIDOrb_ClientBase::pcolConnWaitList->bRemoveIfMember(poccwUs);
            TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciWaitList);
            poccwUs = nullptr;
        }

        //
        //  Look at the status. If the other thread succeeded, then try again.
        //  Else, just fall through with a null return.
        //
        if (eRes == TOrbCConnWait::EWaitStates::Success)
        {
            //
            //  Now we can look up our server. If this fails, we just let it
            //  fail since it has to be some pretty pathological scenario
            //  We have to re-lock the main mutex here.
            //
            TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);
            psrvtNew = psrvtFindServer(m_ipepSrv, kCIDLib::True);

            // If we got one, then bump the ref count and indicate we have a ref
            if (psrvtNew)
            {
                psrvtNew->m_c4RefCount++;
                m_bSrvRefd = kCIDLib::True;
            }
        }
    }
     else
    {
        // We are the first, so let's try to set up the target
        TSrvTarget* psrvtTry = new TSrvTarget(m_ipepSrv);
        if (psrvtTry->bInitialize())
        {
            // Set up our return pointer and bump the ref count
            psrvtNew = psrvtTry;
            psrvtNew->m_c4RefCount++;
            m_bSrvRefd = kCIDLib::True;
        }
         else
        {
            try
            {
                delete psrvtTry;
            }

            catch(...)
            {
            }
        }

        //
        //  Ok, we can now add our new server target if we added one, and
        //  either way, wake up any waiting threads and give them the good or
        //  bad news. We need to relock the list again.
        //
        TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);

        // If we created a new server target, then add it to the list
        if (psrvtNew)
        {
            CIDOrb_ClientBase::pcolServers->Add(psrvtNew);
            TStatsCache::c8IncCounter(CIDOrb_ClientBase::sciSrvTargets);
        }

        // Remove our wait item from the list
        CIDOrb_ClientBase::pcolConnWaitList->bRemoveIfMember(poccwUs);
        TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciWaitList);

        //
        //  And search for other waiters. Note that we are not responsible for
        //  removing them from the wait list. They do that themselves.
        //
        const tCIDLib::TCard4 c4Count = CIDOrb_ClientBase::pcolConnWaitList->c4ElemCount();
        TOrbCConnWait* poccwCur;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            poccwCur = CIDOrb_ClientBase::pcolConnWaitList->pobjAt(c4Index);

            // It's for our server, so check it.
            if (poccwCur->m_ipepTarget == m_ipepSrv)
            {
                //
                //  If this guy is waiting or it's connecting then we set his
                //  status and wake him up. They are responsible for removing
                //  themselves from the list.
                //
                if (poccwCur->m_eState == TOrbCConnWait::EWaitStates::Waiting)
                {
                    if (psrvtNew)
                        poccwCur->m_eState = TOrbCConnWait::EWaitStates::Success;
                    else
                        poccwCur->m_eState = TOrbCConnWait::EWaitStates::Failure;
                    poccwCur->m_evWait.Trigger();
                }
            }
        }
    }
    return psrvtNew;
}


//
//  Drops any server target reference we have. If we have a ref, it decrements
//  the ref count. if the ref count goes zero, we move it onto the cache list
//  where we'll either reuse it later, or the scavenger thread will clean it
//  up in a bit if it's not accessed again.
//
//  !WE ASSUME that the caller has locked the sync mutex before calling us!
//
tCIDLib::TVoid TOrbClientBase::RemoveSrvRef()
{
    // Lock the overall mutex while we do this
    TMtxLocker lockSrv(CIDOrb_ClientBase::pmtxSync);

    // If we've not referenced a server yet, then return
    if (!m_bSrvRefd)
        return;

    // And now mark us as not having a reference
    m_bSrvRefd = kCIDLib::False;

    //
    //  Find our server target object. We look up the one with the end point
    //  that our object id is for.
    //
    tCIDLib::TCard4 c4Index;
    TSrvTarget* psrvtOurs = psrvtFindServer(m_ipepSrv, c4Index, kCIDLib::False);
    if (psrvtOurs)
    {
        // If the ref count is already zero, that's bad
        if (!psrvtOurs->m_c4RefCount)
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcClient_BadSrvRefCount
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , m_ipepSrv
                , m_ooidThis.strClientProxyClass()
            );
        }

        psrvtOurs->m_c4RefCount--;

        //
        //  If its zero now, then let's move this guy over to the cache
        //  list. If it is reused before it times out of the cache, we
        //  won't have to recreate it.
        //
        //  NOTE:   If it's currently not connected, then don't bother
        //          putting it in the cache. One reason it's last ref is
        //          being dropped might be that it went offline and all of
        //          it's clients have bailed out with errors.
        //
        //  NOTE:   If an error occurs, we can't log anything because if
        //          this is being called in the context of an ORB based
        //          logging client, we'd be screwed. So we have to just
        //          throw another error and format the original error as
        //          the aux text.
        //
        if (!psrvtOurs->m_c4RefCount)
        {
            try
            {
                if (psrvtOurs->bIsConnected())
                {
                    //
                    //  Time stamp it with its stale out time and then orphan
                    //  it out of one collection and into the other.
                    //
                    psrvtOurs->m_enctCacheStamp = TTime::enctNow()
                                                  + CIDOrb_ClientBase::enctCachePeriod;
                    CIDOrb_ClientBase::pcolCache->Add
                    (
                        CIDOrb_ClientBase::pcolServers->pobjOrphanAt(c4Index)
                    );
                    TStatsCache::c8IncCounter(CIDOrb_ClientBase::sciSrvCache);
                    TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciSrvTargets);
                }
                 else
                {
                    // It was offline anyway, so drop it
                    CIDOrb_ClientBase::pcolServers->RemoveAt(c4Index);
                    TStatsCache::c8DecCounter(CIDOrb_ClientBase::sciSrvTargets);
                }
            }

            catch(const TError& errToCatch)
            {
                TString strMsg(errToCatch);
                facCIDOrb().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbErrs::errcClient_SrvRemove
                    , errToCatch
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , m_ipepSrv
                    , m_ooidThis.strClientProxyClass()
                );
            }
        }
    }
}

