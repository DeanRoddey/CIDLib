//
// FILE NAME: CIDXML_NameIDPool.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains the out of line implementation stuff for the name/id pool
//  template class. We pre-generate the variations required to avoid a lot of
//  code generation overhead.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  For this file we want to pre instantiate some collections, but not for any
//  other files. Only this file defines this toke, which prevents the use of the
//  extern declaration.
// ---------------------------------------------------------------------------
#define CIDXML_PREINST 1



// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Pre-generate some variations
// ---------------------------------------------------------------------------
template class TXMLNameIDPool<TDTDAttrDef>;
template class TXMLNameIDPool<TDTDElemDecl>;
template class TXMLNameIDPool<TDTDEntityDecl>;
template class TXMLNameIDPool<TDTDNotationDecl>;
template class TXMLNameIDPoolCursor<TDTDAttrDef>;
template class TXMLNameIDPoolCursor<TDTDElemDecl>;
template class TXMLNameIDPoolCursor<TDTDEntityDecl>;
template class TXMLNameIDPoolCursor<TDTDNotationDecl>;



// ---------------------------------------------------------------------------
//  CLASS: TXMLNameIDPool
// PREFIX: xnip
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLNameIDPool: Constructors and Destructor
// ---------------------------------------------------------------------------
template <class TElem>
TXMLNameIDPool<TElem>::TXMLNameIDPool(TKeyExtract pfnKeyExtract) :

    m_apobjById(0)
    , m_c4IdCount(64)
    , m_c4IdIndex(0)
    , m_c4IterBucket(c4Modulus)
    , m_c4SeqId(1)
    , m_pIter(0)
    , m_pfnKeyExtract(pfnKeyExtract)
{
    // Init the bucket pointers
    TRawMem::SetMemBuf
    (
        m_aBuckets
        , tCIDLib::TCard1(0)
        , sizeof(TBucketElem*) * c4Modulus
    );

    //
    //  Do the initial allocation for the id array. We don't have to init it
    //  since the m_c4IdIndex indicates which elements are legal.
    //
    m_apobjById = new TElem*[m_c4IdCount];
}

template <class TElem> TXMLNameIDPool<TElem>::~TXMLNameIDPool()
{
    // Remove all items and then free the id array
    RemoveAll();
    delete [] m_apobjById;
}


// ---------------------------------------------------------------------------
//  TXMLNameIDPool: Public, non-virtual methods
// ---------------------------------------------------------------------------
template <class TElem> tCIDLib::TBoolean TXMLNameIDPool<TElem>::bEmpty() const
{
    //
    //  Search the bucket list until we find one that is non-zero. If we find
    //  one, then its not empty.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Modulus; c4Index++)
    {
        if (m_aBuckets[c4Index])
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


template <class TElem> tCIDLib::TBoolean TXMLNameIDPool<TElem>::bNext()
{
    // If the iterator is not valid, then just return false now
    if (m_c4IterBucket == c4Modulus)
        return kCIDLib::False;

    //
    //  Search forward on the current bucket list. If we hit the end of it
    //  then search for the next non-empty bucket list.
    //
    m_pIter = m_pIter->pNext;
    if (m_pIter)
        return kCIDLib::True;

    // We ate up that bucket list, so find the next one
    m_c4IterBucket++;
    for (; m_c4IterBucket < c4Modulus; m_c4IterBucket++)
    {
        if (m_aBuckets[m_c4IterBucket])
        {
            m_pIter = m_aBuckets[m_c4IterBucket];
            return kCIDLib::True;
        }
    }
    m_c4IterBucket = 0;
    m_pIter = 0;
    return kCIDLib::False;
}


template <class TElem> tCIDLib::TBoolean TXMLNameIDPool<TElem>::bResetIter()
{
    //
    //  Reset our iterator node pointer by searching for the first node in
    //  the list that we can find. If we don't find one, then return false.
    //
    m_pIter = 0;
    for (m_c4IterBucket = 0; m_c4IterBucket < c4Modulus; m_c4IterBucket++)
    {
        if (m_aBuckets[m_c4IterBucket])
        {
            m_pIter = m_aBuckets[m_c4IterBucket];
            return kCIDLib::True;
        }
    }

    // We fall out with m_c4IterBucket set to the modulus value
    return kCIDLib::False;
}


template <class TElem>
tCIDLib::TCard4 TXMLNameIDPool<TElem>::c4AddNew(TElem* const pobjToAdd)
{
    // Get the key and hash it
    const tCIDLib::THashVal hshKey = m_pfnKeyExtract(*pobjToAdd).hshCalcHash(c4Modulus);

    #if CID_DEBUG_ON
    if (hshKey >= c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcPool_InvalidHash
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(hshKey)
            , TCardinal(c4Modulus)
        );
    }
    #endif

    //
    //  Put this element into the bucket list. Either the bucket is empty,
    //  so we make this one the head. Or we slip this one into an existing
    //  bucket list at the head.
    //
    if (m_aBuckets[hshKey])
        m_aBuckets[hshKey] = new TBucketElem(pobjToAdd, m_aBuckets[hshKey]);
    else
        m_aBuckets[hshKey] = new TBucketElem(pobjToAdd, 0);

    // Bump the sequence id to invalidate cursors
    m_c4SeqId++;

    //
    //  Set this element to have the next available id, and add it to the
    //  'by id' array. Return the id at which it was added.
    //
    return c4AddToIdArray(pobjToAdd);
}


template <class TElem> tCIDLib::TCard4 TXMLNameIDPool<TElem>::c4SeqId() const
{
    return m_c4SeqId;
}


template <class TElem>
TElem* TXMLNameIDPool<TElem>::pobjById(const tCIDLib::TCard4 c4Id)
{
    if (c4Id >= m_c4IdIndex)
        return 0;

    return m_apobjById[c4Id];
}


template <class TElem>
TElem* TXMLNameIDPool<TElem>::pobjByName(const TString& strName)
{
    // Hash the name string
    const tCIDLib::THashVal hshKey = strName.hshCalcHash(c4Modulus);

    #if CID_DEBUG_ON
    if (hshKey >= c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcPool_InvalidHash
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(hshKey)
            , TCardinal(c4Modulus)
        );
    }
    #endif

    //
    //  See if there is anything in this bucket. If not, then we know we ain't
    //  gonna match and can return now.
    //
    if (!m_aBuckets[hshKey])
        return 0;

    // We have to search the list and compare the names to find the actual one
    TBucketElem* pCur = m_aBuckets[hshKey];
    while (pCur)
    {
        // If this one matches, then we are done
        if (m_pfnKeyExtract(*pCur->pobjElem) == strName)
            return pCur->pobjElem;

        // Try another one
        pCur = pCur->pNext;
    }

    // Never found it
    return 0;
}

