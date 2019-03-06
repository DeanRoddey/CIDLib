//
// FILE NAME: CIDOrbWU_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the facility class for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbWU_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacCIDOrbWU, TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDOrbWU
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDOrbWU: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDOrbWU::TFacCIDOrbWU() :

    TFacility
    (
        L"CIDOrbWU"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}


TFacCIDOrbWU::~TFacCIDOrbWU()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDOrbWU: Public, non-virtual methods
// ---------------------------------------------------------------------------


