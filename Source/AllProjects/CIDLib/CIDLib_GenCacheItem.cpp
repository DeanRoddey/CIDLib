//
// FILE NAME: CIDLib_GenCacheItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2014
//
// COPYRIGHT: 2014..2014, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This file implements the generic cache item base class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  Do magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDGenCacheItem, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCIDGenCacheItem
//  PREFIX: ci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDGenCacheItem: Public, static methods
// ---------------------------------------------------------------------------

//
//  Do a least recently used pruning of a standard cache of items, removing up to
//  the indicated number of items.
//
tCIDLib::TCard4
TCIDGenCacheItem::c4LRUDiscard(         tCIDLib::TGenCache& colTar
                                , const tCIDLib::TCard4     c4UpTo)
{
    //
    //  We create a non-adopting vector of the items which we can sort by the
    //  time stamp.
    //
    TRefVector<const TCIDGenCacheItem> colSort
    (
        tCIDLib::EAdoptOpts::NoAdopt, colTar.c4ElemCount()
    );

    // Copy over the pointers and sort, asscending so the oldest ones are first
    {
        tCIDLib::TGenCache::TCursor cursTar(&colTar);
        for (; cursTar; ++cursTar)
            colSort.Add(&cursTar.objRCur());

        colSort.Sort
        (
            [](const TCIDGenCacheItem& cci1, const TCIDGenCacheItem& cci2)
            {
                if (cci1.m_enctStamp < cci2.m_enctStamp)
                    return tCIDLib::ESortComps::FirstLess;
                else if (cci1.m_enctStamp > cci2.m_enctStamp)
                    return tCIDLib::ESortComps::FirstGreater;
                return tCIDLib::ESortComps::Equal;
            }
        );
    }

    // Now go through up to the indicated number and remove them from the cache
    tCIDLib::TCard4 c4Removed = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4UpTo; c4Index++)
    {
        // Break out if we've emptied the cache
        if (colTar.bIsEmpty())
            break;

        // Should always be present, but don't tempt fate
        if (colTar.bRemoveKeyIfExists(colSort[c4Index]->strName()))
            c4Removed++;
    }
    return c4Removed;
}



// ---------------------------------------------------------------------------
//  TCIDGenCacheItem: Constructors and destructor
// ---------------------------------------------------------------------------
TCIDGenCacheItem::TCIDGenCacheItem() :

    m_c4Bytes(0)
    , m_c4SerialNum(1)
    , m_enctStamp(0)
{
    m_c4SerialNum = c4GenInitSerialNum();
}

TCIDGenCacheItem::
TCIDGenCacheItem(const  TString&        strName
                , const tCIDLib::TCard4 c4SerialNum) :

    m_c4Bytes(0)
    , m_c4SerialNum(c4SerialNum)
    , m_enctStamp(0)
    , m_strName(strName)
{
}

TCIDGenCacheItem::TCIDGenCacheItem(const TCIDGenCacheItem& cciSrc) :

    m_c4Bytes(cciSrc.m_c4SerialNum)
    , m_c4SerialNum(cciSrc.m_c4SerialNum)
    , m_enctStamp(cciSrc.m_enctStamp)
    , m_strName(cciSrc.m_strName)
{
    // Copy over the useful bytes
    m_mbufData.CopyIn(cciSrc.m_mbufData, m_c4Bytes);
}

TCIDGenCacheItem::~TCIDGenCacheItem()
{
}


// ---------------------------------------------------------------------------
//  TCIDGenCacheItem: Public operators
// ---------------------------------------------------------------------------
TCIDGenCacheItem& TCIDGenCacheItem::operator=(const TCIDGenCacheItem& cciSrc)
{
    if (this != &cciSrc)
    {
        m_c4Bytes = cciSrc.m_c4Bytes;
        m_c4SerialNum = cciSrc.m_c4SerialNum;
        m_strName = cciSrc.m_strName;

        // Copy over the useful bytes
        m_mbufData.CopyIn(cciSrc.m_mbufData, m_c4Bytes);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCIDGenCacheItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If the caller has the current serial number, then assume he has the current
//  data and return zero bytes and false. Else copy back over the data and return
//  true.
//
tCIDLib::TBoolean
TCIDGenCacheItem::bQueryData(tCIDLib::TCard4&   c4SerialNum
                            , tCIDLib::TCard4&  c4Bytes
                            , TMemBuf&          mbufToFill) const
{
    // If the serial number hasn't changed, return nothing
    if (m_c4SerialNum == c4SerialNum)
    {
       c4Bytes = 0;
       return kCIDLib::False;
    }

    if (mbufToFill.c4Size() < m_c4Bytes)
        mbufToFill.Reallocate(m_c4Bytes, kCIDLib::False);
    mbufToFill.CopyIn(m_mbufData, m_c4Bytes);

    c4Bytes = m_c4Bytes;
    c4SerialNum = m_c4SerialNum;
    return kCIDLib::True;
}


// Return the current byte count
tCIDLib::TCard4 TCIDGenCacheItem::c4Bytes() const
{
    return m_c4Bytes;
}


// Bump the serial number and return the new value
tCIDLib::TCard4 TCIDGenCacheItem::c4IncSerialNum()
{
    m_c4SerialNum++;

    // Don't ever let it be zero
    if (!m_c4SerialNum)
        m_c4SerialNum++;

    return m_c4SerialNum;
}



// Get or set the serial number
tCIDLib::TCard4 TCIDGenCacheItem::c4SerialNum() const
{
    return m_c4SerialNum;
}

tCIDLib::TCard4 TCIDGenCacheItem::c4SerialNum(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SerialNum = c4ToSet;
    return m_c4SerialNum;
}


// Return the time stamp
tCIDLib::TEncodedTime TCIDGenCacheItem::enctTimeStamp() const
{
    return m_enctStamp;
}


// Provide const access to the raw data
const TMemBuf& TCIDGenCacheItem::mbufData() const
{
    return m_mbufData;
}


// Provide access to the name (key)
const TString& TCIDGenCacheItem::strName() const
{
    return m_strName;
}


// Set new data on the cache item
tCIDLib::TVoid
TCIDGenCacheItem::SetData(const tCIDLib::TCard4 c4Bytes, const TMemBuf& mbufSrc)
{
    if (m_mbufData.c4Size() < c4Bytes)
        m_mbufData.Reallocate(c4Bytes, kCIDLib::False);
    m_mbufData.CopyIn(mbufSrc, c4Bytes);

    m_c4Bytes = c4Bytes;
}

tCIDLib::TVoid
TCIDGenCacheItem::SetData(const tCIDLib::TCard4 c4Bytes, THeapBuf&& mbufSrc)
{
    m_mbufData = tCIDLib::ForceMove(mbufSrc);
    m_c4Bytes = c4Bytes;
}


//
//  Set the time stamp to now plus the indicated number of seconds. Defaults to
//  zero seconds which will just set it to now.
//
tCIDLib::TVoid TCIDGenCacheItem::UpdateTimeStamp(const tCIDLib::TCard4 c4SecsFromNow)
{
    m_enctStamp = TTime::enctNowPlusSecs(c4SecsFromNow);
}


// ---------------------------------------------------------------------------
//  TCIDGenCacheItem: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Generate a semi-random starting serial number
tCIDLib::TCard4 TCIDGenCacheItem::c4GenInitSerialNum()
{
    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(TTime::enctNow() & 0xFFFFFFFF);
    if (!c4Ret)
        c4Ret++;

    return c4Ret;
}

