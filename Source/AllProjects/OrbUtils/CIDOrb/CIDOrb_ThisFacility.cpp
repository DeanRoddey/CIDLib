//
// FILE NAME: CIDOrb_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/06/2000
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
//  This file implements the facility class for this facility.
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
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDOrb_ThisFacility
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The timeout period for the name server object id cache. We set it
        //  pretty low, to 50 seconds. For the most important, high traffic
        //  object ids, we just continue to update the object's timeout stamp each
        //  time the object id is successfully used.
        //
        //  This way, it will time out pretty fast when not used often, so that
        //  there's little danger of a bad one hanging around in the cache, but
        //  the often used ones will remain cached as long as they are still
        //  working.
        //
        //  The auto-rebinder regularly hits the name server at least once every
        //  enctForcedNSTO seconds if no one else is hitting it for other reasons,
        //  so as long as we keep this time not too large, but enough larger than
        //  the forced NS ping timeout, that will insure that the name server
        //  itself doesn't time out of the object id cache ever as long as it
        //  is being successfully accessed.
        //
        //  That will avoid lots of name resolutions since the name server is
        //  accessed a lot.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TEncodedTime enctCacheTO = kCIDLib::enctOneSecond * 50;


        // -----------------------------------------------------------------------
        //  If we've not accessed the name server for some reason within this
        //  length of time, next time someone does an object lookup, we'll force
        //  a trip to the name server, even if the object being accessed is in
        //  the cache and good.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TEncodedTime enctForcedNSTO = (kCIDLib::enctOneSecond * 30);


        // -----------------------------------------------------------------------
        //  Max entries we'll keep in the name server cache
        // -----------------------------------------------------------------------
        constexpr  tCIDLib::TCard4 c4MaxNSQSize = 2048;
    }
}





// ---------------------------------------------------------------------------
//  CLASS: TFacCIDOrb
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDOrb: Public, static data members
//
//  Note that the name server exists really at the CIDOrbUC facility level,
//  but we have a special requirement to watch for it being accessed for
//  object id caching reasons. So its binding is defined down here.
// ---------------------------------------------------------------------------
const TString TFacCIDOrb::strCmd_Ping(L"$$$IntOrbCmd_Ping");
const TString TFacCIDOrb::strFauxNSBinding(L"/CIDLib/FakeNSBinding");


// ---------------------------------------------------------------------------
//  TFacCIDOrb: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDOrb::TFacCIDOrb() :

    TFacility
    (
        L"CIDOrb"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_c4CmdOverhead(0)
    , m_c4ReplyOverhead(0)
    , m_c4TimeoutAdjust(0)
    , m_c8LastNSCookie(0)
    , m_colNSCache(173, TStringKeyOps(), tCIDLib::EMTStates::Safe)
    , m_enctNextForcedNS(0)
    , m_enctTimeoutAdjust(0)
    , m_poccmSrv(nullptr)
    , m_pcrypSecure(nullptr)
    , m_thrMonitor
      (
        L"CIDOrbMonitorThread"
        , TMemberFunc<TFacCIDOrb>(this, &TFacCIDOrb::eMonThread)
      )
{
    // See if the timeout adjustment is set
    TString strVal;
    if (TProcEnvironment::bFind(L"CID_ORBTIMEOUTADJUST", strVal))
    {
        try
        {
            m_c4TimeoutAdjust = strVal.c4Val(tCIDLib::ERadices::Dec);
        }

        catch(...)
        {
            // It's bad, so set it to 5 seconds
            m_c4TimeoutAdjust = 5000;
        }
        m_enctTimeoutAdjust = m_c4TimeoutAdjust * kCIDLib::enctOneMilliSec;
    }

    // Set up any of the stats cache items we support
    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_ActiveCmds
        , tCIDLib::EStatItemTypes::Counter
        , m_sciActiveCmds
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_QueuedCmds
        , tCIDLib::EStatItemTypes::Counter
        , m_sciQueuedCmds
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_RegisteredObjs
        , tCIDLib::EStatItemTypes::Counter
        , m_sciRegisteredObjs
    );

    TStatsCache::RegisterItem
    (
        kCIDOrb::pszStat_Srv_WorkQItems
        , tCIDLib::EStatItemTypes::Counter
        , m_sciWorkQItems
    );

    //
    //  Determine the worst case payload overhead by streaming out a command
    //  object in either mode, but with no actual payload. That will tell
    //  us how many bytes are required above and beyond the payload itself.
    //
    //  This doesn't have to be super-accurate, but must at least reflect
    //  the minimum overhead, so we add a bit more for safety.
    //
    TBinMBufOutStream strmOut(1024UL);
    TOrbCmd ocmdTmp(1);

    ocmdTmp.SetCmdMode();
    strmOut << ocmdTmp << kCIDLib::FlushIt;
    m_c4CmdOverhead = strmOut.c4CurPos() + 8;

    strmOut.Reset();
    ocmdTmp.SetReplyMode();
    strmOut << ocmdTmp << kCIDLib::FlushIt;
    m_c4ReplyOverhead = strmOut.c4CurPos() + 8;

    // Start the monitor thread
    m_thrMonitor.Start();
}


TFacCIDOrb::~TFacCIDOrb()
{
    // In case they didn't do it manually, which they should
    Terminate();
}


