//
// FILE NAME: CIDOrb_ObList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/2000
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
//  This is implementation file for the server object list.
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


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbSObjList,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TOrbSObjList
//  PREFIX: sobjl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TOrbSObjList::TOrbSObjList() :

    m_c4BucketInd(kCIDOrb::c4IdModulus)
    , m_c4ElemCount(0)
    , m_pabiList(nullptr)
    , m_pbiIter(nullptr)
{
    m_pabiList = new TBucketItem*[kCIDOrb::c4IdModulus];
    TRawMem::SetMemBuf
    (
        m_pabiList
        , tCIDLib::TCard1(0)
        , sizeof(TBucketItem*) * kCIDOrb::c4IdModulus
    );
}

TOrbSObjList::~TOrbSObjList()
{
    RemoveAll();
    delete [] m_pabiList;
}



// ---------------------------------------------------------------------------
//  TOrbSObjList: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TOrbSObjList::Add(TOrbServerBase* const porbsToAdd, const tCIDLib::EAdoptOpts eAdopt)
{
    // Make sure it gets cleaned up if something gones wrong and we are adopting
    TJanitor<TOrbServerBase> janTmp(porbsToAdd, eAdopt == tCIDLib::EAdoptOpts::Adopt);

    // Get its hash out, since they pre-hash themselves
    const tCIDLib::THashVal hshToAdd = porbsToAdd->ooidThis().hshKey();

    #if CID_DEBUG_ON
    if (hshToAdd >= kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(hshToAdd)
        );
    }
    #endif

    //
    //  Now see if this key already exists. We definitely cannot have two
    //  objects with the same key. If it doesn't, then we end up at the end
    //  of the list and can just stick the new on the end. We can short cut
    //  it if the bucket is empty.
    //
    TBucketItem* pbiCur = m_pabiList[hshToAdd];
    if (!pbiCur)
    {
        TBucketItem* pbiNew = new TBucketItem;
        pbiNew->eAdopt = eAdopt;
        pbiNew->pbiNext = nullptr;
        pbiNew->porbsThis = janTmp.pobjOrphan();
        m_pabiList[hshToAdd] = pbiNew;
    }
     else
    {
        const TOrbObjId& ooidTest = porbsToAdd->ooidThis();
        while (kCIDLib::True)
        {
            if (pbiCur->porbsThis->ooidThis() == ooidTest)
            {
                facCIDOrb().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbErrs::errcServ_DupKey
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Duplicate
                    , ooidTest
                );
            }

            //
            //  If on the last item in this list, we didn't find one, so
            //  break while pbiCur still points to the last item. Else, move
            //  up to the next one.
            //
            if (!pbiCur->pbiNext)
                break;
            pbiCur = pbiCur->pbiNext;
        }

        // Ok, we survived, so patch the new one on the end
        TBucketItem* pbiNew = new TBucketItem;
        pbiNew->pbiNext = pbiNew;
        pbiNew->pbiNext = nullptr;
        pbiNew->porbsThis = janTmp.pobjOrphan();
    }

    m_c4ElemCount++;
}


tCIDLib::TBoolean TOrbSObjList::bNext()
{
    // If its invalid, return false
    if (m_c4BucketInd == kCIDOrb::c4IdModulus)
        return kCIDLib::False;

    // If the bucket index is valid, then the iter pointer must be non-zero
    if (!m_pbiIter)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadIterState
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );

        // Won't get here, but makes the analyzer happy
        return kCIDLib::False;
    }

    //
    //  Move the current node up to the next available node. If we are at the
    //  end of the current bucket, then move up to the next available one, and
    //  we might hit the end. Otherwise, just move up to the next node in
    //  the current bucket.
    //
    if (m_pbiIter->pbiNext)
    {
        m_pbiIter = m_pbiIter->pbiNext;
    }
     else
    {
        // Look for the next non-empty bucket
        m_c4BucketInd++;
        while (m_c4BucketInd < kCIDOrb::c4IdModulus)
        {
            if (m_pabiList[m_c4BucketInd])
                break;
            m_c4BucketInd++;
        }

        // If at the end zero the iter node, else start on the new bucket
        if (m_c4BucketInd == kCIDOrb::c4IdModulus)
            m_pbiIter = nullptr;
        else
            m_pbiIter = m_pabiList[m_c4BucketInd];
    }

    // Return true if we have a next node
    return (m_pbiIter != nullptr);
}


tCIDLib::TBoolean TOrbSObjList::bResetIter()
{
    m_c4BucketInd = 0;
    m_pbiIter = nullptr;
    while (m_c4BucketInd < kCIDOrb::c4IdModulus)
    {
        if (m_pabiList[m_c4BucketInd])
            break;
        m_c4BucketInd++;
    }

    // If we found a non-empty bucket, then start at its first node
    if (m_c4BucketInd != kCIDOrb::c4IdModulus)
        m_pbiIter = m_pabiList[m_c4BucketInd];

    return (m_c4BucketInd != kCIDOrb::c4IdModulus);
}


tCIDLib::TCard4 TOrbSObjList::c4ElemCount() const
{
    return m_c4ElemCount;
}


