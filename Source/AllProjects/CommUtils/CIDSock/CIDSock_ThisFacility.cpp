//
// FILE NAME: CIDSock_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/06/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TFacCIDSock class.
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
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(THostAdapter, TObject)
RTTIDecls(TFacCIDSock,TFacility)



// ---------------------------------------------------------------------------
//  We need a little class for our name to IP address cache. We have to keep
//  a hash set of these.
// ---------------------------------------------------------------------------
class TIPLookupItem
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TIPLookupItem& ipliSrc)
        {
            return ipliSrc.m_strKey;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TIPLookupItem(  const   TString&        strKey
                        , const TKrnlIPAddr&    kipaCache) :

            m_enctEnd(0)
            , m_kipaCache(kipaCache)
            , m_strKey(strKey)
        {
            // We only keep them for 30 seconds at a time
            m_enctEnd = TTime::enctNowPlusSecs(30);
        }

        TIPLookupItem(const TIPLookupItem& ipliSrc) :

            m_enctEnd(ipliSrc.m_enctEnd)
            , m_kipaCache(ipliSrc.m_kipaCache)
            , m_strKey(ipliSrc.m_strKey)
        {
        }

        ~TIPLookupItem()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIPLookupItem& operator=(const TIPLookupItem& ipliSrc)
        {
            if (&ipliSrc  != this)
            {
                m_enctEnd   = ipliSrc.m_enctEnd;
                m_kipaCache = ipliSrc.m_kipaCache;
                m_strKey    = ipliSrc.m_strKey;
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public class members
        //
        //  m_enctEnd
        //      The end time for this cached item. Once we go over this time,
        //      the name lookup has to be done again.
        //
        //  m_kipaCache
        //      The cached IP address. We store kernel level address objects
        //      here.
        //
        //  m_strKey
        //      This is a key that is a combination of the address text and
        //      the IP address type, so that it's unique for a given target
        //      address and IPV4/6 combination.
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctEnd;
        TKrnlIPAddr             m_kipaCache;
        TString                 m_strKey;
};



// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDSock_ThisFacility
{
    // -----------------------------------------------------------------------
    //  We implement a name to IP address cache. We use a keyed has set. We
    //  make it thread safe so that we can use its mutex for sync. We make the
    //  modulus fairly large relative to the size, so that we minimimze search
    //  time here. This is a very performance sensitive cache.
    //
    //  We also provide a string for building up keys. Since we have to sync
    //  anyway, we can avoid constantly creating/destroying a string.
    //
    //  And we have a time stamp that we use to go through and fully flush any
    //  out of date items every couple hours, so that it doesn't accumulate
    //  with entries that never get used again. And, should it get full, we'll
    //  also do a pass to remove out of date entries.
    // -----------------------------------------------------------------------
    using TIPCache = TKeyedHashSet<TIPLookupItem, TString, TStringKeyOps>;
    TIPCache colNameCache
    (
        173
        , new TStringKeyOps
        , &TIPLookupItem::strKey
        , tCIDLib::EMTStates::Safe
    );
    const tCIDLib::TCard4 c4MaxCacheItems = 2048;
    TString strKeyTemp;
    tCIDLib::TEncodedTime enctFlushPass = 0;
}



// ---------------------------------------------------------------------------
//  A local helper to build up name lookup cache keys
// ---------------------------------------------------------------------------
static tCIDLib::TVoid
MakeCacheKey(const  TString&                strAddr
            , const tCIDSock::EAddrTypes    eType
            ,       TString&                strKey)
{
    strKey = strAddr;
    strKey.Append(kCIDLib::chUnderscore);
    if (eType == tCIDSock::EAddrTypes::Unspec)
        strKey.Append(L"_U");
    else if (eType == tCIDSock::EAddrTypes::IPV4)
        strKey.Append(L"_4");
    else
        strKey.Append(L"_6");
}




// ---------------------------------------------------------------------------
//  CLASS: THostAdapter
// PREFIX: hadp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THostAdapter: Constructors and Destructor
// ---------------------------------------------------------------------------
THostAdapter::THostAdapter() :

    m_bDHCPEnabled(kCIDLib::False)
    , m_bIPV4Enabled(kCIDLib::False)
    , m_bIPV6Enabled(kCIDLib::False)
    , m_bIsDedicated(kCIDLib::False)
    , m_bIsLoopback(kCIDLib::False)
    , m_bIsReady(kCIDLib::False)
    , m_bIsTunnel(kCIDLib::False)
    , m_c4AddrCnt(0)
    , m_c4HWAddrSz(0)
    , m_strDescr()
    , m_strName()
{
    TRawMem::SetMemBuf(m_ac1HWAddr, tCIDLib::TCard1(0), kCIDSock::c4MaxHWAddrSz);
}

THostAdapter::THostAdapter(const THostAdapter& hadpSrc) :

    m_bDHCPEnabled(hadpSrc.m_bDHCPEnabled)
    , m_bIPV4Enabled(hadpSrc.m_bIPV4Enabled)
    , m_bIPV6Enabled(hadpSrc.m_bIPV6Enabled)
    , m_bIsDedicated(hadpSrc.m_bIsDedicated)
    , m_bIsLoopback(hadpSrc.m_bIsLoopback)
    , m_bIsReady(hadpSrc.m_bIsReady)
    , m_bIsTunnel(hadpSrc.m_bIsTunnel)
    , m_c4AddrCnt(hadpSrc.m_c4AddrCnt)
    , m_c4HWAddrSz(hadpSrc.m_c4HWAddrSz)
    , m_strDescr(hadpSrc.m_strDescr)
    , m_strName(hadpSrc.m_strName)
{
    TRawMem::CopyMemBuf(m_ac1HWAddr, hadpSrc.m_ac1HWAddr, kCIDSock::c4MaxHWAddrSz);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDSock::c4MaxAddrs; c4Index++)
        m_aipaAddrs[c4Index] = hadpSrc.m_aipaAddrs[c4Index];
}

THostAdapter::~THostAdapter()
{
}


// ---------------------------------------------------------------------------
//  THostAdapter: Public operators
// ---------------------------------------------------------------------------
THostAdapter& THostAdapter::operator=(const THostAdapter& hadpSrc)
{
    if (this != &hadpSrc)
    {
        m_bDHCPEnabled  = hadpSrc.m_bDHCPEnabled;
        m_bIPV4Enabled  = hadpSrc.m_bIPV4Enabled;
        m_bIPV6Enabled  = hadpSrc.m_bIPV6Enabled;
        m_bIsDedicated  = hadpSrc.m_bIsDedicated;
        m_bIsLoopback   = hadpSrc.m_bIsLoopback;
        m_bIsReady      = hadpSrc.m_bIsReady;
        m_bIsTunnel     = hadpSrc.m_bIsTunnel;

        m_c4AddrCnt     = hadpSrc.m_c4AddrCnt;
        m_c4HWAddrSz    = hadpSrc.m_c4HWAddrSz;
        m_strDescr      = hadpSrc.m_strDescr;
        m_strName       = hadpSrc.m_strName;

        TRawMem::CopyMemBuf(m_ac1HWAddr, hadpSrc.m_ac1HWAddr, kCIDSock::c4MaxHWAddrSz);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDSock::c4MaxAddrs; c4Index++)
            m_aipaAddrs[c4Index] = hadpSrc.m_aipaAddrs[c4Index];
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  THostAdapter: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return some capabilities flags
tCIDLib::TBoolean THostAdapter::bDHCPEnabled() const
{
    return m_bDHCPEnabled;
}

tCIDLib::TBoolean THostAdapter::bIPV4Enabled() const
{
    return m_bIPV4Enabled;
}

tCIDLib::TBoolean THostAdapter::bIPV6Enabled() const
{
    return m_bIPV6Enabled;
}

tCIDLib::TBoolean THostAdapter::bIsDedicated() const
{
    return m_bIsDedicated;
}

tCIDLib::TBoolean THostAdapter::bIsLoopback() const
{
    return m_bIsLoopback;
}

tCIDLib::TBoolean THostAdapter::bIsReady() const
{
    return m_bIsReady;
}

tCIDLib::TBoolean THostAdapter::bIsTunnel() const
{
    return m_bIsTunnel;
}


// Access specific bytes in our hardware address
tCIDLib::TCard1 THostAdapter::c1HWAddrByteAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4HWAddrSz)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , clsIsA()
            , TCardinal(m_c4HWAddrSz)
        );
    }

    return m_ac1HWAddr[c4At];
}