// ---------------------------------------------------------------------------
//  TFacCIDOrb: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Checks to see if we have the object id for the indicated binding in our
//  cache, and it's been refreshed within the timeout period. If so, we give
//  it back and return true. If it's there but timed out, we remove it and
//  return false. If not there, we just return false.
//
//  However, we want to force a trip to the name server at least once every
//  couple minutes if it hasn't been required for other reasons, so that we
//  can see if it's cycled.
//
//  NOTE: If the forced time has passed, we only force a failure if the thing
//  we are being asked about is not the name server itself. We want to force
//  a trip to the name server at least every so often, not force the name
//  server address to re-resolved periodically when we know it's working fine
//  because it hasn't timed out of the cache.
//
tCIDLib::TBoolean
TFacCIDOrb::bCheckOOIDCache(const   TString&    strBindingName
                            ,       TOrbObjId&  ooidToFill)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());

    try
    {
        //
        //  If we have gone beyond the next scheduled forced name server trip
        //  time, then say we didn't find it. Reset the timer for the next
        //  time around before we return.
        //
        const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
        if (m_enctNextForcedNS < enctNow)
        {
            if (strBindingName != strFauxNSBinding)
            {
                m_enctNextForcedNS = enctNow + CIDOrb_ThisFacility::enctForcedNSTO;
                return kCIDLib::False;
            }
        }

        // See if this guy is in the cache
        TObjIdCache::TPair* pkobjBinding = m_colNSCache.pkobjFindByKey(strBindingName);
        if (!pkobjBinding)
            return kCIDLib::False;

        // If it's timed out, remove it and force them to go back the name server
        if (pkobjBinding->objValue().enctCache() < enctNow)
        {
            m_colNSCache.RemoveKey(strBindingName);
            return kCIDLib::False;
        }

        ooidToFill = pkobjBinding->objValue();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return kCIDLib::True;
}




// Return true if we have the indicated server side object interface
tCIDLib::TBoolean
TFacCIDOrb::bHasInterface(const TOrbObjId& ooidToCheck) const
{
    // And ask the object list if it holds this key
    TCritSecLocker lockObjList(&m_crsObjList);
    return (m_colObjList.porbsFind(ooidToCheck.oidKey()) != nullptr);
}


// Provide access to the timeout adjustment value
tCIDLib::TCard4 TFacCIDOrb::c4TimeoutAdjust() const
{
    return m_c4TimeoutAdjust;
}



//
//  We keep track of the last name server cookie seen, for fast recovery
//  purposes, so we can flush the OOID cache quickly after the name server
//  cycles.
//
//  We make this last cookie we've seen available to others. Since we force
//  a trip to the name server at least once every so often, it should be
//  pretty up todate.
//
tCIDLib::TCard8 TFacCIDOrb::c8LastNSCookie() const
{
    tCIDLib::TCard8 c8Ret;
    {
        TMtxLocker mtxlCache(m_colNSCache.pmtxLock());
        c8Ret = m_c8LastNSCookie;
    }
    return c8Ret;
}


//
//  Any time someone has to go to the name server to get an object id, it
//  will get back the name server's current cookie. It can call us here and,
//  if ours isn't the same, we flush the cache and store the new one. This
//  makes sure we don't leave stuff hanging around if the name server
//  cycles.
//
tCIDLib::TVoid TFacCIDOrb::CheckNSCookie(const tCIDLib::TCard8 c8NSCookie)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());

    if (c8NSCookie != m_c8LastNSCookie)
    {
        m_c8LastNSCookie = c8NSCookie;
        FlushOIDCache();
    }

    //
    //  Update the time at which we'll force another trip to the name
    //  server as well, since we know there was just one done. This will
    //  prevent unecessary forced trips to the name server.
    //
    m_enctNextForcedNS = TTime::enctNow() + CIDOrb_ThisFacility::enctForcedNSTO;
}



//
//  We can be told to only accept connections from a particular client
//  address. This method turns that back off again. We just delgate to
//  the client connectio manager, if it exists.
//
tCIDLib::TVoid TFacCIDOrb::ClearOnlyAcceptFrom()
{
    if (m_poccmSrv)
        m_poccmSrv->ClearOnlyAcceptFrom();
}



//
//  A server calls this to de-register one of his objects from the server side
//  ORB.
//
tCIDLib::TVoid TFacCIDOrb::DeregisterObject(const TOrbServerBase* const porbsToDereg)
{
    #if CID_DEBUG_ON
    if (!m_atomServerInit)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_NotInitialized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    #endif

    if (!porbsToDereg)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_NulObject
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );

        // Won't happen but makes analyzer happy
        return;
    }

    //
    //  Lock the object list while we do this and orphan this object out of
    //  the list so that no other threads can see it. Once we get it out of
    //  the list, we can release the list again and let other business
    //  continue. No other threads will see this as an available object.
    //
    tCIDLib::EAdoptOpts eAdopt;
    TOrbServerBase* porbsTarget = nullptr;
    {
        TCritSecLocker lockObjList(&m_crsObjList);
        porbsTarget = m_colObjList.porbsOrphan(porbsToDereg, eAdopt);
    }
    TStatsCache::c8DecCounter(m_sciRegisteredObjs);

    // If it wasn't in the list, that's bad
    if (!porbsTarget)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_ObjNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , porbsToDereg->ooidThis()
        );

        // Won't get here but makes analyzer happy
        return;
    }

    // Put a janitor on it that will delete it if we adopted it
    TJanitor<TOrbServerBase> janTarget(porbsTarget, eAdopt == tCIDLib::EAdoptOpts::Adopt);

    //
    //  Ok, now we have to wait for all threads to exit the object, since
    //  we cannot delete it or terminate it until it is free of threads using
    //  it. We set it's cleanup flag to give any threads a hint that they need
    //  to get out.
    //
    porbsTarget->SetCleanupFlag();
    tCIDLib::TCard4 c4Count = 0;
    while (kCIDLib::True)
    {
        TThread::Sleep(100);

        if (!porbsTarget->c4EnteredCount())
            break;

        c4Count++;
        if (c4Count > 100)
        {
            //
            //  We can only assume that the remaining threads are blocked
            //  on something (which they should not be doing), so we just
            //  have to just continue.
            //
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcServ_ExitTimeout
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , porbsTarget->clsIsA()
            );
        }
    }

    //
    //  Ok, now call the object's terminate method to get it to clean up it's
    //  internal state.
    //
    try
    {
        porbsTarget->Terminate();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_UnknownExceptDereg
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , porbsTarget->clsIsA()
        );
    }

    // The janitor will clean it up if that's appropriate
}


