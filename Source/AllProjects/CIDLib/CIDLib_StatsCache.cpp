//
// FILE NAME: CIDLib_StatsCache.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/07/2009
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
//  This file implements the statistics cache namespace and its small
//  associated classes.
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
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TStatsCacheItemInfo,TObject)



// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDLib_StatsCache
{
    // -----------------------------------------------------------------------
    //  We just use a simple array of pointers to cache items. It's of fixed
    //  size, large enough to more than handle the number of items we'll need.
    // -----------------------------------------------------------------------
    static constexpr tCIDLib::TCard4    c4CacheSz(2048);
    static tCIDLib::TCard4              c4CacheUsed(0);
    static TStatsCacheNode*             apscnCache[c4CacheSz];


    // -----------------------------------------------------------------------
    //  If we get a call that provides just the item, and it's not correct,
    //  there's not much we can do, but we bump this counter which can be
    //  read for sanity checking purposes later.
    // -----------------------------------------------------------------------
    static tCIDLib::TCard8              c8BadItemRefs;


    // -----------------------------------------------------------------------
    //  The persistent format version for our info class
    // -----------------------------------------------------------------------
    static constexpr tCIDLib::TCard2    c2FmtVersion = 1;


    // -----------------------------------------------------------------------
    //  A time stamp of the last time that the list was changed
    // -----------------------------------------------------------------------
    static tCIDLib::TEncodedTime        enctLastChange;

    // -----------------------------------------------------------------------
    //  A mutex for local locking. We use a kernel one because of the low level
    //  natur eof this stuff.
    // -----------------------------------------------------------------------
    static TKrnlMutex kmtxSync;
};



// ---------------------------------------------------------------------------
//   CLASS: TStatsCacheNode
//  PREFIX: scn
//
//  We need a cache item node class that's only visible internally.
// ---------------------------------------------------------------------------
class TStatsCacheNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStatsCacheNode() :

            m_c4KeyLen(0)
            , m_c8ChangeStamp(0)
            , m_c8CreateStamp(TTime::enctNow())
            , m_c8Value(0)
            , m_eType(tCIDLib::EStatItemTypes::Value)
            , m_pszKey(nullptr)
        {
        }

        TStatsCacheNode(const   tCIDLib::TCh*           pszKey
                        , const tCIDLib::EStatItemTypes eType
                        , const tCIDLib::TCard8         c8Value = 0) :

            m_c8CreateStamp(TTime::enctNow())
            , m_pszKey(nullptr)
        {
            Set(pszKey, eType, c8Value);
        }

        ~TStatsCacheNode()
        {
            delete [] m_pszKey;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsInThisScope
        (
            const   tCIDLib::TCh* const     pszScope
            , const tCIDLib::TCard4         c4ScopeLen
        )   const;

        tCIDLib::TBoolean bIsThisKey
        (
            const   tCIDLib::TCh* const     pszKey
            , const tCIDLib::TCard4         c4SrcLen
        )   const;

        tCIDLib::TBoolean bSetIfHigher
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TCard8 c8Decrement()
        {
            if (m_c8Value)
            {
                m_c8Value--;
                m_c8ChangeStamp = TTime::enctNow();
            }
            return m_c8Value;
        }

        tCIDLib::TCard8 c8Increment()
        {
            m_c8Value++;
            m_c8ChangeStamp = TTime::enctNow();
            return m_c8Value;
        }

        tCIDLib::TCard8 c8Stamp() const
        {
            return m_c8ChangeStamp;
        }

        tCIDLib::TCard8 c8Value() const
        {
            return m_c8Value;
        }

        tCIDLib::TCard8 c8Value(const tCIDLib::TCard8 c8ToSet)
        {
            m_c8Value = c8ToSet;
            m_c8ChangeStamp = TTime::enctNow();
            return m_c8Value;
        }

        tCIDLib::EStatItemTypes eType() const
        {
            return m_eType;
        }

        const tCIDLib::TCh* pszKey() const
        {
            return m_pszKey;
        }

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh*           pszKey
            , const tCIDLib::EStatItemTypes eType
            , const tCIDLib::TCard8         c8Value
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TStatsCacheNode(const TStatsCacheNode&);
        tCIDLib::TVoid operator=(const TStatsCacheNode&);


    private :
        // -------------------------------------------------------------------
        //  Private data values
        //
        //  m_c4KeyLen
        //      We pre-store the key length to help speed up searches.
        //
        //  m_c8ChangeStamp
        //      A time stamp that is bumped any time the value is set.
        //
        //  m_c8CreateStamp
        //      A time stamp that is set when the item is created, and which
        //      is used to allow for checking for new items having been added
        //      since a previous query.
        //
        //  m_c8Value
        //      The value of this node.
        //
        //  m_eType
        //      The type of this item, which really just means how the value
        //      should be interpreted.
        //
        //  m_pszKey
        //      The key for this node. It's the full path and allocated to
        //      hold the key, so we have to delete it when we destruct.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4KeyLen;
        tCIDLib::TCard8         m_c8ChangeStamp;
        tCIDLib::TCard8         m_c8CreateStamp;
        tCIDLib::TCard8         m_c8Value;
        tCIDLib::EStatItemTypes m_eType;
        tCIDLib::TCh*           m_pszKey;
};



