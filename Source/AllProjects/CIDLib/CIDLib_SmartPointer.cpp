//
// FILE NAME: CIDLib_SmartPointer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some out of line stuff to support the smart pointers
//  and keep their public profiles as light as possible. We want them to be
//  simple so we don't do a base class. We just do a namespace.
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
#include    "CIDLib_.hpp"


tCIDLib::TVoid TSmartPtrHelpers::CheckRef(const tCIDLib::TVoid* pToCheck)
{
    if (!pToCheck)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSPtr_RefReleaseErr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
}

tCIDLib::TVoid TSmartPtrHelpers::CheckRefNotZero(const tCIDLib::TCard4 c4ToCheck)
{
    if (!c4ToCheck)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSPtr_RefUnderflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
}

tCIDLib::TVoid TSmartPtrHelpers::LogReleaseError()
{
    facCIDLib().LogMsg
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcSPtr_RefReleaseErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
    );
}

tCIDLib::TVoid TSmartPtrHelpers::ThrowAcquireError()
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcSPtr_RefAcquireErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
    );
}

tCIDLib::TVoid TSmartPtrHelpers::ThrowReleaseError()
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcSPtr_RefReleaseErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
    );
}
