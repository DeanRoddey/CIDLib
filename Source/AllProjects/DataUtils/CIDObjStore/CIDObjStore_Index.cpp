//
// FILE NAME: CIDObjStore_Index.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/01/2003
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
//  This file implements the object store implementation. There are actually
//  a few files, since this is a complex class. This particular file provides
//  the public methods that the public engine (wrapper around this impl)
//  class calls, and any general private methods that aren't related to
//  manipulating either the store files or the free list/index data.
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
#include    "CIDObjStore_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOSFreeListItem,TObject)
RTTIDecls(TOSStoreItem,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TOSFreeListItem
//  PREFIX: fli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOSFreeListItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TOSFreeListItem::TOSFreeListItem() :

    m_c4Offset(0)
    , m_c4Size(0)
{
}

TOSFreeListItem::TOSFreeListItem(const  tCIDLib::TCard4 c4Size
                                , const tCIDLib::TCard4 c4Offset) :
    m_c4Offset(c4Offset)
    , m_c4Size(c4Size)
{
}

TOSFreeListItem::TOSFreeListItem(const TOSFreeListItem& fliToCopy) :

    m_c4Offset(fliToCopy.m_c4Offset)
    , m_c4Size(fliToCopy.m_c4Size)
{
}

TOSFreeListItem::~TOSFreeListItem()
{
}


// ---------------------------------------------------------------------------
//  TOSFreeListItem: Public operators
// ---------------------------------------------------------------------------
TOSFreeListItem& TOSFreeListItem::operator=(const TOSFreeListItem& fliToAssign)
{
    if (this != &fliToAssign)
    {
        m_c4Offset  = fliToAssign.m_c4Offset;
        m_c4Size    = fliToAssign.m_c4Size;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TOSFreeListItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOSFreeListItem::AddToSize(const TOSFreeListItem& fliToTake)
{
    m_c4Size += fliToTake.m_c4Size;
}

tCIDLib::TVoid TOSFreeListItem::AddToSize(const tCIDLib::TCard4 c4ToAdd)
{
    m_c4Size += c4ToAdd;
}


tCIDLib::TCard4 TOSFreeListItem::c4AfterIt() const
{
    return m_c4Offset + m_c4Size;
}


tCIDLib::TCard4 TOSFreeListItem::c4Offset() const
{
    return m_c4Offset;
}

tCIDLib::TCard4 TOSFreeListItem::c4Offset(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Offset = c4ToSet;
    return m_c4Offset;
}


tCIDLib::TCard4 TOSFreeListItem::c4Size() const
{
    return m_c4Size;
}

tCIDLib::TCard4 TOSFreeListItem::c4Size(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Size = c4ToSet;
    return m_c4Size;
}


tCIDLib::TVoid TOSFreeListItem::MarkUnused()
{
    m_c4Offset  = 0;
    m_c4Size    = 0;
}


tCIDLib::TVoid TOSFreeListItem::ReduceSize(const tCIDLib::TCard4 c4ToSub)
{
    if (c4ToSub >= m_c4Size)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_TookTooMuch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4ToSub)
            , TCardinal(m_c4Size)
        );
    }
    m_c4Size -= c4ToSub;
}


tCIDLib::TVoid TOSFreeListItem::Set(const   tCIDLib::TCard4 c4Size
                                    , const tCIDLib::TCard4 c4Offset)
{
    m_c4Offset  = c4Offset;
    m_c4Size    = c4Size;
}


tCIDLib::TVoid TOSFreeListItem::TakeFromBottom(const tCIDLib::TCard4 c4ToTake)
{
    if (c4ToTake >= m_c4Size)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_TookTooMuch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4ToTake)
            , TCardinal(m_c4Size)
        );
    }

    m_c4Size -= c4ToTake;
    m_c4Offset += c4ToTake;
}




// ---------------------------------------------------------------------------
//   CLASS: TOSStoreItem
//  PREFIX: osi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOSStoreItem: Public, static methods
// ---------------------------------------------------------------------------
const TString& TOSStoreItem::strKey(const TOSStoreItem& osiSrc)
{
    return osiSrc.m_strKey;
}