//
//  Do an efficient check to see if this guy is in the passed scope. We do
//  a from the end comparison to quickly discard mismatches.
//
tCIDLib::TBoolean
TStatsCacheNode::bIsInThisScope(const   tCIDLib::TCh* const pszScope
                                , const tCIDLib::TCard4     c4ScopeLen) const
{
    // If the key length is not longer than the scope, can't match
    if (m_c4KeyLen <= c4ScopeLen)
        return kCIDLib::False;

    // We start at the same point in both strings, at the end of the scope
    const tCIDLib::TCh* psz1 = pszScope + c4ScopeLen;
    const tCIDLib::TCh* psz2 = m_pszKey + c4ScopeLen;
    while(psz1 != pszScope)
    {
        psz1--;
        psz2--;
        if (*psz1 != *psz2)
            break;
    }

    // If we made it all the way down, then they matched
    return (psz1 == pszScope);
}


//
//  Do an efficient check to see if this guy has the key passed. We do a
//  from the end comparison, to quickly discard mismatches.
//
tCIDLib::TBoolean
TStatsCacheNode::bIsThisKey(const   tCIDLib::TCh* const pszKey
                            , const tCIDLib::TCard4     c4SrcLen) const
{
    // We can do a quick check of the length first
    if (m_c4KeyLen != c4SrcLen)
        return kCIDLib::False;

    const tCIDLib::TCh* psz1 = pszKey + c4SrcLen;
    const tCIDLib::TCh* psz2 = m_pszKey + m_c4KeyLen;
    while(psz1 != pszKey)
    {
        psz1--;
        psz2--;
        if (*psz1 != *psz2)
            break;
    }

    // If we make it all the way down to the first character, they matched
    return (psz1 == pszKey);
}