//
//  The worker threads in the client connection manager will call this
//  method to actually do the dispatching. We find out if the target
//  object for the passed command is present and, if so, call its dispatch
//  method, which will do the work internal to it required to get the call
//  done.
//
//  We inc/dec an entered counter on the target object, so that we know if
//  any threads are in a server object at any given time. The counter inside
//  the target is a thread safe counter, so we don't have to lock in order
//  to do these.
//
tCIDLib::TVoid
TFacCIDOrb::DispatchCmd(const TString& strMethod, TOrbCmd& orbcToDispatch)
{
    //
    //  Check for any magic 'inter-orb' messages, which are not
    //  to be sent along to any server side object.
    //
    //  <TBD> None yet


    //
    //  Look up the server object to which this command is targeted. Lock the
    //  list while we do this, and get the entered count bumped while we
    //  have the list locked. This will insure that the object cannot get
    //  dumped while we are in it.
    //
    TOrbServerBase* porbsTarget = nullptr;
    {
        TCritSecLocker lockObjList(&m_crsObjList);
        porbsTarget = m_colObjList.porbsFind(orbcToDispatch.oidKey());

        // If we found it, bump the entered count, while it's still locked
        if (porbsTarget)
            porbsTarget->c4IncEnteredCount();
    }

    //
    //  !!!!
    //  NOTE: We must dec the count now before exit if we found the object
    //  !!!!
    //


    //
    //  If not found, fill in an error for return to client. No need to
    //  decrement here since we never found to object.
    //
    //
    //
    //  >>>> IT IS IMPORTANT that we send this particular error, because the
    //       client side ORB will watch for it, and remove the object id from
    //       the client side object id cache. Otherwise, it would continue to
    //       be used until it timed out of the cache. This way, the client can
    //       recover quickly if a server cycles.
    //
    if (!porbsTarget)
    {
        // Format out the error text
        TString strText
        (
            kOrbErrs::errcServ_ObjNotFound, facCIDOrb(), orbcToDispatch.oidKey()
        );

        TError errToRet
        (
            facCIDOrb().strName()
            , CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_ObjNotFound
            , strText
            , TString::strEmpty()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );

        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << errToRet;
        orbcToDispatch.bRetStatus(kCIDLib::False);
        return;
    }

    try
    {
        // Bump the active server command calblacks counter while we are in here
        TSafeCard4Janitor janCount(&m_scntActiveCmds);

        // Assume it will work
        orbcToDispatch.bRetStatus(kCIDLib::True);

        //
        //  Increment the entered count on this object, dispatch the command,
        //  and then decrement the count.
        //
        porbsTarget->Dispatch(strMethod, orbcToDispatch);
        porbsTarget->c4DecEnteredCount();
    }

    catch(TError& errToCatch)
    {
        // Bump down the entered count now that we are out of it
        porbsTarget->c4DecEnteredCount();

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << errToCatch;
        orbcToDispatch.bRetStatus(kCIDLib::False);
    }

    catch(...)
    {
        // Bump down the entered count now that we are out of it
        porbsTarget->c4DecEnteredCount();

        TError errToRet
        (
            facCIDOrb().strName()
            , CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_UnknownExcept
            , facCIDOrb().strMsg(kOrbErrs::errcServ_UnknownExcept)
            , strMethod
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );

        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << errToRet;
        orbcToDispatch.bRetStatus(kCIDLib::False);
    }
}


//
//  A helper for reading in basic raw packets. The caller gets back the
//  sequence id, the read result and if any data was available the data
//  and count of bytes.
//
tCIDOrb::EReadRes
TFacCIDOrb::eReadPacket(        TThread&            thrCaller
                        ,       TStreamSocket&      sockSrc
                        ,       tCIDLib::TCard4&    c4Id
                        , const tCIDLib::TCard4     c4WaitFor
                        ,       tCIDLib::TCard4&    c4DataBytes
                        ,       TMemBuf&            mbufToFill)
{
    // First we call the packet reader
    tCIDOrb::TPacketHdr hdrRead;
    tCIDOrb::EReadRes eRes = eReadPacketHdr(thrCaller, sockSrc, c4WaitFor, hdrRead);

    //
    //  Return the sequence id. If we didn't really get a packet, then
    //  don't worry, just do it anyway. The caller will ignore it, and
    //  this way we avoid the brittleness of trying to only store it when
    //  specific read results occur.
    //
    c4Id = hdrRead.c4SequenceId;

    // If a packet and it's non-zero, then read the packet
    if ((eRes == tCIDOrb::EReadRes::Packet) && hdrRead.c4DataBytes)
        eRes = eReadPacketData(thrCaller, sockSrc, hdrRead, c4DataBytes, mbufToFill);

    return eRes;
}