// ---------------------------------------------------------------------------
//  TOSStoreItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TOSStoreItem::TOSStoreItem() :

    m_c4Allocated(0)
    , m_c4CurUsed(0)
    , m_c4KeyBytes(0)
    , m_c4Offset(0)
    , m_c4VersionNum(0)
{
}

TOSStoreItem::TOSStoreItem( const   tCIDLib::TCard4 c4Offset
                            , const tCIDLib::TCard4 c4CurUsed
                            , const tCIDLib::TCard4 c4Allocated
                            , const tCIDLib::TCard4 c4Version
                            , const TString&        strKey
                            , const tCIDLib::TCard4 c4KeyBytes) :

    m_c4Allocated(c4Allocated)
    , m_c4CurUsed(c4CurUsed)
    , m_c4KeyBytes(c4KeyBytes)
    , m_c4Offset(c4Offset)
    , m_c4VersionNum(c4Version)
    , m_strKey(strKey)
{
    BreakOutScope();
}

TOSStoreItem::TOSStoreItem(const TOSStoreItem& osiSrc) :

    m_c4Allocated(osiSrc.m_c4Allocated)
    , m_c4CurUsed(osiSrc.m_c4CurUsed)
    , m_c4KeyBytes(osiSrc.m_c4KeyBytes)
    , m_c4Offset(osiSrc.m_c4Offset)
    , m_c4VersionNum(osiSrc.m_c4VersionNum)
    , m_strKey(osiSrc.m_strKey)
    , m_strName(osiSrc.m_strName)
    , m_strScope(osiSrc.m_strScope)
{
}

TOSStoreItem::~TOSStoreItem()
{
}