// If the passed value is higher than the current value, it's the new value
tCIDLib::TBoolean
TStatsCacheNode::bSetIfHigher(const tCIDLib::TCard8 c8ToSet)
{
    if (c8ToSet > m_c8Value)
    {
        m_c8Value = c8ToSet;
        m_c8ChangeStamp = TTime::enctNow();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Set up this node after the fact
tCIDLib::TVoid
TStatsCacheNode::Set(const  tCIDLib::TCh*           pszKey
                    , const tCIDLib::EStatItemTypes eType
                    , const tCIDLib::TCard8         c8Value)
{
    m_c4KeyLen = TRawStr::c4StrLen(pszKey);
    m_c8ChangeStamp = TTime::enctNow();
    m_c8Value = c8Value;
    m_eType = eType;

    delete [] m_pszKey;
    m_pszKey = TRawStr::pszReplicate(pszKey);
}



// ---------------------------------------------------------------------------
//   CLASS: TSyncJanitor
//  PREFIX: jan
//
//  A simple local janitor to handle locking/unlocking our sync mutex
// ---------------------------------------------------------------------------
class TSyncJanitor
{
    public :
        TSyncJanitor()
        {
            CIDLib_StatsCache::kmtxSync.bLock();
        }

        TSyncJanitor(const TSyncJanitor&) = delete;
        TSyncJanitor(TSyncJanitor&&) = delete;

        ~TSyncJanitor()
        {
            CIDLib_StatsCache::kmtxSync.bUnlock();
        }

        tCIDLib::TVoid operator=(const TSyncJanitor&) = delete;
};



// ---------------------------------------------------------------------------
//  Local internal helper methods
// ---------------------------------------------------------------------------

//
//  A higher level helper to get the value when there's already an item known
//  to be set.
//
//  We assume the caller has locked.
//
static tCIDLib::TCard8 c8FindValue(const TStatsCacheItem& sciToUse)
{
    // If they passed us a bad one, return zero
    tCIDLib::TCard4 c4At;
    const TStatsCacheNode* pscnVal = nullptr;
    if (!sciToUse.bHasValidData(pscnVal, c4At))
    {
        CIDLib_StatsCache::c8BadItemRefs++;
        return 0;
    }

    // We got a good one, so return the value
    return pscnVal->c8Value();
}


//
//  Adds a new item to the cache and bumps the count of used cache items,
//  returning the index of the new item and a pointer to it.
//
//  It is assumed that the caller has locked.
//
static TStatsCacheNode* pscnAdd(const   tCIDLib::TCh* const     pszKey
                                , const tCIDLib::EStatItemTypes eType
                                ,       tCIDLib::TCard4&        c4At)
{
    //
    //  If, by some crazy chance, we happen to exceed the size of the
    //  cache, we have to panic.
    //
    if (CIDLib_StatsCache::c4CacheUsed >= CIDLib_StatsCache::c4CacheSz)
    {
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , L"CIDLib Stats Cache"
            , L""
            , 0
            , 0
            , 0
            , L"The stats cache size has been exceeded"
            , kCIDLib::pszEmptyZStr
        );
        TProcess::ExitProcess(tCIDLib::EExitCodes::RuntimeError);
    }

    TStatsCacheNode* pscnRet = new TStatsCacheNode(pszKey, eType);
    c4At = CIDLib_StatsCache::c4CacheUsed++;

    CIDLib_Suppress(6386);  // We range checked above already
    CIDLib_StatsCache::apscnCache[c4At] = pscnRet;

    // Update the last change time stamp
    CIDLib_StatsCache::enctLastChange = TTime::enctNow();
    return pscnRet;
}


//
//  Try to find the indicated key in our cache. If found, return it and
//  its index within the cache.
//
//  It is assumed taht the caller has locked.
//
static TStatsCacheNode* pscnFind(const  tCIDLib::TCh* const pszKey
                                ,       tCIDLib::TCard4&    c4At)
{
    c4At = 0;
    const tCIDLib::TCard4 c4KeyLen = TRawStr::c4StrLen(pszKey);
    while (c4At < CIDLib_StatsCache::c4CacheUsed)
    {
        // Check this one and if it matches, break out
        if (CIDLib_StatsCache::apscnCache[c4At]->bIsThisKey(pszKey, c4KeyLen))
            break;

        // Not there yet, so move up and try again
        c4At++;
    }

    //
    //  If we went to the end of the used cache items, then we did
    //  not find it.
    //
    if (c4At >= CIDLib_StatsCache::c4CacheUsed)
        return 0;
    return CIDLib_StatsCache::apscnCache[c4At];
}


//
//  Try to find the the indicated key. We first check the passed cache
//  item. If it's set, we return the item it refers to. If not, we try
//  to find it by the key.
//
//  It is assumed that the caller has locked.
//
static TStatsCacheNode*
pscnFind(const  tCIDLib::TCh* const pszKey
        ,       TStatsCacheItem&    sciToUse
        ,       tCIDLib::TCard4&    c4At)
{
    c4At = 0;

    // First check the item. If set up, then we are done
    TStatsCacheNode* pscnRet = 0;
    if (sciToUse.bHasValidData(pscnRet, c4At))
        return pscnRet;

    // Not set up, so call the other version for a key based lookup
    return pscnFind(pszKey, c4At);
}




// ---------------------------------------------------------------------------
//   CLASS: TStatsCacheItem
//  PREFIX: sci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStatsCacheItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TStatsCacheItem::TStatsCacheItem() :

    m_c4Id(kCIDLib::c4MaxCard)
    , m_pscnData(nullptr)
{
}

TStatsCacheItem::TStatsCacheItem(const  tCIDLib::TCard4     c4Id
                                ,       TStatsCacheNode*    pscnData) :

    m_c4Id(c4Id)
    , m_pscnData(pscnData)
{
}

TStatsCacheItem::~TStatsCacheItem()
{
    // Nothing to do, we own nothing
}


// ---------------------------------------------------------------------------
//  TStatsCacheItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Indicate if we have good data and if so return a pointer to it
tCIDLib::TBoolean
TStatsCacheItem::bHasValidData( TStatsCacheNode*&   pscnToFill
                                , tCIDLib::TCard4&  c4Id)
{
    c4Id = m_c4Id;
    pscnToFill = m_pscnData;
    return (pscnToFill != nullptr);
}

tCIDLib::TBoolean
TStatsCacheItem::bHasValidData( const   TStatsCacheNode*&   pscnToFill
                                ,       tCIDLib::TCard4&    c4Id) const
{
    c4Id = m_c4Id;
    pscnToFill = m_pscnData;
    return (pscnToFill != nullptr);
}


// Return the id of this item. It'll be c4MaxCard if we've not been setup
tCIDLib::TCard4 TStatsCacheItem::c4Id() const
{
    return m_c4Id;
}


// Return the stamp of this item
tCIDLib::TCard8 TStatsCacheItem::c8Stamp() const
{
    if (m_pscnData)
        return m_pscnData->c8Stamp();

    return 0;
}


// Return the type of this item
tCIDLib::EStatItemTypes TStatsCacheItem::eType() const
{
    if (m_pscnData)
        return m_pscnData->eType();

    // Not much we can do, just return value type
    return tCIDLib::EStatItemTypes::Value;
}


// Return the key of the item we are pointing to
const tCIDLib::TCh* TStatsCacheItem::pszKey() const
{
    if (m_pscnData)
        return m_pscnData->pszKey();
    return nullptr;
}


tCIDLib::TVoid
TStatsCacheItem::Set(const tCIDLib::TCard4 c4Id, TStatsCacheNode* const pscnData)
{
    m_c4Id = c4Id;
    m_pscnData = pscnData;
}



// ---------------------------------------------------------------------------
//   CLASS: TStatsCacheItemInfo
//  PREFIX: scii
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStatsCacheItemInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TStatsCacheItemInfo::eCompNames(const   TStatsCacheItemInfo&    scii1
                                , const TStatsCacheItemInfo&    scii2)
{
    return scii1.m_strName.eCompare(scii2.m_strName);
}


// ---------------------------------------------------------------------------
//  TStatsCacheItemInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TStatsCacheItemInfo::TStatsCacheItemInfo() :

    m_c4Id(kCIDLib::c4MaxCard)
    , m_c8Stamp(0)
    , m_c8Value(0)
    , m_eType(tCIDLib::EStatItemTypes::Value)
    , m_strName()
{
}

TStatsCacheItemInfo::
TStatsCacheItemInfo(const   tCIDLib::TCh* const pszName
                    , const tCIDLib::TCard4     c4Id
                    , const TStatsCacheNode&    scnSrc) :

    m_c4Id(c4Id)
    , m_c8Stamp(scnSrc.c8Stamp())
    , m_c8Value(scnSrc.c8Value())
    , m_eType(scnSrc.eType())
    , m_strName(pszName)
{
}

TStatsCacheItemInfo::~TStatsCacheItemInfo()
{
}


// ---------------------------------------------------------------------------
//  TStatsCacheItemInfo: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TStatsCacheItemInfo::operator==(const TStatsCacheItemInfo& sciiSrc) const
{
    if (&sciiSrc == this)
        return kCIDLib::True;

    return
    (
        (m_c4Id         == sciiSrc.m_c4Id)
        && (m_c8Stamp   == sciiSrc.m_c8Stamp)
        && (m_c8Value   == sciiSrc.m_c8Value)
        && (m_eType     == sciiSrc.m_eType)
        && (m_strName   == sciiSrc.m_strName)
    );
}


// ---------------------------------------------------------------------------
//  TStatsCacheItemInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TStatsCacheItemInfo::c4Id() const
{
    return m_c4Id;
}


tCIDLib::TCard8 TStatsCacheItemInfo::c8Stamp() const
{
    return m_c8Stamp;
}

tCIDLib::TCard8 TStatsCacheItemInfo::c8Stamp(const tCIDLib::TCard8 c8ToSet)
{
    m_c8Stamp = c8ToSet;
    return m_c8Stamp;
}


tCIDLib::TCard8 TStatsCacheItemInfo::c8Value() const
{
    return m_c8Value;
}

tCIDLib::EStatItemTypes TStatsCacheItemInfo::eType() const
{
    return m_eType;
}

const TString& TStatsCacheItemInfo::strName() const
{
    return m_strName;
}


// -------------------------------------------------------------------
//  TStatsCacheItemInfo: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TVoid TStatsCacheItemInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // We should get a start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDLib_StatsCache::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Stream in our members
    strmToReadFrom  >> m_strName
                    >> m_eType
                    >> m_c4Id
                    >> m_c8Stamp
                    >> m_c8Value;

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

}

