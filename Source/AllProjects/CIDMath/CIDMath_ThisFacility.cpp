//
// FILE NAME: CIDMath_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TFacCIDMath class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDMath,TFacility)



// ---------------------------------------------------------------------------
//  TFacCIDMath: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDMath::TFacCIDMath() :

    TFacility
    (
        L"CIDMath"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDMath::~TFacCIDMath()
{
}
