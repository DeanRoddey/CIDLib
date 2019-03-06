//
// FILE NAME: CIDMacroDbg_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements our facility class.
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
#include    "CIDMacroDbg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDMacroDbg,TGUIFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDMacroDbg
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDMacroDbg: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDMacroDbg::TFacCIDMacroDbg() :

    TGUIFacility
    (
        L"CIDMacroDbg"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgsAndRes
    )
{
}

TFacCIDMacroDbg::~TFacCIDMacroDbg()
{
}




