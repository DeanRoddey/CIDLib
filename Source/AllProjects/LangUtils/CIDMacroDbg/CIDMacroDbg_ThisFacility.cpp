//
// FILE NAME: CIDMacroDbg_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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




