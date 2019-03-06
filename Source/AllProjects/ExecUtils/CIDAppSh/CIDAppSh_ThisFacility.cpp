//
// FILE NAME: CIDAppSh_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the facilty class for this facility.
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
#include    "CIDAppSh_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDAppSh,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDAppSh
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDAppSh: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDAppSh::TFacCIDAppSh() :

    TFacility
    (
        L"CIDAppSh"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDAppSh::~TFacCIDAppSh()
{
}

