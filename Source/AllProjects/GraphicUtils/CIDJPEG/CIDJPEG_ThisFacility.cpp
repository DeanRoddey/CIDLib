//
// FILE NAME: CIDJPEG_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
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
#include    "CIDJPEG_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDJPEG,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDJPEG_ThisFacility
{
}



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDJPEG
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDJPEG: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDJPEG::TFacCIDJPEG() :

    TFacility
    (
        L"CIDJPEG"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDJPEG::~TFacCIDJPEG()
{
}