tCIDLib::TVoid TStatsCacheItemInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_StatsCache::c2FmtVersion
                    << m_strName
                    << m_eType
                    << m_c4Id
                    << m_c8Stamp
                    << m_c8Value
                    << tCIDLib::EStreamMarkers::EndObject;
}





// ---------------------------------------------------------------------------
//  NAMESPACE: TStatsCache
// ---------------------------------------------------------------------------

//
//  Efficiently check the state of a bit flag type. These are used for control flags,
//  so they always want to be gotten efficiently.
//
tCIDLib::TBoolean
TStatsCache::bCheckBitFlag( const   TStatsCacheItem&    sciToUse
                            , const tCIDLib::TCard4     c4BitNum)
{
    TSyncJanitor janLock;
    const tCIDLib::TCard8 c8Val = c8FindValue(sciToUse);
    return (c8Val & (tCIDLib::TCard8(1) << c4BitNum)) != 0;
}


//
//  Get the value of flag items in a few different ways. These are boolean values
//  stored as 0 or 1 in the card 8 value member.
//
tCIDLib::TBoolean
TStatsCache::bCheckFlag(const   tCIDLib::TCh* const pszKey
                        ,       TStatsCacheItem&    sciToUse)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnFlag = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a flag type
    if (!pscnFlag)
    {
        pscnFlag = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Flag, c4At);
        sciToUse.Set(c4At, pscnFlag);
    }

    // Else let's return the actual value
    return (pscnFlag->c8Value() != 0);
}

