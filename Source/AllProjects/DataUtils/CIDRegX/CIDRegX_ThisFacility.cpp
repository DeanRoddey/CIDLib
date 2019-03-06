//
// FILE NAME: CIDRegX_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    "CIDRegX_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDRegX,TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDRegX
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDRegX: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDRegX::TFacCIDRegX() :

    TFacility
    (
        L"CIDRegX"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDRegX::~TFacCIDRegX()
{
}