//
//  A higher level helper than the raw packet reader above, this one is
//  specialized for the server side, and makes things a lot more efficient
//  since it doesn't have to gen up a work item unless it is actually
//  going to be needed. It checks the header first and if it is a packet
//  with data, it gens up the work queue item and fills it in.
//
//  We also need an input stream to read the data into. We optimize here by
//  making the calling thread pass us a tmp stream, which it can keep
//  around all the time. Since probably 99% of all packets quite small, we
//  can use their passed stream almost all the time, only allocating a big
//  one locally here when it is necessary. And they have to pass us the
//  underlying memory buffer so that we can read directly into it and then
//  fact the end of stream on the stream and stream in from that.
//
tCIDOrb::EReadRes
TFacCIDOrb::eReadPacket(        TThread&                thrCaller
                        ,       TStreamSocket&          sockSrc
                        ,       tCIDLib::TCard4&        c4Id
                        , const tCIDLib::TCard4         c4WaitFor
                        ,       TMemBuf&                mbufTmp
                        ,       TBinMBufInStream&       strmTmp
                        ,       TWorkQItemPtr&          wqipNew)
{
    //
    //  First we call the packet header reader. This will tell us if we
    //  have any packet data, and if so how big it is.
    //
    tCIDOrb::TPacketHdr hdrRead;
    tCIDOrb::EReadRes eRes = eReadPacketHdr(thrCaller, sockSrc, c4WaitFor, hdrRead);

    //
    //  Return the sequence id. If we didn't really get a packet, then
    //  don't worry, just do it anyway. The caller will ignore it, and
    //  this way we avoid the brittleness of trying to only store it when
    //  specific read results occur.
    //
    c4Id = hdrRead.c4SequenceId;

    //
    //  If it's a packet, so let's read it
    //
    if (eRes == tCIDOrb::EReadRes::Packet)
    {
        //
        //  It better not be zero bytes. For now, return a lost connection status
        //  to recycle, as a kind of a punt. In debug mode, throw.
        //
        if (!hdrRead.c4DataBytes)
        {
            CIDAssert2(L"Got a zero sized ORB data packet");
            return tCIDOrb::EReadRes::Lost;
        }

        //
        //  If we don't allocate a stream, then the janitor just does
        //  nothing, else it will free the local buffer on the way out.
        //
        TMemBuf*          pmbufToUse = &mbufTmp;
        TBinMBufInStream* pstrmToUse = &strmTmp;
        THeapBuf*         pmbufBig = nullptr;
        TBinMBufInStream* pstrmBig = nullptr;
        if (hdrRead.c4DataBytes > mbufTmp.c4MaxSize())
        {
            pmbufBig = new THeapBuf(hdrRead.c4DataBytes, hdrRead.c4DataBytes);
            pstrmBig = new TBinMBufInStream(pmbufBig, 0, tCIDLib::EAdoptOpts::Adopt);
            pmbufToUse = pmbufBig;
            pstrmToUse = pstrmBig;
        }
        TJanitor<TBinMBufInStream> janBigStream(pstrmBig);

        //
        //  Now let's call the packet data reader with this guy's stream
        //  to fill in.
        //
        tCIDLib::TCard4 c4BytesRead;
        eRes = eReadPacketData(thrCaller, sockSrc, hdrRead, c4BytesRead, *pmbufToUse);

        // Double check that he got what we asked for
        if (c4BytesRead != hdrRead.c4DataBytes)
        {
            CIDAssert2(L"Did not read the number of ORB packet bytes expected");

            // For now, return a lost connection as a punt
            return tCIDOrb::EReadRes::Lost;
        }

        //
        //  If it's still a good packet, then we need to gen up the work queue item
        //  and read in the passed data into its ORB command object.
        //
        if (eRes == tCIDOrb::EReadRes::Packet)
        {
            // Provisionally get a work item from the pool bit enough for the data
            TWorkQItemPtr wqipTmp(hdrRead.c4DataBytes);

            pstrmToUse->Reset();
            pstrmToUse->SetEndIndex(c4BytesRead);
            *pstrmToUse >> wqipTmp->ocmdThis();

            // It worked so give it back to the caller
            wqipNew = tCIDLib::ForceMove(wqipTmp);
        }
    }
    return eRes;
}


// Provide access to the remote client timeout adjust
tCIDLib::TEncodedTime TFacCIDOrb::enctTimeoutAdjust() const
{
    return m_enctTimeoutAdjust;
}


//
//  Remove all entries from the object id cache. This is typically called if
//  the CIDOrbUC facility class' helper method that returns a name server
//  proxy cannot connect to the name server.
//
tCIDLib::TVoid TFacCIDOrb::FlushOIDCache()
{
    // This is a single operation, so no need to lock explicitly
    try
    {
        m_colNSCache.RemoveAll();
    }

    catch(...)
    {
    }
}


//
//  Just returns the port number we are listening on. That might be different from what the
//  user passed us if he passed in zero. The client connection manager will return us the
//  port that was actually connected to.
//
tCIDLib::TIPPortNum TFacCIDOrb::ippnORB() const
{
    #if CID_DEBUG_ON
    if (!m_atomServerInit)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_NotInitialized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    #endif

    return m_poccmSrv->ippnListen();
}


//
//  Initializes the client side orb support. This doesn't involve a whole
//  lot of work. We call the orb client base's static init, and he does
//  all the real work.
//
//  We assume this is done during startup when there are no threading
//  issues.
//
tCIDLib::TVoid TFacCIDOrb::InitClient()
{
    // If we aren't already initialized, then do it
    if (!m_atomClientInit)
    {
        TBaseLock lockInit;
        if (!m_atomClientInit)
        {
            //  Reset some client side stuff
            m_c8LastNSCookie = 0;

            // Init the client support
            TOrbClientBase::InitializeOrbClient();

            // If verbose logging, then log a client initialized message
            if (bLogStatus())
            {
                LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_ClientInited
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::AppStatus
                );
            }

            // Indicate that client support is initialized (do this last!)
            m_atomClientInit.Set();
        }
    }
}


//
//  Cranks up the server side support if not already going. Note that we
//  only create stuff that hasn't been created already, since some apps
//  start and stop the server side support and therefore some stuff may
//  have been created already.
//
//  We assume this is done during startup when there are no threading
//  issues.
//
tCIDLib::TVoid
TFacCIDOrb::InitServer( const   tCIDLib::TIPPortNum ippnListen
                        , const tCIDLib::TCard4     c4MaxClients)
{
    // If we aren't initialized, then do it
    if (!m_atomServerInit)
    {
        TBaseLock lockInit;
        if (!m_atomServerInit)
        {
            // Initialize some server side stuff
            TStatsCache::SetValue(m_sciActiveCmds, 0);
            TStatsCache::SetValue(m_sciQueuedCmds, 0);
            TStatsCache::SetValue(m_sciRegisteredObjs, 0);
            m_scntActiveCmds.c4SetValue(0);


            // Initialize the core Orb server support
            TOrbServerBase::InitializeOrbServer();

            //
            //  And create and initialize the client connection manager.  Once we do this,
            //  clients can start connecting. Note that if the port was zero, this will
            //  select one. But our ippnOrb() method above gets it from this object so that
            //  is ok.
            //
            if (!m_poccmSrv)
                m_poccmSrv = new TOrbClientConnMgr(ippnListen, c4MaxClients);

            // Indicate that server support is initialized (do this last!)
            m_atomServerInit.Set();

            // If verbose logging, then log a server initialized message
            if (bLogInfo())
            {
                LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbMsgs::midStatus_ServerInited
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::AppStatus
                );
            }
        }
    }
}


