//
// FILE NAME: CIDKernel_HashMap.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TKrnlHashMap class, which is a very simple
//  hashed collection for some use internally within the kernel, or by
//  programs which are written to the kernel layer.
//
//  It is all platform independent.
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
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TKrnlHashMap
//  PREFIX: khshm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlHashMap: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlHashMap::TKrnlHashMap(const tCIDLib::TCard4 c4Modulus) :

    m_apnodeTable(0)
    , m_c4CurList(0)
    , m_c4ElemCount(0)
    , m_c4Modulus(c4Modulus)
    , m_pnodeCur(0)
{
    // Allocate the node list
    m_apnodeTable = new TNode*[c4Modulus];

    // And zero them out
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Modulus; c4Index++)
        m_apnodeTable[c4Index] = 0;
}

TKrnlHashMap::~TKrnlHashMap()
{
    // Flush the list
    RemoveAll();

    // And delete the table itself
    delete [] m_apnodeTable;
}


// ---------------------------------------------------------------------------
//  TKrnlHashMap: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlHashMap::bAdd( const   tCIDLib::TCh* const pszKey
                    , const tCIDLib::TCh* const pszValue)
{
    // See if its already in the list
    tCIDLib::THashVal hshKey;
    TNode* pnodeNew = pnodeFind(pszKey, hshKey);
    if (pnodeNew)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    // Create a new node for this guy
    pnodeNew = new TNode;

    // Replicate the passed strings into our node
    pnodeNew->pszKey = TRawStr::pszReplicate(pszKey);
    pnodeNew->pszValue = TRawStr::pszReplicate(pszValue);

    //
    //  Put it into the list. We put it at the head of the list it
    //  hashes to, since this is fast.
    //
    pnodeNew->pnodeNext = m_apnodeTable[hshKey];
    m_apnodeTable[hshKey] = pnodeNew;

    // Bump the element count
    m_c4ElemCount++;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHashMap::bCurKey(const tCIDLib::TCh*& pszToFill) const
{
    // Make sure that the iterator is valid
    if (!m_pnodeCur)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcHshMap_InvalidIterator);
        return kCIDLib::False;
    }
    pszToFill = m_pnodeCur->pszKey;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlHashMap::bCurKeyAndValue(  const   tCIDLib::TCh*& pszKeyToFill
                                , const tCIDLib::TCh*& pszValToFill) const
{
    // Make sure that the iterator is valid
    if (!m_pnodeCur)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcHshMap_InvalidIterator);
        return kCIDLib::False;
    }

    pszKeyToFill = m_pnodeCur->pszKey;
    pszValToFill = m_pnodeCur->pszValue;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHashMap::bCurValue(const tCIDLib::TCh*& pszToFill) const
{
    // Make sure that the iterator is valid
    if (!m_pnodeCur)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcHshMap_InvalidIterator);
        return kCIDLib::False;
    }

    pszToFill = m_pnodeCur->pszValue;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHashMap::bNext()
{
    if (!m_pnodeCur)
        return kCIDLib::False;

    //
    //  See if there is another element in the current list. If so, just
    //  move up to the next node.
    //
    m_pnodeCur = m_pnodeCur->pnodeNext;
    if (m_pnodeCur)
        return kCIDLib::True;

    //
    //  Look for another non-empty list after the current one. if we cannot
    //  find one, then give up.
    //
    m_c4CurList++;
    for (; m_c4CurList < m_c4Modulus; m_c4CurList++)
    {
        // If nothing in this list, move on
        if (m_apnodeTable[m_c4CurList])
            break;
    }

    if (m_c4CurList == m_c4Modulus)
        return kCIDLib::False;

    // We found one, so point at its first element
    m_pnodeCur = m_apnodeTable[m_c4CurList];

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHashMap::bRemoveKey(const tCIDLib::TCh* const pszKey)
{
    // Hash the passed key
    tCIDLib::THashVal hshKey = TRawStr::hshHashStr(pszKey, m_c4Modulus);
    if (!hshKey)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcStr_BadUnicode);
        return kCIDLib::False;
    }

    //
    //  We need to go through the list that this key is in and find this
    //  node and its previous node.
    //
    TNode* pnodePrev = 0;
    TNode* pnodeFind = m_apnodeTable[hshKey];

    while (pnodeFind)
    {
        // If this key matches, then break out
        if (TRawStr::bCompareStr(pnodeFind->pszKey, pszKey))
            break;

        // Save the current as previous and move up
        pnodePrev = pnodeFind;
        pnodeFind = pnodeFind->pnodeNext;
    }

    if (!pnodeFind)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
        return kCIDLib::False;
    }

    // If we are deleting the iterator node, move it up
    if (m_pnodeCur == pnodeFind)
        bNext();

    //
    //  If the previous node is 0, then this is the head node, else we have
    //  to point the previous node out the found node's next node.
    //
    if (!pnodePrev)
        m_apnodeTable[hshKey] = pnodeFind->pnodeNext;
    else
        pnodePrev->pnodeNext = pnodeFind->pnodeNext;

    // Now clean up the found node
    delete [] pnodeFind->pszKey;
    delete [] pnodeFind->pszValue;
    delete pnodeFind;

    // Bump down the element count, checking for underflow
    if (!m_c4ElemCount)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcHshMap_ElemCountUnderflow);
        return kCIDLib::False;
    }

    m_c4ElemCount--;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHashMap::bResetIter()
{
    //
    //  Search for the first non-empty list slot. If there is not one,
    //  just return kCIDLib::False.
    //
    m_pnodeCur = 0;
    m_c4CurList = 0;
    for (; m_c4CurList < m_c4Modulus; m_c4CurList++)
    {
        // If nothing in this list, move on
        if (m_apnodeTable[m_c4CurList])
            break;
    }

    if (m_c4CurList == m_c4Modulus)
        return kCIDLib::False;

    // We found one, so point current node at first element in that list
    m_pnodeCur = m_apnodeTable[m_c4CurList];

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlHashMap::bUpdateKey(const  tCIDLib::TCh* const pszKey
                        , const tCIDLib::TCh* const pszNewValue)
{
    // See if the node is here
    tCIDLib::THashVal hshKey;
    TNode* pnodeUpdate = pnodeFind(pszKey, hshKey);

    if (!pnodeUpdate)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
        return kCIDLib::False;
    }

    // Delete the current value field
    delete [] pnodeUpdate->pszValue;
    pnodeUpdate->pszValue = TRawStr::pszReplicate(pszNewValue);
    return kCIDLib::True;
}


