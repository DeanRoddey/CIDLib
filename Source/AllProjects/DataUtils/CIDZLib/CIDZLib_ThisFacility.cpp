//
// FILE NAME: CIDZLib_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
#include    "CIDZLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDZLib,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDZLib_ThisFacility
{
}



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDZLib
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDZLib: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDZLib::TFacCIDZLib() :

    TFacility
    (
        L"CIDZLib"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDZLib::~TFacCIDZLib()
{
}