//
//  Tells us to only accept client connections from a given address. We just
//  pass this on to the client connection manager if he exists.
//
tCIDLib::TVoid TFacCIDOrb::OnlyAcceptFrom(const TIPAddress& ipaSource)
{
    if (m_poccmSrv)
        m_poccmSrv->OnlyAcceptFrom(ipaSource);
}


//
//  Dynamically create a client proxy from the indicated object id. It'll be
//  a generic client proxy, seen through the base client class. But that is
//  sometimes used just to query stats or to check for connectivity.
//
TOrbClientBase* TFacCIDOrb::porbcDynamic(const TOrbObjId& ooidToUse)
{
    //
    //  Do a dynamic create by class name, using the client proxy class in
    //  the object id.
    //
    TOrbClientBase* porbcRet =
        ::pobjMakeNewOfClass<TOrbClientBase>(ooidToUse.strClientProxyClass());

    // Now set it up, post facto
    porbcRet->SetObjId(ooidToUse, TString::strEmpty());

    // And now its happy so return it
    return porbcRet;
}


//
//  We set a fairly short timeout on object id cache objects. But, each time
//  one of the core ones is used successfully, it will be refreshed and so
//  it's time out time will continue to be pushed forward as long as it is
//  known to be working.
//
//  They have to pass in the actual object id as well, so that we can compare
//  it to the cached one. Otherwise, they might extend the time of an old one
//  in the cache, when they are using a newer one.
//
//  If it's not the same as the one cached under this binding, we just don't
//  refresh. If we don't find the binding, we store it in the cache.
//
tCIDLib::TVoid
TFacCIDOrb::RefreshObjIDCache(  const   TString&    strBindingName
                                , const TOrbObjId&  ooidToCheck)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());

    // Calculate the new timeout time
    const tCIDLib::TEncodedTime enctNewTO
    (
        TTime::enctNow() + CIDOrb_ThisFacility::enctCacheTO
    );

    // See if this one is in the cache
    TObjIdCache::TPair* pkobjBinding = m_colNSCache.pkobjFindByKey(strBindingName);
    if (pkobjBinding)
    {
        // We found it. See if the object ids are the same
        if (pkobjBinding->objValue() == ooidToCheck)
        {
            // It's the same guy, so update the timeout time
            pkobjBinding->objValue().enctCache(enctNewTO);
        }
    }
     else
    {
        //
        //  Not in the cache yet, so add it. This would be sort of unusual,
        //  but we don't want to waste the opportunity to cache it.
        //
        TObjIdCache::TPair& kobjNew = m_colNSCache.kobjAdd
        (
            strBindingName, ooidToCheck
        );
        kobjNew.objValue().enctCache(enctNewTO);
    }
}


//
//  Registers a server side object with the ORB. WE adopt it. The object id
//  should already have been set on it. We will call it's initialize method,
//  and then add it to the object list.
//
tCIDLib::TVoid
TFacCIDOrb::RegisterObject(         TOrbServerBase* const   porbsToReg
                            , const tCIDLib::EAdoptOpts     eAdopt)
{
    #if CID_DEBUG_ON
    if (!m_atomServerInit)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_NotInitialized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    if (!porbsToReg)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_NulObject
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    #endif

    //
    //  If we are adopting it, make sure we get it cleaned up if it fails the
    //  init. If not, we pass a null and just leave it alone if init throws.
    //
    TJanitor<TOrbServerBase> janTmp(porbsToReg, eAdopt == tCIDLib::EAdoptOpts::Adopt);
    porbsToReg->Initialize();

    {
        TCritSecLocker lockObjList(&m_crsObjList);
        m_colObjList.Add(janTmp.pobjOrphan(), eAdopt);
    }
    TStatsCache::c8IncCounter(m_sciRegisteredObjs);
}


// Removes the passed binding from the object id cache if it exists
tCIDLib::TVoid TFacCIDOrb::RemoveFromOIDCache(const TString& strBindingName)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());
    m_colNSCache.bRemoveIfExists(strBindingName);
}


//
//  Removes the passed object id from the object id cache. This is called when
//  the client base class sees an object not found error coming back from a
//  remote call. In order to allow for recovery, we need to remove that object
//  id from the object id cache.
//
//  In this case we have to just do a brute force search of the cache to find
//  the indicated object id, and possibly to find it under more than one
//  binding. So we search the whole cache and do an object id comparison,
//  though they do hash themselves so it's not quite as bad as it could be.
//
tCIDLib::TVoid TFacCIDOrb::RemoveFromOIDCache(const TOrbObjId& ooidToRemove)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());

    try
    {
        TObjIdCache::TNCCursor cursCache(&m_colNSCache);
        while (cursCache.bIsValid())
        {
            TObjIdCache::TPair& kobjCur = *cursCache;
            if (kobjCur.objValue() == ooidToRemove)
                m_colNSCache.RemoveAt(cursCache);
            else
                ++cursCache;
        }
    }

    catch(...)
    {
    }
}



//
//  Our client and server sides create threads to spool input and output
//  back and forth. They all need to have unique names, so we provide a
//  method to create them.
//
TString TFacCIDOrb::strNextSpoolThreadName(const tCIDLib::TBoolean bServer)
{
    if (bServer)
        return facCIDLib().strNextThreadName(L"CIDOrbSrvSpool");

    return facCIDLib().strNextThreadName(L"CIDOrbClientSpool");
}