tCIDLib::TCard4
THostAdapter::c4QueryHWAddr(        tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4MaxBytes) const
{
    if (c4MaxBytes < m_c4HWAddrSz)
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_HWAddrBufSz
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
            , TCardinal(c4MaxBytes)
            , TCardinal(m_c4HWAddrSz)
        );
    }

    TRawMem::CopyMemBuf(pc1ToFill, m_ac1HWAddr, m_c4HWAddrSz);
    return m_c4HWAddrSz;
}


tCIDLib::TCard4 THostAdapter::c4AddrCnt() const
{
    return m_c4AddrCnt;
}


tCIDLib::TCard4 THostAdapter::c4HWAddrSz() const
{
    return m_c4HWAddrSz;
}


const TIPAddress& THostAdapter::ipaAddrAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4AddrCnt)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , clsIsA()
            , TCardinal(m_c4AddrCnt)
        );
    }
    return m_aipaAddrs[c4At];
}


const TString& THostAdapter::strDescr() const
{
    return m_strDescr;
}


const TString& THostAdapter::strName() const
{
    return m_strName;
}


// ---------------------------------------------------------------------------
//  THostAdapter: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid THostAdapter::LoadFrom(const TKrnlIP::TAdaptorInfo& kadpSrc)
{
    m_bDHCPEnabled  = kadpSrc.bDHCPEnabled;
    m_bIPV4Enabled  = kadpSrc.bIPV4Enabled;
    m_bIPV6Enabled  = kadpSrc.bIPV6Enabled;
    m_bIsDedicated  = kadpSrc.bDedicated;
    m_bIsLoopback   = kadpSrc.bLoopback;
    m_bIsReady      = kadpSrc.bReady;
    m_bIsTunnel     = kadpSrc.bTunnel;
    m_c4AddrCnt     = kadpSrc.c4AddrCnt;
    m_c4HWAddrSz    = kadpSrc.c4HWAddrSz;
    m_strDescr      = kadpSrc.szDescr;
    m_strName       = kadpSrc.szName;

    TRawMem::CopyMemBuf(m_ac1HWAddr, kadpSrc.ac1HWAddr, kCIDSock::c4MaxHWAddrSz);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDSock::c4MaxAddrs; c4Index++)
        m_aipaAddrs[c4Index] = TIPAddress(kadpSrc.akipaAddrs[c4Index]);
}