tCIDLib::TVoid TKrnlHashMap::RemoveAll()
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Modulus; c4Index++)
    {
        // If nothing in this list, move on
        if (!m_apnodeTable[c4Index])
            continue;

        TNode* pnodeCur = m_apnodeTable[c4Index];
        while (pnodeCur)
        {
            TNode* pnodeNext = pnodeCur->pnodeNext;

            // Delete the strings and then the node itself
            delete [] pnodeCur->pszKey;
            delete [] pnodeCur->pszValue;
            delete pnodeCur;

            pnodeCur = pnodeNext;
        }

        // Zero out this list header
        m_apnodeTable[c4Index] = 0;
    }

    // Zero out the element count
    m_c4ElemCount = 0;
}


const tCIDLib::TCh*
TKrnlHashMap::pszValueForKey(const tCIDLib::TCh* const pszKey)
{
    // See if the node is here
    tCIDLib::THashVal hshKey;
    TNode* pnodeRet = pnodeFind(pszKey, hshKey);

    if (!pnodeRet)
        return 0;

    return pnodeRet->pszValue;
}



// ---------------------------------------------------------------------------
//  TKrnlHashMap: Private, non-virtual methods
// ---------------------------------------------------------------------------
TKrnlHashMap::TNode*
TKrnlHashMap::pnodeFind(const  tCIDLib::TCh* const  pszKeyToFind
                        ,       tCIDLib::THashVal&  hshElem) const
{
    // Get the hash of the element
    hshElem = TRawStr::hshHashStr(pszKeyToFind, m_c4Modulus);
    if (!hshElem)
    {
        // <TBD> This shoudl really be an error, not just a not found!
        return nullptr;
    }

    // If this list is empty, then obviously not here
    if (!m_apnodeTable[hshElem])
        return nullptr;

    // The list is not empty so check the list
    TNode* pnodeCur = m_apnodeTable[hshElem];
    while (pnodeCur)
    {
        if (TRawStr::bCompareStr(pnodeCur->pszKey, pszKeyToFind))
            return pnodeCur;
        pnodeCur = pnodeCur->pnodeNext;
    }
    return nullptr;
}