template <class TElem>
TElem* TXMLNameIDPool<TElem>::pobjByName(const tCIDLib::TCh* const pszName)
{
    // Hash the name string
    const tCIDLib::THashVal hshKey = TRawStr::hshHashStr(pszName, c4Modulus);

    #if CID_DEBUG_ON
    if (hshKey >= c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcPool_InvalidHash
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(hshKey)
            , TCardinal(c4Modulus)
        );
    }
    #endif

    //
    //  See if there is anything in this bucket. If not, then we know we ain't
    //  gonna match and can return now.
    //
    if (!m_aBuckets[hshKey])
        return 0;

    // We have to search the list and compare the names to find the actual one
    TBucketElem* pCur = m_aBuckets[hshKey];
    while (pCur)
    {
        // If this one matches, then we are done
        if (m_pfnKeyExtract(*pCur->pobjElem).bCompare(pszName))
            return pCur->pobjElem;

        // Try another one
        pCur = pCur->pNext;
    }

    // Never found it
    return 0;
}


template <class TElem> const TElem& TXMLNameIDPool<TElem>::objCur() const
{
    if (m_c4IterBucket == c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CursorInvalid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    return *(m_pIter->pobjElem);
}


template <class TElem> TElem& TXMLNameIDPool<TElem>::objCur()
{
    if (m_c4IterBucket == c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CursorInvalid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    return *(m_pIter->pobjElem);
}


template <class TElem> tCIDLib::TVoid TXMLNameIDPool<TElem>::RemoveAll()
{
    // Delete all of the elements in our buckets
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Modulus; c4Index++)
    {
        if (m_aBuckets[c4Index])
        {
            TBucketElem* pCur = m_aBuckets[c4Index];
            TBucketElem* pNext = 0;
            while (pCur)
            {
                // Save the next node before we delete this one
                pNext = pCur->pNext;

                // Delete the bucket element
                delete pCur;

                // And now make the saved next the current
                pCur = pNext;
            }
            m_aBuckets[c4Index] = 0;
        }
    }

    // Invalidate the internal iterator
    m_c4IterBucket = c4Modulus;
    m_pIter = 0;

    // Bump the sequence id to invalidate cursors
    m_c4SeqId++;
}


// ---------------------------------------------------------------------------
//  TXMLNameIDPool: Private, non-virtual methods
// ---------------------------------------------------------------------------
template <class TElem>
tCIDLib::TCard4 TXMLNameIDPool<TElem>::c4AddToIdArray(TElem* const pobjToAdd)
{
    // Set the id on the newly added object
    pobjToAdd->c4Id(m_c4IdIndex);

    // See if we need to expand the array
    if (m_c4IdIndex == m_c4IdCount)
    {
        // Calculate a new size and allocate a new array
        const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(m_c4IdCount * 1.25);
        TElem** apobjNew = new TElem*[c4NewSize];

        // Copy over the old stuff
        TRawMem::CopyMemBuf(apobjNew, m_apobjById, sizeof(TElem*) * m_c4IdCount);

        // Now store the new stuff and delete the old array
        delete [] m_apobjById;
        m_apobjById = apobjNew;
        m_c4IdCount = c4NewSize;
    }

    // Add this entry at the current index and bump the index
    m_apobjById[m_c4IdIndex++] = pobjToAdd;

    // Return the id we stored it at
    return m_c4IdIndex - 1;
}



// ---------------------------------------------------------------------------
//  TXMLNameIDPoolCursor: Constructors and Destructor
// ---------------------------------------------------------------------------
template <class TElem> TXMLNameIDPoolCursor<TElem>::
TXMLNameIDPoolCursor(const TXMLNameIDPool<TElem>* const pxnipToIter) :

    m_c4IterBucket(TXMLNameIDPool<TElem>::c4Modulus)
    , m_c4SeqId(0)
    , m_pIter(0)
    , m_pxnipIter(pxnipToIter)
{
}

template <class TElem> TXMLNameIDPoolCursor<TElem>::
TXMLNameIDPoolCursor(const TXMLNameIDPoolCursor& xnipcToCopy) :

    m_c4IterBucket(xnipcToCopy.m_c4IterBucket)
    , m_c4SeqId(xnipcToCopy.m_c4SeqId)
    , m_pIter(xnipcToCopy.m_pIter)
    , m_pxnipIter(xnipcToCopy.m_pxnipIter)
{
}

template <class TElem> TXMLNameIDPoolCursor<TElem>::~TXMLNameIDPoolCursor()
{
}


// ---------------------------------------------------------------------------
//  TXMLNameIDPoolCursor: Public, inherited methods
// ---------------------------------------------------------------------------
template <class TElem> tCIDLib::TBoolean TXMLNameIDPoolCursor<TElem>::bNext()
{
    // Make sure we are still legal
    if (m_c4SeqId != m_pxnipIter->c4SeqId())
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CursorInvalid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Search forward on the current bucket list. If we hit the end of it
    //  then search for the next non-empty bucket list.
    //
    m_pIter = m_pIter->pNext;
    if (m_pIter)
        return kCIDLib::True;

    // We ate up that bucket list, so find the next one
    m_c4IterBucket++;
    for (; m_c4IterBucket < TXMLNameIDPool<TElem>::c4Modulus; m_c4IterBucket++)
    {
        if (m_pxnipIter->m_aBuckets[m_c4IterBucket])
        {
            m_pIter = m_pxnipIter->m_aBuckets[m_c4IterBucket];
            return kCIDLib::True;
        }
    }
    m_pIter = 0;
    return kCIDLib::False;
}

template <class TElem> tCIDLib::TBoolean TXMLNameIDPoolCursor<TElem>::bReset()
{
    // Get our sequence id back up to date
    m_c4SeqId = m_pxnipIter->c4SeqId();

    //
    //  And seek up to find the first available node in the bucket list.
    //  If we don't, we'll end up with the index being equal to the modulus.
    //
    for (m_c4IterBucket = 0; m_c4IterBucket < TXMLNameIDPool<TElem>::c4Modulus; m_c4IterBucket++)
    {
        if (m_pxnipIter->m_aBuckets[m_c4IterBucket])
        {
            m_pIter = m_pxnipIter->m_aBuckets[m_c4IterBucket];
            return kCIDLib::True;
        }
    }
    m_pIter = 0;
    return kCIDLib::False;
}

template <class TElem> const TElem& TXMLNameIDPoolCursor<TElem>::objCur() const
{
    // Make sure we are still legal
    if (m_c4SeqId != m_pxnipIter->c4SeqId())
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CursorInvalid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Make sure we aren't already at the end
    if (m_c4IterBucket == TXMLNameIDPool<TElem>::c4Modulus)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CursorInvalid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    return *(m_pIter->pobjElem);
}


template <class TElem> TXMLNameIDPoolCursor<TElem>&
TXMLNameIDPoolCursor<TElem>::operator=(const TXMLNameIDPoolCursor<TElem>& xnipcToAssign)
{
    if (this == &xnipcToAssign)
        return *this;

    m_c4IterBucket  = xnipcToAssign.m_c4IterBucket;
    m_c4SeqId       = xnipcToAssign.m_c4SeqId;
    m_pIter         = xnipcToAssign.m_pIter;
    m_pxnipIter     = xnipcToAssign.m_pxnipIter;

    return *this;
}