tCIDLib::TBoolean TStatsCache::bCheckFlag(const tCIDLib::TCh* const pszKey)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnFlag = pscnFind(pszKey, c4At);

    // If not found, just return false now, else return the actual value
    if (!pscnFlag)
        return kCIDLib::False;
    return (pscnFlag->c8Value() != 0);
}

tCIDLib::TBoolean
TStatsCache::bCheckFlag(const TStatsCacheItem& sciToUse)
{
    TSyncJanitor janLock;
    return (c8FindValue(sciToUse) != 0);
}


//
//  Handles polling of values efficiently. See the header comments.
//
tCIDLib::TBoolean
TStatsCache::bPollValuesInScope(const   TString&                strScope
                                , const tCIDLib::TBoolean       bDirectOnly
                                , const tCIDLib::TCard8         c8QueryStamp
                                ,       tCIDLib::TCard8&        c8PollStamp
                                ,       TIDList&                fcolIdList
                                ,       TValueList&             fcolValueList)
{
    //
    //  Flush both lists first. This is called from an ORB API so it's
    //  important that, should we return without filling in the lists that
    //  they be empty, so that it doesn't stream back useless info.
    //
    fcolIdList.RemoveAll();
    fcolValueList.RemoveAll();

    // And how lock while we do this
    TSyncJanitor janLock;

    // If the query stamp is out of date, then return false now
    if (c8QueryStamp != CIDLib_StatsCache::enctLastChange)
        return kCIDLib::False;

    //
    //  We are good so let's find anything in the indicted scope and changed
    //  since the provided stamp.
    //
    const tCIDLib::TCh* pszScope = strScope.pszBuffer();
    const tCIDLib::TCard4 c4ScopeLen = strScope.c4Length();

    for (tCIDLib::TCard4 c4Index = 0;
                    c4Index < CIDLib_StatsCache::c4CacheUsed; c4Index++)
    {
        TStatsCacheNode* pscnCur = CIDLib_StatsCache::apscnCache[c4Index];

        //
        //  If it's not changed since the passed poll stamp. then we don't
        //  even have to do the work to see if it's in scope.
        //
        if (pscnCur->c8Stamp() <= c8PollStamp)
            continue;

        // It's potentially a keeper, so see if it's in the requested scope
        if (pscnCur->bIsInThisScope(pszScope, c4ScopeLen))
        {
            // Get a pointer to the relative part of the item key
            const tCIDLib::TCh* pszName = pscnCur->pszKey() + c4ScopeLen;

            //
            //  If they only want direct children, then the subsequent
            //  name part cannot contain any more slashes.
            //
            if (!bDirectOnly
            ||  !TRawStr::pszFindChar(pszName, kCIDLib::chForwardSlash))
            {
                fcolIdList.c4AddElement(c4Index);
                fcolValueList.c4AddElement(pscnCur->c8Value());
            }
        }
    }
    return kCIDLib::True;
}


