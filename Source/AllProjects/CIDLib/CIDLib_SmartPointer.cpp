//
// FILE NAME: CIDLib_SmartPointer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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


tCIDLib::TVoid TSmartPtrHelpers::ThrowNullRef(const tCIDLib::TCard4 c4Line)
{
    facCIDLib().ThrowKrnlErr
    (
        CID_FILE
        , c4Line
        , kCIDErrs::errcSPtr_NoReference
        , TKrnlError::kerrLast()
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
    );
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