tCIDLib::TBoolean TOrbSObjList::bRemove(TOrbServerBase* const porbsToRemove)
{
    //
    //  Call the orphan method to get the object out of the list. He'll tell us
    //  whether it was marked as adopted or not.
    //
    tCIDLib::EAdoptOpts eAdopt;
    TOrbServerBase* porbsRemove = porbsOrphan(porbsToRemove, eAdopt);
    if (!porbsRemove)
        return kCIDLib::False;

    // We found it. If we adopted this one, then clean it up
    if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
    {
        try
        {
            delete porbsRemove;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }
    return kCIDLib::True;
}


const TOrbServerBase& TOrbSObjList::objCur() const
{
    if (m_c4BucketInd == kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_InvalidIterator
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // If the bucket index is valid, then the iter pointer must be non-zero
    if (!m_pbiIter)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadIterState
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );

        // Won't get here but makes the analyzer happy
        return *static_cast<TOrbServerBase*>(nullptr);
    }

    return *m_pbiIter->porbsThis;
}

TOrbServerBase& TOrbSObjList::objCur()
{
    if (m_c4BucketInd == kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_InvalidIterator
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // If the bucket index is valid, then the iter pointer must be non-zero
    if (!m_pbiIter)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadIterState
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );

        // Won't get here but makes the analyzer happy
        return *static_cast<TOrbServerBase*>(nullptr);
    }

    return *m_pbiIter->porbsThis;
}


TOrbServerBase* TOrbSObjList::porbsFind(const TOrbId& oidToFind)
{
    const tCIDLib::THashVal hshKey = oidToFind.hshThis();

    //
    //  Make sure that the hash is legal. Unlike other places here, this one
    //  is not debug-only, because this will usually be called due to calls
    //  from clients, and we can't ever trust them to be correct.
    //
    if (hshKey >= kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(hshKey)
        );
    }

    // If the bucket is empty, we are done
    if (!m_pabiList[hshKey])
        return nullptr;

    // There are entries here, so search the bucket
    TBucketItem* pbiCur = m_pabiList[hshKey];
    while (pbiCur)
    {
        if (pbiCur->porbsThis->ooidThis().oidKey() == oidToFind)
            return pbiCur->porbsThis;

        pbiCur = pbiCur->pbiNext;
    }
    return nullptr;
}

const TOrbServerBase* TOrbSObjList::porbsFind(const TOrbId& oidToFind) const
{
    const tCIDLib::THashVal hshKey = oidToFind.hshThis();

    //
    //  Make sure that the hash is legal. Unlike other places here, this one
    //  is not debug-only, because this will usually be called due to calls
    //  from clients, and we can't ever trust them to be correct.
    //
    if (hshKey >= kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(hshKey)
        );
    }

    // If the bucket is empty, we are done
    if (!m_pabiList[hshKey])
        return nullptr;

    // There are entries here, so search the bucket
    TBucketItem* pbiCur = m_pabiList[hshKey];
    while (pbiCur)
    {
        if (pbiCur->porbsThis->ooidThis().oidKey() == oidToFind)
            return pbiCur->porbsThis;

        pbiCur = pbiCur->pbiNext;
    }
    return nullptr;
}


//
//  Since we are orphaning, the original adoption setting doesn't matter. If it
//  was adopted, the caller is assuming responsibility.
//
TOrbServerBase*
TOrbSObjList::porbsOrphan(TOrbServerBase* const porbsToOrphan, tCIDLib::EAdoptOpts& eAdopt)
{
    // Get its hash out, since they pre-hash themselves
    const tCIDLib::THashVal hshToOrphan = porbsToOrphan->ooidThis().hshKey();

    #if CID_DEBUG_ON
    if (hshToOrphan >= kCIDOrb::c4IdModulus)
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcServ_BadHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(hshToOrphan)
        );
    }
    #endif

    // If the bucket is empty, we its not present
    if (!m_pabiList[hshToOrphan])
        return nullptr;

    //
    //  Search for this object, by pointer value. We have to keep up with
    //  the previous node, becase we are going to have to patch around it if
    //  we find it.
    //
    TBucketItem* pbiCur = m_pabiList[hshToOrphan];
    TBucketItem* pbiPrev = nullptr;
    while (pbiCur)
    {
        if (pbiCur->porbsThis == porbsToOrphan)
            break;

        pbiPrev = pbiCur;
        pbiCur = pbiCur->pbiNext;
    }

    if (!pbiCur)
        return nullptr;

    if (!pbiPrev)
        m_pabiList[hshToOrphan] = pbiCur->pbiNext;
    else
        pbiPrev->pbiNext = pbiCur->pbiNext;

    // Get the data and adoption status out, then delete the bucket object
    eAdopt = pbiCur->eAdopt;
    TOrbServerBase* porbsRet = pbiCur->porbsThis;
    delete pbiCur;

    // Bump down the element count. Watch for possible underflows
    if (!m_c4ElemCount)
    {
        if (facCIDOrb().bLogWarnings())
        {
            facCIDOrb().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kOrbMsgs::midStatus_ObjListUnderflow
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }
     else
    {
        m_c4ElemCount--;
    }

    // And return the object we orphaned
    return porbsRet;
}


tCIDLib::TVoid TOrbSObjList::RemoveAll()
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDOrb::c4IdModulus; c4Index++)
    {
        // Nothing to do for empty buckets
        if (!m_pabiList[c4Index])
            continue;

        TBucketItem* pbiCur = m_pabiList[c4Index];
        while (pbiCur)
        {
            // Remember the next before we trash the current
            TBucketItem* pbiNext = pbiCur->pbiNext;

            // If adopted, delete the user data, then kill the bucket item
            if (pbiCur->eAdopt == tCIDLib::EAdoptOpts::Adopt)
            {
                try
                {
                    delete pbiCur->porbsThis;
                }

                catch(...)
                {
                }
            }
            delete pbiCur;

            // And now we can move forward
            pbiCur = pbiNext;
        }
        m_pabiList[c4Index] = nullptr;
    }

    // Reset the element count now
    m_c4ElemCount = 0;
}