//
//  Set flag items in a few different ways
//
tCIDLib::TBoolean
TStatsCache::bSetFlag(  const   tCIDLib::TCh* const pszKey
                        ,       TStatsCacheItem&    sciToUse
                       , const  tCIDLib::TBoolean   bNewState)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnFlag = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a flag type
    if (!pscnFlag)
    {
        pscnFlag = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Flag, c4At);
        sciToUse.Set(c4At, pscnFlag);
    }

    pscnFlag->c8Value(bNewState ? 1 : 0);
    return bNewState;
}

tCIDLib::TBoolean
TStatsCache::bSetFlag(  const   tCIDLib::TCh* const pszKey
                        , const tCIDLib::TBoolean   bNewState)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnFlag = pscnFind(pszKey, c4At);

    // If not found, we have to fault it in assuming a flag type
    if (!pscnFlag)
        pscnFlag = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Flag, c4At);

    pscnFlag->c8Value(bNewState ? 1 : 0);
    return bNewState;
}

tCIDLib::TBoolean
TStatsCache::bSetFlag(TStatsCacheItem& sciToUse, const tCIDLib::TBoolean bNewState)
{
    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = nullptr;
    if (sciToUse.bHasValidData(pscnVal, c4At))
        pscnVal->c8Value(bNewState ? 1 : 0);
    else
        CIDLib_StatsCache::c8BadItemRefs++;
    return bNewState;
}


//
//  Set the of an item if the new value is higher than the current. The
//  return indicates if it was higher. The assumption here is that this is
//  a 'value' type.
//
tCIDLib::TBoolean
TStatsCache::bSetIfHigher(  const   tCIDLib::TCh* const pszKey
                            ,       TStatsCacheItem&    sciToUse
                            , const tCIDLib::TCard8     c8ToSet)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a value type
    if (!pscnVal)
    {
        pscnVal = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Value, c4At);
        sciToUse.Set(c4At, pscnVal);
    }

    // If the new value is higher, take it
    return pscnVal->bSetIfHigher(c8ToSet);
}

tCIDLib::TBoolean
TStatsCache::bSetIfHigher(          TStatsCacheItem&    sciToUse
                            , const tCIDLib::TCard8     c8ToSet)
{
    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = nullptr;
    if (sciToUse.bHasValidData(pscnVal, c4At))
    {
        if (pscnVal->bSetIfHigher(c8ToSet))
            return kCIDLib::True;
    }
     else
    {
        CIDLib_StatsCache::c8BadItemRefs++;
    }
    return kCIDLib::False;
}


