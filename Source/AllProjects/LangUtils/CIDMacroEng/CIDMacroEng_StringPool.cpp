//
// FILE NAME: CIDMacroEng_StringPool.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements
//
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngStringPool,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TMEngStringPool
// PREFIX: mspl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringPool: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringPool::TMEngStringPool() :

    m_colPool(8)
{
    // Add the initial items
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
        m_colPool.objAdd(TPoolItem());
}

TMEngStringPool::~TMEngStringPool()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringPool: Protected, non-virtual methods
// ---------------------------------------------------------------------------
TString* TMEngStringPool::pstrLease(tCIDLib::TCard4& c4Id)
{
    // Try to find one that's not in use
    c4Id = 0;
    const tCIDLib::TCard4 c4Count = m_colPool.c4ElemCount();
    for (; c4Id < c4Count; c4Id++)
    {
        TPoolItem& piCur = m_colPool[c4Id];

        if (!piCur.m_bInUse)
        {
            piCur.m_bInUse = kCIDLib::True;
            piCur.m_strToUse.Clear();
            return &piCur.m_strToUse;
        }
    }

    // If not maxed out, then add a new one
    if (c4Count >= 256)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcStrP_MaxedOut
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
        );
    }

    TPoolItem& piNew = m_colPool.objAdd(TPoolItem());
    piNew.m_bInUse = kCIDLib::True;
    piNew.m_strToUse.Clear();
    return &piNew.m_strToUse;
}


tCIDLib::TVoid TMEngStringPool::Return(const tCIDLib::TCard4 c4Id)
{
    // Make sure this one is in use and a legal index
    if (c4Id >= m_colPool.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcStrP_BadIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }

    TPoolItem& piRet = m_colPool[c4Id];
    if (!piRet.m_bInUse)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcStrP_NotInUse
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    piRet.m_bInUse = kCIDLib::False;
}