//
//  We provide the low level send/receive methods here in this class so that
//  we can do things like apply encryption to incoming and outgoing packets.
//
tCIDLib::TVoid
TFacCIDOrb::SendMsg(        TStreamSocket&          sockTar
                    , const tCIDOrb::TPacketHdr&    hdrToSend
                    , const TMemBuf&                mbufData)
{
    //
    //  And let's send the header first, then the packet. If encrypted, the
    //  header's data hash is on the plain text, not the encrypted text.
    //
    sockTar.SendThrottled
    (
        &hdrToSend
        , kCIDLib::enctFiveSeconds + m_enctTimeoutAdjust
        , sizeof(tCIDOrb::TPacketHdr)
    );

    //
    //  If encrypting, then we need to encrypt the data first, and then
    //  send it.
    //
    if (m_pcrypSecure)
    {
        //
        //  We have to stay reentrant here, and we don't want to allocate
        //  and free a buffer for every single reply. So we use a local
        //  buffer and encrypt and send the data a chunk at a time. Most
        //  replies will fit in a single chunk anyway.
        //
        const tCIDLib::TCard4 c4BufSz = 4096;
        const tCIDLib::TCard1* pc1ToSend = mbufData.pc1Data();
        const tCIDLib::TCard1* pc1End = pc1ToSend + hdrToSend.c4DataBytes;
        tCIDLib::TCard1 ac1Encrypt[c4BufSz];
        while (pc1ToSend < pc1End)
        {
            //
            //  Figure out how much to read this time. It's either a
            //  buffer's worth, or enough to finish the packet.
            //
            tCIDLib::TCard4 c4ToSend = pc1End - pc1ToSend;
            if (c4ToSend > c4BufSz)
                c4ToSend = c4BufSz;

            // Encrypt another chunk
            const tCIDLib::TCard4 c4CryptBytes = m_pcrypSecure->c4Encrypt
            (
                pc1ToSend, ac1Encrypt, c4ToSend, c4BufSz
            );

            //
            //  Send this chunk and move the input pointer up, by the
            //  bytes eaten, not bytes sent since the encrypter will
            //  pad up to the block size. So we send the encrypted count
            //  and move up by the source bytes we encrypted.
            //
            sockTar.SendThrottled
            (
                ac1Encrypt
                , kCIDLib::enctFiveSeconds + m_enctTimeoutAdjust
                , c4CryptBytes
            );
            pc1ToSend += c4ToSend;
        }
    }
     else
    {
        sockTar.SendThrottled
        (
            mbufData
            , kCIDLib::enctFiveSeconds + m_enctTimeoutAdjust
            , hdrToSend.c4DataBytes
        );
    }
}


//
//  When an encrypter is set on the ORB, each packet sent out and recieved
//  is encrypted/decrypted using this encrypter. It can be null to disable
//  encryption.
//
tCIDLib::TVoid TFacCIDOrb::SetEncrypter(TBlockEncrypter* const pcrypToAdopt)
{
    // This can only be done before the ORB is started up
    if (m_atomServerInit || m_atomClientInit)
    {
        // Clean up the encrypter first, since we own it
        delete pcrypToAdopt;

        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcGen_NotWhenRunning
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // If we currently have one, then clean it up
    if (m_pcrypSecure)
    {
        delete m_pcrypSecure;
        m_pcrypSecure = nullptr;
    }

    // And store the new one. It can be null sohave to suppress warning
    #pragma warning(suppress : 6001)
    m_pcrypSecure = pcrypToAdopt;
}



//
//  Stores the passed object id into the object id cache and sets its
//  time to when it should time out.
//
//  If the cache is full, we first do a sweep through it in order to
//  get rid of anything that's out of date. If we still are full, we'll
//  do a panic and just clear the cache and start over.
//
tCIDLib::TVoid
TFacCIDOrb::StoreObjIDCache(const   TString&    strBindingName
                            , const TOrbObjId&  ooidToStore)
{
    TMtxLocker mtxlCache(m_colNSCache.pmtxLock());

    const tCIDLib::TEncodedTime enctNow = TTime::enctNow();

    //
    //  If we should get full, we just make a pass through the whole cache
    //  and try to remove anything out of date.
    //
    if (m_colNSCache.bIsFull(CIDOrb_ThisFacility::c4MaxNSQSize))
    {
        TObjIdCache::TNCCursor cursCache(&m_colNSCache);
        while (cursCache.bIsValid())
        {
            TObjIdCache::TPair& kobjCur = cursCache.objWCur();
            if (kobjCur.objValue().enctCache() < enctNow)
                m_colNSCache.RemoveAt(cursCache);
            else
                ++cursCache;
        }

        // If still full, just clear it
        if (m_colNSCache.bIsFull(CIDOrb_ThisFacility::c4MaxNSQSize))
            m_colNSCache.RemoveAll();
    }

    // Add a new key/value pair to the map if not already there
    tCIDLib::TBoolean bAdded;
    TObjIdCache::TPair& kobjNew = m_colNSCache.kobjAddIfNew
    (
        strBindingName, ooidToStore, bAdded
    );

    //
    //  Update the cache time. They will only call us if they just
    //  successfully connected using this object id, so we know it's
    //  still good.
    //
    kobjNew.objValue().enctCache(enctNow + CIDOrb_ThisFacility::enctCacheTO);
}



//
//  Stops the ORB (both sides if they are started) and cleans up.) The application
//  should call this proactively. If they don't we call it in our destructor.
//
tCIDLib::TVoid TFacCIDOrb::Terminate()
{
    // If in verbose logging mode, log that we started the termination
    if (bLogInfo())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbMsgs::midStatus_TermStarted
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    // Stop the monitor thread
    try
    {
        m_thrMonitor.ReqShutdownSync(5000);
        m_thrMonitor.eWaitForDeath(4000);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        LogEventObj(errToCatch);
    }

    if (m_atomClientInit)
    {
        // Clear the flag first
        m_atomClientInit.SetValue(kCIDLib::False);

        TOrbClientBase::TerminateOrbClient();

        // Clear out the name server cache now
        m_colNSCache.RemoveAll();

        // If in verbose logging mode, log that we did the client term ok
        if (bLogInfo())
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ClientTermDone
                , tCIDLib::ESeverities::Info
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }

    if (m_atomServerInit)
    {
        // Clear the flag first
        m_atomServerInit.SetValue(kCIDLib::False);

        //
        //  Tell the client connection manager to stop accepting client
        //  connections and shut down the listener thread and existing
        //  client connections.
        //
        m_poccmSrv->Terminate();

        //
        //  Call the terminate method on all of the server objects. Catch
        //  an errors so that we get them all cleaned up.
        //
        if (m_colObjList.bResetIter())
        {
            do
            {
                try
                {
                    m_colObjList.objCur().Terminate();
                }

                catch(TError& errToCatch)
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    LogEventObj(errToCatch);

                    facCIDOrb().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcServ_SrvObjTermError
                        , errToCatch.strErrText()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::NotReady
                        , m_colObjList.objCur().ooidThis()
                    );
                }

                catch(...)
                {
                    facCIDOrb().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kOrbErrs::errcServ_SrvObjTermError
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::NotReady
                        , m_colObjList.objCur().ooidThis()
                    );
                }
            }   while (m_colObjList.bNext());
        }

        // Zero our server side stats
        TStatsCache::SetValue(m_sciActiveCmds, 0);
        TStatsCache::SetValue(m_sciQueuedCmds, 0);
        TStatsCache::SetValue(m_sciRegisteredObjs, 0);

        //
        //  Clean up the client connection manager if we have one. Thi
        //   shuts down all the server side stuff.
        //
        try
        {
            delete m_poccmSrv;
        }
        catch(...)
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , L"Failed to clean up client connection manager"
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }
        m_poccmSrv = nullptr;

        // Clean up the encrypter if we have one
        try
        {
            delete m_pcrypSecure;
        }
        catch(...)
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , L"Failed to clean up ORB encrypter"
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }
        m_pcrypSecure = nullptr;

        // Now terminate the server side Orb support
        TOrbServerBase::TerminateOrbServer();
    }

    // If in more verbose logging mode, log that we are cleaned up
    if (bLogInfo())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbMsgs::midStatus_TermDone
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }
}