//
//  For outside code to set a value. No faulting is done. Just a return
//  status that indicates if it could be set or not.
//
tCIDLib::TBoolean
TStatsCache::bSetValue( const   tCIDLib::TCh* const pszPath
                        , const tCIDLib::TCard8     c8ToSet)
{
    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = pscnFind(pszPath, c4At);

    // If not found, return false now
    if (!pscnVal)
        return kCIDLib::False;

    // We found it. Massage the value passed if needed
    tCIDLib::TCard8 c8RealVal = c8ToSet;
    switch (pscnVal->eType())
    {
        case tCIDLib::EStatItemTypes::Flag :
            // Make it a one or zero
            c8RealVal = c8ToSet ? 1 : 0;
            break;

        case tCIDLib::EStatItemTypes::Percent :
            // If not a valid percent, return false
            if (c8ToSet > 100)
                return kCIDLib::False;
            break;

        default :
            break;
    };

    // Store the massaged value and return success
    pscnVal->c8Value(c8RealVal);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TStatsCache::bSetFlagBit(const  tCIDLib::TCh* const pszPath
                        , const tCIDLib::TCard4     c4BitNum
                        , const tCIDLib::TBoolean   bState)
{
    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = pscnFind(pszPath, c4At);

    // If not found, or the wrong type, return false now
    if (!pscnVal || (pscnVal->eType() != tCIDLib::EStatItemTypes::BitFlags))
        return kCIDLib::False;

    // Get the current value and update the appropriate bit
    tCIDLib::TCard8 c8Val = pscnVal->c8Value();
    if (bState)
        c8Val |= (tCIDLib::TCard8(1) << c4BitNum);
    else
        c8Val &= ~(tCIDLib::TCard8(1) << c4BitNum);

    // Store the updated value and return success
    pscnVal->c8Value(c8Val);
    return kCIDLib::True;
}



//
//  Query a list of items that meet some criteria.
//
tCIDLib::TCard4
TStatsCache::c4QueryItemsInScope(const  TString&                strScope
                                ,       TStatsCache::TItemList& colToFill)
{
    colToFill.RemoveAll();

    const tCIDLib::TCard4   c4ScopeLen = strScope.c4Length();
    const tCIDLib::TCh*     pszScope = strScope.pszBuffer();

    CIDAssert
    (
        (c4ScopeLen != 0)
          && (strScope.chFirst() == kCIDLib::chForwardSlash)
          && (strScope.chLast() == kCIDLib::chForwardSlash)
        , L"Invalid stats cache scope was provided"
    );

    // Lock while we do this
    {
        TSyncJanitor janLock;

        //
        //  Run through the whole list and find anything that includes the
        //  passed scope as the first part of the key.
        //
        //  We compare backwards to find mismatches as quickly as possible,
        //  starting at the end of the scope.
        //
        for (tCIDLib::TCard4 c4Index = 0;
                        c4Index < CIDLib_StatsCache::c4CacheUsed; c4Index++)
        {
            TStatsCacheNode* pscnCur = CIDLib_StatsCache::apscnCache[c4Index];
            if (pscnCur->bIsInThisScope(pszScope, c4ScopeLen))
                colToFill.objAdd(TStatsCacheItem(c4Index, pscnCur));
        }
    }
    return colToFill.c4ElemCount();
}


//
//  Query a list of the relative names and the values of all of the items
//  under the indicated scope. They can ask that we don't clear the list
//  first, so that they can get multiple scopes into the list and then
//  sort them and display them.
//
tCIDLib::TCard4
TStatsCache::c4QueryValuesInScope(  const   TString&            strScope
                                    ,       TInfoList&          colToFill
                                    , const tCIDLib::TBoolean   bDirectOnly
                                    ,       tCIDLib::TCard8&    c8Stamp
                                    , const tCIDLib::TBoolean   bClearFirst)
{
    // Only clear the list first if we were asked to
    if (bClearFirst)
        colToFill.RemoveAll();

    const tCIDLib::TCard4   c4ScopeLen = strScope.c4Length();
    const tCIDLib::TCh*     pszScope = strScope.pszBuffer();

    //
    //  As scope has to start and end with a forward slash and have at least
    //  one character in between.
    //
    CIDAssert
    (
        (c4ScopeLen > 2)
          && (strScope.chFirst() == kCIDLib::chForwardSlash)
          && (strScope.chLast() == kCIDLib::chForwardSlash)
        , L"Invalid stats cache scope was provided"
    );

    // Lock while we do this
    tCIDLib::TCard4 c4Count = 0;
    {
        TSyncJanitor janLock;

        //
        //  Run through the whole list and find anything that includes the
        //  passed scope as the first part of the key.
        //
        //  We compare backwards to find mismatches as quickly as possible,
        //  starting at the end of the scope.
        //
        for (tCIDLib::TCard4 c4Index = 0;
                        c4Index < CIDLib_StatsCache::c4CacheUsed; c4Index++)
        {
            TStatsCacheNode* pscnCur = CIDLib_StatsCache::apscnCache[c4Index];
            if (pscnCur->bIsInThisScope(pszScope, c4ScopeLen))
            {
                // Get a pointer to the relative part of the item key
                const tCIDLib::TCh* pszName = pscnCur->pszKey() + c4ScopeLen;

                //
                //  If they only want direct children, then the subsequent
                //  name part cannot contain any more slashes.
                //
                if (!bDirectOnly
                ||  !TRawStr::pszFindChar(pszName, kCIDLib::chForwardSlash))
                {
                    // Looks ok, so take it
                    colToFill.objAdd
                    (
                        TStatsCacheItemInfo(pszName, c4Index, *pscnCur)
                    );
                    c4Count++;
                }
            }
        }

        // Give him the current stamp
        c8Stamp = CIDLib_StatsCache::enctLastChange;
    }
    return c4Count;
}


// Return our bad item ref counter
tCIDLib::TCard8 TStatsCache::c8BadItemRefs()
{
    tCIDLib::TCard8 c8Ret = 0;
    {
        TSyncJanitor janLock;
        c8Ret = CIDLib_StatsCache::c8BadItemRefs;
    }
    return c8Ret;
}


// Get the value of items
tCIDLib::TCard8
TStatsCache::c8CheckValue(  const   tCIDLib::TCh* const pszKey
                            ,       TStatsCacheItem&    sciToUse)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnFlag = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a value type
    if (!pscnFlag)
    {
        pscnFlag = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Value, c4At);
        sciToUse.Set(c4At, pscnFlag);
    }

    // Else let's return the actual value
    return pscnFlag->c8Value();
}