// ---------------------------------------------------------------------------
//    CLASS: TFacCIDSock
//   PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDSock: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDSock::TFacCIDSock() :

    TFacility
    (
        L"CIDSock"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
    //
    //  Query the information that we cache and hand out to callers. We
    //  store the TCP/IP version and the host name.
    //
    if (!TKrnlIP::bQueryTCPVersion(m_c4MajVersion, m_c4MinVersion))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_GetIPVersion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store the local host name
    TKrnlString kstrHost;
    if (!TKrnlIP::bQueryLocalName(kstrHost))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_GetIPHostName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    m_strHostName = kstrHost.pszValue();
}

TFacCIDSock::~TFacCIDSock()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDSock: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Queries host adaptor information on the local named adaptor. Returns false
//  if not found. There is another version below that will throw if not found.
//
tCIDLib::TBoolean
TFacCIDSock::bQueryAdapterInfo( const   TString&        strName
                                ,       THostAdapter&   hadpToFill) const
{
    tCIDLib::TBoolean        bFound;
    TKrnlIP::TAdaptorInfo kadpToFill;
    if (!TKrnlIP::bQueryAdaptorInfo(strName.pszBuffer(), kadpToFill, bFound))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryAdapterByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName
        );
    }

    if (bFound)
        hadpToFill.LoadFrom(kadpToFill);
    return bFound;
}