// ---------------------------------------------------------------------------
//  TFacCIDOrb: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::EExitCodes TFacCIDOrb::eMonThread(TThread& thrThis, tCIDLib::TVoid*)
{
    thrThis.Sync();

    // And now loop till asked to shut down
    while (kCIDLib::True)
    {
        try
        {
            // Sleep a while and wait for a shutdown request as we do
            if (!thrThis.bSleep(2000))
                break;

            // Update our various stats
            TStatsCache::SetValue(m_sciWorkQItems, TWorkQItem::c4UsedQItems());

            if (m_poccmSrv)
                TStatsCache::SetValue(m_sciQueuedCmds, m_poccmSrv->c4QueuedCmds());

            TStatsCache::SetValue(m_sciActiveCmds, m_scntActiveCmds.c4Value());
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


//
//  Once eReadPacketHdr() has been called to get the newly arriving
//  header, this method is called to read in the data. We have to do it
//  in a very careful way to avoid getting locking up for long periods
//  of time, but still being tolerant of slower clients.
//
tCIDOrb::EReadRes
TFacCIDOrb::eReadPacketData(        TThread&                thrCaller
                            ,       TStreamSocket&          sockSrc
                            , const tCIDOrb::TPacketHdr&    hdrRead
                            ,       tCIDLib::TCard4&        c4DataBytes
                            ,       TMemBuf&                mbufToFill)
{
    //
    //  We shouldn't be called if there are no data bytes to read, but
    //  if we are, then return packet status.
    //
    if (!hdrRead.c4DataBytes)
        return tCIDOrb::EReadRes::Packet;

    //
    //  We use c4DataBytes as an accumulator, since we cannot use the
    //  incoming header's data byte count to return to the caller. If
    //  encrypted, the decrypted data will likely be smaller due to
    //  discarded padding. c4SoFar below is used to keep up with raw
    //  bytes read till we read the whole packet, not actual returnable
    //  bytes.
    //
    c4DataBytes = 0;

    //
    //  Ok, read the number of bytes we were told to. What we want to do
    //  here is to be lenient, since they could be sending a good bit of
    //  data in some cases. But, we cannot just wait for a long time, since
    //  others are waiting. So, we have an overall timeout that's pretty
    //  long but we wait for one second chunks at a time. If we don't get
    //  some data on X number of rounds in a row, we consider that a timeout
    //  because the caller sends the data right after sending the header
    //  and it should basically already be here. The only exception is
    //  a large image, which will take a while to come over, but it
    //  should not take longer than 5 seconds on a local network. On a
    //  remote network (or wireless client with slow connection) they will
    //  have set the remote mode and we will wait longer and allow more
    //  timeouts before we give up.
    //
    const tCIDLib::TEncodedTime enctWait
    (
        (6 * kCIDLib::enctOneSecond) + m_enctTimeoutAdjust
    );

    try
    {
        const tCIDLib::TCard4   c4BufSz = 32 * 1024;
        const tCIDLib::TCard4   c4MaxTimeouts = m_c4TimeoutAdjust ? 5 : 3;

        tCIDLib::TCard1         ac1Buf[c4BufSz];
        tCIDLib::TCard1         ac1Decrypt[c4BufSz];
        tCIDLib::TCard4         c4SoFar = 0;
        tCIDLib::TCard4         c4Timeouts = 0;
        tCIDLib::TCard4         c4ThisTime;
        tCIDLib::TEncodedTime   enctCur = TTime::enctNow();
        tCIDLib::TEncodedTime   enctEnd = enctCur + enctWait;
        tCIDLib::TEncodedTime   enctCurWait;

        while ((enctCur < enctEnd) && (c4SoFar < hdrRead.c4DataBytes))
        {
            // Check for shutdown requests
            if (thrCaller.bCheckShutdownRequest())
                return tCIDOrb::EReadRes::Lost;

            //
            //  Figure out how much to read this time. It's either a
            //  buffer's worth, or enough to finish the packet.
            //
            tCIDLib::TCard4 c4ToRead = hdrRead.c4DataBytes - c4SoFar;
            if (c4ToRead > c4BufSz)
                c4ToRead = c4BufSz;

            // And wait for up to another chunk's worth of time
            enctCurWait = enctEnd - enctCur;
            if (enctCurWait > kCIDLib::enctOneSecond)
                enctCurWait = kCIDLib::enctOneSecond;

            c4ThisTime = sockSrc.c4ReceiveRawTO
            (
                ac1Buf, enctCurWait, c4ToRead, tCIDLib::EAllData::OkIfNotAll
            );

            // If we got something, then clear the timeouts, else bump it
            if (c4ThisTime)
            {
                c4Timeouts = 0;
            }
             else
            {
                c4Timeouts++;

                // Check for a loss of connection
                if (!sockSrc.bIsConnected())
                    return tCIDOrb::EReadRes::Lost;
            }

            // If we've timed out too many times in a row, then give up
            if (c4Timeouts >= c4MaxTimeouts)
                break;

            // If we got something this time...
            if (c4ThisTime)
            {
                //
                //  Put this data into the buffer, and bump up the so
                //  far count. If encrypting, then we need to decrypt it
                //  instead of just copying it.
                //
                if (m_pcrypSecure)
                {
                    const tCIDLib::TCard4 c4DBytes = m_pcrypSecure->c4Decrypt
                    (
                        ac1Buf, ac1Decrypt, c4ThisTime, c4BufSz
                    );

                    //
                    //  Copy in the decrypted count, not the bytes read,
                    //  since the encrypter will pad to the block size,
                    //  so the bytes decrypted can be smaller.
                    //
                    mbufToFill.CopyIn(ac1Decrypt, c4DBytes, c4SoFar);
                    c4DataBytes += c4DBytes;
                }
                else
                {
                    mbufToFill.CopyIn(ac1Buf, c4ThisTime, c4SoFar);
                    c4DataBytes += c4ThisTime;
                }
                c4SoFar += c4ThisTime;
            }
            enctCur = TTime::enctNow();
        }

        // If we didn't get a full whack, we failed
        if (c4SoFar < hdrRead.c4DataBytes)
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcComm_PartialPacket
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , sockSrc.ipepRemoteEndPoint()
            );
            return tCIDOrb::EReadRes::Lost;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // If we've lost the connection, then return that, else rethrow
        if (!sockSrc.bIsConnected())
            return tCIDOrb::EReadRes::Lost;
        throw;
    }

    //
    //  Check the hash of the data with what was in the header. If
    //  encrypted, we are hashing the plain text here.
    //
    const tCIDLib::THashVal hshData = mbufToFill.hshCalcHash
    (
        kCIDOrb_::c4PacketHashMod, 0, c4DataBytes
    );

    if (hshData != hdrRead.hshData)
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcComm_BadPacketHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , sockSrc.ipepRemoteEndPoint()
        );
        return tCIDOrb::EReadRes::Lost;
    }
    return tCIDOrb::EReadRes::Packet;
}


