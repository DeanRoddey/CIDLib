//
// FILE NAME: CIDDBase_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
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
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDDBase,TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDDBase
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDDBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDDBase::TFacCIDDBase() :

    TFacility
    (
        L"CIDDBase"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_pEnvHandle(nullptr)
{
}

TFacCIDDBase::~TFacCIDDBase()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDBase: Public, non-virtual methods
// ---------------------------------------------------------------------------

// This one will throw if we haven't been initialized
tCIDDBase::TDBEnvHandle& TFacCIDDBase::hEnv() const
{
    if (!m_pEnvHandle)
    {
        facCIDDBase().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcInit_SQLEnvNotInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }
    return *m_pEnvHandle;
}