// ---------------------------------------------------------------------------
//  TOSStoreItem: Public operators
// ---------------------------------------------------------------------------
TOSStoreItem& TOSStoreItem::operator=(const TOSStoreItem& osiSrc)
{
    if (this != &osiSrc)
    {
        m_c4Allocated   = osiSrc.m_c4Allocated;
        m_c4CurUsed     = osiSrc.m_c4CurUsed;
        m_c4KeyBytes    = osiSrc.m_c4KeyBytes;
        m_c4Offset      = osiSrc.m_c4Offset;
        m_c4VersionNum  = osiSrc.m_c4VersionNum;
        m_strKey        = osiSrc.m_strKey;
        m_strName       = osiSrc.m_strName;
        m_strScope      = osiSrc.m_strScope;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TOSStoreItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOSStoreItem::AddToAllocated(const TOSFreeListItem& fliToTake)
{
    m_c4Allocated += fliToTake.c4Size();
}

tCIDLib::TVoid TOSStoreItem::AddToAllocated(const tCIDLib::TCard4 c4ToAdd)
{
    m_c4Allocated += c4ToAdd;
}


tCIDLib::TCard4 TOSStoreItem::c4Allocated() const
{
    return m_c4Allocated;
}

tCIDLib::TCard4 TOSStoreItem::c4Allocated(const tCIDLib::TCard4 c4ToSet)
{
    // Make sure the allocation is >= the used
    CheckAllocUsed(c4ToSet, m_c4CurUsed, CID_LINE);

    m_c4Allocated = c4ToSet;
    return m_c4Allocated;
}


tCIDLib::TCard4 TOSStoreItem::c4CurUsed() const
{
    return m_c4CurUsed;
}

tCIDLib::TCard4 TOSStoreItem::c4CurUsed(const tCIDLib::TCard4 c4ToSet)
{
    // Make sure the allocation is >= the used
    CheckAllocUsed(m_c4Allocated, c4ToSet, CID_LINE);

    m_c4CurUsed = c4ToSet;
    return m_c4CurUsed;
}


tCIDLib::TCard4 TOSStoreItem::c4KeyBytes() const
{
    return m_c4KeyBytes;
}


tCIDLib::TCard4 TOSStoreItem::c4Offset() const
{
    return m_c4Offset;
}

tCIDLib::TCard4 TOSStoreItem::c4Offset(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Offset = c4ToSet;
    return m_c4Offset;
}


tCIDLib::TCard4
TOSStoreItem::c4NewStorageRequired( const   tCIDLib::TCard4 c4NewSize
                                    , const tCIDLib::TCard4 c4NewReserve) const
{
    return  c4NewSize
            + c4NewReserve
            + m_c4KeyBytes
            + sizeof(TCIDObjStoreImpl::TStoreItemHdr);
}


tCIDLib::TCard4 TOSStoreItem::c4Reserve() const
{
    return m_c4Allocated - m_c4CurUsed;
}


tCIDLib::TCard4 TOSStoreItem::c4StorageRequired() const
{
    //
    //  The data required for this item in storage is the full byte
    //  allocation is needs, plus the flattened key bytes, plus the header
    //  that has to be stored first in the item.
    //
    return m_c4Allocated + m_c4KeyBytes + sizeof(TCIDObjStoreImpl::TStoreItemHdr);
}


tCIDLib::TCard4 TOSStoreItem::c4Version() const
{
    return m_c4VersionNum;
}

tCIDLib::TCard4 TOSStoreItem::c4Version(const tCIDLib::TCard4 c4ToSet)
{
    m_c4VersionNum = c4ToSet;
    return m_c4VersionNum;
}


tCIDLib::TVoid TOSStoreItem::IncVersion()
{
    m_c4VersionNum++;
}


tCIDLib::TVoid TOSStoreItem::MarkUnused()
{
    m_c4Allocated   = 0;
    m_c4CurUsed     = 0;
    m_c4KeyBytes    = 0;
    m_c4Offset      = 0;
    m_c4VersionNum  = 0;
    m_strKey.Clear();

    m_strName.Clear();
    m_strScope.Clear();
}


const TString& TOSStoreItem::strKey() const
{
    return m_strKey;
}


const TString& TOSStoreItem::strName() const
{
    return m_strName;
}


const TString& TOSStoreItem::strScope() const
{
    return m_strScope;
}


tCIDLib::TVoid
TOSStoreItem::Set(  const   tCIDLib::TCard4 c4Offset
                    , const tCIDLib::TCard4 c4CurUsed
                    , const tCIDLib::TCard4 c4Allocated
                    , const tCIDLib::TCard4 c4Version
                    , const TString&        strKey
                    , const tCIDLib::TCard4 c4KeyBytes)
{
    // Make sure the allocation is >= the used
    CheckAllocUsed(c4Allocated, c4CurUsed, CID_LINE);

    m_c4Allocated   = c4Allocated;
    m_c4CurUsed     = c4CurUsed;
    m_c4KeyBytes    = c4KeyBytes;
    m_c4Offset      = c4Offset;
    m_c4VersionNum  = c4Version;
    m_strKey        = strKey;

    BreakOutScope();
}


// ---------------------------------------------------------------------------
//  TOSStoreItem: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOSStoreItem::BreakOutScope()
{
    // Find the last separator in the key
    tCIDLib::TCard4 c4Ofs;
    if (!m_strKey.bLastOccurrence(kCIDLib::chForwardSlash, c4Ofs) || (c4Ofs < 2))
    {
        // It cannot be a well formed
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_BadKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , m_strKey
        );
    }
    m_strScope.CopyInSubStr(m_strKey, 0, c4Ofs + 1);
    m_strName.CopyInSubStr(m_strKey, c4Ofs + 1);
}


tCIDLib::TVoid
TOSStoreItem::CheckAllocUsed(const  tCIDLib::TCard4 c4Allocated
                            , const tCIDLib::TCard4 c4Used
                            , const tCIDLib::TCard4 c4Line)
{
    if (c4Allocated < c4Used)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , c4Line
            , kObjSErrs::errcData_BadAllocUsed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Allocated)
            , TCardinal(c4Used)
            , m_strKey
        );
    }
}