tCIDLib::TCard8 TStatsCache::c8CheckValue(const tCIDLib::TCh* const pszKey)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = pscnFind(pszKey, c4At);

    // If not found, just return zero, else return the actual value
    if (!pscnVal)
        return 0;
    return pscnVal->c8Value();
}

tCIDLib::TCard8 TStatsCache::c8CheckValue(const TStatsCacheItem& sciToUse)
{
    TSyncJanitor janLock;
    return c8FindValue(sciToUse);
}


// Decrement counter type items
tCIDLib::TCard8
TStatsCache::c8DecCounter(  const   tCIDLib::TCh* const pszKey
                            ,       TStatsCacheItem&    sciToUse)
{
    CIDAssert((pszKey != 0) && (*pszKey != 0), L"Empty/null stats cache key");

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnCnt = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a value type
    if (!pscnCnt)
    {
        pscnCnt = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Counter, c4At);
        sciToUse.Set(c4At, pscnCnt);
    }
    return pscnCnt->c8Decrement();
}

tCIDLib::TCard8 TStatsCache::c8DecCounter(TStatsCacheItem& sciToUse)
{
    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnCnt = nullptr;
    if (sciToUse.bHasValidData(pscnCnt, c4At))
        return pscnCnt->c8Decrement();

    // Not much we can do but return zero
    CIDLib_StatsCache::c8BadItemRefs++;
    return 0;
}


// Increment counter type items
tCIDLib::TCard8
TStatsCache::c8IncCounter(  const   tCIDLib::TCh* const pszKey
                            ,       TStatsCacheItem&    sciToUse)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    // Try to find it
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnCnt = pscnFind(pszKey, sciToUse, c4At);

    // If not found, we have to fault it in assuming a value type
    if (!pscnCnt)
    {
        pscnCnt = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Counter, c4At);
        sciToUse.Set(c4At, pscnCnt);
    }
    return pscnCnt->c8Increment();
}

tCIDLib::TCard8 TStatsCache::c8IncCounter(TStatsCacheItem& sciToUse)
{
    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnCnt = nullptr;
    if (sciToUse.bHasValidData(pscnCnt, c4At))
        return pscnCnt->c8Increment();

    // Not much we can do but return zero
    CIDLib_StatsCache::c8BadItemRefs++;
    return 0;
}


// Registers an item explicitly
tCIDLib::TVoid
TStatsCache::RegisterItem(  const   tCIDLib::TCh* const     pszKey
                            , const tCIDLib::EStatItemTypes eType
                            ,       TStatsCacheItem&        sciToFill)
{
    TSyncJanitor janLock;

    //
    //  First see if it's already present. If already present, we'll just
    //  update the type. Else we add it.
    //
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnNew = pscnFind(pszKey, c4At);
    if (pscnNew)
        pscnNew->Set(pszKey, eType, 0);
    else
        pscnNew = pscnAdd(pszKey, eType, c4At);

    // And either way set up the item
    sciToFill.Set(c4At, pscnNew);
}


// Set the value of items directly, used for value type items
tCIDLib::TVoid
TStatsCache::SetValue(  const   tCIDLib::TCh* const pszKey
                        ,       TStatsCacheItem&    sciToUse
                        , const tCIDLib::TCard8     c8ToSet)
{
    CIDAssert
    (
        (pszKey != 0) && (*pszKey == kCIDLib::chForwardSlash)
        , L"Empty/null stats cache key"
    );

    TSyncJanitor janLock;

    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = nullptr;
    if (!sciToUse.bHasValidData(pscnVal, c4At))
    {
        pscnVal = pscnAdd(pszKey, tCIDLib::EStatItemTypes::Value, c4At);
        sciToUse.Set(c4At, pscnVal);
    }
    pscnVal->c8Value(c8ToSet);
}

tCIDLib::TVoid
TStatsCache::SetValue(TStatsCacheItem& sciToUse, const tCIDLib::TCard8 c8ToSet)
{
    TSyncJanitor janLock;

    //
    //  We can't really do anything if they didn't provide a good item. But,
    //  if they do, set it.
    //
    tCIDLib::TCard4 c4At;
    TStatsCacheNode* pscnVal = nullptr;
    if (sciToUse.bHasValidData(pscnVal, c4At))
        pscnVal->c8Value(c8ToSet);
    else
        CIDLib_StatsCache::c8BadItemRefs++;
}


