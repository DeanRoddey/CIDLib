//
// FILE NAME: CIDObjStore_SeqData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/2002
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the simple class that we use to build a list of
//  all of the free and index items and sort them into a contiguous sequence
//  covering the whole store.
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
RTTIDecls(TOSSeqData,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TOSSeqData
//  PREFIX: osd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOSSeqData: Constructors and Destructor
// ---------------------------------------------------------------------------
TOSSeqData::TOSSeqData() :

    m_c4ListInd(0)
    , m_c4Ofs(0)
    , m_c4Size(0)
    , m_eType(tCIDObjStore_::EItemTypes::None)
    , m_posiSrc(nullptr)
{
}

TOSSeqData::TOSSeqData(         TOSFreeListItem&    fliSrc
                        , const tCIDLib::TCard4     c4ListInd) :

    m_c4ListInd(c4ListInd)
    , m_c4Ofs(fliSrc.c4Offset())
    , m_c4Size(fliSrc.c4Size())
    , m_eType(tCIDObjStore_::EItemTypes::FreeList)
    , m_posiSrc(nullptr)
{
}

TOSSeqData::TOSSeqData(TOSStoreItem* const posiSrc) :

    m_c4ListInd(0)
    , m_c4Ofs(posiSrc->c4Offset())
    , m_c4Size(posiSrc->c4StorageRequired())
    , m_eType(tCIDObjStore_::EItemTypes::Index)
    , m_posiSrc(posiSrc)
{
}

TOSSeqData::~TOSSeqData()
{
}


// ---------------------------------------------------------------------------
//  TOSSeqData: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOSSeqData::Combine(const TOSSeqData& osdToAdd)
{
    // If debugging, making sure they are both of the same type
    #if CID_DEBUG_ON
    if (m_eType != osdToAdd.m_eType)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_BadCombine
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    // Add the source's size to our size
    m_c4Size += osdToAdd.m_c4Size;
}

