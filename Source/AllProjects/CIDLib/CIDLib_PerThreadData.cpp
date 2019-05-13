//
// FILE NAME: CIDLib_PerThreadData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
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
//  This file implements the TPerThreadImpl class, which is the basis
//  for the template class TPerThreadData. TPerThreadData provides a type
//  safe interface to the generic interface of TPerThreadImpl (which
//  does the interfacing to the underlying kernel services and keeps that
//  cruft out of line.)
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
RTTIDecls(TPerThreadImpl, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TPerThreadImpl
//  PREFIX: pbthr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPerThreadData: Constructors and destructor
// ---------------------------------------------------------------------------
TPerThreadImpl::TPerThreadImpl() :

    m_pkptdThis(nullptr)
{
}

TPerThreadImpl::~TPerThreadImpl()
{
}


// ---------------------------------------------------------------------------
//  TPerThreadData: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid* TPerThreadImpl::pThis()
{
    tCIDLib::TVoid* pRet = nullptr;
    try
    {
        pRet = pUserData();
    }

    catch(const TKrnlError& kerrToCatch)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_GetPerThread
            , kerrToCatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return pRet;
}


const tCIDLib::TVoid* TPerThreadImpl::pThis() const
{
    const tCIDLib::TVoid* pRet = nullptr;
    try
    {
        pRet = pUserData();
    }

    catch(const TKrnlError& kerrToCatch)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_GetPerThread
            , kerrToCatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return pRet;
}


tCIDLib::TVoid* TPerThreadImpl::pThis(tCIDLib::TVoid* const pToAdopt)
{
    tCIDLib::TVoid* pRet = nullptr;
    try
    {
        pRet = pUserData(pToAdopt);
    }

    catch(const TKrnlError& kerrToCatch)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_GetPerThread
            , kerrToCatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return pRet;
}