//
//  This method will wait for a packet header to arrive and give back the
//  header data. The caller can then react to this appropriately. Sometimes
//  there's nothing else to do, such as when we see a keepalive header,
//  since nothing more than a headedr is going to arrive.
//
tCIDOrb::EReadRes
TFacCIDOrb::eReadPacketHdr(         TThread&                thrCaller
                            ,       TStreamSocket&          sockSrc
                            , const tCIDLib::TCard4         c4WaitFor
                            ,       tCIDOrb::TPacketHdr&    hdrToFill)
{
    tCIDLib::TCard4 c4Read;
    try
    {
        //
        //  For efficiency, if we get a zero timeout, call the non-timeout
        //  version. It's much simpler internally and does what we want
        //  since our sockets are always non-blocking, so it'll return
        //  immediately with no bytes if nothing is ready.
        //
        if (c4WaitFor)
        {
            c4Read = sockSrc.c4ReceiveRawTOMS
            (
                &hdrToFill
                , c4WaitFor + m_c4TimeoutAdjust
                , sizeof(hdrToFill)
                , tCIDLib::EAllData::OkIfNotAll
            );
        }
         else
        {
            c4Read = sockSrc.c4ReceiveRaw(&hdrToFill, sizeof(hdrToFill));
        }

        //
        //  If no bytes at all, then we just didn't get anything or lost
        //  connection. Because we use a passive, event driven scheme, we
        //  need to check specifically because the first read that returns
        //  zero after a connection loss won't throw anything. So this
        //  let's use much more quickly see that we lost it.
        //
        if (!c4Read)
        {
            if (sockSrc.bIsConnected())
                return tCIDOrb::EReadRes::NoPacket;
            return tCIDOrb::EReadRes::Lost;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // If we've lost the connection, then return that, else rethrow
        if (!sockSrc.bIsConnected())
            return tCIDOrb::EReadRes::Lost;
        throw;
    }

    // Validate we got a header's worth
    if (c4Read != sizeof(hdrToFill))
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcComm_PartialHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
            , sockSrc.ipepRemoteEndPoint()
        );
        return tCIDOrb::EReadRes::Lost;
    }

    //
    //  Check for a keepalive ping type packet header. This is sent by the
    //  client side ORB to keep the connection from being dropped by us
    //  when the client side proxies aren't being used often enough to
    //  otherwise keep them alive.
    //
    //  99% of the time, it'll not check more than the first one, since
    //  the magic value will be the usual magic value.
    //
    if ((hdrToFill.c4MagicVal == 0xFEADBEAF)
    &&  (hdrToFill.c4MagicVal2 == 0xBEAFDEAD)
    &&  (hdrToFill.c4DataBytes == 0)
    &&  (hdrToFill.hshData == 0xFFFFFFFF)
    &&  (hdrToFill.c4SequenceId == 0x12345678))
    {
        // It so,so just return that we got a keepalive header
        return tCIDOrb::EReadRes::KeepAlive;
    }

    //
    //  If the magic values aren't right, assume we are hosed in some really
    //  pathological way and just return a lost connection.
    //
    if ((hdrToFill.c4MagicVal != kCIDOrb_::c4MagicVal)
    ||  (hdrToFill.c4MagicVal2 != kCIDOrb_::c4MagicVal2))
    {
        facCIDOrb().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcComm_BadHdrMagicVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , sockSrc.ipepRemoteEndPoint()
        );
        return tCIDOrb::EReadRes::Lost;
    }
    return tCIDOrb::EReadRes::Packet;
}