//
//  Just tries to convert the passed string into an address. The string might
//  be a dotted address or a host name. It will try to convert it both ways,
//  assuming a dotted address first and if that fails it'll do a name
//  resolution.
//
//  If it ends up being a host resolution, the caller can indicate the required
//  address type.
//
//  Note that this indirectly calls eResolveAddr() below, so that caching is
//  still done.
//
tCIDLib::TBoolean
TFacCIDSock::bResolveAddr(  const   TString&                strAddr
                            ,       TIPAddress&             ipaToFill
                            , const tCIDSock::EAddrTypes    eType) const
{
    try
    {
        ipaToFill = TIPAddress(strAddr, eType);
    }

    catch(TError& errToCatch)
    {
        if (facCIDSock().bLogInfo() && !errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Returns a collection of host adapter information objects for this machine
tCIDLib::TCard4
TFacCIDSock::c4QueryAdapterList(TCollection<THostAdapter>& colToFill) const
{
    TKrnlLList<TKrnlIP::TAdaptorInfo> kllstList;

    if (!TKrnlIP::bQueryAdaptorList(kllstList))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryAdapterInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    colToFill.RemoveAll();
    if (kllstList.bResetCursor())
    {
        THostAdapter hadpTmp;

        TKrnlIP::TAdaptorInfo* pkadpCur;
        while(kllstList.bNext(pkadpCur))
        {
            hadpTmp.LoadFrom(*pkadpCur);
            colToFill.objAdd(hadpTmp);
        }
    }
    return colToFill.c4ElemCount();
}


// Return the available address type, which can be unspecified if we support both
tCIDSock::EAddrTypes TFacCIDSock::eAvailAddrType() const
{
    if (TKrnlIP::bIPV6Avail() && TKrnlIP::bIPV4Avail())
        return tCIDSock::EAddrTypes::Unspec;

    if (TKrnlIP::bIPV6Avail())
        return tCIDSock::EAddrTypes::IPV6;

    return tCIDSock::EAddrTypes::IPV4;
}


// Return the preferred address type supported
tCIDSock::EAddrTypes TFacCIDSock::ePrefAddrType() const
{
    return TKrnlIP::eDefAddrType();
}


//
//  Similar to the other version above, but instead of just returning a true
//  or false result, it gives back some more info as to what happened.
//
//  The main version of this returns a kernel level IP address, since some other
//  classes in this facility need to get one in that format. We also have
//  another that most everyone else will use which just calls the main version
//  and then applies that to the caller's CIDSock level IP address object.
//
//  NOTE: We use a shared temp string to build the key, so we have to access it
//  within the lock as well! This means we don't have to constantly create and
//  destroy a string to do a lookup.
//
tCIDSock::EAddrCvtRes
TFacCIDSock::eResolveAddr(  const   TString&                strAddr
                            ,       TIPAddress&             ipaToFill
                            , const tCIDSock::EAddrTypes    eType) const
{
    // Call the other version to get the kernel level address
    TKrnlIPAddr kipaTmp;
    tCIDSock::EAddrCvtRes eRes = eResolveAddr(strAddr, kipaTmp, eType);

    // If it didn't fail, assign it to the caller's higher level object
    if (eRes != tCIDSock::EAddrCvtRes::Failed)
        ipaToFill = kipaTmp;
    return eRes;
}

tCIDSock::EAddrCvtRes
TFacCIDSock::eResolveAddr(  const   TString&                strAddr
                            ,       TKrnlIPAddr&            kipaToFill
                            , const tCIDSock::EAddrTypes    eType) const
{
    //
    //  See if we have a cached entry for this address and IP address type.
    //  If so, and it's not out of date, then return it.
    //
    {
        TMtxLocker mtxlSync(CIDSock_ThisFacility::colNameCache.pmtxLock());

        // Create the cache key for this address and type
        MakeCacheKey(strAddr, eType, CIDSock_ThisFacility::strKeyTemp);
        TIPLookupItem* pipliFind = nullptr;

        //
        //  First see if we need to do a full flush pass. We do this
        //  periodically to prevent the cache from growing over time with
        //  addresses that never get reused (and which would therefore never
        //  get removed.)
        //
        //  We'll also do it if the cache is full, in order to make some
        //  space.
        //
        const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
        if ((CIDSock_ThisFacility::enctFlushPass < enctNow)
        ||  CIDSock_ThisFacility::colNameCache.bIsFull(CIDSock_ThisFacility::c4MaxCacheItems))
        {
            // Reset the time stamp to an hour from now
            CIDSock_ThisFacility::enctFlushPass = enctNow + kCIDLib::enctOneHour;

            // And use a cursor to go all the way through the list
            CIDSock_ThisFacility::TIPCache::TNCCursor cursList
            (
                &CIDSock_ThisFacility::colNameCache
            );

            while (cursList.bIsValid())
            {
                TIPLookupItem& ipliCur = cursList.objWCur();
                if (ipliCur.m_enctEnd < enctNow)
                {
                    // This one is hosed, so remove it, don't bump cursor
                    CIDSock_ThisFacility::colNameCache.RemoveAt(cursList);
                }
                 else
                {
                    //
                    //  Not out of date, so see if it's our guy. If so,
                    //  remember it, but keep going to check them all.
                    //
                    if (ipliCur.m_strKey == CIDSock_ThisFacility::strKeyTemp)
                        pipliFind = &ipliCur;

                    ++cursList;
                }
            }
        }
         else
        {
            // Don't need to do a flush pass, so just do a fast hash lookup
            pipliFind = CIDSock_ThisFacility::colNameCache.pobjFindByKey
            (
                CIDSock_ThisFacility::strKeyTemp
            );

            //
            //  If we have it, but it's out of date, remove it. Re-null the
            //  pointer so we don't try to use it below.
            //
            if (pipliFind && (pipliFind->m_enctEnd < enctNow))
            {
                pipliFind = nullptr;
                CIDSock_ThisFacility::colNameCache.bRemoveKeyIfExists
                (
                    CIDSock_ThisFacility::strKeyTemp
                );
            }
        }

        //
        //  If we found one, we can give this guy back. We only cache them if
        //  they were non-numeric and required a lookup, so we can always
        //  just return that it was (would have been) a name lookup.
        //
        if (pipliFind)
        {
            kipaToFill = pipliFind->m_kipaCache;
            return tCIDSock::EAddrCvtRes::ViaLookup;
        }

        //
        //  We will have to actually do the work, which means we'll likely be
        //  adding to the cache.
        //
        //  Though it is HIGHLY unlikely, make sure the cache isn't still full.
        //  We did a flush pass above if the cache was full, but in theory we
        //  might not have removed anything. If that happens, then the likely
        //  reason is a change in the system clock which made our stamps wrong,
        //  so just flush the cache and start over.
        //
        if (CIDSock_ThisFacility::colNameCache.bIsFull(CIDSock_ThisFacility::c4MaxCacheItems))
            CIDSock_ThisFacility::colNameCache.RemoveAll();
    }

    // Let's do the actual lookup
    tCIDSock::EAddrCvtRes eRes = TKrnlIP::eIPAFromText
    (
        strAddr.pszBuffer(), kipaToFill, kCIDLib::True, eType
    );

    // If it didn't fail, then put it into the cache
    if (eRes != tCIDSock::EAddrCvtRes::Failed)
    {
        //
        //  It worked. If we did it via lookup, i.e. it wasn't already a
        //  numeric address, add it to the cache.
        //
        //  NOTE that we build the key using the type of address requested,
        //  not the type found, so that we can return whatever is found when
        //  there is a request for an unspecified type request, which is
        //  almost always what is done.
        //
        //  That does mean that, if we subsequently get a request for a specific
        //  type and it's really the same as an existing don't care, we will
        //  do a redundant lookup and have the same address in the cache under
        //  two different keys. But, that's not a problem really.
        //
        if (eRes == tCIDSock::EAddrCvtRes::ViaLookup)
        {
            TMtxLocker mtxlSync(CIDSock_ThisFacility::colNameCache.pmtxLock());
            MakeCacheKey(strAddr, eType, CIDSock_ThisFacility::strKeyTemp);

            tCIDLib::TBoolean bAdded;
            CIDSock_ThisFacility::colNameCache.objAddOrUpdate
            (
                TIPLookupItem(CIDSock_ThisFacility::strKeyTemp, kipaToFill)
                , bAdded
            );
        }
    }
    return eRes;
}


// Get the default local IP address
TIPAddress TFacCIDSock::ipaDefLocalAddr(const tCIDSock::EAddrTypes eType) const
{
    TKrnlIPAddr kipaRet;
    if (!TKrnlIP::bQueryDefLocalAddr(kipaRet, eType))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryDefLocalAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TIPAddress(kipaRet);
}


//
//  Same as above but do a end point in this case. This will make various
//  client's code more efficient.
//
TIPEndPoint
TFacCIDSock::ipepDefLocalAddr(  const   tCIDLib::TIPPortNum     ippnToUse
                                , const tCIDSock::EAddrTypes    eType) const
{
    TKrnlIPAddr kipaRet;
    if (!TKrnlIP::bQueryDefLocalAddr(kipaRet, eType))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryDefLocalAddr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return TIPEndPoint(kipaRet, ippnToUse);
}


//
//  Queries host adaptor information on the local named adaptor. Throws if
//  not found. There is another version above that will return false if not
//  found.
//
tCIDLib::TVoid
TFacCIDSock::QueryAdapterInfo(  const   TString&        strName
                                ,       THostAdapter&   hadpToFill) const
{
    tCIDLib::TBoolean        bFound;
    TKrnlIP::TAdaptorInfo kadpToFill;
    if (!TKrnlIP::bQueryAdaptorInfo(strName.pszBuffer(), kadpToFill, bFound)
    ||  !bFound)
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryAdapterByName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName
        );
    }
    hadpToFill.LoadFrom(kadpToFill);
}


//
//  Build an appropriate IP end point for the caller, for use by a listener
//  who wants to listen on the all addresses interface, taking into account
//  what IP protocols are installed on this machine.
//
tCIDLib::TVoid
TFacCIDSock::QueryDefAllListenEP(       TIPEndPoint&        ipepToFill
                                , const tCIDLib::TIPPortNum ippnListenOn)
{
    // Ask for the kernel level IP address for this
    TKrnlIPAddr kipaAll;
    TKrnlIP::QueryDefAllListen(kipaAll);

    // And now set up the end point with this info
    ipepToFill = TIPEndPoint(kipaAll, ippnListenOn);
}



//
//  Returns the supported TCP/IP version on the local machine, as a major
//  and minor version number. If the platfor doesn't support a minor number
//  just return zero.
//
tCIDLib::TVoid TFacCIDSock::QueryTCPVersion(tCIDLib::TCard4&    c4MajVersion
                                            , tCIDLib::TCard4&  c4MinVersion)
{
    c4MajVersion = m_c4MajVersion;
    c4MinVersion = m_c4MinVersion;
}


// Returns the local IP host name
const TString& TFacCIDSock::strIPHostName() const
{
    return m_strHostName;
}


// Looks up a host name by it's address
TString TFacCIDSock::strIPHostName(const TKrnlIPAddr& ipaToFind) const
{
    TKrnlString kstrName;
    if (!TKrnlIP::bQueryNameByAddr(ipaToFind, kstrName))
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_GetRemIPHostName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TIPAddress(ipaToFind)
        );
    }
    return TString(kstrName.pszValue());
}


// Translates a protocol enum into a a human readable form
TString TFacCIDSock::strXlatProto(const tCIDSock::ESockProtos eToXlat) const
{
    static const tCIDLib::TCh* apszMap[tCIDLib::c4EnumOrd(tCIDSock::ESockProtos::Count)] =
    {
        L"IP"
        , L"ICMP"
        , L"IGMP"
        , L"TCP"
        , L"PUP"
        , L"UDP"
        , L"IDP"
        , L"RawIP"
    };
    if (eToXlat >= tCIDSock::ESockProtos::Count)
        return TString(L"???");

    return TString(apszMap[tCIDLib::c4EnumOrd(eToXlat)]);
}

